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


template <typename OutputParam>
class RuleListParser {

public:

    typedef std::function<bool (ComponentList const& key)> RuleCondFunc;
    typedef std::function<void (ComponentList const& key, ComponentReader& source, OutputParam& output)> RuleHandleFunc;

    struct RuleHandleDef {
        RuleCondFunc cond;
        RuleHandleFunc handle;
    };

    static void parseRule(std::vector<RuleHandleDef> const& handlers, ComponentReader& source, OutputParam& output,
                          ComponentList const& key);

private:
    std::vector<RuleHandleDef> qualifiedRules, atRules;

    void parseRule(ComponentReader& source, OutputParam& output, ComponentList const& key);

public:

    void clear();
    void addQualifiedRuleHandler(RuleCondFunc cond, RuleHandleFunc handle);
    void addAtRuleHandler(RuleCondFunc cond, RuleHandleFunc handle);

    void parse(ComponentReader& source, OutputParam& output, bool topLevel = false);

};

template <typename OutputParam>
class DeclarationListParser {

public:

    typedef std::function<bool (std::string const& key)> DeclarationCondFunc;
    typedef std::function<void (std::string const& key, ComponentList const& value, bool important, OutputParam& output)> DeclarationHandleFunc;

    struct DeclarationHandleDef {
        DeclarationCondFunc cond;
        DeclarationHandleFunc handle;
    };

private:

    std::vector<typename RuleListParser<OutputParam>::RuleHandleDef> atRules;
    std::vector<DeclarationHandleDef> declarations;

    bool parseDeclaration(std::string const& key, ComponentList& value, OutputParam& output);

public:

    void clear();
    void addDeclaration(DeclarationCondFunc cond, DeclarationHandleFunc handle);
    void addAtRuleHandler(typename RuleListParser<OutputParam>::RuleCondFunc cond,
                          typename RuleListParser<OutputParam>::RuleHandleFunc handle);

    void parse(ComponentReader& source, OutputParam& output);

};

class Parser {

public:

    void parse(Tokenizer& tokenizer);

};

}
}