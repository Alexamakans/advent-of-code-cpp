#include <algorithm>
#include <expected>
#include <format>
#include <iostream>
#include <optional>
#include <sstream>

#include "util.hpp"

using std::cout, std::println;
using std::string;
using std::unexpected, std::expected;

typedef int AnswerType;

class Rule {
public:
  int left, right;

  Rule(int left, int right) : left(left), right(right) {}
  bool allows(const std::vector<int> &updates) const {
    auto begin = updates.cbegin();
    auto end = updates.cend();
    auto a_index = std::find(begin, end, left) - begin;
    auto b_index = std::find(begin, end, right) - begin;
    return a_index == updates.size() || b_index == updates.size() ||
           a_index < b_index;
  }

  string to_string() const { return std::format("{}|{}", left, right); }
};

template <> struct std::formatter<Rule> {
  constexpr auto parse(std::format_parse_context &context) {
    return context.begin();
  }
  auto format(const Rule &sVal, std::format_context &context) const {
    return std::format_to(context.out(), "{}", sVal.to_string());
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
        goto nextline_p1;
      }
    }
    result += updates.at(updates.size() / 2);
  nextline_p1:
  }

  return result;
}

auto part_two(const string &input) -> expected<AnswerType, string> {
  AnswerType result = 0;

  std::istringstream lines(input);
  auto rules = parse_rules(lines);
  string line;
  while (std::getline(lines, line)) {
    std::vector<int> updates = split(line, ',', parse::to_int);
    bool is_sorted = true;
    for (const auto &rule : rules) {
      if (!rule.allows(updates)) {
        is_sorted = false;
        break;
      }
    }
    if (is_sorted) {
      continue;
    }

    auto get_rule = [&rules](int a, int b) -> std::optional<Rule> {
      for (const auto &rule : rules) {
        if ((rule.left == a && rule.right == b) ||
            (rule.left == b && rule.right == a)) {
          return rule;
        }
      }
      return std::nullopt;
    };

    while (true) {
      bool swapped = false;
      for (auto a = updates.begin(); a != updates.end() - 1; ++a) {
        for (auto b = a + 1; b != updates.end(); ++b) {
          const auto &rule = get_rule(*a, *b);
          if (rule.has_value()) {
            if (rule.value().right == *a) {
              swapped = true;
              std::iter_swap(a, b);
            }
          }
        }
      }
      if (!swapped) {
        break;
      }
    }

    int middle_value = updates.at(updates.size() / 2);
    result += middle_value;
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
