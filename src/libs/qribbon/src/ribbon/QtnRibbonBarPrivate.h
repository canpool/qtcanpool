/****************************************************************************
**
** Qtitan Library by Developer Machines (Microsoft-Ribbon implementation for Qt.C++)
** 
** Copyright (c) 2009-2013 Developer Machines (http://www.devmachines.com)
** Copyright (c) 2019 MaMinJie <canpool@163.com>
**           ALL RIGHTS RESERVED
** 
**  The entire contents of this file is protected by copyright law and
**  international treaties. Unauthorized reproduction, reverse-engineering
**  and distribution of all or any portion of the code contained in this
**  file is strictly prohibited and may result in severe civil and 
**  criminal penalties and will be prosecuted to the maximum extent 
**  possible under the law.
**
**  RESTRICTIONS
**
**  THE SOURCE CODE CONTAINED WITHIN THIS FILE AND ALL RELATED
**  FILES OR ANY PORTION OF ITS CONTENTS SHALL AT NO TIME BE
**  COPIED, TRANSFERRED, SOLD, DISTRIBUTED, OR OTHERWISE MADE
**  AVAILABLE TO OTHER INDIVIDUALS WITHOUT WRITTEN CONSENT
**  AND PERMISSION FROM DEVELOPER MACHINES
**
**  CONSULT THE END USER LICENSE AGREEMENT FOR INFORMATION ON
**  ADDITIONAL RESTRICTIONS.
**
****************************************************************************/
#ifndef QTN_RIBBONBARPRIVATE_H
#define QTN_RIBBONBARPRIVATE_H

#include <QStyle>
#include <QStack>

#include "QtnRibbonBar.h"
#include "QtnRibbonTabBar.h"

#include "QtitanDef.h"

QTITAN_BEGIN_NAMESPACE

class RibbonTabBar;
class RibbonTitleButton;
class OfficeFrameHelper;
class StyleOptionTitleBar;
class RibbonSystemButton;
class MenuMinimizedGroups;
class RibbonKeyTip;

/* RibbonBarPrivate */
class RibbonBarPrivate : public QObject
{
public:
    Q_OBJECT
    QTN_DECLARE_PUBLIC(RibbonBar)
public:
    explicit RibbonBarPrivate();
    virtual ~RibbonBarPrivate();

public:
    void init();
    void layoutRibbon();
    void layoutTitleButton();
    void layoutContextHeaders();
    void layoutCorner();

    void removeAllContextHeaders();
    bool reduceContextHeaders();

    void updateMinimizedModePage(RibbonPage* page);
    void swapVisiblePages(int index);

    QAction* createSystemButton(const QIcon& icon, const QString& text);

    RibbonPage* insertPage(int indTab, int index);
    void insertPage(int indTab, int index, RibbonPage* page);
    void removePage(int index, bool deletePage = true);
    int getIndexPage(RibbonPage* page) const;

    void currentChanged(int index);
    bool validIndex(int index) const { return index >= 0 && index < m_listPages.count(); }

    void titleBarOptions(StyleOptionTitleBar& opt) const;
    QStyle::SubControl getSubControl(const QPoint& pos) const;

    RibbonTitleButton* findTitleButton(QStyle::SubControl subControl) const;
    QAction* addTitleButton(QStyle::SubControl subControl, bool add, QRect& rcControl);

    ContextHeader* hitTestContextHeaders(const QPoint& point) const;

    void showKeyTips(QWidget* w);
    bool hideKeyTips();
    void createKeyTips();
    void createGroupKeyTips();
    void createWidgetKeyTips(RibbonGroup* group, QWidget* widget, const QString& prefix, const QRect& rcGroups, bool visible);
    bool createPopupKeyTips(QMenu* levalMenu);
    void destroyKeyTips();
    void calcPositionKeyTips();
    int getMaxHeightPages() const;

private Q_SLOTS:
    void processClickedSubControl();

protected:
    bool pressTipCharEvent(const QKeyEvent* key);
    virtual bool eventFilter(QObject*, QEvent* event);

public:
    QRect m_rcPageArea;
    QRect m_rcTabBar;
    int m_currentIndexPage;
    RibbonTabBar* m_tabBar;
    RibbonQuickAccessBar* m_quickAccessBar;
    RibbonSystemButton* m_controlSystemButton;
    OfficeFrameHelper* m_frameHelper;
    QRect m_rcTitle;
    QRect m_rcHeader;
    QRect m_rcTitleText;
    QRect m_rcQuickAccess;

    bool m_showTitleAlways;
    bool m_quickAccessOnTop;
    bool m_ribbonBarVisible;
    bool m_minimizationEnabled;
    bool m_minimized;
    bool m_keyTipsComplement;
    bool m_keyTipsEnabled;
    bool m_keyTipsShowing;

    QAction* m_actCloseButton;
    QAction* m_actNormalButton;
    QAction* m_actMaxButton;
    QAction* m_actMinButton;

    QList<ContextHeader*> m_listContextHeaders;
    QList<RibbonKeyTip*> m_keyTips;
    QStack<QWidget*> m_levels;
    int m_countKey;

    QStyle::SubControl m_hoveredSubControl;
    QStyle::SubControl m_activeSubControl;
    QString m_windowTitle;
    bool m_initFlag;

protected:
    QList<RibbonPage*> m_listPages;
    QVector<int> m_dirtyPages;
    bool m_wheelEvent;
private:
    Q_DISABLE_COPY(RibbonBarPrivate)
};

QTITAN_END_NAMESPACE

#endif // QTN_RIBBONPRIVATE_H


