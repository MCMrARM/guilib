#include "style_props_common.h"
STYLE_PROP(left, left, float, -1.f, { el.notifyPositionDataChanged(); })
STYLE_PROP(right, right, float, -1.f, { el.notifyPositionDataChanged(); })
STYLE_PROP(top, top, float, -1.f, { el.notifyPositionDataChanged(); })
STYLE_PROP(bottom, bottom, float, -1.f, { el.notifyPositionDataChanged(); })
STYLE_PROP(width, width, float, -1.f, { el.notifyPositionDataChanged(); })
STYLE_PROP(height, height, float, -1.f, { el.notifyPositionDataChanged(); })
STYLE_PROP(marginLeft, margin-left, float, 0.f, { el.notifyPositionDataChanged(); })
STYLE_PROP(marginTop, margin-top, float, 0.f, { el.notifyPositionDataChanged(); })
STYLE_PROP(marginRight, margin-right, float, 0.f, { el.notifyPositionDataChanged(); })
STYLE_PROP(marginBottom, margin-bottom, float, 0.f, { el.notifyPositionDataChanged(); })
STYLE_PROP(paddingLeft, padding-left, float, 0.f, { el.notifyPositionDataChanged(); })
STYLE_PROP(paddingTop, padding-top, float, 0.f, { el.notifyPositionDataChanged(); })
STYLE_PROP(paddingRight, padding-right, float, 0.f, { el.notifyPositionDataChanged(); })
STYLE_PROP(paddingBottom, padding-bottom, float, 0.f, { el.notifyPositionDataChanged(); })
STYLE_PROP2(display, display, GuiDisplayMode, GuiDisplayMode::BLOCK)
STYLE_PROP2(borderWidth, border-width, float, 0.f)
STYLE_PROP2(borderColor, border-color, GuiColor, GuiColor())
STYLE_PROP2(backgroundColor, background-color, GuiColor, GuiColor(1.f, 1.f, 1.f, 0.f))
STYLE_PROP2(backgroundImage, background-image, GuiNinepatchImage, GuiNinepatchImage())