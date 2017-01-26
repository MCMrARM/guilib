#pragma once

#include <guilib/font.h>

class GuiPixelFont : public guilib::GuiFont {

public:

    GuiPixelFont(std::string const& fontPath);

    virtual float getCharacterWidth(int c);

    virtual void draw(float x, float y, std::string const& text, guilib::GuiColor const& color);

};