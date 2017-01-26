#include "image.h"

#include <stdexcept>
#include <vector>
#include <png.h>

Image::Image(std::vector<unsigned char> data, int width, int height, ImageFormat format) :
        data(data), width(width), height(height), format(format) {
    //
}

Image Image::fromStream(std::istream& stream) {
    char sig[8];
    stream.read(sig, 8);
    if (stream.gcount() == 8 && png_sig_cmp((png_bytep) sig, 0, 8))
        throw std::runtime_error("Not a valid png image: invalid signature");

    png_struct* png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if (!png_ptr)
        throw std::runtime_error("Failed to allocate PNG read struct.");
    png_info* info_ptr = png_create_info_struct(png_ptr);
    if (!info_ptr) {
        png_destroy_read_struct(&png_ptr, NULL, NULL);
        throw std::runtime_error("Failed to allocate PNG info struct.");
    }

    png_set_read_fn(png_ptr, (void*) &stream, [](png_structp png_ptr, png_bytep out, png_size_t size) {
        std::istream* stream = (std::istream*) png_get_io_ptr(png_ptr);
        stream->read((char*) out, size);
        if (stream->gcount() != size)
            png_error(png_ptr, "IO error");
    });
    png_set_sig_bytes(png_ptr, 8);
    png_read_info(png_ptr, info_ptr);

    ImageFormat format = ImageFormat::RGBA;

    int depth, colorType;
    png_uint_32 width, height;
    png_get_IHDR(png_ptr, info_ptr, &width, &height, &depth, &colorType, NULL, NULL, NULL);

    if (colorType == PNG_COLOR_TYPE_RGB)
        format = ImageFormat::RGB;
    else if (colorType == PNG_COLOR_TYPE_GRAY)
        format = ImageFormat::GRAY;
    else if (colorType == PNG_COLOR_TYPE_GRAY_ALPHA) {
        format = ImageFormat::GRAY;
        png_set_strip_alpha(png_ptr);
        png_read_update_info(png_ptr, info_ptr);
    }
    if (colorType == PNG_COLOR_TYPE_PALETTE && depth <= 8) {
        png_set_palette_to_rgb(png_ptr);
        if (png_get_valid(png_ptr, info_ptr, PNG_INFO_tRNS)) {
            png_set_tRNS_to_alpha(png_ptr);
            format = ImageFormat::RGBA;
        } else {
            format = ImageFormat::RGB;
        }
        png_read_update_info(png_ptr, info_ptr);
    } else if (depth < 8 || png_get_valid(png_ptr, info_ptr, PNG_INFO_tRNS)) {
        png_set_expand(png_ptr);
        png_read_update_info(png_ptr, info_ptr);
    }
    if (depth == 16) {
        png_set_strip_16(png_ptr);
        png_read_update_info(png_ptr, info_ptr);
    }

    png_size_t rowBytes = png_get_rowbytes(png_ptr, info_ptr);

    png_bytep* rows = new png_bytep[height];
    std::vector<unsigned char> data(rowBytes * height);
    for (unsigned int i = 0; i < height; i++)
        rows[i] = &data[(height - 1 - i) * rowBytes];

    png_read_image(png_ptr, rows);

    png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
    delete[] rows;

    return Image(std::move(data), (int) width, (int) height, format);
}