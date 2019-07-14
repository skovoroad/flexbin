#include <fstream>
#include <cstdlib>

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
  ASSERT_EQ(l, r);
};

template<typename T> 
inline void test_fundamental_type_required_field()
{
  struct TestData {
    T value_;
    bool operator==(const TestData &rhs) const { 
      return value_ == rhs.value_; 
    }

    FLEXBIN_CLASS_ID(1);
    FLEXBIN_SERIALIZE_REQUIRED(value_);

  };

  {
    TestData t1 { std::numeric_limits<T>::min() };
    TestData t2 { 0 };
    serialize_and_compare(t1, t2);  
  }
  {
    TestData t1 { std::numeric_limits<T>::min()+1 };
    TestData t2 { 0 };
    serialize_and_compare(t1, t2);  
  }
  {
    TestData t1 { std::numeric_limits<T>::max() };
    TestData t2 { 0 };
    serialize_and_compare(t1, t2);  
  }
  {
    TestData t1 { std::numeric_limits<T>::max()-1 };
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

    T value = std::numeric_limits<T>::min() + static_cast<T>(
      random * static_cast<float>(std::numeric_limits<T>::max()) - 
      random * static_cast<float>(std::numeric_limits<T>::min()));

//    std::cout << random << " " << (T) value << std::endl;
    TestData t1 { value };
    TestData t2 { 0 };
    serialize_and_compare(t1, t2);  
  }
}

TEST(TestFlexbin, FundamentalRequired)
{
  test_fundamental_type_required_field<uint64_t>();
  test_fundamental_type_required_field<uint32_t>();
  test_fundamental_type_required_field<uint16_t>();
  test_fundamental_type_required_field<uint8_t>();
  test_fundamental_type_required_field<int64_t>();
  test_fundamental_type_required_field<int32_t>();
  test_fundamental_type_required_field<int16_t>();
  test_fundamental_type_required_field<int8_t>();
}


int main(int argc, char** argv)
{
   ::testing::InitGoogleTest(&argc, argv);
   return RUN_ALL_TESTS();
}


