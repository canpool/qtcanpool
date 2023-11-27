/**
 * Copyright (C) 2023 maminjie <canpool@163.com> (https://github.com/canpool)
 * Copyright (C) 2021-2023 Antonio Dias (https://github.com/antonypro)
 * SPDX-License-Identifier: MIT
**/

#include "macosnative.h"
#include <Cocoa/Cocoa.h>

void macOSNative::setStyle(long winid, StyleType type)
{
    NSView *nativeView = reinterpret_cast<NSView*>(winid);
    NSWindow *nativeWindow = [nativeView window];

    switch (type)
    {
    case StyleType::NoState:
    {
        [nativeWindow setStyleMask:NSWindowStyleMaskResizable |
                                   NSWindowStyleMaskMiniaturizable |
                                   NSWindowStyleMaskClosable |
                                   NSWindowStyleMaskTitled |
                                   NSWindowStyleMaskFullSizeContentView];
        [nativeWindow setMovableByWindowBackground:NO];
        [nativeWindow setMovable:NO];
        [nativeWindow setTitlebarAppearsTransparent:YES];
        [nativeWindow setShowsToolbarButton:NO];
        [nativeWindow setTitleVisibility:NSWindowTitleHidden];
        [nativeWindow standardWindowButton:NSWindowMiniaturizeButton].hidden = YES;
        [nativeWindow standardWindowButton:NSWindowCloseButton].hidden = YES;
        [nativeWindow standardWindowButton:NSWindowZoomButton].hidden = YES;
        [nativeWindow makeKeyWindow];

        break;
    }
    case StyleType::Disabled:
    {
        [nativeWindow setStyleMask:NSWindowStyleMaskFullSizeContentView];
        [nativeWindow setMovableByWindowBackground:NO];
        [nativeWindow setMovable:NO];
        [nativeWindow setTitlebarAppearsTransparent:YES];
        [nativeWindow setShowsToolbarButton:NO];
        [nativeWindow setTitleVisibility:NSWindowTitleHidden];
        [nativeWindow standardWindowButton:NSWindowMiniaturizeButton].hidden = YES;
        [nativeWindow standardWindowButton:NSWindowCloseButton].hidden = YES;
        [nativeWindow standardWindowButton:NSWindowZoomButton].hidden = YES;
        [nativeWindow makeKeyWindow];

        break;
    }
    case StyleType::Fullscreen:
    {
        [nativeWindow setStyleMask:0];

        break;
    }
    }
}

//\cond HIDDEN_SYMBOLS
@interface Handler : NSObject
{
}
@end
//\endcond

Handler *m_handler;

void macOSNative::registerNotification(const char *notification_name, long wid)
{
    NSView *nativeView = reinterpret_cast<NSView*>(wid);
    NSWindow *nativeWindow = [nativeView window];

    [[NSNotificationCenter defaultCenter]
    addObserver:m_handler
    selector:@selector(NotificationHandler:)
    name:[NSString stringWithUTF8String:notification_name]
    object:nativeWindow];
}

void macOSNative::unregisterNotification()
{
    [[NSNotificationCenter defaultCenter]
    removeObserver:m_handler];

    [m_handler release];
}

//\cond HIDDEN_SYMBOLS
@implementation Handler
+ (void)load
{
    m_handler = static_cast<Handler*>(self);
}

+(void)NotificationHandler:(NSNotification*)notification
{
    const NSString *str = [notification name];

    const NSWindow *nativeWindow = [notification object];

    const NSView *nativeView = [nativeWindow contentView];

    const char *cstr = [str cStringUsingEncoding:NSUTF8StringEncoding];

    macOSNative::handleNotification(cstr, long(nativeView));
}
@end
//\endcond

//\cond HIDDEN_SYMBOLS
@interface ThemeChangeHandler : NSObject
{
}
@end
//\endcond

ThemeChangeHandler *m_theme_change_handler;

//\cond HIDDEN_SYMBOLS
@implementation ThemeChangeHandler
+ (void)load
{
    m_theme_change_handler = static_cast<ThemeChangeHandler*>(self);
}

+(void)ThemeChangeNotification:(NSNotification*)notification
{
    const NSString *str = [notification name];

    const char *cstr = [str cStringUsingEncoding:NSUTF8StringEncoding];

    macOSNative::handleNotification(cstr, 0);
}
@end
//\endcond

void macOSNative::registerThemeChangeNotification()
{
    [[NSDistributedNotificationCenter defaultCenter]
    addObserver:m_theme_change_handler
    selector:@selector(ThemeChangeNotification:)
    name:@"AppleInterfaceThemeChangedNotification"
    object:nil];
}

const char *macOSNative::themeName()
{
    NSString *str = [[NSUserDefaults standardUserDefaults] stringForKey:@"AppleInterfaceStyle"];
    const char *cstr = [str cStringUsingEncoding:NSUTF8StringEncoding];
    return cstr;
}
