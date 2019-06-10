/***************************************************************************
 **
 **  Copyright (C) 2019 MaMinJie <canpool@163.com>
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
#include "litemodebar.h"

#include <QStackedWidget>
#include <QIcon>

using namespace QLite;

namespace QLite
{
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
}

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

#include "litemodebar.moc"
