/**
 * Copyright (C) 2018-2022 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
**/
#ifndef FANCYBAR_P_H
#define FANCYBAR_P_H

#include "qcanpool.h"
#include <QWidget>

class QHBoxLayout;
class QSpacerItem;
class QToolButton;
class QMenuBar;
class QToolBar;
class QLabel;
class QMouseEvent;

QCANPOOL_BEGIN_NAMESPACE

class QuickAccessBar;

class FancyBar;
class FancyTitleBar;

/* FancyBarPrivate */
class FancyBarPrivate : public QWidget
{
    Q_OBJECT
public:
    explicit FancyBarPrivate(QWidget *parent = nullptr);
    ~FancyBarPrivate();

    void init();
    void createTitleWidget();
    void createMenuWidget();

    void showQuickAccessBar(bool show);
    void showMenuBar(bool show);
    void showMenuWidget(bool show);

    void setFancyStyle(int style);

    void registerWidget(QWidget *widget);

protected:
    QSize sizeHint() const;

public:
    FancyBar            *q;

    QHBoxLayout         *m_logoLayout;
    // title widget
    QWidget             *m_titleWidget;
    FancyTitleBar       *m_titleBar;
    QToolButton         *m_logoButton;
    QuickAccessBar      *m_quickAccessBar;
    QSpacerItem         *m_leftSpacerItem;
    QSpacerItem         *m_rightSpacerItem;
    QLabel              *m_titleLabel;
    QWidget             *m_systemToolBar;

    // menu widget
    QWidget             *m_menuWidget;
    QMenuBar            *m_menuBar;
    QToolBar            *m_toolBar;
    QHBoxLayout         *m_menuAdditionalControlArea;
    QHBoxLayout         *m_menuBarArea;
    QSpacerItem         *m_middleSpacerItem;

    QList<QToolButton *> m_additionalButtons;

    int     m_style;
    int     m_titleBarHeight;

    bool    m_bQuickAccessVisible;
    bool    m_bMenuBarVisible;
    bool    m_bMenuWidgetVisible;
};

QCANPOOL_END_NAMESPACE

#endif // FANCYBAR_P_H
