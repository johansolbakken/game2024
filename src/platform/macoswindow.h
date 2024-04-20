#pragma once

#include "core/base.h"
#include "core/window.h"

#include <Cocoa/Cocoa.h>

namespace JohanGame2024
{
    class MacOSWindow : public Window
    {
    public:
        MacOSWindow(const WindowProps &props);
        virtual ~MacOSWindow();

        virtual void onUpdate() override;

    private:
        NSWindow *m_window;
    };
}