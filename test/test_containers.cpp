#include <vector>
#include "test_object.h"

template<typename T>
inline void serialize_and_compare(const T& l, T& r) {
  auto result = fbtest::serialize_and_return_classid(l, r);
  uint16_t classid = std::get<fbtest::result_class_id>(result);
  
  ASSERT_TRUE(std::get<fbtest::result_fbout_good>(result));
  ASSERT_TRUE(std::get<fbtest::result_fbin_good>(result));
  ASSERT_TRUE(std::get<fbtest::result_class_id>(result));
  ASSERT_EQ (static_cast<uint16_t>(T::flexbin_class_id), classid);
  ASSERT_EQ(l, r) << " " << typeid(l).name()  ;
};

TEST(TestFlexbin, Vector)
{
  struct Vectors {
    struct Substruct {
      uint16_t a_;
      std::string b_;

      bool operator==(const Substruct &r) const { 
        return a_ == r.a_ && 
               b_  == r.b_ 
               ; 
      }

      FLEXBIN_CLASS_ID(666);
      FLEXBIN_SERIALIZE_REQUIRED(a_, b_);    
    };


    std::vector<Substruct> a_;
    std::vector<uint64_t> b_;
    std::vector<int8_t> c_;
    std::vector<uint64_t> d_;

    FLEXBIN_CLASS_ID(1234);
    FLEXBIN_SERIALIZE_REQUIRED(a_);
    FLEXBIN_SERIALIZE_FIXED(b_);
//    FLEXBIN_SERIALIZE_OPTIONAL(c_);
//    FLEXBIN_SERIALIZE_SIMPLIFIED(d_);

    bool operator==(const Vectors &r) const { 
      return a_ == r.a_ && 
             b_  == r.b_ //&&
             //c_  == r.c_ &&
             //d_  == r.d_
             ; 
    }
  };

  Vectors a1 { 
    { {1, "one"}, {2, "two"} }, 
    {} , 
    { -41} , 
    { 321, 5454545, 5555555555, 111}, 
  };

  Vectors a2;

//  EnumStruct a1 { EnumStruct::Test_A, EnumStruct::Test_B, EnumStruct::Test_C, EnumStruct::Test_D};
//  EnumStruct a2 { EnumStruct::Test_D, EnumStruct::Test_C, EnumStruct::Test_B, EnumStruct::Test_A,};

  serialize_and_compare(a1, a2);
}