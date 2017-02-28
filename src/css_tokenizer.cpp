#include <guilib/css_tokenizer.h>

#include <cmath>
#include <guilib/string_util.h>

using namespace guilib::css;

CssTokenizer::CssTokenizer(std::istream& stream) : stream(stream) {
    for (int i = 0; i < 3; i++)
        nextChars[i] = stream.get();
}

int CssTokenizer::consume() {
    int ret = nextChars[0];
    for (int i = 0; i < 2; i++)
        nextChars[i] = nextChars[i + 1];
    nextChars[2] = stream.get();
    return ret;
}

void CssTokenizer::consume(int n) {
    while (n--)
        consume();
}

int CssTokenizer::peek(int i) {
    return nextChars[i];
}

void CssTokenizer::reconsume(int c) {
    stream.putback((char) nextChars[2]);
    for (int i = 2; i >= 1; i--)
        nextChars[i] = nextChars[i - 1];
    nextChars[0] = c;
}

bool CssTokenizer::willStartNumber(int i, int j, int k) {
    if (i == '+' || i == '-')
        return isDigit(j) || (j == '.' && isDigit(k));
    if (i == '.' && isDigit(j))
        return true;
    return isDigit(i);
}

bool CssTokenizer::willStartIdentifier(int i, int j, int k) {
    if (i == '-')
        return (isNameCodePoint(j) || isValidEscape(j, k));
    if (isNameCodePoint(i))
        return true;
    return isValidEscape(i, j);
}

int CssTokenizer::getHexDigit(int ch) {
    if (ch >= '0' && ch <= '9')
        return ch - '0';
    if (ch >= 'a' && ch <= 'f')
        return ch - 'a' + 10;
    if (ch >= 'A' && ch <= 'F')
        return ch - 'A' + 10;
    return -1;
}

int CssTokenizer::consumeHexDigit() {
    return getHexDigit(consume());
}

void CssTokenizer::consumeWhitespace() {
    while (isWhitespace(peek(0)))
        consume();
}

void CssTokenizer::consumeComment() {
    int c;
    while (!isEOF(c = consume())) {
        if (c == '*' && peek(0) == '/') {
            consume();
            break;
        }
    }
}

std::string CssTokenizer::consumeName() {
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

int CssTokenizer::consumeEscapedCodePoint() {
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

CssNumberToken CssTokenizer::consumeNumber() {
    CssNumberToken ret;
    bool neg = false;
    int c = consume();
    ret.integer = 0;
    if (c == '+' || c == '-') {
        neg = (c == '-');
        c = consume();
    }
    while (isDigit(c)) {
        ret.integer = ret.integer * 10 + c;
        c = consume();
    }
    ret.number = ret.integer;
    if (peek(0) == '.' && isDigit(peek(1))) {
        ret.useFloat = true;
        double frac = 0;
        while (isDigit(c)) {
            frac = frac / 10.0 + c / 10.0;
            c = consume();
        }
        ret.number += frac;
    }
    if ((peek(0) == 'E' || peek(0) == 'e') && isDigit(peek(1))) {
        ret.useFloat = true;
        int e = 0;
        while (isDigit(c)) {
            e = e * 10 + c;
            c = consume();
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

CssToken CssTokenizer::consumeStringToken(int endChar) {
    CssToken ret (CssTokenType::STRING);

    std::string str;
    int c;
    while (true) {
        c = consume();
        if (isEOF(c))
            break;
        if (c == '\n') {
            ret.type = CssTokenType::BAD_STRING;
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

CssToken CssTokenizer::consumeHashToken() {
    if (isNameCodePoint(peek(0)) || isValidEscape(peek(0), peek(1))) {
        CssToken ret (CssTokenType::HASH);
        ret.hash.isId = willStartIdentifier();
        ret.hash.name = consumeName();
        return ret;
    }
    return CssToken(CssTokenType::INVALID);
}

CssToken CssTokenizer::consumeNumericToken() {
    CssNumberToken num = consumeNumber();
    if (willStartIdentifier()) {
        CssToken ret (CssTokenType::DIMENSION);
        ret.dimension.number = num;
        ret.dimension.unit = consumeName();
        return ret;
    } else {
        CssToken ret (CssTokenType::NUMBER);
        ret.number = num;
        if (peek(0) == '%') {
            consume();
            ret.type = CssTokenType::PERCENTAGE;
        }
        return ret;
    }
}

CssToken CssTokenizer::consumeIdentLikeToken() {
    std::string name = consumeName();
    if (peek(0) == '(') {
        if (StrUtil::equalsIgnoreCase(name, "url"))
            return consumeUrlToken();

        CssToken ret (CssTokenType::FUNCTION);
        ret.string.value = name;
        return ret;
    }
    CssToken ret (CssTokenType::IDENT);
    ret.string.value = name;
    return ret;
}

CssToken CssTokenizer::consumeUrlToken() {
    consumeWhitespace();
    if (isEOF(peek(0)))
        return CssToken(CssTokenType::URL);
    if (peek(0) == '\'' || peek(0) == '"') {
        CssToken token = consumeStringToken();
        if (token.type != CssTokenType::BAD_STRING) {
            token.type = CssTokenType::URL;
            consumeWhitespace();
            if (peek(0) == ')' || isEOF(peek(0))) {
                consume();
                return token;
            }
        }
        token.type = CssTokenType::BAD_URL;
        consumeBadUrlRemnants();
        return token;
    } else {
        CssToken token (CssTokenType::URL);
        std::string str;
        int c;
        while (!isEOF(c = getchar())) {
            if (isWhitespace(c)) {
                consumeWhitespace();
                if (peek(0) == ')' || isEOF(peek(0))) {
                    consume();
                } else {
                    consumeBadUrlRemnants();
                    token.type = CssTokenType::BAD_URL;
                }
                break;
            }
            if (c == '"' || c == '\'' || c == '(' || isNonPrintable(c)) {
                consumeBadUrlRemnants();
                token.type = CssTokenType::BAD_URL;
                break;
            }
            if (c == '\\') {
                if (isValidEscape(c, peek(0))) {
                    StrUtil::appendUTF8Char(str, consumeEscapedCodePoint());
                    continue;
                }
                consumeBadUrlRemnants();
                token.type = CssTokenType::BAD_URL;
                break;
            }
            StrUtil::appendUTF8Char(str, c);
        }
        token.string.value = str;
        return token;
    }
}

void CssTokenizer::consumeBadUrlRemnants() {
    int c;
    while (!isEOF(c = getchar())) {
        if (isValidEscape(c, peek(0)))
            consumeEscapedCodePoint();
    }
}

CssToken CssTokenizer::consumeUnicodeRangeToken() {
    CssToken ret (CssTokenType::UNICODE_RANGE);
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

CssToken CssTokenizer::next() {
    int c = consume();
    if (isEOF(c))
        return CssToken(CssTokenType::END_OF_FILE);
    if (isWhitespace(c)) { // whitespace
        consumeWhitespace();
        return CssToken(CssTokenType::WHITESPACE);
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
            CssToken token = consumeHashToken();
            if (token)
                return token;
            break;
        }
        case '$':
            if (peek(0) == '=') {
                consume();
                return CssToken(CssTokenType::SUFFIX_MATCH);
            }
            break;
        case '(':
            return CssToken(CssTokenType::BRACKET_OPEN);
        case ')':
            return CssToken(CssTokenType::BRACKET_CLOSE);
        case '*':
            if (peek(0) == '*') {
                consume();
                return CssToken(CssTokenType::SUBSTRING_MATCH);
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
            return CssToken(CssTokenType::COMMA);
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
                return CssToken(CssTokenType::CDC);
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
            return CssToken(CssTokenType::COLON);
        case ';':
            return CssToken(CssTokenType::SEMICOLON);
        case '<':
            if (peek(0) == '!' && peek(1) == '-' && peek(2) == '-') {
                consume(3);
                return CssToken(CssTokenType::CDO);
            }
            break;
        case '@':
            if (willStartIdentifier()) {
                CssToken token (CssTokenType::AT_KEYWORD);
                token.string.value = consumeName();
                return token;
            }
            break;
        case '[':
            return CssToken(CssTokenType::SQUARE_BRACKET_OPEN);
        case '\\':
            if (isValidEscape(c, peek(0))) {
                reconsume(c);
                return consumeIdentLikeToken();
            }
            break;
        case ']':
            return CssToken(CssTokenType::SQUARE_BRACKET_CLOSE);
        case '^':
            if (peek(0) == '=') {
                consume();
                return CssToken(CssTokenType::PREFIX_MATCH);
            }
            break;
        case '{':
            return CssToken(CssTokenType::CURELY_BRACKET_OPEN);
        case '}':
            return CssToken(CssTokenType::CURELY_BRACKET_CLOSE);
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
                return CssToken(CssTokenType::DASH_MATCH);
            } else if (peek(0) == '|') {
                consume();
                return CssToken(CssTokenType::COLUMN);
            }
            break;
        case '~':
            if (peek(0) == '=') {
                consume();
                return CssToken(CssTokenType::INCLUDE_MATCH);
            }
            break;
        default:
            break;
    }
    {
        CssToken token (CssTokenType::DELIM);
        token.delim.codePoint = c;
        return token;
    }
}