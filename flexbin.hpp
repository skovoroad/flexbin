#pragma once
#include <iostream>
#include <tuple>
#include <string>

namespace flexbin
{
  struct istream : public std::basic_istream<char>
  {
  	istream(std::streambuf * buf) : std::basic_istream<char>(buf)
  	{
  	}

  	template<typename T>
  	std::basic_istream<char>& operator>> ( T& )
  	{
      return *this;
  	}
  };

  struct ostream : public std::basic_ostream<char>
  {
  	ostream(std::streambuf * buf) : std::basic_ostream<char>(buf)
  	{
  	}

    template<typename T> std::basic_ostream<char>& operator<< (const T& obj);
  };


  template<typename T>
  struct type_traits
  {
    static const T default_value_;
    static const uint8_t code_; 
  };

  template<>
  struct type_traits<uint64_t>
  {
    static const uint64_t default_value_ = 0;
    static const uint8_t code_ = 16; 
  };

  template<>
  struct type_traits<uint32_t>
  {
    static const uint32_t default_value_ = 0;
    static const uint8_t code_ = 8; 
  };

  template<>
  struct type_traits<std::string>
  {
    static const std::string default_value_;
    static const uint8_t code_ = 21; 
  };

  template<typename T>
  struct store_strategy_fixed {
    size_t write( ostream& ostr, const T& value) {
      ostr << value;
      return 0;
    };
  };

  struct store_strategy_required {
    template<typename T>    
    static size_t write( ostream& ostr, const T& value) {
/*      if(std::is_fundamental<T>())
      {
        ostr.write(&type_traits<t>::code, 1);

        // field id !!! how to get it?

        ostr.write(&value, sizeof(value));        
      }
      else*/
      {
        ostr << value;
      }
      return 0;
    };
  };

  template<typename T>
  struct store_strategy_optional{
    size_t write( ostream& ostr, const T& value) {
      ostr << value;
      return 0;
    };
  };

  template<typename T>
  struct store_strategy_simplified{
    size_t write( ostream& ostr, const T& value) {
      ostr << value;
      return 0;
    };
  };

  template<typename T> 
  std::basic_ostream<char>& ostream::operator<< (const T& obj)
  {
    {
      auto field_serializer_required = [this](auto&&... args) { 
            ( ( store_strategy_required::write(*this, args) ) , 
            ...
          );
      };

      std::apply( 
        field_serializer_required,
        obj.flexbin_serialize_required() 
      );
    }
    return *this;
  }

  template<> 
  std::basic_ostream<char>& ostream::operator<< <uint64_t> (const uint64_t& val)
  {
    return *this;
  }

  template<> 
  std::basic_ostream<char>& ostream::operator<< <uint32_t> (const uint32_t& val)
  {
    return *this;
  }

  template<> 
  std::basic_ostream<char>& ostream::operator<< <std::string> (const std::string& val)
  {
    return *this;
  }

} // flexbin

#define FLEXBIN_SERIALIZE_FIXED(...) \
  auto flexbin_serialize_fixed() const { return std::forward_as_tuple(__VA_ARGS__); } 

#define FLEXBIN_SERIALIZE_REQUIRED(...) \
  auto flexbin_serialize_required() const { return std::forward_as_tuple(__VA_ARGS__); } 

#define FLEXBIN_SERIALIZE_OPTIONAL(...) \
  auto flexbin_serialize_optional() const { return std::forward_as_tuple(__VA_ARGS__); } 

#define FLEXBIN_SERIALIZE_SIMPLIFIED(...) \
  auto flexbin_serialize_simplified() const { return std::forward_as_tuple(__VA_ARGS__); } 
