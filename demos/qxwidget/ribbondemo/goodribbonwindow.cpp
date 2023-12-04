/**
 * Copyleft (C) 2023 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MIT
**/
#include "goodribbonwindow.h"
#include "qxribbon/ribbonbar.h"
#include "qxribbon/windowbuttongroup.h"

#include <QApplication>
#include <QFile>

QX_RIBBON_BEGIN_NAMESPACE

class GoodRibbonWindowPrivate
{
    QX_DECLARE_PUBLIC(GoodRibbonWindow)
public:
    GoodRibbonWindowPrivate();
    void setMenuWidget(QWidget *menuBar);
    void resizeRibbon();
public:
    RibbonBar *m_ribbonBar;
    WindowButtonGroup *m_windowButtonGroup;
    int m_theme;
};

GoodRibbonWindowPrivate::GoodRibbonWindowPrivate()
    : m_ribbonBar(Q_NULLPTR)
    , m_windowButtonGroup(Q_NULLPTR)
    , m_theme(RibbonTheme::Office2013Theme)
{
}

void GoodRibbonWindowPrivate::setMenuWidget(QWidget *menuBar)
{
    Q_Q(GoodRibbonWindow);
    RibbonBar *bar = qobject_cast<RibbonBar *>(menuBar);

    if (bar) {
        m_ribbonBar = bar;
        m_ribbonBar->installEventFilter(q);
    } else {
        m_ribbonBar = Q_NULLPTR;
    }

    if (Q_NULLPTR == m_windowButtonGroup) {
        m_windowButtonGroup = new WindowButtonGroup(q);
        m_windowButtonGroup->setSignalEnabled(true);
        QObject::connect(m_windowButtonGroup, &WindowButtonGroup::buttonMinimizeClicked,
                         q, &GoodRibbonWindow::showMinimized);
        QObject::connect(m_windowButtonGroup, &WindowButtonGroup::buttonMaximzieClicked, q, [q]() {
            if (q->isMaximized()) {
                q->showNormal();
            } else {
                q->showMaximized();
            }
        });
        QObject::connect(m_windowButtonGroup, &WindowButtonGroup::buttonCloseClicked,
                         q, &GoodRibbonWindow::close);
    }
    Qt::WindowStates s = q->windowState();
    if (s.testFlag(Qt::WindowFullScreen)) {
        m_windowButtonGroup->setVisible(false);
    } else {
        m_windowButtonGroup->setVisible(true);
    }
    m_windowButtonGroup->setWindowStates(s);
}

void GoodRibbonWindowPrivate::resizeRibbon()
{
    if (m_ribbonBar == Q_NULLPTR) {
        return;
    }
    if (m_windowButtonGroup && m_windowButtonGroup->isVisible()) {
        m_ribbonBar->setWindowButtonsSize(m_windowButtonGroup->size());
    } else {
        m_ribbonBar->setWindowButtonsSize(QSize(0, 0));
    }
    m_ribbonBar->resizeRibbon();
}

GoodRibbonWindow::GoodRibbonWindow(QWidget *parent)
    : QGoodWindow(parent)
{
    addCaptionClassName("QxRibbon::RibbonBar");

    QX_INIT_PRIVATE(GoodRibbonWindow)
    Q_D(GoodRibbonWindow);
    setRibbonTheme(ribbonTheme());
    setMenuWidget(new RibbonBar(this));
    d->resizeRibbon();
}

GoodRibbonWindow::~GoodRibbonWindow()
{
    QX_FINI_PRIVATE()
}

RibbonBar *GoodRibbonWindow::ribbonBar() const
{
    Q_D(const GoodRibbonWindow);
    return d->m_ribbonBar;
}

void GoodRibbonWindow::setRibbonTheme(int theme)
{
    Q_D(GoodRibbonWindow);
    RibbonTheme::setTheme(theme, this);
    d->m_theme = theme;

    if (d->m_ribbonBar) {
        d->m_ribbonBar->updateRibbonTheme();
        d->m_ribbonBar->resizeRibbon();
    }
}

int GoodRibbonWindow::ribbonTheme() const
{
    Q_D(const GoodRibbonWindow);
    return d->m_theme;
}

void GoodRibbonWindow::updateWindowFlags(Qt::WindowFlags flags)
{
    Q_D(GoodRibbonWindow);
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

void GoodRibbonWindow::setMenuWidget(QWidget *menuBar)
{
    Q_D(GoodRibbonWindow);
    QGoodWindow::setMenuWidget(menuBar);
    d->setMenuWidget(menuBar);
}

void GoodRibbonWindow::setMenuBar(QMenuBar *menuBar)
{
    Q_D(GoodRibbonWindow);
    QGoodWindow::setMenuBar(menuBar);
    d->setMenuWidget(menuBar);
}

void GoodRibbonWindow::resizeEvent(QResizeEvent *event)
{
    Q_D(GoodRibbonWindow);
    if (d->m_ribbonBar) {
        if (d->m_ribbonBar->size().width() != this->size().width()) {
            d->m_ribbonBar->setFixedWidth(this->size().width());
        }
        if (d->m_windowButtonGroup && d->m_windowButtonGroup->isVisible()) {
            d->m_ribbonBar->setWindowButtonsSize(d->m_windowButtonGroup->size());
        } else {
            d->m_ribbonBar->setWindowButtonsSize(QSize(0, 0));
        }
    }
    QGoodWindow::resizeEvent(event);
}

bool GoodRibbonWindow::eventFilter(QObject *obj, QEvent *e)
{
    return QGoodWindow::eventFilter(obj, e);
}

bool GoodRibbonWindow::event(QEvent *e)
{
    Q_D(GoodRibbonWindow);
    switch (e->type()) {
    case QEvent::WindowStateChange: {
        if (d->m_windowButtonGroup) {
            Qt::WindowStates s = windowState();
            if (s.testFlag(Qt::WindowFullScreen)) {
                d->m_windowButtonGroup->setVisible(false);
            } else {
                d->m_windowButtonGroup->setVisible(true);
            }
            d->m_windowButtonGroup->setWindowStates(s);
            d->resizeRibbon();
        }
        break;
    }
    default:
        break;
    }
    return QGoodWindow::event(e);
}

void GoodRibbonWindow::loadTheme(const QString &themeFile)
{
    RibbonTheme::loadTheme(themeFile, this);
}

QX_RIBBON_END_NAMESPACE
