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
#ifndef QTN_RIBBONBUTTON_H
#define QTN_RIBBONBUTTON_H

#include <QStyle>
#include <QToolButton>

#include "QtitanDef.h"

class QStyleOption;

QTITAN_BEGIN_NAMESPACE

/* RibbonButton */
class RibbonButton : public QToolButton
{
    Q_OBJECT
public:
    RibbonButton(QWidget* parent = Q_NULL);
    virtual ~RibbonButton();

public:
    virtual QSize sizeHint() const;
private:
    Q_DISABLE_COPY(RibbonButton)
};

/* RibbonSeparator */
class RibbonSeparator : public QWidget
{
    Q_OBJECT
public:
    explicit RibbonSeparator(QWidget* parent = Q_NULL);

public:
    virtual QSize sizeHint() const;

protected:
    virtual void paintEvent(QPaintEvent*);

protected:
    void initStyleOption(QStyleOption* option) const;

private:
    Q_DISABLE_COPY(RibbonSeparator)
};


/* RibbonTitleButton */
class RibbonTitleButton : public QToolButton
{
    Q_OBJECT
public:
    RibbonTitleButton(QWidget* parent, QStyle::SubControl subControl);
    virtual ~RibbonTitleButton();

public:
    QStyle::SubControl getSubControl() const;

protected:
    virtual void paintEvent(QPaintEvent*);

private:
    QStyle::SubControl m_subControl;
};

QTITAN_END_NAMESPACE

#endif // QTN_RIBBONBUTTON_H
