# ConstMapper
## Overview
**ConstMapper** is a simple template library for mapping. 

## Features

- **Only depends STL**: not need to add extra library.
- **Static Definition**: Enables compile-time definition of constant mappings, ensuring high efficiency and reduced runtime overhead.
- **Bi-Directional Conversion**: Supports conversions in both directions between any defined pairs, making it a highly flexible solution.
- **Support C++17 or later** 

## Installation

This is single header file library.<br>
Just include the header file in your source code.

## Example
```cpp
constexpr auto map = const_mapper::ConstMapper<3, std::string_view, int, std::uint8_t>{{{
    {"value_0", 0, 0},
    {"value_1", 1, 10},
    {"value_2", 2, 20}}}};

constexpr auto str1 = map.to<std::string_view, int>(1); // str == "value_1";
constexpr auto str2 = map.to<std::string_view, std::uint8_t>(20); // str == "value_2";
constexpr auto uint0 = map.to<std::uint8_t, int>(0); // str == "value_2";
```

## License

This project is licensed under the MIT License. See the `LICENSE` file for details.
