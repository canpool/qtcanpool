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
#ifndef QTN_RIBBONQUICKACCESSBAR_H
#define QTN_RIBBONQUICKACCESSBAR_H

#include <QToolBar>

#include "QtitanDef.h"

QTITAN_BEGIN_NAMESPACE

class RibbonQuickAccessBarPrivate;
/* RibbonQuickAccessBar */
class QTITAN_EXPORT RibbonQuickAccessBar : public QToolBar
{
    Q_OBJECT
public:
    RibbonQuickAccessBar(QWidget* parent = Q_NULL);
    virtual ~RibbonQuickAccessBar();

public:
    QAction* actionCustomizeButton() const;
    void setActionVisible(QAction* action, bool visible);
    bool isActionVisible(QAction* action) const;
    int getVisibleCount() const;
    void deleteAction(QAction *action);
public:
    virtual QSize sizeHint() const;

protected slots:
    void customizeAction(QAction*);

protected:
    virtual bool event(QEvent* event);
    virtual void actionEvent(QActionEvent*);
    virtual void paintEvent(QPaintEvent*);

private:
    QTN_DECLARE_PRIVATE(RibbonQuickAccessBar)
    Q_DISABLE_COPY(RibbonQuickAccessBar)
};

QTITAN_END_NAMESPACE

#endif // QTN_RIBBONQUICKACCESSBAR_H
