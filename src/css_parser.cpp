#include <guilib/css_parser.h>

#include <guilib/css_tokenizer.h>

using namespace guilib::css;

ComponentValue ComponentValue::INVALID (TokenType::INVALID);

std::string ComponentValue::toString() const {
    if (isFunction()) {
        return asFunction().name + "(" + asFunction().value.toString() + ")";
    }
    return Token::toString();
}

ComponentValue const& ComponentList::operator[](ssize_t index) const {
    if (index < 0)
        index = storage.size() + index;
    if (index >= storage.size())
        return ComponentValue::INVALID;
    return storage.at((size_t) index);
}

std::string ComponentList::toString() const {
    std::string s;
    for (auto const& it : *this)
        s.append(it.toString());
    return s;
}

ComponentReader::ComponentReader(Tokenizer& tokenizer) : tokenizer(tokenizer) {
    //
}

FunctionComponent ComponentReader::parseFunction(std::string const& name) {
    FunctionComponent fcn;
    fcn.name = name;
    ComponentValue val (TokenType::INVALID);
    while (!(val = next()).isEOF() && val.getType() != TokenType::BRACKET_CLOSE)
        fcn.value.append(std::move(val));
    return fcn;
}

ComponentValue ComponentReader::next() {
    if (currentBlockScopeEof)
        return Token(TokenType::END_OF_FILE);
    ComponentValue ret = tokenizer.next();
    if (ret.getType() == TokenType::CURLY_BRACKET_CLOSE && currentBlockScope > 0) {
        if (blocksWereProperlyOpened.top()) {
            currentBlockScopeEof = true;
            return ComponentValue(TokenType::END_OF_FILE);
        } else if (currentBlockScope > 0) {
            currentBlockScope--;
            blocksWereProperlyOpened.pop();
        }
    }
    if (ret.getType() == TokenType::CURLY_BRACKET_OPEN) {
        currentBlockScope++;
        blocksWereProperlyOpened.push(false);
    }
    if (ret.getType() == TokenType::FUNCTION)
        ret.function = parseFunction(ret.asString().value);
    return ret;
}

void ComponentReader::enterBasicBlock() {
    blocksWereProperlyOpened.top() = true;
}

void ComponentReader::exitBasicBlock() {
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
void RuleListParser<OutputParam>::parseRule(std::vector<RuleHandleDef> const& handlers, ComponentReader& source, OutputParam& output,
                                            ComponentList const& key) {
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
void RuleListParser<OutputParam>::parseRule(ComponentReader& source, OutputParam& output, ComponentList const& key) {
    std::vector<RuleHandleDef>* handlers = &qualifiedRules;
    if (key[0].getType() == TokenType::AT_KEYWORD)
        handlers = &atRules;
    parseRule(*handlers, source, output, key);
}

template <typename OutputParam>
void RuleListParser<OutputParam>::parse(ComponentReader& source, OutputParam& output, bool topLevel) {
    ComponentList keyComponents;
    ComponentValue component (TokenType::INVALID);
    while (!(component = source.next()).isEOF()) {
        if (component.getType() == TokenType::WHITESPACE && keyComponents.size() == 0)
            continue;
        if (component.getType() == TokenType::CDO || component.getType() == TokenType::CDC) {
            if (topLevel)
                continue;
        }
        if (component.getType() == TokenType::CURLY_BRACKET_OPEN) {
            parseRule(source, output, keyComponents);
            keyComponents.clear();
            continue;
        }
        keyComponents.append(std::move(component));
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
bool DeclarationListParser<OutputParam>::parseDeclaration(std::string const& key, ComponentList& value, OutputParam& output) {
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
void DeclarationListParser<OutputParam>::parse(ComponentReader& source, OutputParam& output) {
    ComponentValue component (TokenType::INVALID);
    while (!(component = source.next()).isEOF()) {
        if (component.getType() == TokenType::WHITESPACE || component.getType() == TokenType::SEMICOLON)
            continue;
        if (component.getType() == TokenType::AT_KEYWORD) {
            ComponentList keyComponents;
            keyComponents.append(std::move(component));
            while (!(component = source.next()).isEOF()) {
                if (component.getType() == TokenType::CURLY_BRACKET_OPEN) {
                    RuleListParser<OutputParam>::parseRule(atRules, source, output, keyComponents);
                    break;
                }
                keyComponents.append(std::move(component));
            }
            continue;
        }
        if (component.getType() == TokenType::IDENT) {
            // consume a declaration
            std::string key = component.asIdent().value;
            component = source.next();
            if (component.getType() == TokenType::WHITESPACE)
                component = source.next();
            if (component.getType() != TokenType::COLON) // parse error
                continue;
            ComponentList components;
            if ((component = source.next()).getType() != TokenType::WHITESPACE)
                components.append(std::move(component)); // trim first token
            while (!(component = source.next()).isEOF() && component.getType() != TokenType::SEMICOLON)
                components.append(std::move(component));
            if (components.size() > 0 && component.getType() == TokenType::WHITESPACE)
                components.pop(); // trim last token
            parseDeclaration(key, components, output);
            continue;
        }
        // parse error
        while (!(component = source.next()).isEOF()) {
            if (component.getType() == TokenType::SEMICOLON)
                break;
        }
    }
}

void Parser::parse(Tokenizer& tokenizer) {
    ComponentReader reader (tokenizer);
    RuleListParser<void*> test;
    DeclarationListParser<void*> test2;
    test.addQualifiedRuleHandler([](ComponentList const&) { return true; }, [&test2](ComponentList const& key, ComponentReader& source, void*& output) {
        //printf("Parse qualified rule\n");
        test2.parse(source, output);
    });
    void* np;
    test.parse(reader, np);
}