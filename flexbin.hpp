#pragma once
#include <type_traits>
#include <iostream>
#include <tuple>
#include <string>

#include "flexbin_type_traits.hpp"
#include "flexbin_write.hpp"
#include "flexbin_streams.hpp"

#define FLEXBIN_CLASS_ID(id)  enum { flexbin_class_id = id };

#define FLEXBIN_SERIALIZE_FIXED(...) \
  auto flexbin_serialize_fixed() const { return std::forward_as_tuple(__VA_ARGS__); } \

#define FLEXBIN_SERIALIZE_REQUIRED(...) \
  auto flexbin_serialize_required() const { return std::forward_as_tuple(__VA_ARGS__); } \

#define FLEXBIN_SERIALIZE_OPTIONAL(...) \
  auto flexbin_serialize_optional() const { return std::forward_as_tuple(__VA_ARGS__); } 

#define FLEXBIN_SERIALIZE_SIMPLIFIED(...) \
  auto flexbin_serialize_simplified() const { return std::forward_as_tuple(__VA_ARGS__); } 

namespace flexbin
{

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

#ifdef _MSC_VER 
    // fuck this shit! MSVC cannot to proper template arguments deducing
    size_t write_required_fields(ostream& ostr, const T& obj) {
      __if_exists(T::flexbin_serialize_required)
      {
        auto field_serializer_required = [this, &ostr](auto&&... args) {
          ((write_required(ostr, ++field_id, args)), ...);
        };
        std::apply(field_serializer_required, obj.flexbin_serialize_required());
      }
      return 0;
    }

    size_t write_optional_fields(ostream& ostr, const T& obj) {
      __if_exists(T::flexbin_serialize_optional)
      {
        auto field_serializer_optional = [this, &ostr](auto&&... args) {
          ((write_optional(ostr, ++field_id, args)), ...);
        };
        std::apply(field_serializer_optional, obj.flexbin_serialize_optional());
      }
      return 0;
    }

    // Write fixed fields if exists
    size_t write_fixed_fields(ostream& ostr, const T& obj) {
      __if_exists(T::flexbin_serialize_fixed)
      {
        auto field_serializer_fixed = [this, &ostr](auto&&... args) {
          ((write_fixed(ostr, ++field_id, args)), ...);
        };
        std::apply(field_serializer_fixed, obj.flexbin_serialize_fixed());
      }
      return 0;
    }

    size_t write_simplified_fields(ostream& ostr, const T& obj) {
      __if_exists(T::flexbin_serialize_simplified)
      {
        auto field_serializer_simplified = [this, &ostr](auto&&... args) {
          ((write_simplified(ostr, ++field_id, args)), ...);
        };
        std::apply(field_serializer_simplified, obj.flexbin_serialize_simplified());
      }
      return 0;
    }
#else
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
#endif
  };

  template<typename T>
  struct flexbin_reader
  {
    uint8_t field_id = 0;

    constexpr static bool required_fields_exists = (has_required_fields<T>::yes != 0);
    constexpr static bool optional_fields_exists = (has_optional_fields<T>::yes != 0);
    constexpr static bool fixed_fields_exists = (has_fixed_fields<T>::yes != 0);
    constexpr static bool simplified_fields_exists = (has_simplified_fields<T>::yes != 0);
  };


  template<typename T>
  std::basic_ostream<char>& ostream::operator<< (const T& obj)
  {
    flexbin_writer<T> writer;
    
    writer.write_fixed_fields(*this, obj); // fixed fields goes first

    writer.write_required_fields(*this, obj);
    writer.write_optional_fields(*this, obj);
    writer.write_simplified_fields(*this, obj);

    return *this;
  }

  template<typename T>
  std::basic_istream<char>& istream::operator>> (T& obj)
  {
    return *this;
  }

} // namespace flexbin 

