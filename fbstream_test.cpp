#include <fstream>
#include "gtest/gtest.h"
#include "flexbin.hpp"

namespace test_data
{
  struct test_struct
  {
    uint64_t val64_;
    uint32_t val32_;
  };

  inline bool operator==(const test_struct& lhs, 
					     const test_struct& rhs)
  {
    return lhs.val64_ == rhs.val64_ &&
           lhs.val32_ == rhs.val32_;
  }

}


class TestFlexbin : public ::testing::Test {
public:
    TestFlexbin() { /* init protected members here */ }
    ~TestFlexbin() { /* free protected members here */ }
    void SetUp() { /* called before every test */ }
    void TearDown() { /* called after every test */ }
};

TEST(TestFlexbin, SimpleInOutEquality)
{
  test_data::test_struct a { 10^5, 1};
  test_data::test_struct b { 0, 0};

  std::filebuf fbuf;
  fbuf.open("/tmp/inout",std::ios_base::in | std::ios_base::out);
  flexbin::istream fbin(&fbuf);
  flexbin::ostream fbout(&fbuf);

  fbin >> a;
  fbout << b;

  ASSERT_EQ(a, b);

}

int main(int argc, char** argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}


