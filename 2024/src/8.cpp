#include <cmath>
#include <cstdint>
#include <cstdlib>
#include <expected>
#include <format>
#include <iostream>
#include <numeric>
#include <sstream>
#include <string>
#include <unordered_map>
#include <unordered_set>

#include "thirdpartyutils.hpp"
#include "util.hpp"

using std::cout, std::println;
using std::string;
using std::unexpected, std::expected;

typedef uint64_t AnswerType;

typedef std::vector<Point> Batch;
typedef std::unordered_set<Point> BatchResult;
typedef std::unordered_set<Point> FinalResult;

struct Provider {
  std::string line_buffer;
  Batch batch_buffer;

  std::unordered_map<char, std::vector<Point>> antennas;
  std::unordered_map<char, std::vector<Point>> cached_antennas;
  int width, height;

  void prepare(const std::string &input) {
    if (cached_antennas.size()) {
      this->antennas = cached_antennas;
      return;
    }
    std::istringstream lines(input);
    int y = 0;
    while (std::getline(lines, line_buffer)) {
      int x = 0;
      for (const char c : line_buffer) {
        if (c != '.') {
          if (!cached_antennas.contains(c)) {
            cached_antennas.insert_or_assign(c, std::vector<Point>{});
          }
          cached_antennas.at(c).push_back({x, y});
        }
        ++x;
      }
      width = x;
      ++y;
    }
    height = y;
    this->antennas = cached_antennas;
  }

  Batch provide() {
    const auto &entry = antennas.begin();
    Batch batch = entry->second;
    antennas.erase(entry->first);
    return batch;
  }

  bool done() const { return antennas.size() <= 0; }
};

struct Consumer {
  int width, height;
  int part = 1;
  void add_antinodes(const Line &line, const Point &step_size,
                     BatchResult &result) const {
    if (part == 1) {
      Point a(line.a.x + step_size.x, line.a.y + step_size.y);
      Point b(line.b.x - step_size.x, line.b.y - step_size.y);

      if (a.x >= 0 && a.x < width && a.y >= 0 && a.y < height) {
        result.insert(a);
      }
      if (b.x >= 0 && b.x < width && b.y >= 0 && b.y < height) {
        result.insert(b);
      }
    } else if (part == 2) {
      result.insert(line.a);
      // Since we are covering the entire line anyways, just start from line.a
      Point a(line.a.x + step_size.x, line.a.y + step_size.y);
      Point b(line.a.x - step_size.x, line.a.y - step_size.y);
      while (true) {
        bool one_good = false;
        if (a.x >= 0 && a.x < width && a.y >= 0 && a.y < height) {
          result.insert(a);
          a.x += step_size.x;
          a.y += step_size.y;
          one_good = true;
        }
        if (b.x >= 0 && b.x < width && b.y >= 0 && b.y < height) {
          result.insert(b);
          b.x -= step_size.x;
          b.y -= step_size.y;
          one_good = true;
        }

        if (!one_good) {
          return;
        }
      }
    }
  }

  BatchResult consume(Batch input) const {
    BatchResult unique_locations;
    for (const auto &pair : cdistinct_pairs(input)) {
      Point a = pair.first;
      Point b = pair.second;
      Point step_size(a.x - b.x, a.y - b.y);

      if (part == 2) {
        // Doesn't seem to happen with my input, but dividing but GCD lets us
        // take the smallest possible step that is perfectly in line with the
        // antennas.
        //
        // Disable for part 1 since there's a condition on the distance between
        // antennas and antinodes.
        int gcd = std::gcd(step_size.x, step_size.y);
        step_size.x /= gcd;
        step_size.y /= gcd;
      }

      add_antinodes(Line(a, b), step_size, unique_locations);
    }
    return unique_locations;
  }
};

struct Solver {
  Provider provider;
  Consumer consumer;

  FinalResult combine(FinalResult accumulator, BatchResult value) const {
    accumulator.insert(value.begin(), value.end());
    return accumulator;
  }
};

auto make_solver() -> Multithreader<Batch, BatchResult, FinalResult> auto {
  auto out = Solver{};
  return out;
}

auto solver = make_solver();

auto part_one(const std::string &input) -> expected<AnswerType, string> {
  solver.provider.prepare(input);
  solver.consumer.part = 1;
  solver.consumer.width = solver.provider.width;
  solver.consumer.height = solver.provider.height;
  AnswerType result =
      execute<Batch, BatchResult, FinalResult>(input, solver, {}).size();
  return result;
}

auto part_two(const string &input) -> expected<AnswerType, string> {
  solver.consumer.part = 2;
  AnswerType result =
      execute<Batch, BatchResult, FinalResult>(input, solver, {}).size();
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
  println(cout, "Day 8");
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
