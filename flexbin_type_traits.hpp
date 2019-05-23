#pragma once
#include <iostream>

namespace flexbin
{
  template<typename T>
  struct type_traits
  {
    enum { code_ = 29 /* "object" field id */ };
    inline static size_t write( std::basic_ostream<char>& ostr, const T& ) { return 0;}
    inline static size_t pack( std::basic_ostream<char>& ostr, const T&) { return 0;}
  };

  template<>
  struct type_traits<uint64_t>
  {
    enum { default_value_ = 0 };
    enum { code_ = 16 };

    inline auto candidates(const uint64_t& value) const{
      return std::make_tuple(
        static_cast<uint8_t>(value),
        static_cast<uint16_t>(value),
        static_cast<uint32_t>(value)
      );
    }

    inline static size_t write( std::basic_ostream<char>& ostr, const uint64_t& val) { 
      ostr.write(reinterpret_cast<const char*>(&val), sizeof(uint64_t));
      return sizeof(uint64_t); 
    }

    inline static size_t pack( std::basic_ostream<char>& ostr, const uint64_t&) { 
      return 0;
      }
  };

  template<>
  struct type_traits<uint32_t>
  {
    enum { default_value_ = 0 };
    enum { code_ = 8 };

    inline static size_t write( std::basic_ostream<char>& ostr, const uint32_t& val ) { 
      ostr.write(reinterpret_cast<const char*>(&val), sizeof(uint32_t));
      return sizeof(uint32_t); 
     }

    inline static size_t pack( std::basic_ostream<char>& ostr, const uint32_t&) { 
      return 0;
      }

    inline auto candidates(const uint32_t& value) const {
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
    
    inline static size_t write( std::basic_ostream<char>& ostr, const uint8_t& val ) { 
      ostr.write(reinterpret_cast<const char*>(&val), sizeof(uint8_t));
      return sizeof(uint8_t); 
     }

    inline static size_t pack( std::basic_ostream<char>& ostr, const uint8_t&) { 
      return 0;
      }

    inline auto candidates(const uint8_t& value) const {
      return std::make_tuple(
        static_cast<uint8_t>(value)
      );
    }


  };

  template<>
  struct type_traits<std::string>
  {
    enum { code_ = 21 };

    inline static size_t write( std::basic_ostream<char>& ostr, const std::string& ) { 
      return 0; 
    }

    inline static size_t pack( std::basic_ostream<char>& ostr, const std::string&) { 
      return 0;
    }
  };
} // namespace flexbin