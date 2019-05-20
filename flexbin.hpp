#pragma once
#include <iostream>
#include <tuple>
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
  struct type_traits
  {
    enum { code_ = 29 /* "object" field id */ };
    inline static size_t write( ostream& ostr, const T& ) { return 0;}

  };

  template<>
  struct type_traits<uint64_t>
  {
    enum { default_value_ = 0 };
    enum { code_ = 16 };

    inline static size_t write( ostream& ostr, const uint64_t& val) { 
      ostr.write(reinterpret_cast<const char*>(&val), sizeof(uint64_t));
      return sizeof(uint64_t); 
    }

  };

  template<>
  struct type_traits<uint32_t>
  {
    enum { default_value_ = 0 };
    enum { code_ = 8 };

    inline static size_t write( ostream& ostr, const uint32_t& val ) { 
      ostr.write(reinterpret_cast<const char*>(&val), sizeof(uint32_t));
      return sizeof(uint32_t); 
     }
  };

  template<>
  struct type_traits<uint8_t>
  {
    enum { default_value_ = 0 };
    enum { code_ = 2 };
    
    inline static size_t write( ostream& ostr, const uint8_t& val ) { 
      ostr.write(reinterpret_cast<const char*>(&val), sizeof(uint8_t));
      return sizeof(uint8_t); 
     }
  };

  template<>
  struct type_traits<std::string>
  {
    enum { code_ = 21 };

    inline static size_t write( ostream& ostr, const std::string& ) { 
      return 0; 
    }
  };

  template<typename T>
  struct store_strategy_fixed {
    size_t write( ostream& ostr, uint8_t field_id,  const T& value) {
      ostr << value;
      return 0;
    };
  };

  template <typename T, class Enabler = void>
  struct field_writer { 
    static size_t write( ostream& ostr, uint8_t field_id, const T& value) { 
      // field header
      uint8_t code = type_traits<T>::code_;
      ostr.write(reinterpret_cast<const char*>(&code), 1);
      ostr.write(reinterpret_cast<const char*>(&field_id), 1);

      // object header
      uint32_t object_size = 0; // TODO
      ostr.write(reinterpret_cast<const char*>(&object_size), 4);

      uint16_t class_id =  T::flexbin_class_id;
      ostr.write(reinterpret_cast<const char*>(&class_id), 2);

      // write fields
      ostr << value;

      const uint8_t end_marker = 255;
      ostr.write(reinterpret_cast<const char*>(&end_marker), 1);
      return 0; 
    }
  };

  template <>
  struct field_writer<std::string, void> { 
    static size_t write( ostream& ostr, uint8_t field_id, const std::string& value) { 
       return 0;
    }
  };

  template <typename T>
  struct field_writer<T, std::enable_if_t<std::is_fundamental<T>::value> > { 
    static size_t write( ostream& ostr, uint8_t field_id, const T& value) {  
      uint8_t code = type_traits<T>::code_;
      ostr.write(reinterpret_cast<const char*>(&code), 1);
      ostr.write(reinterpret_cast<const char*>(&field_id), 1);
      size_t nbytes = type_traits<T>::write(ostr, value);
      return nbytes + 2;
    }
  };

  template <typename T>
  size_t field_write(ostream& ostr, uint8_t field_id, const T& value) { 
    return field_writer<T>::write(ostr, field_id, value);
  }

  struct store_strategy_required {
    template<typename T>    
    static size_t write( ostream& ostr, uint8_t field_id, const T& value) {
        return field_write(ostr, field_id, value);
    };
  };

  template<typename T>
  struct store_strategy_optional{
    size_t write( ostream& ostr,  uint8_t field_id, const T& value) {
      if(type_traits<T> ::default_value_ == value) {
        return 0;
      }
      return write_object(ostr, field_id, value);
    };
  };

  template<typename T>
  struct store_strategy_simplified{
    size_t write( ostream& ostr,  uint8_t field_id, const T& value) {
      ostr << value;
      return 0;
    };
  };

  template<typename T> 
  std::basic_ostream<char>& ostream::operator<< (const T& obj)
  {
    {
      uint8_t field_id = 0;
      auto field_serializer_required = [this, &field_id](auto&&... args) { 
            ( ( store_strategy_required::write(*this, ++field_id,  args) ) , 
            ...
          );
      };

      std::apply( 
        field_serializer_required,
        obj.flexbin_serialize_required() 
      );
/*
      auto field_serializer_optional = [this, &field_id](auto&&... args) { 
            ( ( store_strategy_optional::write(*this, ++field_id,  args) ) , 
            ...
          );
      };

      std::apply( 
        field_serializer_optional,
        obj.flexbin_serialize_optional() 
      );*/
    }
    return *this;
  }

} // flexbin

#define FLEXBIN_CLASS_ID(id)  enum { flexbin_class_id = id };

#define FLEXBIN_SERIALIZE_FIXED(...) \
  auto flexbin_serialize_fixed() const { return std::forward_as_tuple(__VA_ARGS__); } 

#define FLEXBIN_SERIALIZE_REQUIRED(...) \
  auto flexbin_serialize_required() const { return std::forward_as_tuple(__VA_ARGS__); } 

#define FLEXBIN_SERIALIZE_OPTIONAL(...) \
  auto flexbin_serialize_optional() const { return std::forward_as_tuple(__VA_ARGS__); } 

#define FLEXBIN_SERIALIZE_SIMPLIFIED(...) \
  auto flexbin_serialize_simplified() const { return std::forward_as_tuple(__VA_ARGS__); } 
