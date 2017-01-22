#include <guilib/element.h>
#include <guilib/inline_builder.h>

#define DEF_NAME GuiElement
#define DEF_LISTH <guilib/style_props_element.h>
#include <guilib/style_def_callbacks.h>

using namespace guilib;

GuiElement::GuiElement(std::weak_ptr<GuiElement> parent) : parent(parent), styleManager(*this) {
    //
}

void GuiElement::setInlinePosAndSize(float x, float y, float w, float h) {
    cleft = x;
    ctop = y;
    cwidth = w;
    cheight = h;
    cdirty = false;
}

void GuiElement::buildInline(GuiInlineBuilder& builder) {
    auto size = getComputedOuterSize();
    builder.add(size.width, size.height);
}

void GuiElement::setStyleState(GuiStyleState newState) {
    styleManager.updateState(newState);
}

GuiMargin GuiElement::getMargin() {
    if (cdirty)
        recalculatePosAndSize();
    return cmargin;
}

GuiMargin GuiElement::getPadding() {
    if (cdirty)
        recalculatePosAndSize();
    return cpadding;
}

GuiComputedPosition GuiElement::getComputedPosition() {
    if (cdirty)
        recalculatePosAndSize();
    return {cleft, ctop};
}

GuiComputedSize GuiElement::getComputedOuterSize() {
    if (cdirty)
        recalculatePosAndSize();
    return {cwidth, cheight};
}

GuiComputedSize GuiElement::getComputedSize() {
    GuiComputedSize size = getComputedOuterSize();
    GuiMargin margin = getMargin();
    return {size.width - margin.left - margin.right,
            size.height - margin.top - margin.bottom};
}

GuiComputedSize GuiElement::getComputedInnerSize() {
    GuiComputedSize size = getComputedSize();
    GuiMargin padding = getPadding();
    return {size.width - padding.left - padding.right,
            size.height - padding.top - padding.bottom};
}

void GuiElement::recalculatePosAndSize() {
    cmargin = {style().marginLeft(), style().marginTop(), style().marginRight(), style().marginBottom()};
    cpadding = {style().paddingLeft(), style().paddingTop(), style().paddingRight(), style().paddingBottom()};

    float pwidth = cwidth;
    float pheight = cheight;
    if (style().display() != GuiDisplayMode::INLINE) {
        if (pwidth != -1.f)
            cwidth = style().width();
        if (pheight != -1.f)
            cheight = style().height();
    }

    if (parent.expired() || style().display() != GuiDisplayMode::BLOCK)
        return;
    GuiComputedSize ps = parent.lock()->getComputedInnerSize();

    float left = style().left();
    float right = style().right();
    float top = style().top();
    float bottom = style().bottom();

    if (left != -1.f && right != -1.f) {
        this->cwidth = ps.width - left - right;
        this->cleft = left;
    } else if (left == -1.f && right == -1.f) {
        this->cwidth = 0.f;
        this->cleft = 0.f;
    } else if (left == -1.f) {
        this->cleft = ps.width - right - cwidth;
    } else if (right == -1.f) {
        this->cleft = left;
    }
    if (top != -1.f && bottom != -1.f) {
        this->cheight = ps.height - top - bottom;
        this->ctop = top;
    } else if (top == -1.f && bottom == -1.f) {
        this->cheight = 0.f;
        this->ctop = 0.f;
    } else if (top == -1.f) {
        this->ctop = ps.height - bottom - cheight;
    } else if (bottom == -1.f) {
        this->ctop = top;
    }
    cdirty = false;
    if (pwidth != cwidth || pheight != cheight)
        onSizeChanged();
}

void GuiElement::onParentSizeChange() {
    notifyPositionDataChanged();
}

void GuiElement::notifyPositionDataChanged() {
    printf("NotifyPositionDataChanged\n");
    cdirty = true;
}