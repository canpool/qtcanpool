/**
 * Copyright (C) 2020-2022 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
**/
#ifndef RIBBONBAR_P_H
#define RIBBONBAR_P_H

#include "qcanpool.h"
#include "ribbonbar.h"
#include <QObject>
#include <QStackedWidget>
#include <QIcon>

class QTabBar;
class QAction;
class QToolButton;
class QToolBar;
class QHBoxLayout;
class QVBoxLayout;

QCANPOOL_BEGIN_NAMESPACE

class RibbonPage;
class RibbonBar;

class RibbonStackedWidget : public QStackedWidget
{
    Q_OBJECT
public:
    explicit RibbonStackedWidget(QWidget *parent = nullptr);
    virtual ~RibbonStackedWidget();

    bool isPopup() const;
    void setPopup(bool popup);

signals:
    void aboutToShow();
    void aboutToHide();

protected:
    virtual bool event(QEvent *event);

private:
    bool m_isPopup;
};


/* RibbonBarPrivate */
class RibbonBarPrivate : public QObject
{
    Q_OBJECT
public:
    explicit RibbonBarPrivate();
    virtual ~RibbonBarPrivate();

    void init(void);
    void setRibbonStyle(RibbonBar::RibbonStyle style);

    inline bool validateIndex(int index) const
    { return index >= 0 && index < m_listPages.count(); }

    void insertPage(int index, RibbonPage *page);
    void removePage(int index);
    int pageIndex(RibbonPage *page) const;

    int addAction(QAction *action);

    void showPagePopup();

    void cleanLayout(QLayout *layout);

public slots:
    void showMinMaximized();
    void slotCurrentTabChanged(int index);
    void slotTabBarClicked(int index);
    void slotTabBarDoubleClicked(int index);
    void slotPagePopupHide();

public:
    RibbonBar               *q;
    FancyTitleBar           *m_titleBar;
    QWidget                 *m_titleWidget;
    QTabBar                 *m_tabBar;
    QuickAccessBar          *m_quickAccessBar;
    RibbonStackedWidget     *m_stack;
    QToolBar                *m_baseToolBar;
    QToolBar                *m_topToolBar;
    QToolBar                *m_bottomToolBar;
    QHBoxLayout             *m_topLayout;
    QHBoxLayout             *m_bottomLayout;
    QVBoxLayout             *m_mainLayout;
    QWidget                 *m_mainWidget;
    QAction                 *m_minimizeAction;
    QToolButton             *m_minimizeButton;

    QIcon m_normalIcon;
    QIcon m_minimizeIcon;

    QList<RibbonPage *> m_listPages;

    bool m_minimized;
    bool m_frameless;
    RibbonBar::RibbonStyle m_style;
};

QCANPOOL_END_NAMESPACE

#endif // RIBBONBAR_P_H
