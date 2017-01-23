#pragma once

#include <string>
#include <vector>
#include "element.h"

#define DEF_NAME GuiText
#define DEF_LISTH "style_props_text.h"
#include "style_def.h"

namespace guilib {

class GuiFont;

class GuiText : public GuiElement {

    GUI_OBJECT_STYLE(GuiText, GuiElement)

private:

    std::string text;
    std::vector<std::string> drawLines;
    std::shared_ptr<GuiFont> font;

    void updateWordWrap(GuiInlineBuilder* builder = nullptr);

public:

    GuiText(std::weak_ptr<GuiElement> parent, std::string const& text);

    virtual bool isInlineSupported() { return true; }

    virtual void draw(float x, float y);

    virtual void draw(float x, float y, float width, float height, int mlLineNr);

    virtual void buildInline(GuiInlineBuilder& builder);

    void notifyFontFamilyChanged();

};

}