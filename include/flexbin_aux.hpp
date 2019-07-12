#pragma once

namespace flexbin
{

  // serializable class may not have fixed/required/optional/simplified section, 
  // so we need sfinae to detect it
  template<int N> struct creatable_non_zero { creatable_non_zero(int ) {}; char tmp[N]; };

  template<typename T>
  struct has_required_fields {
    template <typename C> static char test( creatable_non_zero<sizeof(&C::flexbin_serialize_required)> ) ;
    template <typename C> static long test(...);
    enum { yes = sizeof(test<T>( 0 )) == sizeof(char) };
  };

  template<typename T>
  struct has_optional_fields {
    template <typename C> static char test( creatable_non_zero<sizeof(&C::flexbin_serialize_optional)> ) ;
    template <typename C> static long test(...);
    enum { yes = sizeof(test<T>( 0 )) == sizeof(char) };
  };

  template<typename T>
  struct has_fixed_fields {
    template <typename C> static char test( creatable_non_zero<sizeof(&C::flexbin_serialize_fixed)> ) ;
    template <typename C> static long test(...);
    enum { yes = sizeof(test<T>( 0 )) == sizeof(char) };
  };

  template<typename T>
  struct has_simplified_fields {
    template <typename C> static char test( creatable_non_zero<sizeof(&C::flexbin_serialize_simplified)> ) ;
    template <typename C> static long test(...);
    enum { yes = sizeof(test<T>( 0 )) == sizeof(char) };
  };


//  template <typename T>
//  using is_vector = std::is_same<T, std::vector< typename T::value_type, typename T::allocator_type > >;
}

