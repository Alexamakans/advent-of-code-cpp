#include <cmath>
#include <cstdint>
#include <cstdlib>
#include <expected>
#include <format>
#include <iostream>
#include <sstream>
#include <string>
#include <unordered_map>

#include "util.hpp"

using std::cout, std::println;
using std::string;
using std::unexpected, std::expected;

typedef uint64_t AnswerType;

std::unordered_map<std::pair<uint64_t, uint64_t>, uint64_t> cache;

AnswerType solve_recurse(uint64_t stone, uint64_t blinks) {
  if (blinks == 0) {
    return 1;
  }
  auto key = std::make_pair(stone, blinks);
  if (cache.contains(key)) {
    return cache.at(key);
  }
  AnswerType result = 0;
  if (stone == 0) {
    result += solve_recurse(1, blinks - 1);
  } else {
    const auto num_digits = get_num_digits(stone);
    if (num_digits % 2 == 0) {
      auto [left, right] = split_number(stone, num_digits / 2);
      result += solve_recurse(left, blinks - 1);
      result += solve_recurse(right, blinks - 1);
    } else {
      result += solve_recurse(stone * 2024, blinks - 1);
    }
  }
  cache.insert_or_assign(key, result);
  return result;
}

AnswerType solve(const std::string &input, uint64_t blinks) {
  AnswerType result = 0;
  std::vector<uint64_t> stones = split(input, ' ', parse::to_uint64);
  for (const auto stone : stones) {
    result += solve_recurse(stone, blinks);
  }
  return result;
}

auto part_one(const std::string &input) -> expected<AnswerType, string> {
  return solve(input, 25);
}

auto part_two(const string &input) -> expected<AnswerType, string> {
  return solve(input, 75);
}

int main() {
  std::ostringstream buffer;
  buffer << std::cin.rdbuf();
  string input = buffer.str();

  println(cout, " --- PART 1 LOGS ---");
  reset_timer();
  AnswerType part_one_result =
      part_one(input)
          .or_else([](string error) {
            println(cout, "\033[1;31m{}\033[0m", error);
            return expected<AnswerType, string>(0);
          })
          .value();
  auto part_one_took_microseconds = get_timer_microseconds();
  println(cout);
  println(cout);

  println(cout, " --- PART 2 LOGS ---");
  reset_timer();
  AnswerType part_two_result =
      part_two(input)
          .or_else([](string error) {
            println(cout, "\033[1;31m{}\033[0m", error);
            return expected<AnswerType, string>(0);
          })
          .value();
  auto part_two_took_microseconds = get_timer_microseconds();
  println(cout);
  println(cout);

  println(cout, "-----------------------------------------");
  println(cout, "Day 11");
  println(cout, "\tPart 1");
  println(cout, "\t\tAnswer: {}", part_one_result);
  println(cout, "\t\tTook {} us ({} ms) ({} s)", part_one_took_microseconds,
          float(part_one_took_microseconds) / 1000.0,
          float(part_one_took_microseconds) / 1000000.0);
  println(cout, "\tPart 2");
  println(cout, "\t\tAnswer: {}", part_two_result);
  println(cout, "\t\tTook {} us ({} ms) ({} s)", part_two_took_microseconds,
          float(part_two_took_microseconds) / 1000.0,
          float(part_two_took_microseconds) / 1000000.0);
  println(cout, "-----------------------------------------");
  return 0;
}
