/**
 * Copyright (C) 2020-2022 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
**/
#include "ribboncontainers.h"

#include <QGridLayout>
#include <QHBoxLayout>
#include <QVBoxLayout>

#include <QAction>
#include <QToolButton>

QCANPOOL_BEGIN_NAMESPACE

RibbonContainer::RibbonContainer(QWidget *parent)
    : QWidget(parent)
{
}

RibbonContainer::~RibbonContainer()
{
}

/* RibbonGridContainerPrivate */
class RibbonGridContainerPrivate
{
public:
    RibbonGridContainerPrivate();

    void init();

public:
    RibbonGridContainer *q;
    QGridLayout *m_gridLayout;
};

RibbonGridContainerPrivate::RibbonGridContainerPrivate()
{}

void RibbonGridContainerPrivate::init()
{
    m_gridLayout = new QGridLayout(q);
    m_gridLayout->setSpacing(1);
    m_gridLayout->setContentsMargins(2, 1, 2, 1);
    q->setLayout(m_gridLayout);
}

/* RibbonGridContainer */
RibbonGridContainer::RibbonGridContainer(QWidget *parent)
    : RibbonContainer(parent)
    , d(new RibbonGridContainerPrivate)
{
    d->q = this;
    d->init();
}

RibbonGridContainer::~RibbonGridContainer()
{
    delete d;
}

void RibbonGridContainer::addWidget(QWidget *widget)
{
    int cnt = d->m_gridLayout->count();
    int column = cnt / 3;
    int row = column ? (cnt % 3) : cnt;
    d->m_gridLayout->addWidget(widget, row, column, Qt::AlignLeft);
}

/* RibbonActionContainerPrivate */
class RibbonActionContainerPrivate
{
public:
    RibbonActionContainerPrivate();

    void init();

    QAction *addAction(const QIcon &icon, const QString &text, RibbonGroup::GroupSize size);
    void addAction(QAction *action, RibbonGroup::GroupSize size); // for action that has menu

public:
    RibbonActionContainer *q;
    QHBoxLayout *m_largeLayout;
    QVBoxLayout *m_mediumLayout;
    QVBoxLayout *m_smallLayout;
};

RibbonActionContainerPrivate::RibbonActionContainerPrivate()
{}

void RibbonActionContainerPrivate::init()
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

QAction *RibbonActionContainerPrivate::addAction(const QIcon &icon, const QString &text,
                                                      RibbonGroup::GroupSize size)
{
    QAction *action = new QAction(icon, text, q);
    addAction(action, size);
    return action;
}

void RibbonActionContainerPrivate::addAction(QAction *action, RibbonGroup::GroupSize size)
{
    QToolButton *button = new QToolButton(q);
    button->setAutoRaise(true);
    button->setDefaultAction(action);
    QBoxLayout *layout = nullptr;

    switch (size) {
        case RibbonGroup::GroupLarge: {
            button->setIconSize(QSize(32, 32));
            button->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
            layout = m_largeLayout;
        }
        break;
        case RibbonGroup::GroupMedium: {
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

/* RibbonActionContainer */
RibbonActionContainer::RibbonActionContainer(QWidget *parent)
    : RibbonContainer(parent)
    , d(new RibbonActionContainerPrivate)
{
    d->q = this;
    d->init();
}

RibbonActionContainer::~RibbonActionContainer()
{
    delete d;
}

QAction *RibbonActionContainer::addAction(const QIcon &icon, const QString &text,
                                               RibbonGroup::GroupSize size)
{
    return d->addAction(icon, text, size);
}

void RibbonActionContainer::addAction(QAction *action, RibbonGroup::GroupSize size)
{
    return d->addAction(action, size);
}

QCANPOOL_END_NAMESPACE
