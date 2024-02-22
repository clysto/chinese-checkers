#include <httplib.h>

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

  std::cout << "Server running at http://" << host << ":" << port << std::endl;

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
    std::cout << "   state: " << state << std::endl;
    std::cout << "   depth: " << searchDepth << std::endl;
    Move move = gameState.searchBestMove(searchDepth);
    std::cout << "bestmove: " << move.src << " " << move.dst << std::endl;
    res.set_header("Access-Control-Allow-Origin", "*");
    res.set_content(std::to_string(move.src) + " " + std::to_string(move.dst), "text/plain");
  });

  svr.listen(host, port);
}