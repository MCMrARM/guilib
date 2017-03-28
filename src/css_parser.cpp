#include <guilib/css_parser.h>

#include <guilib/css_tokenizer.h>
#include <guilib/string_util.h>

using namespace guilib::css;

ComponentValue ComponentValue::INVALID (TokenType::INVALID);

bool ComponentValue::isFunction(std::string const& name) const {
    return isFunction() && StrUtil::equalsIgnoreCase(asFunction().name, name);
}

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

void ParserHelper::parseRule(RuleHandleFunc const& handler, ComponentList const& key, ComponentReader& source) {
    source.enterBasicBlock();
    if (handler(key, source)) {
        source.exitBasicBlock();
        return;
    }
    while (!source.next().isEOF());
    source.exitBasicBlock();
}

void ParserHelper::parseRuleList(ComponentReader& source, RuleHandleFunc qualifiedHandler, RuleHandleFunc atHandler,
                                 bool topLevel) {
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
            if (keyComponents[0].getType() == TokenType::AT_KEYWORD)
                parseRule(atHandler, keyComponents, source);
            else
                parseRule(qualifiedHandler, keyComponents, source);
            keyComponents.clear();
            continue;
        }
        keyComponents.append(std::move(component));
    }
}

void ParserHelper::parseDeclaration(DeclarationHandleFunc const& handler, std::string const& key,
                                    ComponentList value) {
    bool important = false;
    if (value[-2].isDelim('!') && value[-1].isIdent("important")) {
        value.pop();
        value.pop();
        important = true;
    }
    handler(key, value, important);
}

void ParserHelper::parseDeclarationList(ComponentReader& source, DeclarationHandleFunc declarationHandler,
                                        RuleHandleFunc atHandler) {
    ComponentValue component (TokenType::INVALID);
    while (!(component = source.next()).isEOF()) {
        if (component.getType() == TokenType::WHITESPACE || component.getType() == TokenType::SEMICOLON)
            continue;
        if (component.getType() == TokenType::AT_KEYWORD) {
            ComponentList keyComponents;
            keyComponents.append(std::move(component));
            while (!(component = source.next()).isEOF()) {
                if (component.getType() == TokenType::CURLY_BRACKET_OPEN) {
                    parseRule(atHandler, keyComponents, source);
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

            parseDeclaration(declarationHandler, key, std::move(components));
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
        printf("Parse qualified rule\n");
        test2.parse(source, output);
    });
    void* np;
    test.parse(reader, np);
}