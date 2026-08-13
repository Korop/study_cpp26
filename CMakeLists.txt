cmake_minimum_required(VERSION 3.20)

project(InterviewCpp VERSION 1.0.0 LANGUAGES CXX)

# Require C++23, fallback to 20 if compiler does not support it
# But we'll try to set 26 if available
set(CMAKE_CXX_STANDARD 26)
set(CMAKE_CXX_STANDARD_REQUIRED OFF) 
set(CMAKE_CXX_EXTENSIONS OFF)

# Common compiler flags for MSVC, GCC, Clang
if(MSVC)
    add_compile_options(/W4 /permissive- /EHsc /Zc:__cplusplus)
else()
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# Find macOS SDK if running on Apple
if(APPLE)
    execute_process(
        COMMAND xcrun --show-sdk-path
        OUTPUT_VARIABLE INTERVIEW_MACOS_SDK
        OUTPUT_STRIP_TRAILING_WHITESPACE
    )
    if(EXISTS "${INTERVIEW_MACOS_SDK}/usr/include/c++/v1/iostream")
        include_directories(SYSTEM "${INTERVIEW_MACOS_SDK}/usr/include/c++/v1")
    endif()
endif()

# We can optionally include Threads if any task needs it
find_package(Threads REQUIRED)

# Include the tasks directory where dynamic globbing happens
add_subdirectory(tasks)
