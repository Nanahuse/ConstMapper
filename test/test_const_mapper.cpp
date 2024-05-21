#include <gtest/gtest.h>

#include "const_mapper.hpp"

TEST(TestConstMapper, tuple_index) {
  using tuple = std::tuple<std::string_view, int, std::uint8_t>;

  constexpr auto str_index = tuple_index_v<tuple, std::string_view>;
  constexpr auto int_index = tuple_index_v<tuple, int>;
  constexpr auto uint_index = tuple_index_v<tuple, std::uint8_t>;
  // constexpr auto uint_index = tuple_index_v<tuple, std::uint16_t>;  // fail.

  static_assert(str_index == 0);
  static_assert(int_index == 1);
  static_assert(uint_index == 2);
}

TEST(TestConstMapper, to_index) {
  constexpr auto map =
      const_mapper::ConstMapper<4, std::string_view, int, std::uint8_t>{
          {{{"value_0", 0, 0},
            {"value_1", -1, 1},
            {"value_2", -2, 2},
            {"value_3", -3, 3}}}};

  for (auto i = 0; i < 4; ++i) {
    auto string = "value_" + std::to_string(i);

    auto value_int = map.to<1, 0>(string);
    auto value_uint = map.to<2, 0>(string);
    auto value_str = map.to<0, 2>(i);

    EXPECT_EQ(value_int, -i);
    EXPECT_EQ(value_uint, i);
    EXPECT_EQ(value_str, string);
  }

  try {
    map.to<0, 1>(100);
    EXPECT_TRUE(false);
  } catch (const std::out_of_range& e) {
    // expected here
  } catch (const std::exception& e) {
    EXPECT_TRUE(false);
  }
}

TEST(TestConstMapper, to_type) {
  constexpr auto map =
      const_mapper::ConstMapper<4, std::string_view, int, std::uint8_t>{
          {{{"value_0", 0, 0},
            {"value_1", -1, 1},
            {"value_2", -2, 2},
            {"value_3", -3, 3}}}};

  for (auto i = 0; i < 4; ++i) {
    auto string = "value_" + std::to_string(i);

    auto value_int = map.to<int, std::string_view>(string);
    auto value_uint = map.to<std::uint8_t, std::string_view>(string);
    auto value_str = map.to<std::string_view, std::uint8_t>(i);

    EXPECT_EQ(value_int, -i);
    EXPECT_EQ(value_uint, i);
    EXPECT_EQ(value_str, string);
  }

  try {
    map.to<int, std::string_view>("");
    EXPECT_TRUE(false);
  } catch (const std::out_of_range& e) {
    // expected here
  } catch (const std::exception& e) {
    EXPECT_TRUE(false);
  }
}

TEST(TestConstMapper, pattern) {
  constexpr auto map =
      const_mapper::ConstMapper<4, std::string_view, int, std::uint8_t>{
          {{{"value_0", 0, 0},
            {"value_1", -1, 1},
            {"value_2", -2, 2},
            {"value_3", -3, 3}}}};

  for (auto i = 0; i < 4; ++i) {
    auto string = "value_" + std::to_string(i);

    auto value_str = map.pattern_to<std::string_view, int, std::uint8_t>(-i, i);

    EXPECT_EQ(value_str, string);
  }

  try {
    map.pattern_to<std::string_view, int, std::uint8_t>(-1, 0);
    EXPECT_TRUE(false);
  } catch (const std::out_of_range& e) {
    // expected here
  } catch (const std::exception& e) {
    EXPECT_TRUE(false);
  }
}

TEST(TestConstMapper, any) {
  using namespace const_mapper;
  constexpr auto map = ConstMapper<4, std::string_view, Anyable<int>>{
      {{{"value_2", 2}, {"value_3", 3}, {"value_any", {}}}}};

  for (auto i = 0; i < 4; ++i) {
    auto string = "value_" + std::to_string(i);

    auto value_str = map.to<std::string_view, Anyable<int>>(i);

    if (i < 2) {
      constexpr auto any_str = "value_any";
      EXPECT_EQ(value_str, any_str);
    } else {
      EXPECT_EQ(value_str, string);
    }
  }
}

int main(int argc, char** argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}