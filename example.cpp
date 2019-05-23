#include <fstream>
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
    uint32_t val32_2_;
    uint8_t  val8_;

    test_substruct ss_;

    FLEXBIN_CLASS_ID(777);

    FLEXBIN_SERIALIZE_FIXED(val32_)    
    FLEXBIN_SERIALIZE_REQUIRED(val64_, ss_)
    FLEXBIN_SERIALIZE_OPTIONAL(val8_)    
    FLEXBIN_SERIALIZE_SIMPLIFIED(val32_2_)    
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


int main(int argc, char** argv)
{
  test_data::test_struct a { 10^5, 1, 7, 77, { "first"}};
  test_data::test_struct b { 0, 1, 7, 88,{ "second"}};

  std::filebuf fbuf;
  fbuf.open("/tmp/inout",std::ios_base::in | std::ios_base::out);
  flexbin::istream fbin(&fbuf);
  flexbin::ostream fbout(&fbuf);

  fbout << b;
  fbout.flush();
  fbin >> a;
}
