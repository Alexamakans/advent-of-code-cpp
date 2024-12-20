#include <expected>
#include <format>
#include <iostream>
#include <regex>
#include <sstream>

#include "util.hpp"

using std::cout, std::println;
using std::string;
using std::unexpected, std::expected;

typedef int AnswerType;

auto part_one(const string &input) -> expected<AnswerType, string> {
  AnswerType result = 0;
  std::regex re(R"RE(mul\((\d+,\d+)\))RE", std::regex_constants::ECMAScript);
  int submatches[] = {1};

  std::istringstream lines(input);
  string line;
  while (std::getline(lines, line)) {
    auto it =
        std::sregex_token_iterator(line.begin(), line.end(), re, submatches);
    auto end = std::sregex_token_iterator();
    for (; it != end; ++it) {
      const auto &match = *it;
      auto s = match.str();
      auto parts = split(s, ',', parse::to_int);
      result += parts.at(0) * parts.at(1);
    }
  }
  return result;
}

auto part_two(const string &input) -> expected<AnswerType, string> {
  AnswerType result = 0;
  std::regex re(R"RE(mul\((\d+,\d+)\)|(do\(\))|(don't\(\)))RE",
                std::regex_constants::ECMAScript);
  int submatches[] = {1, 2, 3};
  bool active = true;

  std::istringstream lines(input);
  string line;
  while (std::getline(lines, line)) {
    auto it =
        std::sregex_token_iterator(line.begin(), line.end(), re, submatches);
    auto end = std::sregex_token_iterator();
    for (; it != end; ++it) {
      const auto &match = *it;
      if (!match.matched) {
        continue;
      }

      auto s = match.str();
      if (s == "don't()") {
        active = false;
      } else if (s == "do()") {
        active = true;
      } else if (active) {
        auto parts = split(s, ',', parse::to_int);
        result += parts.at(0) * parts.at(1);
      }
    }
  }
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
  println(cout, "Day 3");
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

