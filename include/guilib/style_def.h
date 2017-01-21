#include <map>
#include <string>
#include "style_common.h"

#define STYLE_PROP2(name, cssname, type, def) STYLE_PROP(name, cssname, type, def, )

namespace guilib {

struct DEF_NAME {

#define STYLE_PROP(name, cssname, type, def) GuiStyleProperty<type> name;
#include DEF_LISTH
#undef STYLE_PROP

    DEF_NAME() {}
    DEF_NAME(std::map<std::string, std::string> const& prop) {

#define STYLE_PROP(name, cssname, type, def) \
        if (prop.count(#cssname) > 0) \
            name.setFromString(prop.at(#cssname));
#include DEF_LISTH
#undef STYLE_PROP

    }

};

#define _MGR_NAME(A, B) A ## B
#define MGR_NAME(X) _MGR_NAME(X, Manager)
class MGR_NAME(DEF_NAME) {

private:
    GuiStyleObjectData<DEF_NAME>& data;
    GuiStyleState& state;

public:
    MGR_NAME(DEF_NAME)(GuiStyleObjectData<DEF_NAME>& data, GuiStyleState& state) : data(data), state(state) { }

#define STYLE_PROP(name, cssname, type, def) \
    void name(type const& val) { \
        data.overrides.name = val; \
    } \
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

};
#undef MGR_NAME
#undef _MGR_NAME

};

#undef DEF_NAME
#undef DEF_LISTH