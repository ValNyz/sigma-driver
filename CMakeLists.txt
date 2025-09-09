cmake_minimum_required(VERSION 3.16)
project(indi_sigma CXX C)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake_modules/")
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../cmake_modules/")
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../../cmake_modules/")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# C++ + debug defaults
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug CACHE STRING "Build type" FORCE)
endif()

option(ENABLE_ASAN "Enable AddressSanitizer" ON)
option(ENABLE_UBSAN "Enable UndefinedBehaviorSanitizer" OFF)

# ---- Library: core + vendor ----
add_library(sigma_ptp
    apex.cpp
    log.cpp
    schema.cpp
    sigma_ptp.cpp
    ptp/usb_transport.cpp
    ptp/ptp.cpp
)

target_include_directories(sigma_ptp PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

# target_compile_options(sigma_ptp PRIVATE
#     $<$<CXX_COMPILER_ID:GNU,Clang>:-Wall -Wextra -Wpedantic>
#     $<$<CXX_COMPILER_ID:MSVC>:/W4 /permissive->
# )

# libusb-1.0
find_package(PkgConfig QUIET)
if(PkgConfig_FOUND)
    pkg_check_modules(LIBUSB1 QUIET libusb1-1.0)
endif()
if(LIBUSB_FOUND)
    target_link_libraries(sigma_ptp PUBLIC PkgConfig::LIBUSB)
else()
    find_library(LIBUSB1_LIBRARY NAMES usb-1.0 libusb1-1.0)
    if(NOT LIBUSB1_LIBRARY)
        message(FATAL_ERROR "libusb1-1.0 not found. Install it (e.g. apt install libusb-1.0-0-dev).")
    endif()
    target_link_libraries(sigma_ptp PUBLIC ${LIBUSB1_LIBRARY})
endif()

# ---- Example app (edit or add more) ----
add_executable(sigma_camera_test
    sigma_camera_test.cpp
)

# target_include_directories(sigma_camera_test PUBLIC
#     ${CMAKE_CURRENT_SOURCE_DIR}
# )

target_link_libraries(sigma_camera_test PRIVATE sigma_ptp)