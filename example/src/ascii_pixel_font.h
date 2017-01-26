#pragma once

#include <guilib/font.h>
#include <guilib/texture.h>

class GuiPixelFont : public guilib::GuiFont {

private:

    std::shared_ptr<guilib::Texture> fontTex;
    int w, h;

public:

    GuiPixelFont(std::string const& fontPath);

    virtual float getCharacterWidth(int c);

    virtual void draw(float x, float y, std::string const& text, guilib::GuiColor const& color);

};