#include <guilib/container.h>

using namespace guilib;

GuiContainer::GuiContainer(std::weak_ptr<GuiElement> parent) : GuiElement(parent), styleManager(*this){
    //
}

void GuiContainer::draw(float x, float y) {
    GuiElement::draw(x, y);
    auto padding = getPadding();
    x += padding.left;
    y += padding.top;
    for (auto& line : inlineLines) {
        for (auto& el : line.elements) {
            auto gui = el.element.lock();
            auto margin = gui->getMargin();
            if (el.isNonInline()) {
                auto pos = gui->getComputedPosition();
                gui->draw(pos.left + margin.left, pos.top + margin.top);
            } else {
                if (gui->style().display() == GuiDisplayMode::INLINE)
                    gui->draw(x + el.left, y + el.top, el.width, el.height, el.argLineNo);
                else
                    gui->draw(x + el.left + margin.left, y + el.top + margin.top);
            }
        }
    }
}

void GuiContainer::onSizeChanged() {
    repositionElements();
}

void GuiContainer::repositionElements() {
    GuiInlineBuilder builder (getComputedInnerSize().width);
    for (auto& el : elements) {
        GuiDisplayMode disp = el->style().display();
        if ((disp == GuiDisplayMode::INLINE || disp == GuiDisplayMode::INLINE_BLOCK) && el->isInlineSupported()) {
            builder.setElement(el);
            el->buildInline(builder);
        } else {
            builder.addNonInline(el);
        }
    }
    builder.resetElement();
    inlineLines = builder.buildAndAlign(style().textAlign());
}

void GuiContainer::addElement(std::shared_ptr<GuiElement> element) {
    elements.push_back(element);
    repositionElements();
}

void GuiContainer::clear() {
    elements.clear();
}