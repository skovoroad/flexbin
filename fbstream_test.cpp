#include <fstream>
#include "gtest/gtest.h"
#include "flexbin.hpp"

namespace test_data
{
  struct test_substruct
  {
    std::string strval_;

    FLEXBIN_CLASS_ID(666);

    FLEXBIN_SERIALIZE_REQUIRED(strval_)
  };

  struct test_struct
  {
    uint64_t val64_;
    uint32_t val32_;
    uint8_t  val8_;

    test_substruct ss_;

    FLEXBIN_CLASS_ID(777);

    FLEXBIN_SERIALIZE_FIXED(val32_)    
    FLEXBIN_SERIALIZE_REQUIRED(val64_, ss_)
    FLEXBIN_SERIALIZE_OPTIONAL(val8_)    

  };

  inline bool operator==(const test_struct& lhs, const test_struct& rhs)
  {
    std::cout << "comparing "  << std::endl
      << lhs.val64_ << " " << lhs.val8_ << " " << lhs.val32_ << " " << lhs.ss_.strval_ << std::endl
      << rhs.val64_ << " " << rhs.val8_ << " " << rhs.val32_ << " " << rhs.ss_.strval_ << std::endl
      ;


    return lhs.val64_ == rhs.val64_ &&
           lhs.val32_ == rhs.val32_ &&
           lhs.val8_ == rhs.val8_ 
    ;
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
  test_data::test_struct a { 10^5, 1, 7, { "first"}};
  test_data::test_struct b { 0, 1, 7, { "second"}};

  std::filebuf fbuf;
  fbuf.open("/tmp/inout",std::ios_base::in | std::ios_base::out);
  flexbin::istream fbin(&fbuf);
  flexbin::ostream fbout(&fbuf);

  fbout << b;
  fbout.flush();
  fbin >> a;

  ASSERT_EQ(a, b);

}


// todo:
// 1. compact value representation
// + 2. flexbin fields encoding
// 3. requered, fixed, optional, simplified
// 4. flexstring
// 5. all types support
// 6. tests

// questions
// why field id? it does nothing (if no optional or simplified)
// fixed cannot be object or string (no place for length)
// 

int main(int argc, char** argv)
{
   ::testing::InitGoogleTest(&argc, argv);
   return RUN_ALL_TESTS();
}


