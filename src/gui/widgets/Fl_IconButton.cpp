#include "Fl_IconButton.hpp"

#include <FL/fl_draw.H>

#include <cstring>
#include <iostream>
#include <string>

Fl_IconButton::Fl_IconButton(int x, int y, int w, int h, const char* label, int icon)
    : Fl_Button(x, y, w, h, label), svg_image(nullptr, ICON_TABLE[icon]) {
  box(FL_FLAT_BOX);
  color(0x2060c800);
  labelcolor(0xffffff00);
  down_box(FL_FLAT_BOX);
  down_color(0x174faa00);
  labelfont(FL_HELVETICA_BOLD);
  labelsize(16);
  clear_visible_focus();
  image(svg_image);
  align(FL_ALIGN_CENTER | FL_ALIGN_INSIDE | FL_ALIGN_IMAGE_NEXT_TO_TEXT);
}
