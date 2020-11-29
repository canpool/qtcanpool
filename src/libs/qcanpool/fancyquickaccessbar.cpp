/***************************************************************************
 **
 **  Copyright (C) 2018-2020 MaMinJie <canpool@163.com>
 **  Contact: https://github.com/canpool
 **           https://gitee.com/icanpool
 **
 **  GNU Lesser General Public License Usage
 **  Alternatively, this file may be used under the terms of the GNU Lesser
 **  General Public License version 3 as published by the Free Software
 **  Foundation and appearing in the file LICENSE.LGPL3 included in the
 **  packaging of this file. Please review the following information to
 **  ensure the GNU Lesser General Public License version 3 requirements
 **  will be met: https://www.gnu.org/licenses/lgpl-3.0.html.
 **
 **  GNU General Public License Usage
 **  Alternatively, this file may be used under the terms of the GNU
 **  General Public License version 2.0 or (at your option) the GNU General
 **  Public license version 3 or any later version approved by the KDE Free
 **  Qt Foundation. The licenses are as published by the Free Software
 **  Foundation and appearing in the file LICENSE.GPL2 and LICENSE.GPL3
 **  included in the packaging of this file. Please review the following
 **  information to ensure the GNU General Public License requirements will
 **  be met: https://www.gnu.org/licenses/gpl-2.0.html and
 **  https://www.gnu.org/licenses/gpl-3.0.html.
 **
****************************************************************************/
#include "fancyquickaccessbar.h"

#include <QApplication>
#include <QStyleOption>
#include <QToolButton>
#include <QMenu>
#include <QPainter>
#include <QHBoxLayout>
#include <QMouseEvent>

#include "fancybar.h"
#include "fancybutton.h"

QCANPOOL_BEGIN_NAMESPACE

class FancyQuickAccessBarPrivate : public QWidget
{
    Q_OBJECT
public:
    FancyQuickAccessBarPrivate(QWidget *parent = nullptr);
    void init();

    void addAction(QAction *action);

public:
    FancyQuickAccessBar *q;

    FancyButton *m_accessButton;
    QMenu       *m_menu;
    bool        m_bAddEnable;
    QList<QAction *> m_checkActions;
    QList<QAction *> m_actions;
    QList<FancyButton *> m_actionButtons;

    QHBoxLayout *m_accessArea;

    QAction *m_customizeAction;
    QAction *m_separatorAction;

    FancyButton *m_menuButton;

private Q_SLOTS:
    void aboutToShowCustomizeMenu();
    void aboutToHideCustomizeMenu();
    void accessMenuTriggered(QMouseEvent *e);
    void menuTriggered(QMouseEvent *e);
    void aboutToShowMenu();
    void aboutToHideMenu();
};

FancyQuickAccessBarPrivate::FancyQuickAccessBarPrivate(QWidget *parent)
    : QWidget(parent)
{
    q = nullptr;
    m_accessButton = nullptr;
    m_menu = nullptr;
    m_bAddEnable = true;
    m_checkActions.clear();
    m_actions.clear();
    m_actionButtons.clear();
    m_accessArea = nullptr;
    m_customizeAction = nullptr;
    m_separatorAction = nullptr;
    m_menuButton = nullptr;
}

void FancyQuickAccessBarPrivate::init()
{
    m_accessButton = new FancyButton();
    m_accessButton->setHasBorder(false);
    m_accessButton->setHasMenu(true);
    m_accessButton->setIcon(QIcon(":/main/arrow"));
    m_accessButton->setToolTip(tr("Quick Access Menu"));
    connect(m_accessButton, SIGNAL(menuTriggered(QMouseEvent *)), this, SLOT(accessMenuTriggered(QMouseEvent *)));
    m_accessArea = new QHBoxLayout();
    m_accessArea->setContentsMargins(2, 0, 0, 0);
    m_accessArea->setSpacing(0);
    m_customizeAction = new QAction(tr("Customize Quick Access Bar"), this);
    m_customizeAction->setCheckable(false);
    m_customizeAction->setIconVisibleInMenu(false);
    m_customizeAction->setEnabled(false);
    m_menu = new QMenu(this);
    connect(m_menu, SIGNAL(aboutToShow()), this, SLOT(aboutToShowCustomizeMenu()));
    connect(m_menu, SIGNAL(aboutToHide()), this, SLOT(aboutToHideCustomizeMenu()));
    m_menu->addAction(m_customizeAction);
    m_separatorAction = m_menu->addSeparator();
    QHBoxLayout *layout = new QHBoxLayout();
    layout->setMargin(0);
    layout->setSpacing(0);
    layout->addLayout(m_accessArea);
    layout->addWidget(m_accessButton/*, 0, Qt::AlignTop*/);
    setLayout(layout);
}

void FancyQuickAccessBarPrivate::addAction(QAction *action)
{
    QAction *checkAction = new QAction(action->text(), this);
    FancyButton *button = new FancyButton();
    button->setHasBorder(false);
    button->setText(action->text());
    button->setToolTip(action->toolTip());
    button->setIcon(action->icon());
    button->setDefaultAction(action);
    connect(button, SIGNAL(menuTriggered(QMouseEvent *)), this, SLOT(menuTriggered(QMouseEvent *)));
    connect(button, SIGNAL(clicked(bool)), action, SIGNAL(triggered(bool)));
    checkAction->setCheckable(true);
    checkAction->setChecked(true);
    m_accessArea->addWidget(button/*, 0, Qt::AlignTop*/);
    m_menu->insertAction(m_separatorAction, checkAction);
    connect(checkAction, SIGNAL(toggled(bool)), button, SLOT(setVisible(bool)));
    m_actions.append(action);
    m_checkActions.append(checkAction);
    m_actionButtons.append(button);
}

void FancyQuickAccessBarPrivate::aboutToShowCustomizeMenu()
{
    m_accessButton->select(true);
}

void FancyQuickAccessBarPrivate::aboutToHideCustomizeMenu()
{
    m_accessButton->select(false);
}

void FancyQuickAccessBarPrivate::accessMenuTriggered(QMouseEvent *e)
{
    FancyButton *button = qobject_cast<FancyButton *>(sender());

    if (button == nullptr) {
        return;
    }

    int x = e->x();
    int y = e->y();
    QPoint pos = e->globalPos();
    pos.setX(pos.x() - x);
    pos.setY(pos.y() - y + button->height());
    m_menu->popup(pos);
}

void FancyQuickAccessBarPrivate::menuTriggered(QMouseEvent *e)
{
    FancyButton *button = qobject_cast<FancyButton *>(sender());
    if (button == nullptr) {
        return;
    }
    QAction *action = button->defaultAction();
    QMenu *menu = action->menu();
    if (menu == nullptr) {
        return;
    }
    connect(menu, SIGNAL(aboutToShow()), this, SLOT(aboutToShowMenu()));
    connect(menu, SIGNAL(aboutToHide()), this, SLOT(aboutToHideMenu()));
    m_menuButton = button;
    int x = e->x();
    int y = e->y();
    QPoint pos = e->globalPos();
    pos.setX(pos.x() - x);
    pos.setY(pos.y() - y + button->height());
    menu->popup(pos);
}

void FancyQuickAccessBarPrivate::aboutToShowMenu()
{
    if (m_menuButton) {
        m_menuButton->select(true);
    }
}

void FancyQuickAccessBarPrivate::aboutToHideMenu()
{
    if (m_menuButton) {
        m_menuButton->select(false);
    }
}

FancyQuickAccessBar::FancyQuickAccessBar(QWidget *parent)
    : QToolBar(parent), d(new FancyQuickAccessBarPrivate(this))
{
    d->q = this;
    d->init();
    this->setStyleSheet("QToolBar{border:none;background-color: transparent;margin:0px;}");
    addWidget(d);
}

QAction *FancyQuickAccessBar::actionCustomizeButton() const
{
    return d->m_customizeAction;
}

void FancyQuickAccessBar::setActionVisible(QAction *action, bool visible)
{
    int index = d->m_actions.indexOf(action);
    if (index == -1) {
        return;
    }
    d->m_checkActions.at(index)->setChecked(visible);
}

bool FancyQuickAccessBar::isActionVisible(QAction *action) const
{
    int index = d->m_actions.indexOf(action);
    if (index == -1) {
        return false;
    }
    return d->m_checkActions.at(index)->isChecked();
}

int FancyQuickAccessBar::visibleCount() const
{
    int cnt = 0;
    for (int i = 0; i < d->m_checkActions.count(); i++) {
        if (d->m_checkActions.at(i)->isChecked()) {
            cnt++;
        }
    }
    return cnt;
}

void FancyQuickAccessBar::setHoverColor(const QColor &color)
{
    foreach (FancyButton * button, d->m_actionButtons) {
        button->setHoverColor(color);
    }
}

void FancyQuickAccessBar::setPressColor(const QColor &color)
{
    foreach (FancyButton * button, d->m_actionButtons) {
        button->setPressColor(color);
    }
}

void FancyQuickAccessBar::setTextColor(const QColor &color)
{
    foreach (FancyButton * button, d->m_actionButtons) {
        button->setTextColor(color);
    }
}

void FancyQuickAccessBar::actionEvent(QActionEvent *event)
{
    if (d->m_bAddEnable) {
        d->m_bAddEnable = false;
        QToolBar::actionEvent(event);
        return;
    }

    switch (event->type()) {
        case QEvent::ActionAdded: {
            QAction *action = event->action();
            d->addAction(action);
        }
        break;
        default:
        break;
    }
}

QCANPOOL_END_NAMESPACE

#include "fancyquickaccessbar.moc"
