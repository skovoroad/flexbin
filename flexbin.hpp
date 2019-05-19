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

    template<typename T> std::basic_ostream<char>& operator<< (const T& obj);
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
      ostr << value;
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

  template<typename T>
  struct type_traits
  {
    static const T default_value_;
    static const uint8_t code_; 

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
