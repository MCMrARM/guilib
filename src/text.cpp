#include <guilib/text.h>
#include <guilib/font.h>
#include <guilib/inline_builder.h>
#include <utf8proc.h>

using namespace guilib;

GuiText::GuiText(std::weak_ptr<GuiElement> parent, std::string const& text) : GuiElement(parent), text(text),
                                                                              styleManager(*this) {
    font = GuiFont::defaultFont;
}

void GuiText::draw(float x, float y) {
    auto padding = getPadding();
    x += padding.left;
    y += padding.top;

    auto& color = style().textColor();
    float lineHeight = 10.f;
    for (auto& line : drawLines) {
        font->draw(x, y, line, color);
        y += lineHeight;
    }
}

void GuiText::draw(float x, float y, int mlLineNr) {
    if (mlLineNr == 0)
        x += getPadding().left;
    font->draw(x, y, drawLines[mlLineNr], style().textColor());
}

void GuiText::updateWordWrap(GuiInlineBuilder* builder) {
    drawLines.clear();

    float maxW;
    if (builder != nullptr)
        maxW = builder->getRemainingWidth();
    else
        maxW = getComputedInnerSize().width;

    float lineHeight = 10.f;

    const uint8_t* strs = (const uint8_t*) text.c_str();
    size_t strslen = text.length();

    const uint8_t* str = strs;
    const uint8_t* stre = strs + strslen;

    size_t lastAddedOff = 0;
    size_t lastSpaceOff = 0;

    int lineNo = 0, n, c;
    float w = 0.f;
    if (builder != nullptr)
        w += getPadding().left + getMargin().left;
    while ((n = (int) utf8proc_iterate(str, strslen - (str - strs), &c)) > 0) {
        if (c == ' ') {
            lastSpaceOff = str - strs;
        }
        w += font->getCharacterWidth(c);
        if (builder != nullptr && &str[n] == stre)
            w += getPadding().right + getMargin().right;
        if (w >= maxW) {
            drawLines.push_back(text.substr(lastAddedOff, lastSpaceOff - lastAddedOff));
            if (builder != nullptr) {
                builder->add(w, lineHeight, lineNo);
                builder->nextLine();
                maxW = builder->getRemainingWidth();
            }
            lastAddedOff = lastSpaceOff + 1;
            lineNo++;
            str = strs + lastSpaceOff + 1;
            w = 0.f;
        }

        str = &str[n];
    }
    drawLines.push_back(text.substr(lastAddedOff));
    if (builder != nullptr)
        builder->add(w, lineHeight, lineNo);
}

void GuiText::buildInline(GuiInlineBuilder& builder) {
    updateWordWrap(&builder);
}