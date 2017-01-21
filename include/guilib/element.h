#pragma once

#include <memory>

#define DEF_NAME GuiElement
#define DEF_LISTH "style_props_element.h"
#include "style_def.h"

namespace guilib {

class GuiContainer;

struct GuiComputedPosition {
    float left, top;
};
struct GuiComputedSize {
    float width, height;
};

class GuiElement {

public:

    struct StyleManager : public GuiElementStyleManager {
        StyleManager(GuiElement& element) : GuiElementStyleManager(element, element.elementStyleData) { }
    };

protected:

    std::weak_ptr<GuiElement> parent;

    GuiStyleState state;

    // computed position
    float cleft = 0.f, ctop = 0.f;
    float cwidth = 0.f, cheight = 0.f;
    bool cdirty = true;

    /**
     * This function will be called to change the used style state. If you need to
     * @param newState the new state to set
     */
    virtual void setStyleState(GuiStyleState newState);

private:

    GuiStyleObjectData<GuiElementStyle> elementStyleData;
    StyleManager styleManager;

    void recalculatePosAndSize();

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
    virtual void onParentSizeChange();

    /**
     * Allows you to change this GuiElement's appearance or get property values.
     * @return the style manager
     */
    StyleManager& style() { return styleManager; }

    /**
     * Allows you to get this GuiElement's style-related property values.
     * @return a const reference to the style manager
     */
    StyleManager const& style() const { return styleManager; }

    /**
     * Returns the current element position (and caches it for further calls).
     * @return a structure containing the position
     */
    GuiComputedPosition getComputedPosition();

    /**
     * Returns the current element size (and caches it for further calls).
     * @return a structure containing the size
     */
    GuiComputedSize getComputedSize();

    /**
     * This function is supposed to be only called internally; forces recalculation of the position coordinates and the
     * size of this element.
     */
    void notifyPositionDataChanged();

};

}