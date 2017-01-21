#include "style_common.h"

#define _MGR_NAME(A, B) A ## B
#define MGR_NAME(X) _MGR_NAME(X, StyleManager)

void guilib::MGR_NAME(DEF_NAME)::updateState(guilib::GuiStyleState const& newState) {
    guilib::GuiStyleState oldState = state;
    state = newState;
#undef STYLE_PROP2
#define STYLE_PROP2(name, cssname, type, def)
#define STYLE_PROP(name, cssname, type, def, cb) \
        if (!data.overrides.name.isSet() && \
                ((newState.pressed != oldState.pressed && data.press.name.isSet()) || \
                 (newState.hovered != oldState.hovered && data.hover.name.isSet()))) { \
            /* create context */ \
            auto val = name(); \
            cb \
        }
#include DEF_LISTH
#undef STYLE_PROP
}

#undef STYLE_PROP2
#define STYLE_PROP2(name, cssname, type, def)
#define STYLE_PROP(name, cssname, type, def, cb) \
void guilib::MGR_NAME(DEF_NAME)::name(type const& val) { \
    data.overrides.name = val; \
    cb \
}

#include DEF_LISTH
#undef STYLE_PROP
#undef STYLE_PROP2

#undef MGR_NAME
#undef _MGR_NAME