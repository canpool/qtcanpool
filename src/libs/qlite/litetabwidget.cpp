/***************************************************************************
 **
 **  Copyright (C) 2019-2020 MaMinJie <canpool@163.com>
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
#include "litetabwidget.h"

#include <QStackedWidget>
#include <QBoxLayout>
#include <QIcon>

#include "litetabbar.h"

QLITE_BEGIN_NAMESPACE

class LiteTabWidgetPrivate : public QObject
{
    Q_OBJECT
public:
    LiteTabWidgetPrivate();
    void init();
    void updateTabBarPosition();


    LiteTabBar *m_tabBar;
    QStackedWidget *m_stack;
    QBoxLayout *m_layout;

    LiteTabWidget *q;
    LiteTabWidget::TabPosition m_pos;
};

LiteTabWidgetPrivate::LiteTabWidgetPrivate()
    : m_tabBar(nullptr), m_stack(nullptr),
      m_layout(nullptr), q(nullptr),
      m_pos(LiteTabWidget::North)
{
}

void LiteTabWidgetPrivate::init()
{
    m_tabBar = new LiteTabBar(q);
    m_tabBar->setObjectName(QLatin1String("qlite_tabwidget_tabbar"));

    m_stack = new QStackedWidget(q);
    m_stack->setObjectName(QLatin1String("qlite_tabwidget_stackedwidget"));
    m_stack->setLineWidth(0);

    m_layout = new QBoxLayout(QBoxLayout::TopToBottom);
    m_layout->setMargin(0);
    m_layout->setSpacing(1);
    m_layout->addWidget(m_tabBar);
    m_layout->addWidget(m_stack);
    q->setLayout(m_layout);

    q->setObjectName(QLatin1String("qlite_tabwidget"));

    connect(m_tabBar, SIGNAL(currentChanged(int)), m_stack, SLOT(setCurrentIndex(int)));
}

void LiteTabWidgetPrivate::updateTabBarPosition()
{
    switch (m_pos) {
        case LiteTabWidget::North: {
            m_layout->setDirection(QBoxLayout::TopToBottom);
            m_tabBar->setDirection(LiteTabBar::Horizontal);
            break;
        }
        case LiteTabWidget::South: {
            m_layout->setDirection(QBoxLayout::BottomToTop);
            m_tabBar->setDirection(LiteTabBar::Horizontal);
            break;
        }
        case LiteTabWidget::West: {
            m_layout->setDirection(QBoxLayout::LeftToRight);
            m_tabBar->setDirection(LiteTabBar::Vertical);
            break;
        }
        case LiteTabWidget::East: {
            m_layout->setDirection(QBoxLayout::RightToLeft);
            m_tabBar->setDirection(LiteTabBar::Vertical);
            break;
        }
    }
}

LiteTabWidget::LiteTabWidget(QWidget *parent)
    : LiteWidget(parent), d(new LiteTabWidgetPrivate())
{
    d->q = this;
    d->init();
}

LiteTabWidget::~LiteTabWidget()
{
    delete d;
}

int LiteTabWidget::addTab(QWidget *widget, const QString &label)
{
    return insertTab(-1, widget, label);
}

int LiteTabWidget::addTab(QWidget *widget, const QIcon &icon, const QString &label)
{
    return insertTab(-1, widget, icon, label);
}

int LiteTabWidget::insertTab(int index, QWidget *widget, const QString &label)
{
    return insertTab(index, widget, QIcon(), label);
}

int LiteTabWidget::insertTab(int index, QWidget *widget, const QIcon &icon, const QString &label)
{
    if(!widget)
        return -1;
    index = d->m_stack->insertWidget(index, widget);
    d->m_tabBar->insertTab(index, icon, label);

    return index;
}

void LiteTabWidget::removeTab(int index)
{
    if (QWidget *w = d->m_stack->widget(index)) {
        d->m_stack->removeWidget(w);
        d->m_tabBar->removeTab(index);
    }
}

int LiteTabWidget::currentIndex() const
{
    return d->m_stack->currentIndex();
}

QWidget *LiteTabWidget::currentWidget() const
{
    return d->m_stack->currentWidget();
}

QWidget *LiteTabWidget::widget(int index) const
{
    return d->m_stack->widget(index);
}

int LiteTabWidget::indexOf(QWidget *widget) const
{
    return d->m_stack->indexOf(widget);
}

int LiteTabWidget::count() const
{
    return d->m_tabBar->count();
}

void LiteTabWidget::setTabEnabled(QWidget *widget, bool enable)
{
    int index = d->m_stack->indexOf(widget);
    d->m_tabBar->setTabEnabled(index, enable);
}

void LiteTabWidget::setTabVisible(QWidget *widget, bool visible)
{
    int index = d->m_stack->indexOf(widget);
    d->m_tabBar->setTabVisible(index, visible);
}

LiteTabBar *LiteTabWidget::tabBar() const
{
    return d->m_tabBar;
}

LiteTabWidget::TabPosition LiteTabWidget::tabPosition() const
{
    return d->m_pos;
}

void LiteTabWidget::setTabPosition(LiteTabWidget::TabPosition pos)
{
    if (d->m_pos == pos)
        return;
    d->m_pos = pos;
    d->updateTabBarPosition();
}

void LiteTabWidget::setTabButtonStyle(Qt::ToolButtonStyle style)
{
    d->m_tabBar->setTabButtonStyle(style);
}

void LiteTabWidget::setTabSpacing(int spacing)
{
    d->m_tabBar->setTabSpacing(spacing);
}

QLITE_END_NAMESPACE

#include "litetabwidget.moc"
