#include <gtest/gtest.h>

#include <random>

#include "const_mapper.hpp"

using namespace const_mapper;

namespace {
static constexpr auto loop = 10000000;
}

TEST(Performance, ref_std_unordered_map) {
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_int_distribution distrib(0, 9);

  const auto map = std::unordered_map<std::uint8_t, int>{
      {0, 0}, {1, -1}, {2, -2}, {3, -3}, {4, -4}, {5, -5}, {6, -6}, {7, -7}, {8, -8}, {9, -9},
  };

  for (auto j = 0; j < loop; ++j) {
    auto i = distrib(gen);
    auto expected = -i;
    auto value = map.at(static_cast<std::uint8_t>(i));
    EXPECT_EQ(value, expected);
  }
}

TEST(Performance, ref_std_map) {
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_int_distribution distrib(0, 9);

  const auto map = std::map<std::uint8_t, int>{
      {0, 0}, {1, -1}, {2, -2}, {3, -3}, {4, -4}, {5, -5}, {6, -6}, {7, -7}, {8, -8}, {9, -9},
  };

  for (auto j = 0; j < loop; ++j) {
    auto i = distrib(gen);
    auto expected = -i;
    auto value = map.at(static_cast<std::uint8_t>(i));
    EXPECT_EQ(value, expected);
  }
}

TEST(Performance, const_mapper_to) {
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_int_distribution distrib(0, 9);

  constexpr auto map = ConstMapper<10, std::uint8_t, int>{
      {{{0, 0}, {1, -1}, {2, -2}, {3, -3}, {4, -4}, {5, -5}, {6, -6}, {7, -7}, {8, -8}, {9, -9}}}};

  for (auto j = 0; j < loop; ++j) {
    auto i = distrib(gen);
    auto expected = -i;
    auto value = map.to<int, std::uint8_t>(static_cast<std::uint8_t>(i));
    EXPECT_EQ(value, expected);
  }
}

TEST(Performance, const_mapper_pattern_match) {
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_int_distribution distrib(0, 9);

  constexpr auto map = ConstMapper<10, std::uint8_t, int>{
      {{{0, 0}, {1, -1}, {2, -2}, {3, -3}, {4, -4}, {5, -5}, {6, -6}, {7, -7}, {8, -8}, {9, -9}}}};

  for (auto j = 0; j < loop; ++j) {
    auto i = distrib(gen);
    auto expected = -i;
    auto key = static_cast<std::uint8_t>(i);
    auto value = map.pattern_match(std::make_tuple(key, Result{}));
    EXPECT_EQ(value, expected);
  }
}

TEST(Performance, const_mapper_pattern_match_pick_up_2_values) {
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_int_distribution distrib(0, 9);

  constexpr auto map = ConstMapper<10, std::uint8_t, std::uint16_t, int>{{{
      {0, 0, 0},
      {1, 2, -1},
      {2, 4, -2},
      {3, 6, -3},
      {4, 8, -4},
      {5, 10, -5},
      {6, 12, -6},
      {7, 14, -7},
      {8, 16, -8},
      {9, 18, -9},
  }}};

  for (auto j = 0; j < loop; ++j) {
    auto i = distrib(gen);
    auto expected = std::make_tuple<std::uint16_t, int>(static_cast<std::uint16_t>(2 * i), -i);
    auto key = static_cast<std::uint8_t>(i);
    auto value = map.pattern_match(std::make_tuple(key, Result{}, Result{}));
    EXPECT_EQ(value, expected);
  }
}
