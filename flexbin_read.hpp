#pragma once

#include "flexbin_streams.hpp"
#include "flexbin_aux.hpp"

namespace flexbin
{
  ///////////////////
  // Read methods selector for different field types: fundamental, std::string or struct/class
  template <typename T, class Enabler = void>
  struct field_reader {

    static bool read(istream& istr, uint8_t field_id, const T& value) {
      //istr >> value;
      ???
      return true;
    }
  };

  template <>
  struct field_reader<std::string, void> {
    static bool read(istream& istr, uint8_t field_id, const std::string& value) {
      //return type_traits<std::string>::write(ostr, value);
      return false;
    }
  };

  template <typename T>
  struct field_reader<T, std::enable_if_t<std::is_fundamental<T>::value> > {

    static bool read(istream& istr, uint8_t field_id, const T& value) {
      //return type_traits<T>::write(ostr, value);
      return false;
    }
  };



  //////////////////////////
  // Write strategies: fixes, optional, required, simplified
  template<typename T>
  inline bool read_required(istream& istr, uint8_t field_id, const T& value) {
    return field_reader<T>::read(istr, field_id, value);
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

    bool read_header( istream& ostr, const T& obj ) {
      return false;
    }  

    bool read_bottom(istream& ostr, const T& obj) {
      return false;
    }
  };

}