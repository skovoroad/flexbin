#pragma once
#include <type_traits>
#include <iostream>
#include <tuple>
#include <string>

namespace flexbin
{
  struct istream : public std::basic_istream<char>
  {
    istream(std::streambuf * buf) : std::basic_istream<char>(buf)
    {
    }

    template<typename T>
    std::basic_istream<char>& operator>> ( T& )
    {
      return *this;
    }
  };

  struct ostream : public std::basic_ostream<char>
  {
    ostream(std::streambuf * buf) : std::basic_ostream<char>(buf)
    {
    }

    template<typename T> std::basic_ostream<char>& operator<< (const T& obj);
  };


  template<typename T>
  struct type_traits
  {
    enum { code_ = 29 /* "object" field id */ };
    inline static size_t write( ostream& ostr, const T& ) { return 0;}
    inline static size_t pack( ostream& ostr, const T&) { return 0;}
  };

  template<>
  struct type_traits<uint64_t>
  {
    enum { default_value_ = 0 };
    enum { code_ = 16 };

    inline static size_t write( ostream& ostr, const uint64_t& val) { 
      ostr.write(reinterpret_cast<const char*>(&val), sizeof(uint64_t));
      return sizeof(uint64_t); 
    }

    inline static size_t pack( ostream& ostr, const uint64_t&) { 
      return 0;
      }

  };

  template<>
  struct type_traits<uint32_t>
  {
    enum { default_value_ = 0 };
    enum { code_ = 8 };

    inline static size_t write( ostream& ostr, const uint32_t& val ) { 
      ostr.write(reinterpret_cast<const char*>(&val), sizeof(uint32_t));
      return sizeof(uint32_t); 
     }

    inline static size_t pack( ostream& ostr, const uint32_t&) { 
      return 0;
      }

  };

  template<>
  struct type_traits<uint8_t>
  {
    enum { default_value_ = 0 };
    enum { code_ = 2 };
    
    inline static size_t write( ostream& ostr, const uint8_t& val ) { 
      ostr.write(reinterpret_cast<const char*>(&val), sizeof(uint8_t));
      return sizeof(uint8_t); 
     }

    inline static size_t pack( ostream& ostr, const uint8_t&) { 
      return 0;
      }

  };

  template<>
  struct type_traits<std::string>
  {
    enum { code_ = 21 };

    inline static size_t write( ostream& ostr, const std::string& ) { 
      return 0; 
    }

    inline static size_t pack( ostream& ostr, const std::string&) { 
      return 0;
    }

  };


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
       return 0;
    }
    static size_t pack( ostream& ostr, uint8_t field_id, const std::string& value) { 
       return write(ostr, field_id, value);
    }
  };

  template <typename T>
  struct field_writer<T, std::enable_if_t<std::is_fundamental<T>::value> > { 
    static size_t write( ostream& ostr, uint8_t field_id, const T& value) {  
      size_t nbytes = type_traits<T>::write(ostr, value);
      return nbytes;
    }
    
    static size_t pack( ostream& ostr, uint8_t field_id, const T& value) {  
      size_t nbytes = type_traits<T>::pack(ostr, value);
      return nbytes;
    }
  };

  template <typename T>
  size_t field_write(ostream& ostr, uint8_t field_id, const T& value) { 
    return field_writer<T>::write(ostr, field_id, value);
  }
  
  template <typename T>
  size_t field_pack(ostream& ostr, uint8_t field_id, const T& value) { 
    return field_writer<T>::pack(ostr, field_id, value);
  }

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

  // serializable class may not have fixed/required/optional/simplified section, 
  // so we need sfinae to detect it
  template<int N> struct creatable_non_zero { creatable_non_zero(int ) {}; char tmp[N]; };

  template<typename T>
  struct has_required_fields {
    template <typename C> static char test( creatable_non_zero<sizeof(&C::flexbin_serialize_required)> ) ;
    template <typename C> static long test(...);
    enum { yes = sizeof(test<T>( 0 )) == sizeof(char) };
  };

  template<typename T>
  struct has_optional_fields {
    template <typename C> static char test( creatable_non_zero<sizeof(&C::flexbin_serialize_optional)> ) ;
    template <typename C> static long test(...);
    enum { yes = sizeof(test<T>( 0 )) == sizeof(char) };
  };

  template<typename T>
  struct has_fixed_fields {
    template <typename C> static char test( creatable_non_zero<sizeof(&C::flexbin_serialize_fixed)> ) ;
    template <typename C> static long test(...);
    enum { yes = sizeof(test<T>( 0 )) == sizeof(char) };
  };

  template<typename T>
  struct has_simplified_fields {
    template <typename C> static char test( creatable_non_zero<sizeof(&C::flexbin_serialize_simplified)> ) ;
    template <typename C> static long test(...);
    enum { yes = sizeof(test<T>( 0 )) == sizeof(char) };
  };


  template<typename T>
  struct flexbin_writer
  {
    uint8_t field_id = 0;

    constexpr static bool required_fields_exists = (has_required_fields<T>::yes != 0);
    constexpr static bool optional_fields_exists = (has_optional_fields<T>::yes != 0);
    constexpr static bool fixed_fields_exists    = (has_fixed_fields<T>::yes != 0);
    constexpr static bool simplified_fields_exists = (has_simplified_fields<T>::yes != 0);

   // Write required fields if exists
    template<typename C = size_t>
    typename std::enable_if< required_fields_exists , C>::type
    write_required_fields(ostream& ostr, const T& obj) {
      auto field_serializer_required = [this, &ostr](auto&&... args) { 
          ( ( write_required(ostr, ++field_id,  args) ) , ... );
        };
      std::apply( field_serializer_required, obj.flexbin_serialize_required());
      return 0;
    }

   // Write required fields if not exists
    template<typename C = size_t>
    typename std::enable_if< !required_fields_exists , C >::type
    write_required_fields( ostream& ostr, const T& obj ) {
      return 0;
    }

   // Write optional fields if exists
    template<typename C = size_t>
    typename std::enable_if< optional_fields_exists , C>::type
    write_optional_fields(ostream& ostr, const T& obj) {
      auto field_serializer_optional = [this, &ostr](auto&&... args) { 
          ( ( write_optional(ostr, ++field_id,  args) ) , ... );
        };
      std::apply( field_serializer_optional, obj.flexbin_serialize_optional());
      return 0;
    }

   // Write optional fields if not exists
    template<typename C = size_t>
    typename std::enable_if< !optional_fields_exists , C >::type
    write_optional_fields( ostream& ostr, const T& obj ) {
      return 0;
    }

   // Write fixed fields if exists
    template<typename C = size_t>
    typename std::enable_if< fixed_fields_exists , C>::type
    write_fixed_fields(ostream& ostr, const T& obj) {
      auto field_serializer_fixed = [this, &ostr](auto&&... args) { 
          ( ( write_fixed(ostr, ++field_id,  args) ) , ... );
        };
      std::apply( field_serializer_fixed, obj.flexbin_serialize_fixed());
      return 0;
    }

   // Write fixed fields if not exists
    template<typename C = size_t>
    typename std::enable_if< !fixed_fields_exists , C >::type
    write_fixed_fields( ostream& ostr, const T& obj ) {
      return 0;
    }

   // Write simplified fields if exists
    template<typename C = size_t>
    typename std::enable_if< simplified_fields_exists , C>::type
    write_simplified_fields(ostream& ostr, const T& obj) {
      auto field_serializer_simplified = [this, &ostr](auto&&... args) { 
          ( ( write_simplified(ostr, ++field_id,  args) ) , ... );
        };
      std::apply( field_serializer_simplified, obj.flexbin_serialize_simplified());
      return 0;
    }

   // Write simplified fields if exists
    template<typename C = size_t>
    typename std::enable_if< !simplified_fields_exists , C >::type
    write_simplified_fields( ostream& ostr, const T& obj ) {
      return 0;
    }
  };


  template<typename T>
  std::basic_ostream<char>& ostream::operator<< (const T& obj)
  {
    {
      flexbin_writer<T> writer;
      writer.write_fixed_fields(*this, obj);
      writer.write_required_fields(*this, obj);
      writer.write_optional_fields(*this, obj);
      writer.write_simplified_fields(*this, obj);
    }
    return *this;
  }
} // namespace flexbin 

#define FLEXBIN_CLASS_ID(id)  enum { flexbin_class_id = id };

#define FLEXBIN_SERIALIZE_FIXED(...) \
  auto flexbin_serialize_fixed() const { return std::forward_as_tuple(__VA_ARGS__); } \

#define FLEXBIN_SERIALIZE_REQUIRED(...) \
  auto flexbin_serialize_required() const { return std::forward_as_tuple(__VA_ARGS__); } \

#define FLEXBIN_SERIALIZE_OPTIONAL(...) \
  auto flexbin_serialize_optional() const { return std::forward_as_tuple(__VA_ARGS__); } 

#define FLEXBIN_SERIALIZE_SIMPLIFIED(...) \
  auto flexbin_serialize_simplified() const { return std::forward_as_tuple(__VA_ARGS__); } 
