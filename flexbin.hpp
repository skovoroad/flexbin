#pragma once
#include <type_traits>
#include <iostream>
#include <tuple>
#include <string>

#include "flexbin_type_traits.hpp"
#include "flexbin_write.hpp"
#include "flexbin_read.hpp"
#include "flexbin_streams.hpp"
#include "flexbin_buffer.hpp"

#define FLEXBIN_CLASS_ID(id)  enum { flexbin_class_id = id };

#define FLEXBIN_SERIALIZE_FIXED(...) \
  auto flexbin_serialize_fixed() const { return std::forward_as_tuple(__VA_ARGS__); } \
  auto flexbin_deserialize_fixed()     { return std::forward_as_tuple(__VA_ARGS__); } 

#define FLEXBIN_SERIALIZE_REQUIRED(...) \
  auto flexbin_serialize_required() const { return std::forward_as_tuple(__VA_ARGS__); } \
  auto flexbin_deserialize_required()     { return std::forward_as_tuple(__VA_ARGS__); } 

#define FLEXBIN_SERIALIZE_OPTIONAL(...) \
  auto flexbin_serialize_optional() const { return std::forward_as_tuple(__VA_ARGS__); } \
  auto flexbin_deserialize_optional()     { return std::forward_as_tuple(__VA_ARGS__); }

#define FLEXBIN_SERIALIZE_SIMPLIFIED(...) \
  auto flexbin_serialize_simplified() const { return std::forward_as_tuple(__VA_ARGS__); } \
  auto flexbin_deserialize_simplified()     { return std::forward_as_tuple(__VA_ARGS__); } 

namespace flexbin
{

  template<typename T>
  std::basic_ostream<char>& ostream::operator<< (const T& obj)
  {
    flexbin_writer<T> writer;
    if ( !(writer.write_header(*this, obj) &&
      writer.write_fixed_fields(*this, obj) && // fixed fields goes first
      writer.write_required_fields(*this, obj) &&
      writer.write_optional_fields(*this, obj) &&
      writer.write_simplified_fields(*this, obj) &&
      writer.write_bottom(*this, obj)) )
    {
      setstate(rdstate() & std::ios_base::badbit);
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
      setstate(rdstate() & std::ios_base::badbit);
    }

    return *this;
  }

  inline bool class_id(const void *data, size_t nbytes,  uint16_t & retval)
  {
    memmap_buffer buffer(reinterpret_cast<const char *>(data), nbytes);
    istream istr(&buffer);

    uint32_t size = 0;
    if (!type_traits<uint32_t>::read(istr, size))
      return false;

    if (!type_traits<uint16_t>::read(istr, retval))
      return false;

    return true;
  }

  template <typename T>
  inline std::size_t class_object_size(const T& obj) {
    memmap_buffer buffer(reinterpret_cast<const char *>(nullptr), static_cast<size_t>(0) );
    ostream ostr(&buffer, true);
    ostr << obj;
    return ostr.written_count(); 
  }


} // namespace flexbin 

