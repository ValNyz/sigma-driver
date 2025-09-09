cmake_minimum_required(VERSION 3.16)
project(sigma_tests CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)
FetchContent_Declare(
  catch2
  GIT_REPOSITORY https://github.com/catchorg/Catch2.git
  GIT_TAG v3.5.4
)
FetchContent_MakeAvailable(catch2)

# ---- ptp/utils.h tests (header-only) ----
add_executable(unit_ptp_utils
  unit/ptp_utils_tests.cpp
)
target_include_directories(unit_ptp_utils PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_link_libraries(unit_ptp_utils PRIVATE Catch2::Catch2WithMain)

# ---- schema.cpp tests ----
add_executable(unit_schema
  unit/schema_tests.cpp
  ${CMAKE_SOURCE_DIR}/src/schema.cpp
  ${CMAKE_SOURCE_DIR}/src/apex.cpp
)
target_include_directories(unit_schema PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_link_libraries(unit_schema PRIVATE Catch2::Catch2WithMain)

# Coverage flags (optional)
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
  foreach(tgt IN ITEMS unit_ptp_utils unit_schema)
    target_compile_options(${tgt} PRIVATE -O0 -g --coverage)
    target_link_options(${tgt} PRIVATE --coverage)
  endforeach()
endif()

include(CTest)
add_test(NAME unit_ptp_utils COMMAND unit_ptp_utils)
add_test(NAME unit_schema     COMMAND unit_schema)

