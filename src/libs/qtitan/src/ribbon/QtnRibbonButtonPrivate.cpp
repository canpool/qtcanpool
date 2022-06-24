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
#include <QApplication>
#include <QPainter>

#include "QtnRibbonButtonPrivate.h"
#include "QtnRibbonBar.h"
#include "QtnRibbonGroup.h"
#include "QtnRibbonControls.h"
#include "QtnRibbonStyle.h"
#include "QtnStyleHelpers.h"

#ifdef DEBUG_MEMORY_ENABLED
#include "QtitanMSVSDebug.h"
#endif

QTITAN_USE_NAMESPACE


/*!
    \class Qtitan::RibbonButton
    \internal
*/
RibbonButton::RibbonButton(QWidget* parent)
    : QToolButton(parent)
    , m_saveButtonStyle(Qt::ToolButtonIconOnly)
    , m_saveArrowType(Qt::NoArrow)
{
    setAutoRaise(true);
}

RibbonButton::~RibbonButton()
{
}

QSize RibbonButton::sizeHint() const
{
    RibbonButton* thisButton = (RibbonButton*)this;

    if (m_saveButtonStyle != toolButtonStyle())
        thisButton->m_sizeHint = QSize();

    if (m_saveArrowType != arrowType())
        thisButton->m_sizeHint = QSize();

    if (m_sizeHint.isValid())
        return m_sizeHint;

    thisButton->m_saveButtonStyle = toolButtonStyle();
    thisButton->m_saveArrowType = arrowType();

    QSize sz = QToolButton::sizeHint();
    if (RibbonControl* control = qobject_cast<RibbonControl *>(parentWidget()))
    {
        if (RibbonBar* ribbonBar = control->parentGroup()->ribbonBar())
            sz.setHeight(ribbonBar->rowItemHeight());
    }

    int w = 0, h = 0;
    QStyleOptionToolButton opt;
    initStyleOption(&opt);

    QString textButton = text();

    if (!textButton.isEmpty() && (bool)style()->styleHint((QStyle::StyleHint)RibbonStyle::SH_RibbonItemUpperCase, 0, this))
        textButton = textButton.toUpper();

    if (opt.toolButtonStyle != Qt::ToolButtonTextOnly)
    {
        QSize icon = opt.iconSize;
        QAction* action = defaultAction();
        if (action && !action->icon().isNull())
            icon = action->icon().actualSize(opt.iconSize);
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
            CommonStyle::splitString(textButton, strFirstRow, strSecondRow);

            QSize textFirstSize;
            if (!strFirstRow.isEmpty())
                textFirstSize = opt.fontMetrics.size(Qt::TextShowMnemonic, strFirstRow);

            if (!strSecondRow.isEmpty())
            {
                QSize textSecondSize = opt.fontMetrics.size(Qt::TextShowMnemonic, strSecondRow);
                textFirstSize.setWidth(qMax(textFirstSize.width(), textSecondSize.width()));
            }

            indicatorCenter = strSecondRow.isEmpty() && opt.features & QStyleOptionToolButton::HasMenu;

            h = sz.height();

            if (RibbonControl* control = qobject_cast<RibbonControl *>(parentWidget()))
            {
                if (RibbonBar* ribbonBar = control->parentGroup()->ribbonBar())
                    h = ribbonBar->rowItemHeight() * ribbonBar->rowItemCount();
            }

            // if the text is more than icon
            if (textFirstSize.width() > w)
                w = textFirstSize.width();
            // add pixel
            w += mode == MenuButtonPopup || mode == QToolButton::InstantPopup ? 4 : 10;
        } 
        else
        {
            QSize textSize = opt.fontMetrics.size(Qt::TextShowMnemonic, textButton);
            textSize.setWidth(textSize.width() + opt.fontMetrics.width(QLatin1Char(' ')) * 2);
            h = sz.height();

            if (opt.toolButtonStyle == Qt::ToolButtonTextBesideIcon) 
            {
                w += textSize.width() + 6;

                if (textSize.height() > h)
                   h = textSize.height();
            } 
            else 
                w = textSize.width() + 4;
        }
    }
    else 
    {
        h = sz.height();
        w = h;
        if ((opt.features & QStyleOptionToolButton::HasMenu) || (opt.subControls & QStyle::SC_ToolButtonMenu))
            w += 1;
    }

    opt.rect.setSize(QSize(w, h)); // PM_MenuButtonIndicator depends on the height

    if ((mode == MenuButtonPopup || mode == QToolButton::InstantPopup) && !indicatorCenter)
        w += style()->pixelMetric(QStyle::PM_MenuButtonIndicator, &opt, this);

    thisButton->m_sizeHint = QSize(w, h).expandedTo(QApplication::globalStrut());
    return m_sizeHint;
}

bool RibbonButton::event(QEvent* event)
{
    bool result = QToolButton::event(event);
    if (event->type() == QEvent::LayoutRequest)
        m_sizeHint = QSize();
    return result;
}

void RibbonButton::actionEvent(QActionEvent* event)
{
    QToolButton::actionEvent(event);
    switch (event->type()) 
    {
        case QEvent::ActionChanged:
            m_sizeHint = QSize();
            break;
        case QEvent::ActionAdded:
            m_sizeHint = QSize();
            break;
        case QEvent::ActionRemoved:
            m_sizeHint = QSize();
            break;
        default:
            ;
    }
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

QStyle::SubControl RibbonTitleButton::subControl() const
{
    return m_subControl;
}

void RibbonTitleButton::paintEvent(QPaintEvent* event)
{
    Q_UNUSED(event);

    QPainter p(this);
    StyleCaptionButton opt;
    initStyleOption(&opt);
    opt.activeSubControls = m_subControl;
    style()->drawPrimitive(QStyle::PE_PanelButtonTool, &opt, &p, this);
}
