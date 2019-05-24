#pragma once
#include "flexbin_streams.hpp"

namespace flexbin
{
////////////////////
  template<typename T, typename candidateT>
  void pack_if_equal(std::basic_ostream<char>& ostr,
    const T& value,
    size_t & already,
    uint8_t field_id,
    const candidateT& candidate
    ) {
    if (already > 0)
      return;
    if (value == candidate)
    {
      // pack candidate with its own type
      already = 1; // nbytes!!!
    }
  }

  template<typename T>
  size_t pack_value(std::basic_ostream<char>& ostr, const T& value, uint8_t field_id) {
    auto pack_versions = type_traits<T>::candidates(value);
    size_t packed_nbytes = 0;

    auto pack_candidates = [&](auto&&... args) {
      ((pack_if_equal(ostr, value, packed_nbytes, field_id, args)), ...);
    };

    std::apply(pack_candidates, pack_versions);
    return packed_nbytes;
  }
///////////////////


  template <typename T, class Enabler = void>
  struct field_writer { 
    static size_t write( ostream& ostr, uint8_t field_id, const T& value) { 
      // object header
      uint32_t object_size = 0; // TODO
      ostr.write(reinterpret_cast<const char*>(&object_size), 4);

      uint16_t class_id =  T::flexbin_class_id;
      ostr.write(reinterpret_cast<const char*>(&class_id), 2);

      // write fields
      ostr << value;

      const uint8_t end_marker = 255;
      ostr.write(reinterpret_cast<const char*>(&end_marker), 1);
      return 0; 
    }

    static size_t pack( ostream& ostr, uint8_t field_id, const T& value) { 
       return write(ostr, field_id, value);
    }
  };

  template <>
  struct field_writer<std::string, void> { 
    static size_t write( ostream& ostr, uint8_t field_id, const std::string& value) { 
       return type_traits<std::string>::write(ostr, value);
    }
    
    static size_t pack( ostream& ostr, uint8_t field_id, const std::string& value) { 
       return write(ostr, field_id, value);
    }
  };

  template <typename T>
  struct field_writer<T, std::enable_if_t<std::is_fundamental<T>::value> > { 
    static size_t write( ostream& ostr, uint8_t field_id, const T& value) {  
      return type_traits<T>::write(ostr, value);
    }

    static size_t pack( ostream& ostr, uint8_t field_id, const T& value) {  
      //return type_traits<T>::pack(ostr, value);
      return pack_value(ostr, value, field_id);
    }
  };
//////////////////////////
  template <typename T>
  size_t field_write(ostream& ostr, uint8_t field_id, const T& value) { 
    return field_writer<T>::write(ostr, field_id, value);
  }
  
  
  template <typename T>
  size_t field_pack(ostream& ostr, uint8_t field_id, const T& value) { 
    return field_writer<T>::pack(ostr, field_id, value);
  }
//////////////////////////

  template<typename T>    
  inline size_t write_fixed( ostream& ostr, uint8_t field_id,  const T& value) {
    return field_write(ostr, field_id, value);
  };

  template<typename T>    
  inline size_t write_required( ostream& ostr, uint8_t field_id, const T& value) {
    uint8_t code = type_traits<T>::code_;
    ostr.write(reinterpret_cast<const char*>(&code), 1);
    ostr.write(reinterpret_cast<const char*>(&field_id), 1);
    return field_pack(ostr, field_id, value);
  };

  template<typename T>    
  inline size_t write_optional( ostream& ostr,  uint8_t field_id, const T& value) {
    if(type_traits<T> ::default_value_ == value) {
     return 0;
    }
    uint8_t code = type_traits<T>::code_;
    ostr.write(reinterpret_cast<const char*>(&code), 1);
    ostr.write(reinterpret_cast<const char*>(&field_id), 1);
    return field_pack(ostr, field_id, value);
  };

  template<typename T>    
  inline size_t write_simplified( ostream& ostr,  uint8_t field_id, const T& value) {
    return type_traits<T>::write(ostr, value);
  };
} //namespace flexbin