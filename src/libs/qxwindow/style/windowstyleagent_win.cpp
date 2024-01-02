#include "windowstyleagent_p.h"
#include "qxwindow/windowevent_p.h"
#include "qxwindow/windowkit_win_p.h"
#include <QtCore/QSet>
#include <QtCore/QVariant>
#include <QtGui/QColor>

QX_WINDOW_BEGIN_NAMESPACE

using WindowStyleAgentSet = QSet<WindowStyleAgentPrivate *>;
Q_GLOBAL_STATIC(WindowStyleAgentSet, g_styleAgentSet)

static WindowStyleAgent::SystemTheme getSystemTheme()
{
    if (isHighContrastModeEnabled()) {
        return WindowStyleAgent::HighContrast;
    } else if (isDarkThemeActive()) {
        return WindowStyleAgent::Dark;
    } else {
        return WindowStyleAgent::Light;
    }
}

static void notifyAllWindowStyleAgents()
{
    auto theme = getSystemTheme();
    for (auto &&ap : qAsConst(*g_styleAgentSet())) {
        ap->notifyThemeChanged(theme);
    }
}

class SystemSettingEventFilter : public AppNativeEventFilter
{
public:
    bool nativeEventFilter(const QByteArray &eventType, void *message, QT_NATIVE_EVENT_RESULT_TYPE *result) override
    {
        Q_UNUSED(eventType)
        if (!result) {
            return false;
        }

        const auto msg = static_cast<const MSG *>(message);
        switch (msg->message) {
        case WM_THEMECHANGED:
        case WM_SYSCOLORCHANGE:
        case WM_DWMCOLORIZATIONCOLORCHANGED: {
            notifyAllWindowStyleAgents();
            break;
        }

        case WM_SETTINGCHANGE: {
            if (isImmersiveColorSetChange(msg->wParam, msg->lParam)) {
                notifyAllWindowStyleAgents();
            }
            break;
        }

        default:
            break;
        }
        return false;
    }

    static SystemSettingEventFilter *instance;

    static inline void install()
    {
        if (instance) {
            return;
        }
        instance = new SystemSettingEventFilter();
    }

    static inline void uninstall()
    {
        if (!instance) {
            return;
        }
        delete instance;
        instance = nullptr;
    }
};

SystemSettingEventFilter *SystemSettingEventFilter::instance = nullptr;

void WindowStyleAgentPrivate::setupSystemThemeHook()
{
    systemTheme = getSystemTheme();

    g_styleAgentSet->insert(this);
    SystemSettingEventFilter::install();
}

void WindowStyleAgentPrivate::removeSystemThemeHook()
{
    if (!g_styleAgentSet->remove(this))
        return;

    if (g_styleAgentSet->isEmpty()) {
        SystemSettingEventFilter::uninstall();
    }
}

QX_WINDOW_END_NAMESPACE
