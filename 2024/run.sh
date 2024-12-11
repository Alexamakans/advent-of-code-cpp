#!/usr/bin/env bash

SCRIPT_ROOT=$(pwd)
INPUT_DIR=$SCRIPT_ROOT/inputs
SOURCE_DIR=$SCRIPT_ROOT/src

[ -d $INPUT_DIR ] || mkdir $INPUT_DIR
[ -d $SOURCE_DIR ] || mkdir $SOURCE_DIR
[ -d $SCRIPT_ROOT/samples ] || mkdir $SCRIPT_ROOT/samples
[ -d $SCRIPT_ROOT/bin ] || mkdir $SCRIPT_ROOT/bin

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
    >$SCRIPT_ROOT/inputs/$DAY
fi

function make_file() {
  cat <<EOF
#include <cmath>
#include <cstdint>
#include <cstdlib>
#include <expected>
#include <format>
#include <iostream>
#include <sstream>
#include <string>

#include "util.hpp"

using std::cout, std::println;
using std::string;
using std::unexpected, std::expected;

typedef uint64_t AnswerType;

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
  BatchResult consume(Batch input) const {
    BatchResult result = 0;
    for (const auto &element : input) {
      // do stuff
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

auto make_solver(int batch_size)
    -> Multithreader<Batch, BatchResult, FinalResult> auto {
  auto out = Solver{};
  out.provider.batch_size = batch_size;
  return out;
}

auto part_one(const std::string &input) -> expected<AnswerType, string> {
  auto solver_instance = make_solver(100);
  AnswerType result =
      execute<Batch, BatchResult, AnswerType>(input, solver_instance, 0);
  return result;
}

auto part_two(const string &input) -> expected<AnswerType, string> {
  auto solver_instance = make_solver(100);
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
  println(cout, "Day $DAY");
  println(cout, "\tPart 1");
  println(cout, "\t\tAnswer: {}", part_one_result);
  println(cout, "\tPart 2");
  println(cout, "\t\tAnswer: {}", part_two_result);
  println(cout, "-----------------------------------------");
  return 0;
}
EOF
} >$SOURCE_DIR/${DAY}.cpp

if [ ! -f "$SOURCE_DIR/${DAY}.cpp" ]; then
  make_file
fi

if [ -f $SCRIPT_ROOT/bin/$DAY ]; then
  rm $SCRIPT_ROOT/bin/$DAY 2>/dev/null
fi

g++ -g -O3 -std=c++23 -I $SOURCE_DIR $SOURCE_DIR/${DAY}.cpp -o $SCRIPT_ROOT/bin/$DAY

if [ $? -eq 0 ]; then
  clear
  time gdb -q -ex run -ex "bt" -ex quit --args $SCRIPT_ROOT/bin/$DAY <$INPUT_DIR/$DAY
fi
