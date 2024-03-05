#include <spdlog/spdlog.h>

#include <game.hpp>
#include <iostream>

int main(int argc, char* argv[]) {
  int timelimit = 8000;
  spdlog::set_level(spdlog::level::err);
  while (true) {
    std::string command;
    std::cin >> command;
    if (command[0] == '#') {
      std::string line;
      std::getline(std::cin, line);
      continue;
    }
    if (command == "QUIT") {
      std::cout << "bye" << std::endl;
      break;
    }
    if (command == "LOG") {
      std::string level;
      std::cin >> level;
      if (level == "DEBUG") {
        spdlog::set_level(spdlog::level::debug);
      }
      if (level == "INFO") {
        spdlog::set_level(spdlog::level::info);
      }
      if (level == "WARN") {
        spdlog::set_level(spdlog::level::warn);
      }
      if (level == "ERR") {
        spdlog::set_level(spdlog::level::err);
      }
      std::cout << "ok" << std::endl;
    }
    if (command == "SEARCH") {
      std::string state;
      std::getline(std::cin, state);
      GameState gameState(state);
      Move move = gameState.searchBestMoveWithTimeLimit(timelimit);
      std::cout << move.src << " " << move.dst << std::endl;
    }
    if (command == "TIMELIMIT") {
      std::cin >> timelimit;
      std::cout << "ok" << std::endl;
    }
  }
}