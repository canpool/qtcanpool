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
#ifndef QTN_RIBBONSLIDERPANE_H
#define QTN_RIBBONSLIDERPANE_H

#include <QWidget>

#include "QtitanDef.h"

QTITAN_BEGIN_NAMESPACE

class RibbonSliderPanePrivate;

/* RibbonSliderPane */
class QTITAN_EXPORT RibbonSliderPane : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(int singleStep READ singleStep WRITE setSingleStep)
    Q_PROPERTY(int value READ value WRITE setValue NOTIFY valueChanged USER true)
    Q_PROPERTY(int sliderPosition READ sliderPosition WRITE setSliderPosition NOTIFY sliderMoved)

public:
    RibbonSliderPane(QWidget* parent = Q_NULL);
    virtual ~RibbonSliderPane();

public:
    void setRange(int min, int max);
    void setScrollButtons(bool on);

    void setSliderPosition(int);
    int sliderPosition() const;

    void setSingleStep(int);
    int singleStep() const;

    int value() const;

public Q_SLOTS:
    void setValue(int);

Q_SIGNALS:
    void valueChanged(int value);
    void sliderMoved(int position);

protected Q_SLOTS:
    void increment();
    void decrement();

private:
    QTN_DECLARE_PRIVATE(RibbonSliderPane)
    Q_DISABLE_COPY(RibbonSliderPane)
};

QTITAN_END_NAMESPACE

#endif // QTN_RIBBONSLIDERPANE_H
