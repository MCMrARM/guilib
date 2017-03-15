#pragma once

#include <istream>
#include <vector>

namespace guilib {
namespace css {

class Tokenizer;

enum TokenType {
    INVALID, IDENT, FUNCTION, AT_KEYWORD, HASH, STRING, BAD_STRING, URL, BAD_URL, DELIM, NUMBER, PERCENTAGE, DIMENSION,
    UNICODE_RANGE, INCLUDE_MATCH, DASH_MATCH, PREFIX_MATCH, SUFFIX_MATCH, SUBSTRING_MATCH, COLUMN, WHITESPACE, CDO, CDC,
    COLON, SEMICOLON, COMMA, SQUARE_BRACKET_OPEN, SQUARE_BRACKET_CLOSE, BRACKET_OPEN, BRACKET_CLOSE,
    CURLY_BRACKET_OPEN, CURLY_BRACKET_CLOSE, END_OF_FILE
};
std::string TokenTypeToString(TokenType type);

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

    std::string toString() const;
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

    operator bool() const { return type != TokenType::INVALID; }

    bool operator==(Token const& second);
    bool operator!=(Token const& second) { return !(*this == second); }

    TokenType getType() const { return type; }
    bool isEOF() const { return type == TokenType::END_OF_FILE; }
    bool isHash() const { return type == TokenType::HASH; }
    bool isString() const { return type == TokenType::STRING || type == TokenType::BAD_STRING; }
    bool isNumber() const { return type == TokenType::NUMBER; }
    bool isPercentage() const { return type == TokenType::PERCENTAGE; }
    bool isDimension() const { return type == TokenType::DIMENSION; }
    bool isIdent() const { return type == TokenType::IDENT; }
    bool isFunction() const { return type == TokenType::FUNCTION; }
    bool isUrl() const { return type == TokenType::URL || type == TokenType::BAD_URL; }
    bool isDelim() const { return type == TokenType::DELIM; }
    bool isUnicodeRange() const { return type == TokenType::UNICODE_RANGE; }

    bool isString(std::string const& value) const { return isString() && asString().value == value; }
    bool isIdent(std::string const& value) const { return isIdent() && asIdent().value == value; }
    bool isDelim(int codePoint) const { return isDelim() && asDelim().codePoint == codePoint; }

    HashToken const& asHash() const { return hash; }
    StringToken const& asString() const { return string; }
    NumberToken const& asNumber() const { return number; }
    NumberToken const& asPercentage() const { return number; }
    DimensionToken const& asDimension() const { return dimension; }
    StringToken const& asIdent() const { return string; }
    StringToken const& asFunction() const { return string; }
    StringToken const& asUrl() const { return string; }
    DelimToken const& asDelim() const { return delim; }
    UnicodeRangeToken const& asUnicodeRange() const { return unicodeRange; }

    std::string toString() const;
    std::string toDebugString() const;

    static Token INVALID;

};

class Tokenizer {

private:

    std::istream& stream;
    int nextChars[3];
    std::vector<Token> reconsumed;

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

    void reconsume(Token const& token);

};

}
}