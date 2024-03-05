#include <spdlog/spdlog.h>

#include <game.hpp>
#include <iostream>

int main(int argc, char* argv[]) {
  int timelimit = 8000;
  spdlog::set_level(spdlog::level::err);
  while (true) {
    std::string command;
    std::cin >> command;
    for (char& c : command) {
      c = std::toupper(c);
    }
    if (command == "QUIT") {
      std::cout << "bye" << std::endl;
      break;
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