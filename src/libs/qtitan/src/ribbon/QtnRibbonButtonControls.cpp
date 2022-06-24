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
#include "QtnRibbonButtonControls.h"
#include "QtnRibbonGroup.h"
#include "QtnRibbonButtonPrivate.h"
#include "QtnRibbonControlsPrivate.h"
#ifdef DEBUG_MEMORY_ENABLED
#include "QtitanMSVSDebug.h"
#endif

QTITAN_USE_NAMESPACE


RibbonButtonControlPrivate::RibbonButtonControlPrivate()
    : m_toolButton(Q_NULL)
{
}

RibbonButtonControlPrivate::~RibbonButtonControlPrivate()
{
}

void RibbonButtonControlPrivate::init()
{
    QTN_P(RibbonButtonControl);
    p.sizeDefinition(RibbonControlSizeDefinition::GroupLarge)->setStretchable(false);
    p.sizeDefinition(RibbonControlSizeDefinition::GroupMedium)->setStretchable(false);
    p.sizeDefinition(RibbonControlSizeDefinition::GroupSmall)->setStretchable(false);
    p.sizeDefinition(RibbonControlSizeDefinition::GroupPopup)->setStretchable(false);

    m_toolButton = new RibbonButton(&p);
    p.setContentWidget(m_toolButton);
}


/* RibbonButtonControl */
RibbonButtonControl::RibbonButtonControl(RibbonGroup* parentGroup)
    : RibbonWidgetControl(parentGroup, true)
{
    QTN_INIT_PRIVATE(RibbonButtonControl);
    QTN_D(RibbonButtonControl);
    d.init();
}

RibbonButtonControl::~RibbonButtonControl()
{
    QTN_FINI_PRIVATE();
}

const QPixmap& RibbonButtonControl::largeIcon() const
{
    QTN_D(const RibbonButtonControl);
    return d.m_largeIcon;
}

void RibbonButtonControl::setLargeIcon(const QPixmap& icon)
{
    QTN_D(RibbonButtonControl);
    d.m_largeIcon = icon;
}

const QPixmap& RibbonButtonControl::smallIcon() const
{
    QTN_D(const RibbonButtonControl);
    return d.m_smallIcon;
}

void RibbonButtonControl::setSmallIcon(const QPixmap& icon)
{
    QTN_D(RibbonButtonControl);
    d.m_smallIcon = icon;
}

const QString& RibbonButtonControl::label() const
{
    QTN_D(const RibbonButtonControl);
    return d.m_label;
}

void RibbonButtonControl::setLabel(const QString& label)
{
    QTN_D(RibbonButtonControl);
    d.m_label = label;
}

void RibbonButtonControl::setMenu(QMenu* menu)
{
    QTN_D(RibbonButtonControl);
    d.m_toolButton->setMenu(menu);
}

QMenu* RibbonButtonControl::menu() const
{
    QTN_D(const RibbonButtonControl);
    return d.m_toolButton->menu();
}

void RibbonButtonControl::setPopupMode(QToolButton::ToolButtonPopupMode mode)
{
    QTN_D(RibbonButtonControl);
    d.m_toolButton->setPopupMode(mode);
}

QToolButton::ToolButtonPopupMode RibbonButtonControl::popupMode() const
{
    QTN_D(const RibbonButtonControl);
    return d.m_toolButton->popupMode();
}

void RibbonButtonControl::setToolButtonStyle(Qt::ToolButtonStyle style)
{
    QTN_D(RibbonButtonControl);
    d.m_toolButton->setToolButtonStyle(style);
    if (style == Qt::ToolButtonIconOnly)
    {
        sizeDefinition(RibbonControlSizeDefinition::GroupLarge)->setImageSize(RibbonControlSizeDefinition::ImageSmall);
        sizeDefinition(RibbonControlSizeDefinition::GroupLarge)->setLabelVisible(false);
        sizeDefinition(RibbonControlSizeDefinition::GroupMedium)->setImageSize(RibbonControlSizeDefinition::ImageSmall);
        sizeDefinition(RibbonControlSizeDefinition::GroupMedium)->setLabelVisible(false);
        sizeDefinition(RibbonControlSizeDefinition::GroupSmall)->setImageSize(RibbonControlSizeDefinition::ImageSmall);
        sizeDefinition(RibbonControlSizeDefinition::GroupSmall)->setLabelVisible(false);
        sizeDefinition(RibbonControlSizeDefinition::GroupPopup)->setImageSize(RibbonControlSizeDefinition::ImageSmall);
        sizeDefinition(RibbonControlSizeDefinition::GroupPopup)->setLabelVisible(false);
    }
    else if (style == Qt::ToolButtonTextOnly)
    {
        sizeDefinition(RibbonControlSizeDefinition::GroupLarge)->setImageSize(RibbonControlSizeDefinition::ImageNone);
        sizeDefinition(RibbonControlSizeDefinition::GroupLarge)->setLabelVisible(true);
        sizeDefinition(RibbonControlSizeDefinition::GroupMedium)->setImageSize(RibbonControlSizeDefinition::ImageNone);
        sizeDefinition(RibbonControlSizeDefinition::GroupMedium)->setLabelVisible(true);
        sizeDefinition(RibbonControlSizeDefinition::GroupSmall)->setImageSize(RibbonControlSizeDefinition::ImageNone);
        sizeDefinition(RibbonControlSizeDefinition::GroupSmall)->setLabelVisible(true);
        sizeDefinition(RibbonControlSizeDefinition::GroupPopup)->setImageSize(RibbonControlSizeDefinition::ImageNone);
        sizeDefinition(RibbonControlSizeDefinition::GroupPopup)->setLabelVisible(true);
    }
    else if (style == Qt::ToolButtonTextBesideIcon)
    {
        sizeDefinition(RibbonControlSizeDefinition::GroupLarge)->setImageSize(RibbonControlSizeDefinition::ImageSmall);
        sizeDefinition(RibbonControlSizeDefinition::GroupMedium)->setImageSize(RibbonControlSizeDefinition::ImageSmall);
        sizeDefinition(RibbonControlSizeDefinition::GroupSmall)->setImageSize(RibbonControlSizeDefinition::ImageSmall);
        sizeDefinition(RibbonControlSizeDefinition::GroupPopup)->setImageSize(RibbonControlSizeDefinition::ImageSmall);
    }
}

Qt::ToolButtonStyle RibbonButtonControl::toolButtonStyle() const
{
    QTN_D(const RibbonButtonControl);
    return d.m_toolButton->toolButtonStyle();
}

QToolButton* RibbonButtonControl::widget() const
{
    QTN_D(const RibbonButtonControl);
    return d.m_toolButton;
}

QSize RibbonButtonControl::sizeHint() const
{
    QTN_D(const RibbonButtonControl);
    QSize sz = RibbonWidgetControl::sizeHint();

    if (d.m_toolButton && parentGroup())
    {
        if (RibbonBar* ribbonBar = parentGroup()->ribbonBar())
        {
            Qt::ToolButtonStyle style = toolButtonStyle();
            if (style == Qt::ToolButtonTextBesideIcon || style == Qt::ToolButtonIconOnly || style == Qt::ToolButtonTextOnly)
                sz.setHeight(ribbonBar->rowItemHeight());
            else if (style == Qt::ToolButtonTextUnderIcon)
                sz.setHeight(ribbonBar->rowItemHeight() * ribbonBar->rowItemCount());
        }
    }

    return sz;
}

void RibbonButtonControl::sizeChanged(RibbonControlSizeDefinition::GroupSize size)
{
    QTN_D(RibbonButtonControl);
    RibbonControl::sizeChanged(size);

    d.m_toolButton->setDefaultAction(defaultAction());

    if (sizeDefinition(size)->imageSize() == RibbonControlSizeDefinition::ImageLarge)
    {
        const QPixmap& px = largeIcon();
        d.m_toolButton->setIconSize(px.size());
        d.m_toolButton->setIcon(px);
        d.m_toolButton->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    }
    else if (sizeDefinition(size)->imageSize() == RibbonControlSizeDefinition::ImageSmall)
    {
        const QPixmap& px = smallIcon();
        d.m_toolButton->setIconSize(px.size());
        d.m_toolButton->setIcon(px);
        d.m_toolButton->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);

        if (sizeDefinition(size)->isLabelVisible())
            d.m_toolButton->setText(label());
        else
            d.m_toolButton->setToolButtonStyle(Qt::ToolButtonIconOnly);
    }
}

void RibbonButtonControl::actionChanged()
{
    const QSize bugImageSize = QSize(32, 32);
    const QSize smallImageSize = QSize(16, 16);

    if (defaultAction() != Q_NULL)
    {
        if (!defaultAction()->iconText().isEmpty())
            setLabel(defaultAction()->iconText());
        else 
            setLabel(defaultAction()->text());

        setLargeIcon(defaultAction()->icon().pixmap(bugImageSize));
        setSmallIcon(defaultAction()->icon().pixmap(smallImageSize));
    }
    else
    {
        setLabel("Empty");
    }

    RibbonControl::actionChanged();
}



/* RibbonCheckBoxControl */
RibbonCheckBoxControl::RibbonCheckBoxControl(const QString& text, RibbonGroup* parentGroup)
    : RibbonWidgetControl(parentGroup, true/*ignoreActionSettings*/)
{
    QCheckBox* checkBox = new QCheckBox(text, this);
    setContentWidget(checkBox);
}

RibbonCheckBoxControl::~RibbonCheckBoxControl()
{
}

QCheckBox* RibbonCheckBoxControl::widget() const
{
    return qobject_cast<QCheckBox*>(contentWidget());
}

/* RibbonCheckBoxControl */
RibbonRadioButtonControl::RibbonRadioButtonControl(const QString& text, RibbonGroup* parentGroup)
    : RibbonWidgetControl(parentGroup, true/*ignoreActionSettings*/)
{
    QRadioButton* radioButton = new QRadioButton(text, this);
    setContentWidget(radioButton);
}

RibbonRadioButtonControl::~RibbonRadioButtonControl()
{
}

QRadioButton* RibbonRadioButtonControl::widget() const
{
    return qobject_cast<QRadioButton*>(contentWidget());
}
