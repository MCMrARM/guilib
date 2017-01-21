#pragma once

#include "ninepatch.h"
#include "color.h"

namespace guilib {

template <typename T>
struct GuiStyleProperty {

private:

    T currentValue;
    bool valueSet = false;
    bool important = false;

public:

    T const& get() const { return currentValue; }

    bool isSet() const { return valueSet; }

    bool isImportant() const { return important; }

    operator T() const { return currentValue; }

    GuiStyleProperty<T>& operator=(T const& value) {
        currentValue = value;
        valueSet = true;
        return *this;
    }

    void setFromString(std::string const& str);

};

struct GuiStyleState {

    bool hovered = false;
    bool pressed = false;

};

template <typename T>
struct GuiStyleObjectData {

    T normal, hover, press;
    T overrides;

};

#define GUI_OBJECT_STYLE(type, parent) \
    public: \
    struct StyleManager : public parent::StyleManager, public type ## StyleManager { \
        StyleManager(type& el) : parent::StyleManager(el), type ## StyleManager(el, el.styleData) { } \
    }; \
    private: \
    GuiStyleObjectData<type ## Style> styleData; \
    StyleManager styleManager; \
    public: \
    StyleManager& style() { return styleManager; } \
    StyleManager const& style() const { return styleManager; } \
    protected:

};
