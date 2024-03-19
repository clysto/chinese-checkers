#include <FL/Fl.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Double_Window.H>
#include <FL/Fl_Flex.H>
#include <FL/Fl_PNG_Image.H>
#include <FL/Fl_SVG_Image.H>
#include <FL/fl_draw.H>
#include <spdlog/spdlog.h>

#include <functional>
#include <game.hpp>
#include <iostream>
#include <stack>
#include <thread>

#include "fonts/RubikMonoOne-Regular.h"
#include "fonts/icon.h"
#include "utils.hpp"
#include "widgets/Fl_ChessBoard.hpp"
#include "widgets/Fl_IconButton.hpp"

namespace App {
using cb_t = std::function<void(Fl_Widget* w)>;

auto adapter = [](Fl_Widget* w, void* data) {
  cb_t* func = reinterpret_cast<cb_t*>(data);
  (*func)(w);
};

void styled_line(Fl_Box* line) {
  line->box(FL_FLAT_BOX);
  line->color(0x457dd900);
}

void init() {
  Fl::lock();
  Fl_load_memory_font("RubicMonoOne.ttf", (const char*)RubikMonoOne_Regular_ttf, RubikMonoOne_Regular_ttf_len);
  Fl::set_font(FL_FREE_FONT, "Rubik Mono One Regular");
#if defined(_WIN32)
  Fl::set_font(FL_HELVETICA, "微软雅黑");
  Fl::set_font(FL_HELVETICA_BOLD, "微软雅黑 Bold");
  Fl_PNG_Image* icon = new Fl_PNG_Image("icon.png", (const unsigned char*)chinesecheckers_png, chinesecheckers_png_len);
  Fl_Double_Window::default_icon(icon);
  delete icon;
#endif
}

int run(int argc, char* argv[]) {
  init();
  int center_x = (Fl::w() - 620) / 2;
  int center_y = (Fl::h() - 724) / 2;
  auto window = new Fl_Double_Window(center_x, center_y, 620, 724, "中国跳棋");
  auto gameArea = new Fl_Flex(0, 0, 620, 684);
  int my_color = RED;
  std::stack<Move> history;
  auto game_state = new GameState();
  gameArea->box(FL_FLAT_BOX);
  gameArea->color(0xe8b06100);
  gameArea->margin(15);
  gameArea->begin();
  auto board = new Fl_ChessBoard(0, 0, 0, 0);
  board->set_game_state(game_state);
  board->user_color(my_color);
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

  cb_t cb1 = [&game_state, &board, &history](Fl_Widget* w) {
    auto old_state = game_state;
    game_state = new GameState();
    history = std::stack<Move>();
    board->set_game_state(game_state);
    delete old_state;
  };

  cb_t cb2 = [&board, btn4](Fl_Widget* w) {
    if (board->number()) {
      board->number(false);
      btn4->label(" 显示编号");
    } else {
      board->number(true);
      btn4->label(" 隐藏编号");
    }
  };

  cb_t cb4 = [&board, &game_state](Fl_Widget* w) {
    std::string state = game_state->toString();
    if (state.length() > 0) {
      Fl::copy(state.c_str(), state.length(), 1);
    }
  };

  cb_t cb3 = [&board, &game_state, &history](Fl_Widget* w) {
    history.push(board->get_user_last_move());
    std::thread searchThread([&game_state, &board, &history]() {
      GameState state = *game_state;
      Move move = state.searchBestMove(10);
      history.push(move);
      Fl::lock();
      board->move(move);
      Fl::unlock();
      Fl::awake();
    });
    searchThread.detach();
  };

  cb_t cb5 = [&board, &game_state, &my_color, &history](Fl_Widget* w) {
    if (game_state->turn == my_color && history.size() >= 2) {
      Move move = history.top();
      history.pop();
      game_state->undoMove(move);
      move = history.top();
      history.pop();
      game_state->undoMove(move);
      board->fill_moves();
      board->redraw();
    }
  };

  btn1->callback(adapter, &cb1);
  btn2->callback(adapter, &cb5);
  btn3->callback(adapter, &cb4);
  btn4->callback(adapter, &cb2);
  board->callback(adapter, &cb3);
  window->size_range(400, 500, 0, 0);
  window->show(argc, argv);
  return Fl::run();
}
}  // namespace App

int main(int argc, char* argv[]) { return App::run(argc, argv); }
