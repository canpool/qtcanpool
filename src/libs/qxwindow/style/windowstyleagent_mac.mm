#include "windowstyleagent_p.h"

#include <Cocoa/Cocoa.h>

#include <QtCore/QVariant>

QX_WINDOW_BEGIN_NAMESPACE

static WindowStyleAgent::SystemTheme getSystemTheme() {
    NSString *osxMode =
        [[NSUserDefaults standardUserDefaults] stringForKey:@"AppleInterfaceStyle"];
    bool isDark = [osxMode isEqualToString:@"Dark"];
    return isDark ? WindowStyleAgent::Dark : WindowStyleAgent::Light;
}

static void notifyAllWindowStyleAgents();

QX_WINDOW_END_NAMESPACE

//
// Objective C++ Begin
//

@interface QWK_SystemThemeObserver : NSObject {
}
@end

@implementation QWK_SystemThemeObserver

- (id)init {
    self = [super init];
    if (self) {
        [[NSDistributedNotificationCenter defaultCenter]
            addObserver:self
               selector:@selector(interfaceModeChanged:)
                   name:@"AppleInterfaceThemeChangedNotification"
                 object:nil];
    }
    return self;
}

- (void)dealloc {
    [[NSDistributedNotificationCenter defaultCenter] removeObserver:self];
    [super dealloc];
}

- (void)interfaceModeChanged:(NSNotification *)notification {
    QWK::notifyAllWindowStyleAgents();
}

@end

//
// Objective C++ End
//

QX_WINDOW_BEGIN_NAMESPACE

using WindowStyleAgentSet = QSet<WindowStyleAgentPrivate *>;
Q_GLOBAL_STATIC(WindowStyleAgentSet, g_styleAgentSet)

static QWK_SystemThemeObserver *g_systemThemeObserver = nil;

void notifyAllWindowStyleAgents() {
    auto theme = getSystemTheme();
    for (auto &&ap : std::as_const(*g_styleAgentSet())) {
        ap->notifyThemeChanged(theme);
    }
}

void WindowStyleAgentPrivate::setupSystemThemeHook() {
    systemTheme = getSystemTheme();

    // Alloc
    if (g_styleAgentSet->isEmpty()) {
        g_systemThemeObserver = [[QWK_SystemThemeObserver alloc] init];
    }

    g_styleAgentSet->insert(this);
}

void WindowStyleAgentPrivate::removeSystemThemeHook() {
    if (!g_styleAgentSet->remove(this))
        return;

    if (g_styleAgentSet->isEmpty()) {
        // Delete
        [g_systemThemeObserver release];
        g_systemThemeObserver = nil;
    }
}

QX_WINDOW_END_NAMESPACE
