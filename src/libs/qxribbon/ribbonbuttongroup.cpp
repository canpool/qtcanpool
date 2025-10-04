/**
 * Copyleft (C) 2023 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MIT
**/
#include "ribbonbuttongroup.h"
#include "ribbongroup_p.h"
#include "ribbonbutton.h"

#include <QActionEvent>
#include <QHBoxLayout>
#include <QWidgetAction>

QX_RIBBON_BEGIN_NAMESPACE

/* RibbonButtonGroupPrivate */
class RibbonButtonGroupPrivate
{
    QX_DECLARE_PUBLIC(RibbonButtonGroup)
public:
    RibbonButtonGroupPrivate();

    void init();
public:
    QMap<QAction *, QWidget *> m_items;
};

RibbonButtonGroupPrivate::RibbonButtonGroupPrivate()
{
}

void RibbonButtonGroupPrivate::init()
{
    Q_Q(RibbonButtonGroup);
    QHBoxLayout *layout = new QHBoxLayout(q);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);
    q->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);
}

/* RibbonButtonGroup */
RibbonButtonGroup::RibbonButtonGroup(QWidget *parent)
    : QFrame(parent)
{
    QX_INIT_PRIVATE(RibbonButtonGroup)
    Q_D(RibbonButtonGroup);
    d->init();
}

RibbonButtonGroup::~RibbonButtonGroup()
{
    QX_FINI_PRIVATE()
}

QAction *RibbonButtonGroup::addAction(QAction *a)
{
    QWidget::addAction(a);
    return a;
}

QAction *RibbonButtonGroup::addAction(const QString &text, const QIcon &icon,
                                      QToolButton::ToolButtonPopupMode popMode)
{
    Q_D(RibbonButtonGroup);
    QAction *a = new QAction(icon, text, this);

    addAction(a);
    RibbonButton *btn = qobject_cast<RibbonButton *>(d->m_items.value(a));
    if (btn) {
        btn->setPopupMode(popMode);
    }
    return a;
}

QAction *RibbonButtonGroup::addMenu(QMenu *menu, QToolButton::ToolButtonPopupMode popMode)
{
    Q_D(RibbonButtonGroup);
    QAction *a = menu->menuAction();

    addAction(a);
    RibbonButton *btn = qobject_cast<RibbonButton *>(d->m_items.value(a));
    if (btn) {
        btn->setPopupMode(popMode);
    }
    return a;
}

QAction *RibbonButtonGroup::addSeparator()
{
    QAction *a = new QAction(this);

    a->setSeparator(true);
    addAction(a);
    return (a);
}

QAction *RibbonButtonGroup::addWidget(QWidget *w)
{
    QWidgetAction *a = new QWidgetAction(this);

    a->setDefaultWidget(w);
    w->setAttribute(Qt::WA_Hover);
    addAction(a);
    return a;
}

QSize RibbonButtonGroup::sizeHint() const
{
    return layout()->sizeHint();
}

QSize RibbonButtonGroup::minimumSizeHint() const
{
    return layout()->minimumSize();
}

void RibbonButtonGroup::actionEvent(QActionEvent *e)
{
    Q_D(RibbonButtonGroup);
    QAction *action = e->action();
    QWidgetAction *widgetAction = qobject_cast<QWidgetAction *>(action);
    QWidget *widget = nullptr;

    switch (e->type()) {
    case QEvent::ActionAdded: {
        if (widgetAction) {
            widget = widgetAction->requestWidget(this);
            if (widget != Q_NULLPTR) {
                widget->setAttribute(Qt::WA_LayoutUsesWidgetRect);
                widget->show();
            }
        } else if (action->isSeparator()) {
            RibbonSeparator *sp = new RibbonSeparator(this);
            sp->setTopBottomMargins(3, 3);
            widget = sp;
        }
        if (!widget) {
            RibbonButton *button = new RibbonButton(this);
            button->setAutoRaise(true);
            button->setFocusPolicy(Qt::NoFocus);
            button->setButtonType(RibbonButton::SmallButton);
            button->setToolButtonStyle(Qt::ToolButtonIconOnly);
            button->setDefaultAction(action);
            QObject::connect(button, &RibbonButton::triggered, this, &RibbonButtonGroup::actionTriggered);
            widget = button;
        }
        layout()->addWidget(widget);
        d->m_items.insert(action, widget);
    } break;
    case QEvent::ActionChanged: {
        layout()->invalidate();
    } break;
    case QEvent::ActionRemoved: {
        action->disconnect(this);
        widget = d->m_items.value(action);
        layout()->removeWidget(widget);
        d->m_items.remove(action);
        if (!widgetAction) {
            // destroy the RibbonButton/RibbonSeparator
            delete widget;
        }
        if (action->parentWidget() == this) {
            action->setParent(nullptr);
            delete action;
        }
        layout()->invalidate();
    } break;
    default:
        break;
    }
}

QX_RIBBON_END_NAMESPACE
