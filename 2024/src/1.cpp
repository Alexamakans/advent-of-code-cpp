#include <algorithm>
#include <cstdlib>
#include <expected>
#include <format>
#include <iostream>
#include <sstream>
#include <string>

#include "util.cpp"

using std::cout, std::println;
using std::string;
using std::unexpected, std::expected;

auto part_one(const string &input) -> expected<int, string> {
  /*
   * Pair up the smallest number in the left list with the smallest number in
   * the right list, second smallest with the second smallest, etc.
   * Output the sum of the difference of each pair.
   */

  std::istringstream lines(input);
  string line;

  std::vector<int> left, right;

  while (std::getline(lines, line)) {
    auto parts = split_to_ints(line, ' ');
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
  /*
   * This time, you'll need to figure out exactly how often each number from the
   * left list appears in the right list. Calculate a total similarity score by
   * adding up each number in the left list after multiplying it by the number
   * of times that number appears in the right list.
   *
   * sum for all i: left(i) * count(right, left(i))
   */

  std::istringstream lines(input);
  string line;

  std::vector<int> left, right;

  while (std::getline(lines, line)) {
    auto parts = split_to_ints(line, ' ');
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
  int part_one_result = part_one(input)
                            .or_else([](string error) {
                              println(cout, "\033[1;31m{}\033[0m", error);
                              return expected<int, const char *>(0);
                            })
                            .value();
  println(cout);
  println(cout);

  println(cout, " --- PART 2 LOGS ---");
  int part_two_result = part_two(input)
                            .or_else([](string error) {
                              println(cout, "\033[1;31m{}\033[0m", error);
                              return expected<int, const char *>(0);
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
