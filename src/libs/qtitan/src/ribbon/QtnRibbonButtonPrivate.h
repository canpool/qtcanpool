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
#ifndef QTN_RIBBONBUTTON_H
#define QTN_RIBBONBUTTON_H

#include <QStyle>
#include <QToolButton>

#include "QtitanDef.h"


QTITAN_BEGIN_NAMESPACE

class RibbonButton : public QToolButton
{
    Q_OBJECT
public:
    RibbonButton(QWidget* parent = Q_NULL);
    virtual ~RibbonButton();
public:
    virtual QSize sizeHint() const;

protected:
    virtual bool event(QEvent* event);
    virtual void actionEvent(QActionEvent* event);
private:
    QSize m_sizeHint;
    Qt::ToolButtonStyle m_saveButtonStyle;
    Qt::ArrowType m_saveArrowType;
private:
    Q_DISABLE_COPY(RibbonButton)
};

/* RibbonTitleButton */
class RibbonTitleButton : public QToolButton
{
    Q_OBJECT
public:
    RibbonTitleButton(QWidget* parent, QStyle::SubControl subControl);
    virtual ~RibbonTitleButton();
public:
    QStyle::SubControl subControl() const;
protected:
    virtual void paintEvent(QPaintEvent*);
private:
    QStyle::SubControl m_subControl;
private:
    Q_DISABLE_COPY(RibbonTitleButton)
};

QTITAN_END_NAMESPACE

#endif // QTN_RIBBONBUTTON_H
