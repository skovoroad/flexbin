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

template<typename T, typename TDeleter>
inline void serialize_and_compare(const std::unique_ptr<T, TDeleter>& l, std::unique_ptr<T, TDeleter>& r) {
  auto result = fbtest::serialize_and_return_classid(l, r);
  uint16_t classid = std::get<fbtest::result_class_id>(result);
  
  ASSERT_TRUE(std::get<fbtest::result_fbout_good>(result));
  ASSERT_TRUE(std::get<fbtest::result_fbin_good>(result));
  ASSERT_TRUE(std::get<fbtest::result_class_id>(result));
  ASSERT_EQ (static_cast<uint16_t>(T::flexbin_class_id), classid);
  ASSERT_EQ(*l, *r) << " " << typeid(l).name()  ;
};

template<typename T>
inline void serialize_and_compare(const T& l, T& r) {
  auto result = fbtest::serialize_and_return_classid(l, r);
  uint16_t classid = std::get<fbtest::result_class_id>(result);

  ASSERT_TRUE(std::get<fbtest::result_fbout_good>(result));
  ASSERT_TRUE(std::get<fbtest::result_fbin_good>(result));
  ASSERT_TRUE(std::get<fbtest::result_class_id>(result));
  ASSERT_EQ(static_cast<uint16_t>(T::flexbin_class_id), classid);
  ASSERT_EQ(l, r) << " " << typeid(l).name();
};

class SampleStructDeleter {
public:
  void operator( )(SampleStruct* ptr) const {
    delete ptr;
  }
};

struct SomeStructWithPtr {
  std::unique_ptr<SampleStruct, SampleStructDeleter> ptr;

  bool operator==(const SomeStructWithPtr& r) const {
    return ptr->a_ == r.ptr->a_ &&
      ptr->b_ == r.ptr->b_ &&
      ptr->c_ == r.ptr->c_ &&
      ptr->d_ == r.ptr->d_
      ;
  }


  FLEXBIN_CLASS_ID(2434);
  FLEXBIN_SERIALIZE_REQUIRED(ptr);

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

TEST(TestFlexbin, UniquePtrWithDeleter)
{
  std::unique_ptr<SampleStruct, SampleStructDeleter> a1{ new SampleStruct{999, 888, 777, 666} };
  std::unique_ptr<SampleStruct, SampleStructDeleter> a2{ new SampleStruct{111, 222, 333, 555} };

  serialize_and_compare(a1, a2);
}

TEST(TestFlexbin, UniquePtrWithDeleterMember)
{
  SomeStructWithPtr a1;
  a1.ptr.reset(new SampleStruct{ 999, 888, 777, 666 });
  SomeStructWithPtr a2;
  serialize_and_compare(a1, a2);
}
