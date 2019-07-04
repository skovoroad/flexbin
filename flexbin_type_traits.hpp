#pragma once
#include <iostream>
#include "flexbin_streams.hpp"

namespace flexbin
{
  template<typename T>
  struct type_traits
  {
    enum { code_ = 29 /* "object" field id */ };
    //inline static size_t write( std::basic_ostream<char>& ostr, const T& ) { return 0;}
  };

  template<typename T>
  struct type_traits<std::vector<T>>
  {
    enum { code_ = 25};
  };


  template<>
  struct type_traits<uint64_t>
  {
    enum { default_value_ = 0 };
    enum { code_ = 16 };

    inline static auto candidates(const uint64_t& value) {
      return std::make_tuple(
        static_cast<uint8_t>(value),
        static_cast<uint16_t>(value),
        static_cast<uint32_t>(value),
        static_cast<uint64_t>(value)
      );
    }

    inline static size_t write( ostream& ostr, const uint64_t& val) { 
      ostr.write(reinterpret_cast<const char*>(&val), sizeof(uint64_t));
      return sizeof(uint64_t); 
    }

    inline static bool read(istream& istr, uint64_t& val) {
      istr.read(reinterpret_cast<char*>(&val), sizeof(uint64_t));
      return istr.good();
    }
  };

  template<>
  struct type_traits<uint32_t>
  {
    enum { default_value_ = 0 };
    enum { code_ = 8 };

    inline static size_t write(ostream& ostr, const uint32_t& val ) {
      ostr.write(reinterpret_cast<const char*>(&val), sizeof(uint32_t));
      return sizeof(uint32_t); 
     }

    inline static bool read(istream& istr, uint32_t& val) {
      istr.read(reinterpret_cast<char*>(&val), sizeof(uint32_t));
      return istr.good();
    }

    inline static auto candidates(const uint32_t& value)  {
      return std::make_tuple(
        static_cast<uint8_t>(value),
        static_cast<uint16_t>(value),
        static_cast<uint32_t>(value)
      );
    }
  };

  template<>
  struct type_traits<uint16_t>
  {
    enum { default_value_ = 0 };
    enum { code_ = 4 };

    inline static size_t write(ostream& ostr, const uint16_t& val) {
      ostr.write(reinterpret_cast<const char*>(&val), sizeof(uint16_t));
      return sizeof(uint16_t);
    }

    inline static bool read(istream& istr, uint16_t& val) {
      istr.read(reinterpret_cast<char*>(&val), sizeof(uint16_t));
      return istr.good();
    }

    inline static auto candidates(const uint16_t& value) {
      return std::make_tuple(
        static_cast<uint8_t>(value),
        static_cast<uint16_t>(value)
      );
    }
  };


  template<>
  struct type_traits<uint8_t>
  {
    enum { default_value_ = 0 };
    enum { code_ = 2 };
    
    inline static size_t write(ostream& ostr, const uint8_t& val ) {
      ostr.write(reinterpret_cast<const char*>(&val), sizeof(uint8_t));
      return sizeof(uint8_t); 
     }

    inline static bool read(istream& istr, uint8_t& val) {
      istr.read(reinterpret_cast<char*>(&val), sizeof(uint8_t));
      return istr.good();
    }

    inline static auto candidates(const uint8_t& value) {
      return std::make_tuple(
        static_cast<uint8_t>(value)
      );
    }
  };

  template<>
  struct type_traits<std::string>
  {
    enum { code_ = 21 };
    enum { default_value_ = 0 };

    inline static size_t write(ostream& ostr, const std::string& str) {
      size_t len = str.length();
      type_traits<size_t>::write(ostr, len);
      ostr.write(str.data(), len);
      //ostr << str;
      return len + sizeof(size_t); 
    }

    inline static bool read(istream& istr, std::string& val) {
      size_t len = 0;
      if (!type_traits<size_t >::read(istr, len)) {
        // ...
        return false;
      }
      val.resize(len);
      istr.read(val.data(), len);

      return istr.good();
    }

    inline static auto candidates(const std::string value) {
      return std::make_tuple(
        static_cast<std::string>(value)
      );
    }

/*
    inline static size_t write(ostream& ostr, const std::string& ) {
      return 0; 
    }

    inline static bool read(std::basic_istream<char>& istr, std::string& val) {
      return istr.good();
    }*/
  };

  constexpr uint8_t end_marker = 255;
} // namespace flexbin