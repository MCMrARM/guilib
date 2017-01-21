#include <guilib/container.h>

using namespace guilib;

GuiContainer::GuiContainer(std::weak_ptr<GuiElement> parent) : GuiElement(parent) {
    //
}

void GuiContainer::draw() {
    for (auto& ch : elements)
        ch->draw();
}

void GuiContainer::onSizeChanged() {
    //
}

void GuiContainer::addElement(std::shared_ptr<GuiElement> element) {
    elements.push_back(element);
}

void GuiContainer::clear() {
    elements.clear();
}