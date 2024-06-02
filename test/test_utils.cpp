#include <gtest/gtest.h>

#include "const_mapper.hpp"

using namespace const_mapper;

TEST(TestUtils, tuple_index) {
  using tuple = std::tuple<std::string_view, int, std::uint8_t>;

  constexpr auto str_index = tuple_index<tuple, std::string_view>();
  constexpr auto int_index = tuple_index<tuple, int>();
  constexpr auto uint_index = tuple_index<tuple, std::uint8_t>();
  // constexpr auto uint_index = tuple_index_v<tuple, std::uint16_t>;  // must fail.

  static_assert(str_index == 0);
  static_assert(int_index == 1);
  static_assert(uint_index == 2);
  std::cout << "Test output" << std::endl;
}

TEST(TestUtils, un_tuple) {
  constexpr auto tmp0 = un_tuple_if_one_element(std::tuple(10));
  EXPECT_EQ(tmp0, 10);
  constexpr auto tmp1 = un_tuple_if_one_element(std::tuple(10, 20));
  EXPECT_EQ(tmp1, std::tuple(10, 20));
}
