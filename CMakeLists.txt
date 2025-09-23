cmake_minimum_required(VERSION 3.16)
project(indi_sigma CXX C)

include(GNUInstallDirs)

# C++ + debug defaults
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Debug CACHE STRING "Build type" FORCE)
endif()

# Use pkg-config for dependencies
find_package(PkgConfig REQUIRED)
pkg_check_modules(LIBUSB REQUIRED IMPORTED_TARGET libusb-1.0)

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
  ${LIBUSB_INCLUDE_DIRS}
)
target_link_libraries(ptp_sigma PUBLIC
  PkgConfig::LIBUSB
)

# Install the library
install(TARGETS ptp_sigma
  EXPORT ptp_sigmaTargets
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
  INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

# Install public headers
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/include/
  DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

# In sigma-driver/CMakeLists.txt
option(BUILD_EXAMPLES "Build example programs" ON)

if(BUILD_EXAMPLES)
  # ---- Example app (edit or add more) ----
  add_executable(sigma_capture_example
    examples/sigma_capture_example.cpp
  )
  target_link_libraries(sigma_capture_example
    ptp_sigma
    ${LIBUSB_LIBRARIES}
  )
  install(TARGETS sigma_capture_example
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})


  add_executable(sigma_bulb_example
    examples/sigma_bulb_example.cpp
  )
  target_link_libraries(sigma_bulb_example
    ptp_sigma
    ${LIBUSB_LIBRARIES}
  )
  install(TARGETS sigma_bulb_example
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
endif()

# In sigma-driver/CMakeLists.txt
option(BUILD_TESTS "Build tests" ON)

if(BUILD_TESTS)
  enable_testing()
  add_subdirectory(tests)
endif()
