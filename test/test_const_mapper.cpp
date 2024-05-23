#include <gtest/gtest.h>

#include "const_mapper.hpp"

using namespace const_mapper;

TEST(TestConstMapper, tuple_index) {
  using tuple = std::tuple<std::string_view, int, std::uint8_t>;

  constexpr auto str_index = tuple_index_v<tuple, std::string_view>;
  constexpr auto int_index = tuple_index_v<tuple, int>;
  constexpr auto uint_index = tuple_index_v<tuple, std::uint8_t>;
  // constexpr auto uint_index = tuple_index_v<tuple, std::uint16_t>;  // must fail.

  static_assert(str_index == 0);
  static_assert(int_index == 1);
  static_assert(uint_index == 2);
}

TEST(TestConstMapper, un_tuple) {
  constexpr auto tmp0 = un_tuple_if_one(std::tuple(10));
  EXPECT_EQ(tmp0, 10);
  constexpr auto tmp1 = un_tuple_if_one(std::tuple(10, 20));
  EXPECT_EQ(tmp1, std::tuple(10, 20));
}

TEST(TestConstMapper, to_index) {
  constexpr auto map = ConstMapper<4, std::string_view, int, std::uint8_t>{{{
      {"value_0", 0, 0},
      {"value_1", -1, 1},
      {"value_2", -2, 2},
      {"value_3", -3, 3},
  }}};

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
  } catch (const std::out_of_range &e) {
    // expected here
  } catch (const std::exception &e) {
    EXPECT_TRUE(false);
  }
}

TEST(TestConstMapper, to_type) {
  constexpr auto map = ConstMapper<4, std::string_view, int, std::uint8_t>{{{
      {"value_0", 0, 0},
      {"value_1", -1, 1},
      {"value_2", -2, 2},
      {"value_3", -3, 3},
  }}};

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
  } catch (const std::out_of_range &e) {
    // expected here
  } catch (const std::exception &e) {
    EXPECT_TRUE(false);
  }
}

TEST(TestConstMapper, pattern) {
  constexpr auto map = ConstMapper<4, std::string_view, int, std::uint8_t>{{{
      {"value_0", 0, 0},
      {"value_1", -1, 1},
      {"value_2", -2, 2},
      {"value_3", -3, 3},
  }}};

  for (auto i = 0; i < 4; ++i) {
    auto string = "value_" + std::to_string(i);

    auto value_str = map.pattern_to<std::string_view, int, std::uint8_t>(-i, i);

    EXPECT_EQ(value_str, string);
  }

  try {
    map.pattern_to<std::string_view, int, std::uint8_t>(-1, 0);
    EXPECT_TRUE(false);
  } catch (const std::out_of_range &e) {
    // expected here
  } catch (const std::exception &e) {
    EXPECT_TRUE(false);
  }
}

TEST(TestConstMapper, pattern_tuple) {
  constexpr auto map = ConstMapper<6, std::string_view, Anyable<int>, Anyable<std::uint8_t>>{{{
      {"value_0", 0, 0},
      {"value_1", -1, 1},
      {"value_2", -2, 2},
      {"value_3", -3, 3},
      {"value_4", -4, {}},
      {"value_any", {}, {}},
  }}};

  {
    constexpr auto value_str = map.pattern_match(std::tuple<Result, int, std::uint8_t>({}, 0, 0));
    constexpr auto expected = "value_0";
    EXPECT_EQ(value_str, expected);
  }
  {
    constexpr auto value_str = map.pattern_match(std::tuple<Result, int, std::uint8_t>({}, -2, 2));
    constexpr auto expected = "value_2";
    EXPECT_EQ(value_str, expected);
  }
  {
    constexpr auto value_str = map.pattern_match(std::tuple<Result, int, std::uint8_t>({}, -4, 2));
    constexpr auto expected = "value_4";
    EXPECT_EQ(value_str, expected);
  }
  {
    constexpr auto value_str = map.pattern_match(std::tuple<Result, Ignore, std::uint8_t>({}, {}, 2));
    constexpr auto expected = "value_2";
    EXPECT_EQ(value_str, expected);
  }
  {
    constexpr auto value_str = map.pattern_match(std::tuple<Result, int, std::uint8_t>({}, -1, 2));
    constexpr auto expected = "value_any";
    EXPECT_EQ(value_str, expected);
  }
  {
    constexpr auto value = map.pattern_match(std::tuple<Result, Result, std::uint8_t>({}, {}, 2));
    constexpr auto expected = std::tuple<std::string_view, Anyable<int>>("value_2", -2);
    EXPECT_EQ(value, expected);
  }
}

TEST(TestConstMapper, any) {
  using namespace const_mapper;
  constexpr auto map = ConstMapper<4, std::string_view, Anyable<int>>{{{
      {"value_2", 2},
      {"value_3", 3},
      {"value_any", {}},
  }}};

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

TEST(TestConstMapper, range) {
  using namespace const_mapper;
  constexpr auto map = ConstMapper<6, std::string_view, Range<int>>{{{
      {"less 2", {CompareType::LessThan, 2}},
      {"less equal 2", {CompareType::LessEqual, 2}},
      {"equal 3", {CompareType::Equal, 3}},
      {"larger 5", {CompareType::LargerThan, 5}},
      {"larger equal 5", {CompareType::LargerEqual, 5}},
      {"any", {}},
  }}};

  {
    constexpr auto value_str = map.to<std::string_view, Range<int>>(1);
    constexpr auto expected = "less 2";
    EXPECT_EQ(value_str, expected);
  }
  {
    constexpr auto value_str = map.to<std::string_view, Range<int>>(2);
    constexpr auto expected = "less equal 2";
    EXPECT_EQ(value_str, expected);
  }
  {
    constexpr auto value_str = map.to<std::string_view, Range<int>>(3);
    constexpr auto expected = "equal 3";
    EXPECT_EQ(value_str, expected);
  }
  {
    constexpr auto value_str = map.to<std::string_view, Range<int>>(4);
    constexpr auto expected = "any";
    EXPECT_EQ(value_str, expected);
  }
  {
    constexpr auto value_str = map.to<std::string_view, Range<int>>(5);
    constexpr auto expected = "larger equal 5";
    EXPECT_EQ(value_str, expected);
  }
  {
    constexpr auto value_str = map.to<std::string_view, Range<int>>(6);
    constexpr auto expected = "larger 5";
    EXPECT_EQ(value_str, expected);
  }
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}