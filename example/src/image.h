#pragma once

#include <istream>
#include <vector>

enum class ImageFormat {
    GRAY, RGB, RGBA
};

class Image {

private:

    std::vector<unsigned char> data;
    int width, height;
    ImageFormat format;

public:

    Image(std::vector<unsigned char> data, int width, int height, ImageFormat format);

    std::vector<unsigned char>& getData() { return data; }
    std::vector<unsigned char> const& getData() const { return data; }

    int getWidth() const { return width; }
    int getHeight() const { return height; }
    ImageFormat getFormat() const { return format; }

    static Image fromStream(std::istream& path);

};