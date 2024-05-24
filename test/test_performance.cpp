#include <gtest/gtest.h>

#include "const_mapper.hpp"

using namespace const_mapper;

TEST(Performance, std_unordered_map) {
  const auto map = std::unordered_map<std::uint8_t, int>{
      {0, 0}, {1, -1}, {2, -2}, {3, -3}, {4, -4}, {5, -5}, {6, -6}, {7, -7}, {8, -8}, {9, -9},
  };

  for (auto j = 0; j < 1000000; ++j) {
    for (int i = 0; i < 10; ++i) {
      auto expected = -i;
      auto value = map.at(static_cast<std::uint8_t>(i));
      EXPECT_EQ(value, expected);
    }
  }
}

TEST(Performance, std_map) {
  const auto map = std::map<std::uint8_t, int>{
      {0, 0}, {1, -1}, {2, -2}, {3, -3}, {4, -4}, {5, -5}, {6, -6}, {7, -7}, {8, -8}, {9, -9},
  };

  for (auto j = 0; j < 1000000; ++j) {
    for (int i = 0; i < 10; ++i) {
      auto expected = -i;
      auto value = map.at(static_cast<std::uint8_t>(i));
      EXPECT_EQ(value, expected);
    }
  }
}

TEST(Performance, const_mapper_to) {
  constexpr auto map = ConstMapper<10, std::uint8_t, int>{
      {{{0, 0}, {1, -1}, {2, -2}, {3, -3}, {4, -4}, {5, -5}, {6, -6}, {7, -7}, {8, -8}, {9, -9}}}};

  for (auto j = 0; j < 1000000; ++j) {
    for (int i = 0; i < 10; ++i) {
      auto expected = -i;
      auto value = map.to<int, std::uint8_t>(static_cast<std::uint8_t>(i));
      EXPECT_EQ(value, expected);
    }
  }
}
