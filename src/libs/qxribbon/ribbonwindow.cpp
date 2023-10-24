/**
 * Copyleft (C) 2023 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
**/
#include "ribbonwindow.h"
#include "framelesshelper.h"
#include "ribbonbar.h"
#include "windowbuttongroup.h"

#include <QApplication>
#include <QFile>

QX_RIBBON_BEGIN_NAMESPACE

class RibbonWindowPrivate
{
    QX_DECLARE_PUBLIC(RibbonWindow)
public:
    RibbonWindowPrivate();
    void setMenuWidget(QWidget *menuBar);
    void destroyFrameless();
    void setFrameless(bool frameless);
    void resizeRibbon();
public:
    RibbonBar *m_ribbonBar;
    WindowButtonGroup *m_windowButtonGroup;
    FramelessHelper *m_framelessHelper;
    int m_theme;
    bool m_useRibbon;
    bool m_frameless;
};

RibbonWindowPrivate::RibbonWindowPrivate()
    : m_ribbonBar(Q_NULLPTR)
    , m_windowButtonGroup(Q_NULLPTR)
    , m_framelessHelper(Q_NULLPTR)
    , m_theme(RibbonWindow::Office2013Theme)
    , m_useRibbon(true)
    , m_frameless(true)
{
}

void RibbonWindowPrivate::setMenuWidget(QWidget *menuBar)
{
    Q_Q(RibbonWindow);
    RibbonBar *bar = qobject_cast<RibbonBar *>(menuBar);

    if (bar) {
        m_ribbonBar = bar;
        m_ribbonBar->installEventFilter(q);
        m_useRibbon = true;
    } else {
        m_ribbonBar = Q_NULLPTR;
        m_useRibbon = false;
    }
    setFrameless(m_frameless);
}

void RibbonWindowPrivate::destroyFrameless()
{
    if (m_framelessHelper) {
        delete m_framelessHelper;
        m_framelessHelper = Q_NULLPTR;
    }
    if (m_windowButtonGroup) {
        m_windowButtonGroup->hide();
        delete m_windowButtonGroup;
        m_windowButtonGroup = Q_NULLPTR;
    }
}

void RibbonWindowPrivate::setFrameless(bool frameless)
{
    Q_Q(RibbonWindow);
    if (m_useRibbon && frameless) {
        if (Q_NULLPTR == m_framelessHelper) {
            m_framelessHelper = new FramelessHelper(q);
        }
        m_framelessHelper->setTitleHeight(m_ribbonBar->titleBarHeight());
        if (Q_NULLPTR == m_windowButtonGroup) {
            m_windowButtonGroup = new WindowButtonGroup(q);
        }
        Qt::WindowStates s = q->windowState();
        if (s.testFlag(Qt::WindowFullScreen)) {
            m_windowButtonGroup->setVisible(false);
        } else {
            m_windowButtonGroup->setVisible(true);
        }
        m_windowButtonGroup->setWindowStates(s);
        q->setWindowFlags(q->windowFlags() | Qt::FramelessWindowHint);
    } else {
        destroyFrameless();
        q->setWindowFlags(q->windowFlags() & ~Qt::FramelessWindowHint);
    }
}

void RibbonWindowPrivate::resizeRibbon()
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

RibbonWindow::RibbonWindow(QWidget *parent, bool useRibbon)
    : QMainWindow(parent)
{
    QX_INIT_PRIVATE(RibbonWindow)
    Q_D(RibbonWindow);
    if (useRibbon) {
        setRibbonTheme(ribbonTheme());
        setMenuWidget(new RibbonBar(this));
        d->resizeRibbon();
    }
}

RibbonWindow::~RibbonWindow()
{
    QX_FINI_PRIVATE()
}

RibbonBar *RibbonWindow::ribbonBar() const
{
    Q_D(const RibbonWindow);
    return d->m_ribbonBar;
}

void RibbonWindow::setRibbonTheme(int theme)
{
    Q_D(RibbonWindow);
    switch (theme) {
    case Office2013Theme:
        loadTheme(":/theme/res/office2013.css");
        break;
    case WpsdarkTheme:
        loadTheme(":/theme/res/wpsdark.css");
        break;
    case NormalTheme:
        loadTheme(":/theme/res/default.css");
        break;
    default:
        break;
    }
    d->m_theme = theme;

    if (d->m_ribbonBar) {
        d->m_ribbonBar->updateRibbonTheme();
        d->m_ribbonBar->resizeRibbon();
    }
}

int RibbonWindow::ribbonTheme() const
{
    Q_D(const RibbonWindow);
    return d->m_theme;
}

bool RibbonWindow::isUseRibbon() const
{
    Q_D(const RibbonWindow);
    return d->m_useRibbon;
}

bool RibbonWindow::isFrameless() const
{
    Q_D(const RibbonWindow);
    return d->m_frameless;
}

void RibbonWindow::setFrameless(bool frameless)
{
    Q_D(RibbonWindow);
    if (d->m_frameless == frameless) {
        return;
    }
    d->m_frameless = frameless;
    d->setFrameless(frameless);
    show(); // the m_windowButtonGroup.isVisible() judgment is added in resizeRibbon(), so show() is called earlier
    d->m_ribbonBar->setWindowTitleVisible(frameless);
    d->resizeRibbon();
}

void RibbonWindow::updateWindowFlags(Qt::WindowFlags flags)
{
    Q_D(RibbonWindow);
    setWindowFlags(flags);
    // Note: This function setWindowFlags calls setParent() when changing the flags for a window,
    // causing the widget to be hidden. You must call show() to make the widget visible again..
    show();
    if (isUseRibbon() && isFrameless()) {
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
}

void RibbonWindow::setMenuWidget(QWidget *menuBar)
{
    Q_D(RibbonWindow);
    QMainWindow::setMenuWidget(menuBar);
    d->setMenuWidget(menuBar);
}

void RibbonWindow::setMenuBar(QMenuBar *menuBar)
{
    Q_D(RibbonWindow);
    QMainWindow::setMenuBar(menuBar);
    d->setMenuWidget(menuBar);
}

void RibbonWindow::resizeEvent(QResizeEvent *event)
{
    Q_D(RibbonWindow);
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
    QMainWindow::resizeEvent(event);
}

bool RibbonWindow::eventFilter(QObject *obj, QEvent *e)
{
    Q_D(RibbonWindow);
    if (isFrameless()) {
        // event post ribbonBar -> mainwindow -> framelessHelper
        if (obj == d->m_ribbonBar) {
            switch (e->type()) {
            case QEvent::MouseButtonPress:
            case QEvent::MouseButtonRelease:
            case QEvent::MouseMove:
            case QEvent::Leave:
            case QEvent::HoverMove:
            case QEvent::MouseButtonDblClick:
                QApplication::sendEvent(this, e); // post to framelessHelper
            default:
                break;
            }
        }
    }
    return QMainWindow::eventFilter(obj, e);
}

bool RibbonWindow::event(QEvent *e)
{
    Q_D(RibbonWindow);
    switch (e->type()) {
    case QEvent::WindowStateChange: {
        if (isUseRibbon() && isFrameless()) {
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
        }
    } break;
    default:
        break;
    }
    return QMainWindow::event(e);
}

void RibbonWindow::loadTheme(const QString &themeFile)
{
    QFile file(themeFile);

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return;
    }
    setStyleSheet(QString::fromUtf8(file.readAll()));
    file.close();
}

QX_RIBBON_END_NAMESPACE
