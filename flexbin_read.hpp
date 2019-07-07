#pragma once

#include <memory>
#include "flexbin_streams.hpp"
#include "flexbin_aux.hpp"

namespace flexbin
{
  template<typename T, typename candidateT>
  inline void unpack_if_type_matches(istream& istr,
    size_t & nbytes,
    uint8_t type_code,
    T& value,
    const candidateT& phony
  ) {
    if (nbytes > 0) // already unpacked 
      return;
    if (type_traits<candidateT>::code_ != type_code)
      return;

    candidateT candidate_value;
    type_traits<candidateT>::read(istr, candidate_value);
    nbytes = sizeof(candidateT);
    value = candidate_value;
  }

  template<typename T>
  size_t unpack_value(istream& istr, uint8_t type, T& value) {

    auto pack_versions = type_traits<T>::candidates(value);
    size_t unpacked_nbytes = 0;

    auto pack_candidates = [&](auto&&... args) {
      ((unpack_if_type_matches(istr, unpacked_nbytes, type, value,  args)), ...);
    };

    std::apply(pack_candidates, pack_versions);
    return unpacked_nbytes;
  }

  ///////////////////
  // Read methods selector for different field types: fundamental, std::string or struct/class
  template <typename T, class Enabler = void>
  struct field_reader {

    static bool read(istream& istr, T& value) {
      istr >> value;
      return true;
    }

    static bool unpack_value(istream& istr, uint8_t type, T& value) {
      istr >> value;
      bool retval = istr.good();
      return istr.good(); 
    }

  };

  template <typename T>
  struct field_reader<T, std::enable_if_t<std::is_base_of<std::string, T>::value> > {    
    static bool read(istream& istr, std::string& value) {
      return type_traits<std::string>::read(istr, value);
    }

    static bool unpack_value(istream& istr, uint8_t type, std::string& value) {
      flexbin::unpack_value(istr, type, value);
      return istr.good(); 
    }
  };

  template <typename T>
  struct field_reader<T, std::enable_if_t<std::is_fundamental<T>::value> > {

    static bool read(istream& istr, T& value) {
      return type_traits<T>::read(istr, value);
    }

    static bool unpack_value(istream& istr, uint8_t type, T& value) {
      flexbin::unpack_value(istr, type, value);
      return istr.good(); 
    }
  };

  template <typename T>
  struct field_reader<T, std::enable_if_t<std::is_enum<T>::value> > {

    static bool read(istream& istr, T& value) {
      return type_traits<T>::read(istr, value);
    }

    static bool unpack_value(istream& istr, uint8_t type, T& value) {
      flexbin::unpack_value(istr, type, value);
      return istr.good(); 
    }

  };


  template <typename T>
  struct field_reader< std::vector<T> > {

    static bool read(istream& istr, std::vector < T>& value) {
    }

    static bool unpack_value(istream& istr, uint8_t type, std::vector < T>& value) {
      size_t len = 0;
      if (!type_traits<size_t>::read(istr, len))
        return false;
      auto elem_type = type_traits<T>::code_;
      while (len-- > 0) {
        T val;
        if (!field_reader<T>::unpack_value(istr, elem_type, val))
          return false;
        value.push_back(val);
      }
      return istr.good();
    }

  };

  template <typename T>
  struct field_reader< std::shared_ptr<T> > {

    static bool read(istream& istr, std::shared_ptr<T>& value) {
      return field_reader<T>::read(istr, *value);
    }

    static bool unpack_value(istream& istr, uint8_t type, std::shared_ptr<T>& value) {
      if(!value)
        value = std::make_shared<T>();
      return field_reader<T>::unpack_value(istr, type, *value);
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
    return field_reader<T>::unpack_value(istr, type, value);
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
//    return field_reader<T>::read(istr, value); // NO! read real type, not T
    return field_reader<T>::unpack_value(istr, type, value);
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
  //  return field_reader<T>::read(istr, value); // NO! read real type, not T
    return field_reader<T>::unpack_value(istr, type, value);
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
    read_fixed_fields(istream& ostr, T& obj) {
      auto field_deserializer_fixed = [this, &ostr](auto&&... args) { 
          ( (success_ = success_ && read_fixed(ostr,  args) ) , ... );
        };
      std::apply(field_deserializer_fixed, obj.flexbin_deserialize_fixed());
      return success_;
    }

   // Read fixed fields if not exists
    template<typename C = bool>
    typename std::enable_if< !fixed_fields_exists , C >::type
    read_fixed_fields( istream& ostr, T& obj ) {
      return true;
    }

    // Read required fields if exists
    template<typename C = bool>
    typename std::enable_if< required_fields_exists , C>::type
    read_required_fields(istream& ostr, T& obj) {
      auto field_deserializer_required = [this, &ostr](auto&&... args) { 
          ( (success_ = success_ && read_required(ostr, ++field_id,  args) ) , ... );
        };
      std::apply(field_deserializer_required, obj.flexbin_deserialize_required());
      return success_;
    }

   // Read required fields if not exists
    template<typename C = bool>
    typename std::enable_if< !required_fields_exists , C >::type
    read_required_fields( istream& ostr, T& obj ) {
      return true;
    }

    // Read optional fields if exists
    template<typename C = bool>
    typename std::enable_if< optional_fields_exists , C>::type
    read_optional_fields(istream& ostr, T& obj) {
      auto field_deserializer_optional = [this, &ostr](auto&&... args) { 
          ( (success_ = success_ && read_optional(ostr, ++field_id,  args) ) , ... );
        };
      std::apply(field_deserializer_optional, obj.flexbin_deserialize_optional());
      return success_;
    }

   // Read optional fields if not exists
    template<typename C = bool>
    typename std::enable_if< !optional_fields_exists , C >::type
    read_optional_fields( istream& ostr, T& obj ) {
      return true;
    }

    // Read simplified fields if exists
    template<typename C = bool>
    typename std::enable_if< simplified_fields_exists , C>::type
    read_simplified_fields(istream& ostr, T& obj) {
      auto field_deserializer_simplified = [this, &ostr](auto&&... args) { 
          ( (success_ = success_ && read_simplified(ostr, ++field_id,  args) ) , ... );
        };
      std::apply(field_deserializer_simplified, obj.flexbin_deserialize_simplified());
      return success_;
    }

   // Read simplified fields if not exists
    template<typename C = bool>
    typename std::enable_if< !simplified_fields_exists , C >::type
    read_simplified_fields( istream& ostr, T& obj ) {
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