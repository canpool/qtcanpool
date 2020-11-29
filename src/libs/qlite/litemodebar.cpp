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
#include "litemodebar.h"

#include <QStackedWidget>
#include <QIcon>

QLITE_BEGIN_NAMESPACE

class LiteModeBarPrivate : public QObject
{
    Q_OBJECT
public:
    LiteModeBarPrivate();
    void init();

public:
    LiteModeBar *q;

    QStackedWidget *m_stack;
};

LiteModeBarPrivate::LiteModeBarPrivate()
    : q(nullptr), m_stack(nullptr)
{
}

void LiteModeBarPrivate::init()
{
    connect(q, SIGNAL(currentChanged(int)), m_stack, SLOT(setCurrentIndex(int)));
}

LiteModeBar::LiteModeBar(QStackedWidget *modeStack, QWidget *parent)
    : LiteTabBar(parent), d(new LiteModeBarPrivate())
{
    Q_ASSERT(modeStack);

    d->m_stack = modeStack;
    d->q = this;
    d->init();

    setObjectName(QLatin1String("qlite_modebar"));
}

LiteModeBar::~LiteModeBar()
{
    delete d;
}

int LiteModeBar::addMode(QWidget *widget, const QString &label)
{
    return insertMode(-1, widget, label);
}

int LiteModeBar::addMode(QWidget *widget, const QIcon &icon, const QString &label)
{
    return insertMode(-1, widget, icon, label);
}

int LiteModeBar::insertMode(int index, QWidget *widget, const QString &label)
{
    return insertMode(index, widget, QIcon(), label);
}

int LiteModeBar::insertMode(int index, QWidget *widget, const QIcon &icon, const QString &label)
{
    if(!widget)
        return -1;
    index = d->m_stack->insertWidget(index, widget);
    insertTab(index, icon, label);

    return index;
}

void LiteModeBar::removeMode(int index)
{
    if (QWidget *w = d->m_stack->widget(index)) {
        d->m_stack->removeWidget(w);
        removeTab(index);
    }
}

QWidget *LiteModeBar::currentMode() const
{
    return d->m_stack->currentWidget();
}

QWidget *LiteModeBar::Mode(int index) const
{
    return d->m_stack->widget(index);
}

int LiteModeBar::indexOf(QWidget *widget) const
{
    return d->m_stack->indexOf(widget);
}

void LiteModeBar::setModeEnabled(QWidget *widget, bool enable)
{
    int index = d->m_stack->indexOf(widget);
    setTabEnabled(index, enable);
}

void LiteModeBar::setModeVisible(QWidget *widget, bool visible)
{
    int index = d->m_stack->indexOf(widget);
    setTabVisible(index, visible);
}

QLITE_END_NAMESPACE

#include "litemodebar.moc"
