/**
 * Copyleft (C) 2023 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MIT
**/
#include "ribbonappwindow.h"
#include "qxribbon/ribbonbar.h"
#include "qxribbon/windowbuttongroup.h"
#include "qxwindow/widgetwindowagent.h"

#include <QApplication>
#include <QFile>

QX_WINDOW_USE_NAMESPACE

QX_RIBBON_BEGIN_NAMESPACE

class RibbonAppWindowPrivate
{
    QX_DECLARE_PUBLIC(RibbonAppWindow)
public:
    RibbonAppWindowPrivate();
    void setMenuWidget(QWidget *menuBar);
    void resizeRibbon();
public:
    RibbonBar *m_ribbonBar;
    WindowButtonGroup *m_windowButtonGroup;
    WidgetWindowAgent *m_windowAgent;
    int m_theme;
};

RibbonAppWindowPrivate::RibbonAppWindowPrivate()
    : m_ribbonBar(Q_NULLPTR)
    , m_windowButtonGroup(Q_NULLPTR)
    , m_windowAgent(Q_NULLPTR)
    , m_theme(RibbonTheme::LightOffice2013Theme)
{
}

void RibbonAppWindowPrivate::setMenuWidget(QWidget *menuBar)
{
    Q_Q(RibbonAppWindow);
    RibbonBar *bar = qobject_cast<RibbonBar *>(menuBar);

    if (bar) {
        m_ribbonBar = bar;
        m_ribbonBar->installEventFilter(q);
    } else {
        m_ribbonBar = Q_NULLPTR;
    }
    if (Q_NULLPTR == m_windowAgent) {
        m_windowAgent = new WidgetWindowAgent(q);
        m_windowAgent->setup(q);
        m_windowAgent->setTitleBar(bar);
        m_windowAgent->addCaptionClassName("QxRibbon::RibbonBar");
    }

    if (Q_NULLPTR == m_windowButtonGroup) {
        m_windowButtonGroup = new WindowButtonGroup(q);
        m_windowButtonGroup->setSignalEnabled(true);
        m_windowButtonGroup->setWindowBorder(2);
        QObject::connect(m_windowButtonGroup, &WindowButtonGroup::buttonMinimizeClicked,
                         q, &RibbonAppWindow::showMinimized);
        QObject::connect(m_windowButtonGroup, &WindowButtonGroup::buttonMaximzieClicked, q, [q]() {
            if (q->isMaximized()) {
                q->showNormal();
            } else {
                q->showMaximized();
            }
        });
        QObject::connect(m_windowButtonGroup, &WindowButtonGroup::buttonCloseClicked,
                         q, &RibbonAppWindow::close);
        if (m_ribbonBar) {
            m_ribbonBar->setWindowButtonGroup(m_windowButtonGroup);
        }
        m_windowAgent->setSystemButton(WindowAgentBase::Maximize, m_windowButtonGroup->widgetForMaximizeButton());
    }
    Qt::WindowStates s = q->windowState();
    if (s.testFlag(Qt::WindowFullScreen)) {
        m_windowButtonGroup->setVisible(false);
    } else {
        m_windowButtonGroup->setVisible(true);
    }
    m_windowButtonGroup->setWindowStates(s);
    m_windowAgent->setHitTestVisible(m_windowButtonGroup, true);
}

void RibbonAppWindowPrivate::resizeRibbon()
{
    if (m_ribbonBar == Q_NULLPTR) {
        return;
    }
    m_ribbonBar->resizeRibbon();
}

RibbonAppWindow::RibbonAppWindow(QWidget *parent)
    : QMainWindow(parent)
{
    QX_INIT_PRIVATE(RibbonAppWindow)
    Q_D(RibbonAppWindow);
    setRibbonTheme(ribbonTheme());
    setMenuWidget(new RibbonBar(this));
    d->resizeRibbon();
}

RibbonAppWindow::~RibbonAppWindow()
{
    QX_FINI_PRIVATE()
}

RibbonBar *RibbonAppWindow::ribbonBar() const
{
    Q_D(const RibbonAppWindow);
    return d->m_ribbonBar;
}

void RibbonAppWindow::setRibbonTheme(int theme)
{
    Q_D(RibbonAppWindow);
    RibbonTheme::setTheme(theme, this);
    d->m_theme = theme;

    if (d->m_ribbonBar) {
        d->m_ribbonBar->updateRibbonTheme();
        d->m_ribbonBar->resizeRibbon();
    }
}

int RibbonAppWindow::ribbonTheme() const
{
    Q_D(const RibbonAppWindow);
    return d->m_theme;
}

void RibbonAppWindow::updateWindowFlags(Qt::WindowFlags flags)
{
    Q_D(RibbonAppWindow);
    setWindowFlags(flags);
    // Note: This function setWindowFlags calls setParent() when changing the flags for a window,
    // causing the widget to be hidden. You must call show() to make the widget visible again..
    show();
    if (d->m_windowButtonGroup) {
        d->m_windowButtonGroup->updateWindowFlags(flags);
        // FIXME: In the WpsLiteStyle style, there is a short time overlap between rightButtonGroup and
        // windowButtonGroup. Because the button group will be displayed first, and then resize the ribbon.
        // I tried to hide the button group first and display the button group after resize ribbon,
        // but because resize ribbon is a trigger event and non-blocking, the button group was displayed
        // before resize was completed, so I failed.
        d->resizeRibbon();
    }
}

void RibbonAppWindow::setMenuWidget(QWidget *menuBar)
{
    Q_D(RibbonAppWindow);
    QMainWindow::setMenuWidget(menuBar);
    d->setMenuWidget(menuBar);
}

void RibbonAppWindow::setMenuBar(QMenuBar *menuBar)
{
    Q_D(RibbonAppWindow);
    QMainWindow::setMenuBar(menuBar);
    d->setMenuWidget(menuBar);
}

void RibbonAppWindow::resizeEvent(QResizeEvent *event)
{
    Q_D(RibbonAppWindow);
    if (d->m_ribbonBar) {
        if (d->m_ribbonBar->size().width() != this->size().width()) {
            d->m_ribbonBar->setFixedWidth(this->size().width());
        }
    }
    QMainWindow::resizeEvent(event);
}

bool RibbonAppWindow::eventFilter(QObject *obj, QEvent *e)
{
    return QMainWindow::eventFilter(obj, e);
}

bool RibbonAppWindow::event(QEvent *e)
{
    Q_D(RibbonAppWindow);
    switch (e->type()) {
    case QEvent::WindowStateChange: {
        Qt::WindowStates s = windowState();
        if (d->m_windowButtonGroup) {
            if (s.testFlag(Qt::WindowFullScreen)) {
                d->m_windowButtonGroup->setVisible(false);
            } else {
                d->m_windowButtonGroup->setVisible(true);
            }
            d->m_windowButtonGroup->setWindowStates(s);
            d->resizeRibbon();
        }
        emit windowStateChanged(s);
        break;
    }
    default:
        break;
    }
    return QMainWindow::event(e);
}

void RibbonAppWindow::loadTheme(const QString &themeFile)
{
    RibbonTheme::loadTheme(themeFile, this);
}

QX_RIBBON_END_NAMESPACE
