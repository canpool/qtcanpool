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
#ifndef QTN_RIBBONINPUTCONTROLS_H
#define QTN_RIBBONINPUTCONTROLS_H

#include <QComboBox>
#include <QFontComboBox>
#include <QLineEdit>
#include <QSpinBox>
#include <QSlider>
#include <QDateTimeEdit>
#include <QTimeEdit>
#include <QDateEdit>

#include "QtnRibbonControls.h"
#include "QtnRibbonSliderPane.h"
#include "QtitanDef.h"

QTITAN_BEGIN_NAMESPACE


/* RibbonFontComboBoxControl */
class QTITAN_EXPORT RibbonFontComboBoxControl : public RibbonWidgetControl
{
public:
    explicit RibbonFontComboBoxControl(RibbonGroup* parentGroup = Q_NULL);
    virtual ~RibbonFontComboBoxControl();

public:
    QFontComboBox* widget() const;

private:
    Q_DISABLE_COPY(RibbonFontComboBoxControl)
};


/* RibbonLineEditControl */
class QTITAN_EXPORT RibbonLineEditControl : public RibbonWidgetControl
{
public: 
    explicit RibbonLineEditControl(RibbonGroup* parentGroup = Q_NULL);
    virtual ~RibbonLineEditControl();

public:
    QLineEdit* widget() const;

private:
    Q_DISABLE_COPY(RibbonLineEditControl)
};

/* RibbonComboBoxControl */
class QTITAN_EXPORT RibbonComboBoxControl : public RibbonWidgetControl
{
public: 
    explicit RibbonComboBoxControl(RibbonGroup* parentGroup = Q_NULL);
    virtual ~RibbonComboBoxControl();

public:
    QComboBox* widget() const;

private:
    Q_DISABLE_COPY(RibbonComboBoxControl)
};

/* RibbonSpinBoxControl */
class QTITAN_EXPORT RibbonSpinBoxControl : public RibbonWidgetControl
{
public: 
    explicit RibbonSpinBoxControl(RibbonGroup* parentGroup = Q_NULL);
    virtual ~RibbonSpinBoxControl();

public:
    QSpinBox* widget() const;

private:
    Q_DISABLE_COPY(RibbonSpinBoxControl)
};

/* RibbonDoubleSpinBoxControl */
class QTITAN_EXPORT RibbonDoubleSpinBoxControl : public RibbonWidgetControl
{
public: 
    explicit RibbonDoubleSpinBoxControl(RibbonGroup* parentGroup = Q_NULL);
    virtual ~RibbonDoubleSpinBoxControl();

public:
    QDoubleSpinBox* widget() const;

private:
    Q_DISABLE_COPY(RibbonDoubleSpinBoxControl)
};

/* RibbonSliderControl */
class QTITAN_EXPORT RibbonSliderControl : public RibbonWidgetControl
{
public: 
    explicit RibbonSliderControl(Qt::Orientation, RibbonGroup* parentGroup = Q_NULL);
    virtual ~RibbonSliderControl();

public:
    QSlider* widget() const;

private:
    Q_DISABLE_COPY(RibbonSliderControl)
};

/* RibbonSliderPaneControl */
class QTITAN_EXPORT RibbonSliderPaneControl : public RibbonWidgetControl
{
public: 
    explicit RibbonSliderPaneControl(RibbonGroup* parentGroup = Q_NULL);
    virtual ~RibbonSliderPaneControl();

public:
    RibbonSliderPane* widget() const;

private:
    Q_DISABLE_COPY(RibbonSliderPaneControl)
};

/* RibbonDateTimeEditControl */
class QTITAN_EXPORT RibbonDateTimeEditControl : public RibbonWidgetControl
{
public:
    explicit RibbonDateTimeEditControl(RibbonGroup* parentGroup = Q_NULL);
    virtual ~RibbonDateTimeEditControl();

public:
    QDateTimeEdit* widget() const;
    bool calendarPopup() const;
    void setCalendarPopup(bool enable);

private:
    Q_DISABLE_COPY(RibbonDateTimeEditControl)
};

/* RibbonTimeEditControl */
class QTITAN_EXPORT RibbonTimeEditControl : public RibbonWidgetControl
{
public:
    explicit RibbonTimeEditControl(RibbonGroup* parentGroup = Q_NULL);
    virtual ~RibbonTimeEditControl();

public:
    QTimeEdit* widget() const;

private:
    Q_DISABLE_COPY(RibbonTimeEditControl)
};

/* RibbonDateEditControl */
class QTITAN_EXPORT RibbonDateEditControl : public RibbonWidgetControl
{
public:
    explicit RibbonDateEditControl(RibbonGroup* parentGroup = Q_NULL);
    virtual ~RibbonDateEditControl();

public:
    QDateEdit* widget() const;
    bool calendarPopup() const;
    void setCalendarPopup(bool enable);

private:
    Q_DISABLE_COPY(RibbonDateEditControl)
};


QTITAN_END_NAMESPACE


#endif // QTN_RIBBONINPUTCONTROLS_H


