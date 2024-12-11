#ifndef UTIL_HPP
#define UTIL_HPP
#include <charconv>
#include <cmath>
#include <cstdint>
#include <future>
#include <iostream>
#include <optional>
#include <ostream>
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
  while (pow10 <= y)
    pow10 *= 10;
  return x * pow10 + y;
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

#endif // UTIL_HPP
