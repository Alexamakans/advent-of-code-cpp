#include <algorithm>
#include <expected>
#include <format>
#include <iostream>
#include <optional>
#include <sstream>
#include <unordered_map>

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

std::tuple<std::vector<Rule>, std::unordered_map<int, Rule>>
parse_rules(std::istringstream &lines) {
  std::vector<Rule> rules;
  std::unordered_map<int, Rule> mappings;
  string line;
  while (std::getline(lines, line)) {
    if (line.empty()) {
      return std::make_tuple(rules, mappings);
    }
    auto parts = split(line, '|', parse::to_int);
    auto left = parts.at(0);
    auto right = parts.at(1);
    auto rule = Rule(left, right);
    rules.push_back(rule);
    mappings.insert_or_assign(concatenate(left, right), rule);
  }
  return std::make_tuple(rules, mappings);
}

auto part_one(const string &input) -> expected<AnswerType, string> {
  AnswerType result = 0;

  std::istringstream lines(input);
  auto [rules, rulemap] = parse_rules(lines);
  string line;
  std::vector<int> updates;
  while (std::getline(lines, line)) {
    updates = split(line, ',', parse::to_int);
    if (std::all_of(rules.cbegin(), rules.cend(),
                    [&](const auto &rule) { return rule.allows(updates); })) {
      result += updates.at(updates.size() / 2);
    }
  }

  return result;
}

auto part_two(const string &input) -> expected<AnswerType, string> {
  AnswerType result = 0;

  std::istringstream lines(input);
  auto [rules, rulemap] = parse_rules(lines);
  string line;
  std::vector<int> updates;
  while (std::getline(lines, line)) {
    updates = split(line, ',', parse::to_int);
    bool is_sorted = std::all_of(rules.cbegin(), rules.cend(), [&](auto &rule) {
      return rule.allows(updates);
    });
    if (is_sorted) {
      continue;
    }

    std::sort(updates.begin(), updates.end(),
              [&](const auto &a, const auto &b) {
                auto key = concatenate(b, a);
                if (!rulemap.contains(key)) {
                  return 0;
                }
                return 1;
              });

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
  println(cout, "Day 5");
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

