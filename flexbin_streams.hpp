#pragma once
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

    template<typename T> std::basic_ostream<char>& operator<< (const T& obj);
  };
}