#include <algorithm>
#include <cmath>
#include <cstdint>
#include <cstdlib>
#include <expected>
#include <format>
#include <iostream>
#include <iterator>
#include <sstream>
#include <string>

#include "util.hpp"

using std::cout, std::println;
using std::string;
using std::unexpected, std::expected;

typedef uint64_t AnswerType;

struct Block {
  int file_id;
  bool free;
};

void print_block(const Block &block) {
  println(cout, "{{ value = {}, free = {} }}", block.file_id, block.free);
}

typedef std::vector<Block> Disk;

void print_disk(const Disk &disk) {
  for (const Block &block : disk) {
    char c = block.free ? '.' : char('0' + block.file_id);
    print(cout, "{}", c);
  }
  println(cout);
}

Disk parse_input(const std::string &input) {
  Disk disk;
  for (int i = 0; i < input.size(); ++i) {
    char c = input[i];
    int num_blocks = int(c - '0');
    if (num_blocks < 0 || num_blocks > 9) {
      continue;
    }
    bool free = i % 2 == 1;
    int file_id = i / 2;
    // println(cout, "id = {}, c = {}, num_blocks = {}, free = {}", file_id, c,
    // num_blocks, free);
    for (int j = 0; j < num_blocks; ++j) {
      disk.push_back({file_id, free});
    }
  }
  return disk;
}

Block *get_first_empty_block(Disk &disk) {
  auto it = std::find_if(disk.begin(), disk.end(),
                         [](const Block &block) { return block.free; });
  if (it == disk.end()) {
    return nullptr;
  }
  return &(*it);
}

Block *get_last_non_empty_block(Disk &disk) {
  auto it = std::find_if(disk.rbegin(), disk.rend(),
                         [](const Block &block) { return !block.free; });
  if (it == disk.rend()) {
    return nullptr;
  }
  return &(*it);
}

bool is_packed(Disk &disk) {
  const auto last_non_empty_block =
      std::find_if(disk.crbegin(), disk.crend(),
                   [](const Block &block) { return !block.free; });
  if (last_non_empty_block == disk.crend()) {
    return true;
  }
  const auto first_empty_block =
      std::find_if(disk.cbegin(), disk.cend(),
                   [](const Block &block) { return block.free; });
  if (first_empty_block == disk.cend()) {
    return true;
  }
  return std::next(last_non_empty_block).base() < first_empty_block;
}

void pack(Disk &disk) {
  while (!is_packed(disk)) {
    Block *empty = get_first_empty_block(disk);
    if (empty == nullptr) {
      return;
    }

    Block *last_non_empty = get_last_non_empty_block(disk);
    if (last_non_empty == nullptr) {
      return;
    }

    empty->file_id = last_non_empty->file_id;
    empty->free = false;
    last_non_empty->free = true;
  }
}

AnswerType checksum(const Disk &disk) {
  AnswerType sum = 0;
  for (const auto &[i, block] : std::ranges::views::enumerate(disk)) {
    if (block.free) {
      // continue because part 2 allows gaps and cba creating another function.
      continue;
    }
    sum += i * block.file_id;
  }
  return sum;
}

auto part_one(const std::string &input) -> expected<AnswerType, string> {
  Disk disk = parse_input(input);
  pack(disk);
  return checksum(disk);
}

std::tuple<std::vector<Block *>, long> get_file(Disk &disk, int file_id) {
  auto first = std::find_if(disk.begin(), disk.end(), [=](const Block &block) {
    return !block.free && block.file_id == file_id;
  });
  auto start_index = std::distance(disk.begin(), first);
  auto last = std::find_if(disk.rbegin(), disk.rend(), [=](const Block &block) {
                return !block.free && block.file_id == file_id;
              }).base();

  std::vector<Block *> result;
  for (auto it = first; it != last; ++it) {
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
                           [](const Block &block) { return block.free; });
    if (it >= end) {
      return {};
    }

    result.push_back(&(*it));
    while (++it < end && (*it).free) {
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
  auto last_non_empty = get_last_non_empty_block(disk);
  auto highest_file_id = last_non_empty->file_id;
  auto current_file_id = highest_file_id;
  do {
    auto [file, file_start_index] = get_file(disk, current_file_id);
    auto [free_blocks, free_blocks_start_index] =
        get_first_contiguous_block_of_size(disk, file_start_index, file.size());
    if (free_blocks.size() >= file.size()) {
      int i = 0;
      for (auto &file_block : file) {
        file_block->free = true;
        free_blocks[i]->file_id = current_file_id;
        free_blocks[i]->free = false;
        ++i;
      }
    }
  } while (--current_file_id >= 0);
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
