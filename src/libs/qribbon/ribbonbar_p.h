/**
 * Copyleft (C) 2023 maminjie <canpool@163.com>
 **/
#pragma once

#include "commonstyle_p.h"
#include "ribbon_global.h"
#include "ribbonbar.h"
#include "ribboncustomizepage.h"
#include "ribbonquickaccessbar.h"
#include "ribbontabbar.h"
#include <QLabel>
#include <QStack>
#include <QStyle>

QRIBBON_BEGIN_NAMESPACE

class LogotypeLabel : public QLabel
{
public:
    LogotypeLabel(QWidget *parent);
public:
    Qt::AlignmentFlag alignmentLogotype() const { return m_alignment; }
    void setAlignmentLogotype(Qt::AlignmentFlag alignment) { m_alignment = alignment; }
protected:
    virtual void paintEvent(QPaintEvent *p);
private:
    Qt::AlignmentFlag m_alignment;
};

class RibbonTabBar;
class RibbonTitleButton;
class OfficeFrameHelper;
class StyleOptionTitleBar;
class RibbonSystemButton;
class RibbonBackstageCloseButton;
class MenuMinimizedGroups;
class RibbonKeyTip;
class RibbonCustomizeManager;

class RibbonBarPrivate : public QObject
{
public:
    Q_OBJECT
    QRN_DECLARE_PUBLIC(RibbonBar)
public:
    explicit RibbonBarPrivate();
    virtual ~RibbonBarPrivate();
public:
    void init();
    void calcHeightItem();

    void layoutRibbon();
    void layoutTitleButton();
    bool isExistAssociativeTabWithPage(const RibbonTab *tab);
    void layoutContextHeaders();
    void layoutCorner();

    void removeAllContextHeaders();
    bool reduceContextHeaders();

    void updateMinimizedModePage(RibbonPage *page);
    void swapVisiblePages(int index);
    void setVisibleBackstage(bool visible);

    QAction *createSystemButton(const QIcon &icon, const QString &text);

    RibbonPage *insertPage(int indTab, int index);
    void insertPage(int indTab, int index, RibbonPage *page);
    void removePage(int index, bool deletePage = true);
    int indexPage(RibbonPage *page) const;

    bool validIndex(int index) const { return index >= 0 && index < m_listPages.count(); }
    int heightTabs() const;
    int topBorder() const;

    void titleBarOptions(StyleOptionTitleBar &opt) const;
    QStyle::SubControl getSubControl(const QPoint &pos) const;

    RibbonTitleButton *findTitleButton(QStyle::SubControl subControl) const;
    QAction *addTitleButton(QStyle::SubControl subControl, bool add, QRect &rcControl);

    ContextHeader *hitTestContextHeaders(const QPoint &point) const;
    QMenu *createContextMenu();

    void showKeyTips(QWidget *w);
    bool hideKeyTips();
    void createKeyTips();
    void createGroupKeyTips();
    void createWidgetKeyTips(RibbonGroup *group, QWidget *widget, const QString &prefix, const QRect &rcGroups,
                             bool visible);
    bool createPopupKeyTips(QMenu *levalMenu);
    void destroyKeyTips();
    void calcPositionKeyTips();
    int rowItemHeight() const;
    int rowItemCount() const;
    int maxGroupHeight() const;
    void createCustomizeDialog();

private Q_SLOTS:
    void processClickedSubControl();
    void toggledCustomizeBar();
    void toggledQuickAccessBarPos();
    void toggledMinimized();
    void currentChanged(int index);
protected:
    bool pressTipCharEvent(const QKeyEvent *key);
    virtual bool eventFilter(QObject *, QEvent *event);
public:
    QRect m_rcPageArea;
    QRect m_rcTabBar;
    int m_currentIndexPage;
    int m_heightTabs;
    RibbonTabBar *m_tabBar;
    RibbonQuickAccessBar *m_quickAccessBar;
    RibbonSystemButton *m_controlSystemButton;
    QVector<QWidget *> m_visibleWidgets;

    OfficeFrameHelper *m_frameHelper;
    QRect m_rcTitle;
    QRect m_rcHeader;
    QRect m_rcTitleText;
    QRect m_rcQuickAccess;

    bool m_showTitleAlways;
    bool m_ribbonBarVisible;
    bool m_minimizationEnabled;
    bool m_minimized;
    bool m_keyTipsComplement;
    bool m_keyTipsEnabled;
    bool m_keyTipsShowing;
    bool m_titleGroupsVisible;
    RibbonBar::QuickAccessBarPosition m_quickAccessBarPosition;
    RibbonBar::TabBarPosition m_tabBarPosition;

    QAction *m_actCloseButton;
    QAction *m_actNormalButton;
    QAction *m_actMaxButton;
    QAction *m_actMinButton;

    QList<ContextHeader *> m_listContextHeaders;
    QList<RibbonKeyTip *> m_keyTips;
    QStack<QWidget *> m_levels;
    int m_countKey;

    QStyle::SubControl m_hoveredSubControl;
    QStyle::SubControl m_activeSubControl;
    QString m_windowTitle;

    LogotypeLabel *m_logotypeLabel;

    QPixmap m_pixTitleBackground;
    RibbonCustomizeManager *m_customizeManager;
    RibbonCustomizeDialog *m_customizeDialog;
protected:
    QList<RibbonPage *> m_listPages;
    int m_rowItemHeight;
    int m_rowItemCount;
private:
    Q_DISABLE_COPY(RibbonBarPrivate)
};

QRIBBON_END_NAMESPACE
