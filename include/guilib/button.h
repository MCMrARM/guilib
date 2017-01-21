#pragma once

#include <string>
#include "element.h"
#include "skin.h"

namespace guilib {

class GuiButton : public GuiElement {

private:

    bool hasCustomSkin = false;
    GuiButtonSkin skin;
    std::string text;

public:

    GuiButton(std::weak_ptr<GuiElement> parent, std::string const& text);

    void setText(std::string const& text);

    void setOverrideSkin(GuiButtonSkin const& skin);

    virtual void draw();

    virtual void setSkin(GuiSkin const& skin);

};

}