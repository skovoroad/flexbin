#pragma once
#include <type_traits>
#include <iostream>
#include <tuple>
#include <string>

#include "flexbin.hpp"
#include "flexbin_type_traits.hpp"
#include "flexbin_write.hpp"
#include "flexbin_read.hpp"
#include "flexbin_streams.hpp"

#include "flexbin_debug.hpp"

namespace flexbin
{

  template<typename T>
  std::basic_ostream<char>& ostream::operator<< (const T& obj)
  {
    int code = T ::flexbin_class_id;
    flexbin_writer<T> writer;
    if ( !(writer.write_header(*this, obj) &&
      writer.write_fixed_fields(*this, obj) && // fixed fields goes first
      writer.write_required_fields(*this, obj) &&
      writer.write_optional_fields(*this, obj) &&
      writer.write_simplified_fields(*this, obj) &&
      writer.write_bottom(*this, obj)) )
    {
      failed_ = true;
    }
    return *this;
  }

  template<typename T>
  std::basic_istream<char>& istream::operator>> (T& obj)
  {
    flexbin_reader<T> reader;

    if (! (reader.read_header(*this, obj) &&
      reader.read_fixed_fields(*this, obj) &&
      reader.read_required_fields(*this, obj) &&
      reader.read_optional_fields(*this, obj) &&
      reader.read_simplified_fields(*this, obj) &&
      reader.read_bottom(*this, obj)) )
    {
      failed_ = true;
    }

    return *this;
  }

  inline bool class_id(const void *data, size_t nbytes,  uint16_t & retval)
  {
    istream istr(reinterpret_cast<const char *>(data), nbytes);

    uint32_t size = 0;
    if (!type_traits<uint32_t>::read(istr, size))
      return false;

    if (!type_traits<uint16_t>::read(istr, retval))
      return false;

    return true;
  }

  inline bool message_complete(const void *data, size_t nbytes, uint32_t & size)
  {
    istream istr(reinterpret_cast<const char *>(data), nbytes);

    size = 0;
    if (!type_traits<uint32_t>::read(istr, size)) {
      size = sizeof(size); // if we don't know size of message, we need just read size itself
      return false;
    }

    return size <= nbytes;
  }


  template <typename T>
  inline std::size_t class_object_size(const T& obj) {
    ostream ostr(reinterpret_cast<char *>(0), static_cast<size_t>(0) , true);
    ostr << obj;
    auto retval = ostr.written_count();
    FLEXBIN_DEBUG_LOG("class_object_size " << retval)
    return retval;
    
  }


} // namespace flexbin 


