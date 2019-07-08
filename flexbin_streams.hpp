#pragma once
#include <iostream>
#include <cstring>
#include "flexbin_buffer.hpp"

namespace flexbin 
{
  struct istream : public std::basic_istream<char>
  {
    istream(const char *p, size_t nbytes) : 
      mem_buf_(p, nbytes),
      std::basic_istream<char>(&mem_buf_)
    {
    }

    template<typename T> std::basic_istream<char>& operator>> (T&);
  private:
    flexbin::memmap_buffer mem_buf_;
  };

  struct ostream : public std::basic_ostream<char>
  {
    typedef std::basic_ostream<char> base;

    ostream(char *p, size_t nbytes, bool count_mode = false) : 
      pbase_(p),
      mem_buf_(p, nbytes),
      std::basic_ostream<char>(&mem_buf_),
      count_mode_(count_mode)
    {
    }

    template<typename T> std::basic_ostream<char>& operator<< (const T& obj);
    
    basic_ostream& write(const base::char_type* data, std::streamsize nbytes) {
      if (!count_mode_)
        base::write(data, nbytes);
      count_ += nbytes;
      return *this;
    }

    std::streamsize written_count() {
      return count_;
    }

    bool write_direct(std::streamsize pos, const char * p, size_t nbytes) {
      if (!count_mode_)
        std::memcpy( reinterpret_cast<void *>(pbase_ + (size_t)pos), p, nbytes);
      //count_ += nbytes;
      return true;
    }

  private:
    bool count_mode_ = false;
    std::streamsize count_ = 0;
    flexbin::memmap_buffer mem_buf_;
    char *pbase_;
  };
}