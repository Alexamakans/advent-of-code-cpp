#pragma once
#ifndef UTIL_HPP
#define UTIL_HPP
#include <charconv>
#include <chrono>
#include <cmath>
#include <cstdint>
#include <future>
#include <optional>
#include <ranges>
#include <sstream>
#include <string_view>
#include <vector>

namespace parse {
constexpr auto to_string(const std::string_view &sv)
    -> std::optional<std::string> {
  return std::string(sv);
}

constexpr auto to_int(const std::string_view &sv) -> std::optional<int> {
  int value;
  auto [ptr, err] = std::from_chars(sv.data(), sv.data() + sv.size(), value);
  if (err == std::errc{}) {
    return value;
  }
  return std::nullopt;
}

constexpr auto to_int64(const std::string_view &sv) -> std::optional<int64_t> {
  int64_t value;
  auto [ptr, err] = std::from_chars(sv.data(), sv.data() + sv.size(), value);
  if (err == std::errc{}) {
    return value;
  }
  return std::nullopt;
}

constexpr auto to_uint64(const std::string_view &sv)
    -> std::optional<uint64_t> {
  uint64_t value;
  auto [ptr, err] = std::from_chars(sv.data(), sv.data() + sv.size(), value);
  if (err == std::errc{}) {
    return value;
  }
  return std::nullopt;
}
} // namespace parse

template <typename Parser>
auto split(const std::string &s, const char delimiter,
           Parser parser = parse::to_string)
    -> std::vector<
        typename std::invoke_result_t<Parser, std::string_view>::value_type> {
  using T = typename std::invoke_result_t<Parser, std::string_view>::value_type;
  std::vector<T> result;
  for (const auto &token : std::views::split(s, delimiter) |
                               std::views::transform([&](auto &&token) {
                                 return std::string_view(token.begin(),
                                                         token.end());
                               })) {
    if (auto parsed = parser(token); parsed) {
      result.push_back(*parsed);
    }
  }
  return result;
}

template <typename T>
std::string join(const std::vector<T> &vec, std::string delimiter) {
  std::ostringstream s("");

  auto it = vec.cbegin();
  for (; it < vec.cend() - 1; ++it) {
    print(s, "{}{}", *it, delimiter);
  }
  print(s, "{}", *it);
  return s.str();
}

inline uint64_t concatenate(uint64_t x, uint64_t y) {
  uint64_t pow10 = 1;
  while (pow10 <= y) {
    pow10 *= 10;
  }
  return x * pow10 + y;
}

template <typename T> inline int get_num_digits(T x) {
  uint64_t pow10 = 10;
  int num_digits = 1;
  while (pow10 <= x) {
    pow10 *= 10;
    ++num_digits;
  }
  return num_digits;
}

// index determines the last digit in the left number, starting at 0 for the
// right-most digit in x.
template <typename T> inline std::pair<T, T> split_number(T x, int index) {
  int v = pow(10, index);
  auto left = x / v;
  auto right = x % v;
  return std::make_pair(left, right);
}

template <typename T, typename Batch>
concept InputProvider = requires(T t, Batch b, const std::string &input) {
  { t.prepare(input) };
  { t.provide() } -> std::same_as<Batch>;
  { t.done() } -> std::same_as<bool>;
};

template <typename T, typename Batch, typename BatchResult>
concept InputConsumer = requires(T t, Batch input, BatchResult output) {
  { t.consume(input) } -> std::same_as<BatchResult>;
};

template <typename T, typename Batch, typename BatchResult,
          typename FinalResult>
concept Multithreader =
    requires(T t, BatchResult batchResult, FinalResult finalResult) {
      { t.provider } -> InputProvider<Batch>;
      { t.consumer } -> InputConsumer<Batch, BatchResult>;
      { t.combine(finalResult, batchResult) } -> std::same_as<FinalResult>;
    };

template <typename Batch, typename BatchResult, typename FinalResult>
FinalResult execute(const std::string &input,
                    Multithreader<Batch, BatchResult, FinalResult> auto &m,
                    FinalResult starting_value) {
  m.provider.prepare(input);
  std::vector<std::future<BatchResult>> futures;
  while (!m.provider.done()) {
    Batch batch = m.provider.provide();
    futures.push_back(std::async(
        std::launch::async,
        [&](Batch batch) { return m.consumer.consume(batch); }, batch));
  }

  FinalResult result = starting_value;
  for (auto &future : futures) {
    BatchResult batch_result = future.get();
    result = m.combine(result, batch_result);
  }

  return result;
}

auto time_start = std::chrono::high_resolution_clock::now();

inline void reset_timer() {
  time_start = std::chrono::high_resolution_clock::now();
}

inline auto get_timer_microseconds() {
  return std::chrono::duration_cast<std::chrono::microseconds>(
             std::chrono::high_resolution_clock::now() - time_start)
      .count();
}

struct Point {
  int x, y;

  Point() = default;
  Point(int x, int y) : x(x), y(y) {}

  bool operator==(const Point &rhs) const { return x == rhs.x && y == rhs.y; }
};

namespace std {
template <> struct hash<Point> {
  inline size_t operator()(const Point &v) const {
    hash<int> int_hasher;
    return int_hasher(v.x) ^ int_hasher(v.y);
  }
};
}; // namespace std

struct Line {
  Point a, b;

  Line() = default;
  Line(Point a, Point b) : a(a), b(b) {}

  bool operator==(const Line &rhs) const { return a == rhs.a && b == rhs.b; }
};

namespace std {
template <> struct hash<Line> {
  inline size_t operator()(const Line &v) const {
    hash<Point> point_hasher;
    return point_hasher(v.a) ^ point_hasher(v.b);
  }
};
}; // namespace std

namespace std {
template <> struct hash<std::pair<uint64_t, uint64_t>> {
  inline size_t operator()(const std::pair<uint64_t, uint64_t> &v) const {
    hash<uint64_t> hasher;
    return hasher(v.first) ^ hasher(v.second);
  }
};
}; // namespace std

namespace std {
template <> struct hash<std::vector<int>> {
  inline size_t operator()(const std::vector<int> &v) const {
    hash<int> int_hasher{};
    size_t h = int_hasher(v.size());
    for (const auto &e : v) {
      h ^= int_hasher(e);
    }
    return h;
  }
};
}; // namespace std

struct CharGrid {
  std::vector<char> vec;
  size_t width, height;
  CharGrid(const std::string &input) {
    std::istringstream lines(input);
    std::string line;
    while (getline(lines, line)) {
      width = line.size();
      for (const char c : line) {
        vec.push_back(c);
      }
    }
    height = vec.size() / width;
  }

  inline size_t index(size_t x, size_t y) const { return x + y * width; }

  char &at(size_t x, size_t y) { return vec.at(index(x, y)); }

  const char &at(size_t x, size_t y) const { return vec.at(index(x, y)); }

  bool contains(size_t x, size_t y) const {
    return x >= 0 && x < width && y >= 0 && y < height;
  }
};
#endif // UTIL_HPP
