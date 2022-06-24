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
#ifndef QTN_RIBBONCONTROLSPRIVATE_H
#define QTN_RIBBONCONTROLSPRIVATE_H

#include <QObject>

#include "QtnRibbonControls.h"
#include "QtnRibbonButtonControls.h"
#include "QtitanDef.h"

QTITAN_BEGIN_NAMESPACE

/* RibbonGroupPrivate */   
class RibbonControlSizeDefinitionPrivate : public QObject
{
    Q_OBJECT
public:
    QTN_DECLARE_PUBLIC(RibbonControlSizeDefinition)
public:
    explicit RibbonControlSizeDefinitionPrivate();
    virtual ~RibbonControlSizeDefinitionPrivate();
    void init(RibbonControl* parent, RibbonControlSizeDefinition::GroupSize size);
    void updateSizeDefinition();

public:
    RibbonControl* m_parentControl;
    RibbonControlSizeDefinition::ControlImageSize m_imageSize;
    bool m_labelVisible : 1;
    bool m_popup : 1;
    bool m_showSeparator : 1;
    bool m_visible : 1;
    bool m_enabled : 1;
    bool m_stretchable : 1;
    int m_visualIndex;
    int m_minItemCount;
    int m_maxItemCount;

private:
    Q_DISABLE_COPY(RibbonControlSizeDefinitionPrivate)
};

/* RibbonControlPrivate */   
class RibbonControlPrivate : public QObject
{
    Q_OBJECT
public:
    QTN_DECLARE_PUBLIC(RibbonControl)
public:
    explicit RibbonControlPrivate();
    virtual ~RibbonControlPrivate();
    void init(RibbonGroup* parentGroup);
    void changed();

public:
    RibbonGroup* m_parentGroup;
    QList<RibbonControlSizeDefinition*> m_allSizes;
    RibbonControlSizeDefinition::GroupSize m_size;
    QAction* m_action;

private:
    Q_DISABLE_COPY(RibbonControlPrivate)
};

/* RibbonWidgetControlPrivate */   
class RibbonWidgetControlPrivate : public QObject
{
    Q_OBJECT
public:
    QTN_DECLARE_PUBLIC(RibbonWidgetControl)
public:
    explicit RibbonWidgetControlPrivate();
    virtual ~RibbonWidgetControlPrivate();
    void init(bool ignoreActionSettings);
    void initStyleOption(QStyleOptionButton* option) const;
    void clearWidget();

public:
    QWidget* m_widget;
    bool m_visibleText;
    bool m_ignoreActionSettings;
    int m_margin;

private:
    Q_DISABLE_COPY(RibbonWidgetControlPrivate)
};


/* RibbonColumnBreakControlPrivate */   
class RibbonColumnBreakControlPrivate : public QObject
{
    Q_OBJECT
public:
    QTN_DECLARE_PUBLIC(RibbonColumnBreakControl)
public:
    explicit RibbonColumnBreakControlPrivate();
    virtual ~RibbonColumnBreakControlPrivate();
    void init();
    void initStyleOption(QStyleOptionGroupBox* option) const;

public:
    Qt::Alignment m_textAlignment;

private:
    Q_DISABLE_COPY(RibbonColumnBreakControlPrivate)
};

/* RibbonButtonControlPrivate */   
class RibbonButtonControlPrivate : public QObject
{
    Q_OBJECT
public:
    QTN_DECLARE_PUBLIC(RibbonButtonControl)
public:
    explicit RibbonButtonControlPrivate();
    virtual ~RibbonButtonControlPrivate();
    void init();

public:
    QToolButton* m_toolButton;
    QPixmap m_largeIcon;
    QPixmap m_smallIcon;
    QString m_label;

private:
    Q_DISABLE_COPY(RibbonButtonControlPrivate)
};

/* RibbonToolBarSeparatorControlPrivate */
class RibbonToolBarSeparatorControlPrivate : public QWidget
{
    Q_OBJECT
public:
    explicit RibbonToolBarSeparatorControlPrivate(RibbonToolBarControl* parent);
public:
    virtual QSize sizeHint() const;
protected:
    virtual void paintEvent(QPaintEvent* event);
private:
    Q_DISABLE_COPY(RibbonToolBarSeparatorControlPrivate)
};

/* RibbonToolBarControlPrivate */   
class RibbonToolBarControlPrivate : public QObject
{
    Q_OBJECT
public:
    QTN_DECLARE_PUBLIC(RibbonToolBarControl)
    struct RibbonToolBarActionGroupItem
    {
        QAction* action;
        QWidget* widget;
        RibbonToolBarActionGroupItem(QAction* act, QWidget* w) : action(act), widget(w) {}
        RibbonToolBarActionGroupItem(const RibbonToolBarActionGroupItem& other) : action(other.action), widget(other.widget) {}
        bool isEmpty() const { return action == 0 || !action->isVisible();  }
    };
    typedef QList<RibbonToolBarActionGroupItem> RibbonToolBarActionGroupItems;

    struct RibbonToolBarActionsGroup
    {
        int row;
        QRect rect;
        RibbonToolBarActionGroupItems items;
        RibbonToolBarActionsGroup() : row(0) {}
    };

    typedef QList<RibbonToolBarActionsGroup> RibbonToolBarActionGroupList;

public:
    explicit RibbonToolBarControlPrivate();
    virtual ~RibbonToolBarControlPrivate();

public:
    void recalcWidths(int height);
    void showDefaultButton();
    void addActionToGroup(QAction* action, QWidget* widget);
    void removeActionFromGroup(QAction* action);
    void updateGroupsGeometry(const QRect& rect);
    void updateGroupProperties(RibbonToolBarActionGroupItems& group);
    void updateGeometryWidget();
    void updateLayoutParent(bool calcLayoutParent = true);
    QSize calculateSizeHint(const RibbonToolBarActionGroupItems& group) const;
    bool isMixedWidgets() const;
public:
    RibbonToolBarActionGroupList m_groups;
    int m_margin;
    int m_rowsCount;
    QSize m_sizeHint;

    QVector<int> m_vecWidths;
    QMap<int, int> m_mapNonOptWidths;
    bool m_isCalcWidth;
    bool m_dirty;
    int m_calcRows;
    int m_fullWidth;
    QRect m_rect;

private:
    Q_DISABLE_COPY(RibbonToolBarControlPrivate)
};


QTITAN_END_NAMESPACE


#endif // QTN_RIBBONCONTROLSPRIVATE_H


