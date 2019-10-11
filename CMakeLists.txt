cmake_minimum_required(VERSION 2.8)
project(spike CXX)
cmake_policy(SET CMP0025 NEW)

# Download automatically, you can also just copy the conan.cmake file
if(NOT EXISTS "${CMAKE_BINARY_DIR}/conan.cmake")
   message(STATUS "Downloading conan.cmake from https://github.com/conan-io/cmake-conan")
   file(DOWNLOAD "https://raw.githubusercontent.com/conan-io/cmake-conan/v0.12/conan.cmake"
                 "${CMAKE_BINARY_DIR}/conan.cmake")
endif()

include(${CMAKE_BINARY_DIR}/conan.cmake)

conan_cmake_run(CONANFILE conanfile.py
                BASIC_SETUP KEEP_RPATHS CMAKE_TARGETS
                BUILD missing)

set(BOTAN_STREAM_SOURCES "botan_stream_helpers.hpp")
add_executable(spike_cpprest "spike_cpprest.cpp" ${BOTAN_STREAM_SOURCES})

target_link_libraries(spike_cpprest
    CONAN_PKG::boost CONAN_PKG::botan CONAN_PKG::cpprestsdk)
