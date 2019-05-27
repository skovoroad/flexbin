#pragma once

#include "flexbin_streams.hpp"
#include "flexbin_aux.hpp"

namespace flexbin
{
  //////////////////////////
  // Write strategies: fixes, optional, required, simplified
  template<typename T>
  inline bool read_required(ostream& ostr, uint8_t field_id, const T& value) {
    //return field_writer<T>::pack(ostr, field_id, value);
    return false;
  };

  ///////////////////////////

  template<typename T>
  struct flexbin_reader
  {
    uint8_t field_id = 0;
    bool success_ = true;
    constexpr static bool required_fields_exists = (has_required_fields<T>::yes != 0);
    constexpr static bool optional_fields_exists = (has_optional_fields<T>::yes != 0);
    constexpr static bool fixed_fields_exists = (has_fixed_fields<T>::yes != 0);
    constexpr static bool simplified_fields_exists = (has_simplified_fields<T>::yes != 0);

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

   // Write fixed fields if not exists
    template<typename C = bool>
    typename std::enable_if< !fixed_fields_exists , C >::type
    read_required_fields( istream& ostr, const T& obj ) {
      return true;
    }

    bool read_object_header( istream& ostr, const T& obj ) {
/*      size_t nbytes_remain(0);
      ostr.read(reinterpret_cast<char *>(nbytes_remain), sizeof(nbytes_remain));
      if(ostr.fail() || ostr.eof())*/
      return false;
    }  

    bool read_object_endmarker(istream& ostr, const T& obj) {
      return false;
    }
  };

}