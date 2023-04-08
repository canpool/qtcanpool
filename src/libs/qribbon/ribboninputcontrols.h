/**
 * Copyleft (C) 2023 maminjie <canpool@163.com>
 **/
#pragma once

#include "ribbon_global.h"
#include "ribboncontrols.h"
#include "ribbonsliderpane.h"
#include <QComboBox>
#include <QDateEdit>
#include <QDateTimeEdit>
#include <QFontComboBox>
#include <QLineEdit>
#include <QSlider>
#include <QSpinBox>
#include <QTimeEdit>

QRIBBON_BEGIN_NAMESPACE

/* RibbonFontComboBoxControl */
class QRIBBON_EXPORT RibbonFontComboBoxControl : public RibbonWidgetControl
{
public:
    explicit RibbonFontComboBoxControl(RibbonGroup *parentGroup = Q_NULL);
    virtual ~RibbonFontComboBoxControl();
public:
    QFontComboBox *widget() const;
private:
    Q_DISABLE_COPY(RibbonFontComboBoxControl)
};

/* RibbonLineEditControl */
class QRIBBON_EXPORT RibbonLineEditControl : public RibbonWidgetControl
{
public:
    explicit RibbonLineEditControl(RibbonGroup *parentGroup = Q_NULL);
    virtual ~RibbonLineEditControl();
public:
    QLineEdit *widget() const;
private:
    Q_DISABLE_COPY(RibbonLineEditControl)
};

/* RibbonComboBoxControl */
class QRIBBON_EXPORT RibbonComboBoxControl : public RibbonWidgetControl
{
public:
    explicit RibbonComboBoxControl(RibbonGroup *parentGroup = Q_NULL);
    virtual ~RibbonComboBoxControl();
public:
    QComboBox *widget() const;
private:
    Q_DISABLE_COPY(RibbonComboBoxControl)
};

/* RibbonSpinBoxControl */
class QRIBBON_EXPORT RibbonSpinBoxControl : public RibbonWidgetControl
{
public:
    explicit RibbonSpinBoxControl(RibbonGroup *parentGroup = Q_NULL);
    virtual ~RibbonSpinBoxControl();
public:
    QSpinBox *widget() const;
private:
    Q_DISABLE_COPY(RibbonSpinBoxControl)
};

/* RibbonDoubleSpinBoxControl */
class QRIBBON_EXPORT RibbonDoubleSpinBoxControl : public RibbonWidgetControl
{
public:
    explicit RibbonDoubleSpinBoxControl(RibbonGroup *parentGroup = Q_NULL);
    virtual ~RibbonDoubleSpinBoxControl();
public:
    QDoubleSpinBox *widget() const;
private:
    Q_DISABLE_COPY(RibbonDoubleSpinBoxControl)
};

/* RibbonSliderControl */
class QRIBBON_EXPORT RibbonSliderControl : public RibbonWidgetControl
{
public:
    explicit RibbonSliderControl(Qt::Orientation, RibbonGroup *parentGroup = Q_NULL);
    virtual ~RibbonSliderControl();
public:
    QSlider *widget() const;
private:
    Q_DISABLE_COPY(RibbonSliderControl)
};

/* RibbonSliderPaneControl */
class QRIBBON_EXPORT RibbonSliderPaneControl : public RibbonWidgetControl
{
public:
    explicit RibbonSliderPaneControl(RibbonGroup *parentGroup = Q_NULL);
    virtual ~RibbonSliderPaneControl();
public:
    RibbonSliderPane *widget() const;
private:
    Q_DISABLE_COPY(RibbonSliderPaneControl)
};

/* RibbonDateTimeEditControl */
class QRIBBON_EXPORT RibbonDateTimeEditControl : public RibbonWidgetControl
{
public:
    explicit RibbonDateTimeEditControl(RibbonGroup *parentGroup = Q_NULL);
    virtual ~RibbonDateTimeEditControl();
public:
    QDateTimeEdit *widget() const;
    bool calendarPopup() const;
    void setCalendarPopup(bool enable);
private:
    Q_DISABLE_COPY(RibbonDateTimeEditControl)
};

/* RibbonTimeEditControl */
class QRIBBON_EXPORT RibbonTimeEditControl : public RibbonWidgetControl
{
public:
    explicit RibbonTimeEditControl(RibbonGroup *parentGroup = Q_NULL);
    virtual ~RibbonTimeEditControl();
public:
    QTimeEdit *widget() const;
private:
    Q_DISABLE_COPY(RibbonTimeEditControl)
};

/* RibbonDateEditControl */
class QRIBBON_EXPORT RibbonDateEditControl : public RibbonWidgetControl
{
public:
    explicit RibbonDateEditControl(RibbonGroup *parentGroup = Q_NULL);
    virtual ~RibbonDateEditControl();
public:
    QDateEdit *widget() const;
    bool calendarPopup() const;
    void setCalendarPopup(bool enable);
private:
    Q_DISABLE_COPY(RibbonDateEditControl)
};

QRIBBON_END_NAMESPACE
