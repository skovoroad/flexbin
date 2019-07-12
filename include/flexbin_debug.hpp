//#define FLEXBIN_DEBUG_LOG_ON

#ifdef FLEXBIN_DEBUG_LOG_ON
  #include <iostream>
  #define FLEXBIN_DEBUG_LOG( smth )  { std::cerr << smth << std::endl;}
#else
#define FLEXBIN_DEBUG_LOG(smth) //
#endif