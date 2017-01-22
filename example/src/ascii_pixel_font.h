#pragma once

#include <guilib/font.h>

class GuiPixelFont : public guilib::GuiFont {

public:

    virtual float getCharacterWidth(int c);

    virtual void draw(float x, float y, std::string const& text, guilib::GuiColor const& color);

};