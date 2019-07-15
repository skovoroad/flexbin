#include <memory>
#include "test_object.h"

template<typename T>
inline void serialize_and_compare(const std::shared_ptr<T>& l, std::shared_ptr<T>& r) {
  auto result = fbtest::serialize_and_return_classid(l, r);
  uint16_t classid = std::get<fbtest::result_class_id>(result);
  
  ASSERT_TRUE(std::get<fbtest::result_fbout_good>(result));
  ASSERT_TRUE(std::get<fbtest::result_fbin_good>(result));
  ASSERT_TRUE(std::get<fbtest::result_class_id>(result));
  ASSERT_EQ (static_cast<uint16_t>(T::flexbin_class_id), classid);
  ASSERT_EQ(*l, *r) << " " << typeid(l).name()  ;
};

template<typename T>
inline void serialize_and_compare(const std::unique_ptr<T>& l, std::unique_ptr<T>& r) {
  auto result = fbtest::serialize_and_return_classid(l, r);
  uint16_t classid = std::get<fbtest::result_class_id>(result);
  
  ASSERT_TRUE(std::get<fbtest::result_fbout_good>(result));
  ASSERT_TRUE(std::get<fbtest::result_fbin_good>(result));
  ASSERT_TRUE(std::get<fbtest::result_class_id>(result));
  ASSERT_EQ (static_cast<uint16_t>(T::flexbin_class_id), classid);
  ASSERT_EQ(*l, *r) << " " << typeid(l).name()  ;
};


TEST(TestFlexbin, SharedPtr)
{
  std::shared_ptr<SampleStruct> a1{ new SampleStruct{999, 888, 777, 666} };
  std::shared_ptr<SampleStruct> a2{ new SampleStruct{111, 222, 333, 555} };

  serialize_and_compare(a1, a2);	
}

TEST(TestFlexbin, UniquePtr)
{
  std::unique_ptr<SampleStruct> a1{ new SampleStruct{999, 888, 777, 666} };
  std::unique_ptr<SampleStruct> a2{ new SampleStruct{111, 222, 333, 555} };

  serialize_and_compare(a1, a2);	
}