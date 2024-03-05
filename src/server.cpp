#include <httplib.h>
#include <spdlog/spdlog.h>

#include <game.hpp>

int main(int argc, char* argv[]) {
  using namespace httplib;

  std::signal(SIGINT, [](int) { exit(0); });

  std::string host = "localhost";
  int port = 1234;

  if (argc > 1) {
    host = argv[1];
  }
  if (argc > 2) {
    port = std::stoi(argv[2]);
  }

  Server svr;

  spdlog::info("Server running at http://{}:{}", host, port);

  svr.Get("/search", [](const Request& req, Response& res) {
    std::string state = req.get_param_value("state");
    std::string time = req.get_param_value("time");
    int searchTime = 1;
    if (!time.empty()) {
      try {
        searchTime = std::stoi(time);
      } catch (std::invalid_argument const& e) {
        searchTime = 1;
      }
    }
    if (searchTime < 1 || searchTime > 100) {
      searchTime = 1;
    }
    GameState gameState(state);
    spdlog::info("state: {}", state);
    spdlog::info("think: {} seconds", searchTime);
    Move move = gameState.searchBestMove(searchTime);
    spdlog::info("bestmove: {} {}", move.src, move.dst);
    res.set_header("Access-Control-Allow-Origin", "*");
    res.set_content(std::to_string(move.src) + " " + std::to_string(move.dst), "text/plain");
  });

  svr.listen(host, port);
}