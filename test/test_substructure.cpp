#include "gtest/gtest.h"
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

TEST(TestFlexbin, SubstructRequired)
{
  struct A {
    int a_ = 0;
    struct B {
      short c_ = 0;

      FLEXBIN_CLASS_ID(2);
      FLEXBIN_SERIALIZE_REQUIRED(c_);

    } b_;

    FLEXBIN_CLASS_ID(1);
    FLEXBIN_SERIALIZE_REQUIRED(a_, b_);

    bool operator==(const A &r) const { 
      return a_ == r.a_ && b_.c_  == r.b_.c_; 
    }
  };

  A a1 { 999, {999} };
  A a2 { 878787,  {87} };

  serialize_and_compare(a1, a2);
}
/*
TEST(TestFlexbin, SubstructOptional)
{
  struct A {
    int a_ = 0;
    struct B {
      short c_ = 0;

      FLEXBIN_CLASS_ID(2);
      FLEXBIN_SERIALIZE_OPTIONAL(c_);

    } b_;

    FLEXBIN_CLASS_ID(1);
    FLEXBIN_SERIALIZE_OPTIONAL(a_, b_);


    bool operator==(const A &r) const { 
      return a_ == r.a_ && b_.c_  == r.b_.c_; 
    }
  };

  A a1 { 999, {999} };
  A a2 { 878787,  {87} };

  serialize_and_compare(a1, a2);
}*/

TEST(TestFlexbin, SubstructFixed)
{
  struct A {
    int a_ = 0;
    struct B {
      short c_ = 0;

      FLEXBIN_CLASS_ID(2);
      FLEXBIN_SERIALIZE_FIXED(c_);

    } b_;

    FLEXBIN_CLASS_ID(1);
    FLEXBIN_SERIALIZE_FIXED(a_, b_);


    bool operator==(const A &r) const { 
      return a_ == r.a_ && b_.c_  == r.b_.c_; 
    }
  };

  A a1 { 999, {999} };
  A a2 { 878787,  {87} };

  serialize_and_compare(a1, a2);
}

/*
TEST(TestFlexbin, SubstructSimplified)
{
  struct A {
    int a_ = 0;
    struct B {
      short c_ = 0;

      FLEXBIN_CLASS_ID(2);
      FLEXBIN_SERIALIZE_SIMPLIFIED(c_);

    } b_;

    FLEXBIN_CLASS_ID(1);
    FLEXBIN_SERIALIZE_SIMPLIFIED(a_, b_);


    bool operator==(const A &r) const { 
      return a_ == r.a_ && b_.c_  == r.b_.c_; 
    }
  };

  A a1 { 999, {999} };
  A a2 { 878787,  {87} };

  serialize_and_compare(a1, a2);
}
*/