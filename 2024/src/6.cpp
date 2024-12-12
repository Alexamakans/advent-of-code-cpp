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

struct Step {
  int direction;
  Point from;

  bool operator==(const Step &rhs) const {
    return direction == rhs.direction && from == rhs.from;
  }
};

struct step_hash {
  std::size_t operator()(const Step &v) const {
    return std::hash<int>{}(v.direction) ^ std::hash<Point>{}(v.from);
  }
};

// x,y,direction
std::tuple<int, int, int>
get_starting_position(const std::vector<std::vector<char>> &grid) {
  for (const auto &[ty, line] : std::ranges::views::enumerate(grid)) {
    for (const auto &[tx, c] : std::ranges::views::enumerate(line)) {
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
      default:
        continue;
      }
      return {tx, ty, direction};
    }
  }
  throw "oh no";
}

// Returns the steps taken, and if it resulted in a loop or not.
std::tuple<std::unordered_set<Step, step_hash>, bool> walk(
    const std::vector<std::vector<char>> &grid,
    const Point &obstacle = Point{-1, -1},
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
      if (next_x == obstacle.x && next_y == obstacle.y) {
        direction = (direction + 1) % 4;
      } else {
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
      }
    } else {
      break;
    }
  }

  return std::make_tuple(steps, false);
}

typedef std::vector<Point> Batch;
typedef AnswerType BatchResult;
typedef AnswerType FinalResult;

struct Provider {
  int batch_size;
  std::istringstream lines;
  std::string line_buffer;
  Batch batch_buffer;

  bool prepared = false;
  std::tuple<int, int, int> starting_point;
  std::vector<std::vector<char>> grid;
  std::vector<Point> unique_points;

  void prepare(const std::string &input) {
    if (prepared) {
      return;
    }

    AnswerType result = 0;

    {
      std::istringstream lines(input);
      string line;
      while (std::getline(lines, line)) {
        grid.emplace_back(line.begin(), line.end());
      }
    }

    const auto &[steps, loop] = walk(grid);
    std::unordered_set<Point> unique_points_set;
    for (const auto &step : steps) {
      const auto &[it, inserted] = unique_points_set.insert(step.from);
      if (inserted) {
        unique_points.push_back(*it);
      }
    }

    starting_point = get_starting_position(grid);
    prepared = true;
  }

  Batch provide() {
    for (int i = 0; i < batch_size; ++i) {
      if (unique_points.size() == 0) {
        break;
      }
      batch_buffer.push_back(unique_points.back());
      unique_points.pop_back();
    }

    return std::move(batch_buffer);
  }
  bool done() const { return unique_points.size() == 0; }
};

struct Consumer {
  std::tuple<int, int, int> starting_point;
  std::vector<std::vector<char>> *grid;
  BatchResult consume(Batch input) const {
    BatchResult result = 0;
    for (const auto &p : input) {
      auto &c = grid->at(p.y).at(p.x);
      if (c == '^' || c == '>' || c == 'v' || c == '<') {
        continue;
      }
      const auto &[_, loop] = walk(*grid, p, starting_point);
      if (loop) {
        ++result;
      }
    }

    return result;
  }
};

struct Solver {
  Provider provider;
  Consumer consumer;

  FinalResult combine(FinalResult accumulator, BatchResult value) const {
    return accumulator + value;
  }
};

auto make_solver(int batch_size)
    -> Multithreader<Batch, BatchResult, FinalResult> auto {
  auto out = Solver{};
  out.provider.batch_size = batch_size;
  return out;
}

auto solver = make_solver(0);
auto part_one(const string &input) -> expected<AnswerType, string> {
  solver.provider.prepare(input);
  auto num_unique_points = solver.provider.unique_points.size();
  solver.provider.batch_size = num_unique_points / 2 + 1;
  return num_unique_points;
}

auto part_two(const string &input) -> expected<AnswerType, string> {
  solver.consumer.grid = &solver.provider.grid;
  solver.consumer.starting_point = solver.provider.starting_point;
  return execute<Batch, BatchResult>(input, solver, 0);
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
  println(cout, "Day 6");
  println(cout, "\tPart 1");
  println(cout, "\t\tAnswer: {}", part_one_result);
  println(cout, "\tPart 2");
  println(cout, "\t\tAnswer: {}", part_two_result);
  println(cout, "-----------------------------------------");
  return 0;
}
