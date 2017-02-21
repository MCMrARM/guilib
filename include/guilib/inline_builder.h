#pragma once

#include <memory>
#include <vector>
#include "layout_properties.h"

namespace guilib {

class GuiElement;

struct GuiInlineLine {

    struct Element {

        std::weak_ptr<GuiElement> element;
        float left, top;
        float width, height;
        int argLineNo;

        bool isNonInline() const { return argLineNo == -1; }

    };

    std::vector<Element> elements;
    float lineHeight = 0.f;

};

class GuiInlineBuilder {

protected:

    std::vector<GuiInlineLine> lines;
    float lineRemainingWidth;
    float lineTotalWidth;
    std::weak_ptr<GuiElement> currentElement;

public:

    GuiInlineBuilder(float lineWidth);

    void setElement(std::weak_ptr<GuiElement> el);

    void resetElement();

    void add(std::weak_ptr<GuiElement> element, float width, float height, int argLineNo = 0);

    void add(float width, float height, int argLineNo = 0);

    void addNonInline(std::weak_ptr<GuiElement> element);

    void nextLine();

    std::vector<GuiInlineLine> buildAndAlign(GuiTextAlign align);

    float getRemainingWidth() const { return lineRemainingWidth; }

};

}