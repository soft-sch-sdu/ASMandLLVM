cmake_minimum_required(VERSION 3.22)
project(2023spring)

set(CMAKE_CXX_STANDARD 14)

#LLVM configuration -1
set(ENV{LLVM_DIR} /home/leon/llvm15-release)
find_package(LLVM REQUIRED CONFIG)
message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")
include_directories(${LLVM_INCLUDE_DIRS})
separate_arguments(LLVM_DEFINITIONS_LIST NATIVE_COMMAND ${LLVM_DEFINITIONS})
add_definitions(${LLVM_DEFINITIONS_LIST})
#LLVM configuration -1

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}")
add_executable(2023spring main.cpp)

#LLVM configuration -2
    # Find the libraries that correspond to the LLVM components that we wish to use
llvm_map_components_to_libnames(llvm_libs support core irreader)
    # Link against LLVM libraries
target_link_libraries(2023spring ${llvm_libs})
#LLVM configuration -2