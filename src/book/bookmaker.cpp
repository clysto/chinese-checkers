#include <cstdint>
#include <fstream>
#include <game.hpp>
#include <iostream>
#include <sstream>
#include <vector>

int main(int argc, char* argv[]) {
  if (argc != 3) {
    std::cerr << "Usage: " << argv[0] << " <input.txt> <output.dat>" << std::endl;
    return 1;
  }

  std::cout << "sizeof(BookEntry): " << sizeof(BookEntry) << std::endl;

  std::ifstream file(argv[1]);
  std::ofstream book(argv[2], std::ios::binary);
  std::string line;
  bool even = true;
  GameState state;
  std::vector<BookEntry> entries;
  while (std::getline(file, line)) {
    if (even) {
      state = GameState(line);
    } else {
      std::istringstream iss(line);
      BookEntry entry;
      iss >> entry.src >> entry.dst;
      entry.hash = state.hash();
      entries.push_back(entry);
    }
    even = !even;
  }

  std::sort(entries.begin(), entries.end());

  for (const auto& entry : entries) {
    std::cout << "Adding entry: " << entry.src << " -> " << entry.dst << std::endl;
    book.write(reinterpret_cast<const char*>(&entry), sizeof(BookEntry));
  }

  std::cout << "Wrote " << entries.size() << " entries to " << argv[2] << std::endl;

  return 0;
}