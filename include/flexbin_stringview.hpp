#pragma once
#include <string_view>


namespace flexbin {

  template<typename TElem>
  class basic_buffered_stringview : public std::basic_string_view<TElem> {
  public:
    typedef std::basic_string_view<TElem> base;

    basic_buffered_stringview<TElem>(const basic_string_view<TElem>& r)
      : base(r) {}

    basic_string_view<TElem>& operator=(const basic_string_view<TElem>& r) {
      return base::operator=(r);
    }

    basic_buffered_stringview<TElem>(const TElem * p) noexcept
      : base(p) {}

    basic_buffered_stringview<TElem>(const TElem * p, const size_t count)
      : base(p, count) {}

    void bufferize() {
      buffer_ = *this;
      *this = buffer_;
    }

  private:
    std::string buffer_;
  };

  typedef basic_buffered_stringview<char> buffered_string_view;

} // namespace flexbin