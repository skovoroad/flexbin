#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/serialization/shared_ptr.hpp>


#include "gtest/gtest.h"
#include "test_object.h"

#include <fstream>

template<typename T> 
struct BoostSerializer
{
 template<typename TStream>
  static bool to_stream(
    TStream& ostr,
    const T& val)
  {
    boost::archive::binary_oarchive oa(
      ostr, 
      boost::archive::no_header | boost::archive::no_codecvt
    );
    oa << val;
    return true;
  }

  template<typename TStream>
  static bool from_stream(
    TStream& istr,
    T& val)
  {
    boost::archive::binary_iarchive ia(
      istr, 
      boost::archive::no_header | boost::archive::no_codecvt
    );
    ia >> val;
    return true;
  }
}; 

namespace fbtest {
  template<typename T>
  inline std::tuple<bool, bool, bool, uint16_t> custom_serialize_and_return_classid(const T& l, T& r) {
 
     //1. very very slow! avoid it. Use bigger buffer
     //2. using custom serialization u maybe have to handle exception
     auto object_size = flexbin::class_object_size<BoostSerializer<T>>(l);
     std::vector<char> buffer;
     buffer.resize(object_size);
     uint16_t classid(0);
     bool result = false;
     
     flexbin::ostream fbout(buffer.data(), buffer.size());
     fbout.custom_serialize<BoostSerializer<T>, T>(l);
    
     size_t written = fbout.written_count();
     if( written > object_size)
         return result_t(fbout.good(), false, false, 0);
     buffer.resize(written);
     
     result = flexbin::class_id(buffer.data(), buffer.size(), classid);
     flexbin::istream fbin(buffer.data(), buffer.size());
     fbin.custom_deserialize<BoostSerializer<T>, T>(r);
     return result_t(fbout.good(), fbin.good(), result, classid);
   };
} //namespace fbtest

template<typename T>
inline void serialize_and_compare(const T& l, T& r) {
  auto result = fbtest::custom_serialize_and_return_classid(l, r);
  uint16_t classid = std::get<fbtest::result_class_id>(result);
  
  ASSERT_TRUE(std::get<fbtest::result_fbout_good>(result));
  ASSERT_TRUE(std::get<fbtest::result_fbin_good>(result));
  ASSERT_TRUE(std::get<fbtest::result_class_id>(result));
  ASSERT_EQ (static_cast<uint16_t>(T::flexbin_class_id), classid);
  ASSERT_EQ(l, r) << " " << typeid(l).name()  ;
};

struct A {
  int a_ = 0;
  struct B {
    short c_ = 0;

    FLEXBIN_CLASS_ID(2);
    FLEXBIN_SERIALIZE_REQUIRED(c_);

    template<class Archive>
    void serialize(Archive & ar, const unsigned int version)
    {
	ar & c_;
    }
  } ;
  std::shared_ptr<B> b_;
  std::string str1_;

  FLEXBIN_CLASS_ID(1);
//  FLEXBIN_SERIALIZE_REQUIRED(a_, b_);
  template<class Archive>
  void serialize(Archive & ar, const unsigned int version)
  {
      ar & a_;
      ar & b_;
      ar & str1_;
  }

  bool operator==(const A &r) const { 
    return 
      a_ == r.a_ && 
      b_->c_  == r.b_->c_ &&
      str1_ == r.str1_  
    ; 
 //   return a_ == r.a_ ; 
  }
};

TEST(TestFlexbin, CustomSerializer)
{
  A a1 { 
    999 , 
    std::shared_ptr<A::B>(new A::B{999}),
    "lalala" 
  };
  A a2 { 
    878787,  
    nullptr,
    "lululu" 
  };

  serialize_and_compare(a1, a2);
}
