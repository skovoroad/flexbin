#pragma once
#include <type_traits>
#include <iostream>
#include <tuple>
#include <string>

#include "flexbin_type_traits.hpp"
#include "flexbin_write.hpp"
#include "flexbin_read.hpp"
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
    flexbin_reader<T> reader;

    if(!reader.read(*this, obj))
      throw( std::ios_base::failure("malformed object") );
    return *this;
  }

} // namespace flexbin 

