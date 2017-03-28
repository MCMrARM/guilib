#pragma once

#include <functional>
#include <vector>
#include <stack>
#include "css_tokenizer.h"

namespace guilib {
namespace css {

class ComponentValue;

class ComponentList {

private:

    std::vector<ComponentValue> storage;

public:

    ComponentList() { }

    void append(ComponentValue&& token) { storage.push_back(std::move(token)); }
    void pop() { storage.pop_back(); }
    void clear() { storage.clear(); }

    size_t size() const { return storage.size(); }

    ComponentValue const& operator[](ssize_t index) const;

    inline std::vector<ComponentValue>::const_iterator begin() const { return storage.begin(); }
    inline std::vector<ComponentValue>::const_iterator end() const { return storage.end(); }

    std::string toString() const;

};

struct FunctionComponent {
    std::string name;
    ComponentList value;
};

class ComponentValue : public Token {

private:

    FunctionComponent function;

    friend class ComponentReader;

public:

    ComponentValue(TokenType type) : Token(type) { }
    ComponentValue(Token&& token) : Token(token) { }

    FunctionComponent const& asFunction() const { return function; }

    bool isFunction() const { return getType() == TokenType::FUNCTION; }

    bool isFunction(std::string const& name) const;

    std::string toString() const;

    static ComponentValue INVALID;

};

class ComponentReader {

private:

    Tokenizer& tokenizer;
    int currentBlockScope = 0;
    bool currentBlockScopeEof = false;
    std::stack<bool> blocksWereProperlyOpened;

    FunctionComponent parseFunction(std::string const& name);

public:

    ComponentReader(Tokenizer& tokenizer);

    ComponentValue next();

    void enterBasicBlock();
    void exitBasicBlock();

};

/**
 * This class is rather internal but feel free to use it if needed.
 * If you simply want to use the parsing functionality, please instead look into RuleListParser and
 * DeclarationListParser.
 */
class ParserHelper {

public:

    typedef std::function<bool (ComponentList const& key, ComponentReader& source)> RuleHandleFunc;
    typedef std::function<bool (std::string const& key, ComponentList const& value,
                                bool important)> DeclarationHandleFunc;


    static void parseRule(RuleHandleFunc const& handler, ComponentList const& key, ComponentReader& source);

    static void parseRuleList(ComponentReader& source, RuleHandleFunc qualifiedHandler, RuleHandleFunc atHandler,
                              bool topLevel = false);

    static void parseDeclaration(DeclarationHandleFunc const& handler, std::string const& key, ComponentList value);

    static void parseDeclarationList(ComponentReader& source, DeclarationHandleFunc declarationHandler,
                                     RuleHandleFunc atHandler);

};

template <typename OutputParam>
class RuleListParser {

public:

    typedef std::function<bool (ComponentList const& key)> RuleCondFunc;
    typedef std::function<void (ComponentList const& key, ComponentReader& source, OutputParam& output)> RuleHandleFunc;

private:

    struct RuleHandleDef {
        RuleCondFunc cond;
        RuleHandleFunc handle;
    };

    std::vector<RuleHandleDef> qualifiedRules, atRules;

    static bool parseRule(ComponentList const& key, ComponentReader& source,
                          std::vector<RuleHandleDef> const& rules, OutputParam& output) {
        for (RuleHandleDef const& rule : rules) {
            if (rule.cond(key)) {
                rule.handle(key, source, output);
                return true;
            }
        }
        return false;
    }

    template <typename> friend class DeclarationListParser;

public:

    void clear() {
        qualifiedRules.clear();
        atRules.clear();
    }
    void addQualifiedRuleHandler(RuleCondFunc cond, RuleHandleFunc handle) {
        qualifiedRules.push_back({cond, handle});
    }
    void addAtRuleHandler(RuleCondFunc cond, RuleHandleFunc handle) {
        atRules.push_back({cond, handle});
    }

    void parse(ComponentReader& source, OutputParam& output, bool topLevel = false) {
        using namespace std::placeholders;
        ParserHelper::parseRuleList(source, std::bind(parseRule, _1, _2, qualifiedRules, output),
                                    std::bind(parseRule, _1, _2, atRules, output), topLevel);
    }

};

template <typename OutputParam>
class DeclarationListParser {

public:

    typedef std::function<bool (std::string const& key)> DeclarationCondFunc;
    typedef std::function<void (std::string const& key, ComponentList const& value, bool important,
                                OutputParam& output)> DeclarationHandleFunc;

private:

    struct DeclarationHandleDef {
        DeclarationCondFunc cond;
        DeclarationHandleFunc handle;
    };

    std::vector<typename RuleListParser<OutputParam>::RuleHandleDef> atRules;
    std::vector<DeclarationHandleDef> declarations;

    static bool parseDeclaration(std::string const& key, ComponentList const& value, bool important,
                                 std::vector<DeclarationHandleDef> const& rules, OutputParam& output) {
        for (DeclarationHandleDef const& rule : rules) {
            if (rule.cond(key)) {
                rule.handle(key, value, important, output);
                return true;
            }
        }
        return false;
    }

public:

    void clear() {
        atRules.clear();
        declarations.clear();
    }
    void addDeclaration(DeclarationCondFunc cond, DeclarationHandleFunc handle) {
        declarations.push_back({cond, handle});
    }
    void addAtRuleHandler(typename RuleListParser<OutputParam>::RuleCondFunc cond,
                          typename RuleListParser<OutputParam>::RuleHandleFunc handle) {
        atRules.push_back({cond, handle});
    }

    void parse(ComponentReader& source, OutputParam& output) {
        using namespace std::placeholders;
        ParserHelper::parseDeclarationList(source, std::bind(parseDeclaration, _1, _2, _3, declarations, output),
                                           std::bind(RuleListParser<OutputParam>::parseRule, _1, _2, atRules, output));
    }

};

class Parser {

public:

    void parse(Tokenizer& tokenizer);

};

}
}