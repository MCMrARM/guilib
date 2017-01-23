#include <guilib/inline_builder.h>

using namespace guilib;

GuiInlineBuilder::GuiInlineBuilder(float lineWidth) : lineRemainingWidth(lineWidth), lineTotalWidth(lineWidth) {
    //
}

void GuiInlineBuilder::setElement(std::weak_ptr<GuiElement> el) {
    currentElement = el;
}

void GuiInlineBuilder::resetElement() {
    currentElement.reset();
}

void GuiInlineBuilder::add(float width, float height, int argLineNo) {
    add(currentElement, width, height, argLineNo);
}

void GuiInlineBuilder::add(std::weak_ptr<GuiElement> element, float width, float height, int argLineNo) {
    if (this->lines.size() == 0 || getRemainingWidth() < width)
        nextLine();
    if (argLineNo < 0)
        throw std::logic_error("The line number must be >= 0");
    auto& line = this->lines.back();
    line.elements.push_back({element, -1.f, -1.f, width, height, argLineNo});
    line.lineHeight = std::max(line.lineHeight, height);
    lineRemainingWidth -= width;
}

void GuiInlineBuilder::addNonInline(std::weak_ptr<GuiElement> element) {
    if (this->lines.size() == 0)
        nextLine();
    this->lines.back().elements.push_back({element, 0.f, 0.f, 0.f, 0.f, -1});
}

void GuiInlineBuilder::nextLine() {
    this->lines.push_back(GuiInlineLine());
    lineRemainingWidth = lineTotalWidth;
}