/**
 * Copyright (C) 2018-2022 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
**/
#ifndef FANCYTABBARPRIVATE_H
#define FANCYTABBARPRIVATE_H

//
// W A R N I N G
// -------------
//
// This file is not part of the Qt API. It exists purely as an
// implementation detail. This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#include "qcanpool.h"
#include <QObject>
#include <QMap>
#include <QSize>
#include <QBoxLayout>

class QToolButton;
class QBoxLayout;
class QAction;

QCANPOOL_BEGIN_NAMESPACE

class FancyTabBar;

class FancyTabBarPrivate : public QObject
{
    Q_OBJECT

public:
    FancyTabBarPrivate();
    ~FancyTabBarPrivate();

    void init();

    QToolButton *at(int index);

    inline bool validIndex(int index) const { return index >= 0 && index < m_tabs.count(); }
    void setIconSize(QSize size);

    inline QBoxLayout::Direction layoutDirection() const {
        return m_orientation == Qt::Horizontal ? QBoxLayout::LeftToRight : QBoxLayout::TopToBottom;
    }

public slots:
    void switchTab(bool checked);
    void pressTab();

public:
    QList<QToolButton *> m_tabs;
    QList<QToolButton *> m_actionButtons;
    QMap<QAction *, QToolButton *> m_actionTabMap;

    int m_currentIndex;
    int m_spacing;
    Qt::Orientation m_orientation;
    QSize m_iconSize;

    QBoxLayout *m_frontActionLayout;
    QBoxLayout *m_middleActionLayout;
    QBoxLayout *m_backActionLayout;
    QBoxLayout *m_tabLayout;
    QBoxLayout *m_mainLayout;
    FancyTabBar *q;
};

QCANPOOL_END_NAMESPACE

#endif // FANCYTABBARPRIVATE_H
