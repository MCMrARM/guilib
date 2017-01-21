#pragma once

#include <string>
#include "element.h"

namespace guilib {

class GuiButton : public GuiElement {

public:

    GuiButton(std::weak_ptr<GuiElement> parent, std::string const& text);

    void setText(std::string const& text);

    virtual void draw();

};

}