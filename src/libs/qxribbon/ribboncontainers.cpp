﻿/**
 * Copyleft (C) 2023 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MIT
**/
#include "ribboncontainers.h"
#include "ribbonmenu.h"

#include <QDebug>
#include <QBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPaintEvent>
#include <QPainter>
#include <QStylePainter>

#include <QGridLayout>
#include <QActionGroup>
#include <QToolButton>

QX_RIBBON_BEGIN_NAMESPACE

class RibbonCtrlContainerPrivate
{
    QX_DECLARE_PUBLIC(RibbonCtrlContainer)
public:
    RibbonCtrlContainerPrivate();

    void init();
    void setWidget(QWidget *w);
    void takeWidget(QWidget *w);
public:
    QWidget *m_widget;
    QBoxLayout *m_layout;
    QLabel *m_labelPixmap;
    QLabel *m_labelText;
    QIcon m_icon;
    QSize m_iconSize;
    Qt::Orientation m_orientation;
};


RibbonCtrlContainerPrivate::RibbonCtrlContainerPrivate()
    : m_widget(Q_NULLPTR)
    , m_iconSize(18, 18)
    , m_orientation(Qt::Horizontal)
{

}

void RibbonCtrlContainerPrivate::init()
{
    Q_Q(RibbonCtrlContainer);
    QHBoxLayout *headerLayout = new QHBoxLayout();
    headerLayout->setSpacing(2);
    headerLayout->setContentsMargins(0, 0, 0, 0);

    m_labelPixmap = new QLabel(q);
    m_labelPixmap->setObjectName(QLatin1String("logo"));
    m_labelPixmap->setAlignment(Qt::AlignCenter);
    headerLayout->addWidget(m_labelPixmap);

    m_labelText = new QLabel(q);
    m_labelText->setObjectName(QLatin1String("title"));
    headerLayout->addWidget(m_labelText);

    QBoxLayout::Direction direction =
        m_orientation == Qt::Horizontal ? QBoxLayout::LeftToRight : QBoxLayout::TopToBottom;
    m_layout = new QBoxLayout(direction, q);
    m_layout->setSpacing(2);
    m_layout->setContentsMargins(0, 0, 0, 0);
    m_layout->addLayout(headerLayout);
}

void RibbonCtrlContainerPrivate::setWidget(QWidget *w)
{
    Q_Q(RibbonCtrlContainer);
    if (m_widget) {
        QWidget *oldwidget = m_widget;
        takeWidget(oldwidget);
        oldwidget->deleteLater();
    }
    m_widget = w;
    if (!w) {
        return;
    }
    if (w->parent() != q) {
        w->setParent(q);
    }
    m_layout->addWidget(w);
    QSizePolicy sizePolicy(m_orientation == Qt::Horizontal ? QSizePolicy::Expanding : QSizePolicy::Preferred,
                           QSizePolicy::Preferred);
    sizePolicy.setHorizontalStretch(0);
    sizePolicy.setVerticalStretch(0);
    w->setSizePolicy(sizePolicy);
}

void RibbonCtrlContainerPrivate::takeWidget(QWidget *w)
{
    int i = m_layout->indexOf(w);
    if (i >= 0) {
        QLayoutItem *item = m_layout->takeAt(i);
        delete item;
    }
}

RibbonCtrlContainer::RibbonCtrlContainer(QWidget *parent)
    : RibbonCtrlContainer(Qt::Horizontal, parent)
{
}

RibbonCtrlContainer::RibbonCtrlContainer(Qt::Orientation orientation, QWidget *parent)
    : QWidget(parent)
{
    QX_INIT_PRIVATE(RibbonCtrlContainer)
    Q_D(RibbonCtrlContainer);
    d->m_orientation = orientation;
    d->init();
}

RibbonCtrlContainer::~RibbonCtrlContainer()
{
    QX_FINI_PRIVATE()
}

void RibbonCtrlContainer::setIconVisible(bool b)
{
    Q_D(RibbonCtrlContainer);
    d->m_labelPixmap->setVisible(b);
}

void RibbonCtrlContainer::setTitleVisible(bool b)
{
    Q_D(RibbonCtrlContainer);
    d->m_labelText->setVisible(b);
}

void RibbonCtrlContainer::setIcon(const QIcon &i)
{
    Q_D(RibbonCtrlContainer);
    d->m_icon = i;
    d->m_labelPixmap->setPixmap(i.pixmap(d->m_iconSize));
}

QIcon RibbonCtrlContainer::icon() const
{
    Q_D(const RibbonCtrlContainer);
    return d->m_icon;
}

void RibbonCtrlContainer::setIconSize(const QSize &size)
{
    Q_D(RibbonCtrlContainer);
    d->m_iconSize = size;
    setIcon(icon());
}

QSize RibbonCtrlContainer::iconSize() const
{
    Q_D(const RibbonCtrlContainer);
    return d->m_iconSize;
}

void RibbonCtrlContainer::setText(const QString &t)
{
    Q_D(RibbonCtrlContainer);
    d->m_labelText->setText(t);
}

QString RibbonCtrlContainer::text() const
{
    Q_D(const RibbonCtrlContainer);
    return d->m_labelText->text();
}

void RibbonCtrlContainer::setWidget(QWidget *w)
{
    Q_D(RibbonCtrlContainer);
    d->setWidget(w);
}

QWidget *RibbonCtrlContainer::widget() const
{
    Q_D(const RibbonCtrlContainer);
    return d->m_widget;
}

Qt::Orientation RibbonCtrlContainer::orientation() const
{
    Q_D(const RibbonCtrlContainer);
    return d->m_orientation;
}

void RibbonCtrlContainer::setOrientation(Qt::Orientation orientation)
{
    Q_D(RibbonCtrlContainer);
    if (orientation == d->m_orientation) {
        return;
    }
    d->m_orientation = orientation;
    if (orientation == Qt::Horizontal) {
        d->m_layout->setDirection(QBoxLayout::LeftToRight);
        d->m_labelPixmap->setAlignment(Qt::AlignCenter);
    } else {
        d->m_layout->setDirection(QBoxLayout::TopToBottom);
        d->m_labelPixmap->setAlignment(Qt::AlignLeft);
    }
}

/* RibbonGridContainerPrivate */
class RibbonGridContainerPrivate
{
    QX_DECLARE_PUBLIC(RibbonGridContainer)
public:
    RibbonGridContainerPrivate();

    void init();
    void updateLayout();
    void addWidget(QWidget *widget);

public:
    QGridLayout *m_gridLayout;
    QActionGroup *m_actionGroup;
    int m_columnCount = 5;
};

RibbonGridContainerPrivate::RibbonGridContainerPrivate()
{}

void RibbonGridContainerPrivate::init()
{
    Q_Q(RibbonGridContainer);

    m_gridLayout = new QGridLayout(q);
    m_gridLayout->setSpacing(0);
    m_gridLayout->setContentsMargins(2, 1, 2, 1);
    q->setLayout(m_gridLayout);

    m_actionGroup = new QActionGroup(q);
    QObject::connect(m_actionGroup, &QActionGroup::triggered, q, [q]() {
        QWidget *parWidget = q->parentWidget();
        while (parWidget) {
            if (RibbonMenu *menu = qobject_cast<RibbonMenu*>(parWidget)) {
                menu->hide();
                break;
            }
            parWidget = parWidget->parentWidget();
        }
    });
}

void RibbonGridContainerPrivate::updateLayout()
{

}

void RibbonGridContainerPrivate::addWidget(QWidget *widget)
{
    int cnt = m_gridLayout->count();
    int row = cnt / m_columnCount;
    int column = row ? (cnt % m_columnCount) : cnt;
    m_gridLayout->addWidget(widget, row, column, Qt::AlignLeft);
}

/* RibbonGridContainer */
RibbonGridContainer::RibbonGridContainer(QWidget *parent)
    : QWidget(parent)
{
    QX_INIT_PRIVATE(RibbonGridContainer);
    Q_D(RibbonGridContainer);
    d->init();
}

RibbonGridContainer::~RibbonGridContainer()
{
    QX_FINI_PRIVATE();
}

QAction *RibbonGridContainer::addAction(const QIcon &icon, const QString &text)
{
    Q_D(RibbonGridContainer);
    QAction *action = new QAction(icon, text, this);
    QToolButton *btn = new QToolButton(this);
    btn->setAutoRaise(true);
    btn->setFocusPolicy(Qt::NoFocus);
    btn->setDefaultAction(action);
    d->m_actionGroup->addAction(action);
    d->addWidget(btn);
    return action;
}

void RibbonGridContainer::setColumnCount(int count)
{
    Q_D(RibbonGridContainer);
    if (count < 3) {
        count = 3;
    }
    if (d->m_columnCount == count) {
        return;
    }
    d->m_columnCount = count;
    d->updateLayout();
}

int RibbonGridContainer::columnCount() const
{
    Q_D(const RibbonGridContainer);
    return d->m_columnCount;
}

QX_RIBBON_END_NAMESPACE
