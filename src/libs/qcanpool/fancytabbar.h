/**
 * Copyright (C) 2018-2022 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
**/
#ifndef FANCYTABBAR_H
#define FANCYTABBAR_H

#include "qcanpool.h"
#include <QWidget>

class QToolButton;

QCANPOOL_BEGIN_NAMESPACE

class FancyTabBarPrivate;

/* FancyTabBar */
class QCANPOOL_SHARED_EXPORT FancyTabBar : public QWidget
{
    Q_OBJECT

public:
    enum ActionPosition { Front, Middle, Back };

public:
    explicit FancyTabBar(QWidget *parent = nullptr);
    virtual ~FancyTabBar();

    /* tabs */
    int addTab(const QString &text);
    int addTab(const QIcon &icon, const QString &text);

    int insertTab(int index, const QString &text);
    int insertTab(int index, const QIcon &icon, const QString &text);

    void removeTab(int index);

    int currentIndex() const;
    int count() const;

    bool isTabEnabled(int index) const;
    void setTabEnabled(int index, bool enable);

    bool isTabVisible(int index) const;
    void setTabVisible(int index, bool visible);

    QString tabText(int index) const;
    void setTabText(int index, const QString &text);

    QIcon tabIcon(int index) const;
    void setTabIcon(int index, const QIcon &icon);

#ifndef QT_NO_TOOLTIP
    void setTabToolTip(int index, const QString &tip);
    QString tabToolTip(int index) const;
#endif

    void setTabIconsOnly(bool iconOnly);

    /* actions */
    QAction *addAction(const QIcon &icon, const QString &text, ActionPosition position = Back);
    void addAction(QAction *action, ActionPosition position = Back);
    void addActionButton(QToolButton *button, ActionPosition position = Back);
    void setActionIconOnly(QAction *action, bool iconOnly);

    /* common */
    void setSpacing(int spacing);

    QSize iconSize() const;
    void setIconSize(QSize size);

    Qt::Orientation orientation() const;
    void setOrientation(Qt::Orientation orientation);

Q_SIGNALS:
    void currentChanged(int index);
    void orientationChanged(Qt::Orientation orientation);

public Q_SLOTS:
    void setCurrentIndex(int index);

private:
    FancyTabBarPrivate *d;
};

QCANPOOL_END_NAMESPACE

#endif // FANCYTABBAR_H
