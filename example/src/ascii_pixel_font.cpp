#include "ascii_pixel_font.h"
#include "image.h"

#include <fstream>

GuiPixelFont::GuiPixelFont(std::string const& fontPath) {
    std::ifstream stream (fontPath);
    Image image = Image::fromStream(stream);
    printf("Test\n");
}

float GuiPixelFont::getCharacterWidth(int c) {
    return 4.f;
}

void GuiPixelFont::draw(float x, float y, std::string const& text, guilib::GuiColor const& color) {
    //printf("Draw string: %s [%f, %f]\n", text.c_str(), x, y);
}