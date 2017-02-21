#pragma once

#include <memory>
#include <vector>
#include "ninepatch.h"
#include "element.h"
#include "inline_builder.h"
#include "layout_properties.h"

#define DEF_NAME GuiContainer
#define DEF_LISTH "style_props_container.h"
#include "style_def.h"

namespace guilib {

class GuiElement;
class GuiNinepatchTexture;

class GuiContainer : public GuiElement {

    GUI_OBJECT_STYLE(GuiContainer, GuiElement)

private:

    std::vector<std::shared_ptr<GuiElement>> elements;
    std::vector<GuiInlineLine> inlineLines;

public:

    GuiContainer(std::weak_ptr<GuiElement> parent);

    virtual void draw(float x, float y);

    virtual void onSizeChanged();

    void repositionElements();

    /**
     * Add an element to this container.
     * @param element the element to add
     */
    void addElement(std::shared_ptr<GuiElement> element);

    /**
     * Clear this element container.
     */
    void clear();

};

}