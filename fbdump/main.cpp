#include <iomanip>
#include <fstream>
#include <iostream>
#include <vector>

#define FLEXBIN_DEBUG_LOG_ON
#include "flexbin.hpp"

namespace flexbin_experimental {
  namespace type_code {
    template<typename T> uint8_t get() { return 29; }
    template<> uint8_t get<uint64_t>() { return 16; }
    template<> uint8_t get<uint8_t>() { return 2; }
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
    inline static size_t pack(ostream& ostr, uint8_t field_id, const T& val) {
      auto candidate = static_cast<TCandidate>(val);
      if (candidate == val) {
        return write_field_header<TCandidate>(ostr, field_id) +
          type_traits<TCandidate>::write(ostr, val);
      }

      return type_packer<T, Rest...>::pack(ostr, field_id, val);
    }

  };

  template<typename T, typename TCandidate>
  struct type_packer<T, TCandidate>
  {
    inline static size_t pack(ostream& ostr, uint8_t field_id, const T& val) {
      auto candidate = static_cast<TCandidate>(val);
      if (candidate == val) {
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
    inline static size_t pack(ostream& ostr, uint8_t field_id, const T& val) {
      return write_field_header<T>(ostr, field_id) +
        type_traits<T>::write(ostr, val);
    }
  };

  template<typename T>
  struct type_writer
  {
    inline static size_t write(ostream& ostr, const T& val) {
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
  /////////////
  struct flexbin_read_dumper {
    size_t indent_ = 0;
    size_t indent_step_ = 2;

    std::string indent() { return std::string(indent_, ' '); }

    bool on_object_header(uint32_t size, uint16_t id = 0) {
      std::cout << indent() << "OBJECT size: " << size << " id: " << id << std::endl;
      indent_ += indent_step_;
      return true;
    }

    template<typename T>
    bool on_value(size_t id, const T& value) {
      std::cout << indent() << "FIELD id: " << id << std::endl;
    }

    bool on_end_marker() {
      if (indent_ < indent_step_) {
        std::cerr << "Malformed packet, start/close object mismatched" << std::endl;
        return false;
      }

      indent_ -= indent_step_;
      std::cout << indent() << "OBJECT end" << std::endl;
      return true;
    }
  };

  struct flexbin_reader {
    bool dump(const char *data, size_t nbytes) {
      size_t pos = 0;
      while (pos < nbytes) {
      
      }
    }

  };

}

void test_new_write()
{
  constexpr size_t bufsize = 256;
  char mem[bufsize];
  std::fill(mem, mem + bufsize, 0);

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

void dump(const char *data, size_t nbytes) {

}

int main(int argc, char** argv)
{
  if (argc < 2) {
    std::cout << "Usage: fbdump filename" << std::endl;
    return -1;
  }
  std::ifstream input(argv[1], std::ios::binary);
  if(!input){
    std::cout << "Error: cannot open file " << argv[1] << std::endl;
    return -1;
  }

  std::vector<char> data(std::istreambuf_iterator<char>(input), {});
  dump(data.data(), data.size());
  return 0; 
}
