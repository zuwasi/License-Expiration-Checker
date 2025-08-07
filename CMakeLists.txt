cmake_minimum_required(VERSION 3.16)
project(LicenseExpirationChecker VERSION 1.0.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find required packages
find_package(PkgConfig REQUIRED)

# Add the executable
add_executable(license_checker
    src/main.cpp
    src/license_checker.cpp
    src/file_utils.cpp
    src/date_utils.cpp
)

# Include directories
target_include_directories(license_checker PRIVATE include)

# Compiler-specific options
if(APPLE)
    target_compile_definitions(license_checker PRIVATE PLATFORM_MAC=1)
    target_link_libraries(license_checker "-framework Foundation")
elseif(UNIX)
    target_compile_definitions(license_checker PRIVATE PLATFORM_LINUX=1)
elseif(WIN32)
    target_compile_definitions(license_checker PRIVATE PLATFORM_WINDOWS=1)
endif()

# Compiler flags
target_compile_options(license_checker PRIVATE
    $<$<CXX_COMPILER_ID:GNU,Clang>:-Wall -Wextra -pedantic>
    $<$<CXX_COMPILER_ID:MSVC>:/W4>
)

# Install target
install(TARGETS license_checker DESTINATION bin)
