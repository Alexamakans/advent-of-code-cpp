#include <cmath>
#include <cstdint>
#include <cstdlib>
#include <expected>
#include <format>
#include <functional>
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

bool can_reach(const std::vector<char> &operators, const Equation &e) {
  const int n = e.parts.size();

  std::function<bool(AnswerType, AnswerType)> backtrack =
      [&](int index, AnswerType val) -> bool {
    if (index == n) {
      return val == e.target;
    }

    for (char op : operators) {
      AnswerType nextValue = val;
      switch (op) {
      case '+':
        nextValue += e.parts[index];
        break;
      case '*':
        nextValue *= e.parts[index];
        break;
      case '|':
        nextValue = concatenate(nextValue, e.parts[index]);
        break;
      }

      if (backtrack(index + 1, nextValue)) {
        return true;
      }
    }

    return false;
  };

  return backtrack(1, e.parts[0]);
}

typedef std::vector<string> Batch;
typedef AnswerType BatchResult;
typedef AnswerType FinalResult;

struct Provider {
  int batch_size;
  std::istringstream lines;
  std::string line_buffer;
  Batch batch_buffer;

  void prepare(const std::string &input) { lines = std::istringstream(input); }

  Batch provide() {
    for (int i = 0; i < batch_size && std::getline(lines, line_buffer); ++i) {
      batch_buffer.push_back(line_buffer);
    }

    return std::move(batch_buffer);
  }
  bool done() const { return lines.eof(); }
};

struct Consumer {
  std::vector<char> operators;
  BatchResult consume(Batch input) const {
    BatchResult result = 0;
    for (const auto &line : input) {
      const Equation equation = parse_line(line);
      if (can_reach(operators, equation)) {
        result += equation.target;
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

auto make_solver(int batch_size, std::vector<char> operators)
    -> Multithreader<Batch, BatchResult, FinalResult> auto {
  auto out = Solver{};
  out.provider.batch_size = batch_size;
  out.consumer.operators = operators;
  return out;
}

auto part_one(const std::string &input) -> expected<AnswerType, string> {
  std::vector<char> operators{'+', '*'};
  auto solver_instance = make_solver(425, operators);
  AnswerType result =
      execute<Batch, BatchResult, AnswerType>(input, solver_instance, 0);
  return result;
}

auto part_two(const string &input) -> expected<AnswerType, string> {
  std::vector<char> operators{'+', '*', '|'};
  auto solver_instance = make_solver(425, operators);
  AnswerType result =
      execute<Batch, BatchResult, AnswerType>(input, solver_instance, 0);
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
  println(cout, "Day 7");
  println(cout, "\tPart 1");
  println(cout, "\t\tAnswer: {}", part_one_result);
  println(cout, "\tPart 2");
  println(cout, "\t\tAnswer: {}", part_two_result);
  println(cout, "-----------------------------------------");
  return 0;
}
