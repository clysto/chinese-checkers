#include <emscripten/bind.h>
#include <emscripten/emscripten.h>

#include <game.hpp>
#include <iostream>
#include <string>

std::string greeting(std::string name) { return "Hello, " + name + "!"; }

EMSCRIPTEN_BINDINGS(wasm) {
  using namespace emscripten;
  function("greeting", &greeting);
  enum_<Color>("Color").value("RED", Color::RED).value("GREEN", Color::GREEN);
  value_object<Move>("Move").field("src", &Move::src).field("dst", &Move::dst);
  class_<GameState>("GameState")
      .constructor<>()
      .property("turn", &GameState::getTurn)
      .function("getBoard", &GameState::getBoard)
      .function("legalMoves", &GameState::legalMoves)
      .function("applyMove", &GameState::applyMove)
      .function("evaluate", &GameState::evaluate)
      .function("isGameOver", &GameState::isGameOver)
      .function("searchBestMove", &GameState::searchBestMove)
      .function("toString", &GameState::toString);
  register_vector<int>("VectorInt");
  register_vector<Move>("VectorMove");
}
