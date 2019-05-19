#pragma oncm
#include <iostream>


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
        *this << obj;
      }
      else
      {
        auto field_serializer = [this](auto&&... args) { 
            ( ( *this << args), ...);
        };

        std::apply( 
          field_serializer,
          obj.flexbin_serialize() 
        );
      }
      return *this;
    }
  };

  template<> std::basic_ostream<char>& ostream::operator<< <uint64_t> (const uint64_t& val)
  {
    // write here
    return *this;
  }

  template<> std::basic_ostream<char>& ostream::operator<< <uint32_t> (const uint32_t& val)
  {
    // write here
    return *this;
  }

} // flexbin

#define FLEXBIN_SERIALIZE(...) \
  auto flexbin_serialize() const { return std::forward_as_tuple(__VA_ARGS__); } 
