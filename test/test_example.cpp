#include <gtest/gtest.h>

#include "const_mapper.hpp"

TEST(TestExample, simple) {
  using namespace const_mapper;
  constexpr auto map = ConstMapper<3, std::string_view, int, std::uint8_t>{{{
      {"value_0", 0, 0},
      {"value_1", 1, 10},
      {"value_2", 2, 20},
  }}};

  // str0 == "value_1";
  constexpr auto str0 = map.to<std::string_view, int>(1);

  // str1 == "value_2";
  constexpr auto str1 = map.to<std::string_view, std::uint8_t>(20);

  // int0 == 2;
  constexpr auto int0 = map.to<int, std::uint8_t>(20);

  {
    constexpr auto expected = "value_1";
    EXPECT_EQ(str0, expected);
  }
  {
    constexpr auto expected = "value_2";
    EXPECT_EQ(str1, expected);
  }
  {
    constexpr int expected = 2;
    EXPECT_EQ(int0, expected);
  }
}

TEST(TestExample, pattern_match) {
  using namespace const_mapper;
  constexpr auto map = ConstMapper<6, std::string_view, Range<int>, Anyable<int>>{{{
      {"less2 & 1", {CompareType::LessThan, 2}, 1},
      {"less2 & 2", {CompareType::LessThan, 2}, 2},
      {"larger5", {CompareType::LargerThan, 5}, {}},
      {"Any", {}, {}},
  }}};

  // str0 == "less2 & 1"
  constexpr auto str0 = map.pattern_match(std::make_tuple(Result{}, 1, 1));

  // str1 == "less2 & 2"
  constexpr auto str1 = map.pattern_match(std::make_tuple(Result{}, 1, 2));

  // str2 == "larger5"
  constexpr auto str2 = map.pattern_match(std::make_tuple(Result{}, 6, -1));

  // str3 == "Any"
  constexpr auto str3 = map.pattern_match(std::make_tuple(Result{}, 5, -1));

  {
    constexpr auto expected = "less2 & 1";
    EXPECT_EQ(str0, expected);
  }
  {
    constexpr auto expected = "less2 & 2";
    EXPECT_EQ(str1, expected);
  }
  {
    constexpr auto expected = "larger5";
    EXPECT_EQ(str2, expected);
  }
  {
    constexpr auto expected = "Any";
    EXPECT_EQ(str3, expected);
  }
}