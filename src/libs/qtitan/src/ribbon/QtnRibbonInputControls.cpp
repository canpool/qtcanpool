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
#include "QtnRibbonInputControls.h"
#ifdef DEBUG_MEMORY_ENABLED
#include "QtitanMSVSDebug.h"
#endif

QTITAN_USE_NAMESPACE



/* RibbonFontComboBoxControl */
RibbonFontComboBoxControl::RibbonFontComboBoxControl(RibbonGroup* parentGroup)
    : RibbonWidgetControl(parentGroup, false/*ignoreActionSettings*/)
{
    setContentWidget(new QFontComboBox(this));
}

RibbonFontComboBoxControl::~RibbonFontComboBoxControl()
{
}

QFontComboBox* RibbonFontComboBoxControl::widget() const
{
    return qobject_cast<QFontComboBox*>(contentWidget());
}


/* RibbonLineEditControl */
RibbonLineEditControl::RibbonLineEditControl(RibbonGroup* parentGroup)
    : RibbonWidgetControl(parentGroup, false/*ignoreActionSettings*/)
{
    setContentWidget(new QLineEdit(this));
}

RibbonLineEditControl::~RibbonLineEditControl()
{
}

QLineEdit* RibbonLineEditControl::widget() const
{
    return qobject_cast<QLineEdit*>(contentWidget());
}

/* RibbonComboBoxControl */
RibbonComboBoxControl::RibbonComboBoxControl(RibbonGroup* parentGroup)
    : RibbonWidgetControl(parentGroup, false/*ignoreActionSettings*/)
{
    setContentWidget(new QComboBox(this));
}

RibbonComboBoxControl::~RibbonComboBoxControl()
{
}

QComboBox* RibbonComboBoxControl::widget() const
{
    return qobject_cast<QComboBox*>(contentWidget());
}

/* RibbonSpinBoxControl */
RibbonSpinBoxControl::RibbonSpinBoxControl(RibbonGroup* parentGroup)
    : RibbonWidgetControl(parentGroup, false/*ignoreActionSettings*/)
{
    setContentWidget(new QSpinBox(this));
}

RibbonSpinBoxControl::~RibbonSpinBoxControl()
{
}

QSpinBox* RibbonSpinBoxControl::widget() const
{
    return qobject_cast<QSpinBox*>(contentWidget());
}

/* RibbonDoubleSpinBoxControl */
RibbonDoubleSpinBoxControl::RibbonDoubleSpinBoxControl(RibbonGroup* parentGroup)
    : RibbonWidgetControl(parentGroup, false/*ignoreActionSettings*/)
{
    setContentWidget(new QDoubleSpinBox(this));
}

RibbonDoubleSpinBoxControl::~RibbonDoubleSpinBoxControl()
{
}

QDoubleSpinBox* RibbonDoubleSpinBoxControl::widget() const
{
    return qobject_cast<QDoubleSpinBox*>(contentWidget());
}


/* RibbonSliderControl */
RibbonSliderControl::RibbonSliderControl(Qt::Orientation orientation, RibbonGroup* parentGroup)
    : RibbonWidgetControl(parentGroup, true/*ignoreActionSettings*/)
{
    setContentWidget(new QSlider(orientation, this));
}

RibbonSliderControl::~RibbonSliderControl()
{
}

QSlider* RibbonSliderControl::widget() const
{
    return qobject_cast<QSlider*>(contentWidget());
}

/* RibbonSliderPaneControl */
RibbonSliderPaneControl::RibbonSliderPaneControl(RibbonGroup* parentGroup)
    : RibbonWidgetControl(parentGroup, true/*ignoreActionSettings*/)
{
    setContentWidget(new RibbonSliderPane(this));
}

RibbonSliderPaneControl::~RibbonSliderPaneControl()
{
}

RibbonSliderPane* RibbonSliderPaneControl::widget() const
{
    return qobject_cast<RibbonSliderPane*>(contentWidget());
}


/* RibbonDateTimeEditControl */
RibbonDateTimeEditControl::RibbonDateTimeEditControl(RibbonGroup* parentGroup)
    : RibbonWidgetControl(parentGroup, false/*ignoreActionSettings*/)
{
    setContentWidget(new QDateTimeEdit(this));
}

RibbonDateTimeEditControl::~RibbonDateTimeEditControl()
{
}

QDateTimeEdit* RibbonDateTimeEditControl::widget() const
{
    return qobject_cast<QDateTimeEdit*>(contentWidget());
}

bool RibbonDateTimeEditControl::calendarPopup() const
{
    QDateTimeEdit* dateTimeEdit = widget();
    Q_ASSERT(dateTimeEdit != Q_NULL);
    return dateTimeEdit->calendarPopup();
}

void RibbonDateTimeEditControl::setCalendarPopup(bool enable)
{
    QDateTimeEdit* dateTimeEdit = widget();
    Q_ASSERT(dateTimeEdit != Q_NULL);
    dateTimeEdit->setCalendarPopup(enable);
}


/* RibbonTimeEditControl */
RibbonTimeEditControl::RibbonTimeEditControl(RibbonGroup* parentGroup)
    : RibbonWidgetControl(parentGroup, false/*ignoreActionSettings*/)
{
    setContentWidget(new QTimeEdit(this));
}

RibbonTimeEditControl::~RibbonTimeEditControl()
{
}

QTimeEdit* RibbonTimeEditControl::widget() const
{
    return qobject_cast<QTimeEdit*>(contentWidget());
}

/* RibbonDateEditControl */
RibbonDateEditControl::RibbonDateEditControl(RibbonGroup* parentGroup)
    : RibbonWidgetControl(parentGroup, false/*ignoreActionSettings*/)
{
    setContentWidget(new QDateEdit(this));
}

RibbonDateEditControl::~RibbonDateEditControl()
{
}

QDateEdit* RibbonDateEditControl::widget() const
{
    return qobject_cast<QDateEdit*>(contentWidget());
}

bool RibbonDateEditControl::calendarPopup() const
{
    QDateEdit* dateEdit = widget();
    Q_ASSERT(dateEdit != Q_NULL);
    return dateEdit->calendarPopup();
}

void RibbonDateEditControl::setCalendarPopup(bool enable)
{
    QDateEdit* dateEdit = widget();
    Q_ASSERT(dateEdit != Q_NULL);
    dateEdit->setCalendarPopup(enable);
}
