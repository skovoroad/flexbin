#include <iomanip>
#include <sstream>
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
    std::string strval_;

    test_substruct ss_;

    FLEXBIN_CLASS_ID(777);

    FLEXBIN_SERIALIZE_FIXED(val32_)
//    FLEXBIN_SERIALIZE_REQUIRED(val64_, ss_)
    FLEXBIN_SERIALIZE_REQUIRED(val64_, strval_)
    FLEXBIN_SERIALIZE_OPTIONAL(val8_)
    FLEXBIN_SERIALIZE_SIMPLIFIED(val32_2_)

    void dump() {
      std::cout << (int)val64_ << " " << (int)val32_ << " " << (int)val32_2_ << " " << (int)val8_ << " " << strval_;
      
    }
  };

  inline bool operator==(const test_struct& lhs, const test_struct& rhs)
  {
/*    std::cout << "comparing "  << std::endl
      << lhs.val64_ << " " << lhs.val8_ << " " << lhs.val32_ << " " << lhs.ss_.strval_ << std::endl
      << rhs.val64_ << " " << rhs.val8_ << " " << rhs.val32_ << " " << rhs.ss_.strval_ << std::endl
      ;
      */

    return lhs.val64_ == rhs.val64_ &&
      lhs.val32_ == rhs.val32_ &&
      lhs.val32_2_ == rhs.val32_2_ &&
      lhs.val8_ == rhs.val8_ && 
      lhs.strval_ == rhs.strval_

    ;
  }
}


int main(int argc, char** argv)
{
  test_data::test_struct a{ 1000, 1, 7, 77, { "first"} , "third"};
  test_data::test_struct b{ 0, 2, 8, 88,{ "second"}, "fourth" };

  std::cout << "A: ";
  a.dump();
  std::cout << std::endl;

  std::cout << "B: ";
  b.dump();
  std::cout << std::endl;

  //std::stringbuf fbuf;
  char mem[128];
  std::memset(mem, 0, 128);
  flexbin::memmap_buffer fbuf(mem, mem + 128);

  flexbin::istream fbin(&fbuf);
  flexbin::ostream fbout(&fbuf);

  auto b_size = flexbin::class_object_size(b);
  std::cerr << "b size" << b_size << std::endl;

  fbout << b;
  if (!fbout) {
    std::cerr << "ostr error" << std::endl;
  }

  std::cout << "Buffer: ";
  std::ios_base::fmtflags f(std::cout.flags());
  //for (size_t i = 0; i < fbuf.str().size(); ++i)
    //std::cout << std::hex << std::setfill('0') << std::setw(2) << (unsigned int)(unsigned char) fbuf.str()[i] << " ";
  for (size_t i = 0; i < 128; ++i)
    std::cout << std::hex << std::setfill('0') << std::setw(2) << (unsigned int)(unsigned char)mem[i] << " ";
  std::cout << std::endl;
  std::cout.flags(f);


  uint16_t classid(0);
  //std::string str = fbuf.str();
  //if (!flexbin::class_id(str.data(), str.size(), classid))
  if (!flexbin::class_id(mem, 128, classid))
  {
    std::cerr << "error getting classid" << std::endl;
  }
  else
  {
    if (classid != (uint16_t)test_data::test_struct::flexbin_class_id)
      std::cerr << "bad class id detected: " << classid << std::endl;
  }

  //  if(str.size() != b_size)
  //    std::cerr << "Bad b size! real size: " << str.size() << std::endl;

    //fbout.flush();
  fbin >> a;
  if (!fbin) {
    std::cerr << "istr error" << std::endl;
    //    return 0;
  }


  std::cout << "A: ";
  a.dump();
  std::cout << std::endl;

  std::cout << "B: ";
  b.dump();
  std::cout << std::endl;

  std::cout << (a == b) << std::endl;
}

  // todo:
  // + 1. compact value representation
  // + 2. flexbin fields encoding
  // + 3. required, fixed, optional, simplified
  // 4. flexstring
  // 5. all types support
  // 6. tests
  // + 7. write complex object ?
  // + 8. reading...
  // + 10. memalloc_buffer for streams, 
  // + 11 preallocate proper size
  // 12. Maybe optimize if only fixed fields?
  // 13. error handling

  /*
    to pack value we need to:
  1. get tuple of all descent types (from traits)
  2. iterate from smallest type to largest
   if static_cast<small>() returns equal value - write it
  3. remove pack/write member-functions from traits
  4. move pack/write function to flexbin_write_strategies.hpp
  5. do we need field_writer structure? can we use just functions?

  */

  // questions
  // fixed cannot be supported for strings (no place for length)

//  !!!!!!!!!!!!!!!!!!!!!!!!
// VC++ this work
//  constexpr size_t sizeof_funptr = sizeof(&test_data::test_struct::flexbin_serialize_optional);
//  flexbin::creatable_non_zero< sizeof_funptr  > tt(0);

// VC++ this doesn't work
//  flexbin::creatable_non_zero< sizeof(&test_data::test_struct::flexbin_serialize_optional) > ttt(0);

