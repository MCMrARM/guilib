#include <guilib/font.h>

using namespace guilib;

std::vector<std::shared_ptr<GuiFont>> GuiFont::fonts;
std::shared_ptr<GuiFont> GuiFont::defaultFont;

void GuiFont::registerFont(std::shared_ptr<GuiFont> font) {
    fonts.push_back(font);
    if (!defaultFont)
        defaultFont = font;
}