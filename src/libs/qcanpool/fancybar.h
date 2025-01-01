/**
 * Copyright (C) 2018-2022 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
**/
#ifndef FANCYBAR_H
#define FANCYBAR_H

#include "qcanpool.h"
#include <QWidget>

class QMenuBar;
class QToolBar;

QCANPOOL_BEGIN_NAMESPACE

class QuickAccessBar;

class FancyTitleBar;
class FancyBarPrivate;

/* FancyBar */
class QCANPOOL_SHARED_EXPORT FancyBar : public QWidget
{
    Q_OBJECT

public:
    enum FancyStyle {
        WindowStyle, MergedStyle, DialogStyle
    };

public:
    explicit FancyBar(QWidget *parent);
    ~FancyBar();

    FancyTitleBar *titleBar() const;

    QMenuBar *menuBar() const;
    void showMenuBar(bool show = false);
    bool isMenuBarVisible() const;

    QToolBar *toolBar() const;

    QuickAccessBar *quickAccessBar() const;
    void showQuickAccess(bool show = true);
    bool isQuickAccessVisible() const;

    void setWidgetResizable(bool resizable);
    void setTitleBarHeight(int height);

    void setFancyStyle(FancyStyle style);
    void updateWidgetFlags();

Q_SIGNALS:
    void maximizationChanged(bool maximized);

private:
    FancyBarPrivate *d;
};

QCANPOOL_END_NAMESPACE

#endif // FANCYBAR_H
