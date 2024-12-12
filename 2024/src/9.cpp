#include <algorithm>
#include <cmath>
#include <cstdint>
#include <cstdlib>
#include <execution>
#include <expected>
#include <iostream>
#include <iterator>
#include <sstream>
#include <string>

#include "util.hpp"

using std::cout, std::println;
using std::string;
using std::unexpected, std::expected;

typedef uint64_t AnswerType;
typedef int Block;
const Block FREE = -1;

typedef std::vector<Block> Disk;

Disk parse_input(const std::string &input) {
  Disk disk;
  disk.reserve(input.size() * 5);
  for (int i = 0; i < input.size(); ++i) {
    char c = input[i];
    int num_blocks = int(c - '0');
    if (num_blocks < 0 || num_blocks > 9) {
      // skip newline and stuff
      continue;
    }
    int file_id = i % 2 == 1 ? FREE : i / 2;
    disk.insert(disk.end(), num_blocks, file_id);
  }
  return disk;
}

Block *get_first_empty_block(Disk &disk) {
  for (Block &block : disk) {
    if (block == FREE) {
      return &block;
    }
  }
  return nullptr;
}

Block *get_last_non_empty_block(Disk &disk) {
  for (auto it = disk.rbegin(); it != disk.rend(); ++it) {
    if (*it != FREE) {
      return &(*it);
    }
  }
  return nullptr;
}

bool is_packed(const Disk &disk) {
  bool found_empty = false;
  for (const Block &block : disk) {
    if (block == FREE) {
      found_empty = true;
    } else if (found_empty) {
      return false;
    }
  }
  return true;
}

void pack(Disk &disk) {
  size_t empty_index = 0;
  size_t non_empty_index = disk.size() - 1;
  while (empty_index < non_empty_index) {
    while (empty_index < disk.size() && disk[empty_index] != FREE) {
      ++empty_index;
    }

    while (non_empty_index > empty_index && disk[non_empty_index] == FREE) {
      --non_empty_index;
    }

    if (empty_index < non_empty_index) {
      disk[empty_index] = disk[non_empty_index];
      disk[non_empty_index] = FREE;
    }
  }
}

AnswerType checksum(const Disk &disk) {
  return std::transform_reduce(std::execution::par, disk.begin(), disk.end(),
                               static_cast<AnswerType>(0), std::plus<>(),
                               [i = 0](Block block) mutable {
                                 return block == FREE ? 0 : (i++) * block;
                               });
}

auto part_one(const std::string &input) -> expected<AnswerType, string> {
  Disk disk = parse_input(input);
  pack(disk);
  return checksum(disk);
}

std::tuple<std::vector<Block *>, long> get_file(Disk &disk, int file_id) {
  auto first = std::find_if(disk.begin(), disk.end(), [=](const Block &block) {
    return block == file_id;
  });
  auto start_index = std::distance(disk.begin(), first);
  auto last = std::find_if(
      first, disk.end(), [=](const Block &block) { return block == file_id; });

  std::vector<Block *> result;
  for (auto it = first; it <= last; ++it) {
    result.push_back(&(*it));
  }
  return std::make_tuple(result, start_index);
}

std::tuple<std::vector<Block *>, long>
get_first_contiguous_block_of_size(Disk &disk, long end_index, size_t size) {
  auto start = disk.begin();
  const auto end = start + end_index;
  while (start < end) {
    std::vector<Block *> result;
    auto it = std::find_if(start, end,
                           [](const Block &block) { return block != FREE; });
    if (it >= end) {
      return {};
    }

    result.push_back(&(*it));
    while (++it < end && *it == FREE) {
      result.push_back(&(*it));
    }

    if (result.size() >= size) {
      return std::make_tuple(result, std::distance(disk.begin(), start));
    }

    start = it;
  }
  return {};
}

void block_pack(Disk &disk) {
  std::vector<std::pair<size_t, size_t>> free_spans;
  size_t start = 0;
  while (start < disk.size()) {
    start = std::find(disk.begin() + start, disk.end(), FREE) - disk.begin();
    if (start == disk.size())
      break;

    size_t end = start;
    while (end < disk.size() && disk[end] == FREE)
      ++end;

    free_spans.emplace_back(start, end - start);
    start = end;
  }

  for (int file_id = *std::max_element(disk.begin(), disk.end()); file_id >= 0;
       --file_id) {
    auto file_start = std::find(disk.begin(), disk.end(), file_id);

    size_t file_size = std::count(file_start, disk.end(), file_id);
    size_t file_start_idx = file_start - disk.begin();

    auto free_it =
        std::find_if(free_spans.begin(), free_spans.end(),
                     [&](const std::pair<size_t, size_t> &span) {
                       return span.first + span.second <= file_start_idx &&
                              span.second >= file_size;
                     });

    if (free_it != free_spans.end()) {
      size_t free_start_idx = free_it->first;
      std::move(disk.begin() + file_start_idx,
                disk.begin() + file_start_idx + file_size,
                disk.begin() + free_start_idx);
      std::fill(disk.begin() + file_start_idx,
                disk.begin() + file_start_idx + file_size, FREE);

      free_it->first += file_size;
      free_it->second -= file_size;

      if (free_it->second == 0) {
        free_spans.erase(free_it);
      }
    }
  }
}

auto part_two(const string &input) -> expected<AnswerType, string> {
  Disk disk = parse_input(input);
  block_pack(disk);
  return checksum(disk);
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
  println(cout, "Day 9");
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
