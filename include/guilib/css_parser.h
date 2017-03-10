#pragma once

#include <functional>
#include <vector>
#include <stack>

namespace guilib {
namespace css {

class Token;
class Tokenizer;
class TokenList;

class TokenReader {

private:

    Tokenizer& tokenizer;
    int currentBlockScope = 0;
    bool currentBlockScopeEof = false;
    std::stack<bool> blocksWereProperlyOpened;

public:

    TokenReader(Tokenizer& tokenizer);

    Token next();

    void enterBasicBlock();
    void exitBasicBlock();

};

template <typename OutputParam>
class RuleListParser {

public:

    typedef std::function<bool (TokenList const& key)> RuleCondFunc;
    typedef std::function<void (TokenList const& key, TokenReader& source, OutputParam& output)> RuleHandleFunc;

    struct RuleHandleDef {
        RuleCondFunc cond;
        RuleHandleFunc handle;
    };

private:
    std::vector<RuleHandleDef> qualifiedRules, atRules;

    bool parseRule(TokenReader& source, OutputParam& output, TokenList const& key);

public:

    void clear();
    void addQualifiedRuleHandler(RuleCondFunc cond, RuleHandleFunc handle);
    void addAtRuleHandler(RuleCondFunc cond, RuleHandleFunc handle);

    void parse(TokenReader& source, OutputParam& output, bool topLevel = false);

};

template <typename OutputParam>
class DeclarationListParser {

public:

    typedef std::function<bool (std::string const& key)> DeclarationCondFunc;
    typedef std::function<void (std::string const& key, TokenList const& value, bool important, OutputParam& output)> DeclarationHandleFunc;

    struct DeclarationHandleDef {
        DeclarationCondFunc cond;
        DeclarationHandleFunc handle;
    };

private:

    std::vector<typename RuleListParser<OutputParam>::RuleHandleDef> atRules;
    std::vector<DeclarationHandleDef> declarations;

    bool parseDeclaration(std::string const& key, TokenList& value, OutputParam& output);

public:

    void clear();
    void addDeclaration(DeclarationCondFunc cond, DeclarationHandleFunc handle);
    void addAtRuleHandler(typename RuleListParser<OutputParam>::RuleCondFunc cond,
                          typename RuleListParser<OutputParam>::RuleHandleFunc handle);

    void parse(TokenReader& source, OutputParam& output);

};

class Parser {

public:

    void parse(Tokenizer& tokenizer);

};

}
}