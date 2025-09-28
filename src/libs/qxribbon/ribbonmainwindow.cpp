/**
 * Copyleft (C) 2025 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MIT
 **/

#include "ribbonmainwindow.h"
#include "ribbonbar.h"

QX_RIBBON_BEGIN_NAMESPACE

class RibbonMainWindowPrivate
{
    QX_DECLARE_PUBLIC(RibbonMainWindow)
public:
    RibbonMainWindowPrivate();
    void setMenuWidget(QWidget *menuBar);
public:
    RibbonBar *m_ribbonBar;
    int m_theme;
};

RibbonMainWindowPrivate::RibbonMainWindowPrivate()
    : m_ribbonBar(Q_NULLPTR)
    , m_theme(RibbonTheme::LightOffice2013Theme)
{
}

void RibbonMainWindowPrivate::setMenuWidget(QWidget *menuBar)
{
    Q_Q(RibbonMainWindow);
    RibbonBar *bar = qobject_cast<RibbonBar *>(menuBar);

    if (m_ribbonBar != Q_NULLPTR) {
        m_ribbonBar->removeEventFilter(q);
        delete m_ribbonBar;
    }

    if (bar) {
        m_ribbonBar = bar;
        m_ribbonBar->installEventFilter(q);
    } else {
        m_ribbonBar = Q_NULLPTR;
    }
}

RibbonMainWindow::RibbonMainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    QX_INIT_PRIVATE(RibbonMainWindow)
    setRibbonTheme(ribbonTheme());
    setMenuWidget(new RibbonBar(this));
    resizeRibbon();
}

RibbonMainWindow::~RibbonMainWindow()
{
    QX_FINI_PRIVATE();
}

RibbonBar *RibbonMainWindow::ribbonBar() const
{
    Q_D(const RibbonMainWindow);
    return d->m_ribbonBar;
}

void RibbonMainWindow::resizeRibbon()
{
    Q_D(RibbonMainWindow);
    if (d->m_ribbonBar) {
        d->m_ribbonBar->resizeRibbon();
    }
}

void RibbonMainWindow::setRibbonTheme(int theme)
{
    Q_D(RibbonMainWindow);
    RibbonTheme::setTheme(theme, this);
    d->m_theme = theme;

    if (d->m_ribbonBar) {
        d->m_ribbonBar->updateRibbonTheme();
        d->m_ribbonBar->resizeRibbon();
    }
}

int RibbonMainWindow::ribbonTheme() const
{
    Q_D(const RibbonMainWindow);
    return d->m_theme;
}

void RibbonMainWindow::updateWindowFlags(Qt::WindowFlags flags)
{
    setWindowFlags(flags);
    // Note: This function setWindowFlags calls setParent() when changing the flags for a window,
    // causing the widget to be hidden. You must call show() to make the widget visible again..
    show();
}

void RibbonMainWindow::setMenuWidget(QWidget *menuBar)
{
    Q_D(RibbonMainWindow);
    QMainWindow::setMenuWidget(menuBar);
    d->setMenuWidget(menuBar);
}

void RibbonMainWindow::setMenuBar(QMenuBar *menuBar)
{
    setMenuWidget(menuBar);
}

void RibbonMainWindow::loadTheme(const QString &themeFile)
{
    RibbonTheme::loadTheme(themeFile, this);
}

bool RibbonMainWindow::event(QEvent *e)
{
    Q_D(RibbonMainWindow);
    switch (e->type()) {
    case QEvent::Resize: {
        if (d->m_ribbonBar) {
            if (d->m_ribbonBar->size().width() != this->size().width()) {
                d->m_ribbonBar->setFixedWidth(this->size().width());
            }
        }
    } break;
    default:
        break;
    }
    return QMainWindow::event(e);
}

QX_RIBBON_END_NAMESPACE
