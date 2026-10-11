#ifndef WINDOWSTYLEAGENT_H
#define WINDOWSTYLEAGENT_H

#include "qxwindow/qxwindow_global.h"
#include <QtCore/QObject>
#include <QtGui/QWindow>
#include <memory>

QX_WINDOW_BEGIN_NAMESPACE

class WindowStyleAgentPrivate;

/*
    Deprecated in 4.2 (K26 / W2). It reports the system colour scheme -
    {Unknown, Light, Dark, HighContrast} - with one implementation per platform,
    but qxtheme already answers the same question and answers it more
    completely: QxThemeManager follows the system on its own (QStyleHints::
    colorScheme() on Qt 6.5 and later, the palette brightness before that, and it
    connects to colorSchemeChanged for later changes). Nothing in this tree
    refers to this class, no page documents it, and it is not even compiled
    unless QX_WINDOW_CONFIG_STYLE_AGENT is turned on.

    Its one unique offering is HighContrast, which QxThemeManager does not report.
    If that ever becomes a real need, it is the reason to reconsider - but decide
    it then, rather than by leaving the class in place.

    Deletion waits at least one release (K26), so 4.2 only marks it. Use
    QxTheme::QxThemeManager instead.
*/
#define QX_WINDOW_WINDOWSTYLEAGENT_DEPRECATED                                                                          \
    QT_DEPRECATED_X("WindowStyleAgent is deprecated; use QxTheme::QxThemeManager to follow the system colour scheme")

class QX_WINDOW_EXPORT QX_WINDOW_WINDOWSTYLEAGENT_DEPRECATED WindowStyleAgent : public QObject
{
    Q_OBJECT
    Q_DECLARE_PRIVATE(WindowStyleAgent)
public:
    explicit WindowStyleAgent(QObject *parent = nullptr);
    ~WindowStyleAgent() override;

    enum SystemTheme {
        Unknown,
        Light,
        Dark,
        HighContrast,
    };
    Q_ENUM(SystemTheme)
public:
    SystemTheme systemTheme() const;

Q_SIGNALS:
    void systemThemeChanged();
protected:
    WindowStyleAgent(WindowStyleAgentPrivate &d, QObject *parent = nullptr);

    const std::unique_ptr<WindowStyleAgentPrivate> d_ptr;
};

QX_WINDOW_END_NAMESPACE

#endif   // WINDOWSTYLEAGENT_H
