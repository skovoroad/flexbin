#include <fstream>
#include <cstdlib>
#include <typeinfo>

#include "flexbin.hpp"
#include "flexbin_impl.hpp"

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

template<typename T>
struct TestDataOneFieldReqiured {
  typedef T Basetype;
  T value_;
  bool operator==(const TestDataOneFieldReqiured<T> &rhs) const { 
    return value_ == rhs.value_; 
  }

  FLEXBIN_CLASS_ID(1);
  FLEXBIN_SERIALIZE_REQUIRED(value_);
};

template<typename T>
struct TestDataOneFieldFixed {
  typedef T Basetype;
  T value_;
  bool operator==(const TestDataOneFieldFixed<T> &rhs) const { 
    return value_ == rhs.value_; 
  }

  FLEXBIN_CLASS_ID(1);
  FLEXBIN_SERIALIZE_FIXED(value_);
};

template<typename T>
struct TestDataOneFieldOptional {
  typedef T Basetype;
  T value_;
  bool operator==(const TestDataOneFieldOptional<T> &rhs) const { 
    return value_ == rhs.value_; 
  }

  FLEXBIN_CLASS_ID(1);
  FLEXBIN_SERIALIZE_OPTIONAL(value_);
};

template<typename T>
struct TestDataOneFieldSimplified {
  typedef T Basetype;
  T value_;
  bool operator==(const TestDataOneFieldSimplified<T> &rhs) const { 
    return value_ == rhs.value_; 
  }

  FLEXBIN_CLASS_ID(1);
  FLEXBIN_SERIALIZE_SIMPLIFIED(value_);
};


template<typename TestData> 
inline void test_fundamental_type_field()
{
  {
    TestData t1 { std::numeric_limits<typename TestData::Basetype>::min() };
    TestData t2 { 0 };
    serialize_and_compare(t1, t2);  
  }
  {
    TestData t1 { std::numeric_limits<typename TestData::Basetype>::min()+1 };
    TestData t2 { 0 };
    serialize_and_compare(t1, t2);  
  }
  {
    TestData t1 { std::numeric_limits<typename TestData::Basetype>::max() };
    TestData t2 { 0 };
    serialize_and_compare(t1, t2);  
  }
  {
    TestData t1 { std::numeric_limits<typename TestData::Basetype>::max()-1 };
    TestData t2 { 0 };
    serialize_and_compare(t1, t2);  
  }
  {
    TestData t1 { 0 };
    TestData t2 { 0 };
    serialize_and_compare(t1, t2);  
  }
  for (size_t i = 0; i < 1000; ++i)
  {
    float random = static_cast<float>(std::rand()) / static_cast<float>(RAND_MAX);

    typename TestData::Basetype value = std::numeric_limits<typename TestData::Basetype>::min() + 
      static_cast<typename TestData::Basetype>(
        random * static_cast<float>(std::numeric_limits<typename TestData::Basetype>::max()) - 
        random * static_cast<float>(std::numeric_limits<typename TestData::Basetype>::min())
      );

    TestData t1 { value };
    TestData t2 { 0 };
    serialize_and_compare(t1, t2);  
  }
}

TEST(TestFlexbin, FundamentalRequired)
{
  test_fundamental_type_field<TestDataOneFieldReqiured<uint64_t>>();
  test_fundamental_type_field<TestDataOneFieldReqiured<uint32_t>>();
  test_fundamental_type_field<TestDataOneFieldReqiured<uint16_t>>();
  test_fundamental_type_field<TestDataOneFieldReqiured<uint8_t>>();
  test_fundamental_type_field<TestDataOneFieldReqiured<int64_t>>();
  test_fundamental_type_field<TestDataOneFieldReqiured<int32_t>>();
  test_fundamental_type_field<TestDataOneFieldReqiured<int16_t>>();
  test_fundamental_type_field<TestDataOneFieldReqiured<int8_t>>();
  test_fundamental_type_field<TestDataOneFieldReqiured<float>>();
  test_fundamental_type_field<TestDataOneFieldReqiured<double>>();
}
TEST(TestFlexbin, FundamentalFixed)
{
  test_fundamental_type_field<TestDataOneFieldFixed<uint64_t>>();
  test_fundamental_type_field<TestDataOneFieldFixed<uint32_t>>();
  test_fundamental_type_field<TestDataOneFieldFixed<uint16_t>>();
  test_fundamental_type_field<TestDataOneFieldFixed<uint8_t>>();
  test_fundamental_type_field<TestDataOneFieldFixed<int64_t>>();
  test_fundamental_type_field<TestDataOneFieldFixed<int32_t>>();
  test_fundamental_type_field<TestDataOneFieldFixed<int16_t>>();
  test_fundamental_type_field<TestDataOneFieldFixed<int8_t>>();
  test_fundamental_type_field<TestDataOneFieldFixed<double>>();
  test_fundamental_type_field<TestDataOneFieldFixed<float>>();
}
TEST(TestFlexbin, FundamentalOptional)
{
  test_fundamental_type_field<TestDataOneFieldOptional<uint64_t>>();
  test_fundamental_type_field<TestDataOneFieldOptional<uint32_t>>();
  test_fundamental_type_field<TestDataOneFieldOptional<uint16_t>>();
  test_fundamental_type_field<TestDataOneFieldOptional<uint8_t>>();
  test_fundamental_type_field<TestDataOneFieldOptional<int64_t>>();
  test_fundamental_type_field<TestDataOneFieldOptional<int32_t>>();
  test_fundamental_type_field<TestDataOneFieldOptional<int16_t>>();
  test_fundamental_type_field<TestDataOneFieldOptional<int8_t>>();
  test_fundamental_type_field<TestDataOneFieldOptional<double>>();
  test_fundamental_type_field<TestDataOneFieldOptional<float>>();
}
TEST(TestFlexbin, FundamentalSimplified)
{
  test_fundamental_type_field<TestDataOneFieldSimplified<uint64_t>>();
  test_fundamental_type_field<TestDataOneFieldSimplified<uint32_t>>();
  test_fundamental_type_field<TestDataOneFieldSimplified<uint16_t>>();
  test_fundamental_type_field<TestDataOneFieldSimplified<uint8_t>>();
  test_fundamental_type_field<TestDataOneFieldSimplified<int64_t>>();
  test_fundamental_type_field<TestDataOneFieldSimplified<int32_t>>();
  test_fundamental_type_field<TestDataOneFieldSimplified<int16_t>>();
  test_fundamental_type_field<TestDataOneFieldSimplified<int8_t>>();
  test_fundamental_type_field<TestDataOneFieldSimplified<double>>();
  test_fundamental_type_field<TestDataOneFieldSimplified<float>>();
}

TEST(TestFlexbin, Enum)
{
  struct EnumStruct {
    enum TestEnum {
      Test_A,
      Test_B,
      Test_C,
      Test_D,
      Test_Unknown
    };

    TestEnum a_ = Test_Unknown;
    TestEnum b_ = Test_Unknown;
    TestEnum c_ = Test_Unknown;
    TestEnum d_ = Test_Unknown;

    FLEXBIN_CLASS_ID(1234);
    FLEXBIN_SERIALIZE_REQUIRED(a_);
    FLEXBIN_SERIALIZE_FIXED(b_);
    //FLEXBIN_SERIALIZE_OPTIONAL(c_);
    //FLEXBIN_SERIALIZE_SIMPLIFIED(d_);


    bool operator==(const EnumStruct &r) const { 
      return a_ == r.a_ && 
             b_  == r.b_ //&&
      //       c_  == r.c_ &&
      //       d_  == r.d_
             ; 
    }
  };

  EnumStruct a1 { EnumStruct::Test_A, EnumStruct::Test_B, EnumStruct::Test_C, EnumStruct::Test_D};
  EnumStruct a2 { EnumStruct::Test_D, EnumStruct::Test_C, EnumStruct::Test_B, EnumStruct::Test_A,};

  serialize_and_compare(a1, a2);
}
