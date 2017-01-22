#pragma once

namespace guilib {

class GuiColor {

public:

    float r, g, b, a;

    GuiColor() : r(1.f), g(1.f), b(1.f), a(1.f) { }
    GuiColor(float r, float g, float b, float a) : r(r), g(g), b(b), a(a) { }
    GuiColor(unsigned int argb);
    GuiColor(GuiColor const& c) : r(c.r), g(c.g), b(c.b), a(c.a) { }

    GuiColor& operator*(GuiColor const& c);

    unsigned int toARGB();

};

}