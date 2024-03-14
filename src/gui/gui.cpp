#include <FL/Fl.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Double_Window.H>
#include <FL/Fl_Flex.H>
#include <FL/Fl_PNG_Image.H>
#include <FL/Fl_SVG_Image.H>
#include <FL/fl_draw.H>

#include <functional>
#include <game.hpp>
#include <iostream>
#include <thread>

#include "fonts/RubikMonoOne-Regular.h"
#include "fonts/icon.h"
#include "utils.hpp"
#include "widgets/Fl_ChessBoard.hpp"
#include "widgets/Fl_IconButton.hpp"

namespace App {
using FunctionCallback = std::function<void(Fl_Widget* w, void* d)>;

auto adapter = [](Fl_Widget* w, void* data) {
  FunctionCallback* func = reinterpret_cast<FunctionCallback*>(data);
  (*func)(w, NULL);
};

void styled_line(Fl_Box* line) {
  line->box(FL_FLAT_BOX);
  line->color(0x457dd900);
}

void init() {
  Fl::lock();
  Fl_load_memory_font((const char*)RubikMonoOne_Regular_ttf, RubikMonoOne_Regular_ttf_len);
  Fl::set_font(FL_FREE_FONT, "Rubik Mono One Regular");
#if defined(_WIN32)
  Fl::set_font(FL_HELVETICA, "微软雅黑");
  Fl::set_font(FL_HELVETICA_BOLD, "微软雅黑 Bold");
#endif
}

Fl_PNG_Image* icon = new Fl_PNG_Image("icon.png", (const unsigned char*)chinesecheckers_png, chinesecheckers_png_len);

int run(int argc, char* argv[]) {
  init();
  auto window = new Fl_Double_Window(620, 684 + 40, "中国跳棋");
  window->icon(icon);
  auto gameArea = new Fl_Flex(0, 0, 620, 684);
  auto game_state = new GameState();
  gameArea->box(FL_FLAT_BOX);
  gameArea->color(0xe8b06100);
  gameArea->margin(15);
  gameArea->begin();
  auto board = new Fl_ChessBoard(0, 0, 0, 0);
  board->set_game_state(game_state);
  gameArea->end();
  auto controlArea = new Fl_Flex(0, 684, 620, 40, Fl_Flex::HORIZONTAL);
  controlArea->box(FL_FLAT_BOX);
  controlArea->color(0x2060c800);
  auto btn1 = new Fl_IconButton(0, 0, 0, 0, " 新游戏", Fl_IconButton::ICON_DICE_5_FILL);
  auto line1 = new Fl_Box(0, 0, 0, 0);
  styled_line(line1);
  auto btn2 = new Fl_IconButton(0, 0, 0, 0, " 悔棋", Fl_IconButton::ICON_ARROW_LEFT_SQUARE_FILL);
  auto line2 = new Fl_Box(0, 0, 0, 0);
  styled_line(line2);
  auto btn3 = new Fl_IconButton(0, 0, 0, 0, " 复制棋盘", Fl_IconButton::ICON_CLIPBOARD_CHECK_FILL);
  auto line3 = new Fl_Box(0, 0, 0, 0);
  styled_line(line3);
  auto btn4 = new Fl_IconButton(0, 0, 0, 0, " 显示编号", Fl_IconButton::ICON_1_CIRCLE_FILL);
  controlArea->end();
  controlArea->fixed(line1, 1);
  controlArea->fixed(line2, 1);
  controlArea->fixed(line3, 1);
  window->resizable(gameArea);
  window->end();

  FunctionCallback cb1 = [&game_state, &board](Fl_Widget* w, void* d) {
    auto old_state = game_state;
    game_state = new GameState();
    board->set_game_state(game_state);
    delete old_state;
  };

  FunctionCallback cb2 = [&board, btn4](Fl_Widget* w, void* d) {
    if (board->number()) {
      board->number(false);
      btn4->label(" 显示编号");
    } else {
      board->number(true);
      btn4->label(" 隐藏编号");
    }
  };

  FunctionCallback cb3 = [&board, &game_state](Fl_Widget* w, void* d) {
    std::thread searchThread([&game_state, &board]() {
      GameState state = *game_state;
      Move move = state.searchBestMove(5);
      Fl::lock();
      board->move(move);
      Fl::unlock();
      Fl::awake();
    });
    searchThread.detach();
  };

  btn1->callback(adapter, &cb1);
  btn4->callback(adapter, &cb2);
  board->callback(adapter, &cb3);
  window->size_range(400, 500, 0, 0);
  window->show(argc, argv);
  return Fl::run();
}
}  // namespace App

int main(int argc, char* argv[]) { return App::run(argc, argv); }
