#include <expected>
#include <iostream>
#include <ranges>
#include <sstream>

#include "util.hpp"

using std::cout, std::println;
using std::string;
using std::unexpected, std::expected;

bool is_safe(std::ranges::range auto &parts) {
  constexpr int DESCENDING = 1;
  constexpr int ASCENDING = -1;

  int dir = 0;

  auto it = std::ranges::begin(parts);
  auto end = std::ranges::end(parts);

  if (it == end) {
    throw "OH NO WHAT THE HECK";
  }

  auto prev = *it;
  ++it;

  for (; it != end; ++it) {
    int diff = prev - *it;
    prev = *it;
    if (diff >= -3 && diff < 0 && dir != DESCENDING) {
      dir = ASCENDING;
    } else if (diff > 0 && diff <= 3 && dir != ASCENDING) {
      dir = DESCENDING;
    } else {
      return false;
    }
  }
  return true;
}

auto part_one(const string &input) -> expected<int, string> {
  int result = 0;

  std::istringstream lines(input);
  string line;
  while (std::getline(lines, line)) {
    auto parts = split(line, ' ', parse::to_int);
    if (is_safe(parts)) {
      result++;
    }
  }

  return result;
}

auto part_two(const string &input) -> expected<int, string> {
  int result = 0;

  std::istringstream lines(input);
  string line;
  while (std::getline(lines, line)) {
    auto all_parts = split(line, ' ', parse::to_int);
    for (int skip_index = 0; skip_index < all_parts.size(); ++skip_index) {
      auto parts =
          all_parts | std::views::filter([&, i = 0](const int &) mutable {
            return i++ != skip_index;
          });
      if (is_safe(parts)) {
        result++;
        break;
      }
    }
  }
  return result;
}

int main() {
  std::ostringstream buffer;
  buffer << std::cin.rdbuf();
  string input = buffer.str();

  println(cout, " --- PART 1 LOGS ---");
  int part_one_result = part_one(input)
                            .or_else([](string error) {
                              println(cout, "\033[1;31m{}\033[0m", error);
                              return expected<int, const char *>(0);
                            })
                            .value();
  println(cout);
  println(cout);

  println(cout, " --- PART 2 LOGS ---");
  int part_two_result = part_two(input)
                            .or_else([](string error) {
                              println(cout, "\033[1;31m{}\033[0m", error);
                              return expected<int, const char *>(0);
                            })
                            .value();
  println(cout);
  println(cout);

  println(cout, "-----------------------------------------");
  println(cout, "Day 2");
  println(cout, "\tPart 1");
  println(cout, "\t\tAnswer: {}", part_one_result);
  println(cout, "\tPart 2");
  println(cout, "\t\tAnswer: {}", part_two_result);
  println(cout, "-----------------------------------------");
  return 0;
}
