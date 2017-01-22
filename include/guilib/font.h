#pragma once

#include "color.h"

namespace guilib {

class GuiFont {

public:

    virtual float getCharacterWidth(int c) = 0;

    virtual void draw(float x, float y, std::string const& text, GuiColor const& color) = 0;

};

}