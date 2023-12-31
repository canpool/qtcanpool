#ifndef WINDOWSTYLEAGENT_H
#define WINDOWSTYLEAGENT_H

#include "qxwindow/qxwindow_global.h"
#include <QtCore/QObject>
#include <QtGui/QWindow>
#include <memory>

QX_WINDOW_BEGIN_NAMESPACE

class WindowStyleAgentPrivate;

class QX_WINDOW_EXPORT WindowStyleAgent : public QObject
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
