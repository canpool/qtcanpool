/**
 * Copyright (C) 2018-2022 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
**/
#ifndef FANCYTABWIDGET_H
#define FANCYTABWIDGET_H

#include "qcanpool.h"
#include <QWidget>

QCANPOOL_BEGIN_NAMESPACE

class FancyTabBar;
class FancyTabWidgetPrivate;

/* FancyTabWidget */
class QCANPOOL_SHARED_EXPORT FancyTabWidget : public QWidget
{
    Q_OBJECT

    Q_PROPERTY(TabPosition tabPosition READ tabPosition WRITE setTabPosition)

public:
    explicit FancyTabWidget(QWidget *parent = nullptr);
    ~FancyTabWidget();

    int addTab(QWidget *widget, const QString &label);
    int addTab(QWidget *widget, const QIcon &icon, const QString &label);

    int insertTab(int index, QWidget *widget, const QString &label);
    int insertTab(int index, QWidget *widget, const QIcon& icon, const QString &label);

    void removeTab(int index);

    int currentIndex() const;
    QWidget *currentWidget() const;
    QWidget *widget(int index) const;
    int indexOf(QWidget *widget) const;
    int count() const;

    void setTabEnabled(QWidget *widget, bool enable);
    void setTabVisible(QWidget *widget, bool visible);

    FancyTabBar *tabBar(void) const;

    enum TabPosition { North, South, West, East };
    Q_ENUM(TabPosition)
    TabPosition tabPosition() const;
    void setTabPosition(TabPosition);

private:
    FancyTabWidgetPrivate *d;
};

QCANPOOL_END_NAMESPACE

#endif // FANCYTABWIDGET_H
