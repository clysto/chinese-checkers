#include "Fl_ChessBoard.hpp"

#include <FL/Fl_Group.H>
#include <FL/Fl_Widget.H>
#include <FL/fl_draw.H>

#include <cmath>
#include <cstring>
#include <iostream>
#include <vector>

const double M_SQRT3 = 1.732050807568877293;

const double CIRCLE_POSITIONS[81][2] = {
    {310, 30},
    {332.5, 68.97114317029974},
    {355, 107.94228634059948},
    {377.5, 146.91342951089922},
    {400, 185.88457268119896},
    {422.5, 224.8557158514987},
    {445, 263.82685902179844},
    {467.5, 302.7980021920982},
    {490, 341.7691453623979},
    {287.5, 68.97114317029974},
    {310, 107.94228634059948},
    {332.5, 146.91342951089922},
    {355, 185.88457268119896},
    {377.5, 224.8557158514987},
    {400, 263.82685902179844},
    {422.5, 302.7980021920982},
    {445, 341.7691453623979},
    {467.5, 380.74028853269766},
    {265, 107.94228634059948},
    {287.5, 146.91342951089922},
    {310, 185.88457268119896},
    {332.5, 224.8557158514987},
    {355, 263.82685902179844},
    {377.5, 302.7980021920982},
    {400, 341.7691453623979},
    {422.5, 380.74028853269766},
    {445, 419.7114317029974},
    {242.5, 146.91342951089922},
    {265, 185.88457268119896},
    {287.5, 224.8557158514987},
    {310, 263.82685902179844},
    {332.5, 302.7980021920982},
    {355, 341.7691453623979},
    {377.5, 380.74028853269766},
    {400, 419.7114317029974},
    {422.5, 458.68257487329714},
    {220, 185.88457268119896},
    {242.5, 224.8557158514987},
    {265, 263.82685902179844},
    {287.5, 302.7980021920982},
    {310, 341.7691453623979},
    {332.5, 380.74028853269766},
    {355, 419.7114317029974},
    {377.5, 458.68257487329714},
    {400, 497.6537180435969},
    {197.5, 224.8557158514987},
    {220, 263.82685902179844},
    {242.5, 302.7980021920982},
    {265, 341.7691453623979},
    {287.5, 380.74028853269766},
    {310, 419.7114317029974},
    {332.5, 458.68257487329714},
    {355, 497.6537180435969},
    {377.5, 536.6248612138966},
    {175, 263.82685902179844},
    {197.5, 302.7980021920982},
    {220, 341.7691453623979},
    {242.5, 380.74028853269766},
    {265, 419.7114317029974},
    {287.5, 458.68257487329714},
    {310, 497.6537180435969},
    {332.5, 536.6248612138966},
    {355, 575.5960043841964},
    {152.5, 302.7980021920982},
    {175, 341.7691453623979},
    {197.5, 380.74028853269766},
    {220, 419.7114317029974},
    {242.5, 458.68257487329714},
    {265, 497.6537180435969},
    {287.5, 536.6248612138966},
    {310, 575.5960043841964},
    {332.5, 614.5671475544962},
    {130, 341.7691453623979},
    {152.5, 380.74028853269766},
    {175, 419.7114317029974},
    {197.5, 458.68257487329714},
    {220, 497.6537180435969},
    {242.5, 536.6248612138966},
    {265, 575.5960043841964},
    {287.5, 614.5671475544962},
    {310, 653.5382907247958},
};

const double FAKE_CIRCLE_POSITIONS[][2] = {
    {512.5, 380.74028853}, {535., 419.7114317},   {557.5, 458.68257487}, {580., 497.65371804},  {490., 419.7114317},
    {512.5, 458.68257487}, {535., 497.65371804},  {467.5, 458.68257487}, {490., 497.65371804},  {445., 497.65371804},
    {107.5, 380.74028853}, {130., 419.7114317},   {152.5, 458.68257487}, {175., 497.65371804},  {85., 419.7114317},
    {107.5, 458.68257487}, {130., 497.65371804},  {62.5, 458.68257487},  {85., 497.65371804},   {40., 497.65371804},
    {445., 185.88457268},  {467.5, 224.85571585}, {490., 263.82685902},  {512.5, 302.79800219}, {490., 185.88457268},
    {512.5, 224.85571585}, {535., 263.82685902},  {535., 185.88457268},  {557.5, 224.85571585}, {580., 185.88457268},
    {40., 185.88457268},   {62.5, 224.85571585},  {85., 263.82685902},   {107.5, 302.79800219}, {85., 185.88457268},
    {107.5, 224.85571585}, {130., 263.82685902},  {130., 185.88457268},  {152.5, 224.85571585}, {175., 185.88457268},
};

const double STAR_SHAPE[18][2] = {
    {290.3, 0},     {329.7, 0},     {418.5, 153.8}, {596.1, 153.8}, {615.8, 187.9}, {527, 341.8},
    {615.8, 495.6}, {596.1, 529.7}, {418.5, 529.7}, {329.7, 683.5}, {290.3, 683.5}, {201.5, 529.7},
    {23.9, 529.7},  {4.2, 495.6},   {93, 341.8},    {4.2, 187.9},   {23.9, 153.8},  {201.5, 153.8},
};

void circle(double x, double y, double r) {
  fl_begin_polygon();
  fl_circle(x, y, r);
  fl_end_polygon();
}

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
  fl_font(FL_FREE_FONT, 16 * scale);
  int text_dx, text_dy, text_width, text_height;
  for (int i = 0; i < 81; i++) {
    double cx, cy;
    if (my_color == RED) {
      cx = CIRCLE_POSITIONS[i][0] * scale + dx;
      cy = CIRCLE_POSITIONS[i][1] * scale + dy;
    } else {
      cx = CIRCLE_POSITIONS[80 - i][0] * scale + dx;
      cy = CIRCLE_POSITIONS[80 - i][1] * scale + dy;
    }
    if (game_state && board[i] == RED) {
      if ((game_state->turn == RED && selected_piece == i) || last_move.dst == i) {
        fl_color(0xffafaf00);
        circle(cx, cy, radius + scale);
        fl_color(0xe5000000);
        circle(cx, cy, radius - 3 * scale);
      } else {
        fl_color(0xe5000000);
        circle(cx, cy, radius);
      }
      if (show_number) {
        fl_color(0x89000000);
        fl_text_extents(std::to_string(i).c_str(), text_dx, text_dy, text_width, text_height);
        fl_graphics_driver->draw(std::to_string(i).c_str(), std::to_string(i).size(),
                                 (float)(cx - text_dx - (float)text_width / 2),
                                 (float)(cy - text_dy - (float)text_height / 2));
      }
    } else if (game_state && board[i] == GREEN) {
      if ((game_state->turn == GREEN && selected_piece == i) || last_move.dst == i) {
        fl_color(0xcfffcf00);
        circle(cx, cy, radius + scale);
        fl_color(0x35cc3500);
        circle(cx, cy, radius - 3 * scale);
      } else {
        fl_color(0x35cc3500);
        circle(cx, cy, radius);
      }
      if (show_number) {
        fl_color(0x207a2000);
        fl_text_extents(std::to_string(i).c_str(), text_dx, text_dy, text_width, text_height);
        fl_graphics_driver->draw(std::to_string(i).c_str(), std::to_string(i).size(),
                                 (float)(cx - text_dx - (float)text_width / 2),
                                 (float)(cy - text_dy - (float)text_height / 2));
      }
    } else {
      double r = radius;
      if (game_state && last_move.src == i) {
        fl_color(game_state->turn == RED ? 0xcfffcf00 : 0xffafaf00);
        circle(cx, cy, r + scale);
        r -= 3 * scale;
      }
      if (selected_piece >= 0 && moves[selected_piece][i] == 1) {
        fl_color(0xd6b17d00);
      } else {
        fl_color(0x977d5a00);
      }
      circle(cx, cy, r);
      if (show_number) {
        fl_color(0x5b4b3600);
        fl_text_extents(std::to_string(i).c_str(), text_dx, text_dy, text_width, text_height);
        fl_graphics_driver->draw(std::to_string(i).c_str(), std::to_string(i).size(),
                                 (float)(cx - text_dx - (float)text_width / 2),
                                 (float)(cy - text_dy - (float)text_height / 2));
      }
    }
  }
  fl_color(0x977d5a00);
  for (int i = 0; i < 40; i++) {
    double cx = FAKE_CIRCLE_POSITIONS[i][0] * scale + dx;
    double cy = FAKE_CIRCLE_POSITIONS[i][1] * scale + dy;
    circle(cx, cy, radius);
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
        if (my_color == GREEN) {
          click_number = 80 - click_number;
        }
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
  if (game_state == nullptr || game_state->isGameOver()) return;
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

void Fl_ChessBoard::user_color(int my_color) {
  this->my_color = my_color;
  fill_moves();
  selected_piece = -1;
  last_move = {-1, -1};
  redraw();
}

int Fl_ChessBoard::color() { return my_color; }

Move Fl_ChessBoard::get_user_last_move() { return last_move; }
