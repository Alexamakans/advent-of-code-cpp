#include <iostream>
#include <sstream>
#include <string>

int part_one(const std::string &input) {
  // Pair up the smallest number in the left list with the smallest number in the right list,
  // second smallest with the second smallest, etc.
  //
  // Output the sum of the difference of each pair.

  std::istringstream lines(input);
  std::string line;
  while (std::getline(lines, line)) {
  }
  return 0;
}

int part_two(const std::string &input) { return 0; }

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
