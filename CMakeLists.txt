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
add_library(ptp_sigma
    src/utils/apex.cpp
    src/utils/log.cpp
    src/sigma/schema.cpp
    src/sigma/sigma_ptp.cpp
    src/ptp/usb_transport.cpp
    src/ptp/ptp.cpp
)

target_include_directories(ptp_sigma PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# target_compile_options(sigma_ptp PRIVATE
#     $<$<CXX_COMPILER_ID:GNU,Clang>:-Wall -Wextra -Wpedantic>
#     $<$<CXX_COMPILER_ID:MSVC>:/W4 /permissive->
# )

# libusb-1.0
find_package(PkgConfig QUIET)
pkg_check_modules(LIBUSB QUIET libusb-1.0)
target_link_libraries(ptp_sigma PUBLIC ${LIBUSB_LIBRARY})

# In sigma-driver/CMakeLists.txt
option(BUILD_EXAMPLES "Build example programs" ON)

if(BUILD_EXAMPLES)
    # ---- Example app (edit or add more) ----
    add_executable(sigma_capture_example
        examples/sigma_capture_example.cpp
        src/ptp/fake_transport.cpp
    )
    target_link_libraries(sigma_capture_example
        ptp_sigma
        ${LIBUSB_LIBRARIES}
    )
endif()

# In sigma-driver/CMakeLists.txt
option(BUILD_TESTS "Build tests" ON)

if(BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()
