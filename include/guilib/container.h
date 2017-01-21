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

    std::vector<std::unique_ptr<GuiElement>> elements;

    GuiNinepatchImage background;

public:

    GuiContainer(std::weak_ptr<GuiElement> parent) : GuiElement(parent) { }

    virtual void draw();

    virtual void onSizeChanged();

    /**
     * Add an element to this container.
     * @param element the element to add
     */
    void addElement(std::unique_ptr<GuiElement> element);

    /**
     * Clear this element container.
     */
    void clear();

    /**
     * Sets the container background image.
     * @param texture the new background to set
     */
    void setBackground(GuiNinepatchTexture const& texture);

};

}