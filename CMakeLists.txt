cmake_minimum_required(VERSION 3.16)
project(indi_sigma CXX C)

# C++ + debug defaults
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug CACHE STRING "Build type" FORCE)
endif()

# ---- Library: core + vendor ----
add_library(libptp_sigma
    apex.cpp
    log.cpp
    schema.cpp
    sigma_ptp.cpp
    ptp/usb_transport.cpp
    ptp/ptp.cpp
)

target_include_directories(libptp_sigma PUBLIC
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
    target_link_libraries(libptp_sigma PUBLIC PkgConfig::LIBUSB)
else()
    find_library(LIBUSB1_LIBRARY NAMES usb-1.0 libusb1-1.0)
    if(NOT LIBUSB1_LIBRARY)
        message(FATAL_ERROR "libusb1-1.0 not found. Install it (e.g. apt install libusb-1.0-0-dev).")
    endif()
    target_link_libraries(libptp_sigma PUBLIC ${LIBUSB1_LIBRARY})
endif()

# ---- Example app (edit or add more) ----
add_executable(sigma_camera_test
    sigma_camera_test.cpp
)

# target_include_directories(sigma_camera_test PUBLIC
#     ${CMAKE_CURRENT_SOURCE_DIR}
# )

target_link_libraries(sigma_camera_test PRIVATE libptp_sigma)
