cmake_minimum_required(VERSION 3.20)
project(const_mapper CXX)

set(CMAKE_CXX_STANDARD 17)

enable_testing()

include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
)
FetchContent_MakeAvailable(googletest)

find_package(GTest REQUIRED)

include_directories(${PROJECT_NAME}
    include
    ${GTEST_INCLUDE_DIRS}
)

add_executable(${PROJECT_NAME} test/test_const_mapper.cpp)
target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -Wpedantic -Werror)

target_link_libraries(${PROJECT_NAME} ${GTEST_LIBRARIES})
