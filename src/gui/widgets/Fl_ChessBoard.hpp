#ifndef Fl_ChessBoard_H
#define Fl_ChessBoard_H

#include <FL/Fl_Widget.H>

#include <game.hpp>

class Fl_ChessBoard : public Fl_Widget {
 private:
  double scale;
  double dx;
  double dy;
  GameState *game_state;
  int selected_piece;
  int moves[81][81];
  Move last_move;
  bool show_number;
  int my_color;

 public:
  Fl_ChessBoard(int x, int y, int w, int h, int my_color = RED);
  void draw() override;
  virtual int handle(int event) override;
  void set_game_state(GameState *game_state);
  void handle_circle_click(int index);
  void fill_moves();
  void number(bool is_show);
  bool number();
  void user_color(int my_color);
  int color();
  void move(Move move);
  Move get_user_last_move();
};

#endif
