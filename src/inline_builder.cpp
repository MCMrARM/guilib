#include <guilib/inline_builder.h>
#include <guilib/element.h>

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

std::vector<GuiInlineLine> GuiInlineBuilder::buildAndAlign(GuiTextAlign align) {
    float y = 0.f;
    for (auto& line : lines) {
        float x = 0.f;
        for (auto& el : line.elements) {
            auto gui = el.element.lock();
            if (el.isNonInline()) {
                gui->onParentSizeChange();
            } else {
                el.left = x;
                el.top = y;
                if (el.argLineNo == 0)
                    gui->setInlinePosAndSize(x, y, el.width, el.height);
                x += el.width;
            }
        }
        float w = x;

        float off = 0.f;
        switch (align) {
            case GuiTextAlign::LEFT:
                off = 0.f;
                break;
            case GuiTextAlign::RIGHT:
                off = lineTotalWidth - w;
                break;
            case GuiTextAlign::CENTER:
                off = lineTotalWidth / 2 - w / 2;
                break;
            case GuiTextAlign::JUSTIFY:
                // TODO:
                break;
        }
        for (auto& el : line.elements)
            if (!el.isNonInline())
                el.left += off;

        y += line.lineHeight;
    }
    return std::move(lines);
}

void GuiInlineBuilder::nextLine() {
    this->lines.push_back(GuiInlineLine());
    lineRemainingWidth = lineTotalWidth;
}