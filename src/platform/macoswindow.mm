#include "macoswindow.h"

#include "core/application.h"

#include <Cocoa/Cocoa.h>

@interface MacOSWindowDelegate : NSObject<NSWindowDelegate>
@end

@implementation MacOSWindowDelegate
- (BOOL)windowShouldClose:(NSWindow*)sender {
    JohanGame2024::Application::instance().close();
    return YES;
}
@end

namespace JohanGame2024 {
    MacOSWindow::MacOSWindow(const WindowProps& props) {
        NSRect screenRect = [[NSScreen mainScreen] frame];
        NSRect windowRect = NSMakeRect(screenRect.size.width / 2 - (float)props.width / 2, screenRect.size.height / 2 - (float)props.height / 2, props.width, props.height);

        m_window = [[NSWindow alloc] initWithContentRect:windowRect styleMask:(NSWindowStyleMaskTitled | NSWindowStyleMaskClosable | NSWindowStyleMaskResizable) backing:NSBackingStoreBuffered defer:NO];
        [m_window setTitle:[NSString stringWithUTF8String:props.title.c_str()]];

        MacOSWindowDelegate* delegate = [[MacOSWindowDelegate alloc] init];
        [m_window setDelegate:delegate];

        [m_window makeKeyAndOrderFront:nil];
    }

    MacOSWindow::~MacOSWindow() {
        [m_window release];
    }

    void MacOSWindow::onUpdate() {
        NSEvent* event = [NSApp nextEventMatchingMask:NSEventMaskAny untilDate:[NSDate distantPast] inMode:NSDefaultRunLoopMode dequeue:YES];
        [NSApp sendEvent:event];
        [NSApp updateWindows];
    }

    Ref<Window> Window::create(const WindowProps& props) {
        return createRef<MacOSWindow>(props);
    }
}