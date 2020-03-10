#pragma once
#include <iostream>
#include <cstring>
#include <memory>
#include "flexbin_buffer.hpp"

namespace flexbin 
{
  struct istream : 
    private flexbin::memmap_buffer,
    public std::basic_istream<char>
  {
    istream(const char *p, size_t nbytes) : 
      flexbin::memmap_buffer(p, nbytes),
      std::basic_istream<char>(this)
    {
    }

    template<typename T> std::basic_istream<char>& operator>> (T&);
    template<typename T> std::basic_istream<char>& operator>> (std::shared_ptr<T>& obj){
      return istream::operator>> (*obj);
    }
    template<typename T, typename TDeleter> std::basic_istream<char>& operator>> (std::unique_ptr<T, TDeleter>& obj){
      return istream::operator>> (*obj);
    }

    bool failed() { return failed_; }
  private:
    bool failed_ = false;
  };

  struct ostream : 
    private flexbin::memmap_buffer,
    public std::basic_ostream<char>
  {
    typedef std::basic_ostream<char> base;

    ostream(char *p, size_t nbytes, bool count_mode = false) : 
      flexbin::memmap_buffer(p, nbytes),
      std::basic_ostream<char>(this), // in fact we ignore flexbin::memmap_buffer inheritance; just phony for maybe future beautiful times
      count_mode_(count_mode),
      pbegin_(p),
      pos_(p),
      pend_(p + nbytes)
    {
    }

    template<typename T> std::basic_ostream<char>& operator<< (const T& obj);
    template<typename T> std::basic_ostream<char>& operator<< (const std::shared_ptr<T>& obj){
      return ostream::operator<<(*obj);
    }
    template<typename T, typename TDeleter> std::basic_ostream<char>& operator<< (const std::unique_ptr<T, TDeleter>& obj){
      return ostream::operator<<(*obj);
    }
    
    basic_ostream& write(const base::char_type* data, size_t nbytes) {
      if (!count_mode_) {
        if(pos_ + nbytes > pend_) {
          failed_ = true;
          return *this;
        }
        std::memcpy(reinterpret_cast<void *>(pos_), data, nbytes);
        pos_ += nbytes;
      }
        //base::write(data, nbytes);
      count_ += nbytes;
      return *this;
    }

    std::streamsize written_count() {
      return count_;
    }

    bool write_direct(size_t pos, const char * p, size_t nbytes) {
      if (!count_mode_) {
        if (pbegin_ + (size_t)pos + nbytes > pend_) {
          failed_ = true;
          return false;
        }
        std::memcpy(reinterpret_cast<void *>(pbegin_ + (size_t)pos), p, nbytes);
      }
      //count_ += nbytes;
      return true;
    }

    bool failed() { return failed_; }
  private:
    bool count_mode_ = false;
    std::streamsize count_ = 0;
    bool failed_ = false;
    char *pbegin_ = nullptr;
    char *pos_ = nullptr;
    char *pend_ = nullptr;
  };
}