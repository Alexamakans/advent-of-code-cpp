#!/usr/bin/env bash

SCRIPT_ROOT=$(pwd)
INPUT_DIR=$SCRIPT_ROOT/inputs
SOURCE_DIR=$SCRIPT_ROOT/src

if [[ $# -eq 0 ]] || [[ $# -gt 2 ]]; then
  echo "Expected 1 or 2 arguments"
  echo "$0 <DAY>"
  echo "$0 <DAY> test"
  exit 1
fi

DAY=$1
if [ $# -eq 2 ] && [ $2 = 'test' ]; then
  INPUT_DIR=$SCRIPT_ROOT/samples
fi

if [ $# -eq 1 ] && [ ! -f "$SCRIPT_ROOT/inputs/$DAY" ] && [ -f "$SCRIPT_ROOT/.env" ]; then
  . $SCRIPT_ROOT/.env
  if [ -z "$AOC_SESSION" ]; then
    echo "AOC_SESSION is not set"
    exit 1
  fi
  curl https://adventofcode.com/2024/day/$DAY/input --cookie "session=$AOC_SESSION" \
    > $SCRIPT_ROOT/inputs/$DAY
fi

function make_file() {
  cat << EOF
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
EOF
} > $SOURCE_DIR/${DAY}.cpp

if [ ! -f "$SOURCE_DIR/${DAY}.cpp" ]; then
  make_file
fi

if [ -f $SCRIPT_ROOT/bin/$DAY ]; then
  rm $SCRIPT_ROOT/bin/$DAY 2>/dev/null
fi

g++ -std=c++23 $SOURCE_DIR/${DAY}.cpp -o $SCRIPT_ROOT/bin/$DAY
clear
time $SCRIPT_ROOT/bin/$DAY < $INPUT_DIR/$DAY
