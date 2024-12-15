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

std::unordered_set<Point> solve_part_one_recurse(const CharGrid &grid, int x,
                                                 int y) {
  std::unordered_set<Point> result;
  char current = grid.at(x, y);
  if (current == '9') {
    result.insert({x, y});
    return result;
  }

  if (grid.contains(x + 1, y) && grid.at(x + 1, y) == current + 1) {
    for (const Point &p : solve_part_one_recurse(grid, x + 1, y)) {
      result.insert(p);
    }
  }
  if (grid.contains(x - 1, y) && grid.at(x - 1, y) == current + 1) {
    for (const Point &p : solve_part_one_recurse(grid, x - 1, y)) {
      result.insert(p);
    }
  }
  if (grid.contains(x, y + 1) && grid.at(x, y + 1) == current + 1) {
    for (const Point &p : solve_part_one_recurse(grid, x, y + 1)) {
      result.insert(p);
    }
  }
  if (grid.contains(x, y - 1) && grid.at(x, y - 1) == current + 1) {
    for (const Point &p : solve_part_one_recurse(grid, x, y - 1)) {
      result.insert(p);
    }
  }

  return result;
}

auto solve_part_one(const CharGrid &grid) -> AnswerType {
  AnswerType result = 0;
  int x = 0;
  int y = 0;

  for (const char c : grid.vec) {
    if (c == '0') {
      result += solve_part_one_recurse(grid, x, y).size();
    }

    ++x;
    if (x >= grid.width) {
      ++y;
      x = 0;
    }
  }
  return result;
}

auto part_one(const std::string &input) -> expected<AnswerType, string> {
  AnswerType result = 0;
  auto grid = CharGrid(input);
  result = solve_part_one(grid);
  return result;
}

AnswerType solve_part_two_recurse(const CharGrid &grid, int x, int y) {
  AnswerType result = 0;
  char current = grid.at(x, y);
  if (current == '9') {
    return 1;
  }

  if (grid.contains(x + 1, y) && grid.at(x + 1, y) == current + 1) {
    result += solve_part_two_recurse(grid, x + 1, y);
  }
  if (grid.contains(x - 1, y) && grid.at(x - 1, y) == current + 1) {
    result += solve_part_two_recurse(grid, x - 1, y);
  }
  if (grid.contains(x, y + 1) && grid.at(x, y + 1) == current + 1) {
    result += solve_part_two_recurse(grid, x, y + 1);
  }
  if (grid.contains(x, y - 1) && grid.at(x, y - 1) == current + 1) {
    result += solve_part_two_recurse(grid, x, y - 1);
  }

  return result;
}

auto solve_part_two(const CharGrid &grid) -> AnswerType {
  AnswerType result = 0;
  int x = 0;
  int y = 0;

  for (const char c : grid.vec) {
    if (c == '0') {
      result += solve_part_two_recurse(grid, x, y);
    }

    ++x;
    if (x >= grid.width) {
      ++y;
      x = 0;
    }
  }
  return result;
}

auto part_two(const string &input) -> expected<AnswerType, string> {
  AnswerType result = 0;
  auto grid = CharGrid(input);
  result = solve_part_two(grid);
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
  println(cout, "Day 10");
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
