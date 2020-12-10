cmake_minimum_required(VERSION 3.4.3)
project(betterIR)
set(CMAKE_CXX_STANDARD 14 CACHE STRING "")

# ensure that LLVMConfig.cmake is on search path
set(LLVM_INS "/usr/lib/llvm-11/" CACHE PATH "LLVM installation directory")
set(LLVM_CONFIG_DIR "${LLVM_INS}lib/cmake/llvm/")
list(APPEND CMAKE_PREFIX_PATH "${LLVM_CMAKE_CONFIG_DIR}")

# load LLVMConfig.cmake
find_package(LLVM 11.0.0 REQUIRED CONFIG)
include_directories(${LLVM_INCLUDE_DIRS})
if (NOT LLVM_ENABLE_RTTI)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-rtti")
endif()

# define pass directories here
add_subdirectory(lib)
