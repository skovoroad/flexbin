#pragma once
#include <iostream>

namespace flexbin 
{
  struct istream : public std::basic_istream<char>
  {
    istream(std::streambuf * buf) : std::basic_istream<char>(buf)
    {
    }

    template<typename T> std::basic_istream<char>& operator>> (T&);
  };

  struct ostream : public std::basic_ostream<char>
  {
    typedef std::basic_ostream<char> base;

    ostream(std::streambuf * buf, bool count_mode = false) : 
      std::basic_ostream<char>(buf),
      count_mode_(count_mode)
    {
    }

    template<typename T> std::basic_ostream<char>& operator<< (const T& obj);
    
    basic_ostream& write(const base::char_type* data, std::streamsize nbytes) {
      if (!count_mode_)
        return base::write(data, nbytes);
      count_ += nbytes;
      return *this;
    }

    std::streamsize written_count() {
      return count_;
    }
  private:
    bool count_mode_ = false;
    std::streamsize count_ = 0;
  };
}