#include <iomanip>
#include <sstream>
#include <vector>
#include <algorithm>

#define FLEXBIN_DEBUG_LOG_ON
#include "flexbin.hpp"

namespace test_data
{
  class special_string : public std::string {
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
} // namespacfe

namespace std {
  template<>
  struct hash<test_data::test_substruct> {
    size_t operator()(const test_data::test_substruct &s) const {
      return hash<string>()(s.strval_) << (s.boolean_ ? 1 : 0);
    }
  };

  template<>
  struct equal_to<test_data::test_substruct> {
    bool operator()(const test_data::test_substruct &l, const test_data::test_substruct &r) const {
      return l.strval_ == r.strval_ && l.boolean_ == r.boolean_;
    }
  };
}

namespace test_data {
  struct test_struct
  {
    uint64_t val64_;
    uint32_t val32_;
    uint32_t val32_2_;
    uint8_t  val8_;
    special_string strval_;
    test_substruct ss_;
    std::unique_ptr<test_substruct> ss2_;
    std::unordered_set<uint64_t> vect_;
    std::unordered_set<test_substruct> vect2_;
    bool boolean_;

    enum SomeEnum { SomeOne, SomeTwo, SomeThree};
    SomeEnum enum_;

    FLEXBIN_CLASS_ID(777);

    FLEXBIN_SERIALIZE_FIXED(val32_)
    FLEXBIN_SERIALIZE_REQUIRED(val64_, ss_, ss2_, strval_, vect_, vect2_, boolean_, enum_)
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

  struct test_substruct2 {
    bool boolean_ = false;

    FLEXBIN_CLASS_ID(887);
    FLEXBIN_SERIALIZE_REQUIRED(boolean_)

  };

  class test_substruct2_deleter {
  public:
    void operator( )(test_substruct2* ptr) const {
      delete ptr;
    }
  };

  struct test_struct2{
    std::unique_ptr<test_substruct2, test_substruct2_deleter> s_;

    test_struct2(bool val) {
      s_.reset(new test_substruct2{val});
    }

    FLEXBIN_CLASS_ID(888);
    FLEXBIN_SERIALIZE_REQUIRED(s_)

    void dump() {
      std::cout << " " << s_->boolean_;
    };
    void reset() {};

    bool operator==(const test_struct2& rhs) {
      return s_->boolean_ == rhs.s_->boolean_;
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

int run()
{
  /*
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
  */

  test_data::test_struct2 a (true);
  test_data::test_struct2 b (false);

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
//  flexbin::memmap_buffer fbuf(mem, mem + bufsize);
//  flexbin::istream fbin(&fbuf);
//  flexbin::ostream fbout(&fbuf);

  flexbin::istream fbin(mem, bufsize);
  flexbin::ostream fbout(mem, bufsize);


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
  for (size_t i = 0; i < bufsize; ++i)
    std::cout << std::hex << std::setfill('0') << std::setw(2) << (unsigned int)(unsigned char)mem[i] << " ";
  std::cout << std::endl;
  std::cout.flags(f);


  uint16_t classid(0);
  if (!flexbin::class_id(mem, bufsize, classid))
  {
    std::cerr << "error getting classid" << std::endl;
  }
  else
  {
    if (classid != (uint16_t)test_data::test_struct::flexbin_class_id)
      std::cerr << "bad class id detected: " << classid << std::endl;
  }

  a.reset();

  fbin >> a;
  if (!fbin) {
    std::cerr << "istr error" << std::endl;
  }


  std::cout << "A: ";
  a.dump();
  std::cout << std::endl;

  std::cout << "B: ";
  b.dump();
  std::cout << std::endl;

  std::cout << (a == b) << std::endl;
  return 0;
}

namespace flexbin_experimental {
  namespace type_code {
    template<typename T> uint8_t get() {return 29;}
    template<> uint8_t get<uint64_t>() {return 16;}
    template<> uint8_t get<uint8_t>()  {return 2;}
  }
  
  template<typename T, class Enabler = void>
  struct type_traits
  {
    enum { code_ = 29 /* "object" field id */ };
  };  

  using ostream = flexbin::ostream;
  using istream = flexbin::istream;

  template<typename T>
  size_t write_field_header(ostream& ostr, uint8_t field_id) {
      uint8_t code = type_traits<T>::code_;
      auto nbytes = sizeof(T) + 2;
      ostr.write(reinterpret_cast<const char*>(&code), 1);
      ostr.write(reinterpret_cast<const char*>(&field_id), 1);
      return nbytes;
  }

  template <typename... Args>
  struct type_packer;

  template<typename T, typename TCandidate, typename... Rest>
  struct type_packer<T, TCandidate, Rest...>
  {
    inline static size_t pack(ostream& ostr, uint8_t field_id, const T& val ) {
      auto candidate = static_cast<TCandidate>(val);
      if(candidate == val) {
        return write_field_header<TCandidate>(ostr, field_id) +
               type_traits<TCandidate>::write(ostr, val);
      }

      return type_packer<T, Rest...>::pack(ostr, field_id, val); 
     }

  };

  template<typename T, typename TCandidate>
  struct type_packer<T, TCandidate>
  {
    inline static size_t pack(ostream& ostr, uint8_t field_id, const T& val ) {
      auto candidate = static_cast<TCandidate>(val);
      if(candidate == val) {
        return write_field_header<TCandidate>(ostr, field_id) +
               type_traits<TCandidate>::write(ostr, val);
      }

        return write_field_header<T>(ostr, field_id) +
               type_traits<T>::write(ostr, val);
     }
  };


  template<typename T>
  struct type_packer<T>
  {
    inline static size_t pack(ostream& ostr, uint8_t field_id, const T& val ) {
      return write_field_header<T>(ostr, field_id) +
               type_traits<T>::write(ostr, val);
     }
  };

  template<typename T>
  struct type_writer
  {
    inline static size_t write(ostream& ostr, const T& val ) {
      ostr.write(reinterpret_cast<const char*>(&val), sizeof(T));
      return sizeof(uint8_t); 
     }

    inline static bool read(istream& istr, T& val) {
      istr.read(reinterpret_cast<char*>(&val), sizeof(T));
      return istr.good();
    }

  };
  ///////////
  template<>
  struct type_traits<uint8_t> : 
    public type_writer<uint8_t>,
           type_packer<uint8_t>
  {
    enum { default_value_ = 0 };
    enum { code_ = 2 };
  };

  template<>
  struct type_traits<uint32_t> : 
    public type_writer<uint32_t>,
           type_packer<uint32_t, uint8_t> // base class first, candidates next
  {
    enum { default_value_ = 0 };
    enum { code_ = 8 };
  };

  template<>
  struct type_traits<uint64_t> : 
    public type_writer<uint64_t>,
           type_packer<uint64_t, uint8_t, uint32_t>// base class first, candidates next
  {
    enum { default_value_ = 0 };
    enum { code_ = 17 };
  };
}

void test_new_write()
{
  constexpr size_t bufsize = 256;
  char mem[bufsize];
  std::fill(mem, mem+bufsize, 0);

  flexbin_experimental::istream fbin(mem, bufsize);
  flexbin_experimental::ostream fbout(mem, bufsize);

  uint64_t value_1 = 7;
  uint64_t value_2 = 777777;
  flexbin_experimental::type_traits<uint64_t>::pack(fbout, 123, value_1);
  flexbin_experimental::type_traits<uint64_t>::pack(fbout, 77, value_2);

  flexbin_experimental::type_traits<uint64_t>::write(fbout, value_1);
  flexbin_experimental::type_traits<uint64_t>::write(fbout, value_2);

  std::cout << "Buffer: ";
  std::ios_base::fmtflags f(std::cout.flags());
  for (size_t i = 0; i < bufsize; ++i)
    std::cout << std::hex << std::setfill('0') << std::setw(2) << (unsigned int)(unsigned char)mem[i] << " ";
  std::cout << std::endl;
  std::cout.flags(f);  
}

int main(int argc, char** argv)
{
  run();
 // test_new_write();
  return 0; 
}
  // todo:
  // 0  shared_ptr, vector - make write/read functions (for fixed fields)
  // + 1. compact value representation
  // + 2. flexbin fields encoding
  // + 3. required, fixed, optional, simplified
  // + 4. flexstring
  // 5. all types support
  // + 6. tests
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


