#pragma once

#include <string>
#include "element.h"

#define DEF_NAME GuiText
#define DEF_LISTH "style_props_text.h"
#include "style_def.h"

namespace guilib {

class GuiText : public GuiElement {

    GUI_OBJECT_STYLE(GuiText, GuiElement)

private:

    std::string text;

public:

    GuiText(std::weak_ptr<GuiElement> parent, std::string const& text);

    virtual void draw();

};

}