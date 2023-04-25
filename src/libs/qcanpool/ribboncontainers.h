/**
 * Copyright (C) 2021-2022 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
**/
#ifndef RIBBONCONTAINERS_H
#define RIBBONCONTAINERS_H

#include "qcanpool.h"
#include <QWidget>

#include "ribbongroup.h"

class QToolBar;

QCANPOOL_BEGIN_NAMESPACE

class QCANPOOL_SHARED_EXPORT RibbonContainer : public QWidget
{
    Q_OBJECT
public:
    explicit RibbonContainer(QWidget *parent = nullptr);
    virtual ~RibbonContainer();
};

/* RibbonGridContainer */
class RibbonGridContainerPrivate;
class QCANPOOL_SHARED_EXPORT RibbonGridContainer : public RibbonContainer
{
    Q_OBJECT
public:
    explicit RibbonGridContainer(QWidget *parent = nullptr);
    virtual ~RibbonGridContainer();

public:
    void addWidget(QWidget *widget);

private:
    RibbonGridContainerPrivate *d;
};

/* RibbonActionContainer */
class RibbonActionContainerPrivate;
class QCANPOOL_SHARED_EXPORT RibbonActionContainer : public RibbonContainer
{
    Q_OBJECT
public:
    explicit RibbonActionContainer(QWidget *parent = nullptr);
    virtual ~RibbonActionContainer();

public:
    QAction *addAction(const QIcon &icon, const QString &text, RibbonGroup::GroupSize size);
    void addAction(QAction *action, RibbonGroup::GroupSize size); // for action that has menu

private:
    RibbonActionContainerPrivate *d;
};

/* RibbonLoftContainer */
class RibbonLoftContainerPrivate;
class QCANPOOL_SHARED_EXPORT RibbonLoftContainer : public RibbonContainer
{
    Q_OBJECT
public:
    enum Position {
        Top, Bottom
    };
public:
    explicit RibbonLoftContainer(QWidget *parent = nullptr);
    virtual ~RibbonLoftContainer();

public:
    QToolBar *toolBar(Position pos);

private:
    RibbonLoftContainerPrivate *d;
};

QCANPOOL_END_NAMESPACE

#endif // RIBBONCONTAINERS_H
