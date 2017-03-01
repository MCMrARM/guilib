#include <guilib/string_util.h>

#include <algorithm>

using namespace guilib;

int StrUtil::utf8_iterator::value() const {
    int ch = -1;
    readUTF8Char((unsigned char const*) &str[off], str.size() - off, ch);
    return ch;
}

size_t StrUtil::utf8_iterator::valueSize() const {
    if (off >= str.size())
        return 0;
    return getUTF8ReadCodePointSize((unsigned char) str[off]);
}

size_t StrUtil::getUTF8ReadCodePointSize(unsigned char firstChar) {
    if ((firstChar & 0b11000000) == 0b10000000) // invalid
        return 0;

    if ((firstChar & 0b11111110) == 0b11111100)
        return 6;
    if ((firstChar & 0b11111100) == 0b11111000)
        return 5;
    if ((firstChar & 0b11111000) == 0b11110000)
        return 4;
    if ((firstChar & 0b11110000) == 0b11100000)
        return 3;
    if ((firstChar & 0b11100000) == 0b11000000)
        return 2;
    return 1;
}

size_t StrUtil::readUTF8Char(unsigned char const* buf, size_t bufLen, int& ch) {
    ch = -1;
    if (bufLen < 1)
        return 0;
    unsigned char c = buf[0];
    if ((c & 0b11000000) == 0b10000000) // invalid
        return 0;

    if ((c & 0b11111110) == 0b11111100) {
        if (bufLen < 6 || (c & 0b1) == 0 || (buf[1] & 0b111111) == 0 || (buf[2] & 0b111111) == 0 ||
                (buf[3] & 0b111111) == 0 || (buf[4] & 0b111111) == 0 || (buf[5] & 0b111111) == 0)
            return 0;
        ch = ((((((((((c & 0b1) << 6) | (buf[1] & 0b111111)) << 6) | (buf[2] & 0b111111)) << 6) |
                (buf[3] & 0b111111)) << 6) | (buf[4] & 0b111111)) << 6) | (buf[5] & 0b111111);
        return 6;
    }
    if ((c & 0b11111100) == 0b11111000) {
        if (bufLen < 5 || (c & 0b11) == 0 || (buf[1] & 0b111111) == 0 || (buf[2] & 0b111111) == 0 ||
            (buf[3] & 0b111111) == 0 || (buf[4] & 0b111111) == 0)
            return 0;
        ch = ((((((((c & 0b11) << 6) | (buf[1] & 0b111111)) << 6) | (buf[2] & 0b111111)) << 6) |
                 (buf[3] & 0b111111)) << 6) | (buf[4] & 0b111111);
        return 5;
    }
    if ((c & 0b11111000) == 0b11110000) {
        if (bufLen < 4 || (c & 0b111) == 0 || (buf[1] & 0b111111) == 0 || (buf[2] & 0b111111) == 0 ||
            (buf[3] & 0b111111) == 0)
            return 0;
        ch = ((((((c & 0b111) << 6) | (buf[1] & 0b111111)) << 6) | (buf[2] & 0b111111)) << 6) |
               (buf[3] & 0b111111);
        return 4;
    }
    if ((c & 0b11110000) == 0b11100000) {
        if (bufLen < 3 || (c & 0b1111) == 0 || (buf[1] & 0b111111) == 0 || (buf[2] & 0b111111) == 0)
            return 0;
        ch = ((((c & 0b1111) << 6) | (buf[1] & 0b111111)) << 6) | (buf[2] & 0b111111);
        return 3;
    }
    if ((c & 0b11100000) == 0b11000000) {
        if (bufLen < 2 || (c & 0b11111) == 0 || (buf[1] & 0b111111) == 0)
            return 0;
        ch = ((c & 0b11111) << 6) | (buf[1] & 0b111111);
        return 2;
    }
    ch = c;
    return 1;
}

size_t StrUtil::writeUTF8Char(unsigned char* buf, int ch) {
    if (ch > 0b111111111111111111111111111) {
        buf[0] = (unsigned char) (0b11111100 | ((ch >> 24) & 0b1));
        buf[1] = (unsigned char) (0b10000000 | ((ch >> 18) & 0b111111));
        buf[2] = (unsigned char) (0b10000000 | ((ch >> 12) & 0b111111));
        buf[3] = (unsigned char) (0b10000000 | ((ch >> 6) & 0b111111));
        buf[4] = (unsigned char) (0b10000000 | (ch & 0b111111));
        return 6;
    } else if (ch > 0b1111111111111111111111) {
        buf[0] = (unsigned char) (0b11111000 | ((ch >> 24) & 0b11));
        buf[1] = (unsigned char) (0b10000000 | ((ch >> 18) & 0b111111));
        buf[2] = (unsigned char) (0b10000000 | ((ch >> 12) & 0b111111));
        buf[3] = (unsigned char) (0b10000000 | ((ch >> 6) & 0b111111));
        buf[4] = (unsigned char) (0b10000000 | (ch & 0b111111));
        return 5;
    } else if (ch > 0b11111111111111111) {
        buf[0] = (unsigned char) (0b11110000 | ((ch >> 18) & 0b111));
        buf[1] = (unsigned char) (0b10000000 | ((ch >> 12) & 0b111111));
        buf[2] = (unsigned char) (0b10000000 | ((ch >> 6) & 0b111111));
        buf[3] = (unsigned char) (0b10000000 | (ch & 0b111111));
        return 4;
    } else if (ch > 0b111111111111) {
        buf[0] = (unsigned char) (0b11100000 | ((ch >> 12) & 0b1111));
        buf[1] = (unsigned char) (0b10000000 | ((ch >> 6) & 0b111111));
        buf[2] = (unsigned char) (0b10000000 | (ch & 0b111111));
        return 3;
    } else if (ch > 0b1111111) {
        buf[0] = (unsigned char) (0b11000000 | ((ch >> 6) & 0b11111));
        buf[1] = (unsigned char) (0b10000000 | (ch & 0b111111));
        return 2;
    }
    buf[0] = (unsigned char) (ch & 0b1111111);
    return 1;
}

StrUtil::utf8_iterator& StrUtil::utf8_iterator::operator++() {
    off = std::min(off + valueSize(), str.size());
    return *this;
}

StrUtil::utf8_iterator_wrapper StrUtil::iterateUTF8(std::string const& str) {
    return utf8_iterator_wrapper(str);
}

void StrUtil::appendUTF8Char(std::string& str, int ch) {
    unsigned char buf[6];
    size_t size = writeUTF8Char(buf, ch);
    str.append((const char*) buf, size);
}

bool StrUtil::equalsIgnoreCase(const std::string& str, const std::string& str2) {
    auto itw1 = iterateUTF8(str), itw2 = iterateUTF8(str2);
    auto it1 = itw1.begin(), it2 = itw2.begin();
    while (true) {
        if (it1 == itw1.end() || it2 == itw2.end()) {
            return (it1 != itw1.end() || it2 != itw2.end());
        }
        if (*it1 != *it2)
            return false;
        ++it1, ++it2;
    }
}