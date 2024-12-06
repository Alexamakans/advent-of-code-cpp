#include <format>
#include <iostream>
#include <sstream>
#include <string>
#include <expected>

#include "util.cpp"

using std::string;
using std::unexpected, std::expected;
using std::cout, std::println;

auto part_one(const string &input) -> expected<int, string> {
  if (false) {
    return unexpected("false is true?");
  }
  return 0;
}

auto part_two(const string &input) -> expected<int, string> {
  if (false) {
    return unexpected("false is true?");
  }
  return 0;
}

int main() {
  std::ostringstream buffer;
  buffer << std::cin.rdbuf();
  string input = buffer.str();

  println(cout, " --- PART 1 LOGS ---");
  int part_one_result = part_one(input).or_else([](string error) {
    println(cout, "\033[1;31m{}\033[0m", error);
    return expected<int, string>(0);
  }).value();
  println(cout);
  println(cout);

  println(cout, " --- PART 2 LOGS ---");
  int part_two_result = part_two(input).or_else([](string error) {
    println(cout, "\033[1;31m{}\033[0m", error);
    return expected<int, string>(0);
  }).value();
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
