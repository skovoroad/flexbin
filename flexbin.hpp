#pragma oncm
#include <iostream>
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

    template<typename T> std::basic_ostream<char>& operator<< (const T& obj)
    {
      if(std::is_fundamental<T>::value)
      {
//        ??? dispatch fixed or requred or simplified or optional ??? 
        *this << obj;
      }
      else
      {
        auto field_serializer_required = [this](auto&&... args) { 
            ( ( *this << args), ...);
        };

        std::apply( 
          field_serializer_required,
          obj.flexbin_serialize_required() 
        );
      }
      return *this;
    }
  };

  template<> 
  std::basic_ostream<char>& ostream::operator<< <uint64_t> (const uint64_t& val)
  {
    // write here
    return *this;
  }

  template<> 
  std::basic_ostream<char>& ostream::operator<< <uint32_t> (const uint32_t& val)
  {
    // write here
    return *this;
  }

  template<> 
  std::basic_ostream<char>& ostream::operator<< <std::string> (const std::string& val)
  {
    // write here
    return *this;
  }

  template<typename T>
  struct type_traits
  {
    static const T default_value;
    static const uint8_t code; 

    static size_t write_fixed(std::streambuf *, const T &);
  };

} // flexbin

#define FLEXBIN_SERIALIZE_FIXED(...) \
  auto flexbin_serialize_fixed() const { return std::forward_as_tuple(__VA_ARGS__); } 

#define FLEXBIN_SERIALIZE_REQUIRED(...) \
  auto flexbin_serialize_required() const { return std::forward_as_tuple(__VA_ARGS__); } 

#define FLEXBIN_SERIALIZE_OPTIONAL(...) \
  auto flexbin_serialize_optional() const { return std::forward_as_tuple(__VA_ARGS__); } 

#define FLEXBIN_SERIALIZE_SIMPLIFIED(...) \
  auto flexbin_serialize_simplified() const { return std::forward_as_tuple(__VA_ARGS__); } 
