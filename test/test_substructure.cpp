#include "gtest/gtest.h"
#include "flexbin.hpp"

template<typename T>
inline void serialize_and_compare(const T& l, T& r) {
  auto object_size = flexbin::class_object_size(l);
  std::vector<char> buffer;
  buffer.resize(object_size);

  flexbin::istream fbin(buffer.data(), buffer.size());
  flexbin::ostream fbout(buffer.data(), buffer.size());

  fbout << l;

  uint16_t classid(0);
  auto result = flexbin::class_id(buffer.data(), buffer.size(), classid);
  ASSERT_TRUE(result);
  ASSERT_EQ (T::flexbin_class_id, classid);

  fbin >> r;
  ASSERT_EQ(l, r) << " " << typeid(l).name()  ;
  ;
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