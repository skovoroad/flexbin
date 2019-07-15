#pragma once

#include "gtest/gtest.h"
#include "flexbin.hpp"

struct SampleStruct {
  int a_ = 0;
  int b_ = 0;
  int c_ = 0;
  int d_ = 0;

  FLEXBIN_CLASS_ID(1234);
  FLEXBIN_SERIALIZE_REQUIRED(a_);
  FLEXBIN_SERIALIZE_FIXED(b_);
  FLEXBIN_SERIALIZE_OPTIONAL(c_);
  FLEXBIN_SERIALIZE_SIMPLIFIED(d_);


  bool operator==(const SampleStruct &r) const { 
    return a_ == r.a_ && 
           b_  == r.b_ &&
           c_  == r.c_ &&
           d_  == r.d_
           ; 
  }
};

namespace fbtest {
  typedef std::tuple<bool, bool, bool, uint16_t> result_t;
  enum result_fields {
    result_fbout_good = 0,
    result_fbin_good = 1,
    result_get_class_id_good = 2,
    result_class_id = 3
  };

  template<typename T>
  inline std::tuple<bool, bool, bool, uint16_t> serialize_and_return_classid(const T& l, T& r) {
    auto object_size = flexbin::class_object_size(l);
    std::vector<char> buffer;
    buffer.resize(object_size);

    flexbin::istream fbin(buffer.data(), buffer.size());
    flexbin::ostream fbout(buffer.data(), buffer.size());

    fbout << l;

    uint16_t classid(0);
    auto result = flexbin::class_id(buffer.data(), buffer.size(), classid);
    fbin >> r;

    return result_t(fbout.good(), fbin.good(), result, classid);
  };
}