#include <guilib/css_tokenizer.h>

#include <cmath>
#include <sstream>
#include <guilib/string_util.h>

using namespace guilib::css;

std::string guilib::css::TokenTypeToString(TokenType type) {
    switch (type) {
        case IDENT: return "IDENT";
        case FUNCTION: return "FUNCTION";
        case AT_KEYWORD: return "AT_KEYWORD";
        case HASH: return "HASH";
        case STRING: return "STRING";
        case BAD_STRING: return "BAD_STRING";
        case URL: return "URL";
        case BAD_URL: return "BAD_URL";
        case DELIM: return "DELIM";
        case NUMBER: return "NUMBER";
        case PERCENTAGE: return "PERCENTAGE";
        case DIMENSION: return "DIMENSION,";
        case UNICODE_RANGE: return "UNICODE_RANGE";
        case INCLUDE_MATCH: return "INCLUDE_MATCH";
        case DASH_MATCH: return "DASH_MATCH";
        case PREFIX_MATCH: return "PREFIX_MATCH";
        case SUFFIX_MATCH: return "SUFFIX_MATCH";
        case SUBSTRING_MATCH: return "SUBSTRING_MATCH";
        case COLUMN: return "COLUMN";
        case WHITESPACE: return "WHITESPACE";
        case CDO: return "CDO";
        case CDC: return "CDC,";
        case COLON: return "COLON";
        case SEMICOLON: return "SEMICOLON";
        case COMMA: return "COMMA";
        case SQUARE_BRACKET_OPEN: return "SQUARE_BRACKET_OPEN";
        case SQUARE_BRACKET_CLOSE: return "SQUARE_BRACKET_CLOSE";
        case BRACKET_OPEN: return "BRACKET_OPEN";
        case BRACKET_CLOSE: return "BRACKET_CLOSE,";
        case CURELY_BRACKET_OPEN: return "CURELY_BRACKET_OPEN";
        case CURELY_BRACKET_CLOSE: return "CURELY_BRACKET_CLOSE";
        case END_OF_FILE: return "END_OF_FILE";
        default: return "INVALID";
    }
}

std::string Token::toDebugString() const {
    std::stringstream ss;
    ss << TokenTypeToString(type);
    if (isHash())
        ss << " name: " << asHash().name << ", isIt: " << asHash().isId;
    if (isString() || isIdentifier())
        ss << " value: " << asString().value;
    if (isNumber() || isPercentage())
        ss << " value: " << asNumber().number;
    if (isDimension())
        ss << " value: " << asDimension().number.number << ", unit: " << asDimension().unit;

    return ss.str();
}

Tokenizer::Tokenizer(std::istream& stream) : stream(stream) {
    for (int i = 0; i < 3; i++)
        nextChars[i] = stream.get();
}

int Tokenizer::consume() {
    int ret = nextChars[0];
    for (int i = 0; i < 2; i++)
        nextChars[i] = nextChars[i + 1];
    nextChars[2] = stream.get();
    return ret;
}

void Tokenizer::consume(int n) {
    while (n--)
        consume();
}

int Tokenizer::peek(int i) {
    return nextChars[i];
}

void Tokenizer::reconsume(int c) {
    stream.putback((char) nextChars[2]);
    for (int i = 2; i >= 1; i--)
        nextChars[i] = nextChars[i - 1];
    nextChars[0] = c;
}

bool Tokenizer::willStartNumber(int i, int j, int k) {
    if (i == '+' || i == '-')
        return isDigit(j) || (j == '.' && isDigit(k));
    if (i == '.' && isDigit(j))
        return true;
    return isDigit(i);
}

bool Tokenizer::willStartIdentifier(int i, int j, int k) {
    if (i == '-')
        return (isNameCodePoint(j) || isValidEscape(j, k));
    if (isNameCodePoint(i))
        return true;
    return isValidEscape(i, j);
}

int Tokenizer::getHexDigit(int ch) {
    if (ch >= '0' && ch <= '9')
        return ch - '0';
    if (ch >= 'a' && ch <= 'f')
        return ch - 'a' + 10;
    if (ch >= 'A' && ch <= 'F')
        return ch - 'A' + 10;
    return -1;
}

int Tokenizer::consumeHexDigit() {
    return getHexDigit(consume());
}

void Tokenizer::consumeWhitespace() {
    while (isWhitespace(peek(0)))
        consume();
}

void Tokenizer::consumeComment() {
    int c;
    while (!isEOF(c = consume())) {
        if (c == '*' && peek(0) == '/') {
            consume();
            break;
        }
    }
}

std::string Tokenizer::consumeName() {
    std::string str;
    int c;
    while (true) {
        c = consume();
        if (isNameCodePoint(c))
            StrUtil::appendUTF8Char(str, c);
        else if (isValidEscape(c, peek(1)))
            StrUtil::appendUTF8Char(str, consumeEscapedCodePoint());
        else
            break;
    }
    return str;
}

int Tokenizer::consumeEscapedCodePoint() {
    int c = consume();
    if (isHexDigit(c)) {
        int num = getHexDigit(c);
        int i = 1;
        while (isHexDigit(peek(0)) && i++ <= 6)
            num = num * 16 + consumeHexDigit();
        if (isWhitespace(peek(0)))
            consume();
        if (num == 0 || isSurrogate(num) || num >= 0x10FFFF)
            return 0xFFFD;
        return num;
    } else if (isEOF(c)) {
        return 0xFFFD;
    } else {
        return c;
    }
}

NumberToken Tokenizer::consumeNumber() {
    NumberToken ret;
    bool neg = false;
    int c = peek(0);
    ret.integer = 0;
    if (c == '+' || c == '-') {
        neg = (c == '-');
        consume();
    }
    while (isDigit(peek(0))) {
        c = consume();
        ret.integer = ret.integer * 10 + (c - '0');
    }
    ret.number = ret.integer;
    if (peek(0) == '.' && isDigit(peek(1))) {
        ret.useFloat = true;
        double frac = 0;
        while (isDigit(peek(0))) {
            c = consume();
            frac = frac / 10.0 + (c - '0') / 10.0;
        }
        ret.number += frac;
    }
    if ((peek(0) == 'E' || peek(0) == 'e') && isDigit(peek(1))) {
        ret.useFloat = true;
        int e = 0;
        while (isDigit(peek(0))) {
            c = consume();
            e = e * 10 + (c - '0');
        }
        ret.number *= std::pow(10.0, e);
        ret.integer = (int) ret.number;
    }
    if (neg) {
        ret.number *= -1.0;
        ret.integer *= -1;
    }
    return ret;
}

Token Tokenizer::consumeStringToken(int endChar) {
    Token ret (TokenType::STRING);

    std::string str;
    int c;
    while (true) {
        c = consume();
        if (isEOF(c))
            break;
        if (c == '\n') {
            ret.type = TokenType::BAD_STRING;
            reconsume(c);
            break;
        }
        if (c == '\\') {
            c = peek(0);
            if (isEOF(c))
                break;
            if (c == '\n') {
                consume();
                continue;
            }
            if (isValidEscape(c, peek(1))) {
                StrUtil::appendUTF8Char(str, consumeEscapedCodePoint());
                continue;
            }
        }
        if (c == endChar)
            break;
        StrUtil::appendUTF8Char(str, c);
    }
    return ret;
}

Token Tokenizer::consumeHashToken() {
    if (isNameCodePoint(peek(0)) || isValidEscape(peek(0), peek(1))) {
        Token ret (TokenType::HASH);
        ret.hash.isId = willStartIdentifier();
        ret.hash.name = consumeName();
        return ret;
    }
    return Token(TokenType::INVALID);
}

Token Tokenizer::consumeNumericToken() {
    NumberToken num = consumeNumber();
    if (willStartIdentifier()) {
        Token ret (TokenType::DIMENSION);
        ret.dimension.number = num;
        ret.dimension.unit = consumeName();
        return ret;
    } else {
        Token ret (TokenType::NUMBER);
        ret.number = num;
        if (peek(0) == '%') {
            consume();
            ret.type = TokenType::PERCENTAGE;
        }
        return ret;
    }
}

Token Tokenizer::consumeIdentLikeToken() {
    std::string name = consumeName();
    if (peek(0) == '(') {
        if (StrUtil::equalsIgnoreCase(name, "url"))
            return consumeUrlToken();

        Token ret (TokenType::FUNCTION);
        ret.string.value = name;
        return ret;
    }
    Token ret (TokenType::IDENT);
    ret.string.value = name;
    return ret;
}

Token Tokenizer::consumeUrlToken() {
    consumeWhitespace();
    if (isEOF(peek(0)))
        return Token(TokenType::URL);
    if (peek(0) == '\'' || peek(0) == '"') {
        Token token = consumeStringToken();
        if (token.type != TokenType::BAD_STRING) {
            token.type = TokenType::URL;
            consumeWhitespace();
            if (peek(0) == ')' || isEOF(peek(0))) {
                consume();
                return token;
            }
        }
        token.type = TokenType::BAD_URL;
        consumeBadUrlRemnants();
        return token;
    } else {
        Token token (TokenType::URL);
        std::string str;
        int c;
        while (!isEOF(c = getchar())) {
            if (isWhitespace(c)) {
                consumeWhitespace();
                if (peek(0) == ')' || isEOF(peek(0))) {
                    consume();
                } else {
                    consumeBadUrlRemnants();
                    token.type = TokenType::BAD_URL;
                }
                break;
            }
            if (c == '"' || c == '\'' || c == '(' || isNonPrintable(c)) {
                consumeBadUrlRemnants();
                token.type = TokenType::BAD_URL;
                break;
            }
            if (c == '\\') {
                if (isValidEscape(c, peek(0))) {
                    StrUtil::appendUTF8Char(str, consumeEscapedCodePoint());
                    continue;
                }
                consumeBadUrlRemnants();
                token.type = TokenType::BAD_URL;
                break;
            }
            StrUtil::appendUTF8Char(str, c);
        }
        token.string.value = str;
        return token;
    }
}

void Tokenizer::consumeBadUrlRemnants() {
    int c;
    while (!isEOF(c = getchar())) {
        if (isValidEscape(c, peek(0)))
            consumeEscapedCodePoint();
    }
}

Token Tokenizer::consumeUnicodeRangeToken() {
    Token ret (TokenType::UNICODE_RANGE);
    int num = 0;
    int q = 1;
    int i = 0;
    while (isHexDigit(peek(0)) && i++ <= 6)
        num = num * 16 + consumeHexDigit();
    while (peek(0) == '?' && i++ <= 6)
        q *= 16;
    ret.unicodeRange.start = num * q;
    if (q > 1) {
        ret.unicodeRange.end = num * q + q - 1;
        return ret;
    }
    if (peek(0) == '-' && isHexDigit(peek(1))) {
        consume();
        num = 0, i = 0;
        while (isHexDigit(peek(0)) && i++ <= 6)
            num = num * 16 + consumeHexDigit();
        ret.unicodeRange.end = num;
    } else {
        ret.unicodeRange.end = ret.unicodeRange.start;
    }
    return ret;
}

Token Tokenizer::next() {
    int c = consume();
    if (isEOF(c))
        return Token(TokenType::END_OF_FILE);
    if (isWhitespace(c)) { // whitespace
        consumeWhitespace();
        return Token(TokenType::WHITESPACE);
    }
    if (isDigit(c)) {
        reconsume(c);
        return consumeNumericToken();
    }
    if (isNameCodePoint(c)) {
        reconsume(c);
        return consumeIdentLikeToken();
    }
    switch (c) {
        case '"':
        case '\'':
            return consumeStringToken(c);
        case '#': {
            Token token = consumeHashToken();
            if (token)
                return token;
            break;
        }
        case '$':
            if (peek(0) == '=') {
                consume();
                return Token(TokenType::SUFFIX_MATCH);
            }
            break;
        case '(':
            return Token(TokenType::BRACKET_OPEN);
        case ')':
            return Token(TokenType::BRACKET_CLOSE);
        case '*':
            if (peek(0) == '*') {
                consume();
                return Token(TokenType::SUBSTRING_MATCH);
            }
            break;
        case '+':
        case '.':
            if (willStartNumber()) {
                reconsume(c);
                return consumeNumericToken();
            }
            break;
        case ',':
            return Token(TokenType::COMMA);
        case '-':
            if (willStartNumber()) {
                reconsume(c);
                return consumeNumericToken();
            }
            if (willStartIdentifier()) {
                reconsume(c);
                return consumeIdentLikeToken();
            }
            if (peek(0) == '-' && peek(1) == '>') {
                consume(2);
                return Token(TokenType::CDC);
            }
            break;
        case '/':
            if (peek(0) == '*') {
                consume();
                consumeComment();
                return next();
            }
            break;
        case ':':
            return Token(TokenType::COLON);
        case ';':
            return Token(TokenType::SEMICOLON);
        case '<':
            if (peek(0) == '!' && peek(1) == '-' && peek(2) == '-') {
                consume(3);
                return Token(TokenType::CDO);
            }
            break;
        case '@':
            if (willStartIdentifier()) {
                Token token (TokenType::AT_KEYWORD);
                token.string.value = consumeName();
                return token;
            }
            break;
        case '[':
            return Token(TokenType::SQUARE_BRACKET_OPEN);
        case '\\':
            if (isValidEscape(c, peek(0))) {
                reconsume(c);
                return consumeIdentLikeToken();
            }
            break;
        case ']':
            return Token(TokenType::SQUARE_BRACKET_CLOSE);
        case '^':
            if (peek(0) == '=') {
                consume();
                return Token(TokenType::PREFIX_MATCH);
            }
            break;
        case '{':
            return Token(TokenType::CURELY_BRACKET_OPEN);
        case '}':
            return Token(TokenType::CURELY_BRACKET_CLOSE);
        case 'U':
        case 'u':
            if (peek(0) == '+' && (isHexDigit(peek(1)) || peek(1) == 0x3F)) {
                consume();
                return consumeUnicodeRangeToken();
            }
            reconsume(c);
            return consumeIdentLikeToken();
        case '|':
            if (peek(0) == '=') {
                consume();
                return Token(TokenType::DASH_MATCH);
            } else if (peek(0) == '|') {
                consume();
                return Token(TokenType::COLUMN);
            }
            break;
        case '~':
            if (peek(0) == '=') {
                consume();
                return Token(TokenType::INCLUDE_MATCH);
            }
            break;
        default:
            break;
    }
    {
        Token token (TokenType::DELIM);
        token.delim.codePoint = c;
        return token;
    }
}