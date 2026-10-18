/**
 * Copyright (C) 2021-2023 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MIT
 **/
#include "ribbonquickaccessbar_p.h"
#include "ribbonquickaccessbar.h"
#include "ribbonutils.h"

#include <QToolButton>
#include <QAction>
#include <QCoreApplication>
#include <QStyle>
#include <QEvent>
#include <QActionEvent>
#include <QActionGroup>
#include <QLayout>

QX_RIBBON_BEGIN_NAMESPACE

RibbonQuickAccessButton::RibbonQuickAccessButton(QWidget *parent)
    : QToolButton(parent)
    , m_orientation(Qt::Horizontal)
{
    updateIcon();
}

RibbonQuickAccessButton::~RibbonQuickAccessButton()
{
}

/*!
 * The customize glyph is a mask painted in the palette's text colour, so it
 * inverts with the theme instead of the style sheet naming a frozen file.
 */
void RibbonQuickAccessButton::updateIcon()
{
    setIcon(tintedIcon(QStringLiteral(":/qxribbon/res/qaccess_button.png"), palette().color(QPalette::ButtonText),
                       iconSize()));
}

void RibbonQuickAccessButton::changeEvent(QEvent *e)
{
    if (e->type() == QEvent::PaletteChange) {
        updateIcon();
    }
    QToolButton::changeEvent(e);
}

QSize RibbonQuickAccessButton::sizeHint() const
{
    QSize sz = QToolButton::sizeHint();
    if (m_orientation == Qt::Horizontal) {
        return QSize(sz.width() / 2, sz.height());
    } else {
        return QSize(sz.width(), sz.height() / 2);
    }
}

void RibbonQuickAccessButton::setOrientation(Qt::Orientation orientation)
{
    m_orientation = orientation;
}

RibbonQuickAccessBarPrivate::RibbonQuickAccessBarPrivate()
    : m_menu(Q_NULLPTR)
    , m_actionAccessPopup(Q_NULLPTR)
    , m_customizeGroup(Q_NULLPTR)
    , m_customizeDialogAction(Q_NULLPTR)
    , m_accessPopup(Q_NULLPTR)
    , m_removingAction(false)
    , m_takingAction(false)
{
}

RibbonQuickAccessBarPrivate::~RibbonQuickAccessBarPrivate()
{
}

void RibbonQuickAccessBarPrivate::init()
{
    Q_Q(RibbonQuickAccessBar);
    q->setAutoFillBackground(false);
    q->setAttribute(Qt::WA_NoSystemBackground);
    q->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

    const int sz = q->style()->pixelMetric(QStyle::PM_SmallIconSize, Q_NULLPTR, q);
    q->setIconSize(QSize(sz, sz));

    m_accessPopup = new RibbonQuickAccessButton(q);
    m_accessPopup->setPopupMode(QToolButton::InstantPopup);
    QObject::connect(q, SIGNAL(orientationChanged(Qt::Orientation)), m_accessPopup,
                     SLOT(setOrientation(Qt::Orientation)));
    m_menu = new QMenu(q);
    m_accessPopup->setMenu(m_menu);

    m_actionAccessPopup = q->addWidget(m_accessPopup);

    m_customizeGroup = new QActionGroup(q);
    m_customizeGroup->setExclusive(false);
    connect(m_customizeGroup, SIGNAL(triggered(QAction *)), this, SLOT(customizeAction(QAction *)));

    // B8: the customize menu ends with the entry that opens the application's
    // customization dialog, as in Office. The action is owned by the bar, not by the
    // menu: the menu is cleared on every hide (aboutToHideCustomizeMenu), and clear()
    // deletes the actions the menu owns.
    m_customizeDialogAction =
        new QAction(QCoreApplication::translate("RibbonQuickAccessBar", "Customize Quick Access Toolbar..."), q);
    m_customizeDialogAction->setObjectName(QStringLiteral("qx_RibbonCustomizeQabAction"));
    QObject::connect(m_customizeDialogAction, &QAction::triggered, q, &RibbonQuickAccessBar::customizeDialogRequested);

    connect(m_menu, SIGNAL(aboutToShow()), this, SLOT(aboutToShowCustomizeMenu()));
    connect(m_menu, SIGNAL(aboutToHide()), this, SLOT(aboutToHideCustomizeMenu()));
}

QuickAccessAction *RibbonQuickAccessBarPrivate::findQuickAccessAction(QAction *action) const
{
    foreach (QAction *a, m_actionList) {
        QuickAccessAction *act = dynamic_cast<QuickAccessAction *>(a);
        if (act && action == act->m_srcAction) {
            return act;
        }
    }
    return Q_NULLPTR;
}

void RibbonQuickAccessBarPrivate::updateAction(QAction *action)
{
    if (QuickAccessAction *wrapper = findQuickAccessAction(action)) {
        wrapper->update();
    }
}

/*!
 * An item whose action carries a menu pops that menu up on a click, and carries no
 * menu triangle.
 *
 * The triangle is twelve pixels of a control twenty-two wide: the largest shape in
 * a row of 16-pixel glyphs, and far enough right of the glyph to read as a mark of
 * its own. A style sheet cannot drop it - ::menu-indicator takes an image, but a
 * sheet draws that image as it stands, so it cannot follow the palette, and a rule
 * that names only the sub-control is passed over while the platform style draws the
 * button. What does turn it off is InstantPopup: the style asks for an indicator on
 * a button that carries a menu *and* still triggers something of its own, and an
 * item here has nothing of its own to trigger - a click used to fire an action
 * whose only content is that menu. The one dropdown sign the row keeps is the
 * customize button at the end, which draws a chevron of its own
 * (RibbonQuickAccessButton paints it from a mask).
 */
void RibbonQuickAccessBarPrivate::updateActionPopupMode(QAction *action)
{
    Q_Q(RibbonQuickAccessBar);
    // The customize popup is a widget action with a menu on the button, not on the
    // action, and it is InstantPopup already; asking it to follow this rule would
    // turn it back into a delayed popup.
    if (Q_UNLIKELY(!action || action == m_actionAccessPopup)) {
        return;
    }
    QToolButton *button = qobject_cast<QToolButton *>(q->widgetForAction(action));
    if (Q_UNLIKELY(!button)) {
        return;
    }
    const QToolButton::ToolButtonPopupMode mode =
        action->menu() ? QToolButton::InstantPopup : QToolButton::DelayedPopup;
    if (button->popupMode() != mode) {
        button->setPopupMode(mode);
    }
}

void RibbonQuickAccessBarPrivate::setActionVisible(QAction *action, bool visible)
{
    if (QuickAccessAction *wrapper = findQuickAccessAction(action)) {
        if (wrapper->isChecked() == visible) {
            return;
        }
        setActionVisible(wrapper, action, visible);
    }
}

void RibbonQuickAccessBarPrivate::setActionVisible(QuickAccessAction *wrapper, QAction *action, bool visible)
{
    wrapper->setChecked(visible);
    setActionShown(wrapper, action, visible);
    wrapper->update();
}

void RibbonQuickAccessBarPrivate::setActionShown(QuickAccessAction *action, QAction *srcAction, bool visible)
{
    Q_Q(RibbonQuickAccessBar);
    bool actionInBar = (q->widgetForAction(srcAction) != NULL);
    bool actionIsVisible = srcAction->isVisible();

    if (visible) {
        if (actionInBar) {
            if (actionIsVisible) {
                return;
            }
            blockSignals(true);
            srcAction->setVisible(true);
            blockSignals(false);
            return;
        }
        QAction *before = nextSrcAction(action);
        if (before) {
            q->insertAction(before, srcAction);
        } else {
            q->addAction(srcAction);
        }
    } else {
        if (!actionInBar) {
            return;
        }
        m_takingAction = true;
        q->removeAction(srcAction);
    }
}

QAction *RibbonQuickAccessBarPrivate::nextSrcAction(QAction *action)
{
    Q_Q(RibbonQuickAccessBar);
    bool flag = false;
    foreach (QAction *act, m_actionList) {
        if (flag) {
            QuickAccessAction *a = dynamic_cast<QuickAccessAction *>(act);
            // wrapper action isChecked or srcAction inBar
            if (a->isChecked() || (q->widgetForAction(a->m_srcAction) != NULL)) {
                return a->m_srcAction;
            }
        } else if (act == action) {
            flag = true;
        }
    }
    return NULL;
}

void RibbonQuickAccessBarPrivate::customizeAction(QAction *action)
{
    Q_Q(RibbonQuickAccessBar);
    if (QuickAccessAction *act = dynamic_cast<QuickAccessAction *>(action)) {
        setActionShown(act, act->m_srcAction, act->isChecked());
        Q_EMIT q->customizeActionChanged();
    }
}

void RibbonQuickAccessBarPrivate::aboutToShowCustomizeMenu()
{
    Q_Q(RibbonQuickAccessBar);
    m_menu->clear();
    foreach (QAction *action, m_actionList) {
        m_menu->addAction(action);
    }
    m_menu->addSeparator();
    m_menu->addAction(m_customizeDialogAction);
    Q_EMIT q->showCustomizeMenu(m_menu);
}

void RibbonQuickAccessBarPrivate::aboutToHideCustomizeMenu()
{
    m_menu->clear();
}

RibbonQuickAccessBar::RibbonQuickAccessBar(QWidget *parent)
    : QToolBar(parent)
{
    QX_INIT_PRIVATE(RibbonQuickAccessBar)
    Q_D(RibbonQuickAccessBar);
    d->init();
    layout()->setSizeConstraint(QLayout::SetFixedSize);
}

RibbonQuickAccessBar::~RibbonQuickAccessBar(){QX_FINI_PRIVATE()}

QAction *RibbonQuickAccessBar::actionCustomizeButton() const
{
    Q_D(const RibbonQuickAccessBar);
    return d->m_actionAccessPopup;
}

QAction *RibbonQuickAccessBar::actionCustomizeDialog() const
{
    Q_D(const RibbonQuickAccessBar);
    return d->m_customizeDialogAction;
}

void RibbonQuickAccessBar::setActionVisible(QAction *action, bool visible)
{
    Q_D(RibbonQuickAccessBar);
    d->setActionVisible(action, visible);
}

bool RibbonQuickAccessBar::isActionVisible(QAction *action) const
{
    Q_D(const RibbonQuickAccessBar);
    if (QuickAccessAction *wrapper = d->findQuickAccessAction(action)) {
        return wrapper->isChecked();
    }
    return false;
}

int RibbonQuickAccessBar::visibleCount() const
{
    Q_D(const RibbonQuickAccessBar);
    int visibleCount = 0;
    QList<QAction *> list = d->m_customizeGroup->actions();
    for (int i = 0, count = list.count(); i < count; ++i) {
        if (QuickAccessAction *act = dynamic_cast<QuickAccessAction *>(list[i])) {
            if (act->isChecked()) {
                visibleCount++;
            }
        }
    }
    return visibleCount;
}

QByteArray RibbonQuickAccessBar::state() const
{
    Q_D(const RibbonQuickAccessBar);
    QByteArray s;
    foreach (QAction *act, d->m_actionList) {
        s.append(act->isChecked() ? '1' : '0');
    }
    return s;
}

void RibbonQuickAccessBar::setState(const QByteArray &s)
{
    if (s.isEmpty()) {
        return;
    }
    Q_D(RibbonQuickAccessBar);
    int cnt = static_cast<int>(s.size());
    int j = 0;
    foreach (QAction *action, d->m_actionList) {
        if (QuickAccessAction *act = dynamic_cast<QuickAccessAction *>(action)) {
            if (j < cnt) {
                if (s.at(j) == '1' && !act->isChecked()) {
                    d->setActionVisible(act, act->m_srcAction, true);
                } else if (s.at(j) == '0' && act->isChecked()) {
                    d->setActionVisible(act, act->m_srcAction, false);
                }
            }
        }
        ++j;
    }
}

bool RibbonQuickAccessBar::event(QEvent *event)
{
    if (parentWidget() && (event->type() == QEvent::Hide || event->type() == QEvent::Show)) {
        adjustSize();
    } else if (event->type() == QEvent::StyleChange) {
        const int sz = style()->pixelMetric(QStyle::PM_SmallIconSize, Q_NULLPTR, this);
        setIconSize(QSize(sz, sz));
    }
    return QToolBar::event(event);
}

void RibbonQuickAccessBar::actionEvent(QActionEvent *event)
{
    Q_D(RibbonQuickAccessBar);
    QToolBar::actionEvent(event);
    if (d->m_actionAccessPopup) {
        if (event->type() == QEvent::ActionAdded) {
            if (!d->m_removingAction) {
                // remove, then add back in QEvent::ActionRemoved
                removeAction(d->m_actionAccessPopup);
            }
            QuickAccessAction *quickAccessAction = d->findQuickAccessAction(event->action());
            if (event->action() != d->m_actionAccessPopup && !quickAccessAction) {
                QuickAccessAction *act = new QuickAccessAction(this, event->action());
                d->m_actionList.append(act);
                d->m_customizeGroup->addAction(act);
                adjustSize();
            } else if (quickAccessAction) {
                quickAccessAction->update();
            }
            d->updateActionPopupMode(event->action());
        } else if (event->type() == QEvent::ActionRemoved) {
            if (event->action() == d->m_actionAccessPopup) {
                d->m_removingAction = true;
                addAction(d->m_actionAccessPopup);
                d->m_removingAction = false;
            } else {
                if (d->m_takingAction) {
                    d->m_takingAction = false;
                    return;
                }
                QuickAccessAction *act = d->findQuickAccessAction(event->action());
                if (act) {
                    d->m_actionList.removeOne(act);
                    d->m_customizeGroup->removeAction(act);
                    delete act;
                }
            }
        } else if (event->type() == QEvent::ActionChanged) {
            if (event->action() == d->m_actionAccessPopup) {
                d->m_accessPopup->setDefaultAction(d->m_actionAccessPopup);
            } else if (!dynamic_cast<QuickAccessAction *>(event->action())) {
                d->updateAction(event->action());
            }
            // The menu may arrive after the action does - setMenu() on an action that
            // is already in the bar - and that is the change that decides the popup
            // mode.
            d->updateActionPopupMode(event->action());
        }
    }
}

QX_RIBBON_END_NAMESPACE
