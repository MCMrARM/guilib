#pragma once

#include <memory>
#include <vector>
#include "ninepatch.h"
#include "element.h"

namespace guilib {

class GuiElement;
class GuiNinepatchTexture;

class GuiContainer : public GuiElement {

private:

    std::vector<std::shared_ptr<GuiElement>> elements;

public:

    GuiContainer(std::weak_ptr<GuiElement> parent);

    virtual void draw();

    virtual void onSizeChanged();

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