cmake_minimum_required(VERSION 3.13)

project(mul2shift)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 17)

find_package(LLVM
    REQUIRED
    CONFIG
    HINTS "$ENV{LLVM15}/lib/cmake"
    NO_DEFAULT_PATH
)

add_definitions(${LLVM_DEFINITIONS})
include_directories(${LLVM_INCLUDE_DIRS})
link_directories(${LLVM_LIBRARY_DIR})

list(APPEND CMAKE_MODULE_PATH "${LLVM_CMAKE_DIR}")
include(AddLLVM)

add_llvm_pass_plugin(Mul2Shift Mul2Shift.cpp)
