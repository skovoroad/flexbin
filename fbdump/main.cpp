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
    template<> uint8_t get<uint32_t>() { return 8; }
    template<> uint8_t get<uint16_t>() { return 4; }
    template<> uint8_t get<uint8_t>() { return 2; }
  }

  template<typename T, class Enabler = void>
  struct type_traits
  {
  };

  using ostream = flexbin::ostream;
  using istream = flexbin::istream;

  template<typename T>
  size_t write_field_header(ostream& ostr, uint8_t field_id) {
    uint8_t code = type_code::get<T>();
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
  };

  template<>
  struct type_traits<uint16_t> :
    public type_writer<uint16_t>,
    type_packer<uint16_t, uint8_t> // base class first, candidates next
  {
  };


  template<>
  struct type_traits<uint32_t> :
    public type_writer<uint32_t>,
    type_packer<uint32_t, uint8_t, uint16_t> // base class first, candidates next
  {
  };

  template<>
  struct type_traits<uint64_t> :
    public type_writer<uint64_t>,
    type_packer<uint64_t, uint8_t, uint16_t, uint32_t>// base class first, candidates next
  {
  };
  /////////////
  struct flexbin_read_dumper {
    static bool on_object_header(uint32_t size, uint16_t id = 0) {
      std::cout << "OBJECT size: " << (int) size << " id: " << (int)id << std::endl;
      return true;
    }

    static bool on_value(uint8_t type, uint8_t id) {
      std::cout << "FIELD type: " << (int) id << " id: " << (int) id << std::endl;
      return true;
    }

    static bool on_end_marker() {
      std::cout << "OBJECT end" << std::endl;
      return true;
    }
  };

  template<typename handler>
  struct flexbin_reader {
    bool read_header(flexbin::istream& istr) {
      uint32_t size = 0;
      if (!type_traits<uint32_t>::read(istr, size))
        return false;

      uint16_t id = 0;
      if (!type_traits<uint16_t>::read(istr, id))
        return false;

      return handler::on_object_header(size, id);

    }

    bool read_variable(flexbin::istream& istr) {
      uint8_t type = 0;
      if (!type_traits<uint8_t>::read(istr, type))
        return false;

      uint8_t field_id = 0;
      if (!type_traits<uint8_t>::read(istr, field_id))
        return false;

      return handler::on_value(type, field_id);
    }

    bool read_object(flexbin::istream& istr) {
      if (!read_header(istr))
        return false;

      while (read_variable(istr))
        void(0);

      return true;
    }


    bool dump(const char *data, size_t nbytes) {
      flexbin::istream istr(data, nbytes);
      return read_object(istr);
    }
  };

}

/*
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
*/

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

  flexbin_experimental::flexbin_reader<flexbin_experimental::flexbin_read_dumper> dumper;
  dumper.dump(data.data(), data.size());
  return 0; 
}
