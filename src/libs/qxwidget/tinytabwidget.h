/**
 * Copyright (C) 2023 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#pragma once

#include "qxglobal.h"
#include <QWidget>

class QToolButton;

QX_BEGIN_NAMESPACE

class TinyTabBar;
class TinyTabWidgetPrivate;

class QX_WIDGET_EXPORT TinyTabWidget : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(TabPosition tabPosition READ tabPosition WRITE setTabPosition)
public:
    explicit TinyTabWidget(QWidget *parent = Q_NULLPTR);
    ~TinyTabWidget();

    int addTab(QWidget *widget, const QString &label);
    int addTab(QWidget *widget, const QIcon &icon, const QString &label);

    int insertTab(int index, QWidget *widget, const QString &label);
    int insertTab(int index, QWidget *widget, const QIcon &icon, const QString &label);

    void removeTab(int index);

    int currentIndex() const;
    QWidget *currentWidget() const;
    QWidget *widget(int index) const;
    int indexOf(QWidget *widget) const;
    int count() const;

    void setTabEnabled(int index, bool enable);
    void setTabVisible(int index, bool visible);

    TinyTabBar *tabBar() const;

    enum TabPosition { North, South, West, East };
    Q_ENUM(TabPosition)
    TabPosition tabPosition() const;
    void setTabPosition(TabPosition);

    QToolButton *addButton(const QString &text);
    QToolButton *addButton(const QIcon &icon, const QString &text);
    void removeButton(QToolButton *button);

Q_SIGNALS:
    void currentChanged(int index);
public Q_SLOTS:
    void setCurrentIndex(int index);
    void setCurrentWidget(QWidget *widget);
private:
    QX_DECLARE_PRIVATE(TinyTabWidget)
};

QX_END_NAMESPACE
