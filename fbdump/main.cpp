#include <iomanip>
#include <fstream>
#include <iostream>
#include <vector>

#define FLEXBIN_DEBUG_LOG_ON
#include "flexbin.hpp"

namespace flexbin {
/*  namespace type_code {
    template<typename T> uint8_t get() { return 29; }
    template<typename T> uint8_t get<std::vector<T>>() { return 25; }
    template<typename T> uint8_t get<std::unordered_set<T>>() { return 25; }
    template<typename T> uint8_t get<std::basic_string<T>>() { return 21; }
    template<> uint8_t get<uint64_t>() { return type_traits<uint64_t>::name(); }
    template<> uint8_t get<uint32_t>() { return 8; }
    template<> uint8_t get<uint16_t>() { return 4; }
    template<> uint8_t get<uint8_t>() { return 2; }
    template<> uint8_t get<int64_t>() { return 16; }
    template<> uint8_t get<int32_t>() { return 8; }
    template<> uint8_t get<int16_t>() { return 4; }
    template<> uint8_t get<int8_t>() { return 2; }
    template<> uint8_t get<bool>() { return 1; }
  }
*/
    const char * type_name (uint8_t code) {
      switch(code) {
        case 29: return "object";
        case flexbin::type_traits<uint64_t>::code_: return "uint64";
        case flexbin::type_traits<uint32_t>::code_: return "uint32";
        case flexbin::type_traits<uint16_t>::code_: return "uint16";
        case flexbin::type_traits<uint8_t>::code_:  return "uint8";
        case flexbin::type_traits<int64_t>::code_:  return "int64";
        case flexbin::type_traits<int32_t>::code_:  return "int32";
        case flexbin::type_traits<int16_t>::code_:  return "int16";
        case flexbin::type_traits<int8_t>::code_ :  return "int8";
        case flexbin::type_traits<bool>::code_ :    return "bool";
        case flexbin::type_traits<std::vector<int>>::code_:          return "vector";
        case flexbin::type_traits<std::basic_string<char>>::code_:   return "string";
        default: return "unknown";
      }
      return "unknown";
    }


  struct flexbin_read_dumper {
    static bool on_object_header(uint32_t size, uint16_t id = 0) {
      std::cout << "OBJECT size: " << (int) size << " id: " << (int)id << std::endl;
      return true;
    }

    static bool on_value(uint8_t type, uint8_t id) {
      std::cout << "FIELD type: " << flexbin::type_name(id) << "(" << (int)id << ")" << " id: " << (int) id << std::endl;
      return true;
    }

    static bool on_end_marker() {
      std::cout << "OBJECT end" << std::endl;
      return true;
    }
  };

  template<typename handler>
  struct flexbin_buffer_explorer {
    bool read_header(flexbin::istream& istr) {
      uint32_t size = 0;
      if (!flexbin::type_traits<uint32_t>::read(istr, size))
        return false;

      uint16_t id = 0;
      if (!flexbin::type_traits<uint16_t>::read(istr, id))
        return false;

      return handler::on_object_header(size, id);
    }

    template<typename T>
    bool read_value(flexbin::istream& istr) {
      T tmp;
      flexbin::type_traits<T>::read(istr, tmp);
      return true;
    }    

    bool read_vector(flexbin::istream& istr) {
      size_t size = 0;
      if (!flexbin::type_traits<size_t>::read(istr, size))
        return false;

      // we don't know element type


      return false;
    }

    bool read_string(flexbin::istream& istr) {
      return false;
    }

    bool handle_value(uint8_t type, flexbin::istream& istr) {
      switch (type){
        case 29:  
          return read_object(istr);
        case 200:  
          return read_value<int>(istr);
        case flexbin::type_traits<std::vector<int>>::code_:        
          return read_vector(istr);
        case flexbin::type_traits<std::basic_string<char>>::code_:  
          return read_string(istr);
        case flexbin::type_traits<bool>::code_:
          return read_value<bool>(istr);
        case flexbin::type_traits<uint64_t>::code_:
          return read_value<uint64_t>(istr);
        case flexbin::type_traits<uint32_t>::code_:
          return read_value<uint32_t>(istr);
        case flexbin::type_traits<uint16_t>::code_:
          return read_value<uint16_t>(istr);
        case flexbin::type_traits<uint8_t>::code_:
          return read_value<uint8_t>(istr);
        case flexbin::type_traits<int64_t>::code_:
          return read_value<int64_t>(istr);
        case flexbin::type_traits<int32_t>::code_:
          return read_value<int32_t>(istr);
        case flexbin::type_traits<int16_t>::code_:
          return read_value<int16_t>(istr);
        case flexbin::type_traits<int8_t>::code_:
          return read_value<int8_t>(istr);
        default:
          return false;
      };
      return false;
    }

    bool read_variable(flexbin::istream& istr) {
      uint8_t type = 0;
      if (!flexbin::type_traits<uint8_t>::read(istr, type))
        return false;

      if(type == flexbin::end_marker) {
        handler::on_end_marker();        
        return false;
      }

      uint8_t field_id = 0;
      if (!flexbin::type_traits<uint8_t>::read(istr, field_id))
        return false;

      handler::on_value(type, field_id);
      return handle_value(type, istr);
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

} // namespace flexbin

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

  flexbin::flexbin_buffer_explorer<flexbin::flexbin_read_dumper> dumper;
  dumper.dump(data.data(), data.size());
  return 0; 
}
