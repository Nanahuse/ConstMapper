/**
 * MIT License
 *
 * Copyright (c) 2024 Nanahuse
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include <array>
#include <optional>
#include <stdexcept>
#include <tuple>
#include <type_traits>
#include <typeinfo>

namespace {
/**
 * タプルの要素から指定した型のインデックスを取得する。
 * 複数の要素と一致する場合は一番最初のもの。
 */
template <class Tuple, class T, std::size_t index>
inline constexpr std::size_t tuple_index() {
  if constexpr (std::is_same_v<T, std::tuple_element_t<index, Tuple>>) {
    return index;
  } else {
    if constexpr (index + 1 == std::tuple_size_v<Tuple>) {
      return index + 1;  // not found
    } else {
      return tuple_index<Tuple, T, index + 1>();
    }
  }
}

template <class Tuple, class T>
inline constexpr std::size_t tuple_index() {
  constexpr auto index = tuple_index<Tuple, T, 0>();

  static_assert(index < std::tuple_size_v<Tuple>, "Tuple does not contain T.");

  return index;
}

}  // namespace

namespace const_mapper {

template <class Tuple, class T>
inline constexpr auto tuple_index_v = tuple_index<Tuple, T>();

template <class Tuple, class T>
inline constexpr auto tuple_contains() {
  return tuple_index<Tuple, T>() < std::tuple_size_v<Tuple>;
}

template <class T>
class Anyable {
 public:
  using type = T;

  constexpr Anyable() {}
  constexpr Anyable(T value) : value_(value) {}

  constexpr bool operator==(const Anyable<T> &rhs) const {
    return (value_ && rhs.value_) ? (value_ == rhs.value_) : true;
  }

  constexpr bool operator==(const T &rhs) const { return (value_) ? (value_ == rhs) : true; }

  const std::optional<T> value() const { return value_; }

 private:
  std::optional<T> value_;
};

enum class CompareType {
  Any,
  LargerThan,
  LargerEqual,
  Equal,
  LessEqual,
  LessThan,
};

template <class T>
class Range {
 public:
  using type = T;

  constexpr Range() : value_(){};
  constexpr Range(CompareType compare_type, T value) : compare_type_(compare_type), value_(value) {}

  constexpr bool operator==(const T &rhs) const {
    switch (compare_type_) {
      case CompareType::LargerThan:
        return rhs > value_;
      case CompareType::LargerEqual:
        return rhs >= value_;
      case CompareType::Equal:
        return rhs == value_;
      case CompareType::LessEqual:
        return rhs <= value_;
      case CompareType::LessThan:
        return rhs < value_;
      case CompareType::Any:
        return true;

      default:
        return false;
    }
  }

 private:
  CompareType compare_type_ = CompareType::Any;
  T value_;
};

class Result {};

class Ignore {};

template <std::size_t N, class... Args>
class ConstMapper {
  using Tuple = std::tuple<Args...>;

  template <std::size_t index>
  using Type = std::tuple_element_t<index, Tuple>;

  static constexpr auto tuple_size = std::tuple_size_v<Tuple>;

 public:
  explicit constexpr ConstMapper(std::array<Tuple, N> list) : map_data_(list) {}

  template <std::size_t i_to, std::size_t i_from, class Key>
  constexpr auto to(const Key &key) const {
    if constexpr (!(i_to < tuple_size)) {
      static_assert(false, "i_to out of tuple range");
    } else if constexpr (!(i_from < tuple_size)) {
      static_assert(false, "i_from out of tuple range");
    } else {
      for (const auto &tuple : map_data_) {
        if (std::get<i_from>(tuple) == key) {
          return std::get<i_to>(tuple);
        }
      }
      throw std::out_of_range("key not found.");
    }
  }

  template <class To, class From, class Key = From>
  constexpr To to(const Key &key) const {
    constexpr auto i_to = tuple_index<Tuple, To, 0>();
    constexpr auto i_from = tuple_index<Tuple, From, 0>();

    if constexpr (!(i_to < tuple_size)) {
      static_assert(false, "Tuple does not contain `To` element.");
    } else if constexpr (!(i_from < tuple_size)) {
      static_assert(false, "Tuple does not contain `From` element.");
    } else {
      return to<i_to, i_from>(key);
    }
  }

  template <class To, class... Keys>
  constexpr To pattern_to(const Keys &...keys) const {
    for (const auto &tuple : map_data_) {
      if (check_pattern(tuple, keys...)) {
        return std::get<To>(tuple);
      }
    }

    throw std::out_of_range("key not found.");
  }

  template <class... Keys>
  constexpr auto pattern_match(const std::tuple<Keys...> &key) const {
    static_assert(tuple_size == sizeof...(Keys), "tuple size dose not match.");
    static_assert(tuple_contains<std::tuple<Keys...>, Result>(), "Result is not setted.");
    constexpr auto result_index = tuple_index_v<std::tuple<Keys...>, Result>;
    using TResult = Type<result_index>;
    return pattern_match_impl<TResult, result_index>(key);
  }

  template <class To, std::size_t index, class... Keys>
  constexpr To pattern_match_impl(const std::tuple<Keys...> &key) const {
    for (const auto &tuple : map_data_) {
      if (check_match<0>(tuple, key)) {
        return std::get<index>(tuple);
      }
    }

    throw std::out_of_range("key not found.");
  }

 private:
  std::array<Tuple, N> map_data_;

  template <class Key, class... Keys>
  constexpr bool check_pattern(const Tuple &tuple, const Key &key, const Keys &...keys) const {
    return (std::get<Key>(tuple) == key) && check_pattern(tuple, keys...);
  }

  template <class Key>
  constexpr bool check_pattern(const Tuple &tuple, const Key &key) const {
    return std::get<Key>(tuple) == key;
  }

  template <std::size_t index, class KeyTuple>
  constexpr bool check_match(const Tuple &tuple, const KeyTuple &key_tuple) const {
    if constexpr (index + 1 == tuple_size) {
      return compare(std::get<index>(tuple), std::get<index>(key_tuple));
    } else {
      return compare(std::get<index>(tuple), std::get<index>(key_tuple)) && check_match<index + 1>(tuple, key_tuple);
    }
  }

  template <class T0, class T1>
  static constexpr bool compare(const T0 &t0, const T1 &t1) {
    return (t0 == t1);
  }

  template <class T>
  static constexpr bool compare([[maybe_unused]] const T &t0, [[maybe_unused]] const Result &t1) {
    return true;
  }

  template <class T>
  static constexpr bool compare([[maybe_unused]] const T &t0, [[maybe_unused]] const Ignore &t1) {
    return true;
  }
};
}  // namespace const_mapper
