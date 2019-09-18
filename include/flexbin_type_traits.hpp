#pragma once
#include <iostream>
#include <vector>
#include <unordered_set>
#include <unordered_map>
#include "flexbin_streams.hpp"
#include "flexbin_stringview.hpp"

namespace flexbin {
  template<typename T, class Enabler = void>
  struct type_traits
  {
    enum { code_ = 29 /* "object" field id */ };
  };

  template<typename T>
  bool write_field_header(ostream& ostr, uint8_t field_id) {
    uint8_t code = type_traits<T>::code_;
    auto nbytes = sizeof(T) + 2;
    ostr.write(reinterpret_cast<const char*>(&code), 1);
    ostr.write(reinterpret_cast<const char*>(&field_id), 1);
    return !ostr.failed() && ostr.good();
  }

  /////////////////
  // packer for fundamental types
  template <typename... Args>
  struct type_packer;

  template<typename T, typename TCandidate, typename... Rest>
  struct type_packer<T, TCandidate, Rest...>
  {
    inline static bool pack(ostream& ostr, uint8_t field_id, const T& val) {
      auto candidate = static_cast<TCandidate>(val);
      if (candidate == val) {
        return write_field_header<TCandidate>(ostr, field_id) &&
          type_traits<TCandidate>::write(ostr, candidate);
      }

      return type_packer<T, Rest...>::pack(ostr, field_id, val);
    }
  };

  template<typename T, typename TCandidate>
  struct type_packer<T, TCandidate>
  {
    inline static bool pack(ostream& ostr, uint8_t field_id, const T& val) {
      auto candidate = static_cast<TCandidate>(val);
      if (candidate == val) {
        return write_field_header<TCandidate>(ostr, field_id) &&
          type_traits<TCandidate>::write(ostr, candidate);
      }

      return write_field_header<T>(ostr, field_id) &&
        type_traits<T>::write(ostr, val);
    }
  };


  template<typename T>
  struct type_packer<T>
  {
    inline static bool pack(ostream& ostr, uint8_t field_id, const T& val) {
      return write_field_header<T>(ostr, field_id) &&
        type_traits<T>::write(ostr, val);
    }
  };
  // packer
  /////////////////
  // writer for fundamental types
  template<typename T>
  struct type_writer
  {
    inline static bool write(ostream& ostr, const T& val) {
      ostr.write(reinterpret_cast<const char*>(&val), sizeof(T));
      return !ostr.failed() && ostr.good();
    }

    inline static bool read(istream& istr, T& val) {
      istr.read(reinterpret_cast<char*>(&val), sizeof(T));
      return istr.good();
    }

    inline static T default_value() { return 0;  }
  };

  ///////////
  //  fundamental types traits
  template<>
  struct type_traits<float> :
    public type_writer<float>,
    type_packer<float>
  {
    enum { code_ = 18 };
  };

  template<>
  struct type_traits<double> :
    public type_writer<double>,
    type_packer<double>
  {
    enum { code_ = 19 };
  };

  
  template<>
  struct type_traits<uint8_t> :
    public type_writer<uint8_t>,
    type_packer<uint8_t>
  {
    enum { code_ = 2 };
  };

  template<>
  struct type_traits<uint16_t> :
    public type_writer<uint16_t>,
    type_packer<uint16_t, uint8_t>
  {
    enum { code_ = 4 };
  };


  template<>
  struct type_traits<uint32_t> :
    public type_writer<uint32_t>,
    type_packer<uint32_t, uint8_t, uint16_t> // base class first, candidates next
  {
    enum { code_ = 8 };
  };

  template<>
  struct type_traits<uint64_t> :
    public type_writer<uint64_t>,
    type_packer<uint64_t, uint8_t, uint16_t, uint32_t>// base class first, candidates next
  {
    enum { code_ = 16 };
  };

  ///////////
  template<>
  struct type_traits<int8_t> :
    public type_writer<int8_t>,
    type_packer<int8_t>
  {
    enum { code_ = 3 };
  };

  template<>
  struct type_traits<int16_t> :
    public type_writer<int16_t>,
    type_packer<int16_t, int8_t>
  {
    enum { code_ = 5 };
  };

  template<>
  struct type_traits<int32_t> :
    public type_writer<int32_t>,
    type_packer<int32_t, int8_t, int16_t> // base class first, candidates next
  {
    enum { code_ = 9 };
  };

  template<>
  struct type_traits<int64_t> :
    public type_writer<int64_t>,
    type_packer<int64_t, int8_t, int16_t, int32_t>// base class first, candidates next
  {
    enum { code_ = 17 };
  };


  template<typename T>
  struct type_traits<std::vector<T>>
  {
    inline constexpr static std::vector<T> default_value() { return std::vector<T>(); }

    enum { code_ = 25 };
  };

  template<typename T>
  struct type_traits<std::unordered_set<T>>
  {
    enum { code_ = 25 };
  };

  template<typename TKey, typename TValue>
  struct type_traits<std::unordered_multimap<TKey, TValue>>
  { // write like array of pairs key, value, key, value, key, value, key, value, key, value
    enum { code_ = 25 };
  };

  template<typename T>
  struct type_traits<std::unique_ptr<T>>
  {
    enum { code_ = type_traits<T>::code_ };
    inline constexpr static  std::unique_ptr<T> default_value() { return std::unique_ptr<T>(); }
  };

  template<typename T>
  struct type_traits<std::shared_ptr<T>>
  {
    enum { code_ = type_traits<T>::code_ };
    inline constexpr static std::shared_ptr<T> default_value() { return std::shared_ptr<T>(); }
  };

  template<typename T>
  struct type_traits<T, std::enable_if_t<std::is_enum<T>::value> >
    : public type_packer<T>
  {
    enum { code_ = 200 };
    inline constexpr static T default_value() { return 0; }

    constexpr static size_t enum_bytes = sizeof(T);

    inline static bool write(ostream& ostr, const T& val) {
      ostr.write(reinterpret_cast<const char*>(&val), enum_bytes);
      return !ostr.failed() && ostr.good();
    }

    inline static bool read(istream& istr, T& val) {
      istr.read(reinterpret_cast<char*>(&val), enum_bytes);
      return istr.good();
    }
  };

  template<>
  struct type_traits<bool>
    : public type_packer<bool>
  {
    inline constexpr static bool default_value() { return false; }

    enum { code_ = 1 };

    inline static bool write(ostream& ostr, const bool& val) {
      char bval = val ? 1 : 0;
      ostr.write(reinterpret_cast<const char*>(&bval), 1);
      return !ostr.failed() && ostr.good();
    }

    inline static bool read(istream& istr, bool& val) {
      char bval;
      istr.read(reinterpret_cast<char*>(&bval), 1);
      if (bval != 0 && bval != 1)
        return false;
      val = bval == 0 ? false : true;
      return istr.good();
    }
  };

  template<typename T>
  struct type_traits<std::basic_string<T>>
  {
    enum { code_ = 21 };
    inline constexpr static std::basic_string<T> default_value() { return std::basic_string<T>(); }


    typedef uint16_t len_t;
    inline static bool write(ostream& ostr, const std::basic_string<T>& str) {
      len_t len = static_cast<len_t>(str.size());
      type_traits<len_t>::write(ostr, len);
      ostr.write(reinterpret_cast<const char*>(str.data()), len * sizeof(T));
      return !ostr.failed() && ostr.good();
    }

    inline static bool read(istream& istr, std::basic_string<T>& val) {
      len_t len = 0;
      if (!type_traits<len_t>::read(istr, len)) 
        return false;

      val.resize(len);
      istr.read(reinterpret_cast<char*>(val.data()), len * sizeof(T));
      return istr.good();
    }
  };

  template<typename T>
  struct type_traits<basic_buffered_stringview<T>>
  {
    enum { code_ = 21 };
    inline constexpr static basic_buffered_stringview<T> default_value() { return basic_buffered_stringview<T>(); }

    typedef uint16_t len_t;

    inline static bool write(ostream& ostr, const basic_buffered_stringview<T>& str) {
      len_t len = static_cast<len_t>(str.size());
      type_traits<len_t>::write(ostr, len);
      ostr.write(reinterpret_cast<const char*>(str.data()), len * sizeof(T));
      return !ostr.failed() && ostr.good();
    }

    inline static bool read(istream& istr, basic_buffered_stringview<T>& str) {
      len_t len = 0;
      if (!type_traits<len_t >::read(istr, len))
        return false;

      std::basic_string<T> buffer;
      buffer.resize(len);
      istr.read(reinterpret_cast<char*>(buffer.data()), len * sizeof(T));

      str = buffer;
      str.bufferize();

      return istr.good();
    }
  };

  constexpr uint8_t end_marker = 255;
}