/**
 * Copyright (C) 2023 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#include "windowtoolbar.h"
#include "windowtoolbar_p.h"

#include <QApplication>
#include <QToolBar>
#include <QAction>
#include <QToolButton>
#include <QStyle>
#include <QLayout>
#include <QHBoxLayout>

QX_WIDGET_BEGIN_NAMESPACE


WindowToolBarPrivate::WindowToolBarPrivate()
    : m_signalEnabled(false)
{

}

void WindowToolBarPrivate::init()
{
    Q_Q(WindowToolBar);

    const int sz = qApp->style()->pixelMetric(QStyle::PM_SmallIconSize, Q_NULLPTR, Q_NULLPTR);

    m_toolBar = new QToolBar(q);
    m_toolBar->layout()->setSizeConstraint(QLayout::SetFixedSize);
    m_toolBar->setObjectName(QLatin1String("qx_WindowToolBar"));
    m_toolBar->setIconSize(QSize(sz, sz));

    m_minimizeAction = new QAction(q);
    m_minimizeAction->setText(tr("minimize"));

    m_maximizeAction = new QAction(q);
    m_maximizeTip = QString(tr("maximize"));
    m_normalTip = QString(tr("restore"));
    m_maximizeAction->setText(m_maximizeTip);

    m_closeAction = new QAction(q);
    m_closeAction->setText(tr("close"));

    m_toolBar->addAction(m_minimizeAction);
    m_toolBar->addAction(m_maximizeAction);
    m_toolBar->addAction(m_closeAction);

    QToolButton *actionToolButton = nullptr;
    actionToolButton = qobject_cast<QToolButton *>(m_toolBar->widgetForAction(m_minimizeAction));
    m_minimizeAction->setIcon(actionToolButton->style()->standardIcon(QStyle::SP_TitleBarMinButton));
    actionToolButton->setObjectName(QLatin1String("minimize"));

    actionToolButton = qobject_cast<QToolButton *>(m_toolBar->widgetForAction(m_maximizeAction));
    m_maximizeIcon = actionToolButton->style()->standardIcon(QStyle::SP_TitleBarMaxButton);
    m_normalIcon = actionToolButton->style()->standardIcon(QStyle::SP_TitleBarNormalButton);
    m_maximizeAction->setIcon(m_maximizeIcon);
    actionToolButton->setObjectName(QLatin1String("maximize"));

    actionToolButton = qobject_cast<QToolButton *>(m_toolBar->widgetForAction(m_closeAction));
    m_closeAction->setIcon(actionToolButton->style()->standardIcon(QStyle::SP_TitleBarCloseButton));
    actionToolButton->setObjectName(QLatin1String("close"));

    connect(m_minimizeAction, SIGNAL(triggered()), this, SLOT(buttonClicked()));
    connect(m_maximizeAction, SIGNAL(triggered()), this, SLOT(buttonClicked()));
    connect(m_closeAction, SIGNAL(triggered()), this, SLOT(buttonClicked()));

    QHBoxLayout *lay = new QHBoxLayout(q);
    lay->setContentsMargins(0, 0, 0, 0);
    lay->setSpacing(0);
    lay->addStretch();
    lay->addWidget(m_toolBar);
}

void WindowToolBarPrivate::windowStateChange(QObject *obj)
{
    QWidget *w = static_cast<QWidget *>(obj);
    if (w) {
        setWindowStates(w->windowState());
    }
}

void WindowToolBarPrivate::setWindowStates(Qt::WindowStates states)
{
    if (m_maximizeAction) {
        bool on = states.testFlag(Qt::WindowMaximized);
        m_maximizeAction->setChecked(on);
        m_maximizeAction->setIcon(on ? m_normalIcon : m_maximizeIcon);
        m_maximizeAction->setToolTip(on ? m_normalTip : m_maximizeTip);
    }
}

void WindowToolBarPrivate::buttonClicked()
{
    Q_Q(WindowToolBar);
    QAction *action = qobject_cast<QAction *>(sender());

    if (m_signalEnabled) {
        if (action == m_minimizeAction) {
            emit q->buttonMinimizeClicked();
        } else if (action == m_maximizeAction) {
            emit q->buttonMaximzieClicked();
        } else if (action == m_closeAction) {
            emit q->buttonCloseClicked();
        }
    } else {
        QWidget *pw = q->parentWidget();
        if (pw) {
            if (action == m_minimizeAction) {
                pw->showMinimized();
            } else if (action == m_maximizeAction) {
                if (pw->isMaximized()) {
                    pw->showNormal();
                } else {
                    pw->showMaximized();
                }
            } else if (action == m_closeAction) {
                pw->close();
            }
        }
    }
}

WindowToolBar::WindowToolBar(QWidget *parent, Qt::WindowFlags flags)
    : QWidget{parent}
{
    Q_ASSERT(parent);
    if (parent) {
        parent->setMouseTracking(true);
        parent->setAttribute(Qt::WA_Hover, true);
        parent->installEventFilter(this);
    }

    QX_INIT_PRIVATE(WindowToolBar)
    Q_D(WindowToolBar);
    d->init();
    updateWindowFlags(flags);
}

WindowToolBar::~WindowToolBar()
{
    if (QWidget *pw = parentWidget()) {
        pw->removeEventFilter(this);
    }
    QX_FINI_PRIVATE()
}

QAction *WindowToolBar::addAction(const QIcon &icon, const QString &text)
{
    Q_D(WindowToolBar);
    QAction *action = new QAction(icon, text);
    d->m_toolBar->insertAction(d->m_minimizeAction, action);
    return action;
}

void WindowToolBar::addAction(QAction *action)
{
    Q_D(WindowToolBar);
    d->m_toolBar->insertAction(d->m_minimizeAction, action);
}

void WindowToolBar::insertAction(QAction *before, QAction *action)
{
    Q_D(WindowToolBar);
    d->m_toolBar->insertAction(before, action);
}

QAction *WindowToolBar::addSeparator()
{
    Q_D(WindowToolBar);
    return insertSeparator(d->m_minimizeAction);
}

QAction *WindowToolBar::insertSeparator(QAction *before)
{
    Q_D(WindowToolBar);
    return d->m_toolBar->insertSeparator(before);
}

QAction *WindowToolBar::addWidget(QWidget *widget)
{
    Q_D(WindowToolBar);
    return insertWidget(d->m_minimizeAction, widget);
}

QAction *WindowToolBar::insertWidget(QAction *before, QWidget *widget)
{
    Q_D(WindowToolBar);
    return d->m_toolBar->insertWidget(before, widget);
}

void WindowToolBar::updateWindowFlags(Qt::WindowFlags flags)
{
    Q_D(WindowToolBar);
    if (flags == Qt::WindowFlags()) {
        if (QWidget *pw = parentWidget()) {
            flags = pw->windowFlags();
        }
    }
    d->m_maximizeAction->setVisible(flags & Qt::WindowMaximizeButtonHint);
    d->m_minimizeAction->setVisible(flags & Qt::WindowMinimizeButtonHint);
    d->m_closeAction->setVisible(flags & Qt::WindowCloseButtonHint);
}

void WindowToolBar::setWindowStates(Qt::WindowStates states)
{
    Q_D(WindowToolBar);
    d->setWindowStates(states);
}

QSize WindowToolBar::iconSize() const
{
    Q_D(const WindowToolBar);
    return d->m_toolBar->iconSize();
}

void WindowToolBar::setIconSize(const QSize &size)
{
    Q_D(WindowToolBar);
    d->m_toolBar->setIconSize(size);
}

/**
 * If the signal is disabled, the window button's signal
 * will be directly applied to the parent window; otherwise,
 * only the button signal will be sent
 */
bool WindowToolBar::signalIsEnabled() const
{
    Q_D(const WindowToolBar);
    return d->m_signalEnabled;
}

void WindowToolBar::setSignalEnabled(bool enable)
{
    Q_D(WindowToolBar);
    d->m_signalEnabled = enable;
}

bool WindowToolBar::eventFilter(QObject *object, QEvent *event)
{
    Q_D(WindowToolBar);
    switch (event->type()) {
    case QEvent::WindowStateChange: {
        if (!d->m_signalEnabled) {
            d->windowStateChange(object);
            return true;
        }
        break;
    }
    default:
        break;
    }

    return QObject::eventFilter(object, event);
}

QX_WIDGET_END_NAMESPACE
