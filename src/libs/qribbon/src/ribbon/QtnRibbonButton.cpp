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
#include <QAction>
#include <QApplication>
#include <QStyleOption>
#include <QActionEvent>
#include <QPainter>

#include "QtnRibbonButton.h"
#include "QtnStyleHelpers.h"
#include "QtnRibbonStyle.h"
#include "QtnRibbonGroup.h"
#include "../src/styles/QtnCommonStyle.h"

QTITAN_USE_NAMESPACE

/*!
    \class Qtitan::RibbonButton
    \internal
*/
RibbonButton::RibbonButton(QWidget* parent)
    : QToolButton(parent)
{
    setAutoRaise(true);
}

RibbonButton::~RibbonButton()
{
}

QSize RibbonButton::sizeHint() const
{
    ensurePolished();

    int w = 0, h = 0;
    QStyleOptionToolButton opt;
    initStyleOption(&opt);

    QFontMetrics fm = fontMetrics();
    if (opt.toolButtonStyle != Qt::ToolButtonTextOnly)
    {
        QSize icon = opt.iconSize;
        QAction* action = defaultAction();
        if (action && !action->icon().isNull())
            icon = action->icon().actualSize(QSize(32,32));
        w = icon.width();
        h = icon.height();
    }

    bool indicatorCenter = false;
    QToolButton::ToolButtonPopupMode mode = QToolButton::popupMode();
    if (opt.toolButtonStyle != Qt::ToolButtonIconOnly)
    {
        if (opt.toolButtonStyle == Qt::ToolButtonTextUnderIcon)
        {
            QString strFirstRow, strSecondRow;
            CommonStyle::splitString(text(), strFirstRow, strSecondRow);

            QSize textFirstSize;
            if (!strFirstRow.isEmpty())
                textFirstSize = fm.size(Qt::TextShowMnemonic, strFirstRow);

            if (!strSecondRow.isEmpty())
            {
                QSize textSecondSize = fm.size(Qt::TextShowMnemonic, strSecondRow);
                textFirstSize.setWidth(qMax(textFirstSize.width(), textSecondSize.width()));
            }

            indicatorCenter = strSecondRow.isEmpty() && opt.features & QStyleOptionToolButton::HasMenu;

            h = style()->pixelMetric((QStyle::PixelMetric)RibbonStyle::PM_RibbonHeightGroup, 0, this);

            // if the text is more than icon
            if (textFirstSize.width() > w)
                w = textFirstSize.width();
            // add pixel
            w += mode == MenuButtonPopup || mode == QToolButton::InstantPopup ? 4 : 10;
        } 
        else
        {
            QSize textSize = fm.size(Qt::TextShowMnemonic, text());
            textSize.setWidth(textSize.width() + fm.width(QLatin1Char(' '))*2);
            h = style()->pixelMetric((QStyle::PixelMetric)RibbonStyle::PM_RibbonHeightGroup, 0, 0)/3;

            if (opt.toolButtonStyle == Qt::ToolButtonTextBesideIcon)
            {
                w += 3 + textSize.width();
                if (mode != InstantPopup)
                    w += 3;

                if (textSize.height() > h)
                   h = textSize.height();
            } 
            else 
            {
                w = textSize.width() + 3;
            }
        }
    }
    else 
    {
        h = style()->pixelMetric((QStyle::PixelMetric)RibbonStyle::PM_RibbonHeightGroup, 0, 0)/3;
        w = h;
    }

    opt.rect.setSize(QSize(w, h)); // PM_MenuButtonIndicator depends on the height

    if ((mode== MenuButtonPopup || mode == QToolButton::InstantPopup) && !indicatorCenter)
        w += style()->pixelMetric(QStyle::PM_MenuButtonIndicator, &opt, this);

    return QSize(w, h).expandedTo(QApplication::globalStrut());
}


/* RibbonSeparator */
RibbonSeparator::RibbonSeparator(QWidget* parent)
    : QWidget(parent)
{
    setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
}

QSize RibbonSeparator::sizeHint() const
{
    if (RibbonGroup* rg = qobject_cast<RibbonGroup*>(parentWidget()))
    {
        if (!rg->isControlsGrouping())
        {
            QStyleOption opt;
            initStyleOption(&opt);
            int height = style()->pixelMetric((QStyle::PixelMetric)RibbonStyle::PM_RibbonHeightGroup, Q_NULL, Q_NULL);
            const int extent = style()->pixelMetric(QStyle::PM_ToolBarSeparatorExtent, &opt, parentWidget());
            return QSize(extent, height);
        }
    }
    return QSize(0, 0);
}

void RibbonSeparator::paintEvent(QPaintEvent* event)
{
    Q_UNUSED(event);
    if (RibbonGroup* rg = qobject_cast<RibbonGroup*>(parentWidget()))
    {
        if (!rg->isControlsGrouping())
        {
            QPainter p(this);
            QStyleOption opt;
            initStyleOption(&opt);
            style()->drawPrimitive(QStyle::PE_IndicatorToolBarSeparator, &opt, &p, parentWidget());
        }
    }
}

void RibbonSeparator::initStyleOption(QStyleOption* option) const
{
    option->initFrom(this);
    option->state |= QStyle::State_Horizontal;
}



/* RibbonTitleButton */
RibbonTitleButton::RibbonTitleButton(QWidget* parent, QStyle::SubControl subControl)
    : QToolButton(parent)
{
    m_subControl = subControl;
    setAttribute(Qt::WA_Hover, true);
}

RibbonTitleButton::~RibbonTitleButton()
{
}

QStyle::SubControl RibbonTitleButton::getSubControl() const
{
    return m_subControl;
}

void RibbonTitleButton::paintEvent(QPaintEvent* event)
{
    Q_UNUSED(event);

    QPainter p(this);
    StyleOptionTitleBar opt;
    opt.init(this);
    opt.activeSubControls = m_subControl;
    style()->drawPrimitive(QStyle::PE_PanelButtonTool, &opt, &p, this);
}
