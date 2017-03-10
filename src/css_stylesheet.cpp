#include <guilib/css_stylesheet.h>

#include <guilib/css_tokenizer.h>
#include <guilib/css_parser.h>
#include <iostream>

using namespace guilib::css;

Stylesheet::Stylesheet(std::istream& stream) {
    Tokenizer tokenizer (stream);
    Parser parser;
    parser.parse(tokenizer);
}