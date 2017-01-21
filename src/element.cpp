#include <guilib/element.h>

using namespace guilib;

GuiElement::GuiElement(std::weak_ptr<GuiElement> parent) : parent(parent) {
    //
}

GuiComputedPosition GuiElement::getComputedPosition() const {
    return {cleft, ctop};
}

GuiComputedSize GuiElement::getComputedSize() const {
    return {width, height};
}

void GuiElement::onParentSizeChange() {
    if (parent.expired())
        return;
    GuiComputedSize ps = parent.lock()->getComputedSize();
    if (left != -1.f && right != -1.f) {
        this->width = ps.width - left - right;
        this->cleft = left;
    } else if (left == -1.f && right == -1.f) {
        this->width = 0.f;
        this->cleft = 0.f;
    } else if (left == -1.f) {
        this->cleft = ps.width - right - width;
    } else if (right == -1.f) {
        this->cleft = left;
    }
    if (top != -1.f && bottom != -1.f) {
        this->height = ps.height - top - bottom;
        this->ctop = top;
    } else if (top == -1.f && bottom == -1.f) {
        this->height = 0.f;
        this->ctop = 0.f;
    } else if (top == -1.f) {
        this->ctop = ps.height - bottom - height;
    } else if (bottom == -1.f) {
        this->ctop = top;
    }
}

void GuiElement::setPosAndSize(float x, float y, float w, float h) {
    this->left = x;
    this->top = y;
    this->right = -1.f;
    this->bottom = -1.f;
    this->width = w;
    this->height = h;
    onParentSizeChange();
}

void GuiElement::setPos(float left, float top, float right, float bottom, float width, float height) {
    if (left == -1.f && right == -1.f)
        throw std::logic_error("You must anchor the element to either left or right or both");
    if (top == -1.f && bottom == -1.f)
        throw std::logic_error("You must anchor the element to either top or bottom or both");
    this->left = left;
    this->top = top;
    this->right = right;
    this->bottom = bottom;
    this->width = width;
    this->height = height;
    onParentSizeChange();
}

void GuiElement::setPos(GuiAnchors const& anchors, float left, float top, float right, float bottom) {
    if (parent.expired())
        throw std::runtime_error("No parent element to anchor to");
    std::shared_ptr<GuiElement> p = parent.lock();
    GuiComputedSize ps = p->getComputedSize();

    if (!anchors.left && !anchors.right)
        throw std::logic_error("You must anchor the element to either left or right or both");
    if (!anchors.top && !anchors.bottom)
        throw std::logic_error("You must anchor the element to either top or bottom or both");

    this->left = left;
    this->right = right;
    this->width = std::max<float>(ps.width - left - right, 0.f);
    if (!anchors.left)
        this->left = -1.f;
    if (anchors.right)
        this->right = -1.f;

    this->top = top;
    this->bottom = bottom;
    this->height = std::max<float>(ps.height - top - bottom, 0.f);
    if (!anchors.top)
        this->top = -1.f;
    if (anchors.bottom)
        this->bottom = -1.f;

    onParentSizeChange();
}