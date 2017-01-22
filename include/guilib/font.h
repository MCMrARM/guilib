#pragma once

#include <string>
#include <vector>
#include <memory>
#include "color.h"

namespace guilib {

class GuiFont {

public:

    static std::vector<std::shared_ptr<GuiFont>> fonts;
    static std::shared_ptr<GuiFont> defaultFont;

    static void registerFont(std::shared_ptr<GuiFont> font);

    virtual float getCharacterWidth(int c) = 0;

    virtual void draw(float x, float y, std::string const& text, GuiColor const& color) = 0;

};

}