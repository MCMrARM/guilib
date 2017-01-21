#pragma once

#include "ninepatch.h"
#include "color.h"

namespace guilib {

struct GuiButtonSkin {

    GuiNinepatchImage normal, hover, pressed;
    GuiColor normalColor, hoverColor, pressedColor;

};
struct GuiLabelSkin {

    GuiColor textColor;

};

class GuiSkin {

public:

    GuiButtonSkin button;
    GuiLabelSkin label;

};

}