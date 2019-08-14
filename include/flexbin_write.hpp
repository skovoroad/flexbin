#pragma once

#include <assert.h>
#include <stack>
#include <memory>
#include "flexbin_streams.hpp"
#include "flexbin_aux.hpp"

#include "flexbin_debug.hpp"

namespace flexbin
{
///////////////////
// Write methods selector for different field types: fundamental, std::string or struct/class
  template <typename T, class Enabler = void>
  struct field_writer { 

    static bool write( ostream& ostr, const T& value) { 
      ostr << value;
      return !ostr.failed() && ostr.good();  // FUUUUU
    }

    static bool pack( ostream& ostr, uint8_t field_id, const T& value) { 
      FLEXBIN_DEBUG_LOG("  pack object field: field_id " << (int)field_id << " class id " << T::flexbin_class_id)
      uint8_t code = type_traits<T>::code_;
      ostr.write(reinterpret_cast<const char*>(&code), 1);
      ostr.write(reinterpret_cast<const char*>(&field_id), 1);
      if (ostr.failed() || !ostr.good())
        return false;
      auto retval = write(ostr, value);
      FLEXBIN_DEBUG_LOG("  pack object field end: field_id " << (int)field_id << " class id " << T::flexbin_class_id)
      return retval;

    }
  };

  template <typename T>
  struct field_writer<std::basic_string<T>> {
  //struct field_writer<T, std::enable_if_t<std::is_base_of<std::string, T>::value> > {
    static bool write(ostream& ostr, const std::basic_string<T>& value) {
      FLEXBIN_DEBUG_LOG("write basic_string field:  value " << value)
      return type_traits<std::basic_string<T>>::write(ostr, value);
    }

    static bool pack(ostream& ostr, uint8_t field_id, const std::basic_string<T>& value) {
      FLEXBIN_DEBUG_LOG("pack basic_string field: field_id " << (int)field_id << " value ")
      uint8_t code = type_traits<std::string>::code_;
      ostr.write(reinterpret_cast<const char*>(&code), 1);
      ostr.write(reinterpret_cast<const char*>(&field_id), 1);
      if (ostr.failed() || !ostr.good())
        return false;
      return type_traits<std::basic_string<T>>::write(ostr, value) + 2;
    }
  };


  template <typename T>
  struct field_writer<flexbin::basic_buffered_stringview<T>> {
    static size_t write(ostream& ostr, const flexbin::basic_buffered_stringview<T>& value) {
      FLEXBIN_DEBUG_LOG("write buffered_string_view field:  value " << value)
      return type_traits<flexbin::basic_buffered_stringview<T>>::write(ostr, value);
    }

    static bool pack(ostream& ostr, uint8_t field_id, const flexbin::basic_buffered_stringview<T>& value) {
      FLEXBIN_DEBUG_LOG("pack buffered_string_view field: field_id " << (int)field_id << " value ")
      uint8_t code = type_traits<std::string>::code_;
      ostr.write(reinterpret_cast<const char*>(&code), 1);
      ostr.write(reinterpret_cast<const char*>(&field_id), 1);
      if (ostr.failed() || !ostr.good())
        return false;
      return type_traits<flexbin::basic_buffered_stringview<T>>::write(ostr, value) + 2;
    }
  };

  template <typename T>
  struct field_writer<T, std::enable_if_t<std::is_fundamental<T>::value> > { 

    static bool write( ostream& ostr, const T& value) {  
      FLEXBIN_DEBUG_LOG("write fundamental field:  type " << (int)type_traits<T>::code_ << " value "<< (int)value)
      return type_traits<T>::write(ostr, value);
    }

    static bool pack( ostream& ostr, uint8_t field_id, const T& value) {  
      FLEXBIN_DEBUG_LOG("pack fundamental type " << (int)type_traits<T>::code_ << " field_id" << (int) field_id << " value " << (int)value)
      return type_traits<T>::pack(ostr, field_id, value);
    }
  };

  template <typename T>
  struct field_writer<T, std::enable_if_t<std::is_enum<T>::value> > { 

    static bool write( ostream& ostr, const T& value) {
      FLEXBIN_DEBUG_LOG("write enum field:  value " << (int)value)
      return type_traits<T>::write(ostr, value);
    }

    static bool pack( ostream& ostr, uint8_t field_id, const T& value) {
      FLEXBIN_DEBUG_LOG("pack enum field: field_id" << field_id << " value " << (int) value)
      return type_traits<T>::pack(ostr, field_id, value);
    }
  };

  template <typename T>
  struct field_writer< std::vector<T> > {
    static bool write(ostream& ostr, const std::vector<T> & value) {
      size_t size = value.size();
      FLEXBIN_DEBUG_LOG("|write vector field: size" << size << " type: " << type_traits<T>::code_)
      ostr.write(reinterpret_cast<const char*>(&size), sizeof(size));
      if (ostr.failed() || !ostr.good())
        return false;
      for (const auto & v : value) {
        if(!field_writer<T>::write(ostr, v))
          return false;
      }
      FLEXBIN_DEBUG_LOG("|write vector " )
      return true;
    }

    static bool pack(ostream& ostr, uint8_t field_id, const std::vector<T>& value) {
      uint8_t code = type_traits<std::vector<T>>::code_;
      ostr.write(reinterpret_cast<const char*>(&code), 1);
      ostr.write(reinterpret_cast<const char*>(&field_id), 1);
      if (ostr.failed() || !ostr.good())
        return false;
      return write(ostr, value);
    }
  };

  template <typename T>
  struct field_writer< std::unordered_set<T> > {
    static bool write(ostream& ostr, const std::unordered_set<T> & value) {
      size_t size = value.size();
      ostr.write(reinterpret_cast<const char*>(&size), sizeof(size));
      if (ostr.failed() || !ostr.good())
        return false;
      FLEXBIN_DEBUG_LOG("|write unordered_set field: size" << size << " type: " << type_traits<T>::code_)
      for (const auto & v : value) {
        if(!field_writer<T>::write(ostr, v))
          return false;
      }
      FLEXBIN_DEBUG_LOG("|write unordered_set ")
        return size + sizeof(size_t);
    }

    static bool pack(ostream& ostr, uint8_t field_id, const std::unordered_set<T>& value) {
      uint8_t code = type_traits<std::unordered_set<T>>::code_;
      ostr.write(reinterpret_cast<const char*>(&code), 1);
      ostr.write(reinterpret_cast<const char*>(&field_id), 1);
      if (ostr.failed() || !ostr.good())
        return false;
      return write(ostr, value);
    }
  };

  template <typename TKey, typename TValue>
  struct field_writer< std::unordered_multimap<TKey, TValue> > {
    static bool write(ostream& ostr, const std::unordered_multimap<TKey, TValue> & value) {
      size_t size = value.size();
      ostr.write(reinterpret_cast<const char*>(&size), sizeof(size));
      if (ostr.failed() || !ostr.good())
        return false;
      FLEXBIN_DEBUG_LOG("|write unordered_multimap field: size" << size << " type key: " << type_traits<TKey>::code_
        << " type value: " << type_traits<TValue>::code_ )
        for (const auto & v : value) {
          if (!field_writer<TKey>::write(ostr, v.first))
            return false;
          if (!field_writer<TValue>::write(ostr, v.second))
            return false;
        }
      FLEXBIN_DEBUG_LOG("|write unordered_multimap ")
        return size + sizeof(size_t);
    }

    static bool pack(ostream& ostr, uint8_t field_id, const std::unordered_multimap<TKey, TValue>& value) {
      uint8_t code = type_traits<std::unordered_multimap<TKey, TValue>>::code_;
      ostr.write(reinterpret_cast<const char*>(&code), 1);
      ostr.write(reinterpret_cast<const char*>(&field_id), 1);
      if (ostr.failed() || !ostr.good())
        return false;
      return write(ostr, value);
    }
  };


  template <typename T, typename TDeleter>
  struct field_writer< std::unique_ptr<T, TDeleter> > {
    static bool write(ostream& ostr, const std::unique_ptr<T, TDeleter> & value) {
      FLEXBIN_DEBUG_LOG("write unique_ptr field: type: " << type_traits<T>::code_)
      return field_writer<T>::write(ostr, *value);
    }

    static bool pack(ostream& ostr, uint8_t field_id, const std::unique_ptr<T, TDeleter>& value) {
      FLEXBIN_DEBUG_LOG("pack unique_ptr field: field id " << (int)field_id << " type: " << type_traits<T>::code_)
      return field_writer<T>::pack(ostr, field_id, *value);
    }
  };

  template <typename T>
  struct field_writer< std::shared_ptr<T> > {
    static bool write(ostream& ostr, const std::shared_ptr<T> & value) {
      FLEXBIN_DEBUG_LOG("write shared_ptr field: field id type: " << type_traits<T>::code_)
      return field_writer<T>::write(ostr, *value);
    }

    static bool pack(ostream& ostr, uint8_t field_id, const std::shared_ptr<T>& value) {
      FLEXBIN_DEBUG_LOG("pack shared_ptr field: field id " << (int)field_id << " type: " << type_traits<T>::code_)
      return field_writer<T>::pack(ostr, field_id, *value);
    }
  };


//////////////////////////
// Write strategies: fixes, optional, required, simplified
  template<typename T>    
  inline bool write_fixed( ostream& ostr, const T& value) {
    return field_writer<T>::write(ostr, value);
  };

  template<typename T>    
  inline bool write_required( ostream& ostr, uint8_t field_id, const T& value) {
    FLEXBIN_DEBUG_LOG("-- write required field: field id " << (int)field_id )
    auto retval = field_writer<T>::pack(ostr, field_id, value);
    return retval;
  };

  template<typename T>    
  inline bool write_optional( ostream& ostr,  uint8_t field_id, const T& value) {
    if(type_traits<T> ::default_value_ == value) 
     return true;
    return  field_writer<T>::pack(ostr, field_id, value);
  };

  template<typename T>    
  inline bool write_simplified( ostream& ostr,  uint8_t field_id, const T& value) {
    uint8_t code = type_traits<T>::code_;
    ostr.write(reinterpret_cast<const char*>(&code), 1);
    ostr.write(reinterpret_cast<const char*>(&field_id), 1);
    if (ostr.failed() || !ostr.good())
      return false;
    return type_traits<T>::write(ostr, value) + 2;
  };

///////////////////////////
//

  template<typename T>
  struct flexbin_writer
  {
    uint8_t field_id = 0;
    std::stack<std::streamsize> object_size_pos;
    bool success_ = true;

    constexpr static bool required_fields_exists = (has_required_fields<T>::yes != 0);
    constexpr static bool optional_fields_exists = (has_optional_fields<T>::yes != 0);
    constexpr static bool fixed_fields_exists    = (has_fixed_fields<T>::yes != 0);
    constexpr static bool simplified_fields_exists = (has_simplified_fields<T>::yes != 0);

    // object header
    bool write_header(ostream& ostr, const T& obj)
    {
      uint32_t object_size = 0; 
      //int size_pos = ostr.tellp();
      std::streamsize size_pos = ostr.written_count();

      if (size_pos < 0)
        throw(std::ios_base::failure("inapproriate type of buffer doesn't support rewind "));
      object_size_pos.push(size_pos);

      ostr.write(reinterpret_cast<const char*>(&object_size), 4);

      uint16_t class_id = T::flexbin_class_id;
      ostr.write(reinterpret_cast<const char*>(&class_id), 2);
      FLEXBIN_DEBUG_LOG("== write object header: class id " << (int)T::flexbin_class_id << " size_pos " << size_pos)
      return true;
    }

    bool write_bottom(ostream& ostr, const T& obj)
    {
      ostr.write(reinterpret_cast<const char*>(&end_marker), 1);

      std::streamsize object_end_pos = ostr.written_count();
      assert(!object_size_pos.empty());
      std::streamsize size_pos = object_size_pos.top();

      if(object_end_pos <= size_pos) {
        FLEXBIN_DEBUG_LOG("ERROR write bottom, bad size pos: class id " << (int)T::flexbin_class_id << 
          " size_pos " << size_pos << " object_end_pos " << object_end_pos )
        return false;
      }
      auto object_size = static_cast<uint32_t>(object_end_pos - size_pos);      
      FLEXBIN_DEBUG_LOG("== write object bottom: class id " << (int)T::flexbin_class_id << " size_pos " << size_pos
          << " object_size " << object_size
        )
      ostr.write_direct(size_pos, reinterpret_cast<const char *> (&object_size), sizeof(object_size));
      object_size_pos.pop();
      return true;
    }

#ifdef _MSC_VER 
    // fuck this shit! MSVC cannot to proper template arguments deducing
    bool write_required_fields(ostream& ostr, const T& obj) {
      __if_exists(T::flexbin_serialize_required)
      {
        auto field_serializer_required = [this, &ostr](auto&&... args) {
          ((success_ = success_ && write_required(ostr, ++field_id, args)), ...);
        };
        std::apply(field_serializer_required, obj.flexbin_serialize_required());
      }
      return success_ ;
    }

    bool write_optional_fields(ostream& ostr, const T& obj) {
      __if_exists(T::flexbin_serialize_optional)
      {
        auto field_serializer_optional = [this, &ostr](auto&&... args) {
          ((success_  = success_ && write_optional(ostr, ++field_id, args)), ...);
        };
        std::apply(field_serializer_optional, obj.flexbin_serialize_optional());
      }
      return success_;
    }

    // Write fixed fields if exists
    bool write_fixed_fields(ostream& ostr, const T& obj) {
      __if_exists(T::flexbin_serialize_fixed)
      {
        auto field_serializer_fixed = [this, &ostr](auto&&... args) {
          ((success_ = success_ && write_fixed(ostr, args)), ...);
        };
        std::apply(field_serializer_fixed, obj.flexbin_serialize_fixed());
      }
      return success_;
    }

    bool write_simplified_fields(ostream& ostr, const T& obj) {
      __if_exists(T::flexbin_serialize_simplified)
      {
        auto field_serializer_simplified = [this, &ostr](auto&&... args) {
          ((success_  = success_ && write_simplified(ostr, ++field_id, args)), ...);
        };
        std::apply(field_serializer_simplified, obj.flexbin_serialize_simplified());
      }
      return success_;
    }
#else
   // Write required fields if exists
    template<typename C = bool >
    typename std::enable_if< required_fields_exists , C>::type
    write_required_fields(ostream& ostr, const T& obj) {
      auto field_serializer_required = [this, &ostr](auto&&... args) { 
          ( (success_  = success_ && write_required(ostr, ++field_id,  args)) , ... );
        };
      std::apply( field_serializer_required, obj.flexbin_serialize_required());
      return success_;
    }

   // Write required fields if not exists
    template<typename C = bool>
    typename std::enable_if< !required_fields_exists , C >::type
    write_required_fields( ostream& ostr, const T& obj ) {
      return success_;
    }

   // Write optional fields if exists
    template<typename C = bool>
    typename std::enable_if< optional_fields_exists , C>::type
    write_optional_fields(ostream& ostr, const T& obj) {
      auto field_serializer_optional = [this, &ostr](auto&&... args) { 
          ( (success_  = success_ && write_optional(ostr, ++field_id,  args)) , ... );
        };
      std::apply( field_serializer_optional, obj.flexbin_serialize_optional());
      return success_;
    }

   // Write optional fields if not exists
    template<typename C = bool>
    typename std::enable_if< !optional_fields_exists , C >::type
    write_optional_fields( ostream& ostr, const T& obj ) {
      return true;
    }

   // Write fixed fields if exists
    template<typename C = bool>
    typename std::enable_if< fixed_fields_exists , C>::type
    write_fixed_fields(ostream& ostr, const T& obj) {
      auto field_serializer_fixed = [this, &ostr](auto&&... args) { 
          ( (success_ = success_ && write_fixed(ostr, args)) , ... );
        };
      std::apply( field_serializer_fixed, obj.flexbin_serialize_fixed());
      return success_;
    }

   // Write fixed fields if not exists
    template<typename C = bool>
    typename std::enable_if< !fixed_fields_exists , C >::type
    write_fixed_fields( ostream& ostr, const T& obj ) {
      return success_;
    }

   // Write simplified fields if exists
    template<typename C = bool>
    typename std::enable_if< simplified_fields_exists , C>::type
    write_simplified_fields(ostream& ostr, const T& obj) {
      auto field_serializer_simplified = [this, &ostr](auto&&... args) { 
          ( (success_  = success_ && write_simplified(ostr, ++field_id,  args)) , ... );
        };
      std::apply( field_serializer_simplified, obj.flexbin_serialize_simplified());
      return success_;
    }

   // Write simplified fields if exists
    template<typename C = bool>
    typename std::enable_if< !simplified_fields_exists , C >::type
    write_simplified_fields( ostream& ostr, const T& obj ) {
      return true;
    }
#endif
  };


} //namespace flexbin