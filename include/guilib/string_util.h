#pragma once

#include <string>

namespace guilib {

class StrUtil {

public:

    struct utf8_iterator {

    protected:

        const std::string& str;
        size_t off;

    public:

        utf8_iterator(const std::string& str, size_t off) : str(str), off(off) { }

        int value() const;
        size_t valueSize() const;

        size_t offset() const { return off; }
        int operator*() const { return value(); }

        bool operator==(utf8_iterator const &i) const { return (&i.str == &str && i.off == off); }
        bool operator!=(utf8_iterator const &i) const { return !(*this == i); }

        utf8_iterator& operator++();

    };

    struct utf8_iterator_wrapper {

    protected:

        std::string const& str;

    public:

        utf8_iterator_wrapper(std::string const& str) : str(str) { }

        utf8_iterator begin() const { return utf8_iterator(str, 0); }
        utf8_iterator end() const { return utf8_iterator(str, str.size()); }

    };

    /**
     * Converts the specified number to a string.
     * @param i the number to convert
     * @return a string which represents the specified number
     */
    static std::string toString(int i) { return std::to_string(i); }

    /**
     * Converts the specified number to a string.
     * @param i the number to convert
     * @return a string which represents the specified number
     */
    static std::string toString(double i) { return std::to_string(i); }

    /**
     * Returns a wrapper object that provides function that can be used to iterate over the string using the UTF8
     * encoding.
     * @param str the string to iterate over
     * @return a iterator wrapper object
     */
    static utf8_iterator_wrapper iterateUTF8(std::string const& str);

    /**
     * Get the UTF8 code point size (used bytes). Useful if you want to know how many bytes do you need to read.
     * If the byte does not start a valid UTF8 character, this function will return 0.
     * @param firstChar the first byte of the character
     * @return the byte size of the character (or 0 in case of an error)
     */
    static size_t getUTF8ReadCodePointSize(unsigned char firstChar);

    /**
     * Read a UTF8 character from a buffer. If the buffer size is zero then this function will return zero. If a error
     * occurs the function will return 0 and the character number will be set to -1.
     * @param buf the buffer
     * @param bufLen length of the buffer
     * @param ch a reference to a variable that will hold the resulting character
     * @return number of bytes processed by this function
     */
    static size_t readUTF8Char(unsigned char const* buf, size_t bufLen, int& ch);

    /**
     * Write an UTF8 character to a buffer. The buffer size is required to be at least 6 bytes.
     * @param buf the buffer
     * @param ch the character
     * @return the count of bytes used
     */
    static size_t writeUTF8Char(unsigned char buf[6], int ch);

    /**
     * Append a UTF8 character to a string.
     * @param str the output string
     * @param ch the character to append
     */
    static void appendUTF8Char(std::string& str, int ch);

    /**
     * Checks two strings for equality. This function will not work properly with UTF8 strings.
     * @param str the first string
     * @param str2 the second string
     * @return are the two strings equal?
     */
    static bool equalsIgnoreCase(std::string const& str, std::string const& str2);

};

}