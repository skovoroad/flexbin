#pragma once 

#include <limits>
#include <ios>
#include <streambuf>

namespace flexbin
{

  template<
    typename charT,
    typename traitsT = std::char_traits<charT>
  >
  class basic_memmap_buffer
    : public std::basic_streambuf<
        charT,
        traitsT
      >
  {
  public:
    typedef charT
      char_type;
    typedef charT*
      pointer;
    typedef charT const*
      const_pointer;
    typedef traitsT
      traits_type;
    typedef typename traits_type::int_type
      int_type;
    typedef typename traits_type::pos_type
      pos_type;
    typedef typename traits_type::off_type
      off_type;
    typedef std::size_t size_type;
  public:
    basic_memmap_buffer(const_pointer ptr,
                        size_type n)
    {
      pointer from = const_cast<pointer>(ptr);
      pointer to = from + n;
      this->setp(from, to);
      this->setg(from, from, to);
    }

    basic_memmap_buffer(const_pointer begin,
                        const_pointer end)
    {
      pointer from = const_cast<pointer>(begin);
      pointer to = const_cast<pointer>(end);
      this->setp(from, to);
      this->setg(from, from, to);
    }
  public:
    void reset()
    {
      this->setp(
        this->pbase(),
        this->epptr()
      );
      this->setg(
        this->eback(),
        this->eback(),
        this->egptr()
      );
    }
    // returns a pointer to the
    // beginning of the underlying
    // buffer
    pointer get() const
    {
      return this->pbase();
    }

    // number of elements allready written
    // into the output buffer
    size_type putcount() const
    {
      return size_type(
        this->pptr() - this->pbase()
      );
    }
    // number of elements allready red from
    // the buffer
    size_type getcount() const
    {
      return size_type(
        this->gptr() - this->eback()
      );
    }
    // number of elements available in
    // the input buffer
    size_type avlcount() const
    {
      return size_type(
        this->egptr() - this->gptr()
      );
    }
    // capacity of the underlying buffer
    size_type capacity() const
    {
      return size_type(
        this->epptr() - this->pbase()
      );
    }

    bool syncareas()
    { // sync GET-area pointers according
      // to the PUT-area pointers
      if(this->gptr() < this->pptr())
      {
        this->setg(
          this->eback(),
          this->gptr(),
          this->pptr()
        );
        return true;
      }
      return false;
    }
  protected:
    //int_type overflow(int_type c = traits_type::eof())
    //{ // not enough space to output data...
    //  // default implementation is enough to do the job
    //  return traits_type::eof();
    //}
    //int_type underflow()
    //{ // no data to read from the input buffer...
    //  // default implementation is enough to do the job
    //  return traits_type::eof();
    //}
    pos_type seekoff(off_type off, std::ios_base::seekdir way,
                      std::ios_base::openmode which)
    {
      pos_type pos(off);
      switch(way)
      {
        case std::ios_base::beg:
          break;
        case std::ios_base::cur:
          if((which & (std::ios_base::in && which & std::ios_base::out)) ||
              which & std::ios_base::out)
          {
            pos += int_type(
              this->pptr() - this->pbase()
            );
          }
          else
          if(which & std::ios_base::in)
          {
            pos += int_type(
              this->gptr() - this->eback()
            );
          }
          else
          {
            return pos_type(off_type(-1));
          }
        break;
        case std::ios_base::end:
          if(which & std::ios_base::in && which & std::ios_base::out)
          {
            pos = int_type(
              this->egptr() - this->pbase()
            ) + pos;
          }
          else
          if(which & std::ios_base::out)
          {
            pos = int_type(
              this->epptr() - this->pbase()
            ) + pos;
          }
          else
          if(which & std::ios_base::in)
          {
            pos = int_type(
              this->egptr() - this->eback()
            ) + pos;
          }
          else
          {
            return pos_type(off_type(-1));
          }
        break;
        default:
          return pos_type(off_type(-1));
      }
      return this->seekpos(pos, which);
    }

    pos_type seekpos(pos_type pos, std::ios_base::openmode which)
    {
      off_type offset = off_type(pos);
      if(which & (std::ios_base::in && which & std::ios_base::out))
      {
        if(offset < 0 ||
            offset > (this->egptr() - this->eback()) ||
            offset > (this->epptr() - this->pbase()))
        {
          return pos_type(off_type(-1));
        }
        this->setg(
          this->eback(),
          this->eback() + offset,
          this->egptr()
        );
        this->setp(
          this->pbase(),
          this->epptr()
        );
        this->pbump(int(offset));
      }
      else
      if(which & std::ios_base::in)
      {
        if(offset < 0 || offset > (this->egptr() - this->eback()))
        {
          return pos_type(off_type(-1));
        }
        this->setg(
          this->eback(),
          this->eback() + offset,
          this->egptr()
        );
      }
      else
      if(which & std::ios_base::out)
      {
        if(offset < 0 || offset > (this->epptr() - this->pbase()))
        {
          return pos_type(off_type(-1));
        }
        this->setp(
          this->pbase(),
          this->epptr()
        );
        this->pbump(int(offset));
      }
      return pos;
    }
  }; // class <basic_memmap_buffer>


  typedef basic_memmap_buffer<
    char,
    std::char_traits<char>
  > memmap_buffer;
} 

