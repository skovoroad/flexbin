#include <string>
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

TEST(TestFlexbin, String)
{
  struct Strings {
    std::string a_;
    std::string b_;
    std::string c_;
    std::string d_;

    FLEXBIN_CLASS_ID(1234);
    FLEXBIN_SERIALIZE_REQUIRED(a_);
    FLEXBIN_SERIALIZE_FIXED(b_);
    //FLEXBIN_SERIALIZE_OPTIONAL(c_);
    //FLEXBIN_SERIALIZE_SIMPLIFIED(d_);

    bool operator==(const Strings &r) const { 
      return a_ == r.a_ && 
             b_  == r.b_ //&&
             //c_  == r.c_ &&
             //d_  == r.d_
             ; 
    }
  };

  Strings a1 { "one", "", "two three", "FIVE"  };
  Strings a2 { "SIx", "seven,eight", "9", "t e n"  };

  serialize_and_compare(a1, a2);
}

TEST(TestFlexbin, BasicString)
{
  struct Strings {
    std::basic_string<uint8_t> a_;
    std::basic_string<uint16_t> b_;
    std::basic_string<uint32_t> c_;
    std::basic_string<uint64_t> d_;

    FLEXBIN_CLASS_ID(1234);
    FLEXBIN_SERIALIZE_REQUIRED(a_);
    FLEXBIN_SERIALIZE_FIXED(b_);
    //FLEXBIN_SERIALIZE_OPTIONAL(c_);
    //FLEXBIN_SERIALIZE_SIMPLIFIED(d_);

    bool operator==(const Strings &r) const {
      return a_ == r.a_ &&
        b_ == r.b_ //&&
        //c_  == r.c_ &&
        //d_  == r.d_
        ;
    }
  };

  Strings a1{ {0, 1, 250}, {1111, 22222}, { (uint32_t)0-1, 321}, {11111111111,4444444444}};
  Strings a2{  };

  serialize_and_compare(a1, a2);
}


TEST(TestFlexbin, StringView)
{
  struct Strings {
    flexbin::buffered_string_view a_;
    flexbin::buffered_string_view b_;
    flexbin::buffered_string_view c_;
    flexbin::buffered_string_view d_;

    FLEXBIN_CLASS_ID(1234);
    FLEXBIN_SERIALIZE_REQUIRED(a_);
    FLEXBIN_SERIALIZE_FIXED(b_);
    //FLEXBIN_SERIALIZE_OPTIONAL(c_);
    //FLEXBIN_SERIALIZE_SIMPLIFIED(d_);

    bool operator==(const Strings &r) const {
      return a_ == r.a_ &&
        b_ == r.b_ //&&
        //c_  == r.c_ &&
        //d_  == r.d_
        ;
    }
  };

  Strings a1{ "one", "", "two three", "FIVE" };
  Strings a2{ "SIx", "seven,eight", "9", "t e n" };

  serialize_and_compare(a1, a2);

}
