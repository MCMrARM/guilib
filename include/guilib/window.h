#pragma once

#include "container.h"

namespace guilib {

struct GuiWindowFlags {

    /**
     * This flag will force the window to be always on top. This means that it'll always be displayed over any current
     * window, even if it's a fullscreen one.
     */
    bool alwaysOnTop = false;

    /**
     * This flag will make the window "modal" - all other windows will be blocked by this one and won't receive any
     * input.
     */
    bool modal = false;

    /**
     * This flag will determine whether a close button should be displayed on the title bar.
     */
    bool closeable = false;

    /**
     * This flag will make the window only displayed when the user isn't in an fullscreen menu (either MCPE one like
     * pause, settings, etc, or a window handled by this gui library).
     */
    bool onlyShowInGame = true;

    /**
     * This flag will make the window fullscreen.
     */
    bool fullscreen = false;

    /**
     * This flag is only allowed when the window is fullscreen. If this is set to true, game won't be rendered in the
     * background.
     */
    bool fullscreenDrawGameBehind = false;

    /**
     * This flag is only allowed for fullscreen windows that require drawing game behind or modal popups. If you set it
     * to true, a half-transparent black overlay background will be drawn.
     */
    bool blackBackground = false;

};

class GuiWindow : public GuiContainer {

public:

    GuiWindowFlags flags;

    GuiWindow(float width, float height, GuiWindowFlags flags = GuiWindowFlags());

    virtual void draw();

};

}