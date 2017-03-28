#include <map>
#include <string>
#include "style_common.h"
#include "style_rule.h"

namespace guilib {

class DEF_NAME;

#define _STYLE_NAME(A, B) A ## B
#define STYLE_NAME(X) _STYLE_NAME(X, Style)
#define MGR_NAME(X) _STYLE_NAME(X, StyleManager)

struct STYLE_NAME(DEF_NAME) {

    static StyleRule::RegistrationHandle<STYLE_NAME(DEF_NAME)> Registration;

#define STYLE_PROP(name, cssname, type, def, cb) GuiStyleProperty<type> name = def;
#include DEF_LISTH
#undef STYLE_PROP

    STYLE_NAME(DEF_NAME)() {}
    STYLE_NAME(DEF_NAME)(std::map<std::string, std::string> const& prop) {

#define STYLE_PROP(name, cssname, type, def, cb) \
        if (prop.count(#cssname) > 0) \
            name.setFromString(prop.at(#cssname));
#include DEF_LISTH
#undef STYLE_PROP

    }

};

class MGR_NAME(DEF_NAME) {

private:
    DEF_NAME& el;
    GuiStyleObjectData<STYLE_NAME(DEF_NAME)>& data;
    GuiStyleState state;

public:
    MGR_NAME(DEF_NAME)(DEF_NAME& el, GuiStyleObjectData<STYLE_NAME(DEF_NAME)>& data) : el(el), data(data) { }

    void updateState(GuiStyleState const& newState);

#define STYLE_PROP(name, cssname, type, def, cb) \
    type const& name() const { \
        if (data.overrides.name.isSet()) \
            return data.overrides.name.get(); \
        if (state.pressed & data.press.name.isSet()) \
            return data.press.name.get(); \
        if (state.hovered && data.hover.name.isSet()) \
            return data.hover.name.get(); \
        return data.normal.name.get(); \
    }
#include DEF_LISTH
#undef STYLE_PROP

#undef STYLE_PROP2
#define STYLE_PROP(name, cssname, type, def, cb) \
    void name(type const& val);
#define STYLE_PROP2(name, cssname, type, def) \
    void name(type const& val) { \
        data.overrides.name = val; \
    }
#include DEF_LISTH
#undef STYLE_PROP
#undef STYLE_PROP2

};
#undef STYLE_NAME
#undef _STYLE_NAME
#undef MGR_NAME
#undef _MGR_NAME

};

#undef DEF_NAME
#undef DEF_LISTH