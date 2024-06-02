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
#pragma once

#include <array>
#include <optional>
#include <stdexcept>
#include <tuple>
#include <type_traits>

namespace const_mapper {

/**
 * Use with `pattern_match`.
 * Select as return value of `pattern_match`.
 *
 * @see ConstMapper::pattern_match
 */
class Result {};

/**
 * Use with `pattern_match`.
 * Ignore this value on `pattern_match`.
 *
 * @see ConstMapper::pattern_match
 */
class Ignore {};

template <std::size_t N, class... Args>
class ConstMapper {
 public:
  /**
   * Type of array element.
   */
  using Tuple = std::tuple<Args...>;

  explicit constexpr ConstMapper(std::array<Tuple, N> list);

  /**
   * Simple convert.
   * @param i_to index of return value.
   * @param i_from index of key value.
   * @return first i_to value that i_from value match with array element.
   */
  template <std::size_t i_to, std::size_t i_from, class Key>
  constexpr auto to(const Key &key) const;

  /**
   * Type matching conversion.
   * @param To type of return value. if there are 2 or more `To` value in `Tuple`, pick-up first value.
   * @param From type of key value. if there are 2 or more `From` value in `Tuple`, pick-up first value.
   * @return first `To` value that `From` value match with array element.
   */
  template <class To, class From, class Key = From>
  constexpr To to(const Key &key) const;

  /**
   * Pattern matching conversion.
   *
   * @param pattern Pattern of matching. At least one value must be `Result`. tuple_size must be same as size of Tuple.
   * @return values that are setted Result in pattern. If one `Result,` return value. If two or more `Result`s, return
   * tuple of `Result`
   */
  template <class... Types>
  constexpr auto pattern_match(const std::tuple<Types...> &pattern) const;

 private:
  std::array<Tuple, N> map_data_;

  /**
   * Implementation of pattern_match
   * @see pattern_match
   */
  template <class... Types>
  constexpr auto pattern_match_impl(const std::tuple<Types...> &pattern) const;

  /**
   * compare pattern and array elements.
   * @see pattern_match
   */
  template <std::size_t index, class PatternTuple>
  constexpr bool check_pattern_match(const Tuple &tuple, const PatternTuple &pattern_tuple) const;

  /**
   * compare t0 and t1.
   * @return true if (t0 == t1) or (t1 is `Result`) or (t1 is `Ignore`), otherwise false.
   */
  template <class T0, class T1>
  static constexpr bool compare(const T0 &t0, const T1 &t1);

  /**
   * compare t0 and t1.
   * Specialized for `Result`.
   * @return true
   */
  template <class T>
  static constexpr bool compare([[maybe_unused]] const T &t0, [[maybe_unused]] const Result &t1);

  /**
   * compare t0 and t1.
   * Specialized for `Ignore`.
   * @return true
   */
  template <class T>
  static constexpr bool compare([[maybe_unused]] const T &t0, [[maybe_unused]] const Ignore &t1);

  /**
   * Get result of `pattern_match`
   * @see pattern_match
   * @return tuple of results.
   */
  template <class PatternTuple>
  constexpr auto get_result(const Tuple &value_tuple) const;

  /**
   * Get result of `pattern_match`
   * @see pattern_match
   * @return tuple of results.
   */
  template <std::size_t index, class PatternTuple>
  constexpr auto get_result_impl(const Tuple &value_tuple) const;

  static constexpr auto tuple_size();
};

template <class T>
class Anyable {
 public:
  using type = T;

  constexpr Anyable();
  constexpr Anyable(T value);

  constexpr bool operator==(const Anyable<T> &rhs) const;
  constexpr bool operator==(const T &rhs) const;

  constexpr std::optional<T> value() const;
  constexpr operator std::optional<T>() const;

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

  constexpr Range();
  constexpr Range(CompareType compare_type, T value);

  constexpr bool operator==(const T &rhs) const;

 private:
  CompareType compare_type_ = CompareType::Any;
  T value_;
};
}  // namespace const_mapper

namespace {
/**
 * タプルの要素から指定した型のインデックスを取得する。
 * 複数の要素と一致する場合は一番最初のもの。
 */
template <class Tuple, class T, std::size_t index>
inline constexpr std::size_t tuple_index() {
  if constexpr (index >= std::tuple_size_v<Tuple>) {
    return std::tuple_size_v<Tuple>;  // `not found` or `index is out of range`.
  } else if constexpr (std::is_same_v<T, std::tuple_element_t<index, Tuple>>) {
    return index;
  } else {
    return tuple_index<Tuple, T, index + 1>();
  }
}

template <class Tuple, class T>
inline constexpr std::size_t tuple_index() {
  constexpr auto index = tuple_index<Tuple, T, 0>();

  static_assert(index < std::tuple_size_v<Tuple>, "Tuple does not contain T.");

  return index;
}

template <class Tuple, class T>
inline constexpr auto tuple_contains() {
  return tuple_index<Tuple, T>() < std::tuple_size_v<Tuple>;
}

template <class... Args>
constexpr auto un_tuple_if_one_element(const std::tuple<Args...> &tuple) {
  if constexpr (sizeof...(Args) == 1) {
    return std::get<0>(tuple);
  } else {
    return tuple;
  }
}
}  // namespace

namespace const_mapper {

template <class T>
constexpr Anyable<T>::Anyable() {}

template <class T>
constexpr Anyable<T>::Anyable(T value) : value_(value) {}

template <class T>
constexpr bool Anyable<T>::operator==(const Anyable<T> &rhs) const {
  return !value_ || !rhs.value_ || (*value_ == *rhs.value_);
}

template <class T>
constexpr bool Anyable<T>::operator==(const T &rhs) const {
  return !value_ || (*value_ == rhs);
}

template <class T>
constexpr std::optional<T> Anyable<T>::value() const {
  return value_;
}

template <class T>
constexpr Anyable<T>::operator std::optional<T>() const {
  return value_;
}

template <class T>
constexpr Range<T>::Range() : value_(){};

template <class T>
constexpr Range<T>::Range(CompareType compare_type, T value) : compare_type_(compare_type), value_(value) {}

template <class T>
constexpr bool Range<T>::operator==(const T &rhs) const {
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
      throw std::logic_error("no implement error");
  }
}

template <std::size_t N, class... Args>
constexpr auto ConstMapper<N, Args...>::tuple_size() {
  return std::tuple_size_v<Tuple>;
}

template <std::size_t N, class... Args>
constexpr ConstMapper<N, Args...>::ConstMapper(std::array<Tuple, N> list) : map_data_(list) {
  static_assert(N > 1, "N must grater than 1.");
}

template <std::size_t N, class... Args>
template <std::size_t i_to, std::size_t i_from, class Key>
constexpr auto ConstMapper<N, Args...>::to(const Key &key) const {
  if constexpr (!(i_to < tuple_size())) {
    static_assert(false, "i_to out of tuple range");
  } else if constexpr (!(i_from < tuple_size())) {
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

template <std::size_t N, class... Args>
template <class To, class From, class Key>
constexpr To ConstMapper<N, Args...>::to(const Key &key) const {
  constexpr auto i_to = tuple_index<Tuple, To, 0>();
  constexpr auto i_from = tuple_index<Tuple, From, 0>();

  if constexpr (!(i_to < tuple_size())) {
    static_assert(false, "Tuple does not contain `To` element.");
  } else if constexpr (!(i_from < tuple_size())) {
    static_assert(false, "Tuple does not contain `From` element.");
  } else {
    return to<i_to, i_from>(key);
  }
}

template <std::size_t N, class... Args>
template <class... Types>
constexpr auto ConstMapper<N, Args...>::pattern_match(const std::tuple<Types...> &pattern) const {
  static_assert(tuple_size() == sizeof...(Types), "tuple size dose not match.");
  static_assert(tuple_contains<std::tuple<Types...>, Result>(), "No Result value.");
  return pattern_match_impl(pattern);
}

template <std::size_t N, class... Args>
template <class... Types>
constexpr auto ConstMapper<N, Args...>::pattern_match_impl(const std::tuple<Types...> &pattern) const {
  for (const auto &tuple : map_data_) {
    if (check_pattern_match<0>(tuple, pattern)) {
      return un_tuple_if_one_element(get_result<std::tuple<Types...>>(tuple));
    }
  }
  throw std::out_of_range("key not found.");
}

template <std::size_t N, class... Args>
template <std::size_t index, class PatternTuple>
constexpr bool ConstMapper<N, Args...>::check_pattern_match(const Tuple &tuple,
                                                            const PatternTuple &pattern_tuple) const {
  if constexpr (index + 1 == tuple_size()) {
    return compare(std::get<index>(tuple), std::get<index>(pattern_tuple));
  } else {
    return compare(std::get<index>(tuple), std::get<index>(pattern_tuple)) &&
           check_pattern_match<index + 1>(tuple, pattern_tuple);
  }
}

template <std::size_t N, class... Args>
template <class T0, class T1>
constexpr bool ConstMapper<N, Args...>::compare(const T0 &t0, const T1 &t1) {
  return (t0 == t1);
}

template <std::size_t N, class... Args>
template <class T>
constexpr bool ConstMapper<N, Args...>::compare([[maybe_unused]] const T &t0, [[maybe_unused]] const Result &t1) {
  return true;
}

template <std::size_t N, class... Args>
template <class T>
constexpr bool ConstMapper<N, Args...>::compare([[maybe_unused]] const T &t0, [[maybe_unused]] const Ignore &t1) {
  return true;
}

template <std::size_t N, class... Args>
template <class PatternTuple>
constexpr auto ConstMapper<N, Args...>::get_result(const Tuple &value_tuple) const {
  constexpr auto i = tuple_index<PatternTuple, Result>();

  return get_result_impl<i, PatternTuple>(value_tuple);
}

template <std::size_t N, class... Args>
template <std::size_t index, class PatternTuple>
constexpr auto ConstMapper<N, Args...>::get_result_impl(const Tuple &value_tuple) const {
  constexpr auto i = tuple_index<PatternTuple, Result, index + 1>();

  if constexpr (i == tuple_size()) {
    return std::tuple(std::get<index>(value_tuple));
  } else {
    return std::tuple_cat(std::tuple(std::get<index>(value_tuple)), get_result_impl<i, PatternTuple>(value_tuple));
  }
}
}  // namespace const_mapper
