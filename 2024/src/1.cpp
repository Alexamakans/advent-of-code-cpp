#include <expected>
#include <iostream>
#include <sstream>

#include "util.hpp"

using std::cout, std::println;
using std::string;
using std::unexpected, std::expected;

typedef int AnswerType;

auto part_one(const string &input) -> expected<int, string> {
  std::vector<int> left, right;

  std::istringstream lines(input);
  string line;
  while (std::getline(lines, line)) {
    std::vector<int> parts = split(line, ' ', parse::to_int);
    if (parts.size() != 2) {
      return unexpected(
          std::format("expected 2 parts per line, got {}", parts.size()));
    }

    left.emplace_back(parts.at(0));
    right.emplace_back(parts.at(1));
  }

  std::sort(left.begin(), left.end());
  std::sort(right.begin(), right.end());

  int sum_differences = 0;
  for (int i = 0; i < left.size(); i++) {
    sum_differences += abs(left.at(i) - right.at(i));
  }

  return sum_differences;
}

auto part_two(const string &input) -> expected<int, string> {
  std::vector<int> left, right;

  std::istringstream lines(input);
  string line;
  while (std::getline(lines, line)) {
    auto parts = split(line, ' ', parse::to_int);
    if (parts.size() != 2) {
      return unexpected(
          std::format("expected 2 parts per line, got {}", parts.size()));
    }

    left.emplace_back(parts.at(0));
    right.emplace_back(parts.at(1));
  }

  int sum_similarity = 0;

  for (const auto value : left) {
    int occurrences = std::count(right.begin(), right.end(), value);
    int similarity = value * occurrences;
    sum_similarity += similarity;
  }

  return sum_similarity;
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
  println(cout, "Day 1");
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

