/**
 * Copyright (C) 2020-2022 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
**/
#ifndef RIBBONGROUP_P_H
#define RIBBONGROUP_P_H

#include "qcanpool.h"
#include "ribbongroup.h"
#include <QToolBar>
#include <QObject>
#include <QIcon>

class QHBoxLayout;
class QLabel;
class QToolButton;

QCANPOOL_BEGIN_NAMESPACE

class RibbonGroup;

/* RibbonBarInnerContainer */
class RibbonBarInnerContainer : public QToolBar
{
    Q_OBJECT
public:
    RibbonBarInnerContainer(RibbonGroup::GroupSize size, QWidget *parent = nullptr);
    virtual ~RibbonBarInnerContainer();
};

/* RibbonGroupPrivate */
class RibbonGroupPrivate : public QObject
{
    Q_OBJECT
public:
    explicit RibbonGroupPrivate();
    virtual ~RibbonGroupPrivate();

    void init();

    RibbonBarInnerContainer *getContainer(RibbonGroup::GroupSize size);
    QAction *addAction(const QIcon &icon, const QString &text,
                       RibbonGroup::GroupSize size);
    void addAction(QAction *action, RibbonGroup::GroupSize size);
    void formatAction(RibbonBarInnerContainer *container, QAction *action);
    void addToolButton(QToolButton *button, RibbonGroup::GroupSize size);

public:
    RibbonGroup *q;
    QHBoxLayout *m_inLayout;
    QHBoxLayout *m_extLayout;
    QHBoxLayout *m_titleLayout;
    QLabel      *m_titleLabel;
    QToolButton *m_optionButton;

    RibbonBarInnerContainer *m_largeContainer;
    RibbonBarInnerContainer *m_mediumContainer;
    RibbonBarInnerContainer *m_smallContainer;
};

QCANPOOL_END_NAMESPACE

#endif // RIBBONGROUP_P_H
