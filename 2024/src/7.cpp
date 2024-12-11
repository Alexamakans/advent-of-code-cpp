#include <algorithm>
#include <cmath>
#include <cstdint>
#include <cstdlib>
#include <expected>
#include <format>
#include <iostream>
#include <sstream>
#include <string>

#include "util.hpp"

using std::cout, std::println;
using std::string;
using std::unexpected, std::expected;

#define MULTITHREAD
typedef uint64_t AnswerType;

struct Equation {
  uint64_t target;
  std::vector<uint64_t> parts;
};

AnswerType get_max_part_two(const Equation &e) {
  AnswerType result = e.parts[0];
  for (size_t i = 1; i < e.parts.size(); i++) {
    auto operand = e.parts[i];
    auto product = result * operand;
    auto sum = result + operand;
    auto joined = concatenate(result, operand);
    result = std::max(product, std::max(sum, joined));
  }

  return result;
}

std::tuple<AnswerType, AnswerType> get_bounds_part_two(const Equation &e) {
  AnswerType min = e.parts[0];
  AnswerType max = min;
  for (size_t i = 1; i < e.parts.size(); i++) {
    auto operand = e.parts[i];
    auto max_product = max * operand;
    auto max_sum = max + operand;
    auto max_joined = concatenate(max, operand);
    auto min_product = min * operand;
    auto min_sum = min + operand;
    auto min_joined = concatenate(min, operand);
    min = std::min(min_product, std::min(min_sum, min_joined));
    max = std::max(max_product, std::max(max_sum, max_joined));
  }

  return std::make_tuple(min, max);
}

Equation parse_line(const std::string &line) {
  Equation out;

  auto target_and_parts = split(line, ':', parse::to_string);
  out.target = parse::to_uint64(target_and_parts.at(0)).value();
  out.parts = std::move(split(target_and_parts.at(1), ' ', parse::to_uint64));

  return out;
};

bool bruteforce_part_one(const Equation &e) {
  // 0 -> addition
  // 1 -> multiplication
  // right-most bit (LSB) is the operation applied to the first pairing
  uint64_t operations = 0;
  uint64_t limit = 1ull << (e.parts.size() - 1);
  while (operations < limit) {
    uint64_t result = e.parts.at(0);
    for (int i = 1; i < e.parts.size(); ++i) {
      uint64_t operand = e.parts.at(i);
      if (operations & (1 << (i - 1))) {
        result *= operand;
      } else {
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

bool bruteforce_part_two(const Equation &e) {
  // 0 -> addition
  // 1 -> multiplication
  // 2 -> join
  std::vector<int> operations;
  operations.assign(e.parts.size(), 0);
  bool loop = true;
  while (loop) {
    uint64_t result = e.parts.at(0);
    for (int i = 1; i < e.parts.size(); ++i) {
      uint64_t operand = e.parts[i];
      switch (operations[i - 1]) {
      case 2: {
        result = concatenate(result, operand);
        break;
      }
      case 1: {
        result *= operand;
        break;
      }
      case 0: {
        result += operand;
        break;
      }
      }
    }
    if (result == e.target) {
      return true;
    }

    size_t idx = 0;
    while (idx < operations.size()) {
      if (++operations[idx] > 2) {
        operations[idx] = 0;
        ++idx;
      } else {
        break;
      }
    }
    if (idx == operations.size()) {
      break;
    }
  }

  return false;
}

#ifdef MULTITHREAD
#include <future>

AnswerType process_line_part_one(const std::string &line) {
  const auto equation = parse_line(line);
  if (bruteforce_part_one(equation)) {
    return equation.target;
  }
  return 0;
}

AnswerType process_batch_part_one(std::vector<std::string> batch) {
  AnswerType result = 0;
  for (const auto &line : batch) {
    result += process_line_part_one(line);
  }
  return result;
}

AnswerType process_line_part_two(const std::string &line) {
  const auto equation = parse_line(line);
  const auto [min, max] = get_bounds_part_two(equation);
  if (equation.target < min || equation.target > max) {
    return 0;
  }
  if (bruteforce_part_two(equation)) {
    return equation.target;
  }
  return 0;
}

AnswerType process_batch_part_two(std::vector<std::string> batch) {
  AnswerType result = 0;
  for (const auto &line : batch) {
    result += process_line_part_two(line);
  }
  return result;
}

auto part_one(const string &input) -> expected<AnswerType, string> {
  AnswerType result = 0;
  std::vector<Equation> equations;
  int total_lines = 850;
  int lines_per_batch = 110;
  std::vector<std::vector<string>> batches;
  batches.reserve((total_lines / lines_per_batch) + 1);
  std::vector<string> current_batch;
  current_batch.reserve(lines_per_batch);
  int counter = 0;

  std::istringstream lines(input);
  string line;
  while (std::getline(lines, line)) {
    current_batch.push_back(line);
    ++counter;
    if (counter >= lines_per_batch) {
      batches.push_back(std::move(current_batch));
      counter = 0;
    }
  }
  if (counter != 0) {
    batches.push_back(std::move(current_batch));
  }

  std::vector<std::future<AnswerType>> futures;
  for (const auto &batch : batches) {
    futures.push_back(std::async(&process_batch_part_one, batch));
  }

  for (auto &future : futures) {
    result += future.get();
  }

  return result;
}

auto part_two(const string &input) -> expected<AnswerType, string> {
  AnswerType result = 0;
  std::vector<Equation> equations;
  int total_lines = 850;
  int lines_per_batch = 110;
  std::vector<std::vector<string>> batches;
  batches.reserve((total_lines / lines_per_batch) + 1);
  std::vector<string> current_batch;
  current_batch.reserve(lines_per_batch);
  int counter = 0;

  std::istringstream lines(input);
  string line;
  while (std::getline(lines, line)) {
    current_batch.push_back(line);
    ++counter;
    if (counter >= lines_per_batch) {
      batches.push_back(std::move(current_batch));
      counter = 0;
    }
  }
  if (counter != 0) {
    batches.push_back(std::move(current_batch));
  }

  std::vector<std::future<AnswerType>> futures;
  for (const auto &batch : batches) {
    futures.push_back(std::async(&process_batch_part_two, batch));
  }

  for (auto &future : futures) {
    result += future.get();
  }

  return result;
}
#else
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
#endif

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
