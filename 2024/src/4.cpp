#include <algorithm>
#include <cstdlib>
#include <expected>
#include <iostream>
#include <sstream>

#include "util.hpp"

using std::cout, std::println;
using std::string;
using std::unexpected, std::expected;

typedef int AnswerType;

size_t index_from_xy(size_t grid_side_length, size_t x, size_t y);

bool search(const std::vector<char> &grid, size_t grid_side_length,
            long startIndex, long dx, long dy);

auto part_one(const string &input) -> expected<AnswerType, string> {
  AnswerType result = 0;
  size_t grid_side_length;

  std::istringstream lines(input);
  string line;
  std::vector<char> grid;
  while (std::getline(lines, line)) {
    grid_side_length = line.size();
    std::copy(line.begin(), line.end(), std::back_inserter(grid));
  }

  for (auto const [i, c] : std::views::enumerate(grid)) {
    for (int dx = -1; dx <= 1; ++dx) {
      for (int dy = -1; dy <= 1; ++dy) {
        if (dx == 0 && dy == 0) {
          continue;
        }

        if (search(grid, grid_side_length, i, dx, dy)) {
          ++result;
        }
      }
    }
  }

  return result;
}

auto part_two(const string &input) -> expected<AnswerType, string> {
  AnswerType result = 0;
  size_t grid_side_length;

  std::istringstream lines(input);
  string line;
  std::vector<char> grid;
  while (std::getline(lines, line)) {
    grid_side_length = line.size();
    std::copy(line.begin(), line.end(), std::back_inserter(grid));
  }

  for (auto const [i, c] : std::views::enumerate(grid)) {
    if (c != 'A') {
      continue;
    }

    const long sx = i % grid_side_length;
    const long sy = i / grid_side_length;

    if (sx < 1 || sx >= grid_side_length - 1 || sy < 1 ||
        sy >= grid_side_length - 1) {
      continue;
    }

    const size_t upper_left_index =
        index_from_xy(grid_side_length, sx - 1, sy - 1);
    const size_t upper_right_index =
        index_from_xy(grid_side_length, sx + 1, sy - 1);
    const size_t bottom_right_index =
        index_from_xy(grid_side_length, sx + 1, sy + 1);
    const size_t bottom_left_index =
        index_from_xy(grid_side_length, sx - 1, sy + 1);

    const char upper_left = grid.at(upper_left_index);
    const char bottom_right = grid.at(bottom_right_index);

    if (!((upper_left == 'M' && bottom_right == 'S') ||
          (upper_left == 'S' && bottom_right == 'M'))) {
      continue;
    }

    const char bottom_left = grid.at(bottom_left_index);
    const char upper_right = grid.at(upper_right_index);

    if (!((bottom_left == 'M' && upper_right == 'S') ||
          (bottom_left == 'S' && upper_right == 'M'))) {
      continue;
    }

    ++result;
  }

  return result;
}

size_t index_from_xy(size_t grid_side_length, size_t x, size_t y) {
  return x + y * grid_side_length;
}

bool search(const std::vector<char> &grid, size_t grid_side_length,
            long startIndex, long dx, long dy) {
  const long sx = startIndex % grid_side_length;
  const long sy = startIndex / grid_side_length;

  constexpr char needle[5] = "XMAS";

  for (int i = 0; i < (sizeof(needle) / sizeof(needle[0])) - 1; ++i) {
    const long x = sx + dx * i;
    const long y = sy + dy * i;
    if (x < 0 || x >= grid_side_length || y < 0 || y >= grid_side_length) {
      return false;
    }
    const size_t check_index = index_from_xy(grid_side_length, x, y);

    const char c = grid.at(check_index);
    if (c != needle[i]) {
      return false;
    }
  }
  return true;
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
  println(cout, "Day 4");
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

