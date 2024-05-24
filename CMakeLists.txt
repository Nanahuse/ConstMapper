cmake_minimum_required(VERSION 3.20)
project(const_mapper CXX)

set(CMAKE_C_COMPILER "/usr/bin/clang")
set(CMAKE_CXX_COMPILER "/usr/bin/clang++")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wpedantic")
set(CMAKE_CXX_STANDARD 17)

enable_testing()


find_package(GTest REQUIRED)

include_directories(${PROJECT_NAME}
    include
    ${GTEST_INCLUDE_DIRS}
)

add_executable(${PROJECT_NAME}
  test/main.cpp 
  test/test_const_mapper.cpp
  test/test_performance.cpp
  )
target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -Wpedantic -Werror)

target_link_libraries(${PROJECT_NAME} ${GTEST_LIBRARIES})
