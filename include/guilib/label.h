#pragma once

#include <string>
#include "element.h"
#include "skin.h"
#include "color.h"

namespace guilib {

class GuiLabel : public GuiElement {

private:

    GuiLabelSkin skin;
    bool hasCustomColor = false;
    GuiColor customColor;
    std::string text;

public:

    GuiLabel(std::string const& text);
    GuiLabel(std::string const& text, GuiColor customColor);

    void setColor(GuiColor const& color);

    virtual void draw();

    virtual void setSkin(GuiSkin const& skin);

};

}