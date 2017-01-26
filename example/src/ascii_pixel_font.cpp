#include "ascii_pixel_font.h"
#include "image.h"

#include <fstream>
#include <guilib/mesh_builder.h>

#define CHARS_W 16
#define CHARS_H 16

using namespace guilib;

GuiPixelFont::GuiPixelFont(std::string const& fontPath) {
    std::ifstream stream (fontPath);
    Image image = Image::fromStream(stream);
    w = image.getWidth();
    h = image.getHeight();
    fontTex = std::unique_ptr<Texture>(new Texture(TextureImpl(image)));
}

float GuiPixelFont::getCharacterWidth(int c) {
    return 9.f;
}

void GuiPixelFont::draw(float x, float y, std::string const& text, GuiColor const& color) {
    MeshBuilder builder (fontTex);
    builder.color(color);
    for (char c : text) {
        int cid = (int) c;
        int cx = cid % CHARS_W;
        int cy = cid / CHARS_W;

        float x1 = x, x2 = x + 8.f, y1 = y, y2 = y + 8.f;
        float uvX1 = (float) cx / CHARS_W;
        float uvX2 = uvX1 + 1.f / CHARS_W;
        float uvY1 = 1.f - (float) cy / CHARS_H;
        float uvY2 = uvY1 - 1.f / CHARS_H;
        builder.vertexUV(x1, y1, 0.f, uvX1, uvY1);
        builder.vertexUV(x1, y2, 0.f, uvX1, uvY2);
        builder.vertexUV(x2, y1, 0.f, uvX2, uvY1);
        builder.vertexUV(x1, y2, 0.f, uvX1, uvY2);
        builder.vertexUV(x2, y2, 0.f, uvX2, uvY2);
        builder.vertexUV(x2, y1, 0.f, uvX2, uvY1);
        builder.build().draw();
        x += 9.f;
    }
}