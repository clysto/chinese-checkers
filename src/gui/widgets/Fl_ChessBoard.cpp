#include "Fl_ChessBoard.hpp"

#include <FL/Fl_Group.H>
#include <FL/Fl_Widget.H>
#include <FL/fl_draw.H>

#include <iostream>
#include <vector>

const double M_SQRT3 = 1.732050807568877293;

Fl_ChessBoard::Fl_ChessBoard(int x, int y, int w, int h, int my_color)
    : Fl_Widget(x, y, w, h),
      scale(1),
      dx(0),
      dy(0),
      game_state(nullptr),
      selected_piece(-1),
      last_move{-1, -1},
      show_number(false),
      my_color(my_color) {}

void Fl_ChessBoard::draw() {
  double radius = 19;
  dx = x();
  dy = y();
  if (w() * 684.0 > h() * 620.0) {
    scale = h() / 684.0;
    dx += (w() - 620 * scale) / 2;
  } else {
    scale = w() / 620.0;
    dy += (h() - 684 * scale) / 2;
  }
  radius *= scale;
  fl_color(0x4b310c00);
  fl_begin_complex_polygon();
  for (int i = 0; i < 18; i++) {
    fl_vertex(STAR_SHAPE[i][0] * scale + dx, STAR_SHAPE[i][1] * scale + dy);
  }
  std::vector<int> board;
  if (game_state != nullptr) {
    board = game_state->getBoard();
  } else {
    for (int i = 0; i < 81; i++) {
      board.push_back(EMPTY);
    }
  }
  fl_end_complex_polygon();
  fl_color(0x977d5a00);
  fl_font(FL_FREE_FONT, std::round(16 * scale));
  for (int i = 0; i < 81; i++) {
    double cx = CIRCLE_POSITIONS[i][0] * scale + dx;
    double cy = CIRCLE_POSITIONS[i][1] * scale + dy;
    if (game_state && board[i] == RED) {
      if ((game_state->turn == RED && selected_piece == i) || last_move.dst == i) {
        fl_color(0xffafaf00);
        fl_circle(cx, cy, radius + scale);
        fl_color(0xe5000000);
        fl_circle(cx, cy, radius - 3 * scale);
      } else {
        fl_color(0xe5000000);
        fl_circle(cx, cy, radius);
      }
      if (show_number) {
        fl_color(0x89000000);
        fl_draw(std::to_string(i).c_str(), std::round(cx - radius), std::round(cy - radius + scale),
                std::round(2 * radius), std::round(2 * radius), FL_ALIGN_CENTER | FL_ALIGN_INSIDE);
      }
    } else if (game_state && board[i] == GREEN) {
      if ((game_state->turn == GREEN && selected_piece == i) || last_move.dst == i) {
        fl_color(0xcfffcf00);
        fl_circle(cx, cy, radius + scale);
        fl_color(0x35cc3500);
        fl_circle(cx, cy, radius - 3 * scale);
      } else {
        fl_color(0x35cc3500);
        fl_circle(cx, cy, radius);
      }
      if (show_number) {
        fl_color(0x207a2000);
        fl_draw(std::to_string(i).c_str(), std::round(cx - radius), std::round(cy - radius + scale),
                std::round(2 * radius), std::round(2 * radius), FL_ALIGN_CENTER | FL_ALIGN_INSIDE);
      }
    } else {
      double r = radius;
      if (game_state && last_move.src == i) {
        fl_color(game_state->turn == RED ? 0xcfffcf00 : 0xffafaf00);
        fl_circle(cx, cy, r + scale);
        r -= 3 * scale;
      }
      if (selected_piece >= 0 && moves[selected_piece][i] == 1) {
        fl_color(0xd6b17d00);
      } else {
        fl_color(0x977d5a00);
      }
      fl_circle(cx, cy, r);
      if (show_number) {
        fl_color(0x5b4b3600);
        fl_draw(std::to_string(i).c_str(), std::round(cx - radius), std::round(cy - radius + scale),
                std::round(2 * radius), std::round(2 * radius), FL_ALIGN_CENTER | FL_ALIGN_INSIDE);
      }
    }
  }
  fl_color(0x977d5a00);
  for (int i = 0; i < 40; i++) {
    double cx = FAKE_CIRCLE_POSITIONS[i][0] * scale + dx;
    double cy = FAKE_CIRCLE_POSITIONS[i][1] * scale + dy;
    fl_circle(cx, cy, radius);
  }
}

int Fl_ChessBoard::handle(int event) {
  double col, row, first_x, first_y, target_x, target_y, gap = 45 * M_SQRT3 * scale;
  double x = Fl::event_x();
  double y = Fl::event_y();
  int click_number;
  switch (event) {
    case FL_PUSH:
      first_x = CIRCLE_POSITIONS[0][0] * scale + dx;
      first_y = CIRCLE_POSITIONS[0][1] * scale + dy;
      col = y + M_SQRT3 * x;
      row = y - M_SQRT3 * x;
      col -= first_y + M_SQRT3 * first_x;
      row -= first_y - M_SQRT3 * first_x;
      col /= gap;
      row /= gap;
      click_number = std::round(row) * 9 + std::round(col);
      if (click_number < 0 || click_number > 80) {
        selected_piece = -1;
        redraw();
        return 1;
      }
      target_x = CIRCLE_POSITIONS[click_number][0] * scale + dx;
      target_y = CIRCLE_POSITIONS[click_number][1] * scale + dy;

      if ((x - target_x) * (x - target_x) + (y - target_y) * (y - target_y) < 19 * 19 * scale * scale) {
        handle_circle_click(click_number);
      } else {
        selected_piece = -1;
        redraw();
      }
      return 1;
    default:
      return Fl_Widget::handle(event);
  }
}

void Fl_ChessBoard::set_game_state(GameState* game_state) {
  this->game_state = game_state;
  selected_piece = -1;
  last_move = {-1, -1};
  fill_moves();
  if (game_state == nullptr) {
    selected_piece = -1;
    last_move = {-1, -1};
  }
  redraw();
}

void Fl_ChessBoard::handle_circle_click(int index) {
  if (game_state == nullptr) return;
  if ((game_state->board[game_state->turn] >> index) & 1 && my_color == game_state->turn) {
    selected_piece = index;
    redraw();
  } else if (selected_piece != -1 && moves[selected_piece][index] == 1) {
    move({selected_piece, index});
  } else if (selected_piece != -1) {
    selected_piece = -1;
    redraw();
  }
}

void Fl_ChessBoard::move(Move move) {
  last_move = move;
  game_state->applyMove(last_move);
  selected_piece = -1;
  fill_moves();
  if (my_color != game_state->turn) {
    do_callback();
  }
  redraw();
}

void Fl_ChessBoard::fill_moves() {
  std::memset(moves, 0, sizeof(moves));
  for (auto move : game_state->legalMoves()) {
    this->moves[move.src][move.dst] = 1;
  }
}

void Fl_ChessBoard::number(bool is_show) {
  show_number = is_show;
  redraw();
}

bool Fl_ChessBoard::number() { return show_number; }

void Fl_ChessBoard::color(int my_color) {
  this->my_color = my_color;
  fill_moves();
  selected_piece = -1;
  last_move = {-1, -1};
  redraw();
}

int Fl_ChessBoard::color() { return my_color; }
