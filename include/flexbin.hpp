#pragma once

#include <tuple>

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
