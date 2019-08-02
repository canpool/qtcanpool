/****************************************************************************
**
** Qtitan Library by Developer Machines (Microsoft-Ribbon implementation for Qt.C++)
** 
** Copyright (c) 2009-2011 Developer Machines (http://www.devmachines.com)
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
#ifndef QTN_RIBBONGROUPPRIVATE_H
#define QTN_RIBBONGROUPPRIVATE_H

#include <QIcon>

#include "QtnRibbonGroupLayout.h"
#include "QtitanDef.h"

class QAction;
class QToolButton;
class QStyleOptionGroupBox;

QTITAN_BEGIN_NAMESPACE

/* GroupItem */
class GroupItem : public QWidgetItem
{
public:
    GroupItem(QWidget* widget, bool begin, bool group);
public:
    bool isEmpty() const;

public:
    QAction* action;
    QRect rcRow;
    QRect rcItem;
    QSize szItem;
    int tailWidth;
    bool customWidget : 1;
    bool beginGroup   : 1;
    bool wrapGroup    : 1;
    bool wrap         : 1;
};

class ExWidgetWrapper;
class RibbonGroup;
/* GroupLayout */
class GroupDefaultLayout : public GroupLayout
{
    Q_OBJECT
public:
    GroupDefaultLayout(QWidget* parent = Q_NULL);
    virtual ~GroupDefaultLayout();

public:
    virtual void addItem(QLayoutItem* item);
    virtual QLayoutItem* itemAt(int index) const;
    virtual QLayoutItem* takeAt(int index);
    virtual int count() const;
    virtual void invalidate();

    virtual void setGeometry(const QRect& rect);
    virtual QSize minimumSize() const;
    virtual QSize sizeHint() const;

    virtual void insertAction(int index, QAction* action);
    virtual int indexOf(QAction* action) const;
    virtual int indexOf(QWidget* widget) const { return GroupLayout::indexOf(widget); }

    void updateMarginAndSpacing();

    QSize sizeRealHint() const;
    int getMinimumWidth() const;

    QWidget* getNextWidget(const QWidget* w) const;

    QWidget* getWidget(int index) const;
    int getIndexWidget(const QWidget* w) const;
    int getNextIndex  (const QWidget* w) const;

    void setDirty(bool bDirty) { dirty = bDirty; /*invalidate();*/ }

public:
    int rowCount_;

private:
    QList<GroupItem*> items;
    QSize hint, minSize, redSize;
    mutable int width_;
    bool dirty        : 1;
    bool hasSeparator : 1;

private:
    void layoutActions();

    void updateGeomArray() const;
    void updateAlignWidget(QList<ExWidgetWrapper*>& alignWrappers) const;
    void updateBorders(int width, QRect rcBorder) const;
    void updateCenterColumn(int firstItem, int lastItem, int groupHeight) const;
    void updateGeomArray_() const;
    bool extendSize(int widthAvail);
    void resetLayout();

    int getSizeSpecialDynamicSize_() const;
    bool findBestWrapSpecialDynamicSize_() const;
    int wrapSpecialDynamicSize_(int width, bool includeTail) const;
    void afterCalcSize() const;

    GroupItem* createItem(QAction* action);
    QWidget* insertToolButton(RibbonGroup* rg, QAction* action, QToolButton* button);

private:
    friend class RibbonGroup;
    friend class RibbonGroupPrivate;
    Q_DISABLE_COPY(GroupDefaultLayout)
};

class RibbonGroupOption;
class RibbonGroupScroll;
/* RibbonGroupPrivate */
class RibbonGroupPrivate : public QObject
{
    Q_OBJECT
public:
    struct LayoutItemGroupInfo
    {
        GroupItem* item;
    };
    struct LayoutGroupInfo
    {
        LayoutItemGroupInfo* itemInfo;
        int widgetCount;
        int minWidth;
        int width;
        bool oldReduced;
    };

public:
    QTN_DECLARE_PUBLIC(RibbonGroup)
public:
    explicit RibbonGroupPrivate();

public:
    void init();
    void layoutWidgets();
    void enableGroupScroll(bool scrollLeft, bool scrollRight);
    void showGroupScroll();
    void startLayout();
    void finalLayout();
    bool reduceSize(int level, int width);

    void initStyleOption(QStyleOptionGroupBox& opt) const;
    bool extendSize(int widthAvail);
    void resetLayout();

    QWidget* getNextWidget(const QWidget* w) const;
    QWidget* getWidget(int index) const;
    int getIndexWidget(const QWidget* w) const;
    int getNextIndex(const QWidget* w) const;

public slots:
    void pressLeftScrollButton();
    void pressRightScrollButton();

public:
    RibbonGroupOption* m_butOption;
    RibbonGroupScroll* m_buttonScrollGroupLeft;    // Button to draw left scroll
    RibbonGroupScroll* m_buttonScrollGroupRight;   // Button to draw right scroll

    QAction* m_actOption;
    QString m_title;
    QIcon m_icon;
    LayoutGroupInfo* m_info;
    int m_minimizedLavel;
    int m_allowReduce;
    int m_pageScrollPos;
    int m_groupScrollPos;         // Scroll position
    bool m_reduced           : 1; // true if group was reduced
    bool m_controlsGrouping  : 1; // true to group controls
    bool m_controlsCentering : 1; // true to center controls inside groups

private:
    friend class RibbonGroupWrapper;
    Q_DISABLE_COPY(RibbonGroupPrivate)
};

QTITAN_END_NAMESPACE

#endif // QTN_RIBBONGROUPPRIVATE_H


