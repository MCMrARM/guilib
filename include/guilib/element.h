#pragma once

#include <memory>

#define DEF_NAME GuiElement
#define DEF_LISTH "style_props_element.h"
#include "style_def.h"

namespace guilib {

class GuiContainer;
class GuiInlineBuilder;

struct GuiComputedPosition {
    float left, top;
};
struct GuiComputedSize {
    float width, height;
};
struct GuiMargin {
    float left, top, right, bottom;
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
    GuiMargin cmargin, cpadding;
    bool cdirty = true;

    /**
     * This function will be called to change the used style state. If you need to
     * @param newState the new state to set
     */
    virtual void setStyleState(GuiStyleState newState);

    friend class GuiContainer;

    void setInlinePosAndSize(float x, float y, float w, float h);

    /**
     * This function will be called before draw, to draw this element's decorations (border, background, etc).
     * @param x the absolute x (left) coordinate
     * @param y the absolute y (top) coordinate
     * @param width maximal width of the decorations
     * @param height maximal height of the decorations
     * @param noLeftBorder disable rendering of the left border
     * @param noRightBorder disable rendering of the right border
     */
    virtual void drawDecorations(float x, float y, float width, float height,
                                 bool noLeftBorder = false, bool noRightBorder = false);

private:

    GuiStyleObjectData<GuiElementStyle> elementStyleData;
    StyleManager styleManager;

    void recalculatePosAndSize();

public:

    GuiElement(std::weak_ptr<GuiElement> parent);

    virtual ~GuiElement() { }

    /**
     * This function will be called to draw this element if it's a block or inline-block.
     * @param x the absolute x (left) coordinate
     * @param y the absolute y (top) coordinate
     */
    virtual void draw(float x, float y);

    /**
     * This function will be called to draw this element if it's an inline element. The multiline line number may be
     * overridden by building a custom inline block structure via the buildInline function.
     * @param x the absolute x (left) coordinate
     * @param y the absolute y (top) coordinate
     * @param width the width of the element on the multiline line
     * @param height the height of the element on the multiline line
     * @param mlLineNr the multiline line number (you can specify it by overriding buildInline)
     */
    virtual void draw(float x, float y, float width, float height, int mlLineNr) { draw(x, y); }

    /**
     * Build the inline representation of this element. The default implementation only support inline-block display
     * mode, and requires a valid size calculation. Position will be ignored.
     * @param builder an inline builder instance, used to build multiline elements
     */
    virtual void buildInline(GuiInlineBuilder& builder);

    /**
     * Check if this element can be used as a completely inline element (display: inline). Even if this is set to false,
     * it'll support the inline-block display mode.
     * @return can this element be inline?
     */
    virtual bool isInlineSupported() { return false; }

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
     * Returns the element margin.
     */
    GuiMargin getMargin();

    /**
     * Returns the element padding.
     */
    GuiMargin getPadding();

    /**
     * Returns the current element outer size (and caches it for further calls).
     * The outer size is the total size of the element, including margins taken by this element.
     * @return a structure containing the size
     */
    GuiComputedSize getComputedOuterSize();

    /**
     * Returns the current element size (and caches it for further calls) excluding margins but including paddings.
     * @return a structure containing the size
     */
    GuiComputedSize getComputedSize();

    /**
     * Returns the current element inner size (and caches it for further calls).
     * The inner size is the element area excluding margin and padding.
     * @return a structure containing the size
     */
    GuiComputedSize getComputedInnerSize();

    /**
     * This function is supposed to be only called internally; forces recalculation of the position coordinates and the
     * size of this element.
     */
    void notifyPositionDataChanged();

};

}