#include <expected>
#include <format>
#include <iostream>
#include <sstream>

#include "util.hpp"

using std::cout, std::println;
using std::string;
using std::unexpected, std::expected;

typedef int AnswerType;

auto part_one(const string &input) -> expected<AnswerType, string> {
  AnswerType result = 0;
  struct Tile {
    char c;
    bool visited;
  };
  std::vector<std::vector<Tile>> grid;

  {
    std::istringstream lines(input);
    string line;
    while (std::getline(lines, line)) {
      grid.push_back({});
      for (const char c : line) {
        grid.back().push_back({.c = c, .visited = false});
      }
    }
  }

  int sx, sy;
  constexpr int RIGHT = 0;
  constexpr int DOWN = 1;
  constexpr int LEFT = 2;
  constexpr int UP = 3;
  int direction;

  for (const auto &[y, line] : std::ranges::views::enumerate(grid)) {
    for (const auto &[x, tile] : std::ranges::views::enumerate(line)) {
      if (tile.c == '^' || tile.c == '>' || tile.c == 'v' || tile.c == '<') {
        switch (tile.c) {
        case '>':
          direction = RIGHT;
          break;
        case 'v':
          direction = DOWN;
          break;
        case '<':
          direction = LEFT;
          break;
        case '^':
          direction = UP;
          break;
        }
        sx = x;
        sy = y;
        tile.visited = true;
        ++result;
        goto found_start_position_p1;
      }
    }
  }
found_start_position_p1:

  int x = sx;
  int y = sy;
  while (true) {
    int next_x = x;
    int next_y = y;
    switch (direction) {
    case RIGHT:
      ++next_x;
      break;
    case DOWN:
      ++next_y;
      break;
    case LEFT:
      --next_x;
      break;
    case UP:
      --next_y;
      break;
    }

    if (next_x >= 0 && next_x < grid.at(0).size() && next_y >= 0 &&
        next_y < grid.size()) {
      auto &tile = grid.at(next_y).at(next_x);
      if (tile.c == '#') {
        direction = (direction + 1) % 4;
      } else {
        x = next_x;
        y = next_y;
        if (!tile.visited) {
          ++result;
          tile.visited = true;
        }
      }
    } else {
      break;
    }
  }

  return result;
}

auto part_two(const string &input) -> expected<AnswerType, string> {
  // We probably need to keep track of the direction that we leaved a tile with.
  //
  // If we can place an obstruction to replicate the direction we leaved the tile with we
  // are likely to get caught in a loop? The obstruction could interfer with the path back though.
  //
  // Idk
  AnswerType result = 0;

  std::istringstream lines(input);
  string line;
  while (std::getline(lines, line)) {
    // Do stuff
  }

  if (false) {
    return unexpected("false is true?!");
  }
  return result;
}

int main() {
  std::ostringstream buffer;
  buffer << std::cin.rdbuf();
  string input = buffer.str();

  println(cout, " --- PART 1 LOGS ---");
  AnswerType part_one_result =
      part_one(input)
          .or_else([](string error) {
            println(cout, "\033[1;31m{}\033[0m", error);
            return expected<AnswerType, string>(0);
          })
          .value();
  println(cout);
  println(cout);

  println(cout, " --- PART 2 LOGS ---");
  AnswerType part_two_result =
      part_two(input)
          .or_else([](string error) {
            println(cout, "\033[1;31m{}\033[0m", error);
            return expected<AnswerType, string>(0);
          })
          .value();
  println(cout);
  println(cout);

  println(cout, "-----------------------------------------");
  println(cout, "Day 1");
  println(cout, "\tPart 1");
  println(cout, "\t\tAnswer: {}", part_one_result);
  println(cout, "\tPart 2");
  println(cout, "\t\tAnswer: {}", part_two_result);
  println(cout, "-----------------------------------------");
  return 0;
}
