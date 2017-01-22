#include <guilib/text.h>
#include <guilib/font.h>
#include <guilib/inline_builder.h>

using namespace guilib;

extern "C" {
int utf8proc_iterate(const uint8_t* str, size_t str_len, int32_t* dst);
};

void GuiText::draw(float x, float y) {
    auto& color = style().textColor();
    float lineHeight = 10.f;
    for (auto& line : drawLines) {
        font->draw(x, y, line, color);
        y += lineHeight;
    }
}

void GuiText::draw(float x, float y, int mlLineNr) {
    font->draw(x, y, drawLines[mlLineNr], style().textColor());
}

void GuiText::updateWordWrap(GuiInlineBuilder* builder) {
    drawLines.clear();

    float maxW;
    if (builder != nullptr)
        maxW = builder->getRemainingWidth();
    else
        maxW = getComputedSize().width;

    float lineHeight = 10.f;

    const uint8_t* strs = (const uint8_t*) text.c_str();
    size_t strslen = text.length();

    const uint8_t* str = strs;

    size_t lastAddedOff = 0;
    size_t lastSpaceOff = 0;

    int lineNo = 0, n, c;
    float w = 0.f;
    while ((n = utf8proc_iterate(str, strslen - (str - strs), &c)) > 0) {
        if (c == ' ') {
            lastSpaceOff = str - strs;
        }
        w += font->getCharacterWidth(c);
        if (w >= maxW) {
            drawLines.push_back(text.substr(lastAddedOff, lastSpaceOff - lastAddedOff));
            if (builder != nullptr) {
                builder->add(w, lineHeight, lineNo);
                maxW = builder->getRemainingWidth();
            }
            lastAddedOff = lastSpaceOff;
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