#include <cstdint>
#include <expected>
#include <format>
#include <iostream>
#include <sstream>
#include <string>

#include "util.hpp"

using std::cout, std::println;
using std::string;
using std::unexpected, std::expected;

typedef uint64_t AnswerType;

struct Equation {
  uint64_t target;
  std::vector<uint64_t> parts;
};

Equation parse_line(const std::string &line) {
  Equation out;

  auto target_and_parts = split(line, ':', parse::to_string);
  out.target = parse::to_uint64(target_and_parts.at(0)).value();
  out.parts = split(target_and_parts.at(1), ' ', parse::to_uint64);

  return out;
};

bool bruteforce_part_one(const Equation &e) {
  // 0 -> addition
  // 1 -> multiplication
  // right-most bit (LSB) is the operation applied to the first pairing
  uint64_t operations = 0;
  uint64_t limit = 2 << (e.parts.size() - 2);
  while (operations < limit) {
    uint64_t result = e.parts.at(0);
    for (int i = 1; i < e.parts.size(); ++i) {
      uint64_t operand = e.parts.at(i);
      int operator_index = ((operations >> (i - 1)) & 1);
      if (operator_index == 1) {
        result *= operand;
      } else if (operator_index == 0) {
        result += operand;
      }
    }
    if (result == e.target) {
      return true;
    }
    ++operations;
  }

  return false;
}

uint64_t get_asdf(uint64_t v) {
  int asdf = 10;
  while (true) {
    if (v % asdf == v) {
      return asdf;
    }
    asdf *= 10;
  }
}

bool bruteforce_part_two(const Equation &e) {
  // 0 -> addition
  // 1 -> multiplication
  // 2 -> join
  std::vector<int> operations;
  for (const auto &p : e.parts) {
    operations.push_back(0);
  }
  bool loop = true;
  while (loop) {
    uint64_t result = e.parts.at(0);
    for (int i = 1; i < e.parts.size(); ++i) {
      uint64_t operand = e.parts.at(i);
      int operator_index = operations.at(i - 1);
      if (operator_index == 2) {
        auto asdf = get_asdf(operand);
        result = result * asdf + operand;
      } else if (operator_index == 1) {
        result *= operand;
      } else if (operator_index == 0) {
        result += operand;
      }
    }
    if (result == e.target) {
      return true;
    }

    while (true) {
      for (auto [i, op] : std::ranges::views::enumerate(operations)) {
        if (op == 2) {
          op = 0;
          if (i == operations.size() - 1) {
            return false;
          }
        } else {
          ++op;
          goto nextpls;
        }
      }
    }
  nextpls:
  }

  return false;
}

auto part_one(const string &input) -> expected<AnswerType, string> {
  AnswerType result = 0;
  std::vector<Equation> equations;

  std::istringstream lines(input);
  string line;
  while (std::getline(lines, line)) {
    const auto equation = parse_line(line);
    if (bruteforce_part_one(equation)) {
      result += equation.target;
      continue;
    }
  }

  return result;
}

auto part_two(const string &input) -> expected<AnswerType, string> {
  AnswerType result = 0;
  std::vector<Equation> equations;

  std::istringstream lines(input);
  string line;
  while (std::getline(lines, line)) {
    const auto equation = parse_line(line);
    if (bruteforce_part_two(equation)) {
      result += equation.target;
      continue;
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
