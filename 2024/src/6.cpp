#include <expected>
#include <format>
#include <iostream>
#include <sstream>
#include <unordered_set>

#include "util.hpp"

using std::cout, std::println;
using std::string;
using std::unexpected, std::expected;

typedef int AnswerType;

constexpr int RIGHT = 0;
constexpr int DOWN = 1;
constexpr int LEFT = 2;
constexpr int UP = 3;

struct Point {
  int x, y;

  bool operator==(const Point &rhs) const { return x == rhs.x && y == rhs.y; }
};

struct Step {
  int direction;
  Point from;

  bool operator==(const Step &rhs) const {
    return direction == rhs.direction && from == rhs.from;
  }
};

struct point_hash {
  std::size_t operator()(const Point &v) const {
    return v.x * 10 + v.y * 1000000;
  }
};

struct step_hash {
  std::size_t operator()(const Step &v) const {
    return v.direction + point_hash{}(v.from);
    ;
  }
};

// x,y,direction
std::tuple<int, int, int>
get_starting_position(const std::vector<std::vector<char>> &grid) {
  for (const auto &[ty, line] : std::ranges::views::enumerate(grid)) {
    for (const auto &[tx, c] : std::ranges::views::enumerate(line)) {
      if (c == '^' || c == '>' || c == 'v' || c == '<') {
        int direction;
        switch (c) {
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
        return {tx, ty, direction};
      }
    }
  }
  throw "oh no";
}

// Returns the steps taken, and if it resulted in a loop or not.
std::tuple<std::unordered_set<Step, step_hash>, bool> walk(
    const std::vector<std::vector<char>> &grid,
    std::optional<std::tuple<int, int, int>> starting_position = std::nullopt) {
  if (!starting_position) {
    starting_position = get_starting_position(grid);
  }
  auto [x, y, direction] = starting_position.value();
  std::unordered_set<Step, step_hash> steps;
  steps.insert({direction, {x, y}});
  int width = grid.at(0).size();
  int height = grid.size();
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

    if (next_x >= 0 && next_x < width && next_y >= 0 && next_y < height) {
      char c = grid.at(next_y).at(next_x);
      Step step{direction, {next_x, next_y}};
      bool duplicate = steps.contains(step);
      if (duplicate) {
        return std::make_tuple(steps, true);
      }
      if (c == '#') {
        direction = (direction + 1) % 4;
      } else {
        x = next_x;
        y = next_y;
        steps.insert(step);
      }
    } else {
      break;
    }
  }

  return std::make_tuple(steps, false);
}

auto part_one(const string &input) -> expected<AnswerType, string> {
  AnswerType result = 0;
  std::vector<std::vector<char>> grid;

  {
    std::istringstream lines(input);
    string line;
    while (std::getline(lines, line)) {
      grid.emplace_back(line.begin(), line.end());
    }
  }

  const auto &[steps, loop] = walk(grid);
  std::unordered_set<Point, point_hash> unique_points;
  for (const auto &step : steps) {
    unique_points.insert(step.from);
  }
  result = unique_points.size();

  return result;
}

auto part_two(const string &input) -> expected<AnswerType, string> {
  AnswerType result = 0;
  std::vector<std::vector<char>> grid;

  {
    std::istringstream lines(input);
    string line;
    while (std::getline(lines, line)) {
      grid.emplace_back(line.begin(), line.end());
    }
  }

  const auto &[steps, loop] = walk(grid);
  std::unordered_set<Point, point_hash> unique_points;
  for (const auto &step : steps) {
    unique_points.insert(step.from);
  }

  const auto starting_point = get_starting_position(grid);

  std::optional<Point> previous;
  for (const auto &p : unique_points) {
    if (previous) {
      auto &c = grid.at(previous.value().y).at(previous.value().x);
      c = '.';
    }
    auto &c = grid.at(p.y).at(p.x);
    if (c == '^' || c == '>' || c == 'v' || c == '<') {
      continue;
    }
    previous = p;
    c = '#';
    const auto &[_, loop] = walk(grid, starting_point);
    if (loop) {
      ++result;
    }
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
