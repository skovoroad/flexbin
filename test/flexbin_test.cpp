#include <fstream>
#include <cstdlib>
#include <typeinfo>

#include "flexbin.hpp"

#include "gtest/gtest.h"

class TestFlexbin : public ::testing::Test {
public:
    TestFlexbin() { /* init protected members here */ }
    ~TestFlexbin() { /* free protected members here */ }
    void SetUp() { /* called before every test */ }
    void TearDown() { /* called after every test */ }
};

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
  ASSERT_EQ(l, r) << " " << typeid(l).name() << " " << typeid(l.value_).name() ;
  ;
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
inline void test_fundamental_type_required_field()
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

//    std::cout << random << " " << (T) value << std::endl;
    TestData t1 { value };
    TestData t2 { 0 };
    serialize_and_compare(t1, t2);  
  }
}

TEST(TestFlexbin, FundamentalRequired)
{
  test_fundamental_type_required_field<TestDataOneFieldReqiured<uint64_t>>();
  test_fundamental_type_required_field<TestDataOneFieldReqiured<uint32_t>>();
  test_fundamental_type_required_field<TestDataOneFieldReqiured<uint16_t>>();
  test_fundamental_type_required_field<TestDataOneFieldReqiured<uint8_t>>();
  test_fundamental_type_required_field<TestDataOneFieldReqiured<int64_t>>();
  test_fundamental_type_required_field<TestDataOneFieldReqiured<int32_t>>();
  test_fundamental_type_required_field<TestDataOneFieldReqiured<int16_t>>();
  test_fundamental_type_required_field<TestDataOneFieldReqiured<int8_t>>();
}
TEST(TestFlexbin, FundamentalFixed)
{
  test_fundamental_type_required_field<TestDataOneFieldFixed<uint64_t>>();
  test_fundamental_type_required_field<TestDataOneFieldFixed<uint32_t>>();
  test_fundamental_type_required_field<TestDataOneFieldFixed<uint16_t>>();
  test_fundamental_type_required_field<TestDataOneFieldFixed<uint8_t>>();
  test_fundamental_type_required_field<TestDataOneFieldFixed<int64_t>>();
  test_fundamental_type_required_field<TestDataOneFieldFixed<int32_t>>();
  test_fundamental_type_required_field<TestDataOneFieldFixed<int16_t>>();
  test_fundamental_type_required_field<TestDataOneFieldFixed<int8_t>>();
}
TEST(TestFlexbin, FundamentalOptional)
{
  test_fundamental_type_required_field<TestDataOneFieldOptional<uint64_t>>();
  test_fundamental_type_required_field<TestDataOneFieldOptional<uint32_t>>();
  test_fundamental_type_required_field<TestDataOneFieldOptional<uint16_t>>();
  test_fundamental_type_required_field<TestDataOneFieldOptional<uint8_t>>();
  test_fundamental_type_required_field<TestDataOneFieldOptional<int64_t>>();
  test_fundamental_type_required_field<TestDataOneFieldOptional<int32_t>>();
  test_fundamental_type_required_field<TestDataOneFieldOptional<int16_t>>();
  test_fundamental_type_required_field<TestDataOneFieldOptional<int8_t>>();
}
TEST(TestFlexbin, FundamentalSimplified)
{
  test_fundamental_type_required_field<TestDataOneFieldSimplified<uint64_t>>();
  test_fundamental_type_required_field<TestDataOneFieldSimplified<uint32_t>>();
  test_fundamental_type_required_field<TestDataOneFieldSimplified<uint16_t>>();
  test_fundamental_type_required_field<TestDataOneFieldSimplified<uint8_t>>();
  test_fundamental_type_required_field<TestDataOneFieldSimplified<int64_t>>();
  test_fundamental_type_required_field<TestDataOneFieldSimplified<int32_t>>();
  test_fundamental_type_required_field<TestDataOneFieldSimplified<int16_t>>();
  test_fundamental_type_required_field<TestDataOneFieldSimplified<int8_t>>();
}

int main(int argc, char** argv)
{
  std::cout << "uint64_t code: " <<  typeid(uint64_t(0)).name() << std::endl;
  std::cout << "uint32_t code: " <<  typeid(uint32_t(0)).name() << std::endl;
  std::cout << "uint16_t code: " <<  typeid(uint16_t(0)).name() << std::endl;
  std::cout << "uint8_t code: " <<  typeid(uint8_t(0)).name() << std::endl;
  std::cout << "int64_t code: " <<  typeid(int64_t(0)).name() << std::endl;
  std::cout << "int32_t code: " <<  typeid(int32_t(0)).name() << std::endl;
  std::cout << "int16_t code: " <<  typeid(int16_t(0)).name() << std::endl;
  std::cout << "int8_t code: " <<  typeid(int8_t(0)).name() << std::endl;

   ::testing::InitGoogleTest(&argc, argv);
   return RUN_ALL_TESTS();
}


