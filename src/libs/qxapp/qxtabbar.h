/**
 * Copyright (C) 2023 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#pragma once

#include "qxapp_global.h"
#include <QToolBar>

QX_APP_BEGIN_NAMESPACE

class QxTabBarPrivate;

/* QxTabBar */
class QX_APP_EXPORT QxTabBar : public QToolBar
{
    Q_OBJECT
public:
    explicit QxTabBar(QWidget *parent = Q_NULLPTR);
    virtual ~QxTabBar();

    int addTab(const QString &text);
    int addTab(const QIcon &icon, const QString &text);

    int insertTab(int index, const QString &text);
    int insertTab(int index, const QIcon &icon, const QString &text);

    void removeTab(int index);

    void setTabEnabled(int index, bool enable);
    bool isTabEnabled(int index) const;

    void setTabVisible(int index, bool visible);
    bool isTabVisible(int index) const;

    void setTabToolTip(int index, const QString &toolTip);
    QString tabToolTip(int index) const;

    bool isTabChecked(int index) const;

    void setCurrentIndex(int index);
    int currentIndex() const;

    void setTogglable(bool able);
    bool isTogglable() const;

    void setTabStyle(Qt::ToolButtonStyle style);

    int count() const;
Q_SIGNALS:
    void currentChanged(int index);
    void currentToggled(int index, bool checked);
protected:
    virtual void actionEvent(QActionEvent *event);
protected:
    explicit QxTabBar(QxTabBarPrivate *d, QWidget *parent = Q_NULLPTR);
protected:
    QX_DECLARE_PRIVATE(QxTabBar)
private:
    Q_DISABLE_COPY(QxTabBar)
};

QX_APP_END_NAMESPACE
