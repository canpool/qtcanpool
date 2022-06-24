/****************************************************************************
**
** Qtitan Library by Developer Machines (Microsoft-Ribbon implementation for Qt.C++)
** 
** Copyright (c) 2009-2015 Developer Machines (http://www.devmachines.com)
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
#ifndef QTN_RIBBONPAGEPRIVATE_H
#define QTN_RIBBONPAGEPRIVATE_H

#include <QObject>
#include <QBasicTimer>

#include "QtnRibbonPrivate.h"
#include "QtnRibbonTabBar.h"
#include "QtnRibbonControls.h"
#include "QtitanDef.h"

QTITAN_BEGIN_NAMESPACE


/* RibbonPagePrivate */
class RibbonPagePrivate : public QObject
{
    Q_OBJECT
public:
    QTN_DECLARE_PUBLIC(RibbonPage)
public:
    explicit RibbonPagePrivate();
    virtual ~RibbonPagePrivate();
public:
    void init();
    bool validateGroupIndex(int index) const { return index >= 0 && index < m_listGroups.count(); }
    void removeGroup(int index, bool deleteGroup);
    int groupIndex(RibbonGroup* page) const;

    int calcReducedGroupsWidth() const; 
    void enableGroupScroll(bool scrollLeft, bool scrollRight);
    void showGroupScroll(bool onlyCalc);
    void scrollGroupAnimate();
    void startScrollGropsAnimate(int groupScrollPos, int scrollPosTarget);

    bool calcMinimumWidth(RibbonControlSizeDefinition::GroupSize size) const;

    virtual bool eventFilter(QObject*, QEvent* event);

public Q_SLOTS:
    void pressLeftScrollButton();
    void pressRightScrollButton();
    void forcedStopScrollTimer();

public:
    void updateLayout(bool updateScroll = true);
    int calculateGroupsWidth();
    int adjustGroups(int leftOffset, int actualWidth);
    bool isCollapseOrder();
    bool collapseGroups(int& leftOffset, int actualWidth, RibbonControlSizeDefinition::GroupSize size, bool adjust);
    bool expandGroups(int& leftOffset, int actualWidth, RibbonControlSizeDefinition::GroupSize size, bool adjust);
    RibbonControlSizeDefinition::GroupSize getMinGroupSize() const;
    RibbonControlSizeDefinition::GroupSize getMaxGroupSize() const;
    bool canReduce() const;

public:
    RibbonGroupScroll* m_buttonScrollGroupLeft;    // Button to draw left scroll
    RibbonGroupScroll* m_buttonScrollGroupRight;   // Button to draw right scroll

    QList<RibbonGroup*> m_listGroups;
    QList<QAction*> m_listShortcuts;
    RibbonTab* m_associativeTab;
    QBasicTimer m_timer;
    QBasicTimer m_scrollTimer;
    QString m_title;
    QString m_contextTitle;
    QString m_contextGroupName;
    RibbonPage::ContextColor m_contextColor;
    Qt::LayoutDirection m_layoutDirection;

    int m_groupsHeight;
    int m_pageScrollPos;
    int m_groupScrollPos;
    int m_scrollPosTarget;
    double m_animationStep;
    int m_timerElapse;

    bool m_minimazeRibbon : 1;
    bool m_doVisiblePage  : 1;
    bool m_allowPress     : 1; 
    bool m_doPopupPage    : 1;
    bool m_animation      : 1;

private:
    Q_DISABLE_COPY(RibbonPagePrivate)
};

QTITAN_END_NAMESPACE

#endif // QTN_RIBBONPAGEPRIVATE_H
