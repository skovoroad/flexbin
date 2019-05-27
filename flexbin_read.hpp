#pragma once

#include "flexbin_streams.hpp"
#include "flexbin_aux.hpp"

namespace flexbin
{
  template<typename T>
  struct flexbin_reader
  {
    uint8_t field_id = 0;

    constexpr static bool required_fields_exists = (has_required_fields<T>::yes != 0);
    constexpr static bool optional_fields_exists = (has_optional_fields<T>::yes != 0);
    constexpr static bool fixed_fields_exists = (has_fixed_fields<T>::yes != 0);
    constexpr static bool simplified_fields_exists = (has_simplified_fields<T>::yes != 0);

   // Read fixed fields if exists
    template<typename C = size_t>
    typename std::enable_if< fixed_fields_exists , C>::type
    read_required_fields(istream& ostr, const T& obj) {
/*      auto field_serializer_fixed = [this, &ostr](auto&&... args) { 
          ( ( write_fixed(ostr, ++field_id,  args) ) , ... );
        };
      std::apply( field_serializer_fixed, obj.flexbin_serialize_fixed());*/
      return 0;
    }

   // Write fixed fields if not exists
    template<typename C = size_t>
    typename std::enable_if< !fixed_fields_exists , C >::type
    read_required_fields( istream& ostr, const T& obj ) {
      return 0;
    }

    bool read( istream& ostr, const T& obj ) {
      size_t nbytes_remain(0);
      ostr.read(reinterpret_cast<char *>(nbytes_remain), sizeof(nbytes_remain));
      if(ostr.fail() || ostr.eof())
        return 0;
    }  

  };

}