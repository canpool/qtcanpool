/**
 * Copyright (C) 2020-2022 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
**/
#ifndef RIBBONPAGE_H
#define RIBBONPAGE_H

#include "qcanpool.h"
#include <QWidget>

class QLabel;
class QResizeEvent;
class QWheelEvent;

QCANPOOL_BEGIN_NAMESPACE

class RibbonGroup;
class RibbonPagePrivate;

class QCANPOOL_SHARED_EXPORT RibbonPage : public QWidget
{
    Q_OBJECT
public:
    explicit RibbonPage(QWidget *parent = nullptr);
    ~RibbonPage();

public:
    void addGroup(RibbonGroup *group);
    RibbonGroup *addGroup(const QString &title);

    void insertGroup(int index, RibbonGroup *group);
    RibbonGroup *insertGroup(int index, const QString &title);

    void removeGroup(RibbonGroup *group);
    void removeGroup(int index);

    void clearGroups();

    QString title() const;

signals:
    void titleChanged(const QString &title);

public slots:
    void setTitle(const QString &title);

protected:
    void resizeEvent(QResizeEvent *e) override;
    void wheelEvent(QWheelEvent *event) override;
private:
    RibbonPagePrivate *d;
};

QCANPOOL_END_NAMESPACE

#endif // RIBBONPAGE_H
