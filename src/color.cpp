#include <guilib/color.h>

using namespace guilib;

GuiColor::GuiColor(unsigned int argb) {
    r = ((argb >> 16) & 0xff) / 255.f;
    g = ((argb >> 8) & 0xff) / 255.f;
    b = (argb & 0xff) / 255.f;
    a = 1.f - ((argb >> 24) & 0xff) / 255.f;
}

GuiColor& GuiColor::operator*(GuiColor const& c) {
    r *= c.r;
    g *= c.g;
    b *= c.b;
    a *= c.a;
    return *this;
}

unsigned int GuiColor::toARGB() {
    unsigned int r = (unsigned int) (this->r * 255);
    unsigned int g = (unsigned int) (this->g * 255);
    unsigned int b = (unsigned int) (this->b * 255);
    unsigned int a = 255 - (unsigned int) (this->a * 255);
    return b | (g << 8) | (r << 16) | (a << 24);
}