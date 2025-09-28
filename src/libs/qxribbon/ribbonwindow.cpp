/**
 * Copyleft (C) 2023-2025 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MIT
**/
#include "ribbonwindow.h"
#include "framelesshelper.h"
#include "windowbuttongroup.h"

#include <QApplication>

QX_RIBBON_BEGIN_NAMESPACE

class RibbonWindowPrivate
{
    QX_DECLARE_PUBLIC(RibbonWindow)
public:
    RibbonWindowPrivate();
    void destroyFrameless();
    void setFrameless(bool frameless);
    void init();
public:
    WindowButtonGroup *m_windowButtonGroup;
    FramelessHelper *m_framelessHelper;
    bool m_frameless;
};

RibbonWindowPrivate::RibbonWindowPrivate()
    : m_windowButtonGroup(Q_NULLPTR)
    , m_framelessHelper(Q_NULLPTR)
    , m_frameless(true)
{
}

void RibbonWindowPrivate::destroyFrameless()
{
    Q_Q(RibbonWindow);
    RibbonBar *rb = q->ribbonBar();

    if (m_framelessHelper) {
        delete m_framelessHelper;
        m_framelessHelper = Q_NULLPTR;
    }
    if (m_windowButtonGroup) {
        m_windowButtonGroup->hide();
        if (rb) {
            rb->setWindowButtonGroup(Q_NULLPTR);
        }
        delete m_windowButtonGroup;
        m_windowButtonGroup = Q_NULLPTR;
    }
}

void RibbonWindowPrivate::setFrameless(bool frameless)
{
    Q_Q(RibbonWindow);
    RibbonBar *rb = q->ribbonBar();

    if (frameless) {
        if (Q_NULLPTR == m_framelessHelper) {
            m_framelessHelper = new FramelessHelper(q);
        }
        m_framelessHelper->setTitleHeight(rb->titleBarHeight());
        m_framelessHelper->addCaptionClassName("QxRibbon::RibbonBar");
        if (Q_NULLPTR == m_windowButtonGroup) {
            m_windowButtonGroup = new WindowButtonGroup(q);
            if (rb) {
                rb->setWindowButtonGroup(m_windowButtonGroup);
            }
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

void RibbonWindowPrivate::init()
{
    setFrameless(m_frameless);
}

RibbonWindow::RibbonWindow(QWidget *parent)
    : RibbonMainWindow(parent)
{
    QX_INIT_PRIVATE(RibbonWindow)
    Q_D(RibbonWindow);
    d->init();
    resizeRibbon();
}

RibbonWindow::~RibbonWindow()
{
    QX_FINI_PRIVATE()
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
    ribbonBar()->setWindowTitleVisible(frameless);
    resizeRibbon();
}

void RibbonWindow::updateWindowFlags(Qt::WindowFlags flags)
{
    Q_D(RibbonWindow);
    setWindowFlags(flags);
    // Note: This function setWindowFlags calls setParent() when changing the flags for a window,
    // causing the widget to be hidden. You must call show() to make the widget visible again..
    show();
    if (isFrameless() && d->m_windowButtonGroup) {
        d->m_windowButtonGroup->updateWindowFlags(flags);
        // FIXME: In the WpsLiteStyle style, there is a short time overlap between rightButtonGroup and
        // windowButtonGroup. Because the button group will be displayed first, and then resize the ribbon.
        // I tried to hide the button group first and display the button group after resize ribbon,
        // but because resize ribbon is a trigger event and non-blocking, the button group was displayed
        // before resize was completed, so I failed.
        resizeRibbon();
    }
}

bool RibbonWindow::eventFilter(QObject *obj, QEvent *e)
{
    if (isFrameless()) {
        // event post ribbonBar -> mainwindow -> framelessHelper
        if (obj == ribbonBar()) {
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
    return RibbonMainWindow::eventFilter(obj, e);
}

bool RibbonWindow::event(QEvent *e)
{
    Q_D(RibbonWindow);
    switch (e->type()) {
    case QEvent::WindowStateChange: {
        Qt::WindowStates s = windowState();
        if (isFrameless() && d->m_windowButtonGroup) {
            if (s.testFlag(Qt::WindowFullScreen)) {
                d->m_windowButtonGroup->setVisible(false);
            } else {
                d->m_windowButtonGroup->setVisible(true);
            }
            d->m_windowButtonGroup->setWindowStates(s);
            resizeRibbon();
        }
        Q_EMIT windowStateChanged(s);
    } break;
    default:
        break;
    }
    return RibbonMainWindow::event(e);
}

QX_RIBBON_END_NAMESPACE
