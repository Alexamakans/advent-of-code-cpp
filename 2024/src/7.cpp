#include <cstddef>
#include <cstdint>
#include <expected>
#include <format>
#include <iostream>
#include <sstream>
#include <unordered_map>

#include "util.hpp"

using std::cout, std::println;
using std::string;
using std::unexpected, std::expected;

typedef uint64_t AnswerType;

struct Equation {
  uint64_t target;
  std::vector<uint64_t> parts;

  uint64_t get_max() const {
    uint64_t result = parts.at(0);
    for (int i = 1; i < parts.size(); ++i) {
      const auto &val = parts.at(i);
      if (val == 1) {
        result += val;
      } else {
        result *= val;
      }
    }
    return result;
  }

  uint64_t get_min() const {
    uint64_t result = parts.at(0);
    for (int i = 1; i < parts.size(); ++i) {
      const auto &val = parts.at(i);
      if (val == 1) {
        result *= val;
      } else {
        result += val;
      }
    }
    return result;
  }
};

Equation parse_line(const std::string &line) {
  Equation out;

  auto target_and_parts = split(line, ':', parse::to_string);
  out.target = parse::to_uint64(target_and_parts.at(0)).value();
  out.parts = split(target_and_parts.at(1), ' ', parse::to_uint64);

  return out;
};

struct vector_hash {
  std::size_t operator()(std::vector<uint64_t> const &vec) const {
    std::size_t seed = vec.size();
    for (auto x : vec) {
      x = ((x >> 16) ^ x) * 0x45d9f3b;
      x = ((x >> 16) ^ x) * 0x45d9f3b;
      x = (x >> 16) ^ x;
      seed ^= x + 0x9e3779b9 + (seed << 6) + (seed >> 2);
    }
    return seed;
  }
};

struct SolverState {
  uint64_t current_value;
  std::vector<uint64_t> remaining_parts;
};

struct solver_state_hash {
  size_t operator()(const SolverState &v) {
    size_t h = vector_hash{}(v.remaining_parts);
    auto x = v.current_value;
    x = ((x >> 16) ^ x) * 0x45d9f3b;
    x = ((x >> 16) ^ x) * 0x45d9f3b;
    x = ((x >> 16) ^ x);
    h ^= x + 0x9e3779b9 + (h << 6) + (h >> 2);
    return h;
  }
};

struct Solver {
  std::unordered_map<SolverState, std::vector<uint64_t>, solver_state_hash> cache;
  // stuff
};

auto part_one(const string &input) -> expected<AnswerType, string> {
  AnswerType result = 0;
  std::vector<Equation> equations;

  std::istringstream lines(input);
  string line;
  while (std::getline(lines, line)) {
    const auto equation = parse_line(line);
    const auto min = equation.get_min();
    if (min > equation.target) {
      continue;
    } else if (min == equation.target) {
      result += equation.target;
    }
    const auto max = equation.get_max();
    if (max < equation.target) {
      continue;
    } else if (max == equation.target) {
      result += equation.target;
      continue;
    }
    println(cout, "equation [ {}: {} ] MIGHT be possible ({} <= {} <= {})",
            equation.target, join(equation.parts, ", "), min, equation.target,
            max);
  }

  if (false) {
    return unexpected("false is true?!");
  }
  return result;
}

auto part_two(const string &input) -> expected<AnswerType, string> {
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
