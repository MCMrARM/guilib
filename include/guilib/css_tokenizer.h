#pragma once

#include <istream>

namespace guilib {
namespace css {

class Tokenizer;

enum TokenType {
    INVALID, IDENT, FUNCTION, AT_KEYWORD, HASH, STRING, BAD_STRING, URL, BAD_URL, DELIM, NUMBER, PERCENTAGE, DIMENSION,
    UNICODE_RANGE, INCLUDE_MATCH, DASH_MATCH, PREFIX_MATCH, SUFFIX_MATCH, SUBSTRING_MATCH, COLUMN, WHITESPACE, CDO, CDC,
    COLON, SEMICOLON, COMMA, SQUARE_BRACKET_OPEN, SQUARE_BRACKET_CLOSE, BRACKET_OPEN, BRACKET_CLOSE,
    CURELY_BRACKET_OPEN, CURELY_BRACKET_CLOSE, END_OF_FILE
};

struct HashToken {
    bool isId;
    std::string name;
};
struct StringToken {
    std::string value;
};
struct NumberToken {
    int integer;
    double number;
    bool useFloat = false;
};
struct DimensionToken {
    NumberToken number;
    std::string unit;
};
struct DelimToken {
    int codePoint;
};
struct UnicodeRangeToken {
    int start, end;
};

class Token {

private:

    friend class Tokenizer;

    TokenType type;
    HashToken hash;
    StringToken string;
    NumberToken number;
    DimensionToken dimension;
    DelimToken delim;
    UnicodeRangeToken unicodeRange;

public:

    Token(TokenType type) : type(type) { }

    operator bool() { return type != TokenType::INVALID; }

    HashToken const& asHash() const { return hash; }
    StringToken const& asString() const { return string; }
    NumberToken const& asNumber() const { return number; }
    NumberToken const& asPercentage() const { return number; }
    DimensionToken const& asDimension() const { return dimension; }
    StringToken const& asIdentifier() const { return string; }
    StringToken const& asFunction() const { return string; }
    StringToken const& asUrl() const { return string; }
    DelimToken const& asDelim() const { return delim; }
    UnicodeRangeToken const& asUnicodeRange() const { return unicodeRange; }


};

class Tokenizer {

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
    NumberToken consumeNumber();
    void consumeWhitespace();
    void consumeComment();

    Token consumeStringToken(int endChar);
    Token consumeStringToken() { return consumeStringToken(consume()); }
    Token consumeHashToken();
    Token consumeNumericToken();
    Token consumeIdentLikeToken();
    Token consumeUrlToken();
    void consumeBadUrlRemnants();
    Token consumeUnicodeRangeToken();

public:

    Tokenizer(std::istream& stream);

    Token next();

};

}
}