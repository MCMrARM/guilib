#include "style_props_common.h"
STYLE_PROP2(textColor, color, GuiColor, GuiColor())
STYLE_PROP(fontFamily, font-family, std::string, std::string(), { el.notifyFontFamilyChanged(); })
STYLE_PROP2(fontScale, font-scale, float, 1.f)