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
#ifndef QTN_POPUPCOLORBUTTON_H
#define QTN_POPUPCOLORBUTTON_H

#include <QToolButton>

#include "QtitanDef.h"

class QStyleOption;

QTITAN_BEGIN_NAMESPACE

/* PopupColorButton */
class QTITAN_EXPORT PopupColorButton : public QToolButton
{
    Q_OBJECT
    Q_PROPERTY(QColor color READ color WRITE setColor)
public:
    PopupColorButton(QWidget* parent = Q_NULL);
    virtual ~PopupColorButton();

public:
    QColor color() const;
    void setColor(const QColor& color);

Q_SIGNALS:
    void colorChanged(const QColor& color);

protected:
    virtual void paintEvent     (QPaintEvent* event);
    virtual void mousePressEvent(QMouseEvent* event);

protected:
    QColor m_color;
};

QTITAN_END_NAMESPACE

#endif // QTN_POPUPCOLORBUTTON_H
