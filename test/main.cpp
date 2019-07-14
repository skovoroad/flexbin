#include "gtest/gtest.h"

int main(int argc, char** argv)
{
//  string, stirngview, vector, substruct, enum, shared_ptr, unique_ptr
      std::cout << "uint64_t code: " <<  typeid(uint64_t(0)).name() << std::endl;
      std::cout << "uint32_t code: " <<  typeid(uint32_t(0)).name() << std::endl;
      std::cout << "uint16_t code: " <<  typeid(uint16_t(0)).name() << std::endl;
      std::cout << "uint8_t code: " <<  typeid(uint8_t(0)).name() << std::endl;
      std::cout << "int64_t code: " <<  typeid(int64_t(0)).name() << std::endl;
      std::cout << "int32_t code: " <<  typeid(int32_t(0)).name() << std::endl;
      std::cout << "int16_t code: " <<  typeid(int16_t(0)).name() << std::endl;
      std::cout << "int8_t code: " <<  typeid(int8_t(0)).name() << std::endl;

   ::testing::InitGoogleTest(&argc, argv);
   return RUN_ALL_TESTS();
}