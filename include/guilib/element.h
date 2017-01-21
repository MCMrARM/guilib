#pragma once

#include <memory>

namespace guilib {

class GuiSkin;
class GuiContainer;

struct GuiAnchors {
    bool left, right, top, bottom;
};
struct GuiComputedPosition {
    float left, top;
};
struct GuiComputedSize {
    float width, height;
};

class GuiElement {

protected:

    std::weak_ptr<GuiElement> parent;

    // position
    float left = 0.f, top = 0.f, right = 0.f, bottom = 0.f;
    float width = 0.f, height = 0.f;

    // computed position
    float cleft = 0.f, ctop = 0.f;

public:

    GuiElement(std::weak_ptr<GuiElement> parent);

    virtual ~GuiElement() { }

    virtual void draw() = 0;

    /**
     * The function called when the size of this element changes. This will be called by all functions that update the
     * position and shouldn't really be called manually.
     */
    virtual void onSizeChanged() { }

    /**
     * This function notifies the element that the parent element has been resized. It'll cause this element to resize
     * as well. You're only supposed to call it from the container that owns the element.
     */
    void onParentSizeChange();

    /**
     * Set the GUI skin for this element. If this change is called on an container this change will be propagated
     * to all children elements.
     * @param skin the new gui skin to use
     */
    virtual void setSkin(GuiSkin const& skin) { }

    /**
     * Returns the current element position.
     * @return a structure containing the position
     */
    GuiComputedPosition getComputedPosition() const;

    /**
     * Returns the current element size.
     * @return a structure containing the size
     */
    GuiComputedSize getComputedSize() const;

    /**
     * This function sets the element's position and size, relative to the top left corner of the screen.
     * @param x the left coordinate
     * @param y the top coordinate
     * @param w the element width
     * @param h the element height
     */
    void setPosAndSize(float x, float y, float w, float h);

    /**
     * Sets the position relative to the parent element. All values must be passed based on the parent element
     * size and anchors will specify how the element will behave when resizing the parent element.
     * @param anchors the anchors to use to resize this element when the parent element is being resized
     * @param left the left coordinate (relative to the parent element)
     * @param top the top coordinate (relative to the parent element)
     * @param right the right coordinate (relative to the parent element)
     * @param bottom the bottom coordinate (relative to the parent element)
     */
    void setPos(GuiAnchors const& anchors, float left, float top, float right, float bottom);

    /**
     * Sets the position relative to the parent element. All values must be passed based on the parent element
     * size and anchors will specify how the element will behave when resizing the parent element.
     * @param left the left coordinate relative to parent element or -1.f if it shouldn't be used as an anchor
     * @param top the top coordinate relative to the parent element or -1.f if it shouldn't be used as an anchor
     * @param right the right coordinate relative to the parent element or -1.f if it shouldn't be used as an anchor
     * @param bottom the bottom coordinate relative to the parent element or -1.f if it shouldn't be used as an anchor
     * @param width the width of this element
     * @param height the height of this element
     */
    void setPos(float left, float top, float right, float bottom, float width, float height);



};

}