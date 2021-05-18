/***************************************************************************
 **
 **  Copyright (C) 2021 MaMinJie <canpool@163.com>
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
#include "fancyribboncontainers.h"

#include <QGridLayout>
#include <QHBoxLayout>
#include <QVBoxLayout>

#include <QAction>
#include <QToolButton>

QCANPOOL_BEGIN_NAMESPACE

FancyRibbonContainer::FancyRibbonContainer(QWidget *parent)
    : QWidget(parent)
{
}

FancyRibbonContainer::~FancyRibbonContainer()
{
}

/* FancyRibbonGridContainerPrivate */
class FancyRibbonGridContainerPrivate
{
public:
    FancyRibbonGridContainerPrivate();

    void init();

public:
    FancyRibbonGridContainer *q;
    QGridLayout *m_gridLayout;
};

FancyRibbonGridContainerPrivate::FancyRibbonGridContainerPrivate()
{}

void FancyRibbonGridContainerPrivate::init()
{
    m_gridLayout = new QGridLayout(q);
    m_gridLayout->setSpacing(1);
    m_gridLayout->setContentsMargins(2, 1, 2, 1);
    q->setLayout(m_gridLayout);
}

/* FancyRibbonGridContainer */
FancyRibbonGridContainer::FancyRibbonGridContainer(QWidget *parent)
    : FancyRibbonContainer(parent)
    , d(new FancyRibbonGridContainerPrivate)
{
    d->q = this;
    d->init();
}

FancyRibbonGridContainer::~FancyRibbonGridContainer()
{
    delete d;
}

void FancyRibbonGridContainer::addWidget(QWidget *widget)
{
    int cnt = d->m_gridLayout->count();
    int column = cnt / 3;
    int row = column ? (cnt % 3) : cnt;
    d->m_gridLayout->addWidget(widget, row, column, Qt::AlignLeft);
}

/* FancyRibbonActionContainerPrivate */
class FancyRibbonActionContainerPrivate
{
public:
    FancyRibbonActionContainerPrivate();

    void init();

    QAction *addAction(const QIcon &icon, const QString &text, FancyRibbonGroup::GroupSize size);
    void addAction(QAction *action, FancyRibbonGroup::GroupSize size); // for action that has menu

public:
    FancyRibbonActionContainer *q;
    QHBoxLayout *m_largeLayout;
    QVBoxLayout *m_mediumLayout;
    QVBoxLayout *m_smallLayout;
};

FancyRibbonActionContainerPrivate::FancyRibbonActionContainerPrivate()
{}

void FancyRibbonActionContainerPrivate::init()
{
    m_largeLayout = new QHBoxLayout();
    m_largeLayout->setSpacing(0);
    m_largeLayout->setContentsMargins(0, 0, 0, 0);

    m_mediumLayout = new QVBoxLayout();
    m_mediumLayout->setSpacing(0);
    m_mediumLayout->setContentsMargins(0, 0, 0, 0);

    m_smallLayout = new QVBoxLayout();
    m_smallLayout->setSpacing(0);
    m_smallLayout->setContentsMargins(0, 0, 0, 0);

    QHBoxLayout *mainLayout = new QHBoxLayout();
    mainLayout->setSpacing(0);
    mainLayout->setContentsMargins(1, 0, 1, 0);
    mainLayout->addLayout(m_largeLayout);
    mainLayout->addLayout(m_mediumLayout);
    mainLayout->addLayout(m_smallLayout);
    q->setLayout(mainLayout);
}

QAction *FancyRibbonActionContainerPrivate::addAction(const QIcon &icon, const QString &text,
                                                      FancyRibbonGroup::GroupSize size)
{
    QAction *action = new QAction(icon, text, q);
    addAction(action, size);
    return action;
}

void FancyRibbonActionContainerPrivate::addAction(QAction *action, FancyRibbonGroup::GroupSize size)
{
    QToolButton *button = new QToolButton(q);
    button->setAutoRaise(true);
    button->setDefaultAction(action);
    QBoxLayout *layout = nullptr;

    switch (size) {
        case FancyRibbonGroup::GroupLarge: {
            button->setIconSize(QSize(32, 32));
            button->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
            layout = m_largeLayout;
        }
        break;
        case FancyRibbonGroup::GroupMedium: {
            button->setIconSize(QSize(18, 18));
            button->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
            layout = m_mediumLayout;
        }
        break;
        default: {
            button->setIconSize(QSize(18, 18));
            button->setToolButtonStyle(Qt::ToolButtonIconOnly);
            layout = m_smallLayout;
        }
        break;
    }
    button->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);
    layout->addWidget(button);
    layout->setAlignment(button, Qt::AlignLeft);
}

/* FancyRibbonActionContainer */
FancyRibbonActionContainer::FancyRibbonActionContainer(QWidget *parent)
    : FancyRibbonContainer(parent)
    , d(new FancyRibbonActionContainerPrivate)
{
    d->q = this;
    d->init();
}

FancyRibbonActionContainer::~FancyRibbonActionContainer()
{
    delete d;
}

QAction *FancyRibbonActionContainer::addAction(const QIcon &icon, const QString &text,
                                               FancyRibbonGroup::GroupSize size)
{
    return d->addAction(icon, text, size);
}

void FancyRibbonActionContainer::addAction(QAction *action, FancyRibbonGroup::GroupSize size)
{
    return d->addAction(action, size);
}

QCANPOOL_END_NAMESPACE
