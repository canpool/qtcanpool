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
#ifndef QTN_RIBBONSTATUSBAR_H
#define QTN_RIBBONSTATUSBAR_H

#include <QStatusBar>

#include "QtitanDef.h"

QTITAN_BEGIN_NAMESPACE

class RibbonStatusBarSwitchGroupPrivate;
/* RibbonStatusBarSwitchGroup */
class QTITAN_EXPORT RibbonStatusBarSwitchGroup : public QWidget
{
    Q_OBJECT
public:
    explicit RibbonStatusBarSwitchGroup();
    virtual ~RibbonStatusBarSwitchGroup();

public:
    void clear();

protected:
    virtual void actionEvent(QActionEvent* event);
private:
    QTN_DECLARE_PRIVATE(RibbonStatusBarSwitchGroup)
    Q_DISABLE_COPY(RibbonStatusBarSwitchGroup)
};

class RibbonStatusBarPrivate;
/* RibbonStatusBar */
class QTITAN_EXPORT RibbonStatusBar : public QStatusBar
{
    Q_OBJECT
public:
    explicit RibbonStatusBar(QWidget* parent = Q_NULL);
    virtual ~RibbonStatusBar();

public:
    QAction* addAction(const QString& text);
    QAction* addSeparator();

    void addPermanentAction(QAction* action);

    QRect childrenPermanentRect() const;

public:
    using QWidget::addAction;

protected:
    virtual bool event(QEvent* event);
    virtual void actionEvent(QActionEvent* event);

private:
    QTN_DECLARE_PRIVATE(RibbonStatusBar)
    Q_DISABLE_COPY(RibbonStatusBar)
};

QTITAN_END_NAMESPACE

#endif // QTN_RIBBONSTATUSBAR_H
