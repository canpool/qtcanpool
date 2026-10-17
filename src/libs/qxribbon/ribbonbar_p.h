/**
 * Copyleft (C) 2023 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MIT
 **/
#pragma once

#include "qxribbon_global.h"
#include "ribbonbar.h"
#include "ribboncontainers.h"

#include <QStackedWidget>

QX_RIBBON_BEGIN_NAMESPACE

class RibbonTabBar;
class RibbonQuickAccessBar;
class RibbonControlButton;
class RibbonKeyTip;

// Accessibility (B2): register the ribbon's QAccessible factory. Idempotent.
void installRibbonAccessibility();

class RibbonStackedWidget : public QStackedWidget
{
    Q_OBJECT
public:
    explicit RibbonStackedWidget(QWidget *parent = Q_NULLPTR);
    virtual ~RibbonStackedWidget();

    bool isPopup() const;
    void setPopup(bool popup);

    void moveWidget(int from, int to);

Q_SIGNALS:
    void aboutToShow();
    void aboutToHide();
protected:
    virtual bool event(QEvent *event);
private:
    bool m_isPopup;
};

/* RibbonQuickAccessBarContainer */
class RibbonQuickAccessBarContainer : public RibbonCtrlContainer
{
    Q_OBJECT
public:
    RibbonQuickAccessBarContainer(QWidget *parent = Q_NULLPTR);
    ~RibbonQuickAccessBarContainer();
public:
    RibbonQuickAccessBar *quickAccessBar() const;
private:
    RibbonQuickAccessBar *m_quickAccessBar;
};

class PageContextManagerData
{
public:
    RibbonPageContext *pageContext;
    QList<int> tabPageIndex;
    bool operator==(const RibbonPageContext *pageContext)
    {
        return (this->pageContext == pageContext);
    }
};

class RibbonTabData
{
public:
    RibbonPage *page;
    int index;
    RibbonTabData()
        : page(Q_NULLPTR)
        , index(-1)
    {
    }
};

class RibbonBarPrivate : public QObject
{
    Q_OBJECT
    QX_DECLARE_PUBLIC(RibbonBar)
public:
    // KeyTips (B1): a target that can carry a badge
    struct KeyTipTarget {
        QWidget *widget;     ///< Trigger widget (application button / RibbonButton); nullptr for tab targets
        int pageIndex;       ///< >= 0: tab index; < 0: control target
        QString preferred;   ///< Preferred mnemonic (may be empty, then auto-filled from A-Z)
    };

    RibbonBarPrivate();

    void init();

    void setApplicationButton(QAbstractButton *btn);
    bool isPageContextVisible(RibbonPageContext *pageContext);

    void setMinimizedFlag(bool flag);
    void updateMinimumButtonIcon();

    QColor getPageContextColor();

    bool hasOwnTabRow() const
    {
        return ((m_ribbonStyle & 0x00FF) == 0);
    }
    bool isTwoRowStyle() const
    {
        return ((m_ribbonStyle & 0xFF00) != 0);
    }

    void updateTabData();
    void updatePageContextManagerData();

    // KeyTips (B1): the keyboard-cascade badge mode entered with Alt
    void showKeyTips();          ///< Level 1: application button + visible page tabs
    void showControlKeyTips();   ///< Level 2: RibbonButton badges inside the current page
    void renderKeyTips(const QList<KeyTipTarget> &targets);
    void hideKeyTips();
    bool handleKeyTipKeyEvent(QKeyEvent *ke);
    void updateRibbonElementGeometry();
    void updateRibbonBarHeight();

    void showPopupStack();

    void paintInTabBarBelowStyle(QPainter &p);
    void paintInTabBarInlineStyle(QPainter &p);
    void paintBackground(QPainter &painter);
    QPoint paintPageContextTabs(QPainter &painter, bool extendToWindowTop);
    void paintWindowTitle(QPainter &painter, const QString &title, const QRect &titleRegion);
    void paintPageContextTab(QPainter &painter, const QString &title, QRect contextRect, const QColor &color);

    void resizeInTabBarBelowStyle();
    void resizeInTabBarInlineStyle();
    void resizeStackedWidget();
    void resizeRibbon();

    void layoutTabBar(int x, int y, int allowedTabBarWidth, int tabH);
    void placeCornerWidget(QWidget *corner, int x, int y, int centredHeight, int stretchedHeight);

    int minimumWidthInTabBarBelowStyle() const;
    int minimumWidthInTabBarInlineStyle() const;

    int calcMinTabBarWidth() const;
    int mainBarHeight() const;

    int tabIndex(RibbonPage *page) const;

    RibbonButtonGroup *createButtonGroup();

public Q_SLOTS:
    void onWindowTitleChanged(const QString &title);
    void onWindowIconChanged(const QIcon &icon);
    void onPageWindowTitleChanged(const QString &title);
    void onPageContextAdded(RibbonPage *page);
    void onCurrentRibbonTabChanged(int index);
    void onCurrentRibbonTabClicked(int index);
    void onCurrentRibbonTabDoubleClicked(int index);
    void onTabMoved(int from, int to);
    void onStackWidgetHided();
    void onFontChanged();
public:
    QAbstractButton *m_applicationButton;
    RibbonTabBar *m_tabBar;
    QWidget *m_tabBarBaseLine;
    RibbonStackedWidget *m_stack;
    QList<PageContextManagerData> m_currentShowingPageContextList;
    QList<RibbonPageContext *> m_pageContextList;
    QList<RibbonTabData> m_hidedPage;
    int m_iconRightBorderPosition;   ///< Minimum x value of the title bar; changes with the icon and quick access
                                     ///< button
    RibbonControlButton *m_minimumPageButton;   ///< Button that hides the panel
    RibbonButtonGroup *m_topLeftButtonGroup;
    RibbonButtonGroup *m_topRightButtonGroup;
    RibbonButtonGroup *m_bottomLeftButtonGroup;
    RibbonButtonGroup *m_bottomRightButtonGroup;   ///< Button group to the right of the tabbar
    QWidget *m_windowButtonGroup;
    RibbonQuickAccessBarContainer *m_quickAccessBar;
    RibbonBar::QuickAccessBarPosition m_quickAccessBarPosition;
    RibbonBar::TabBarPosition m_tabBarPosition;
    RibbonBar::RibbonStyle m_ribbonStyle;
    RibbonBar::RibbonStyle m_lastShowStyle;
    QList<QColor> m_pageContextColorList;
    int m_pageContextColorListIndex;
    Qt::Alignment m_titleAligment;
    bool m_minimized;
    bool m_titleVisible;
    bool m_pageContextCoverTab;
    bool m_layoutRequest;
    bool m_blockResize;

    bool m_keyTipMode;                 ///< Whether keytip badge mode is active
    QList<RibbonKeyTip *> m_keyTips;   ///< Currently shown KeyTip badges
};

QX_RIBBON_END_NAMESPACE

Q_DECLARE_METATYPE(QX_RIBBON_PREPEND_NAMESPACE(RibbonTabData))
