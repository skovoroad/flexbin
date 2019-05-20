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
    enum { code_ = 0 };
    inline static size_t write( ostream& ostr, const T& ) { return 0;}

  };

  template<>
  struct type_traits<uint64_t>
  {
    enum { default_value_ = 0 };
    enum { code_ = 16 };

    inline static size_t write( ostream& ostr, const uint64_t& val) { 
      ostr.write(reinterpret_cast<const char*>(&val), sizeof(uint64_t));
      return sizeof(uint64_t); 
    }

  };

  template<>
  struct type_traits<uint32_t>
  {
    enum { default_value_ = 0 };
    enum { code_ = 8 };

    inline static size_t write( ostream& ostr, const uint32_t& val ) { 
      ostr.write(reinterpret_cast<const char*>(&val), sizeof(uint32_t));
      return sizeof(uint64_t); 
     }

  };

  template<>
  struct type_traits<std::string>
  {
    enum { code_ = 21 };
    
    inline static size_t write( ostream& ostr, const std::string& ) { 
      return 0; 
    }
  };

  template<typename T>
  struct store_strategy_fixed {
    size_t write( ostream& ostr, uint8_t field_id,  const T& value) {
      ostr << value;
      return 0;
    };
  };

  struct store_strategy_required {
    template<typename T>    
    static size_t write( ostream& ostr,  uint8_t field_id, const T& value) {
      if(std::is_fundamental<T>())
      {
        uint8_t code = type_traits<T>::code_;
        ostr.write(reinterpret_cast<const char*>(&code), 1);
        ostr.write(reinterpret_cast<const char*>(&code, field_id), 1);
        type_traits<T>::write(ostr, value);
      }
      else
      {
        ostr << value;
      }
      return 0;
    };
  };

  template<typename T>
  struct store_strategy_optional{
    size_t write( ostream& ostr,  uint8_t field_id, const T& value) {
      ostr << value;
      return 0;
    };
  };

  template<typename T>
  struct store_strategy_simplified{
    size_t write( ostream& ostr,  uint8_t field_id, const T& value) {
      ostr << value;
      return 0;
    };
  };

  template<typename T> 
  std::basic_ostream<char>& ostream::operator<< (const T& obj)
  {
    {
      uint8_t field_id = 0;
      auto field_serializer_required = [this, &field_id](auto&&... args) { 
            ( ( store_strategy_required::write(*this, ++field_id,  args) ) , 
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
