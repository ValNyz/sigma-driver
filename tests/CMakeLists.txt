cmake_minimum_required(VERSION 3.16)
project(sigma_ptp_tests LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(CTest)

include(FetchContent)
FetchContent_Declare(
  Catch2
  GIT_REPOSITORY https://github.com/catchorg/Catch2.git
  GIT_TAG v3.5.2
)
FetchContent_MakeAvailable(Catch2)

add_executable(cam_tests
  integration/cam_tests.cpp
)

target_include_directories(cam_tests PRIVATE
  ${CMAKE_SOURCE_DIR}/include
  ${CMAKE_SOURCE_DIR}/src
  ${CMAKE_SOURCE_DIR}/tests
)

target_compile_definitions(cam_tests PRIVATE
  TEST_SRCDIR="${CMAKE_CURRENT_SOURCE_DIR}"
)

target_link_libraries(cam_tests PRIVATE
  ptp_sigma
  Catch2::Catch2WithMain
)

add_executable(apex_tests
  unit/apex_tests.cpp
)

target_include_directories(apex_tests PRIVATE
  ${CMAKE_SOURCE_DIR}/include
  ${CMAKE_SOURCE_DIR}/src
)

target_compile_definitions(apex_tests PRIVATE
  TEST_SRCDIR="${CMAKE_CURRENT_SOURCE_DIR}"
)

target_link_libraries(apex_tests PRIVATE
  ptp_sigma
  Catch2::Catch2WithMain
)

add_executable(schema_tests
  unit/schema_tests.cpp
)

target_include_directories(schema_tests PRIVATE
  ${CMAKE_SOURCE_DIR}/include
  ${CMAKE_SOURCE_DIR}/src
)

target_compile_definitions(schema_tests PRIVATE
  TEST_SRCDIR="${CMAKE_CURRENT_SOURCE_DIR}"
)

target_link_libraries(schema_tests PRIVATE
  ptp_sigma
  Catch2::Catch2WithMain
)

add_test(NAME cam COMMAND cam_tests)
add_test(NAME apex COMMAND apex_tests)
add_test(NAME schema COMMAND schema_tests)
