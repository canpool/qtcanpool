/***************************************************************************
 **
 **  Copyright (C) 2018 MaMinJie <canpool@163.com>
 **  Contact: https://github.com/canpool
 **
 **  This program is free software: you can redistribute it and/or modify
 **  it under the terms of the GNU General Public License as published by
 **  the Free Software Foundation, either version 3 of the License, or
 **  (at your option) any later version.
 **
 **  This program is distributed in the hope that it will be useful,
 **  but WITHOUT ANY WARRANTY; without even the implied warranty of
 **  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 **  GNU General Public License for more details.
 **
 **  You should have received a copy of the GNU General Public License
 **  along with this program.  If not, see http://www.gnu.org/licenses/.
 **
****************************************************************************/
#include "quickaccessbar.h"

#include <QApplication>
#include <QStyleOption>
#include <QToolButton>
#include <QMenu>
#include <QPainter>
#include <QHBoxLayout>
#include <QMouseEvent>

#include "fancybar.h"
#include "fancybutton.h"

class QuickAccessBarPrivate : public QWidget
{
    Q_OBJECT
public:
    QuickAccessBarPrivate(QWidget *parent = 0);
    void init();

    void addAction(QAction *action);

public:
    QuickAccessBar *q;

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

QuickAccessBarPrivate::QuickAccessBarPrivate(QWidget *parent)
    : QWidget(parent)
{
    q = 0;
    m_accessButton = 0;
    m_menu = 0;
    m_bAddEnable = true;

    m_checkActions.clear();
    m_actions.clear();
    m_actionButtons.clear();

    m_accessArea = 0;

    m_customizeAction = 0;
    m_separatorAction = 0;

    m_menuButton = 0;
}

void QuickAccessBarPrivate::init()
{
    m_accessButton = new FancyButton();
    m_accessButton->setHasBorder(false);
    m_accessButton->setHasMenu(true);
    m_accessButton->setIcon(QIcon(":/main/arrow"));
    m_accessButton->setToolTip(tr("Quick Access Menu"));
    connect(m_accessButton, SIGNAL(menuTriggered(QMouseEvent*)), this, SLOT(accessMenuTriggered(QMouseEvent*)));

    m_accessArea = new QHBoxLayout();
    m_accessArea->setContentsMargins(2,0,0,0);
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

void QuickAccessBarPrivate::addAction(QAction *action)
{
    QAction *checkAction = new QAction(action->text(), this);
    FancyButton *button = new FancyButton();
    button->setHasBorder(false);
    button->setText(action->text());
    button->setToolTip(action->toolTip());
    button->setIcon(action->icon());
    button->setDefaultAction(action);
    connect(button, SIGNAL(menuTriggered(QMouseEvent*)), this, SLOT(menuTriggered(QMouseEvent*)));
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

void QuickAccessBarPrivate::aboutToShowCustomizeMenu()
{
    m_accessButton->select(true);
}

void QuickAccessBarPrivate::aboutToHideCustomizeMenu()
{
    m_accessButton->select(false);
}

void QuickAccessBarPrivate::accessMenuTriggered(QMouseEvent *e)
{
    FancyButton *button = qobject_cast<FancyButton*>(sender());
    if(button == 0) return;

    int x = e->x();
    int y = e->y();
    QPoint pos = e->globalPos();
    pos.setX(pos.x()-x);
    pos.setY(pos.y()-y+button->height());
//    emit q->menuTriggered(index, pos);
    m_menu->popup(pos);
}

void QuickAccessBarPrivate::menuTriggered(QMouseEvent *e)
{
    FancyButton *button = qobject_cast<FancyButton*>(sender());
    if(button == 0) return;
    QAction *action = button->defaultAction();
    QMenu *menu = action->menu();
    if(menu == 0) return;

    connect(menu, SIGNAL(aboutToShow()), this, SLOT(aboutToShowMenu()));
    connect(menu, SIGNAL(aboutToHide()), this, SLOT(aboutToHideMenu()));
    m_menuButton = button;

    int x = e->x();
    int y = e->y();
    QPoint pos = e->globalPos();
    pos.setX(pos.x()-x);
    pos.setY(pos.y()-y+button->height());
    menu->popup(pos);
}

void QuickAccessBarPrivate::aboutToShowMenu()
{
    if(m_menuButton){
        m_menuButton->select(true);
    }
}

void QuickAccessBarPrivate::aboutToHideMenu()
{
    if(m_menuButton){
        m_menuButton->select(false);
    }
}

QuickAccessBar::QuickAccessBar(QWidget *parent)
    : QToolBar(parent),d(new QuickAccessBarPrivate(this))
{
    d->q = this;
    d->init();

    this->setStyleSheet("QToolBar{border:none;background-color: transparent;margin:0px;}");

    addWidget(d);
}

QAction *QuickAccessBar::actionCustomizeButton() const
{
    return d->m_customizeAction;
}

void QuickAccessBar::setActionVisible(QAction *action, bool visible)
{
    int index = d->m_actions.indexOf(action);
    if(index == -1)return;
    d->m_checkActions.at(index)->setChecked(visible);
}

bool QuickAccessBar::isActionVisible(QAction *action) const
{
    int index = d->m_actions.indexOf(action);
    if(index == -1){
        return false;
    }
    return d->m_checkActions.at(index)->isChecked();
}

int QuickAccessBar::visibleCount() const
{
    int cnt = 0;
    for(int i = 0; i < d->m_checkActions.count(); i++){
        if(d->m_checkActions.at(i)->isChecked()){
            cnt++;
        }
    }
    return cnt;
}

void QuickAccessBar::setHoverColor(const QColor &color)
{
    foreach (FancyButton *button, d->m_actionButtons) {
        button->setHoverColor(color);
    }
}

void QuickAccessBar::setPressColor(const QColor &color)
{
    foreach (FancyButton *button, d->m_actionButtons) {
        button->setPressColor(color);
    }
}

void QuickAccessBar::setTextColor(const QColor &color)
{
    foreach (FancyButton *button, d->m_actionButtons) {
        button->setTextColor(color);
    }
}

void QuickAccessBar::actionEvent(QActionEvent *event)
{
    if(d->m_bAddEnable){
        d->m_bAddEnable = false;
        QToolBar::actionEvent(event);
        return;
    }

    switch (event->type())
    {
    case QEvent::ActionAdded:
    {
        QAction *action = event->action();
        d->addAction(action);
    }break;
    default:
        break;
    }

}

#include "quickaccessbar.moc"
