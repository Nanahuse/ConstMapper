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

namespace const_mapper {

class Result {};

class Ignore {};

template <std::size_t N, class... Args>
class ConstMapper {
 public:
  using Tuple = std::tuple<Args...>;

  template <std::size_t index>
  using Type = std::tuple_element_t<index, Tuple>;

  explicit constexpr ConstMapper(std::array<Tuple, N> list);

  template <std::size_t i_to, std::size_t i_from, class Key>
  constexpr auto to(const Key &key) const;

  template <class To, class From, class Key = From>
  constexpr To to(const Key &key) const;

  template <class To, class... Keys>
  constexpr To pattern_to(const Keys &...keys) const;

  template <class... Keys>
  constexpr auto pattern_match(const std::tuple<Keys...> &key) const;

  static constexpr auto tuple_size();

  constexpr auto begin() const noexcept;
  constexpr auto end() const noexcept;
  constexpr auto size() const noexcept;

 private:
  std::array<Tuple, N> map_data_;

  template <class Key, class... Keys>
  constexpr bool check_pattern(const Tuple &tuple, const Key &key, const Keys &...keys) const;
  template <class Key>
  constexpr bool check_pattern(const Tuple &tuple, const Key &key) const;

  template <std::size_t index, class KeyTuple>
  constexpr bool check_match(const Tuple &tuple, const KeyTuple &key_tuple) const;

  template <class T0, class T1>
  static constexpr bool compare(const T0 &t0, const T1 &t1);
  template <class T>
  static constexpr bool compare([[maybe_unused]] const T &t0, [[maybe_unused]] const Result &t1);
  template <class T>
  static constexpr bool compare([[maybe_unused]] const T &t0, [[maybe_unused]] const Ignore &t1);

  template <class... Keys>
  constexpr auto pattern_match_impl(const std::tuple<Keys...> &key) const;

  template <class KeyTuple>
  constexpr auto get_all_result(const Tuple &value_tuple) const;

  template <std::size_t index, class KeyTuple>
  constexpr auto get_all_result(const Tuple &value_tuple) const;
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

template <class T>
constexpr auto un_tuple_if_one_element(const std::tuple<T> &tuple) {
  return std::get<0>(tuple);
}

template <class... Args>
constexpr auto un_tuple_if_one_element(const std::tuple<Args...> &tuple) {
  return tuple;
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
constexpr Anyable<T>::Anyable() {}

template <class T>
constexpr Anyable<T>::Anyable(T value) : value_(value) {}

template <class T>
constexpr bool Anyable<T>::operator==(const Anyable<T> &rhs) const {
  return (value_ && rhs.value_) ? (value_ == rhs.value_) : true;
}

template <class T>
constexpr bool Anyable<T>::operator==(const T &rhs) const {
  return (value_) ? (value_ == rhs) : true;
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
};

template <std::size_t N, class... Args>
constexpr ConstMapper<N, Args...>::ConstMapper(std::array<Tuple, N> list) : map_data_(list) {}

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
template <class To, class... Keys>
constexpr To ConstMapper<N, Args...>::pattern_to(const Keys &...keys) const {
  for (const auto &tuple : map_data_) {
    if (check_pattern(tuple, keys...)) {
      return std::get<To>(tuple);
    }
  }

  throw std::out_of_range("key not found.");
}

template <std::size_t N, class... Args>
template <class... Keys>
constexpr auto ConstMapper<N, Args...>::pattern_match(const std::tuple<Keys...> &key) const {
  static_assert(tuple_size() == sizeof...(Keys), "tuple size dose not match.");
  static_assert(tuple_contains<std::tuple<Keys...>, Result>(), "Result is not setted.");
  return pattern_match_impl(key);
}

template <std::size_t N, class... Args>
constexpr auto ConstMapper<N, Args...>::begin() const noexcept {
  return map_data_.begin();
}

template <std::size_t N, class... Args>
constexpr auto ConstMapper<N, Args...>::end() const noexcept {
  return map_data_.end();
}

template <std::size_t N, class... Args>
constexpr auto ConstMapper<N, Args...>::size() const noexcept {
  return map_data_.size();
}

template <std::size_t N, class... Args>
template <class Key, class... Keys>
constexpr bool ConstMapper<N, Args...>::check_pattern(const Tuple &tuple, const Key &key, const Keys &...keys) const {
  return (std::get<Key>(tuple) == key) && check_pattern(tuple, keys...);
}

template <std::size_t N, class... Args>
template <class Key>
constexpr bool ConstMapper<N, Args...>::check_pattern(const Tuple &tuple, const Key &key) const {
  return std::get<Key>(tuple) == key;
}

template <std::size_t N, class... Args>
template <std::size_t index, class KeyTuple>
constexpr bool ConstMapper<N, Args...>::check_match(const Tuple &tuple, const KeyTuple &key_tuple) const {
  if constexpr (index + 1 == tuple_size()) {
    return compare(std::get<index>(tuple), std::get<index>(key_tuple));
  } else {
    return compare(std::get<index>(tuple), std::get<index>(key_tuple)) && check_match<index + 1>(tuple, key_tuple);
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
template <class... Keys>
constexpr auto ConstMapper<N, Args...>::pattern_match_impl(const std::tuple<Keys...> &key) const {
  for (const auto &tuple : map_data_) {
    if (check_match<0>(tuple, key)) {
      return un_tuple_if_one_element(get_all_result<std::tuple<Keys...>>(tuple));
    }
  }
  throw std::out_of_range("key not found.");
}

template <std::size_t N, class... Args>
template <class KeyTuple>
constexpr auto ConstMapper<N, Args...>::get_all_result(const Tuple &value_tuple) const {
  constexpr auto i = tuple_index<KeyTuple, Result>();

  return get_all_result<i, KeyTuple>(value_tuple);
}

template <std::size_t N, class... Args>
template <std::size_t index, class KeyTuple>
constexpr auto ConstMapper<N, Args...>::get_all_result(const Tuple &value_tuple) const {
  constexpr auto i = tuple_index<KeyTuple, Result, index + 1>();

  if constexpr (i == tuple_size()) {
    return std::tuple(std::get<index>(value_tuple));
  } else {
    return std::tuple_cat(std::tuple(std::get<index>(value_tuple)), get_all_result<i, KeyTuple>(value_tuple));
  }
}
}  // namespace const_mapper
