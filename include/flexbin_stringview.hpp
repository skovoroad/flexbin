#pragma once
#include <string_view>

//#define FLEXBIN_USE_BOOST

#ifdef FLEXBIN_USE_BOOST
#include <boost/utility/string_view.hpp>

namespace sv = boost;
#else
namespace sv = std;
#endif

namespace flexbin {

  template<typename TElem>
  class basic_buffered_stringview : public sv::basic_string_view<TElem> {
  public:
    typedef sv::basic_string_view<TElem> base;

    basic_buffered_stringview<TElem>()
      : base() {}


    basic_buffered_stringview<TElem>(const sv::basic_string_view<TElem>& r)
      : base(r) {}

    basic_buffered_stringview<TElem>(const basic_buffered_stringview<TElem>& r)
      : base(r) {
      if(!r.buffer_.empty())  // if r is not bufferized - use just as string_view
        bufferize();
    }

    basic_buffered_stringview<TElem>(const std::basic_string<TElem>& r)
      : base(r) {}

    sv::basic_string_view<TElem>& operator=(const sv::basic_string_view<TElem>& r) {
      return base::operator=(r);
    }

    sv::basic_string_view<TElem>& operator=(const basic_buffered_stringview<TElem>& r) {
      base::operator=(r);
      if (!r.buffer_.empty()) // if r is not bufferized - use just as string_view
        bufferize();
      return *this;
    }

    sv::basic_string_view<TElem>& operator=(const std::basic_string<TElem>& r) {
      return base::operator=(r);
    }

    basic_buffered_stringview<TElem>(const TElem * p) noexcept
      : base(p) {}

    basic_buffered_stringview<TElem>(const TElem * p, const size_t count)
      : base(p, count) {}

    void bufferize() {
#ifdef FLEXBIN_USE_BOOST
      buffer_.assign(data(), size());
#else
      buffer_ = *this;
#endif

      *this = buffer_;
    }

    const char * begin() const { return base::data();  }

  private:
    std::basic_string<TElem> buffer_;
  };

  typedef basic_buffered_stringview<char> buffered_string_view;

} // namespace flexbin