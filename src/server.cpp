#include <httplib.h>
#include <spdlog/spdlog.h>

#include <game.hpp>

int main(int argc, char* argv[]) {
  using namespace httplib;

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

  svr.Get("/searchbestmove", [](const Request& req, Response& res) {
    std::string state = req.get_param_value("state");
    std::string depth = req.get_param_value("depth");
    int searchDepth = 5;
    if (!depth.empty()) {
      try {
        searchDepth = std::stoi(depth);
      } catch (std::invalid_argument const& e) {
        searchDepth = 5;
      }
    }
    if (searchDepth < 1 || searchDepth > 10) {
      searchDepth = 5;
    }
    GameState gameState(state);
    spdlog::info("state: {}", state);
    spdlog::info("depth: {}", searchDepth);
    Move move = gameState.searchBestMove(searchDepth);
    spdlog::info("bestmove: {} {}", move.src, move.dst);
    res.set_header("Access-Control-Allow-Origin", "*");
    res.set_content(std::to_string(move.src) + " " + std::to_string(move.dst), "text/plain");
  });

  svr.Get("/timelimitsearch", [](const Request& req, Response& res) {
    std::string state = req.get_param_value("state");
    std::string time = req.get_param_value("time");
    int searchTime = 1000;
    if (!time.empty()) {
      try {
        searchTime = std::stoi(time);
      } catch (std::invalid_argument const& e) {
        searchTime = 1000;
      }
    }
    if (searchTime < 1000 || searchTime > 100000) {
      searchTime = 1000;
    }
    GameState gameState(state);
    spdlog::info("state: {}", state);
    spdlog::info("time: {} ms", searchTime);
    Move move = gameState.searchBestMoveWithTimeLimit(searchTime);
    spdlog::info("bestmove: {} {}", move.src, move.dst);
    res.set_header("Access-Control-Allow-Origin", "*");
    res.set_content(std::to_string(move.src) + " " + std::to_string(move.dst), "text/plain");
  });

  svr.listen(host, port);
}