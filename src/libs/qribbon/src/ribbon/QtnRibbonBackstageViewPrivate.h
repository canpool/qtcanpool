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
#ifndef QTN_RIBBONBACKSTAGEPVIEWRIVATE_H
#define QTN_RIBBONBACKSTAGEPVIEWRIVATE_H

#include <QObject>
#include <QTimer>
#include <QHash>

#include "QtnRibbonBackstageView.h"

class QStyleOptionMenuItem;
class QWidgetAction;

QTITAN_BEGIN_NAMESPACE

class RibbonBar;
class HackFilter;
/* RibbonBackstageViewPrivate */
class RibbonBackstageViewPrivate : public QObject
{
public:
    Q_OBJECT
    QTN_DECLARE_PUBLIC(RibbonBackstageView)
public:
    explicit RibbonBackstageViewPrivate();
    virtual ~RibbonBackstageViewPrivate();

public:
    void init(QWidget* parent);
    void initStyleOption(QStyleOptionMenuItem* option, const QAction* action) const;
    void layoutBackstage();
    void updateActionRects() const;
    void updateGeometryPage(QWidget* widget);

    void setCurrentAction(QAction *action);
    QAction* currentAction() const;

    QRect actionRect(QAction* act) const;

    QWidgetAction* getAction(QWidget* w) const;
    QAction* actionAt(const QPoint& p) const;

protected slots:
    void trackMouseEvent();

public:
    RibbonBar* ribbon_;
    QWidget* activePage_;
    QAction* currentAction_;

    bool mouseDown_;
    bool closePrevented_;

    QTimer timer_;
    mutable bool itemsDirty_;
    mutable int menuWidth_;
    mutable int maxIconWidth_;
    mutable QVector<QRect> actionRects_;
    mutable QHash<QAction*, QWidget*> widgetItems_;
};

QTITAN_END_NAMESPACE

#endif // QTN_RIBBONBACKSTAGEPVIEWRIVATE_H

