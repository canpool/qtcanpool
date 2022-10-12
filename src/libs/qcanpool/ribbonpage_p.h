/**
 * Copyright (C) 2020-2022 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
**/
#ifndef RIBBONPAGE_P_H
#define RIBBONPAGE_P_H

#include "qcanpool.h"
#include <QObject>

class QHBoxLayout;
class QToolButton;

QCANPOOL_BEGIN_NAMESPACE

class RibbonGroup;
class RibbonPage;

/* RibbonPagePrivate */
class RibbonPagePrivate : public QObject
{
    Q_OBJECT
public:
    RibbonPagePrivate();
    ~RibbonPagePrivate();

    void init();

    bool validateGroupIndex(int index) const
    { return index >= 0 && index < m_listGroups.count(); }

    void insertGroup(int index, RibbonGroup *group);
    void removeGroup(int index);
    int groupIndex(RibbonGroup *group);

    int calculateGroupsWidth();
    void updateLayout();

public slots:
    void slotLeftScrollButton();
    void slotRightScrollButton();

public:
    RibbonPage *q;
    QHBoxLayout *m_layout;

    QList<RibbonGroup *> m_listGroups;
    QToolButton *m_leftScrollB;    // Button to draw left scroll
    QToolButton *m_rightScrollB;   // Button to draw right scroll

    QString m_title;

    int m_groupXBase;
    int m_groupsWidth;
};

QCANPOOL_END_NAMESPACE

#endif // RIBBONPAGE_P_H
