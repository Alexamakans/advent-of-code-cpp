#include <cmath>
#include <cstdint>
#include <cstdlib>
#include <expected>
#include <format>
#include <iostream>
#include <sstream>
#include <string>
#include <unordered_set>

#include "util.hpp"

using std::cout, std::println;
using std::string;
using std::unexpected, std::expected;

typedef uint64_t AnswerType;

std::unordered_set<std::pair<uint64_t, uint64_t>> visited;

// perimeter, area
std::pair<AnswerType, AnswerType>
measure_recurse(const CharGrid &grid, const char regionId, int x, int y) {
  auto key = std::make_pair(x, y);
  bool contains = grid.contains(x, y);
  char c = contains ? grid.at(x, y) : regionId;
  if (visited.contains(key) && c == regionId) {
    return std::make_pair(0, 0);
  }
  if (!contains || c != regionId) {
    return std::make_pair(1, 0);
  }
  visited.insert(key);

  std::pair<AnswerType, AnswerType> result{0, 1};
  std::pair<AnswerType, AnswerType> tmp;
  tmp = measure_recurse(grid, regionId, x + 1, y);
  result.first += tmp.first;
  result.second += tmp.second;
  tmp = measure_recurse(grid, regionId, x - 1, y);
  result.first += tmp.first;
  result.second += tmp.second;
  tmp = measure_recurse(grid, regionId, x, y + 1);
  result.first += tmp.first;
  result.second += tmp.second;
  tmp = measure_recurse(grid, regionId, x, y - 1);
  result.first += tmp.first;
  result.second += tmp.second;
  return result;
}

std::pair<AnswerType, AnswerType> measure(const CharGrid &grid, int x, int y) {
  return measure_recurse(grid, grid.at(x, y), x, y);
}

auto part_one(const std::string &input) -> expected<AnswerType, string> {
  AnswerType result = 0;
  CharGrid grid(input);
  for (int x = 0; x < grid.width; ++x) {
    for (int y = 0; y < grid.height; ++y) {
      auto measurement = measure(grid, x, y);
      result += measurement.first * measurement.second;
    }
  }
  return result;
}

auto part_two(const string &input) -> expected<AnswerType, string> {
  AnswerType result = 0;
  return result;
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
  println(cout, "Day 12");
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
