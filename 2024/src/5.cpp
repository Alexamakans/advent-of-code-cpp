#include <algorithm>
#include <expected>
#include <iostream>
#include <sstream>

#include "util.hpp"

using std::cout, std::println;
using std::string;
using std::unexpected, std::expected;

typedef int AnswerType;

class Rule {
  int a, b;

public:
  Rule(int a, int b) : a(a), b(b) {}
  bool allows(const std::vector<int> &updates) const {
    auto a_index =
        std::find(updates.cbegin(), updates.cend(), a) - updates.cbegin();
    auto b_index =
        std::find(updates.cbegin(), updates.cend(), b) - updates.cbegin();
    return a_index == updates.size() || b_index == updates.size() ||
           a_index < b_index;
  }
};

std::vector<Rule> parse_rules(std::istringstream &lines) {
  std::vector<Rule> rules;
  string line;
  while (std::getline(lines, line)) {
    if (line.empty()) {
      return rules;
    }
    auto parts = split(line, '|', parse::to_int);
    rules.push_back(Rule(parts.at(0), parts.at(1)));
  }
  return rules;
}

auto part_one(const string &input) -> expected<AnswerType, string> {
  AnswerType result = 0;

  std::istringstream lines(input);
  auto rules = parse_rules(lines);
  string line;
  while (std::getline(lines, line)) {
    std::vector<int> updates = split(line, ',', parse::to_int);
    for (const auto &rule : rules) {
      if (!rule.allows(updates)) {
        goto nextline;
      }
    }
    result += updates.at(updates.size() / 2);
  nextline:
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
