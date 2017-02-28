#pragma once

#include <istream>

namespace guilib {
namespace css {

class CssTokenizer;

enum CssTokenType {
    INVALID, IDENT, FUNCTION, AT_KEYWORD, HASH, STRING, BAD_STRING, URL, BAD_URL, DELIM, NUMBER, PERCENTAGE, DIMENSION,
    UNICODE_RANGE, INCLUDE_MATCH, DASH_MATCH, PREFIX_MATCH, SUFFIX_MATCH, SUBSTRING_MATCH, COLUMN, WHITESPACE, CDO, CDC,
    COLON, SEMICOLON, COMMA, SQUARE_BRACKET_OPEN, SQUARE_BRACKET_CLOSE, BRACKET_OPEN, BRACKET_CLOSE,
    CURELY_BRACKET_OPEN, CURELY_BRACKET_CLOSE, END_OF_FILE
};

struct CssHashToken {
    bool isId;
    std::string name;
};
struct CssStringToken {
    std::string value;
};
struct CssNumberToken {
    int integer;
    double number;
    bool useFloat = false;
};
struct CssDimensionToken {
    CssNumberToken number;
    std::string unit;
};
struct CssDelimToken {
    int codePoint;
};
struct CssUnicodeRangeToken {
    int start, end;
};

class CssToken {

private:

    friend class CssTokenizer;

    CssTokenType type;
    CssHashToken hash;
    CssStringToken string;
    CssNumberToken number;
    CssDimensionToken dimension;
    CssDelimToken delim;
    CssUnicodeRangeToken unicodeRange;

public:

    CssToken(CssTokenType type) : type(type) { }

    operator bool() { return type != CssTokenType::INVALID; }

    CssHashToken const& asHash() const { return hash; }
    CssStringToken const& asString() const { return string; }
    CssNumberToken const& asNumber() const { return number; }
    CssNumberToken const& asPercentage() const { return number; }
    CssDimensionToken const& asDimension() const { return dimension; }
    CssStringToken const& asIdentifier() const { return string; }
    CssStringToken const& asFunction() const { return string; }
    CssStringToken const& asUrl() const { return string; }
    CssDelimToken const& asDelim() const { return delim; }
    CssUnicodeRangeToken const& asUnicodeRange() const { return unicodeRange; }


};

class CssTokenizer {

private:

    std::istream& stream;
    int nextChars[3];

    int consume();
    void consume(int n);
    int peek(int i);
    void reconsume(int c);

    bool isEOF(int c) { return (c < 0); }
    bool isWhitespace(int c) { return c == ' ' || c == '\t' || c == '\r' || c == '\n'; }
    bool isNonAscii(int c) { return c > 0x80; }
    bool isLetter(int c) { return (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z'); }
    bool isDigit(int c) { return c >= '0' && c <= '9'; }
    bool isHexDigit(int c) { return isDigit(c) || (c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F'); }
    bool isNameCodePoint(int c) { return isNonAscii(c) || isLetter(c) || c == '-'; }
    bool isSurrogate(int c) { return c >= 0xD800 && c <= 0xDFFF; }
    bool isNonPrintable(int c) { return (c >= 0 && c <= 8) || c == 0xB || (c >= 0xE && c <= 0x1F) || c == 0x7F; }
    bool isValidEscape(int i, int j) { return i == '\\' && j != '\n'; }
    bool willStartNumber(int i, int j, int k);
    bool willStartNumber() { return willStartNumber(peek(0), peek(1), peek(2)); }
    bool willStartIdentifier(int i, int j, int k);
    bool willStartIdentifier() { return willStartIdentifier(peek(0), peek(1), peek(2)); }

    int getHexDigit(int ch);

    int consumeHexDigit();
    std::string consumeName();
    int consumeEscapedCodePoint();
    CssNumberToken consumeNumber();
    void consumeWhitespace();
    void consumeComment();

    CssToken consumeStringToken(int endChar);
    CssToken consumeStringToken() { return consumeStringToken(consume()); }
    CssToken consumeHashToken();
    CssToken consumeNumericToken();
    CssToken consumeIdentLikeToken();
    CssToken consumeUrlToken();
    void consumeBadUrlRemnants();
    CssToken consumeUnicodeRangeToken();

public:

    CssTokenizer(std::istream& stream);

    CssToken next();

};

}
}