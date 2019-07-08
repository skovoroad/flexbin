#include <iomanip>
#include <sstream>
#include <vector>
#include <algorithm>
#include "flexbin.hpp"

namespace test_data
{
  class special_string: public std::string {
  public:
  };

  struct test_substruct
  {
    test_substruct() {}
    test_substruct(const char * s, bool b) 
    : strval_(s), boolean_(b) {}

    std::string strval_;
    bool boolean_;    

    FLEXBIN_CLASS_ID(666);

    FLEXBIN_SERIALIZE_REQUIRED(strval_, boolean_)
  };

  struct test_struct
  {
    uint64_t val64_;
    uint32_t val32_;
    uint32_t val32_2_;
    uint8_t  val8_;
    special_string strval_;
    test_substruct ss_;
    std::unique_ptr<test_substruct> ss2_;
    std::vector<uint64_t> vect_;
    std::vector<test_substruct> vect2_;
    bool boolean_;

    enum SomeEnum { SomeOne, SomeTwo, SomeThree};
    SomeEnum enum_;

    FLEXBIN_CLASS_ID(777);

    FLEXBIN_SERIALIZE_FIXED(val32_)
    FLEXBIN_SERIALIZE_REQUIRED(val64_, ss_ , ss2_, strval_, vect_, vect2_, boolean_
      , enum_
      )
    FLEXBIN_SERIALIZE_OPTIONAL(val8_)
    FLEXBIN_SERIALIZE_SIMPLIFIED(val32_2_)
    
//    FLEXBIN_SERIALIZE_REQUIRED( vect2_, boolean_)

    void dump() {
      std::cout << (int)val64_ << " " << (int)val32_ << " " 
        << (int)val32_2_ << " " << (int)val8_ 
        << " " << strval_
        << " " << ss_.strval_ << " " << enum_ 
        << ss2_->strval_ << " " << ss2_->boolean_
        ;

      // " " << vect_
        std::cout << " | ";
        for (auto & v : vect_)
          std::cout <<  " " << v;
        std::cout << " | ";
        for (auto & v : vect2_)
          std::cout <<  " " << v.strval_ << " " <<  v.boolean_;
        std::cout << " | ";
        std::cout << " " << boolean_ << " "<< ss_.boolean_;

      std::cout << " "<< enum_ << " " ;
    }

    void reset() {
      vect_.clear();
      vect2_.clear();

    }

  };


    struct test_struct2{
      bool boolean_ = false;

      FLEXBIN_CLASS_ID(888);
      FLEXBIN_SERIALIZE_REQUIRED(boolean_)

      void dump() {
        std::cout << " " << boolean_ ;
      };
      void reset() {};

      bool operator==(const test_struct2& rhs) {
        return boolean_ == rhs.boolean_;
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
  test_data::test_struct a{ 1000, 1, 7, 77,  "third", 
   { "first", false} ,  
   std::make_unique<test_data::test_substruct>("substr shared", true),
   {567, 765},
   { {"substruct 1", false}, {"substruct 2", false}  }, true, 
   test_data::test_struct::SomeOne 
 };

  test_data::test_struct b{ 0, 2, 8, 88, "fourth",
   { "second", true} , 
   std::make_unique<test_data::test_substruct>("substr 313 shared", false),
   {234, 432} ,
   { {"3 s u b s t r u c t", true}, {" 4 s u b s t r u c t", true}}, false ,
   test_data::test_struct::SomeTwo 
  };

  //test_data::test_struct2 a {true};
  //test_data::test_struct2 b {false};

  std::cout << "A: ";
  a.dump();
  std::cout << std::endl;

  std::cout << "B: ";
  b.dump();
  std::cout << std::endl;

  //std::stringbuf fbuf;
  constexpr size_t bufsize = 256;
  char mem[bufsize];
  //std::memset(mem, 0, 128);
  std::fill(mem, mem+bufsize, 0);
  flexbin::memmap_buffer fbuf(mem, mem + bufsize);

  flexbin::istream fbin(&fbuf);
  flexbin::ostream fbout(&fbuf);

  auto b_size = flexbin::class_object_size(b);
  std::cerr << "b size" << b_size << std::endl;

  uint32_t s;
  if (!flexbin::message_complete(mem, bufsize, s)) {
    std::cerr << "message incomplete! size" << s << std::endl;
  }

  fbout << b;
  if (!fbout) {
    std::cerr << "ostr error" << std::endl;
  }

  std::cout << "Buffer: ";
  std::ios_base::fmtflags f(std::cout.flags());
  //for (size_t i = 0; i < fbuf.str().size(); ++i)
    //std::cout << std::hex << std::setfill('0') << std::setw(2) << (unsigned int)(unsigned char) fbuf.str()[i] << " ";
  for (size_t i = 0; i < bufsize; ++i)
    std::cout << std::hex << std::setfill('0') << std::setw(2) << (unsigned int)(unsigned char)mem[i] << " ";
  std::cout << std::endl;
  std::cout.flags(f);


  uint16_t classid(0);
  //std::string str = fbuf.str();
  //if (!flexbin::class_id(str.data(), str.size(), classid))
  if (!flexbin::class_id(mem, bufsize, classid))
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
  a.reset();

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
// 0  shared_ptr, vector - make write/read functions (for fixed fields)
  // + 1. compact value representation
  // + 2. flexbin fields encoding
  // + 3. required, fixed, optional, simplified
  // + 4. flexstring
  // 5. all types support
  // 6. tests
  // + 7. write complex object ?
  // + 8. reading...
  // + 10. memalloc_buffer for streams, 
  // + 11 preallocate proper size
  // 12. Maybe optimize if only fixed fields?
  // 13. error handling
// 14. Default value for string, T
// 15. Different defaults for different members
// 16. stringview
// 17. read without tuples
// 18. refer right to substruct fields
// 19. debug build checks (same class id etc)


