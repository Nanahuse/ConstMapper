# ConstMapper
## Overview
**ConstMapper** is a simple template library for mapping. 

## Features

- **Only depends on STL**: no need to add any extra libraries.
- **Single Header File**: All you need is include `const_mapper.hpp`.
- **Compile-Time Mapping**: Supports compile-time mapping. As well as at runtime mapping.
- **All-Directional Conversion**: Supports conversions in all directions between any pairs.
- **Pattern Matching**: Supports complex conversion by pattern matching.
- **Support C++17 or later** 

## Installation

This is single header file library.<br>
Just include the header file in your source code.

## Example

[test_const_mapper.cpp](test/test_const_mapper.cpp) will help for you understanding how to use this library.

### Simple Conversion
```cpp
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
```

### Pattern Conversion
```cpp
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
```

## Performance Test

check test detail -> [test_performance.cpp](test/test_performance.cpp)


### optimization: -O0
```
[----------] 5 tests from Performance
[ RUN      ] Performance.ref_std_unordered_map
[       OK ] Performance.ref_std_unordered_map (619 ms)
[ RUN      ] Performance.ref_std_map
[       OK ] Performance.ref_std_map (983 ms)
[ RUN      ] Performance.const_mapper_to
[       OK ] Performance.const_mapper_to (731 ms)
[ RUN      ] Performance.const_mapper_pattern_match
[       OK ] Performance.const_mapper_pattern_match (1299 ms)
[ RUN      ] Performance.const_mapper_pattern_match_pick_up_2_values
[       OK ] Performance.const_mapper_pattern_match_pick_up_2_values (1880 ms)
[----------] 5 tests from Performance (5513 ms total)
```

### optimization: -O3
```
[----------] 5 tests from Performance
[ RUN      ] Performance.ref_std_unordered_map
[       OK ] Performance.ref_std_unordered_map (42 ms)
[ RUN      ] Performance.ref_std_map
[       OK ] Performance.ref_std_map (139 ms)
[ RUN      ] Performance.const_mapper_to
[       OK ] Performance.const_mapper_to (35 ms)
[ RUN      ] Performance.const_mapper_pattern_match
[       OK ] Performance.const_mapper_pattern_match (36 ms)
[ RUN      ] Performance.const_mapper_pattern_match_pick_up_2_values
[       OK ] Performance.const_mapper_pattern_match_pick_up_2_values (38 ms)
[----------] 5 tests from Performance (292 ms total)
```

## License

This project is licensed under the MIT License. See the `LICENSE` file for details.
