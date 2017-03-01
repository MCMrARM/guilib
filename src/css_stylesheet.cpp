#include <guilib/css_stylesheet.h>
#include <guilib/css_tokenizer.h>
#include <iostream>

using namespace guilib::css;

Stylesheet::Stylesheet(std::istream& stream) {
    Tokenizer tokenizer (stream);
    Token token (TokenType::INVALID);
    std::string indent;
    while (!(token = tokenizer.next()).isEOF()) {
        if (token.getType() == TokenType::CURELY_BRACKET_CLOSE || token.getType() == TokenType::BRACKET_CLOSE)
            indent.pop_back();
        std::cout << indent << token.toDebugString() << "\n";
        if (token.getType() == TokenType::CURELY_BRACKET_OPEN || token.getType() == TokenType::BRACKET_OPEN)
            indent.push_back(' ');
    }
}