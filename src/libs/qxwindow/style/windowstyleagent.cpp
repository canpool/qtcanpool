#include "windowstyleagent.h"
#include "windowstyleagent_p.h"
#include <QtCore/QVariant>

QX_WINDOW_BEGIN_NAMESPACE

/*!
    \class WindowStyleAgent
    \brief WindowStyleAgent provides some features related to system theme.

    Qt6.6 started to support system theme detection, this class is intended as an auxiliary
    support for lower versions of Qt. If your Qt already supports it, it is recommended that
    you don't include this class in your build system.
*/

WindowStyleAgentPrivate::WindowStyleAgentPrivate()
{
}

WindowStyleAgentPrivate::~WindowStyleAgentPrivate()
{
    removeSystemThemeHook();
}

void WindowStyleAgentPrivate::init()
{
    setupSystemThemeHook();
}

void WindowStyleAgentPrivate::notifyThemeChanged(WindowStyleAgent::SystemTheme theme)
{
    if (theme == systemTheme)
        return;
    systemTheme = theme;

    Q_Q(WindowStyleAgent);
    Q_EMIT q->systemThemeChanged();
}

/*!
    Constructor. Since it is not related to a concrete window instance, it is better to be used
    as a singleton.
*/
WindowStyleAgent::WindowStyleAgent(QObject *parent) : WindowStyleAgent(*new WindowStyleAgentPrivate(), parent)
{
}

/*!
    Destructor.
*/
WindowStyleAgent::~WindowStyleAgent()
{
}

/*!
    Returns the system theme.
*/
WindowStyleAgent::SystemTheme WindowStyleAgent::systemTheme() const
{
    Q_D(const WindowStyleAgent);
    return d->systemTheme;
}

/*!
    \internal
*/
WindowStyleAgent::WindowStyleAgent(WindowStyleAgentPrivate &d, QObject *parent) : QObject(parent), d_ptr(&d)
{
    d.q_ptr = this;

    d.init();
}

/*!
    \fn void WindowStyleAgent::systemThemeChanged()

    This signal is emitted when the system theme changes.
*/

QX_WINDOW_END_NAMESPACE
