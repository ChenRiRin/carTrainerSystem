#include <FL/Fl.H>
#include "ui/AppWindow.h"

int main(int argc, char** argv) {
    Fl::scheme("gleam");
    Fl::background(242, 242, 247);
    Fl::background2(255, 255, 255);
    Fl::set_color(FL_SELECTION_COLOR, 229, 229, 234);

    AppWindow win;
    win.show(argc, argv);
    return Fl::run();
}
