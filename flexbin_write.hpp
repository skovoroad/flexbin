#pragma once
#include <assert.h>
#include <stack>
#include "flexbin_streams.hpp"
#include "flexbin_aux.hpp"

namespace flexbin
{
////////////////////
// function to "pack" value of fundamental type: f.e. write "small" uint64_t value as uint8_t value
// pack_if_equal - write value, if "small" equal to "big" value
// pack_value - iterate through "smaller" types to pack "bigger"

  template<typename T, typename candidateT>
  void pack_if_equal(ostream& ostr,
    const T& value,
    size_t & nbytes,
    uint8_t field_id,
    const candidateT& candidate
  ) {
    if (nbytes > 0) // already packed with smaller type
      return;
    if (value == candidate)
    {
      uint8_t code = type_traits<candidateT>::code_;
      nbytes = sizeof(candidateT) + 2;
      ostr.write(reinterpret_cast<const char*>(&code), 1);
      ostr.write(reinterpret_cast<const char*>(&field_id), 1);
      ostr.write(reinterpret_cast<const char*>(&candidate), sizeof(candidateT));
    }
  }

  template<typename T>
  size_t pack_value(ostream& ostr, const T& value, uint8_t field_id) {

    auto pack_versions = type_traits<T>::candidates(value);
    size_t packed_nbytes = 0;

    auto pack_candidates = [&](auto&&... args) {
      ((pack_if_equal(ostr, value, packed_nbytes, field_id, args)), ...);
    };

    std::apply(pack_candidates, pack_versions);
    return packed_nbytes;
  }

///////////////////
// Write methods selector for different field types: fundamental, std::string or struct/class
  template <typename T, class Enabler = void>
  struct field_writer { 

    static size_t write( ostream& ostr, const T& value) { 
      ostr << value;
      return sizeof(T);  // FUUUUU
    }

    static size_t pack( ostream& ostr, uint8_t field_id, const T& value) { 
      uint8_t code = type_traits<T>::code_;
      ostr.write(reinterpret_cast<const char*>(&code), 1);
      ostr.write(reinterpret_cast<const char*>(&field_id), 1);
      return write(ostr, value);
    }
  };

  template <>
  struct field_writer<std::string, void> { 

    static size_t write( ostream& ostr, const std::string& value) {
      return type_traits<std::string>::write(ostr, value);
    }
    
    static size_t pack( ostream& ostr, uint8_t field_id, const std::string& value) { 
      uint8_t code = type_traits<std::string>::code_;
      ostr.write(reinterpret_cast<const char*>(&code), 1);
      ostr.write(reinterpret_cast<const char*>(&field_id), 1);
      return type_traits<std::string>::write(ostr, value) + 2;
    }
  };

  template <typename T>
  struct field_writer<T, std::enable_if_t<std::is_fundamental<T>::value> > { 

    static size_t write( ostream& ostr, const T& value) {  
      return type_traits<T>::write(ostr, value);
    }

    static size_t pack( ostream& ostr, uint8_t field_id, const T& value) {  
      //return type_traits<T>::pack(ostr, value);
      return pack_value(ostr, value, field_id);
    }
  };

  template <typename T>
  struct field_writer<T, std::enable_if_t<std::is_enum<T>::value> > { 

    static size_t write( ostream& ostr, const T& value) {  
      return type_traits<T>::write(ostr, value);
    }

    static size_t pack( ostream& ostr, uint8_t field_id, const T& value) {  
      //return type_traits<T>::pack(ostr, value);
      return pack_value(ostr, value, field_id);
    }
  };


  template <typename T>
  struct field_writer< std::vector<T> > {
    static size_t write(ostream& ostr, const std::vector<T> & value) {
      size_t size = value.size();
      ostr.write(reinterpret_cast<const char*>(&size), sizeof(size));
      
      for (const auto & v : value)
        field_writer<T>::write(ostr, v);
      return size;
    }

    static size_t pack(ostream& ostr, uint8_t field_id, const std::vector<T>& value) {
      uint8_t code = type_traits<std::vector<T>>::code_;
      ostr.write(reinterpret_cast<const char*>(&code), 1);
      ostr.write(reinterpret_cast<const char*>(&field_id), 1);
      return write(ostr, value);
    }
  };

//////////////////////////
// Write strategies: fixes, optional, required, simplified
  template<typename T>    
  inline size_t write_fixed( ostream& ostr, const T& value) {
    return field_writer<T>::write(ostr, value);
  };

  template<typename T>    
  inline size_t write_required( ostream& ostr, uint8_t field_id, const T& value) {
    return field_writer<T>::pack(ostr, field_id, value);
  };

  template<typename T>    
  inline size_t write_optional( ostream& ostr,  uint8_t field_id, const T& value) {
    if(type_traits<T> ::default_value_ == value) 
     return 0;
    return  field_writer<T>::pack(ostr, field_id, value);
  };

  template<typename T>    
  inline size_t write_simplified( ostream& ostr,  uint8_t field_id, const T& value) {
    uint8_t code = type_traits<T>::code_;
    ostr.write(reinterpret_cast<const char*>(&code), 1);
    ostr.write(reinterpret_cast<const char*>(&field_id), 1);
    return type_traits<T>::write(ostr, value) + 2;
  };

///////////////////////////
//

  template<typename T>
  struct flexbin_writer
  {
    uint8_t field_id = 0;
    std::stack<std::streampos> object_size_pos;
    bool success_ = true;

    constexpr static bool required_fields_exists = (has_required_fields<T>::yes != 0);
    constexpr static bool optional_fields_exists = (has_optional_fields<T>::yes != 0);
    constexpr static bool fixed_fields_exists    = (has_fixed_fields<T>::yes != 0);
    constexpr static bool simplified_fields_exists = (has_simplified_fields<T>::yes != 0);

    // object header
    bool write_header(ostream& ostr, const T& obj)
    {
      uint32_t object_size = 0; 
      auto size_pos = ostr.tellp();

      if (size_pos < 0)
        throw(std::ios_base::failure("inapproriate type of buffer doesn't support rewind "));
      object_size_pos.push(size_pos);

      ostr.write(reinterpret_cast<const char*>(&object_size), 4);

      uint16_t class_id = T::flexbin_class_id;
      ostr.write(reinterpret_cast<const char*>(&class_id), 2);
      return true;
    }

    bool write_bottom(ostream& ostr, const T& obj)
    {
      ostr.write(reinterpret_cast<const char*>(&end_marker), 1);

      auto object_end_pos = ostr.tellp();
      assert(!object_size_pos.empty());
      auto size_pos = object_size_pos.top();
      if (object_end_pos > size_pos) { // real writing, not just counting object size 

        auto object_size = static_cast<uint32_t>(object_end_pos - size_pos);

        ostr.seekp(size_pos);
        ostr.write(reinterpret_cast<const char*>(&object_size), 4);
        ostr.seekp(object_end_pos);
        object_size_pos.pop();
      }
      return true;
    }

#ifdef _MSC_VER 
    // fuck this shit! MSVC cannot to proper template arguments deducing
    bool write_required_fields(ostream& ostr, const T& obj) {
      __if_exists(T::flexbin_serialize_required)
      {
        auto field_serializer_required = [this, &ostr](auto&&... args) {
          ((success_ = success_ && ( 0 != write_required(ostr, ++field_id, args))), ...);
        };
        std::apply(field_serializer_required, obj.flexbin_serialize_required());
      }
      return success_ ;
    }

    bool write_optional_fields(ostream& ostr, const T& obj) {
      __if_exists(T::flexbin_serialize_optional)
      {
        auto field_serializer_optional = [this, &ostr](auto&&... args) {
          ((success_  = success_ && (0 != write_optional(ostr, ++field_id, args))), ...);
        };
        std::apply(field_serializer_optional, obj.flexbin_serialize_optional());
      }
      return success_;
    }

    // Write fixed fields if exists
    bool write_fixed_fields(ostream& ostr, const T& obj) {
      __if_exists(T::flexbin_serialize_fixed)
      {
        auto field_serializer_fixed = [this, &ostr](auto&&... args) {
          ((success_ = success_ && (0 != write_fixed(ostr, args))), ...);
        };
        std::apply(field_serializer_fixed, obj.flexbin_serialize_fixed());
      }
      return success_;
    }

    bool write_simplified_fields(ostream& ostr, const T& obj) {
      __if_exists(T::flexbin_serialize_simplified)
      {
        auto field_serializer_simplified = [this, &ostr](auto&&... args) {
          ((success_  = success_ && (0 != write_simplified(ostr, ++field_id, args))), ...);
        };
        std::apply(field_serializer_simplified, obj.flexbin_serialize_simplified());
      }
      return success_;
    }
#else
   // Write required fields if exists
    template<typename C = bool >
    typename std::enable_if< required_fields_exists , C>::type
    write_required_fields(ostream& ostr, const T& obj) {
      auto field_serializer_required = [this, &ostr](auto&&... args) { 
          ( (success_  = success_ && (0 != write_required(ostr, ++field_id,  args))) , ... );
        };
      std::apply( field_serializer_required, obj.flexbin_serialize_required());
      return success_;
    }

   // Write required fields if not exists
    template<typename C = bool>
    typename std::enable_if< !required_fields_exists , C >::type
    write_required_fields( ostream& ostr, const T& obj ) {
      return success_;
    }

   // Write optional fields if exists
    template<typename C = bool>
    typename std::enable_if< optional_fields_exists , C>::type
    write_optional_fields(ostream& ostr, const T& obj) {
      auto field_serializer_optional = [this, &ostr](auto&&... args) { 
          ( (success_  = success_ && (0 != write_optional(ostr, ++field_id,  args))) , ... );
        };
      std::apply( field_serializer_optional, obj.flexbin_serialize_optional());
      return success_;
    }

   // Write optional fields if not exists
    template<typename C = bool>
    typename std::enable_if< !optional_fields_exists , C >::type
    write_optional_fields( ostream& ostr, const T& obj ) {
      return true;
    }

   // Write fixed fields if exists
    template<typename C = bool>
    typename std::enable_if< fixed_fields_exists , C>::type
    write_fixed_fields(ostream& ostr, const T& obj) {
      auto field_serializer_fixed = [this, &ostr](auto&&... args) { 
          ( (success_ = success_ && (0!=write_fixed(ostr, args))) , ... );
        };
      std::apply( field_serializer_fixed, obj.flexbin_serialize_fixed());
      return success_;
    }

   // Write fixed fields if not exists
    template<typename C = bool>
    typename std::enable_if< !fixed_fields_exists , C >::type
    write_fixed_fields( ostream& ostr, const T& obj ) {
      return success_;
    }

   // Write simplified fields if exists
    template<typename C = bool>
    typename std::enable_if< simplified_fields_exists , C>::type
    write_simplified_fields(ostream& ostr, const T& obj) {
      auto field_serializer_simplified = [this, &ostr](auto&&... args) { 
          ( (success_  = success_ && (0 != write_simplified(ostr, ++field_id,  args))) , ... );
        };
      std::apply( field_serializer_simplified, obj.flexbin_serialize_simplified());
      return success_;
    }

   // Write simplified fields if exists
    template<typename C = bool>
    typename std::enable_if< !simplified_fields_exists , C >::type
    write_simplified_fields( ostream& ostr, const T& obj ) {
      return true;
    }
#endif
  };


} //namespace flexbin