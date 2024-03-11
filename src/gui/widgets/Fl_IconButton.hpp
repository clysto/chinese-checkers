#ifndef Fl_IconButton_H
#define Fl_IconButton_H

#include <FL/Fl_Button.H>
#include <FL/Fl_SVG_Image.H>

#include <string>

class Fl_IconButton : public Fl_Button {
 private:
  Fl_SVG_Image svg_image;

 public:
  enum { ICON_DICE_5_FILL, ICON_ARROW_LEFT_SQUARE_FILL, ICON_CLIPBOARD_CHECK_FILL, ICON_1_CIRCLE_FILL };
  Fl_IconButton(int x, int y, int w, int h, const char* label, int icon);
};

#endif