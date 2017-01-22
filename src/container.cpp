#include <guilib/container.h>

using namespace guilib;

GuiContainer::GuiContainer(std::weak_ptr<GuiElement> parent) : GuiElement(parent) {
    //
}

void GuiContainer::draw(float x, float y) {
    for (auto& line : inlineLines) {
        for (auto& el : line.elements) {
            auto gui = el.element.lock();
            auto pos = gui->getComputedPosition();
            gui->draw(x + pos.left, y + pos.top);
        }
    }
}

void GuiContainer::onSizeChanged() {
    repositionElements();
}

void GuiContainer::repositionElements() {
    GuiInlineBuilder builder (getComputedSize().width);
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
    inlineLines = builder.build();
    float y = 0.f;
    for (auto& line : inlineLines) {
        float x = 0.f;
        for (auto& el : line.elements) {
            auto gui = el.element.lock();
            if (el.isNonInline()) {
                gui->onParentSizeChange();
            } else {
                gui->setInlinePosAndSize(x, y, el.width, el.height);
                x += el.width;
            }
        }
        y += line.lineHeight;
    }
}

void GuiContainer::addElement(std::shared_ptr<GuiElement> element) {
    elements.push_back(element);
    repositionElements();
}

void GuiContainer::clear() {
    elements.clear();
}