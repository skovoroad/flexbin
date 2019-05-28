#pragma once

#include "flexbin_streams.hpp"
#include "flexbin_aux.hpp"

namespace flexbin
{
  ///////////////////
  // Read methods selector for different field types: fundamental, std::string or struct/class
  template <typename T, class Enabler = void>
  struct field_reader {

    static bool read(istream& istr, T& value) {
      istr >> value;
      return true;
    }
  };

  template <>
  struct field_reader<std::string, void> {
    static bool read(istream& istr, std::string& value) {
      return type_traits<std::string>::read(istr, value);
    }
  };

  template <typename T>
  struct field_reader<T, std::enable_if_t<std::is_fundamental<T>::value> > {

    static bool read(istream& istr, T& value) {
      return type_traits<T>::read(istr, value);
    }
  };



  //////////////////////////
  // Write strategies: fixes, optional, required, simplified
  template<typename T>
  inline bool read_fixed(istream& istr, T& value) {
    return field_reader<T>::read(istr, value);
  };


  template<typename T>
  inline bool read_required(istream& istr, uint8_t field_id, T& value) {
    uint8_t type(0), id(0);
    if (!type_traits<uint8_t>::read(istr, type) || !type_traits<uint8_t>::read(istr, id))
      return false;
    if (id != field_id)
      return false;
    return field_reader<T>::read(istr, value);
  };

  template<typename T>
  inline bool read_optional(istream& istr, uint8_t field_id, T& value) {
    uint8_t type(0), id(0);
    if (!type_traits<uint8_t>::read(istr, type) || !type_traits<uint8_t>::read(istr, id))
      return false;
    if (id != field_id) {
      value = type_traits<T>::default_value_;
      return true;
    }
    return field_reader<T>::read(istr, value);
  };

  template<typename T>
  inline bool read_simplified(istream& istr, uint8_t field_id, T& value) {
    uint8_t type(0), id(0);
    if (!type_traits<uint8_t>::read(istr, type) || !type_traits<uint8_t>::read(istr, id))
      return false;
    if (id != field_id) {
      value = type_traits<T>::default_value_;
      return true;
    }
    return field_reader<T>::read(istr, value);
  };

  ///////////////////////////

  template<typename T>
  struct flexbin_reader
  {
    uint8_t field_id = 0;
    bool success_ = true;
    size_t nbytes_remains = 0;

    constexpr static bool required_fields_exists = (has_required_fields<T>::yes != 0);
    constexpr static bool optional_fields_exists = (has_optional_fields<T>::yes != 0);
    constexpr static bool fixed_fields_exists = (has_fixed_fields<T>::yes != 0);
    constexpr static bool simplified_fields_exists = (has_simplified_fields<T>::yes != 0);

#ifdef _MSC_VER 
    // Read fixed fields if exists
    bool read_fixed_fields(istream& istr, T& obj) {
      __if_exists(T::flexbin_serialize_fixed)
      {
        auto field_deserializer_fixed = [this, &istr](auto&&... args) {
          ((success_ = success_ && read_fixed(istr, args)), ...);
        };
        std::apply(field_deserializer_fixed, obj.flexbin_deserialize_fixed());
      }
      return success_;
    }
    // Read required fields if exists
    bool read_required_fields(istream& istr, T& obj) {
      __if_exists(T::flexbin_serialize_required)
      {
        auto field_deserializer_required = [this, &istr](auto&&... args) {
          ((success_ = success_ && read_required(istr, ++field_id, args)), ...);
        };
        std::apply(field_deserializer_required, obj.flexbin_deserialize_required());
      }
      return success_;
    }
    // Read optional fields if exists
    bool read_optional_fields(istream& istr, T& obj) {
      __if_exists(T::flexbin_serialize_optional)
      {
        auto field_deserializer_optional = [this, &istr](auto&&... args) {
          ((success_ = success_ && read_optional(istr, ++field_id, args)), ...);
        };
        std::apply(field_deserializer_optional, obj.flexbin_deserialize_optional());
        }
      return success_;
      }
    // Read simplified fields if exists
    bool read_simplified_fields(istream& istr, T& obj) {
      __if_exists(T::flexbin_serialize_simplified)
      {
        auto field_deserializer_simplified = [this, &istr](auto&&... args) {
          ((success_ = success_ && read_simplified(istr, ++field_id, args)), ...);
        };
        std::apply(field_deserializer_simplified, obj.flexbin_deserialize_simplified());
      }
      return success_;
    }
#else
    // Read fixed fields if exists
    template<typename C = bool>
    typename std::enable_if< fixed_fields_exists , C>::type
    read_required_fields(istream& ostr, T& obj) {
      auto field_deserializer_required = [this, &ostr](auto&&... args) { 
          ( (success_ = success_ && read_required(ostr, ++field_id,  args) ) , ... );
        };
      std::apply(field_deserializer_required, obj.flexbin_deserialize_required());
      return success_;
    }

   // Read fixed fields if not exists
    template<typename C = bool>
    typename std::enable_if< !fixed_fields_exists , C >::type
    read_required_fields( istream& ostr, const T& obj ) {
      return true;
    }
#endif

    bool read_header( istream& istr,  T& obj ) {
      uint32_t size = 0;
      if(!type_traits<uint32_t>::read(istr, size))
        return false;

      uint16_t id = 0;
      if (!type_traits<uint16_t>::read(istr, id))
        return false;

      if (id != T::flexbin_class_id)
        return false;
      return true;

    }  

    bool read_bottom(istream& istr,  T& obj) {
      uint8_t em = 0;
      if (!type_traits<uint8_t>::read(istr, em))
        return false;

      return end_marker == em;
    }
  };

}