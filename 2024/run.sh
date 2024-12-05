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
#include <iostream>
#include <sstream>
#include <string>

int part_one(const std::string& input) {
  return 0;
}

int part_two(const std::string& input) {
  return 0;
}

int main() {
  std::ostringstream buffer;
  buffer << std::cin.rdbuf();
  std::string input = buffer.str();

  std::cout << " --- PART 1 LOGS ---" << std::endl;
  int part_one_result = part_one(input);
  std::cout << std::endl << std::endl;
  
  std::cout << " --- PART 2 LOGS ---" << std::endl;
  int part_two_result = part_two(input);
  std::cout << std::endl << std::endl;

  std::cout << "-----------------------------------------" << std::endl;
  std::cout << "Day 1" << std::endl;
  std::cout << "\tPart 1" << std::endl;
  std::cout << "\t\tAnswer: " << part_one_result << std::endl;
  std::cout << "\tPart 2" << std::endl;
  std::cout << "\t\tAnswer: " << part_two(input) << std::endl;
  std::cout << "-----------------------------------------" << std::endl;
  return 0;
}
EOF
} > $SOURCE_DIR/${DAY}.cpp

if [ ! -f "$SOURCE_DIR/${DAY}.cpp" ]; then
  make_file
fi

if [ -f $SCRIPT_ROOT/bin/$DAY ]; then
  rm $SCRIPT_ROOT/bin/$DAY >/dev/null
fi

g++ $SOURCE_DIR/${DAY}.cpp -o $SCRIPT_ROOT/bin/$DAY
time $SCRIPT_ROOT/bin/$DAY < $INPUT_DIR/$DAY
