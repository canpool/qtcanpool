/**
 * Copyleft (C) 2023-2025 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MIT && MulanPSL-2.0
 **/

#include "ribbonappwindow.h"
#include "qxribbon/windowbuttongroup.h"
#include "qxwindow/windowagentwidget.h"

QX_WINDOW_USE_NAMESPACE
QX_RIBBON_USE_NAMESPACE

QX_WIDGET_BEGIN_NAMESPACE

class RibbonAppWindowPrivate
{
    QX_DECLARE_PUBLIC(RibbonAppWindow)
public:
    RibbonAppWindowPrivate();
    void init();
public:
    WindowButtonGroup *m_windowButtonGroup;
    WindowAgentWidget *m_windowAgent;
};

RibbonAppWindowPrivate::RibbonAppWindowPrivate()
    : m_windowButtonGroup(Q_NULLPTR)
    , m_windowAgent(Q_NULLPTR)
{
}

void RibbonAppWindowPrivate::init()
{
    Q_Q(RibbonAppWindow);
    RibbonBar *ribbonBar = q->ribbonBar();

    if (Q_NULLPTR == m_windowAgent) {
        m_windowAgent = new WindowAgentWidget(q);
        m_windowAgent->setup(q);
        m_windowAgent->setTitleBar(ribbonBar);
        m_windowAgent->addCaptionClassName(QString::fromLatin1("QxRibbon::RibbonBar"));
    }

    if (Q_NULLPTR == m_windowButtonGroup) {
        m_windowButtonGroup = new WindowButtonGroup(q);
        m_windowButtonGroup->setSignalEnabled(true);
        QObject::connect(m_windowButtonGroup, &WindowButtonGroup::buttonMinimizeClicked, q,
                         &RibbonAppWindow::showMinimized);
        QObject::connect(m_windowButtonGroup, &WindowButtonGroup::buttonMaximzieClicked, q, [q]() {
            if (q->isMaximized()) {
                q->showNormal();
            } else {
                q->showMaximized();
            }
        });
        QObject::connect(m_windowButtonGroup, &WindowButtonGroup::buttonCloseClicked, q, &RibbonAppWindow::close);
        if (ribbonBar) {
            ribbonBar->setWindowButtonGroup(m_windowButtonGroup);
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

RibbonAppWindow::RibbonAppWindow(QWidget *parent, Qt::WindowFlags flags)
    : QX_RIBBON_PREPEND_NAMESPACE(RibbonMainWindow)(parent)
{
    Q_UNUSED(flags);
    QX_INIT_PRIVATE(RibbonAppWindow)
    Q_D(RibbonAppWindow);
    d->init();
    resizeRibbon();
}

RibbonAppWindow::~RibbonAppWindow()
{
    QX_FINI_PRIVATE();
}

void RibbonAppWindow::updateWindowFlags(Qt::WindowFlags flags)
{
    Q_D(RibbonAppWindow);
    if (!(flags & Qt::WindowMaximizeButtonHint)) {
        if (isMaximized() || isFullScreen()) {
            showNormal();
        }
    }
    RibbonMainWindow::updateWindowFlags(flags);
    if (d->m_windowButtonGroup) {
        if (flags & Qt::WindowMaximizeButtonHint) {
            d->m_windowAgent->setSystemButton(WindowAgentBase::Maximize,
                                              d->m_windowButtonGroup->widgetForMaximizeButton());
        } else {
            d->m_windowAgent->removeSystemButton(WindowAgentBase::Maximize);
        }
        d->m_windowButtonGroup->updateWindowFlags(flags);
        // FIXME: In the WpsLiteStyle style, there is a short time overlap between rightButtonGroup and
        // windowButtonGroup. Because the button group will be displayed first, and then resize the ribbon.
        // I tried to hide the button group first and display the button group after resize ribbon,
        // but because resize ribbon is a trigger event and non-blocking, the button group was displayed
        // before resize was completed, so I failed.
        resizeRibbon();
    }
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
            resizeRibbon();
        }
        Q_EMIT windowStateChanged(s);
    } break;
    default:
        break;
    }
    return RibbonMainWindow::event(e);
}

QX_WIDGET_END_NAMESPACE
