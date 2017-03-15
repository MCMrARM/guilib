#include <guilib/css_parser.h>

#include <guilib/css_tokenizer.h>

using namespace guilib::css;

TokenReader::TokenReader(Tokenizer& tokenizer) : tokenizer(tokenizer) {
    //
}

Token TokenReader::next() {
    if (currentBlockScopeEof)
        return Token(TokenType::END_OF_FILE);
    Token ret = tokenizer.next();
    if (ret.getType() == TokenType::CURLY_BRACKET_CLOSE && currentBlockScope > 0) {
        if (blocksWereProperlyOpened.top()) {
            currentBlockScopeEof = true;
            return Token(TokenType::END_OF_FILE);
        } else if (currentBlockScope > 0) {
            currentBlockScope--;
            blocksWereProperlyOpened.pop();
        }
    }
    if (ret.getType() == TokenType::CURLY_BRACKET_OPEN) {
        currentBlockScope++;
        blocksWereProperlyOpened.push(false);
    }
    return ret;
}

void TokenReader::enterBasicBlock() {
    blocksWereProperlyOpened.top() = true;
}

void TokenReader::exitBasicBlock() {
    if (!currentBlockScopeEof || currentBlockScope <= 0)
        throw std::logic_error("Invalid basic block exit");
    currentBlockScopeEof = false;
    currentBlockScope--;
    blocksWereProperlyOpened.pop();
}

template <typename OutputParam>
void RuleListParser<OutputParam>::clear() {
    qualifiedRules.clear();
    atRules.clear();
}

template <typename OutputParam>
void RuleListParser<OutputParam>::addQualifiedRuleHandler(RuleCondFunc cond, RuleHandleFunc handle) {
    qualifiedRules.push_back({cond, handle});
}

template <typename OutputParam>
void RuleListParser<OutputParam>::addAtRuleHandler(RuleCondFunc cond, RuleHandleFunc handle) {
    atRules.push_back({cond, handle});
}

template <typename OutputParam>
void RuleListParser<OutputParam>::parseRule(std::vector<RuleHandleDef> const& handlers, TokenReader& source, OutputParam& output,
                                            TokenList const& key) {
    printf("Parse rule %s\n", key[0].toDebugString().c_str());
    source.enterBasicBlock();
    for (RuleHandleDef const& def : handlers) {
        if (def.cond(key)) {
            def.handle(key, source, output);
            source.exitBasicBlock();
            return;
        }
    }
    while (!source.next().isEOF());
    source.exitBasicBlock();
}

template <typename OutputParam>
void RuleListParser<OutputParam>::parseRule(TokenReader& source, OutputParam& output, TokenList const& key) {
    std::vector<RuleHandleDef>* handlers = &qualifiedRules;
    if (key[0].getType() == TokenType::AT_KEYWORD)
        handlers = &atRules;
    parseRule(*handlers, source, output, key);
}

template <typename OutputParam>
void RuleListParser<OutputParam>::parse(TokenReader& source, OutputParam& output, bool topLevel) {
    TokenList keyTokens;
    Token token (TokenType::INVALID);
    while (!(token = source.next()).isEOF()) {
        if (token.getType() == TokenType::WHITESPACE && keyTokens.size() == 0)
            continue;
        if (token.getType() == TokenType::CDO || token.getType() == TokenType::CDC) {
            if (topLevel)
                continue;
        }
        if (token.getType() == TokenType::CURLY_BRACKET_OPEN) {
            parseRule(source, output, keyTokens);
            keyTokens.clear();
            continue;
        }
        keyTokens.append(std::move(token));
    }
}

template <typename OutputParam>
void DeclarationListParser<OutputParam>::clear() {
    atRules.clear();
    declarations.clear();
}

template <typename OutputParam>
void DeclarationListParser<OutputParam>::addAtRuleHandler(typename RuleListParser<OutputParam>::RuleCondFunc cond,
                                                          typename RuleListParser<OutputParam>::RuleHandleFunc handle) {
    atRules.push_back({cond, handle});
}

template <typename OutputParam>
void DeclarationListParser<OutputParam>::addDeclaration(DeclarationCondFunc cond, DeclarationHandleFunc handle) {
    declarations.push_back({cond, handle});
}

template <typename OutputParam>
bool DeclarationListParser<OutputParam>::parseDeclaration(std::string const& key, TokenList& value, OutputParam& output) {
    printf("Parse declaration: %s\n", key.c_str());
    bool important = false;
    if (value[-2].isDelim('!') && value[-1].isIdent("important")) {
        value.pop();
        value.pop();
        important = true;
    }
    for (DeclarationHandleDef& def : declarations) {
        if (def.cond(key)) {
            def.handle(key, value, important, output);
            return true;
        }
    }
    return false;
}

template <typename OutputParam>
void DeclarationListParser<OutputParam>::parse(TokenReader& source, OutputParam& output) {
    Token token (TokenType::INVALID);
    while (!(token = source.next()).isEOF()) {
        if (token.getType() == TokenType::WHITESPACE || token.getType() == TokenType::SEMICOLON)
            continue;
        if (token.getType() == TokenType::AT_KEYWORD) {
            TokenList keyTokens;
            keyTokens.append(std::move(token));
            while (!(token = source.next()).isEOF()) {
                if (token.getType() == TokenType::CURLY_BRACKET_OPEN) {
                    RuleListParser<OutputParam>::parseRule(atRules, source, output, keyTokens);
                    break;
                }
                keyTokens.append(std::move(token));
            }
            continue;
        }
        if (token.getType() == TokenType::IDENT) {
            // consume a declaration
            std::string key = token.asIdent().value;
            token = source.next();
            if (token.getType() == TokenType::WHITESPACE)
                token = source.next();
            if (token.getType() != TokenType::COLON) // parse error
                continue;
            TokenList tokens;
            if ((token = source.next()).getType() != TokenType::WHITESPACE)
                tokens.append(std::move(token)); // trim first token
            while (!(token = source.next()).isEOF() && token.getType() != TokenType::SEMICOLON)
                tokens.append(std::move(token));
            if (tokens.size() > 0 && token.getType() == TokenType::WHITESPACE)
                tokens.pop(); // trim last token
            parseDeclaration(key, tokens, output);
            continue;
        }
        // parse error
        while (!(token = source.next()).isEOF()) {
            if (token.getType() == TokenType::SEMICOLON)
                break;
        }
    }
}

void Parser::parse(Tokenizer& tokenizer) {
    TokenReader reader (tokenizer);
    RuleListParser<void*> test;
    DeclarationListParser<void*> test2;
    test.addQualifiedRuleHandler([](TokenList const&) { return true; }, [&test2](TokenList const& key, TokenReader& source, void*& output) {
        printf("Parse qualified rule\n");
        test2.parse(source, output);
    });
    void* np;
    test.parse(reader, np);
}