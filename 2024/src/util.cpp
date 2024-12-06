#include <charconv>
#include <ranges>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

std::vector<std::string> split_to_strings(const std::string &s,
                                          const char delimiter) {
  std::vector<std::string> result;
  const auto view = std::string_view(s);
  for (auto &&token : std::views::split(s, delimiter)) {
    std::string_view sv(token.begin(), token.end());
    if (!sv.empty()) {
      result.emplace_back(sv);
    }
  }
  return result;
}

std::vector<int> split_to_ints(const std::string &s, const char delimiter) {
  std::vector<int> result;
  const auto view = std::string_view(s);
  for (auto &&token : std::views::split(s, delimiter)) {
    std::string_view sv(token.begin(), token.end());
    if (!sv.empty()) {
      int value;
      auto [_, err] = std::from_chars(sv.data(), sv.data() + sv.size(), value);
      if (err == std::errc()) {
        result.push_back(value);
      }
    }
  }
  return result;
}
