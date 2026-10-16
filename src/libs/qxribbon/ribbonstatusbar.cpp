/**
 * Copyleft (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MIT
 **/
#include "ribbonstatusbar.h"
#include "ribbonstatusbar_p.h"

#include <QAction>
#include <QFrame>
#include <QLabel>
#include <QStatusBar>
#include <QToolButton>

QX_RIBBON_BEGIN_NAMESPACE

// The width of the hairline separator between two left-section segments.
const int statusBarSeparatorWidth = 2;

RibbonStatusBarPrivate::RibbonStatusBarPrivate()
{
}

RibbonStatusBar::RibbonStatusBar(QWidget *parent)
    : QStatusBar(parent)
{
    QX_INIT_PRIVATE(RibbonStatusBar);
    // A name a theme can target to style the bar specifically; the dark themes
    // already carry a plain QStatusBar rule, so the bar is themed either way.
    setObjectName(QStringLiteral("qx_RibbonStatusBar"));
}

RibbonStatusBar::~RibbonStatusBar()
{
    QX_FINI_PRIVATE();
}

QAction *RibbonStatusBar::addAction(const QString &text)
{
    Q_D(RibbonStatusBar);
    QAction *action = new QAction(text, this);
    QLabel *label = new QLabel(this);
    label->setObjectName(QStringLiteral("qx_RibbonStatusBarSegment"));
    label->setText(text);
    // The segment shows whatever the action's text is, so the host updates a
    // segment the way it would for any other action - by setting its text. The
    // widget is added to the temporary (left) section, so showMessage() hides it
    // the way it hides any QStatusBar widget, and clearing the message brings it
    // back. Visibility is deliberately left to the base class: a message and the
    // widgets it steps over are QStatusBar's own bookkeeping, and reformat() - not
    // virtual - would put a hidden segment back on the next resize.
    QObject::connect(action, &QAction::changed, this, [label, action] {
        label->setText(action->text());
    });
    addWidget(label);
    d->m_left.insert(action, label);
    return action;
}

QAction *RibbonStatusBar::addSeparator()
{
    Q_D(RibbonStatusBar);
    QFrame *frame = new QFrame(this);
    frame->setObjectName(QStringLiteral("qx_RibbonStatusBarSeparator"));
    frame->setFrameShape(QFrame::VLine);
    frame->setFrameShadow(QFrame::Plain);
    frame->setFixedWidth(statusBarSeparatorWidth);
    frame->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred);
    addWidget(frame);
    // A handle the host can keep to remove the separator; it carries no text.
    QAction *action = new QAction(this);
    action->setSeparator(true);
    d->m_left.insert(action, frame);
    return action;
}

void RibbonStatusBar::addPermanentAction(QAction *action)
{
    if (!action)
        return;
    Q_D(RibbonStatusBar);
    // The right, permanent section holds the view switches; the widget stays put
    // while showMessage() scrolls its text across the left, so a click reaches
    // the action the host owns through the button's default action.
    QToolButton *button = new QToolButton(this);
    button->setObjectName(QStringLiteral("qx_RibbonStatusBarPermanentButton"));
    button->setDefaultAction(action);
    button->setAutoRaise(true);
    button->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    addPermanentWidget(button);
    d->m_permanent.insert(action, button);
}

QX_RIBBON_END_NAMESPACE
