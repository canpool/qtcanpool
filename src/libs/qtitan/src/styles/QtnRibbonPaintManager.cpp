/****************************************************************************
**
** Qtitan Library by Developer Machines (Microsoft-Ribbon implementation for Qt.C++)
** 
** Copyright (c) 2022 MaMinJie <canpool@163.com>
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
#include <QBitmap>
#include <QPainter>
#include <QMdiArea>
#include <QComboBox>
#include <QCheckBox>
#include <QLineEdit>
#include <QToolTip>
#include <QDockWidget>
#include <QGroupBox>
#include <QMessageBox>
#include <QTextEdit>
#include <QPainterPath>

#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
#include <qdrawutil.h>
#endif /* Q_OS_WIN*/

#include "QtnRibbonStylePrivate.h"

#include "QtnCommonStyle.h"
#include "QtnRibbonStyle.h"
#include "QtnStyleHelpers.h"
#include "QtnCommonStylePrivate.h"

#include "QtnOfficeFrameHelper.h"
#include "QtnRibbonMainWindow.h"
#include "QtnRibbonBar.h"
#include "QtnRibbonPage.h"
#include "QtnRibbonTabBar.h"
#include "QtnRibbonPrivate.h"
#include "QtnRibbonGroupPrivate.h"
#include "QtnRibbonGroup.h"
#include "QtnRibbonBackstageView.h"
#include "QtnRibbonStatusBar.h"
#include "QtnRibbonSliderPane.h"
#include "QtnRibbonSystemPopupBar.h"
#include "QtnRibbonButtonPrivate.h"
#include "QtnRibbonToolTip.h"
#include "QtnRibbonGalleryControls.h"
#include "QtnRibbonQuickAccessBar.h"
#ifdef DEBUG_MEMORY_ENABLED
#include "QtitanMSVSDebug.h"
#endif


QTITAN_USE_NAMESPACE


static QString qtn_getColorName(RibbonPage::ContextColor color)
{
    switch (color)
    {
    case RibbonPage::ContextColorBlue: 
        return QString("Blue");
    case RibbonPage::ContextColorYellow: 
        return QString("Yellow");
    case RibbonPage::ContextColorGreen: 
        return QString("Green");
    case RibbonPage::ContextColorRed: 
        return QString("Red");
    case RibbonPage::ContextColorPurple: 
        return QString("Purple");
    case RibbonPage::ContextColorCyan: 
        return QString("Cyan");
    case RibbonPage::ContextColorOrange: 
        return QString("Orange");
    default:
        break;
    }
    return QString("");
}

static QColor qtn_getColorToRGB(RibbonPage::ContextColor color)
{
    switch (color)
    {
    case RibbonPage::ContextColorRed:
        return QColor(255, 160, 160);

    case RibbonPage::ContextColorOrange:
        return QColor(239, 189, 55);

    case RibbonPage::ContextColorYellow:
        return QColor(253, 229, 27);

    case RibbonPage::ContextColorGreen:
        return QColor(113, 190, 89);

    case RibbonPage::ContextColorBlue:
        return QColor(128, 181, 196);

    case RibbonPage::ContextColorCyan:
        return QColor(114, 163, 224);

    case RibbonPage::ContextColorPurple:
        return QColor(214, 178, 209);
    default:
        break;
    }
    return QColor();
}


/*!
\class Qtitan::RibbonPaintManager
\internal
*/
RibbonPaintManager::RibbonPaintManager(CommonStyle* baseStyle)
    : OfficePaintManager(baseStyle)
{
}

RibbonPaintManager::~RibbonPaintManager()
{
}


QIcon RibbonPaintManager::qtnStandardIcon(QStyle::StandardPixmap px, const QStyleOption* opt, const QWidget* wd, bool& ret ) const
{
    Q_UNUSED(px);
    Q_UNUSED(opt);
    Q_UNUSED(wd);
    ret = false;
    return QIcon();
}

void RibbonPaintManager::drawRibbonBar(const QStyleOption* opt, QPainter* p, const QWidget* widget) const
{
    Q_UNUSED(widget);
    QTN_D_STYLE(RibbonStyle)

    if (const StyleOptionRibbon* optRibbonBar = qstyleoption_cast<const StyleOptionRibbon*>(opt))
    {
        QRect rcRibbonClient = opt->rect;
        if (optRibbonBar->frameHelper)
            rcRibbonClient.setTop(rcRibbonClient.top() + optRibbonBar->titleBarHeight);

        QRect rcRibbonTabs = optRibbonBar->rectTabBar;
        rcRibbonTabs.setLeft(rcRibbonClient.left()); 
        rcRibbonTabs.setRight(rcRibbonClient.right());

        QRect rcRibbonGroups(QPoint(rcRibbonClient.left(), rcRibbonTabs.bottom()), QPoint(rcRibbonClient.right(), rcRibbonClient.bottom()));

        p->fillRect(rcRibbonTabs, d.m_clrRibbonFace);
        p->fillRect(rcRibbonGroups, d.m_clrRibbonFace);
        
        if (d.m_flatFrame && optRibbonBar->frameHelper)
        {
            if (!optRibbonBar->frameHelper->isDwmEnabled())
            {
                if (!optRibbonBar->frameHelper->isActive())
                    optRibbonBar->frameHelper->fillSolidRect(p, rcRibbonTabs, optRibbonBar->airRegion, d.m_clrRibbonInactiveFace);
            }
            else
            {
                optRibbonBar->frameHelper->fillSolidRect(p, rcRibbonTabs, optRibbonBar->airRegion, QColor(0, 0, 0));
                QPixmap soImage = cached("RibbonTabBackgroundDwm.png");
                if (!soImage.isNull())
                {
                    QRect rcSrc = sourceRectImage(soImage.rect(), 0, 1);
                    QRect rcRibbonClient = opt->rect;
                    QRect rcRibbonTabs = optRibbonBar->rectTabBar;

                    int height = qMin(rcRibbonTabs.height(), rcSrc.height()+1);
                    rcSrc.setTop(rcSrc.bottom() - height);
                    QRect rcArea(QPoint(rcRibbonClient.left(), rcRibbonTabs.bottom() - height), 
                        QPoint(rcRibbonClient.right(), rcRibbonTabs.bottom()));
                    drawImage(soImage, *p, rcArea, rcSrc, QRect(QPoint(10, 0), QPoint(10, 0)), /*isStyle2010()*/true);
                }
            }
        }

        if (optRibbonBar->minimized)
        {
            p->fillRect(rcRibbonClient.left(), rcRibbonClient.bottom() - 2, rcRibbonClient.width(), 1, d.m_clrMinimizedFrameEdgeShadow);
            p->fillRect(rcRibbonClient.left(), rcRibbonClient.bottom() - 1, rcRibbonClient.width(), 1, d.m_clrMinimizedFrameEdgeHighLight);
        }
    }
}

/*! \internal */
void RibbonPaintManager::drawRibbonTabBar(const QStyleOption* opt, QPainter* p, const QWidget* w) const
{
    Q_UNUSED(opt);
    Q_UNUSED(p);
    Q_UNUSED(w);
}

/*! \internal */
void RibbonPaintManager::drawRibbonGroups(const QStyleOption* option, QPainter* p, const QWidget* widget) const
{
    Q_UNUSED(widget);
    RibbonPage::ContextColor pageColor = RibbonPage::ContextColorNone;
    if (const RibbonBar* rb = qobject_cast<const RibbonBar*>(widget))
    {
        if (RibbonPage* page = rb->getPage(rb->currentPageIndex()))
            pageColor = page->contextColor();
    }
    else if (/*const ReducedGroupPopupMenu* pm = qobject_cast<const ReducedGroupPopupMenu*>(widget)*/widget && widget->windowFlags() & Qt::Popup)
    {
        if (const RibbonPage* page = getParentWidget<const RibbonPage>(widget))
            pageColor = page->contextColor();
    }

    if (pageColor != RibbonPage::ContextColorNone)
    {
        QString resName("ContextPage");
        resName += qtn_getColorName(pageColor);
        resName += "Client";

        QPixmap soImage = cached(resName);
        if (!soImage.isNull())
            drawImage(soImage, *p, option->rect, sourceRectImage(soImage.rect(), 0, 1),
            soImage.width() < 5 ? QRect(QPoint(1, 8), QPoint(1, 8)) : QRect(QPoint(8, 8), QPoint(8, 8)));
    }
    else
    {
        QPixmap soImage = cached("RibbonGroups.png");
        if (!soImage.isNull())
            drawImage(soImage, *p, option->rect, sourceRectImage(soImage.rect(), 0, 1), QRect(QPoint(8, 8), QPoint(8, 8)));
    }
}

/*! \internal */
void RibbonPaintManager::drawGroup(const QStyleOption* opt, QPainter* p, const QWidget* widget) const
{
    if (const QStyleOptionGroupBox* gr = qstyleoption_cast<const QStyleOptionGroupBox*>(opt))
    {
        QRect rcEntryCaption = opt->rect;
        rcEntryCaption.setTop(rcEntryCaption.bottom() - gr->lineWidth);

        RibbonPage::ContextColor pageColor = RibbonPage::ContextColorNone;
        if (const RibbonPage* page = getParentWidget<const RibbonPage>(widget))
            pageColor = page->contextColor();

        QPixmap soImage;
        QPixmap soImageCaption;
        if (pageColor == RibbonPage::ContextColorNone)
        {
            if (widget->property(_qtn_TitleGroupsVisible).toBool())
            {
                soImage = cached("RibbonGroupClient.png");
                soImageCaption = cached("RibbonGroupCaption.png");
            }
            else
                soImage = cached("RibbonGroupClientButtom.png");

            if (soImage.isNull())
                soImage = cached("RibbonGroupClient.png");
        }
        else
        {
            QString resName("ContextPage");
            resName += qtn_getColorName(pageColor);
            resName += "GroupClient.png";

            soImage = cached(resName);

            if (soImage.isNull())
            {
                soImage = cached("ContextPageGroupClient.png");
                soImageCaption = cached("ContextPageGroupCaption.png");
            }
        }

        QRect rcEntry(gr->rect);
        if (!soImageCaption.isNull())
            rcEntry.setBottom(rcEntry.bottom() - gr->lineWidth);

        int state = opt->state & QStyle::State_MouseOver ? 1 : 0;

        if (!soImage.isNull())
            drawImage(soImage, *p, rcEntry, sourceRectImage(soImage.rect(), state, 2), QRect(QPoint(5, 5), QPoint(5, 5)), (state == 1 && true));

        if (!soImageCaption.isNull())
            drawImage(soImageCaption, *p, rcEntryCaption, sourceRectImage(soImageCaption.rect(), state, 2), QRect(QPoint(5, 5), QPoint(5, 5)));

        soImage = cached("RibbonGroupSeparator.png");
        if (!soImage.isNull())
        {
            QRect rcGroup(gr->rect);
            drawImage(soImage, *p, QRect(QPoint(rcGroup.right() - 1, rcGroup.top() + 2), QPoint(rcGroup.right() + 1, rcGroup.bottom())), 
                sourceRectImage(soImage.rect()), QRect(QPoint(0, 0), QPoint(0, 0)));
        }
    }
}

// for TitleBar
/*! \internal */
bool RibbonPaintManager::drawToolBar(const QStyleOption* opt, QPainter* p, const QWidget* w) const
{
    Q_UNUSED(w);
    if (const QStyleOptionToolBar* toolbar = qstyleoption_cast<const QStyleOptionToolBar*>(opt)) 
    {
        QPixmap soImage = toolbar->state & QStyle::State_Horizontal ? cached("ToolbarFaceHorizontal.png")
            : cached("ToolbarFaceVertical.png");

        if (!soImage.isNull())
            drawImage(soImage, *p, opt->rect, soImage.rect(), QRect(QPoint(5, 5), QPoint(5, 5)));

        return true;
    }
    else if (const StyleRibbonQuickAccessBar* quickAccessBar = qstyleoption_cast<const StyleRibbonQuickAccessBar*>(opt)) 
    {
        if (quickAccessBar->quickAccessBarPosition != RibbonBar::QATopPosition)
        {
            QPixmap soImage = cached("PanelQuickAccessBar.png");

            if (!soImage.isNull())
                drawImage(soImage, *p, opt->rect, soImage.rect(), QRect(QPoint(5, 5), QPoint(5, 5)));
        }
        return true;
    }

    return false;
}

/*! \internal */
bool RibbonPaintManager::drawIndicatorToolBarSeparator(const QStyleOption* opt, QPainter* p, const QWidget* w) const
{
    if (qobject_cast<const RibbonGroup*>(w))
    {
        if (const QStyleOptionGroupBox* optGroup = qstyleoption_cast<const QStyleOptionGroupBox*>(opt)) 
        {
            if (optGroup->text.isEmpty())
            {
                p->save();
                int margin = 3;
                p->setPen(opt->palette.background().color().darker(114));
                int x1 = opt->rect.center().x();
                p->drawLine(QPoint(x1, opt->rect.top() + margin), QPoint(x1, opt->rect.bottom() - margin));
                p->restore();
            }
            else
            {
                int margin = 3;
                QRect rect = opt->rect;
                QString titleText = optGroup->text;
                QRect rectText = optGroup->fontMetrics.boundingRect(titleText);
                if (optGroup->textAlignment == Qt::AlignBottom)
                {
                    QPainterPath path;
                    path.moveTo (rect.center().x(), rect.top() + margin);
                    path.lineTo (rect.center().x(), rect.bottom() - rectText.width() - margin);

                    p->save();
                    p->setPen(opt->palette.background().color().darker(114));
                    p->drawPath(path);
                    QSize s = rect.size();
                    s.transpose();
                    rect.setSize(s);
                    p->translate(rect.left() - 1, rect.top() + rect.width());
                    p->rotate(-90);
                    p->translate(-rect.left() + 1, -rect.top());
                    int indent = 0;
                    QPalette pal;
                    pal.setColor(QPalette::WindowText, opt->palette.background().color().darker(114));
                    baseStyle()->proxy()->drawItemText(p, rect.adjusted(indent + 1, 1, -indent - 1, -1), Qt::AlignLeft | Qt::AlignVCenter | Qt::TextShowMnemonic, pal,
                        optGroup->state & QStyle::State_Enabled, titleText, QPalette::WindowText);
                    p->restore();
                }
                else if (optGroup->textAlignment == Qt::AlignTop)
                {
                    QPainterPath path;
                    path.moveTo (rect.center().x(), rect.top() + rectText.width() + margin);
                    path.lineTo (rect.center().x(), rect.bottom() - margin);

                    p->save();
                    p->setPen(opt->palette.background().color().darker(114));
                    p->drawPath(path);
                    QSize s = rect.size();
                    s.transpose();
                    rect.setSize(s);
                    p->translate(rect.left() - 1, rect.top() + rect.width());
                    p->rotate(-90);
                    p->translate(-rect.left() + 1, -rect.top());
                    int indent = 0;
                    QPalette pal;
                    pal.setColor(QPalette::WindowText, opt->palette.background().color().darker(114));
                    baseStyle()->proxy()->drawItemText(p, rect.adjusted(indent + rect.width() - rectText.width() - 2, 1, -indent - 1, -1), Qt::AlignLeft | Qt::AlignVCenter | Qt::TextShowMnemonic, pal,
                        optGroup->state & QStyle::State_Enabled, titleText, QPalette::WindowText);
                    p->restore();
                }
                else if (optGroup->textAlignment == Qt::AlignVCenter)
                {
                    p->save();
                    p->setPen(opt->palette.background().color().darker(114));
                    p->drawLine(rect.center().x(), rect.top() + rectText.width() + rectText.width() + margin + 2, rect.center().x(), rect.bottom() - margin);
                    p->drawLine(rect.center().x(), rect.top() + margin, rect.center().x(), rect.bottom() - (rectText.width() + rectText.width() + margin));

                    QSize s = rect.size();
                    s.transpose();
                    rect.setSize(s);
                    p->translate(rect.left() - 1, rect.top() + rect.width());
                    p->rotate(-90);
                    p->translate(-rect.left() + 1, -rect.top());
                    int indent = 0;
                    QPalette pal;
                    pal.setColor(QPalette::WindowText, opt->palette.background().color().darker(114));
                    baseStyle()->proxy()->drawItemText(p, rect.adjusted(indent + (rect.width() - rectText.width())/2 - 2, 1, -indent - 1, -1), Qt::AlignLeft | Qt::AlignVCenter | Qt::TextShowMnemonic, pal,
                        optGroup->state & QStyle::State_Enabled, titleText, QPalette::WindowText);
                    p->restore();
                }
            }
        }
        return true;
    }
    else if (qobject_cast<const RibbonStatusBar*>(w))
    {
        QPixmap soImage = cached("StatusBarSeparator.png");
        if (!soImage.isNull())
        {
            QRect rect = opt->rect;
            rect.setRight(rect.left() + 1);
            rect.setTop(rect.top() - 2);

            drawImage(soImage, *p, rect, soImage.rect(), QRect(QPoint(2, 4), QPoint(2, 2)));
            return true;
        }
    }
    else if (qobject_cast<const RibbonToolBarControl*>(w) && !isStyle2010())
        return true;
    return false;
}

/*! \internal */
bool RibbonPaintManager::drawPanelButtonTool(const QStyleOption* opt, QPainter* p, const QWidget* widget) const
{
    if (widget && qobject_cast<const RibbonBackstageButton*>(widget))
    {
        drawPanelBackstageButton(opt, p, widget);
        return true;
    }

    if (widget && !qobject_cast<const RibbonTitleButton*>(widget))
    {
        if (const QStyleOptionToolButton* toolbutton = qstyleoption_cast<const QStyleOptionToolButton*>(opt)) 
        {
            bool smallSize = opt->rect.height() < 33;

            if (widget && qobject_cast<const RibbonButton*>(widget))
            {
                if (!opt->rect.isEmpty())
                {
                    int minIcon = qMin(opt->rect.height(), opt->rect.width());
                    int actualArea = minIcon*minIcon;

                    int index = -1;
                    int curArea = 0;
                    QList<QSize> lSz = toolbutton->icon.availableSizes();
                    if (lSz.count() > 1)
                    {
                        for (int i = 0, count = lSz.count(); count > i; i++)
                        {
                            QSize curSz = lSz[i];
                            int area = curSz.height()*curSz.width();
                            if (actualArea > area)
                            {
                                if (area > curArea)
                                    index = i;
                                curArea = area;
                            }
                        }
                        int iconExtent = index != -1 ?  toolbutton->icon.actualSize(lSz[index]).height() : smallSize;
                        smallSize = toolbutton->toolButtonStyle == Qt::ToolButtonTextUnderIcon ? iconExtent < 32 : iconExtent;
                    }
                }
            }

            bool enabled  = opt->state & QStyle::State_Enabled;
            bool checked  = opt->state & QStyle::State_On;
            bool selected = opt->state & QStyle::State_MouseOver;
            bool mouseInSplit = opt->state & QStyle::State_MouseOver && toolbutton->activeSubControls & QStyle::SC_ToolButton;
            bool mouseInSplitDropDown = opt->state & QStyle::State_MouseOver && toolbutton->activeSubControls & QStyle::SC_ToolButtonMenu;
            bool pressed  = opt->state & QStyle::State_Sunken;
            bool popuped  = (toolbutton->activeSubControls & QStyle::SC_ToolButtonMenu) && (opt->state & QStyle::State_Sunken);

            if (!(toolbutton->features & QStyleOptionToolButton::MenuButtonPopup))
            {
                bool autoRaise = toolbutton->state & QStyle::State_AutoRaise;
                if (!autoRaise && !qobject_cast<const QStatusBar*>(widget->parentWidget()))
                {
                    QStyleOptionButton tmpBtn;// = *btn;
                    tmpBtn.state = toolbutton->state;
                    tmpBtn.rect = toolbutton->rect;
                    baseStyle()->proxy()->drawPrimitive(QStyle::PE_PanelButtonCommand, &tmpBtn, p, widget);
                }
                else
                {
                    int state = -1;
                    if ( !enabled ) state = -1;

                    else if (popuped) state = 2;
                    else if (checked && !selected && !pressed) state = 2;
                    else if (checked && selected && !pressed) state = 3;
                    else if (selected && pressed) state = 1;
                    else if (selected || pressed) state = 0;

                    QPixmap soButton = smallSize ? cached("ToolbarButtons22.png") : cached("ToolbarButtons50.png");
                    drawImage(soButton, *p, opt->rect, sourceRectImage(soButton.rect(), state, 4), QRect(QPoint(8, 8), QPoint(8, 8)));
                }
                return true;
            }

            RibbonToolBarControl* toolBarControl = qobject_cast<RibbonToolBarControl*>(widget->parentWidget());
            if (toolBarControl)
                smallSize = true;

            int specialOffset = 0;
            QPixmap soImageSplit, soImageSplitDropDown;
            if (toolBarControl && widget->property(_qtn_Group).toBool())
            {
                bool beginGroup = widget->property(_qtn_Begin_Group).toBool();
                bool endGroup = widget->property(_qtn_End_Group).toBool();
                bool wrap       = widget->property(_qtn_Wrap).toBool();
                bool left       = beginGroup || wrap;

                //int indexNext = rg->qtn_d().getNextIndex(widget);
                //QWidget* nextWidget = rg->qtn_d().getWidget(indexNext);
                
                //bool wrapNext       = nextWidget->property(_qtn_Wrap).toBool();

                bool right = endGroup;/// indexNext <= rg->qtn_d().getIndexWidget(widget) || beginGroupNext || wrapNext;

                soImageSplit = left ? cached("ToolbarButtonsSpecialSplitLeft.png") :
                    cached("ToolbarButtonsSpecialSplitCenter.png");

                soImageSplitDropDown = right ? cached("ToolbarButtonsSpecialSplitDropDownRight.png") :
                    cached("ToolbarButtonsSpecialSplitDropDownCenter.png");
                specialOffset = 1;
            }
            else
            {
                soImageSplit = smallSize ? cached("ToolbarButtonsSplit22.png") 
                    : cached("ToolbarButtonsSplit50.png");
                soImageSplitDropDown = smallSize ? cached("ToolbarButtonsSplitDropDown22.png") 
                    : cached("ToolbarButtonsSplitDropDown50.png");
            }

            if(soImageSplit.isNull() || soImageSplitDropDown.isNull())
                return false;

            QRect rcButton = opt->rect;
            QRect popupr = baseStyle()->subControlRect(QStyle::CC_ToolButton, toolbutton, QStyle::SC_ToolButtonMenu, widget);

            QRect rcSplit = smallSize ? QRect(QPoint(rcButton.left(), rcButton.top()), QPoint(rcButton.right() - popupr.width()-2, rcButton.bottom())) 
                : QRect(QPoint(rcButton.left(), rcButton.top()), QPoint(rcButton.right(), rcButton.bottom() - popupr.height()-2));

            int stateSplit = -1;

            if (/*enabledSplit && (selected || pressed || popuped)*/false)
            {
                stateSplit = 4;
            }
            else if (!enabled)
            {
                //            if (isKeyboardSelected(selected)) stateSplit = 5;
            }
            else if (checked)
            {
                if (popuped) stateSplit = 5;
                else if (!selected && !pressed) stateSplit = 2;
                else if (selected && !pressed) stateSplit = 3;
                else if (/*isKeyboardSelected(pressed) ||*/ (selected && pressed)) stateSplit = 1;
                else if (pressed) stateSplit = 2;
                else if (selected || pressed) stateSplit = !mouseInSplit ? 5 : 0;
            }
            else
            {
                if (popuped) stateSplit = 5;
                else if (/*isKeyboardSelected(bPressed) ||*/ (selected && pressed)) stateSplit = 1;
                else if (selected || pressed) stateSplit = !mouseInSplit ? 5 : 0;
            }
            stateSplit += specialOffset;

            if (stateSplit != -1)
            {
                if ( smallSize)
                {
                    drawImage(soImageSplit, *p, rcSplit, sourceRectImage(soImageSplit.rect(), stateSplit, 6+specialOffset), 
                        QRect(QPoint(2, 2), QPoint(2, 2)), QColor(0xFF, 0, 0xFF));
                }
                else
                {
                    drawImage(soImageSplit, *p, rcSplit, sourceRectImage(soImageSplit.rect(), stateSplit, 6+specialOffset), 
                        QRect(QPoint(2, 2), QPoint(2, 2)));
                }
            }

            QRect rcSplitDropDown = smallSize ? QRect(QPoint(rcButton.right() - popupr.width()-1, rcButton.top()), QPoint(rcButton.right(), rcButton.bottom())) :
                QRect(QPoint(rcButton.left(), rcButton.bottom() - popupr.height()-1), QPoint(rcButton.right(), rcButton.bottom()));

            int stateSplitDropDown = -1;

            if (/*enabledDropDown && (selected || pressed || popuped)*/false)
            {
                stateSplitDropDown = 3;
            }
            else if (!enabled)
            {
                //            if (isKeyboardSelected(selected)) stateSplitDropDown = 4;
            }
            else if (checked)
            {
                if (popuped) stateSplitDropDown = 2;
                else if (!selected && !pressed) stateSplitDropDown = 2;
                else if ((selected && pressed)) stateSplitDropDown = 0;
                else if (selected || pressed) stateSplitDropDown = !mouseInSplitDropDown ? 4 : 0;
                else stateSplitDropDown = 4;
            }
            else
            {
                if (popuped) stateSplitDropDown = 2;
                else if ((selected && pressed)) stateSplitDropDown = 0;
                else if (selected || pressed) stateSplitDropDown = !mouseInSplitDropDown ? 4 : 0;
            }

            stateSplitDropDown += specialOffset;
            if (stateSplitDropDown != -1)
            {
                if (smallSize)
                {
                    drawImage(soImageSplitDropDown, *p, rcSplitDropDown, sourceRectImage(soImageSplitDropDown.rect(), stateSplitDropDown, 5+specialOffset), 
                        QRect(QPoint(2, 2), QPoint(2, 2)), QColor(0xFF, 0, 0xFF));
                }
                else
                {
                    drawImage(soImageSplitDropDown, *p, rcSplitDropDown, sourceRectImage(soImageSplitDropDown.rect(), stateSplitDropDown, 5+specialOffset), 
                        QRect(QPoint(2, 2), QPoint(2, 2)), QColor(0xFF, 0, 0xFF));
                }
            }
            return true;
        }
    }
    else if (const StyleCaptionButton* toolbutton = qstyleoption_cast<const StyleCaptionButton*>(opt)) 
    {
        bool down = opt->state & QStyle::State_Sunken;
        bool select = opt->state & QStyle::State_MouseOver;
        bool active = opt->state & QStyle::State_Active;
        int state = !active ? 3 : down && select ? 2 : select || down ? 1 : 0;

        if (down || select)
        {
            QPixmap soImageButton;
            if (toolbutton->activeSubControls == QStyle::SC_TitleBarCloseButton)
                soImageButton = cached("FrameTitleCloseButton.png");

            if (soImageButton.isNull())
                soImageButton = cached("FrameTitleButton.png");

            drawImage(soImageButton, *p, toolbutton->rect, sourceRectImage(soImageButton.rect(), down || select ? 1 : 0, 2),
                QRect(QPoint(3, 3), QPoint(3, 3)), QColor(0xFF, 0, 0xFF));
        }

        QString strGlyphSize("17.png");
        if (toolbutton->rect.width() < 27) strGlyphSize = "17.png";
        if (toolbutton->rect.width() < 15) strGlyphSize = "13.png";

        QString strButton;
        if (toolbutton->activeSubControls == QStyle::SC_TitleBarCloseButton)
            strButton = "FrameTitleClose";
        else if (toolbutton->activeSubControls == QStyle::SC_TitleBarMaxButton)
            strButton = "FrameTitleMaximize";
        else if (toolbutton->activeSubControls == QStyle::SC_TitleBarMinButton)
            strButton = "FrameTitleMinimize";
        else if (toolbutton->activeSubControls == QStyle::SC_TitleBarNormalButton)
            strButton = "FrameTitleRestore";

        strButton += strGlyphSize;

        QPixmap soImageIndicator  = cached(strButton);
        if (!soImageIndicator.isNull())
        {
            QRect rcSrc = sourceRectImage(soImageIndicator.rect(), state, 5);
            QRect rcGlyph(QPoint((toolbutton->rect.right() + toolbutton->rect.left() - rcSrc.width()+1) / 2, (toolbutton->rect.top() + toolbutton->rect.bottom() - rcSrc.height()+2) / 2), rcSrc.size());
            drawImage(soImageIndicator, *p, rcGlyph, rcSrc, QRect(QPoint(0, 0), QPoint(0, 0)), QColor(0xFF, 0, 0xFF));
        }
        return true;
    }
    return false;
}

/*! \internal */
void RibbonPaintManager::drawContextHeaders(const QStyleOption* opt, QPainter* p) const
{
    if (const StyleOptionTitleBar* optTitleBar = qstyleoption_cast<const StyleOptionTitleBar*>(opt))
    {
        const QList<ContextHeader*>& listContextHeaders = *optTitleBar->listContextHeaders;
        Q_FOREACH (const ContextHeader* header, listContextHeaders)
        {
            if (!header || !header->firstTab || !header->lastTab || header->color == RibbonPage::ContextColorNone)
                continue;

            QString resName("ContextTab");
            resName += qtn_getColorName(header->color);
            resName += "Header";

            QPixmap soImage = cached(resName);
            if (soImage.isNull())
                return;

            QRect rc = header->rcRect;
            drawImage(soImage, *p, rc, soImage.rect(), QRect(QPoint(2, 2), QPoint(2, 2)));

            if (optTitleBar->frameHelper && optTitleBar->frameHelper->isDwmEnabled())
            {
                QColor col = helper().getColor("Ribbon", "ContextTabTextColor");
                optTitleBar->frameHelper->drawDwmCaptionText(p, rc, header->strTitle, col, optTitleBar->frameHelper->isActive(), true);
            }
            else
                p->drawText(rc, Qt::AlignCenter | Qt::TextSingleLine, p->fontMetrics().elidedText(header->strTitle, Qt::ElideRight, rc.width()));
        }
    }
}

/*! \internal */
void RibbonPaintManager::fillRibbonTabControl(const QStyleOption* opt, QPainter* p, const QWidget* widget) const
{
    if (const RibbonTab* tab = qobject_cast<const RibbonTab*>(widget))
    {
        if (ContextHeader* contextHeader = tab->getContextHeader())
        {
            if (!contextHeader || !contextHeader->firstTab || !contextHeader->lastTab || contextHeader->color == RibbonPage::ContextColorNone)
                return;

            QRect rcFirst(contextHeader->firstTab->rect());
            QRect rcLast(contextHeader->lastTab->rect());

            QString resName("ContextTab");
            resName += qtn_getColorName(contextHeader->color);
            resName += "Background.png";
            QPixmap soImage = cached(resName);
            if (!soImage.isNull())
            {
                drawImage(soImage, *p, QRect(rcFirst.left(), opt->rect.top(), rcLast.right()+1, rcLast.bottom()), 
                    soImage.rect(), QRect(QPoint(2, 2), QPoint(2, 2)));
            }
        }
    }
}

// for BackstageButton
/*! \internal */
void RibbonPaintManager::drawPanelBackstageButton(const QStyleOption* opt, QPainter* p, const QWidget* w) const
{
    Q_UNUSED(w);
    if (const StyleOptionBackstageButton* btnStyle = qstyleoption_cast<const StyleOptionBackstageButton*>(opt)) 
    {
        QRect rc = btnStyle->rect;
        bool isDefault = btnStyle->features & QStyleOptionButton::DefaultButton;
        bool enabled  = opt->state & QStyle::State_Enabled;
        bool checked  = opt->state & QStyle::State_On;
        bool selected = opt->state & QStyle::State_MouseOver;
        bool pressed  = opt->state & QStyle::State_Sunken;
        //            bool popuped  = false;

        if (btnStyle->flatStyle && !selected && !pressed && !checked)
            return;

        QPixmap soImage = cachedPath(pathForBackstageImages() + "BackstageButton.png");
        int state = isDefault ? 4 : 0;

        if (!enabled) state = 3;
        else if (checked && !selected && !pressed) state = 2;
        else if (checked && selected && !pressed) state = 1;
        else if ((selected && pressed)) state = 2;
        else if (selected || pressed) state = 1;

        if (!soImage.isNull() && state != -1)
            drawImage(soImage, *p, rc, sourceRectImage(soImage.rect(), state, 5), QRect(QPoint(4, 4), QPoint(4, 4)), QColor(0xFF, 0, 0xFF));

        if (checked && btnStyle->tabStyle)
        {
            QPixmap soImage = cachedPath(pathForBackstageImages() + "BackstageButtonGlyph.png");
            if (!soImage.isNull())
            {
                QRect rcSrc = soImage.rect();
                int y = rc.center().y();
                QRect rectImage(QPoint(rc.right() - rcSrc.width(), y - rcSrc.height() / 2), QPoint(rc.right(), y - rcSrc.height() / 2 + rcSrc.height()));
                drawImage(soImage, *p, rectImage, rcSrc);
            }
        }
    }
}

/*! \internal */
QString RibbonPaintManager::pathForBackstageImages() const
{
    QString pathCached = ":/res/Office2007Blue/";
    if (isStyle2010())
        pathCached = ":/res/Office2010Blue/";
    else if (getTheme() == OfficeStyle::Windows7Scenic)
        pathCached = ":/res/Windows7Scenic/";
    return pathCached;
}

/*! \internal */
void RibbonPaintManager::drawReducedGroup(const QStyleOption* opt, QPainter* p, const QWidget* widget) const
{
    Q_UNUSED(widget);
    if (const QStyleOptionToolButton* optGroup = qstyleoption_cast<const QStyleOptionToolButton*>(opt))
    {
        RibbonPage::ContextColor pageColor = RibbonPage::ContextColorNone;
        if (const RibbonPage* page = getParentWidget<const RibbonPage>(widget))
            pageColor = page->contextColor();

        QPixmap soImage;
        if (pageColor != RibbonPage::ContextColorNone)
        {
            QString resName("ContextPage");
            resName += qtn_getColorName(pageColor);
            resName += "GroupButton";
            soImage = cached(resName);
        }
        else
            soImage = cached("RibbonGroupButton.png");

        Q_ASSERT(!soImage.isNull());
        if (soImage.isNull())
            return;

        QRect rcEntry(opt->rect);
        bool bPopuped  = optGroup->features & QStyleOptionToolButton::HasMenu;
        int state = bPopuped ? 2 : (opt->state & QStyle::State_MouseOver ? 1 : 0);
        QRect rcSrc = sourceRectImage(soImage.rect(), state, 3);
        // draw background
        drawImage(soImage, *p, rcEntry, rcSrc, QRect(QPoint(5, 5), QPoint(5, 5)), (state == 1 && /*baseStyle()->isStyle2010()*/true));

        // ----
        QFontMetrics fm(optGroup->fontMetrics);
        int flags = /*Qt::AlignVCenter |*/ Qt::TextSingleLine;

        flags |= Qt::TextShowMnemonic;
        if (!baseStyle()->proxy()->styleHint(QStyle::SH_UnderlineShortcut, optGroup, widget))
            flags |= Qt::TextHideMnemonic;

        if (!optGroup->icon.isNull())
        {
            QPixmap pm;
            QSize pmSize = optGroup->iconSize;

            QIcon::State stateIcon = optGroup->state & QStyle::State_On ? QIcon::On : QIcon::Off;
            QIcon::Mode mode;
            if (!(optGroup->state & QStyle::State_Enabled))
                mode = QIcon::Disabled;
            else if ((opt->state & QStyle::State_MouseOver) && (opt->state & QStyle::State_AutoRaise))
                mode = QIcon::Active;
            else
                mode = QIcon::Normal;

            pm = optGroup->icon.pixmap(optGroup->rect.size().boundedTo(optGroup->iconSize), mode, stateIcon);
            pmSize = pm.size();

            QPoint pt = QPoint(rcEntry.center().x() - pmSize.width() / 2, rcEntry.top() + 4);
            if (pmSize.width() < 20)
            {
                soImage = cached("RibbonGroupButtonIcon.png");
                QRect rc(QPoint(rcEntry.center().x() - soImage.size().width() / 2, rcEntry.top() + 2), soImage.size());

                if (!soImage.isNull())
                    drawImage(soImage, *p, rc, sourceRectImage(soImage.rect(), 0, 1), QRect(QPoint(0, 0), QPoint(0, 0)), /*isStyle2010()*/true);

                pt.setY(rcEntry.top() + 9);
            }
            baseStyle()->proxy()->drawItemPixmap(p, QRect(pt, pmSize), Qt::AlignCenter, pm);
        }

        //===========================================================================================
        QString strFirstRow, strSecondRow;
        CommonStyle::splitString(optGroup->text, strFirstRow, strSecondRow);

        int textHeightTotal = optGroup->rect.height() * 0.5;

        QRect rcText = rcEntry;
        rcText.adjust(4, textHeightTotal - 3, -4, 0);

        QRect rectFirstRow = rcText;
        if (!strFirstRow.isEmpty())
        {
            int textHeight = optGroup->fontMetrics.boundingRect(strFirstRow).height() + optGroup->fontMetrics.descent();
            rectFirstRow.setHeight(textHeight + 3);
            baseStyle()->proxy()->drawItemText(p, rectFirstRow, flags | Qt::AlignHCenter, optGroup->palette, optGroup->state & QStyle::State_Enabled, strFirstRow, QPalette::WindowText);
        }

        QRect rectSecondRow = rectFirstRow;
        if (!strSecondRow.isEmpty())
        {
            rectSecondRow.setTop(rectFirstRow.bottom());
            int textHeight = optGroup->fontMetrics.boundingRect(strSecondRow).height() + optGroup->fontMetrics.descent();
            rectSecondRow.setHeight(textHeight);
            baseStyle()->proxy()->drawItemText(p, rectSecondRow, flags | Qt::AlignLeft, optGroup->palette, optGroup->state & QStyle::State_Enabled, strSecondRow, QPalette::WindowText);
        }
        //===========================================================================================
        soImage = cached("ToolbarButtonDropDownGlyph.png");

        Q_ASSERT(!soImage.isNull());
        if (soImage.isNull())
            return;

        state = !(optGroup->state & QStyle::State_Enabled) ? 3 : opt->state & QStyle::State_MouseOver ? 1 : 0;
        rcSrc = sourceRectImage(soImage.rect(), state, 4);

        int width = opt->fontMetrics.width(strSecondRow) + 7;
        QPoint pt = QPoint(strSecondRow.isEmpty() ? rcText.center().x() - 1 : 
            rectSecondRow.left() + width - 4, strSecondRow.isEmpty() ? rectFirstRow.bottom() : rectSecondRow.center().y());

        QRect rc(pt, rcSrc.size());
        drawImage(soImage, *p, rc, rcSrc, QRect(QPoint(0, 0), QPoint(0, 0)), QColor(0xFF, 0, 0xFF));
    }
}

/*! \internal */
void RibbonPaintManager::drawTabShape(const QStyleOption* opt, QPainter* p, const QWidget* widget) const
{
    const StyleRibbonOptionHeader* ribbonOption = qstyleoption_cast<const StyleRibbonOptionHeader*>(opt);
    Q_ASSERT(ribbonOption != Q_NULL);

    RibbonPage::ContextColor pageColor = RibbonPage::ContextColorNone;
    if (const RibbonTab* tab = qobject_cast<const RibbonTab*>(widget))
        pageColor = tab->contextColor();

    bool selected = opt->state & QStyle::State_Selected;
    bool highlighted = opt->state & QStyle::State_MouseOver;
    bool popuped  = (opt->state & QStyle::State_Selected) && (opt->state & QStyle::State_Sunken);
    bool focused = false;

    if (const RibbonBar* rb = getParentWidget<RibbonBar>(widget))
    {
        if (selected && rb->isMinimized())
        {
            if (popuped)
            {
                focused = false;
                highlighted = false;
            }
            else
            {
                focused = false;
                selected = false;
            }
        }
    }

    if (pageColor != RibbonPage::ContextColorNone)
    {
        int state = 0;
        if (selected || highlighted)
        {
            QString resName("ContextTab");
            resName += qtn_getColorName(pageColor);
            QPixmap soImage = cached(resName);
            if (!soImage.isNull() && soImage.height() < 70)
            {
                if (selected)
                    state = focused ? 2 : 1;
                else if (highlighted)
                    state = 0;
                drawImage(soImage, *p, opt->rect, sourceRectImage(soImage.rect(), state, 3), QRect(QPoint(5, 5), QPoint(5, 5)));
            }
            else
            {
                if (selected)
                    state = focused || highlighted ? 4 : 3;
                else if (highlighted)
                    state = 1;

                if (state == 4)
                {
                    soImage = cached("RibbonTab.png");
                    if(!soImage.isNull())
                    {
                        if (ribbonOption->tabBarPosition == RibbonBar::TabBarBottomPosition)
                        {
                            soImage = soImage.copy(sourceRectImage(soImage.rect(), state, 6));
                            soImage = soImage.transformed(QTransform().rotate(180));
                            drawImage(soImage, *p, opt->rect, soImage.rect(), QRect(QPoint(5, 5), QPoint(5, 5)));
                        }
                        else
                        {
                            drawImage(soImage, *p, opt->rect, sourceRectImage(soImage.rect(), 5, 6), QRect(QPoint(5, 5), QPoint(5, 5)), QColor(0xFF, 0, 0xFF));
                        }
                    }
                }
                else
                {
                    if (!soImage.isNull()) 
                    {
                        if (ribbonOption->tabBarPosition == RibbonBar::TabBarBottomPosition)
                        {
                            soImage = soImage.copy(sourceRectImage(soImage.rect(), state, 5));
                            soImage = soImage.transformed(QTransform().rotate(180));
                            drawImage(soImage, *p, opt->rect, soImage.rect(), QRect(QPoint(5, 5), QPoint(5, 5)));
                        }
                        else
                            drawImage(soImage, *p, opt->rect, sourceRectImage(soImage.rect(), state, 5), QRect(QPoint(5, 5), QPoint(5, 5)));
                    }
                }
            }
        }
    }
    else
    {
        if (selected || highlighted)
        {
            int state = 0;
            if (selected && focused)
                state = 5;
            else if (selected && highlighted)
                state = 4;
            else if (selected)
                state = 3;
            else if (opt->state & QStyle::State_Sunken)
                state = 2;
            else if (highlighted)
                state = 1;  
            QPixmap soImage = cached("RibbonTab.png");
            if(!soImage.isNull())
            {
                if (ribbonOption->tabBarPosition == RibbonBar::TabBarBottomPosition)
                {
                    soImage = soImage.copy(sourceRectImage(soImage.rect(), state, 6));
                    soImage = soImage.transformed(QTransform().rotate(180));
                    drawImage(soImage, *p, opt->rect, soImage.rect(), QRect(QPoint(5, 5), QPoint(5, 5)), QColor(0xFF, 0, 0xFF));
                }
                else
                    drawImage(soImage, *p, opt->rect, sourceRectImage(soImage.rect(), state, 6), QRect(QPoint(5, 5), QPoint(5, 5)), QColor(0xFF, 0, 0xFF));
            }
        }
    }

    if (const RibbonTab* tab = qobject_cast<const RibbonTab*>(widget))
    {
        if (pageColor != RibbonPage::ContextColorNone && tab->getContextHeader() && tab->getContextHeader()->firstTab == tab)
        {
            QRect rcSeparator(QPoint(opt->rect.left()-1, opt->rect.top()), QPoint(opt->rect.left(), opt->rect.bottom() - 2));

            QPixmap soImage = cached("ContextTabSeparator.png");
            if (!soImage.isNull()) 
                drawImage(soImage, *p, rcSeparator, soImage.rect());
        }

        if (pageColor!= RibbonPage::ContextColorNone && tab->getContextHeader() && tab->getContextHeader()->lastTab == tab)
        {
            QRect rcSeparator(QPoint(opt->rect.right(), opt->rect.top()), QPoint(opt->rect.right() + 1, opt->rect.bottom() - 2));
            QPixmap soImage = cached("ContextTabSeparator.png");
            if (!soImage.isNull()) 
                drawImage(soImage, *p, rcSeparator, soImage.rect());
        }
    }
}

/*! \internal */
void RibbonPaintManager::drawTabShapeLabel(const QStyleOption* opt, QPainter* p, const QWidget* w) const
{
    QTN_D_STYLE(RibbonStyle)
    if (const QStyleOptionHeader * optTab = qstyleoption_cast<const QStyleOptionHeader*>(opt)) 
    {
        bool selected = opt->state & QStyle::State_Selected;
        bool highlighted = opt->state & QStyle::State_MouseOver;
        int flags = optTab->textAlignment;

        flags |= Qt::TextShowMnemonic;
        if (!baseStyle()->proxy()->styleHint(QStyle::SH_UnderlineShortcut, opt, w))
            flags |= Qt::TextHideMnemonic;

        QColor textColor = d.m_clrTabNormalText;
        if (highlighted && !selected)
            textColor = d.m_clrTabHighlightText;
        else if (selected) 
            textColor = d.m_clrTabSelectedText;

        QString text = optTab->text;
        if (d.ribbonPaintManager()->isTopLevelMenuItemUpperCase(w))
            text = text.toUpper();

        // draw text
        QPalette pal = optTab->palette;
        pal.setColor(QPalette::WindowText, textColor);
        baseStyle()->proxy()->drawItemText(p, optTab->rect, flags, pal, opt->state & QStyle::State_Enabled, text, QPalette::WindowText);

        fillRibbonTabControl(opt, p, w);
    }
}

// for Slider
/*! \internal */
bool RibbonPaintManager::drawSlider(const QStyleOptionComplex* opt, QPainter* p, const QWidget* w) const
{
    if (!qobject_cast<RibbonSliderPane*>(w->parentWidget()))
        return false;

    if (const QStyleOptionSlider* slider = qstyleoption_cast<const QStyleOptionSlider*>(opt))
    {
        if (slider->orientation == Qt::Horizontal)
        {
            QRect groove = baseStyle()->proxy()->subControlRect(QStyle::CC_Slider, opt, QStyle::SC_SliderGroove, w);
            QRect handle = baseStyle()->proxy()->subControlRect(QStyle::CC_Slider, opt, QStyle::SC_SliderHandle, w);

            QPixmap soTrack = cached("SliderTrack.png");
            QRect rcTrackDest(QPoint(groove.left(), (groove.top() + groove.bottom() - soTrack.height()) / 2), 
                QSize(groove.width(), soTrack.height()));
            p->drawPixmap( rcTrackDest, soTrack, soTrack.rect() );

            if (slider->maximum >= slider->minimum) 
            {
                QPixmap soTick = cached("SliderTick.png");
                QRect rcTrackTickDest( QPoint((groove.left() + groove.right() - soTick.width()) / 2,
                    (groove.top() + groove.bottom() - soTick.height()) / 2), soTick.size() );
                drawImage(soTick, *p, rcTrackTickDest, soTick.rect(), QRect(QPoint(0, 0), QPoint(0, 0)), QColor(0xFF, 0, 0xFF));
            }
            if ((opt->subControls & QStyle::SC_SliderHandle)) 
            {
                QPixmap hndl = cached("SliderThumb.png");
                bool selected = slider->activeSubControls & QStyle::SC_SliderHandle && opt->state & QStyle::State_MouseOver;
                bool nPressetHt = slider->state & QStyle::State_Sunken;

                QRect rcSrc = sourceRectImage( hndl.rect(), nPressetHt ? 2 : selected ? 1 : 0, 3);
                QPoint point((handle.left() + handle.right() - rcSrc.width()) / 2, (handle.top() + handle.bottom() - rcSrc.height()) / 2);
                p->drawPixmap(point, hndl, rcSrc);
            }
            return true;
        }
    }
    return false;
}

/*! \internal */
void RibbonPaintManager::drawGroupScrollButton(const QStyleOption* option, QPainter* p, const QWidget* widget) const
{
    Q_UNUSED(widget)
    if (const QStyleOptionToolButton* toolbutton = qstyleoption_cast<const QStyleOptionToolButton*>(option)) 
    {
        QPixmap soRibbonScrollButton = cached( toolbutton->arrowType == Qt::LeftArrow ? 
            "RibbonGroupScrollButtonLeft.png" : "RibbonGroupScrollButtonRight.png");

        if(soRibbonScrollButton.isNull())
            return;

        QRect rc = toolbutton->rect;

        bool selected = option->state & QStyle::State_MouseOver;
        bool pressed = option->state & QStyle::State_Sunken;

        int state = pressed ? 2 : selected ? 1 : 0;

        QRect rcSrc = sourceRectImage(soRibbonScrollButton.rect(), state, 3);
        drawImage(soRibbonScrollButton, *p, rc, rcSrc, QRect(QPoint(4, 6), QPoint(3, 6)), QColor(0xFF, 0, 0xFF));

        QPixmap soRibbonScrollGlyph = cached( toolbutton->arrowType == Qt::LeftArrow ? 
            "RibbonGroupScrollLeftGlyph.png" : "RibbonGroupScrollRightGlyph.png");

        rcSrc = sourceRectImage(soRibbonScrollGlyph.rect(), state, 3);
        QSize szGlyph = rcSrc.size();

        QRect rcGlyph(QPoint((rc.right() + rc.left() - szGlyph.width()) * 0.5, (rc.top() + rc.bottom() - szGlyph.height()) *0.5), szGlyph);

        drawImage(soRibbonScrollGlyph, *p, rcGlyph, rcSrc, QRect(QPoint(0, 0), QPoint(0, 0)), QColor(0xFF, 0, 0xFF));
    }
}

// for TabBar
/*! \internal */
bool RibbonPaintManager::drawTabBarTabShape(const QStyleOption* opt, QPainter* p, const QWidget* w) const
{
    if (!qobject_cast<QMdiArea*>(w->parentWidget()))
        return OfficePaintManager::drawTabBarTabShape(opt, p, w);

    if (const QStyleOptionTab* tab = qstyleoption_cast<const QStyleOptionTab*>(opt)) 
    {
        QPixmap soImage = cached("AccessTab.png"); 
        if (soImage.isNull())
        {
            Q_ASSERT(false);
            return false;
        }

        QRect rect = opt->rect;

        bool selected = tab->state & QStyle::State_Selected;
        bool highlight = opt->state & QStyle::State_MouseOver;

        int state = 0;
        if (selected && highlight)
            state = 3;
        else if (selected)
            state = 2;
        else if (highlight)
            state = 1;

        QSize sz;
        qreal angle = 0;
        switch (tab->shape) 
        {
        case QTabBar::RoundedNorth:
        case QTabBar::TriangularNorth:
            {
                rect.adjust(0, 0, 11, 0);
                sz = QSize(rect.width(), rect.height());
                break;
            }
        case QTabBar::RoundedSouth:
        case QTabBar::TriangularSouth:
            {
                if (tab->position != QStyleOptionTab::Beginning && tab->position != QStyleOptionTab::OnlyOneTab )
                    rect.adjust(-11, 0, 0, 0);

                sz = QSize(rect.width(), rect.height());
                angle = -180;
                break;
            }
        case QTabBar::RoundedWest:
        case QTabBar::TriangularWest:
            {
                if (tab->position != QStyleOptionTab::Beginning && tab->position != QStyleOptionTab::OnlyOneTab )
                    rect.adjust(0, -11, 0, 0);
                sz = QSize(rect.height(), rect.width());
                angle = -90;
                break;
            }
        case QTabBar::RoundedEast:
        case QTabBar::TriangularEast:
            {
                rect.adjust(0, 0, 0, 11);
                sz = QSize(rect.height(), rect.width());
                angle = 90;
                break;
            }
        default:
            break;
        }

        QPixmap soImageRotate(sz);
        soImageRotate.fill(QColor(0xFF, 0, 0xFF));

        QPainter painter(&soImageRotate);
        int heightMap = sz.height()/7;
        drawImage(soImage, painter, QRect(QPoint(0, 0), sz), sourceRectImage(soImage.rect(), state, 4),
            QRect(QPoint(5, heightMap), QPoint(22, heightMap)));

        QPixmap resultImage = soImageRotate.transformed(QTransform().rotate(angle));
        resultImage.setMask(resultImage.createMaskFromColor(QColor(0xFF, 0, 0xFF)));

        p->drawPixmap(rect, resultImage);
        return true;
    }
    return false;
}

// for menuBar
/*! \internal */
bool RibbonPaintManager::drawFrameMenu(const QStyleOption* opt, QPainter* p, const QWidget* w) const
{
    if (qobject_cast<const RibbonSystemPopupBar*>(w)) 
    {
        QPixmap soRibbonSystemMenu = cached("RibbonSystemMenu.png");
        drawImage(soRibbonSystemMenu, *p, opt->rect, soRibbonSystemMenu.rect(), QRect(6, 18, 6, 29));
        return true;
    }
    else if (qobject_cast<const RibbonPageSystemPopup*>(w))
    {
        QPixmap soRibbonSystemMenu = cached("PopupBarFrame.png");
        drawImage(soRibbonSystemMenu, *p, opt->rect, soRibbonSystemMenu.rect(), QRect(4, 4, 4, 4));
        return true;
    }
    return false;
}

// for QForm
/*! \internal */
bool RibbonPaintManager::drawFrame(const QStyleOption* opt, QPainter* p, const QWidget* w) const
{
    QTN_D_STYLE(const RibbonStyle)
    if (qobject_cast<const RibbonMainWindow*>(w))
    {
        if (const StyleOptionFrame* optFrame = qstyleoption_cast<const StyleOptionFrame*>(opt))
        {
            bool active = optFrame->active;

            QRect rc = optFrame->rect;
            QRect rcBorders = optFrame->clientRect;

            int nRightBorder = rcBorders.left() - rc.left(), nLeftBorder = rcBorders.left() - rc.left(), nBorder = optFrame->frameBorder;
            int nBottomBorder = rc.bottom() - rcBorders.bottom();

            rc.translate(-rc.topLeft());
            QRect rcFrame(rc);

            int nCaptionHeight = optFrame->titleBarSize;
            rcFrame.setTop(rcFrame.top() + nCaptionHeight);

            int nStatusHeight = optFrame->statusHeight;
            bool bHasStatusBar = optFrame->hasStatusBar;

            int bordersHeight = bHasStatusBar ? rcFrame.height() - nStatusHeight - 1 : rcFrame.height();

            if (nLeftBorder > 0) 
                p->fillRect(rc.left() + 0, rcFrame.top(), 1, rcFrame.height(), active ? d.m_clrFrameBorderActive0 : d.m_clrFrameBorderInactive0);
            if (nLeftBorder > 1) 
                p->fillRect(rc.left() + 1, rcFrame.top(), 1, bordersHeight, active ? d.m_clrFrameBorderActive1 : d.m_clrFrameBorderInactive1);

            if (nRightBorder > 0) 
                p->fillRect(rc.right() - 1, rcFrame.top(), 1, rcFrame.height(), active ? d.m_clrFrameBorderActive0 : d.m_clrFrameBorderInactive0);
            if (nRightBorder > 1) 
                p->fillRect(rc.right() - 2, rcFrame.top(), 1, bordersHeight, active ? d.m_clrFrameBorderActive1 : d.m_clrFrameBorderInactive1);

            if (d.m_flatFrame)
            {
                if (nLeftBorder > 2) 
                    p->fillRect(rc.left() + 2, rcFrame.top(), nLeftBorder - 3, bordersHeight, active ? d.m_clrFrameBorderActive2 : d.m_clrFrameBorderInactive2);
                if (nLeftBorder > 2) 
                    p->fillRect(rc.left() + nLeftBorder - 1, rcFrame.top(), 1, bordersHeight, active ? d.m_clrFrameBorderActive3 : d.m_clrFrameBorderInactive3);

                if (nRightBorder > 2) 
                    p->fillRect(rc.right() - nRightBorder + 1, rcFrame.top(), nRightBorder - 3, bordersHeight, active ? d.m_clrFrameBorderActive2 : d.m_clrFrameBorderInactive2);
                if (nRightBorder > 2) 
                    p->fillRect(rc.right() - nRightBorder, rcFrame.top(), 1, bordersHeight, active ? d.m_clrFrameBorderActive3 : d.m_clrFrameBorderInactive3);
            }
            else
            {
                if (nLeftBorder > 2) 
                    p->fillRect(rc.left() + 2, rcFrame.top(), 1, bordersHeight, active ? d.m_clrFrameBorderActive2 : d.m_clrFrameBorderInactive2);
                if (nLeftBorder > 3) 
                    p->fillRect(rc.left() + 3, rcFrame.top(), nLeftBorder - 3, bordersHeight, active ? d.m_clrFrameBorderActive3 : d.m_clrFrameBorderInactive3);

                if (nRightBorder > 2) 
                    p->fillRect(rc.right() - 3, rcFrame.top(), 1, bordersHeight, active ? d.m_clrFrameBorderActive2 : d.m_clrFrameBorderInactive2);
                if (nRightBorder > 3) 
                    p->fillRect(rc.right() - nRightBorder, rcFrame.top(), nRightBorder - 3, bordersHeight, active ? d.m_clrFrameBorderActive3 : d.m_clrFrameBorderInactive3);
            }

            p->fillRect(rc.left(), rc.bottom() - 1, rc.width(), 1,
                active ? optFrame->maximizeFlags ? d.m_clrFrameBorderActive3 : d.m_clrFrameBorderActive0 : d.m_clrFrameBorderInactive0);

            if (nBottomBorder > 1)
            {
                if (d.m_flatFrame)
                {
                    p->fillRect(rc.left() + nLeftBorder, rc.bottom() - nBottomBorder, rc.width() - nLeftBorder - nRightBorder, 1, active ? d.m_clrFrameBorderActive3 : d.m_clrFrameBorderInactive3);
                    p->fillRect(rc.left() + 1, rc.bottom() - nBottomBorder + 1, rc.width() - 2, nBottomBorder - 2, active ? d.m_clrFrameBorderActive2 : d.m_clrFrameBorderInactive2);
                }
                else
                {
                    p->fillRect(rc.left() + 1, rc.bottom() - nBottomBorder, rc.width() - 2, nBottomBorder - 1, active ? d.m_clrFrameBorderActive3 : d.m_clrFrameBorderInactive3);
                }
            }

            QRect rcCaption(QPoint(rc.left(), rc.top()), QPoint(rc.right(), rc.top() + nCaptionHeight));
            if (optFrame->titleVisible)
            {
                QRect rcTopLeft, rcTopRight, rcTopCenter, rcSrcTopLeft, rcSrcTopRight;

                QPixmap soImage = cached("FrameTopLeft.png");
                if (!soImage.isNull())
                {
                    rcSrcTopLeft = sourceRectImage(soImage.rect(), active ? 0 : 1, 2);
                    rcTopLeft = QRect(QPoint(rc.left(), rc.top()), QPoint(rc.left() + rcSrcTopLeft.width(), rcCaption.bottom()));
                    drawImage(soImage, *p, rcTopLeft, rcSrcTopLeft, QRect(QPoint(0, 5), QPoint(0, 3)));
                }
                soImage = cached("FrameTopRight.png");
                if (!soImage.isNull())
                {
                    rcSrcTopRight = sourceRectImage(soImage.rect(), active ? 0 : 1, 2);
                    rcTopRight = QRect(QPoint(rc.right() - rcSrcTopRight.width() - 1, rc.top()), QPoint(rc.right() - 1, rcCaption.bottom()));
                    drawImage(soImage, *p, rcTopRight, rcSrcTopRight, QRect(QPoint(0, 5), QPoint(0, 3)));
                }
                soImage = cached("FrameTopCenter.png");
                if (!soImage.isNull())
                {
                    rcTopCenter = QRect(QPoint(rc.left() + rcTopLeft.width(), rc.top()), QPoint(rc.right() - rcSrcTopRight.width(), rcCaption.bottom()));
                    drawImage(soImage, *p, rcTopCenter, sourceRectImage(soImage.rect(), active ? 0 : 1, 2), QRect(QPoint(0, 5), QPoint(0, 3)));
                }
            }
            else
            {
                QRect rcSrc;
                QPixmap soImage = cached("FrameTopLeft.png");
                if (!soImage.isNull())
                {
                    rcSrc = sourceRectImage(soImage.rect(), active ? 0 : 1, 2);
                    rcSrc.setRight(nLeftBorder);

                    QRect rcTopLeft(QPoint(rc.left(), rcCaption.top()), QPoint(rc.left() + nLeftBorder, rcCaption.bottom()));
                    drawImage(soImage, *p, rcTopLeft, rcSrc, QRect(QPoint(0, 5), QPoint(0, 3)));
                }
                soImage = cached("FrameTopRight.png");
                if (!soImage.isNull())
                {
                    rcSrc = sourceRectImage(soImage.rect(), active ? 0 : 1, 2);
                    rcSrc.setLeft(rcSrc.right() - nRightBorder);

                    QRect rcTopRight(QPoint(rc.right() - nRightBorder - 1, rcCaption.top()), QPoint(rc.right()-1, rcCaption.bottom()));
                    drawImage(soImage, *p, rcTopRight, rcSrc, QRect(QPoint(0, 5), QPoint(0, 3)));
                }
                soImage = cached("FrameTopCenter.png");
                if (!soImage.isNull())
                {
                    rcSrc = sourceRectImage(soImage.rect(), active ? 0 : 1, 2);
                    rcSrc.setBottom(rcSrc.top() + nBorder);

                    QRect rcTopCenter(QPoint(rc.left() + nLeftBorder, rc.top()), QPoint(rc.right() - nRightBorder - 1, rc.top() + nBorder));
                    drawImage(soImage, *p, rcTopCenter, rcSrc);
                }
            }

            if (optFrame->hasStatusBar)
            {
                QRect rcSrc;
                QPixmap soImage;
                if (!d.m_flatFrame)
                {
                    soImage = cached("StatusBarLight.png");
                    if (!soImage.isNull())
                    {
                        rcSrc = QRect(QPoint(0, 0), QPoint(nLeftBorder - 1, soImage.height()));
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
                        QRect rcLight(QPoint(rc.left() + 1, rc.bottom() - nStatusHeight - nBottomBorder + 1), QPoint(rc.left() + nLeftBorder, rc.bottom() - nBottomBorder));
#else
                        QRect rcLight(QPoint(rc.left() + 1, rc.bottom() - nStatusHeight - nBottomBorder), QPoint(rc.left() + nLeftBorder, rc.bottom() - nBottomBorder));
#endif
                        drawImage(soImage, *p, rcLight, rcSrc);
                    }
                    soImage = cached("StatusBarDark.png");
                    if (!soImage.isNull())
                    {
                        rcSrc = QRect(QPoint(0, 0), QPoint(nRightBorder - 1, soImage.height()));
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
                        QRect rcDark(QPoint(rc.right() - nRightBorder, rc.bottom() - nStatusHeight - nBottomBorder + 1), QPoint(rc.right()-2, rc.bottom() - nBottomBorder));
#else
                        QRect rcDark(QPoint(rc.right() - nRightBorder, rc.bottom() - nStatusHeight - nBottomBorder), QPoint(rc.right()-2, rc.bottom() - nBottomBorder));
#endif
                        drawImage(soImage, *p, rcDark, rcSrc);
                    }
                }
                else
                {
                    QRect rcLight(QPoint(rc.left() + 1, rc.bottom() - nStatusHeight - nBottomBorder), QPoint(rc.left() + nLeftBorder, rc.bottom() - nBottomBorder + 1));
                    p->fillRect(rcLight, active ? d.m_clrFrameBorderActive2 : d.m_clrFrameBorderInactive2);

                    QRect rcDark(QPoint(rc.right() - nRightBorder, rc.bottom() - nStatusHeight - nBottomBorder), QPoint(rc.right() - 2, rc.bottom() - nBottomBorder + 1));
                    p->fillRect(rcDark, active ? d.m_clrFrameBorderActive2 : d.m_clrFrameBorderInactive2);

                    if (nBottomBorder > 1)
                    {
                        p->fillRect(QRect(QPoint(rc.left() + nLeftBorder - 1, rc.bottom() - nStatusHeight - nBottomBorder), QSize(1, nStatusHeight)),
                            active ? d.m_clrFrameBorderActive3 : d.m_clrFrameBorderInactive3);
                        p->fillRect(QRect(QPoint(rc.right() - nRightBorder, rc.bottom() - nStatusHeight - nBottomBorder), QSize(1, nStatusHeight)),
                            active ? d.m_clrFrameBorderActive3 : d.m_clrFrameBorderInactive3);
                    }
                }
            }

            bool roundedCornersAlways = true;
            if (optFrame->hasStatusBar || roundedCornersAlways)
            {
                QRect rcSrc;
                QPixmap soImage;
                if (nLeftBorder > 3)
                {
                    soImage = cached("FrameBottomLeft.png");
                    if (!soImage.isNull())
                    {
                        rcSrc = sourceRectImage(soImage.rect(), active ? 0 : 1, 2);
                        QRect rcBottomLeft(QPoint(rc.left(), rc.bottom() - rcSrc.height()), QPoint(rc.left() + rcSrc.width(), rc.bottom()));
                        drawImage(soImage, *p, rcBottomLeft, rcSrc, QRect(QPoint(0, 0), QPoint(0, 0)), QColor(0xFF, 0, 0xFF));
                    }
                }

                if (nRightBorder > 3)
                {
                    soImage = cached("FrameBottomRight.png");
                    if (!soImage.isNull())
                    {
                        rcSrc = sourceRectImage(soImage.rect(), active ? 0 : 1, 2);
                        QRect rcBottomRight(QPoint(rc.right() - rcSrc.width()-1, rc.bottom() - rcSrc.height()), QPoint(rc.right()-1, rc.bottom()));
                        drawImage(soImage, *p, rcBottomRight, rcSrc, QRect(QPoint(0, 0), QPoint(0, 0)), QColor(0xFF, 0, 0xFF));
                    }
                }
            }
            return true;
        }
    }
    return false;
}

/*! \internal */
bool RibbonPaintManager::drawShapedFrame(const QStyleOption* opt, QPainter* p, const QWidget* w) const
{
    QTN_D_STYLE(const RibbonStyle)
    bool ret = false;
    if (const QMdiArea* mdiArea = qobject_cast<const QMdiArea*>(w))
    {
        #if (QT_VERSION < QT_VERSION_CHECK(5, 7, 0))
        if (const QStyleOptionFrameV3* f = qstyleoption_cast<const QStyleOptionFrameV3*>(opt)) 
        #else
        if (const QStyleOptionFrame* f = qstyleoption_cast<const QStyleOptionFrame*>(opt))
        #endif
        {
            int frameShape  = f->frameShape;
            int frameShadow = QFrame::Plain;
            if (f->state & QStyle::State_Sunken)
                frameShadow = QFrame::Sunken;
            else if (f->state & QStyle::State_Raised)
                frameShadow = QFrame::Raised;

            switch (frameShape)
            {
            case QFrame::Panel:
                {
                    if (frameShadow == QFrame::Sunken)
                    {
                        QRect rect = opt->rect;
                        if (QTabBar* tabBar = mdiArea->findChild<QTabBar*>())
                        {
                            int lw = f->lineWidth;
                            int mlw = f->midLineWidth;

                            p->fillRect(opt->rect, d.m_clrHighlightChecked);

                            switch (mdiArea->tabPosition())
                            {
                            case QTabWidget::North:
                                rect.adjust(0, tabBar->sizeHint().height()-1, 0, 0);
                                break;
                            case QTabWidget::South:
                                rect.adjust(0, 0, 0, -tabBar->sizeHint().height()-1);
                                break;
                            case QTabWidget::West:
                                rect.adjust(tabBar->sizeHint().width()-1, 0, 0, 0);
                                break;
                            case QTabWidget::East:
                                rect.adjust(0, 0, -tabBar->sizeHint().width()-1, 0);
                                break;
                            default:
                                break;
                            }
                            rect.adjust(0, 0, -1, -1);
                            QPen savePen = p->pen();
                            p->setPen(opt->palette.dark().color());
                            p->drawRect(rect);
                            p->setPen(savePen);

                            rect.adjust(2, 2, -1, -1);
                            qDrawShadeRect(p, rect, opt->palette, true, lw, mlw);
                        }
                        ret = true;
                    }
                }
                break;
            default:
                break;
            }
        }
    }
    return ret;
}

// for file menu
/*! \internal */
void RibbonPaintManager::drawSystemButton(const QStyleOption* option, QPainter* p, const QWidget* widget) const
{
    QTN_D_STYLE(RibbonStyle)
    if (const StyleSystemToolButton* toolbutton = qstyleoption_cast<const StyleSystemToolButton*>(option))
    {
        if (qobject_cast<const RibbonBar*>(widget->parentWidget()))
        {
            if (toolbutton->toolButtonStyle != Qt::ToolButtonFollowStyle)
            {
                QPixmap soRibbonFileButton = cached("FileButton.png");
                bool popuped  = (toolbutton->activeSubControls & QStyle::SC_ToolButtonMenu) && (option->state & QStyle::State_Sunken);
                bool isHighlighted = option->state & QStyle::State_MouseOver;
                bool enabled = widget->isEnabled();

                bool usePalette = false;
                uint resolve_mask = toolbutton->palette.resolve();
                if (resolve_mask & (1 << QPalette::Button)) 
                    usePalette = true;

                if (usePalette)
                {
                    QBrush brButton = toolbutton->palette.brush(QPalette::Active, QPalette::Button);
                    QBrush brButtonDisabled = toolbutton->palette.brush(QPalette::Disabled, QPalette::Button);
                    QBrush brHighlightButton = (resolve_mask & (1 << QPalette::Highlight)) ? toolbutton->palette.brush(QPalette::Active, QPalette::Highlight) : brButton;
                    QBrush brPopupButton = (resolve_mask & (1 << QPalette::Dark)) ? toolbutton->palette.brush(QPalette::Active, QPalette::Dark) : brButton;

                    QBrush brush = !enabled ? brButtonDisabled : popuped ? (isHighlighted ? brHighlightButton : brPopupButton) : 
                        false ? brButton : isHighlighted ? brHighlightButton : brButton;

                    p->fillRect(toolbutton->rect, brush);

/*
                    QRect rcFill = toolbutton->rect.adjusted(0,1,0,0);
                    QColor color = !enabled ? toolbutton->colorBackground.dark(20) : popuped ? (selected ? toolbutton->colorBackground.dark(105) : toolbutton->colorBackground.dark(115)) : 
                        false ? toolbutton->colorBackground : selected ? toolbutton->colorBackground.dark(105) : toolbutton->colorBackground;

                    QBrush brushSave = p->brush();
                    QPen penSave = p->pen();

                    QPoint pt = rcFill.center();
                    QRadialGradient gradient(pt.x(), pt.y(), rcFill.width());

                    gradient.setColorAt(0.0, color.darker(150));
                    gradient.setColorAt(1.0, color);

                    //    p.setBrush(gradient);
                    p->setRenderHint(QPainter::HighQualityAntialiasing);
                    int x1 = rcFill.left();
                    int x2 = rcFill.right();
                    int y1 = rcFill.top();
                    int y2 = rcFill.bottom();
                    p->fillRect(QRect(x1 + 1, y1 + 1, (x2 - x1) - 2, (y2 - y1) - 1), gradient);

                    ::drawBorderSystemButton(p, rcFill, color.dark());
                    ::drawBorderSystemButton(p, rcFill.adjusted(1, 1, -1, 0), color.dark(120));

                    p->setPen(penSave);
                    p->setBrush(brushSave);
*/

                }
                else if (d.m_fileButtonImageCount == 7)
                {
                    int state = !enabled ? 5 : popuped ? (isHighlighted ? 3 : 2) : false ? 4 : isHighlighted ? 1 : 0;

                    QRect rcSrc = sourceRectImage(soRibbonFileButton.rect(), state, 7);
                    QRect rcDest = toolbutton->rect.adjusted(0, 1, 0, 0);
                    drawImage(soRibbonFileButton, *p, rcDest, rcSrc, QRect(QPoint(3, 3), QPoint(3, 3)));
                }
                else
                {
                    int state = popuped ? 2 : isHighlighted ? 1 :  0;

                    QRect rcSrc = sourceRectImage(soRibbonFileButton.rect(), state, 3);
                    drawImage(soRibbonFileButton, *p, toolbutton->rect.adjusted(0,1,0,0), rcSrc, QRect(QPoint(3, 3), QPoint(3, 3)));
                }

                if (toolbutton->toolButtonStyle == Qt::ToolButtonIconOnly && !toolbutton->icon.isNull())
                {
                    bool enabled  = toolbutton->state & QStyle::State_Enabled;
                    QRect rc = toolbutton->rect;
                    QPixmap pxIcon = toolbutton->icon.pixmap(toolbutton->iconSize, 
                        enabled ? QIcon::Normal : QIcon::Disabled, isHighlighted ? QIcon::On : QIcon::Off);
                    QPoint ptIcon((rc.left() + rc.right() - pxIcon.width()) / 2, (rc.top() + rc.bottom() + 1 - pxIcon.height()) / 2);
                    p->drawPixmap(ptIcon, pxIcon);
                }
                return;
            }
        }

        QPixmap soImage;
        if (toolbutton->rect.width() > 54 && toolbutton->rect.height() > 54)
            soImage = cached("FrameSystemButton52.png");
        if (soImage.isNull())
            soImage = cached("FrameSystemButton.png");

        Q_ASSERT(!soImage.isNull());
        if (soImage.isNull())
            return;

        QRect rc = toolbutton->rect;
        bool enabled  = toolbutton->state & QStyle::State_Enabled;
        bool selected = toolbutton->state & QStyle::State_MouseOver;
        bool popuped  = (toolbutton->activeSubControls & QStyle::SC_ToolButtonMenu) && (toolbutton->state & QStyle::State_Sunken);

        int state = 0;
        if (popuped)
            state = 4;
        else if (selected)
            state = 2;
        else if (!enabled)
            state = 1;

        QRect rcSrc(sourceRectImage(soImage.rect(), state == 0 ? 0 : state == 2 ? 1 : 2, 3));
        QSize szDest = rcSrc.size();
        QPoint ptDest((rc.left() + rc.right() - szDest.width()) / 2, (rc.top() + rc.bottom() - szDest.height()) / 2);
        drawImage(soImage, *p, QRect(ptDest, szDest), rcSrc, QRect(QPoint(0, 0), QPoint(0, 0)));


        if (!toolbutton->icon.isNull())
        {
            QRect rc = toolbutton->rect;
            bool enabled  = toolbutton->state & QStyle::State_Enabled;
            bool selected = toolbutton->state & QStyle::State_MouseOver;
            QPixmap pxIcon = toolbutton->icon.pixmap(toolbutton->iconSize, 
                enabled ? QIcon::Normal : QIcon::Disabled, selected ? QIcon::On : QIcon::Off);
            QPoint ptIcon((rc.left() + rc.right() - pxIcon.width()) / 2, (rc.top() + rc.bottom()/*ЭМ+ 1*/ - pxIcon.height()) / 2);
            p->drawPixmap(ptIcon.x(), ptIcon.y(), pxIcon);
        }
    }
}

/*! \internal */
void RibbonPaintManager::drawQuickAccessButton(const QStyleOption* opt, QPainter* p, const QWidget* w) const
{
    Q_UNUSED(w);
    if (const QStyleOptionToolButton* toolbutton = qstyleoption_cast<const QStyleOptionToolButton*>(opt)) 
    {
        QPixmap soQuickButton = cached("RibbonQuickAccessButton.png");
        Q_ASSERT(!soQuickButton.isNull());
        if (soQuickButton.isNull())
            return;

        bool selected = opt->state & QStyle::State_MouseOver;
        bool popuped  = (toolbutton->activeSubControls & QStyle::SC_ToolButtonMenu) && (opt->state & QStyle::State_Sunken);

        drawImage(soQuickButton, *p, toolbutton->rect, sourceRectImage(soQuickButton.rect(), !w->isEnabled() ? 4 : popuped ? 2 : selected ? 1  : 0, 5), 
            QRect(QPoint(2, 2), QPoint(2, 2)), QColor(0xFF, 0, 0xFF));
    }
}

/*! \internal */
void RibbonPaintManager::drawOptionButton(const QStyleOption* option, QPainter* p, const QWidget* widget) const
{
    Q_UNUSED(widget);
    QPixmap soRibbonOptionButton = cached("RibbonOptionButton.png");

    if(soRibbonOptionButton.isNull())
        return;

    bool selected = option->state & QStyle::State_MouseOver;
    bool pressed = option->state & QStyle::State_Sunken;
    bool enabled = option->state & QStyle::State_Enabled;

    int state = 0;
    if (!enabled) state = 3;
    else if (pressed) state = 2;
    else if (selected) state = 1;

    QRect rcSrc = sourceRectImage(soRibbonOptionButton.rect(), state, 4);
    drawImage(soRibbonOptionButton, *p, option->rect, rcSrc, QRect(QPoint(3, 2), QPoint(2, 2)), QColor(0xFF, 0, 0xFF));
}

/*! \internal */
void RibbonPaintManager::drawPopupResizeGripper(const QStyleOption* option, QPainter* p, const QWidget* widget) const
{
    Q_UNUSED(widget);
    if (const QStyleOptionSizeGrip* sizeGrip = qstyleoption_cast<const QStyleOptionSizeGrip*>(option)) 
    {
        QPixmap soImage = cached("PopupBarResizeGripperFace.png");
        Q_ASSERT(!soImage.isNull());
        if (soImage.isNull())
            return;

        drawImage(soImage, *p, sizeGrip->rect, soImage.rect(), QRect(QPoint(1, 1), QPoint(1, 1)), QColor(0xFF, 0, 0xFF));

        soImage = cached("PopupBarResizeGripperWidth.png");

        QRect rc = soImage.rect();
        QRect rcDest(QPoint(sizeGrip->rect.right() - rc.width(), sizeGrip->rect.bottom() - rc.height()), rc.size());
        drawImage(soImage, *p, rcDest, rc, QRect(QPoint(0, 0), QPoint(0, 0)), QColor(0xFF, 0, 0xFF));
    }
}

/*! \internal */
bool RibbonPaintManager::drawIndicatorArrow(QStyle::PrimitiveElement pe, const QStyleOption* opt, QPainter* p, const QWidget* w) const
{
    if (getParentWidget<RibbonGroup>(w) || getParentWidget<RibbonBar>(w) || qobject_cast<const RibbonBackstageButton*>(w))
    {
        switch(pe)
        {
            case QStyle::PE_IndicatorArrowDown :
                if (qobject_cast<const QToolButton*>(w))
                {
                    if (!qobject_cast<const QTabBar*>(w->parentWidget()))
                    {
                        QPixmap soIndicator = cached("ToolbarButtonDropDownGlyph.png");
                        bool enabled = opt->state & QStyle::State_Enabled;    
                        bool selected = opt->state & QStyle::State_Selected;

//                        QRect rectImage = opt->rect;
                        QRect rcSrc = sourceRectImage(soIndicator.rect(), !enabled ? 3 : selected ? 1 : 0, 4);
                        const QSize sizeImage = rcSrc.size();

                        QPoint ptImage(opt->rect.left() + (opt->rect.width() - sizeImage.width()) / 2  + ((opt->rect.width()  - sizeImage.width())  % 2), 
                            opt->rect.top()  + (opt->rect.height() - sizeImage.height()) / 2 + ((opt->rect.height() - sizeImage.height()) % 2));

                        QRect rcEntry(ptImage, rcSrc.size());
                        drawImage(soIndicator, *p, rcEntry, rcSrc, QRect(QPoint(0, 0), QPoint(0, 0)), QColor(0xFF, 0, 0xFF));
                        return true;
                    }
                }
                break;
            default:
                break;
        }
    }
    return OfficePaintManager::drawIndicatorArrow(pe, opt, p, w);
}

/*! \internal */
void RibbonPaintManager::drawRectangle(QPainter* p, const QRect& rect, bool selected, bool pressed, bool enabled, bool checked, bool popuped,
    Qtitan::BarType barType, Qtitan::BarPosition barPos) const
{
    if (barType != TypePopup)
    {
        bool smallSize = rect.height() < 33;
        QPixmap pixmap = smallSize ? cached("ToolbarButtons22.png") : cached("ToolbarButtons50.png");
        Q_ASSERT(!pixmap.isNull());
        if (pixmap.isNull())
            return;

        int state = -1;

        if (!enabled)
        {
            //            if (isKeyboardSelected(selected)) state = 0;
        }
        else if (popuped) state = 2;
        else if (checked && !selected && !pressed) state = 2;
        else if (checked && selected && !pressed) state = 3;
        else if (/*isKeyboardSelected(pressed) ||*/ (selected && pressed)) state = 1;
        else if (selected || pressed) state = 0;

        if (state != -1)
            drawImage(pixmap, *p, rect, sourceRectImage(pixmap.rect(), state, 4), QRect(QPoint(8, 8), QPoint(8, 8)));
    }
    else if (barPos == BarPopup && selected && barType == TypePopup)
    {
        bool smallSize = rect.height() < 33;
        QPixmap soButton = !enabled ? smallSize ? cached("MenuSelectedDisabledItem22.png") : cached("MenuSelectedDisabledItem54.png") :
            smallSize ? cached("MenuSelectedItem22.png") : cached("MenuSelectedItem54.png");
        QRect rcSrc = soButton.rect();
        drawImage(soButton, *p, rect, rcSrc, QRect(QPoint(4, 4), QPoint(4, 4)));
    }
    else
    {
        OfficePaintManager::drawRectangle(p, rect, selected, pressed, enabled, checked, popuped, barType, barPos);
    }
}

/*! \internal */
void RibbonPaintManager::drawKeyTip(const QStyleOption* opt, QPainter* p, const QWidget* widget) const
{
    Q_UNUSED(widget);
    QTN_D_STYLE(RibbonStyle)

    DrawHelpers::drawGradientFill(*p, opt->rect, d.m_clrTooltipLight, d.m_clrTooltipDark, true);

    QPixmap soImage = cached("TooltipFrame.png");
    if (!soImage.isNull())
        drawImage(soImage, *p, opt->rect, sourceRectImage(soImage.rect(), 0, 1), QRect(QPoint(3, 3), QPoint(3, 3)), QColor(0xFF, 0, 0xFF));
    else
    {
        const QPen oldPen = p->pen();
        p->setPen(d.m_clrTooltipBorder);
        p->drawRect(opt->rect.adjusted(0, 0, -1, -1));
        p->setPen(oldPen);
    }
}

/*! \internal */
void RibbonPaintManager::drawBackstage(const QStyleOption* opt, QPainter* p, const QWidget* widget) const
{
    Q_UNUSED(widget);
    if (const StyleOptionRibbonBackstage* optBackstage = qstyleoption_cast<const StyleOptionRibbonBackstage*>(opt)) 
    {
        p->fillRect(opt->rect, QColor(255, 255, 255));

        QPixmap pixmap = cached("BackstageTopBorder.png");
        if (!pixmap.isNull())
            p->drawPixmap(QRect(QPoint(0, 0), QSize(opt->rect.width(), 2)), pixmap, pixmap.rect());

        pixmap = cached("BackstageMenuBackground.png");
        if (!pixmap.isNull())
            p->drawPixmap(QRect(QPoint(0, 2), QPoint(optBackstage->menuWidth, opt->rect.bottom())), pixmap, pixmap.rect());
    }
}

/*! \internal */
void RibbonPaintManager::drawRibbonBackstageMenu(const QStyleOption* opt, QPainter* p, const QWidget* widget) const
{
    QTN_D_STYLE(const RibbonStyle)
    if (const QStyleOptionMenuItem* menuitem = qstyleoption_cast<const QStyleOptionMenuItem*>(opt))
    {
        bool act = menuitem->state & QStyle::State_Selected;
        bool focus = menuitem->state & QStyle::State_HasFocus;
        bool dis = !(menuitem->state & QStyle::State_Enabled);

        QPalette pal = menuitem->palette;
        QColor clrText = focus ? QColor(Qt::white) : helper().getColor("Ribbon", "BackstageMenuTextColor", QColor(Qt::black));
        if ( dis )
            clrText = menuitem->palette.color(QPalette::Disabled, QPalette::WindowText).light(); //QColor(143, 143, 143);

        pal.setColor(QPalette::WindowText, clrText);

        QRect rcText(opt->rect);
        rcText.adjust(15, 2, 2, 2);

        if (menuitem->menuItemType == QStyleOptionMenuItem::Normal)
        {
            if (act || focus)
            {
                QPixmap soImage = cached("BackstageMenuCommand.png");
                if (!soImage.isNull())
                    drawImage(soImage, *p, opt->rect, sourceRectImage(soImage.rect(), 0, 3), QRect(QPoint(2, 2), QPoint(2, 2)));
            }
        }
        else if (menuitem->menuItemType == QStyleOptionMenuItem::SubMenu)
        {
            if (act || focus)
            {
                QPixmap soImage = cached("BackstageMenuPage.png");
                if (!soImage.isNull())
                    drawImage(soImage, *p, opt->rect, sourceRectImage(soImage.rect(), focus ? 1 : 0, 4), QRect(QPoint(2, 2), QPoint(2, 2)));

                if (focus)
                {
                    soImage = cached("BackstageMenuPageGlyph.png");
                    if (!soImage.isNull())
                    {
                        QRect rcSrc = soImage.rect();
                        QRect rcGlyph(QPoint(opt->rect.right() - rcSrc.width() + 1, (opt->rect.top() + opt->rect.bottom() - rcSrc.height()) / 2), rcSrc.size());
                        p->drawPixmap(rcGlyph, soImage, rcSrc);
                    }
                }
            }
        }
        else if (menuitem->menuItemType == QStyleOptionMenuItem::Separator)
        {
            d.officePaintManager()->drawMenuItemSeparator(opt, p, widget);
            return;
        }

        int iconWidth = 0;
        int indent = menuitem->menuItemType == QStyleOptionMenuItem::SubMenu ? 6 : 0;
        if (!menuitem->icon.isNull()) 
        {
            int iconSize = baseStyle()->pixelMetric(QStyle::PM_LargeIconSize, opt, widget);
            if (iconSize > qMin(opt->rect.height(), opt->rect.width()))
                iconSize = baseStyle()->pixelMetric(QStyle::PM_SmallIconSize, opt, widget);

            QIcon::Mode mode = dis ? QIcon::Disabled : QIcon::Normal;
            QPixmap pixmap = menuitem->icon.pixmap(iconSize, mode);
            QPoint pmr(rcText.left() + indent, (rcText.top() + rcText.bottom() - pixmap.height()) / 2);
            p->drawPixmap(pmr, pixmap);
            iconWidth = pixmap.width();
        }

        indent += !menuitem->icon.isNull() ? 7 : 0;
        rcText.setLeft(rcText.left() + iconWidth + indent);

        if (!menuitem->text.isEmpty())
        {
            int flags = 0;
            flags |= Qt::TextHideMnemonic | Qt::AlignVCenter | Qt::TextSingleLine;

            int index = menuitem->text.indexOf(QLatin1String("&"));
            if (index != -1)
            {
                index = menuitem->text.indexOf(QLatin1String(" "), index);
                if (index != -1)
                    flags &= ~Qt::TextHideMnemonic;
            }
            // draw text
            baseStyle()->proxy()->drawItemText(p, rcText, flags, pal, opt->state & QStyle::State_Enabled, menuitem->text, QPalette::WindowText);
        }
    }
}

/*! \internal */
void RibbonPaintManager::drawRibbonBackstageCloseButton(const QStyleOption* opt, QPainter* p, const QWidget* widget) const
{
    Q_UNUSED(opt);
    Q_UNUSED(p);
    Q_UNUSED(widget);
}

/*! \internal */
bool RibbonPaintManager::drawRibbonBackstageSeparator(const QStyleOption* opt, QPainter* p, const QWidget* widget) const
{
    Q_UNUSED(widget);
    bool ret = false;
#if (QT_VERSION < QT_VERSION_CHECK(5, 7, 0))
    if (const QStyleOptionFrameV3* f = qstyleoption_cast<const QStyleOptionFrameV3*>(opt))
#else
    if (const QStyleOptionFrame* f = qstyleoption_cast<const QStyleOptionFrame*>(opt))
#endif
    {
        p->fillRect(opt->rect, QColor(255, 255, 255));

        if (f->frameShape == QFrame::HLine)
        {
            const uchar _cb[] = {0xcc, 0xcc, 0xcc, 0xcc, 0xcc, 0xcc, 0xcc, 0xcc};
            QBitmap cb = QBitmap::fromData(QSize(8, 8), _cb);
            QBrush brush(QColor(199, 200, 201), cb);
            QRect rc(QPoint(opt->rect.left(), opt->rect.height() / 2), QSize(opt->rect.width(), 1));
            p->fillRect(rc, brush);
            ret = true;
        }
        else if (f->frameShape == QFrame::VLine)
        {
            QPixmap soImage = cachedPath(pathForBackstageImages() +"BackstagePaneSeparator.png");
            if (!soImage.isNull())
            {
                QRect rc(QPoint(opt->rect.width()/2 - 1, opt->rect.top()), QSize(2, opt->rect.height()));
                drawImage(soImage,*p, rc, sourceRectImage(soImage.rect(), 0, 1), QRect(QPoint(0, 30), QPoint(0, 30)));
            }
            ret = true;
        }
    }
    return ret;
}

/*! \internal */
void RibbonPaintManager::setupPalette(QWidget* widget) const
{
    if (qobject_cast<QMdiArea*>(widget))
    {
        QPalette palette = widget->palette();
        QColor color = helper().getColor("TabManager", "AccessBorder");
        QColor light = color.lighter(260);
        QColor dark = color.darker(120);
        dark.setAlpha(155);
        palette.setColor(QPalette::Dark, dark);
        palette.setColor(QPalette::Light, light);
        widget->setPalette(palette);
    }
}

/*! \internal */
void RibbonPaintManager::drawRibbonSliderButton(const QStyleOption* opt, QPainter* p, const QWidget* w) const
{
    Q_UNUSED(w);
    QPixmap soButton = opt->direction == Qt::RightToLeft ? cached("SliderUp.png") : cached("SliderDown.png");

    bool selected = opt->state & QStyle::State_MouseOver;
    bool pressed  = opt->state & QStyle::State_Sunken;
    int state = 0;
    if (pressed)
        state = 2;
    else if (selected)
        state = 1;
    drawImage(soButton, *p, opt->rect, sourceRectImage(soButton.rect(), state, 3));
}

// for stausBar
/*! \internal */
bool RibbonPaintManager::drawPanelStatusBar(const QStyleOption* opt, QPainter* p, const QWidget* w) const
{
    int widthPermanent = 0;
    if (const RibbonStatusBar* statusBar = qobject_cast<const RibbonStatusBar*>(w))
    {
        QRect rect = statusBar->childrenPermanentRect();
        widthPermanent = rect.width();
    }

    QPixmap soDevider = cached("StatusBarDevider.png");
    const int widthDevider = soDevider.width();
    int nStatusHeight = opt->rect.height();
    QRect rc( opt->rect );
    int bRightOffset = widthPermanent == 0 ? w->childrenRect().width() : widthPermanent;
    int nWidth = rc.width() - bRightOffset - widthDevider;

    QPixmap soLight = cached("StatusBarLight.png");
    QRect rcSrc = QRect( QPoint(0, 0), QPoint(0, soLight.height()) );
    QRect rcLight(QPoint(rc.left(), rc.top()), QPoint(nWidth, rc.bottom()+1) );
    drawImage( soLight, *p, rcLight, rcSrc, QRect(QPoint(0, 0), QPoint(0, 0)) );

    QPixmap soDark = cached("StatusBarDark.png");
    rcSrc = QRect(QPoint(0, 0), QPoint(0, soDark.height()) );
    QRect rcDark( QPoint(nWidth + widthDevider, rc.bottom() - nStatusHeight+1), QPoint(rc.right(), rc.bottom()+1) );
    drawImage( soDark, *p, rcDark, rcSrc, QRect(QPoint(0, 0), QPoint(0, 0)) );

    rcSrc = QRect( QPoint(0, 0), QPoint(soDevider.width(), soDevider.height()) );
    QRect rcDevider( QPoint(nWidth, rc.bottom() - nStatusHeight+1), QSize(rcSrc.width(), rc.height()+1) );
    drawImage( soDevider, *p, rcDevider, rcSrc, QRect(QPoint(0, 0), QPoint(0, 0)) );
    return true;
}

// for SizeGrip
/*! \internal */
bool RibbonPaintManager::drawSizeGrip(const QStyleOption* opt, QPainter* p, const QWidget* w) const
{
    Q_UNUSED(opt);
    Q_UNUSED(w);
    QPixmap soGripper = cached("StatusBarGripper.png");
    QRect rcSrc = soGripper.rect();
    QRect rcGripper( QPoint(/*rc.right()-rcSrc.width(), rc.bottom()-rcSrc.height()*/2,-1), QSize(rcSrc.width(), rcSrc.height()));
    drawImage(soGripper, *p, rcGripper, rcSrc, QRect(QPoint(0, 0), QPoint(0, 0)), QColor(0xFF, 0, 0xFF));
    return true;
}



/* RibbonPaintManager2013 */
/*! \internal */
RibbonPaintManager2013::RibbonPaintManager2013(CommonStyle* baseStyle)
    : OfficePaintManager2013(baseStyle)
{
}

/*! \internal */
RibbonPaintManager2013::~RibbonPaintManager2013()
{
}

/*! \internal */
QIcon RibbonPaintManager2013::qtnStandardIcon(QStyle::StandardPixmap px, const QStyleOption* opt, const QWidget* wd, bool& ret ) const
{
    Q_UNUSED(px);
    Q_UNUSED(opt);
    Q_UNUSED(wd);
    ret = false;
    return QIcon();
}

// for RibbonBar
/*! \internal */
void RibbonPaintManager2013::drawRibbonBar(const QStyleOption* option, QPainter* p, const QWidget* widget) const
{
    Q_UNUSED(widget)
    if (const StyleOptionRibbon* optRibbonBar = qstyleoption_cast<const StyleOptionRibbon*>(option))
    {
        p->fillRect(optRibbonBar->rect, ribbonBarColor(optRibbonBar->isBackstageVisible));

        if (!optRibbonBar->pixTitleBackground.isNull())
        {
            QRect rect = optRibbonBar->rect;
            QRect rectSrc = optRibbonBar->pixTitleBackground.rect();
            QRect rectDst = rect;
            rectDst.setLeft(rectDst.right() - rectSrc.width() + 1);
            {
                QRect rt(rectDst);
                rectDst.intersects(rect);

                if (0 < rectDst.width () && rectDst.width() != rectSrc.width())
                {
                    rectSrc.setLeft(rectSrc.left() + (rectDst.left() - rt.left()));
                    rectSrc.setRight(rectSrc.left() + qMin(rectDst.width(), rectSrc.width()));
                }
            }

            rectDst.setBottom(rectDst.top() + rectSrc.height() - 1);
            {
                QRect rt(rectDst);
                rectDst.intersects(rect);

                if (0 < rectDst.height() && rectDst.height() != rectSrc.height())
                {
                    rectSrc.setTop(rectSrc.top() + (rectDst.top() - rt.top()));
                    rectSrc.setBottom(rectSrc.top() + qMin(rectDst.height(), rectSrc.height()));
                }
            }
            drawPixTitleBackground(p, rectDst, optRibbonBar->pixTitleBackground, rectSrc, widget);
        }
    }
}

/*! \internal */
void RibbonPaintManager2013::drawPixTitleBackground(QPainter* p, const QRect& rectDst, const QPixmap& pixTitleBackground, const QRect& rectSrc, const QWidget* widget) const
{
    Q_UNUSED(widget);

    if (theme() == OfficeStyle::Office2013Dark)
    {
        QImage image = pixTitleBackground.toImage();
        QColor color(0, 0, 0);
        QColor clrDest(240, 240, 240);
        QImage imageMask = DrawHelpers::addaptColors(image, color.rgb(), clrDest.rgb(), false);
        imageMask = DrawHelpers::updateImage(imageMask, color.rgb(), clrDest.rgb());
        image.setAlphaChannel(imageMask);
        p->drawImage(rectDst, image, rectSrc);
    }
    else
        p->drawPixmap(rectDst, pixTitleBackground, rectSrc);
}

/*! \internal */
void RibbonPaintManager2013::drawRibbonTabBar(const QStyleOption* opt, QPainter* p, const QWidget* w) const
{
    Q_UNUSED(w);
    QTN_D_STYLE(RibbonStyle)

    if (d.m_clrRibbonTabBarBackground != d.m_clrRibbonFace)
        p->fillRect(opt->rect, d.m_clrRibbonTabBarBackground.isValid() ? d.m_clrRibbonTabBarBackground : d.m_clrRibbonFace);
}

/*! \internal */
void RibbonPaintManager2013::drawRibbonGroups(const QStyleOption* option, QPainter* p, const QWidget* widget) const
{
    Q_UNUSED(widget);
    QTN_D_STYLE(RibbonStyle)
    if (const StyleOptionRibbon* optGroups = qstyleoption_cast<const StyleOptionRibbon*>(option)) 
    {
        p->fillRect(optGroups->rect, m_clrRibbonGroupsFill);

        QRect rectGroups = optGroups->rect;
        QRect rectActiveTab;
        if (const RibbonBar* rb = qobject_cast<const RibbonBar*>(widget))
        {
            QList<RibbonTabBar*> l = widget->findChildren<RibbonTabBar*>();
            if (l.size() > 0)
            {
                RibbonTabBar* tabBar = l[0];
                if (RibbonTab* tab = tabBar->getTab(tabBar->currentIndex()))
                {
                    rectActiveTab = tab->rect();
                    QPoint pntrb(tab->mapTo((QWidget*)rb, rectActiveTab.topLeft()));
                    rectActiveTab.translate(pntrb);
                }
            }
        }

        if (d.m_isActiveTabAccented)
        {
            QPen saveTopPen = p->pen();
            p->setPen(QPen(d.m_clrAccent, d.m_lineWidthTop));

            QPainterPath pathTop;
            pathTop.moveTo(rectGroups.left(), rectGroups.top() + 1);
            pathTop.lineTo(rectActiveTab.left() + 1, rectGroups.top() + 1);
            pathTop.lineTo(rectActiveTab.right() - 2, rectGroups.top() +1 );
            pathTop.lineTo(rectGroups.right() - 1, rectGroups.top() + 1);
            p->drawPath(pathTop);
            p->setPen(saveTopPen);

            QPainterPath path;
            path.moveTo(rectGroups.right() - 1, rectGroups.top() + 1);
            path.lineTo(rectGroups.right() - 1, rectGroups.bottom() - 1);
            path.lineTo(rectGroups.left(), rectGroups.bottom() - 1);
            path.lineTo(rectGroups.left(), rectGroups.top());

            QPen savePen = p->pen();
            p->setPen(d.m_clrRibbonSeparator);
            p->drawPath(path);
            p->setPen(savePen);
        }
        else
        {
            QPainterPath path;
            if (optGroups->tabBarPosition == RibbonBar::TabBarTopPosition)
            {
                path.moveTo(rectGroups.left(), rectGroups.top());
                path.lineTo(rectActiveTab.left() + 1, rectGroups.top());
                path.lineTo(rectActiveTab.right() - 2, rectGroups.top());
                path.lineTo(rectGroups.right() - 1, rectGroups.top());
                path.lineTo(rectGroups.right() - 1, rectGroups.bottom() - 1);
                path.lineTo(rectGroups.left(), rectGroups.bottom() - 1);
                path.lineTo(rectGroups.left(), rectGroups.top());
            }
            else if (optGroups->tabBarPosition == RibbonBar::TabBarBottomPosition)
            {
                path.moveTo(rectGroups.left(), rectGroups.bottom() - 1);
                path.lineTo(rectActiveTab.left(), rectGroups.bottom() - 1);
                path.moveTo(rectActiveTab.right(), rectGroups.bottom() - 1);
                path.lineTo(rectGroups.right() - 1, rectGroups.bottom() - 1);
                path.lineTo(rectGroups.right() - 1, rectGroups.top());
                path.lineTo(rectGroups.left(), rectGroups.top());
                path.lineTo(rectGroups.left(), rectGroups.bottom());
            }
            QPen savePen = p->pen();
            p->setPen(d.m_clrRibbonSeparator);
            p->drawPath(path);
            p->setPen(savePen);
        }
    }
}

/*! \internal */
void RibbonPaintManager2013::drawGroup(const QStyleOption* opt, QPainter* p, const QWidget* widget) const
{
    Q_UNUSED(widget)
    QTN_D_STYLE(RibbonStyle)
    QRect rect = opt->rect;
    QPen savePen = p->pen();
    p->setPen(d.m_clrRibbonSeparator);
    p->drawLine(QPoint(rect.right() - 1, rect.top()), QPoint(rect.right() - 1, rect.bottom()));
    p->setPen(savePen);
}

/*! \internal */
void RibbonPaintManager2013::drawReducedGroup(const QStyleOption* opt, QPainter* p, const QWidget* widget) const
{
    QTN_D_STYLE(RibbonStyle)
    if (const QStyleOptionToolButton* optGroup = qstyleoption_cast<const QStyleOptionToolButton*>(opt))
    {
        QRect rcEntry(opt->rect);
        if (optGroup->features & QStyleOptionToolButton::HasMenu)
            p->fillRect(rcEntry.adjusted(0, 0, -1, 0), m_clrBarShadow);
        else if (opt->state & QStyle::State_MouseOver)
            p->fillRect(rcEntry, DrawHelpers::pixelAlpha(m_clrBarShadow.rgb(), 103));

        QPen savePen = p->pen();
        p->setPen(d.m_clrRibbonSeparator);
        p->drawLine(QPoint(rcEntry.right() - 1, rcEntry.top()), QPoint(rcEntry.right() - 1, rcEntry.bottom()));
        p->setPen(savePen);

        // ----
        QFontMetrics fm(optGroup->fontMetrics);
        int flags = Qt::TextSingleLine;

        flags |= Qt::TextShowMnemonic;
        if (!baseStyle()->proxy()->styleHint(QStyle::SH_UnderlineShortcut, optGroup, widget))
            flags |= Qt::TextHideMnemonic;

        if (!optGroup->icon.isNull())
        {
            QPixmap pm;
            QSize pmSize = optGroup->iconSize;

            QIcon::State stateIcon = optGroup->state & QStyle::State_On ? QIcon::On : QIcon::Off;
            QIcon::Mode mode;
            if (!(optGroup->state & QStyle::State_Enabled))
                mode = QIcon::Disabled;
            else if ((opt->state & QStyle::State_MouseOver) && (opt->state & QStyle::State_AutoRaise))
                mode = QIcon::Active;
            else
                mode = QIcon::Normal;

            pm = optGroup->icon.pixmap(optGroup->rect.size().boundedTo(optGroup->iconSize), mode, stateIcon);
            pmSize = pm.size();

            QRect rectImage = opt->rect;
            if (pmSize.width() < 20)
            {
                rectImage.setTop(rectImage.top() + 10);
                rectImage.setBottom(rectImage.top() + pmSize.height());

                rectImage.setLeft(rectImage.center().x() - pmSize.width() / 2);
                rectImage.setRight(rectImage.left() + pmSize.width());
                rectImage.adjust(-5, -5, 5, 5);
                p->fillRect(rectImage, m_clrBarFace);

                QPen savePen = p->pen();
                p->setPen(d.m_clrMenuPopupSeparator);
                p->drawRect(rectImage);
                p->setPen(savePen);
            }
            baseStyle()->proxy()->drawItemPixmap(p, rectImage, Qt::AlignCenter, pm);
        }

        //===========================================================================================
        QString strFirstRow, strSecondRow;
        CommonStyle::splitString(optGroup->text, strFirstRow, strSecondRow);

        int textHeightTotal = optGroup->rect.height() * 0.5;

        QRect rcText = rcEntry;
        rcText.adjust(4, textHeightTotal - 3, -4, 0);

        QRect rectFirstRow = rcText;
        if (!strFirstRow.isEmpty())
        {
            int textHeight = optGroup->fontMetrics.boundingRect(strFirstRow).height() + optGroup->fontMetrics.descent();
            rectFirstRow.setHeight(textHeight + 3);
            baseStyle()->proxy()->drawItemText(p, rectFirstRow, flags | Qt::AlignHCenter, optGroup->palette, optGroup->state & QStyle::State_Enabled, strFirstRow, QPalette::WindowText);
        }

        QRect rectSecondRow = rectFirstRow;
        if (!strSecondRow.isEmpty())
        {
            rectSecondRow.setTop(rectFirstRow.bottom());
            int textHeight = optGroup->fontMetrics.boundingRect(strSecondRow).height() + optGroup->fontMetrics.descent();
            rectSecondRow.setHeight(textHeight);
            baseStyle()->proxy()->drawItemText(p, rectSecondRow, flags | Qt::AlignLeft, optGroup->palette, optGroup->state & QStyle::State_Enabled, strSecondRow, QPalette::WindowText);
        }
        //===========================================================================================
        OfficePaintManager2013::ImageIcons index = OfficePaintManager2013::Icon_ArowDown;
        OfficePaintManager2013::ImageState state = OfficePaintManager2013::Black2;

        int width = opt->fontMetrics.width(strSecondRow);
        QPoint pt = QPoint(strSecondRow.isEmpty() ? rcText.center().x() - 4 : 
            rectSecondRow.left() + width + 1, strSecondRow.isEmpty() ? rectFirstRow.bottom() : (rectSecondRow.top() + (rectSecondRow.height() - 9)/2) + 1);


        bool enabled  = opt->state & QStyle::State_Enabled;
        if (!enabled)
            state = OfficePaintManager2013::LtGray;
        else if (optGroup->features & QStyleOptionToolButton::HasMenu)
        {
            drawIconByColor(p, index, pt, m_clrHighlightDn);
            return;
        }
        drawIcon(p, pt, index, state, QSize(9, 9));
    }
}

// for TitleBar
/*! \internal */
bool RibbonPaintManager2013::drawToolBar(const QStyleOption* opt, QPainter* p, const QWidget* w) const
{
    if (qstyleoption_cast<const QStyleOptionToolBar*>(opt)) 
    {
        return OfficePaintManager2013::drawToolBar(opt, p, w);
    }
    else if (qstyleoption_cast<const StyleRibbonQuickAccessBar*>(opt)) 
    {
        return true;
    }
    return OfficePaintManager2013::drawToolBar(opt, p, w);
}

/*! \internal */
bool RibbonPaintManager2013::drawIndicatorToolBarSeparator(const QStyleOption* opt, QPainter* p, const QWidget* widget) const
{
    if (qobject_cast<const RibbonGroup*>(widget))
    {
        if (const QStyleOptionGroupBox* optGroup = qstyleoption_cast<const QStyleOptionGroupBox*>(opt)) 
        {
            QTN_D_STYLE(RibbonStyle)
            QRect rect = opt->rect;

            if (optGroup->text.isEmpty())
            {
                QPainterPath path;
                path.moveTo (rect.center().x(), rect.top());
                path.lineTo (rect.center().x(), rect.bottom());

                QPen savePen = p->pen();
                p->setPen(d.m_clrRibbonSeparator);
                p->drawPath(path);
                p->setPen(savePen);
            }
            else
            {
                QString titleText = optGroup->text;
                QRect rectText = optGroup->fontMetrics.boundingRect(titleText);
                if (optGroup->textAlignment == Qt::AlignBottom)
                {
                    QPainterPath path;
                    path.moveTo (rect.center().x(), rect.top());
                    path.lineTo (rect.center().x(), rect.bottom() - rectText.width() - 4);

                    p->save();
                    p->setPen(d.m_clrRibbonSeparator);
                    p->drawPath(path);
                    QSize s = rect.size();
                    s.transpose();
                    rect.setSize(s);
                    p->translate(rect.left() - 1, rect.top() + rect.width());
                    p->rotate(-90);
                    p->translate(-rect.left() + 1, -rect.top());
                    int indent = 0;
                    QPalette pal;
                    pal.setColor(QPalette::WindowText, d.m_clrRibbonSeparator.darker());
                    baseStyle()->proxy()->drawItemText(p, rect.adjusted(indent + 1, 1, -indent - 1, -1), Qt::AlignLeft | Qt::AlignVCenter | Qt::TextShowMnemonic, pal,
                        optGroup->state & QStyle::State_Enabled, titleText, QPalette::WindowText);
                    p->restore();
                }
                else if (optGroup->textAlignment == Qt::AlignTop)
                {
                    QPainterPath path;
                    path.moveTo (rect.center().x(), rect.top() + rectText.width());
                    path.lineTo (rect.center().x(), rect.bottom());

                    p->save();
                    p->setPen(d.m_clrRibbonSeparator);
                    p->drawPath(path);
                    QSize s = rect.size();
                    s.transpose();
                    rect.setSize(s);
                    p->translate(rect.left() - 1, rect.top() + rect.width());
                    p->rotate(-90);
                    p->translate(-rect.left() + 1, -rect.top());
                    int indent = 0;
                    QPalette pal;
                    pal.setColor(QPalette::WindowText, d.m_clrRibbonSeparator.darker());
                    baseStyle()->proxy()->drawItemText(p, rect.adjusted(indent + rect.width() - rectText.width() - 2, 1, -indent - 1, -1), Qt::AlignLeft | Qt::AlignVCenter | Qt::TextShowMnemonic, pal,
                        optGroup->state & QStyle::State_Enabled, titleText, QPalette::WindowText);
                    p->restore();
                }
                else if (optGroup->textAlignment == Qt::AlignVCenter)
                {
                    p->save();
                    p->setPen(d.m_clrRibbonSeparator);
                    p->drawLine(rect.center().x(), rect.top() + rectText.width() + rectText.width() + 4, rect.center().x(), rect.bottom());
                    p->drawLine(rect.center().x(), rect.top(), rect.center().x(), rect.bottom() - (rectText.width() + rectText.width() + 4));

                    QSize s = rect.size();
                    s.transpose();
                    rect.setSize(s);
                    p->translate(rect.left() - 1, rect.top() + rect.width());
                    p->rotate(-90);
                    p->translate(-rect.left() + 1, -rect.top());
                    int indent = 0;
                    QPalette pal;
                    pal.setColor(QPalette::WindowText, d.m_clrRibbonSeparator.darker());
                    baseStyle()->proxy()->drawItemText(p, rect.adjusted(indent + (rect.width() - rectText.width())/2 - 2, 1, -indent - 1, -1), Qt::AlignLeft | Qt::AlignVCenter | Qt::TextShowMnemonic, pal,
                        optGroup->state & QStyle::State_Enabled, titleText, QPalette::WindowText);
                    p->restore();
                }
            }
            return true;
        }
    }
    else if (qobject_cast<const RibbonStatusBar*>(widget))
    {
        return true;
    }
    return false;
}

bool RibbonPaintManager2013::drawPanelButtonTool(const QStyleOption* opt, QPainter* p, const QWidget* w) const 
{ 
    if (!w)
        return false;

    if (w && qobject_cast<const RibbonBackstageButton*>(w))
    {
        drawPanelBackstageButton(opt, p, w);
        return true;
    }

    if (w && !qobject_cast<const RibbonTitleButton*>(w))
    {
        if (const QStyleOptionToolButton* toolbutton = qstyleoption_cast<const QStyleOptionToolButton*>(opt)) 
        {
            bool smallSize = opt->rect.height() < 33;

            RibbonToolBarControl* toolBarControl = qobject_cast<RibbonToolBarControl*>(w->parentWidget());
            if (toolBarControl)
                smallSize = true;

            if (w && qobject_cast<const RibbonButton*>(w))
            {
                if (!opt->rect.isEmpty())
                {
                    int minIcon = qMin(opt->rect.height(), opt->rect.width());
                    int actualArea = minIcon*minIcon;

                    int index = -1;
                    int curArea = 0;
                    QList<QSize> lSz = toolbutton->icon.availableSizes();
                    if (lSz.count() > 1)
                    {
                        for (int i = 0, count = lSz.count(); count > i; i++)
                        {
                            QSize curSz = lSz[i];
                            int area = curSz.height()*curSz.width();
                            if (actualArea > area)
                            {
                                if (area > curArea)
                                    index = i;
                                curArea = area;
                            }
                        }
                        int iconExtent = index != -1 ?  toolbutton->icon.actualSize(lSz[index]).height() : smallSize;
                        smallSize = toolbutton->toolButtonStyle == Qt::ToolButtonTextUnderIcon ? iconExtent < 32 : iconExtent;
                    }
                }
            }

            bool enabled  = opt->state & QStyle::State_Enabled;
            bool checked  = opt->state & QStyle::State_On;
            bool highlighted = opt->state & QStyle::State_MouseOver;
            bool mouseInSplit = opt->state & QStyle::State_MouseOver && toolbutton->activeSubControls & QStyle::SC_ToolButton;
            bool mouseInSplitDropDown = opt->state & QStyle::State_MouseOver && toolbutton->activeSubControls & QStyle::SC_ToolButtonMenu;
            bool pressed  = opt->state & QStyle::State_Sunken;
            bool popuped  = (toolbutton->activeSubControls & QStyle::SC_ToolButtonMenu) && (opt->state & QStyle::State_Sunken);

            if (!(toolbutton->features & QStyleOptionToolButton::MenuButtonPopup))
            {
                bool autoRaise = toolbutton->state & QStyle::State_AutoRaise;
                if (!autoRaise && !qobject_cast<const QStatusBar*>(w->parentWidget()))
                {
                    QStyleOptionButton tmpBtn;// = *btn;
                    tmpBtn.state = toolbutton->state;
                    tmpBtn.rect = toolbutton->rect;
                    baseStyle()->proxy()->drawPrimitive(QStyle::PE_PanelButtonCommand, &tmpBtn, p, w);
                }
                else
                {
                    QColor color;
                    if (!enabled) color = QColor();

                    else if (popuped)
                        color = buttonHiliteColor(w, true/*highlightDn*/);
                    else if (checked && !highlighted && !pressed) 
                        color = buttonHiliteColor(w, true/*highlightDn*/);
                    else if (pressed) 
                        color = buttonHiliteColor(w, true/*highlightDn*/);
                    else if (highlighted) 
                        color = buttonHiliteColor(w, false/*highlight*/);

                    if (color.isValid())
                        p->fillRect(opt->rect, color);

                    if (checked && highlighted)
                    {
                        QColor colorDn = buttonHiliteColor(w, true/*highlightDn*/);
                        DrawHelpers::draw3dRectEx(*p, opt->rect, colorDn, colorDn);
                    }
                }
                return true;
            }

            QRect rcButton = opt->rect;
            QRect popupr = baseStyle()->subControlRect(QStyle::CC_ToolButton, toolbutton, QStyle::SC_ToolButtonMenu, w);

            if (highlighted)
            {
                QPen savePen = p->pen();
                p->setPen(buttonHiliteColor(w, pressed/*highlightDn*/));
                p->drawRect(rcButton.adjusted(0, 0, -1, -1));
                p->setPen(savePen);
            }

            if (mouseInSplit || checked)
            {
                QColor colorSplit;
                if (!enabled)
                {
                    colorSplit = QColor();
                }
                else if (checked)
                {
                    if (popuped) 
                        colorSplit = buttonHiliteColor(w, true/*highlightDn*/);
                    else if (highlighted && !pressed && !checked) 
                        colorSplit = buttonHiliteColor(w, false/*highlight*/);
                    else if (highlighted && pressed) 
                        colorSplit = buttonHiliteColor(w, true/*highlightDn*/);
                    else if (pressed || checked) 
                        colorSplit = buttonHiliteColor(w, true/*highlightDn*/);
                    else if (highlighted || pressed) 
                        colorSplit = !mouseInSplit ? buttonHiliteColor(w, true/*highlightDn*/) : QColor();
                }
                else
                {
                    if (popuped) colorSplit = buttonHiliteColor(w, true/*highlightDn*/);
                    else if (pressed) colorSplit = buttonHiliteColor(w, true/*highlightDn*/);
                    else if (highlighted) colorSplit = buttonHiliteColor(w, false/*highlight*/);
                }
                if ( colorSplit.isValid() )
                {
                    QRect rcSplitUp = smallSize ? QRect(QPoint(rcButton.left(), rcButton.top()), QPoint(rcButton.right() - popupr.width()-2, rcButton.bottom())) 
                        : QRect(QPoint(rcButton.left(), rcButton.top()), QPoint(rcButton.right(), rcButton.bottom() - popupr.height()-2));
                    p->fillRect(rcSplitUp, colorSplit);
                }
            }

            if (mouseInSplitDropDown || popuped || checked)
            {
                if (!popuped)
                {
                    QRect rcSplitDown = smallSize ? QRect(QPoint(rcButton.right() - popupr.width()-1, rcButton.top()), QPoint(rcButton.right(), rcButton.bottom())) :
                        QRect(QPoint(rcButton.left(), rcButton.bottom() - popupr.height()-1), QPoint(rcButton.right(), rcButton.bottom()));

                    QColor colorSplit = buttonHiliteColor(w, pressed/*highlightDn*/);
                    p->fillRect(rcSplitDown, colorSplit);
                }
                else if (popuped || checked)
                    p->fillRect(rcButton, buttonHiliteColor(w, true/*highlightDn*/));
            }
            return true;
        }
    } 
    else if (w && qobject_cast<const RibbonTitleButton*>(w))
    {
        if (const StyleCaptionButton* toolbutton = qstyleoption_cast<const StyleCaptionButton*>(opt)) 
        {
            bool down = toolbutton->state & QStyle::State_Sunken;
            bool highlighted = toolbutton->state & QStyle::State_MouseOver;

            if (down || highlighted)
                p->fillRect(toolbutton->rect, buttonHiliteColor(w, down/*highlightDn*/));

            QRect rectImage = toolbutton->rect;
            QSize sizeImage = QSize(9, 9);
            QPoint ptImage (rectImage.left() + (rectImage.width()  - sizeImage.width())  / 2 + ((rectImage.width()  - sizeImage.width()) % 2), 
                            rectImage.top()  + (rectImage.height() - sizeImage.height()) / 2 + ((rectImage.height() - sizeImage.height()) % 2));

            QColor colText = titleButtonHiliteColor(!(toolbutton->state & QStyle::State_Enabled), w);

            ImageState state = highlighted ? Black2 : Black;
            if (!(toolbutton->state & QStyle::State_Enabled))
                state = Gray;
            else if (colText.isValid())
            {
                if (colText.red() <= 128 || colText.green() <= 128 || colText.blue() <= 128)
                    state = highlighted ? Black2 : Black;
                else
                    state = White;
            }

            ImageIcons index;
            if (toolbutton->activeSubControls == QStyle::SC_TitleBarCloseButton)
                index = Icon_Close;
            else if (toolbutton->activeSubControls == QStyle::SC_TitleBarMaxButton)
                index = Icon_Maximize;
            else if (toolbutton->activeSubControls == QStyle::SC_TitleBarMinButton)
                index = Icon_Minimize;
            else if (toolbutton->activeSubControls == QStyle::SC_TitleBarNormalButton)
                index = Icon_Restore;
            else
            {
                Q_ASSERT(false);
                return false;
            }

            drawIcon(p, ptImage, index, state, sizeImage);

            return true;
        }
    }

    return false; 
}

/*! \internal */
void RibbonPaintManager2013::drawContextHeaders(const QStyleOption* opt, QPainter* p) const
{
    if (const StyleOptionTitleBar* optTitleBar = qstyleoption_cast<const StyleOptionTitleBar*>(opt))
    {
        const QList<ContextHeader*>& listContextHeaders = *optTitleBar->listContextHeaders;
        Q_FOREACH (const ContextHeader* header, listContextHeaders)
        {
            QRect rc = header->rcRect;
            rc.translate(0, 2);
            QColor color = qtn_getColorToRGB(header->color);

            QColor clrDark = DrawHelpers::colorMakeDarker(color.rgb());
            QColor clrLight = DrawHelpers::colorMakePale(color.rgb());

            p->fillRect(rc, clrLight);

            QRect rectTop = header->rcRect;
            rectTop.setBottom(rectTop.top() + 6);

            p->fillRect(rectTop, clrDark);

            rc.setLeft(rc.left() + 4);

            int frameBorder = optTitleBar->frameHelper ? optTitleBar->frameHelper->frameBorder() : 0;

            rc.setTop(rc.top() + (frameBorder - 2));

            QString text = header->strTitle.toUpper();

            QFont saveFont = p->font();
            p->setFont(header->firstTab->font());
            QPen savePen = p->pen();
            p->setPen(DrawHelpers::colorMakeDarker(color.rgb(), .4));
            p->drawText(rc, Qt::AlignCenter | Qt::TextSingleLine, p->fontMetrics().elidedText(text, Qt::ElideRight, rc.width()));
            p->setPen(savePen);
            p->setFont(saveFont);
        }
    }
}

// for Slider
/*! \internal */
bool RibbonPaintManager2013::drawSlider(const QStyleOptionComplex* opt, QPainter* p, const QWidget* w) const
{
    if (!qobject_cast<RibbonSliderPane*>(w->parentWidget()))
        return false;

    if (const QStyleOptionSlider* slider = qstyleoption_cast<const QStyleOptionSlider*>(opt))
    {
        if (slider->orientation == Qt::Horizontal)
        {
            QRect groove = baseStyle()->proxy()->subControlRect(QStyle::CC_Slider, opt, QStyle::SC_SliderGroove, w);
            QRect handle = baseStyle()->proxy()->subControlRect(QStyle::CC_Slider, opt, QStyle::SC_SliderHandle, w);

            QRect rcTrackDest(QPoint(groove.left(), groove.center().y()), QSize(groove.width(), 2));
            rcTrackDest.adjust(3, 0, -2, 0);
            p->fillRect(rcTrackDest, m_clrBarFace);
            DrawHelpers::draw3dRectEx(*p, rcTrackDest, m_clrBarDkShadow, m_clrBarDkShadow);

            if ((opt->subControls & QStyle::SC_SliderHandle)) 
            {
                bool enabled  = opt->state & QStyle::State_Enabled;
                bool selected = slider->activeSubControls & QStyle::SC_SliderHandle && opt->state & QStyle::State_MouseOver;
                bool pressetHt = slider->state & QStyle::State_Sunken;

                QColor clrLine = enabled ? m_clrBarDkShadow : m_clrBarShadow;
                QColor clrFill = m_clrBarFace;

                if (enabled && (pressetHt || selected))
                {
                    clrFill = m_clrAccentLight;
                    clrLine = m_clrHighlightDn;
                }

                handle.adjust(2, 0, -2, -2);
                handle.setLeft(handle.center().x() - 2);
                handle.setRight(handle.center().x() + 2);

                p->fillRect(handle, clrFill);

                QPen savePen = p->pen();
                p->setPen(clrLine);
                p->drawRect(handle);
                p->setPen(savePen);
            }
            return true;
        }
    }
    return false;
}

/*! \internal */
void RibbonPaintManager2013::drawGroupScrollButton(const QStyleOption* option, QPainter* p, const QWidget* widget) const
{
    Q_UNUSED(widget)
    if (const QStyleOptionToolButton* toolbutton = qstyleoption_cast<const QStyleOptionToolButton*>(option)) 
    {
        QTN_D_STYLE(const RibbonStyle)
        QRect rc = toolbutton->rect;

        bool enabled  = toolbutton->state & QStyle::State_Enabled;
        bool highlighted = toolbutton->state & QStyle::State_MouseOver;
        bool pressed = toolbutton->state & QStyle::State_Sunken;


        if (pressed)
            p->fillRect(rc, m_clrHighlightDn);
        else if (pressed || highlighted)
            p->fillRect(rc, d.m_clrHighlight);
        else
            p->fillRect(rc, theme() == OfficeStyle::Office2016Black ? m_clrBarDkShadow : QColor(255, 255, 255));

        QColor clrBorder = !enabled ? m_clrBarShadow : (highlighted || pressed) ? m_clrHighlightDn : m_clrBarDkShadow;

        QPen oldPen = p->pen();
        p->setPen(clrBorder);
        p->drawRect(rc.adjusted(0, -1, -1, -2));
        p->setPen(oldPen);

        ImageIcons iImage = toolbutton->arrowType == Qt::LeftArrow ? OfficePaintManager2013::Icon_ArowLeft : OfficePaintManager2013::Icon_ArowRight;

        OfficePaintManager2013::ImageState imageState = OfficePaintManager2013::Gray;
        if (!enabled)
            imageState = OfficePaintManager2013::LtGray;
        else if (highlighted)
            imageState = OfficePaintManager2013::DkGray;

        QRect rcIndicator(QPoint(rc.x() + (rc.width() - 9)/2, rc.y() + (rc.height() - 9)/2), QSize(9, 9));
        drawIcon(p, rcIndicator, iImage, imageState, QSize(9, 9));
    }
}

// for menuBar
/*! \internal */
bool RibbonPaintManager2013::drawFrameMenu(const QStyleOption* opt, QPainter* p, const QWidget* w) const
{
    QTN_D_STYLE(const RibbonStyle)
    if (qobject_cast<const RibbonSystemPopupBar*>(w) || qobject_cast<const RibbonPageSystemPopup*>(w)) 
    {
        QRect rect = opt->rect;
        DrawHelpers::draw3dRectEx(*p, rect, d.m_clrMenubarBorder, d.m_clrMenubarBorder);
        rect.adjust(1, 1, -1, -1);
        DrawHelpers::draw3dRectEx(*p, rect, d.m_clrBackgroundLight, d.m_clrBackgroundLight);
        QRect rectLeft(1, 1, 2, rect.bottom() - 1);
        p->fillRect(rectLeft, m_clrBarHilite);

        QRect rectFill = opt->rect;
        rectFill.adjust(1, 1, -1, -1);
        p->fillRect(rectFill, m_clrBarLight);
        return true;
    }
    return OfficePaintManager2013::drawFrameMenu(opt, p, w);
}

/*! \internal */
void RibbonPaintManager2013::drawTabShape(const QStyleOption* opt, QPainter* p, const QWidget* w) const
{
    QTN_D_STYLE(RibbonStyle)
    if (const StyleRibbonOptionHeader* optTab = qstyleoption_cast<const StyleRibbonOptionHeader*>(opt)) 
    {
        if (optTab->state & QStyle::State_Selected)
        {
            QRect rectTab = optTab->rect;
            p->fillRect(rectTab, d.m_isActiveTabAccented ? (optTab->state & QStyle::State_MouseOver ? m_clrAccentHilight : d.m_clrAccent) : m_clrRibbonGroupsFill);

            QPainterPath path;
            if (optTab->tabBarPosition == RibbonBar::TabBarTopPosition)
            {
                path.moveTo(rectTab.left(), rectTab.bottom());
                path.lineTo(rectTab.left(), rectTab.top());
                path.lineTo(rectTab.right(), rectTab.top());
                path.lineTo(rectTab.right(), rectTab.bottom());
            }
            else if (optTab->tabBarPosition == RibbonBar::TabBarBottomPosition)
            {
                path.moveTo(rectTab.left(), rectTab.top());
                path.lineTo(rectTab.left(), rectTab.bottom());
                path.lineTo(rectTab.right(), rectTab.bottom());
                path.lineTo(rectTab.right(), rectTab.top());
            }
            else
            {
                return;
            }

            QPen savePen = p->pen();
            p->setPen(d.m_clrRibbonSeparator);
            p->drawPath(path);
            p->setPen(savePen);
        }
        else
            fillRibbonTabControl(optTab, p, w);

        if (optTab->state & QStyle::State_Small)
        {
            QRect rectTab = optTab->rect;
            QPainterPath path;
            path.moveTo(rectTab.right(), rectTab.top());
            path.lineTo(rectTab.right(), rectTab.bottom());
            QPen savePen = p->pen();
            p->setPen(d.m_clrRibbonSeparator);
            p->drawPath(path);
            p->setPen(savePen);
        }
    }
}

/*! \internal */
void RibbonPaintManager2013::drawTabShapeLabel(const QStyleOption* opt, QPainter* p, const QWidget* w) const
{
    QTN_D_STYLE(RibbonStyle)
    if (const QStyleOptionHeader * optTab = qstyleoption_cast<const QStyleOptionHeader*>(opt)) 
    {
        bool selected = opt->state & QStyle::State_Selected;
        bool highlighted = opt->state & QStyle::State_MouseOver;
        int flags = optTab->textAlignment;

        flags |= Qt::TextShowMnemonic;
        if (!baseStyle()->proxy()->styleHint(QStyle::SH_UnderlineShortcut, opt, w))
            flags |= Qt::TextHideMnemonic;

        RibbonPage::ContextColor pageColor = RibbonPage::ContextColorNone;
        if (const RibbonTab* tab = qobject_cast<const RibbonTab*>(w))
            pageColor = tab->contextColor();

        QColor textColor = d.m_clrTabNormalText;
        if (d.m_clrRibbonTabBarText.isValid())
            textColor = d.m_clrRibbonTabBarText;

        if (selected && pageColor != RibbonPage::ContextColorNone)
        {
            textColor = ::qtn_getColorToRGB(pageColor);
            textColor = DrawHelpers::colorMakeDarker(textColor.rgb(), .4);
        }
        else if (selected || highlighted) 
            textColor = d.m_clrTabSelectedText;

        if (d.m_isActiveTabAccented && selected)
            textColor = m_clrRibbonGroupsFill;

        QString text = optTab->text;
        if (d.ribbonPaintManager()->isTopLevelMenuItemUpperCase(w))
            text = text.toUpper();

        // draw text
        QPalette pal = optTab->palette;
        pal.setColor(QPalette::WindowText, textColor);
        baseStyle()->proxy()->drawItemText(p, optTab->rect, flags, pal, opt->state & QStyle::State_Enabled, text, QPalette::WindowText);
    }
}


// for TabBar
/*! \internal */
bool RibbonPaintManager2013::drawTabBarTabShape(const QStyleOption* opt, QPainter* p, const QWidget* w) const
{
    return OfficePaintManager2013::drawTabBarTabShape(opt, p, w);
}

/*! \internal */
bool RibbonPaintManager2013::drawShapedFrame(const QStyleOption* opt, QPainter* p, const QWidget* w) const
{
    return OfficePaintManager2013::drawShapedFrame(opt, p, w);
}

/*! \internal */
void RibbonPaintManager2013::drawSystemButton(const QStyleOption* option, QPainter* p, const QWidget* widget) const
{
    QTN_D_STYLE(const RibbonStyle)
    if (const StyleSystemToolButton* toolbutton = qstyleoption_cast<const StyleSystemToolButton*>(option))
    {
        if (qobject_cast<const RibbonBar*>(widget->parentWidget()))
        {
            if (toolbutton->toolButtonStyle != Qt::ToolButtonFollowStyle)
            {
                bool popuped  = (toolbutton->activeSubControls & QStyle::SC_ToolButtonMenu) && (option->state & QStyle::State_Sunken);
                bool isHighlighted = option->state & QStyle::State_MouseOver;
                bool enabled = widget->isEnabled();

                bool usePalette = false;
                uint resolve_mask = toolbutton->palette.resolve();
                if (resolve_mask & (1 << QPalette::Button)) 
                    usePalette = true;

                if (usePalette)
                {
                    QBrush brButton = toolbutton->palette.brush(QPalette::Active, QPalette::Button);
                    QBrush brButtonDisabled = toolbutton->palette.brush(QPalette::Disabled, QPalette::Button);
                    QBrush brHighlightButton = (resolve_mask & (1 << QPalette::Highlight)) ? toolbutton->palette.brush(QPalette::Active, QPalette::Highlight) : brButton;
                    QBrush brPopupButton = (resolve_mask & (1 << QPalette::Dark)) ? toolbutton->palette.brush(QPalette::Active, QPalette::Dark) : brButton;

                    QBrush brush = !enabled ? brButtonDisabled : popuped ? (isHighlighted ? brHighlightButton : brPopupButton) : 
                        false ? brButton : isHighlighted ? brHighlightButton : brButton;
                    p->fillRect(toolbutton->rect, brush);
                }
                else if (theme() == OfficeStyle::Office2016DarkGray)
                {
                    if (isHighlighted)
                        p->fillRect(toolbutton->rect, m_clrAccentHilight);
                }
                else
                {
                    bool fill = true;
                    if (theme() == OfficeStyle::Office2016Black)
                        fill = isHighlighted;

                    if (fill)
                        p->fillRect(toolbutton->rect, isHighlighted ? m_clrAccentHilight : d.m_clrAccent);
                }

                if (toolbutton->toolButtonStyle == Qt::ToolButtonIconOnly && !toolbutton->icon.isNull())
                {
                    bool enabled  = toolbutton->state & QStyle::State_Enabled;
                    QRect rc = toolbutton->rect;
                    QPixmap pxIcon = toolbutton->icon.pixmap(toolbutton->iconSize, 
                        enabled ? QIcon::Normal : QIcon::Disabled, isHighlighted ? QIcon::On : QIcon::Off);
                    QPoint ptIcon((rc.left() + rc.right() - pxIcon.width()) / 2, (rc.top() + rc.bottom() + 1 - pxIcon.height()) / 2);
                    p->drawPixmap(ptIcon, pxIcon);
                }
                return;
            }
        }

        bool isHighlighted = option->state & QStyle::State_MouseOver;
        if ((toolbutton->activeSubControls & QStyle::SC_ToolButtonMenu) && (toolbutton->state & QStyle::State_Sunken))
            isHighlighted = true;

        p->fillRect(toolbutton->rect.adjusted(0, 0, -1, -1), isHighlighted ? m_clrAccentHilight : d.m_clrAccent);
        if (!toolbutton->icon.isNull())
        {
            QRect rc = toolbutton->rect;
            bool enabled  = toolbutton->state & QStyle::State_Enabled;
            bool selected = toolbutton->state & QStyle::State_MouseOver;
            QPixmap pxIcon = toolbutton->icon.pixmap(toolbutton->iconSize, 
                enabled ? QIcon::Normal : QIcon::Disabled, selected ? QIcon::On : QIcon::Off);
            QPoint ptIcon((rc.left() + rc.right() - pxIcon.width()) / 2, (rc.top() + rc.bottom() - pxIcon.height()) / 2);
            p->drawPixmap(ptIcon.x(), ptIcon.y(), pxIcon);
        }
    }
}

void RibbonPaintManager2013::drawQuickAccessButton(const QStyleOption* opt, QPainter* p, const QWidget* w) const
{
    Q_UNUSED(w);
    fillRibbonButton(opt, p, w);
    ImageIcons index = Icon_CustomizeArowDown;
    QRect rect = opt->rect;
    QRect rectWhite = rect;
    rectWhite.adjust(0, 1, 0, 1);
    drawIcon(p, rectWhite, index, LtGray);
    drawIcon(p, rect, index, Black2);
}

/*! \internal */
void RibbonPaintManager2013::drawOptionButton(const QStyleOption* option, QPainter* p, const QWidget* widget) const
{
    Q_UNUSED(widget);

    fillRibbonButton(option, p, widget);

    bool highlighted = option->state & QStyle::State_MouseOver;
    bool pressed = option->state & QStyle::State_Sunken;
    bool enabled = option->state & QStyle::State_Enabled;
    QRect rect = option->rect;

    OfficePaintManager2013::ImageState state = OfficePaintManager2013::Black2;
    OfficePaintManager2013::ImageIcons index = OfficePaintManager2013::Icon_LaunchArrow;

    if (!enabled)
        state = OfficePaintManager2013::LtGray;
    else if (highlighted && pressed)
    {
        drawIconByColor(p, index, rect, m_clrHighlightDn);
        return;
    }
    drawIcon(p, rect, index, state);
}

/*! \internal */
void RibbonPaintManager2013::drawPopupResizeGripper(const QStyleOption* opt, QPainter* p, const QWidget* w) const
{
    Q_UNUSED(w);

    QTN_D_STYLE(const RibbonStyle)
    if (const QStyleOptionSizeGrip* sizeGrip = qstyleoption_cast<const QStyleOptionSizeGrip*>(opt)) 
    {
        p->fillRect(sizeGrip->rect, d.m_clrBackgroundLight);

        QPoint pt(sizeGrip->rect.right() - 3, sizeGrip->rect.bottom() - 3);
        for (int y = 0; y < 3; y++)
        {
            for (int x = 0; x < 3 - y; x++)
            {
                p->fillRect(QRect(QPoint(pt.x() + 1 - x * 4, pt.y() + 1 - y * 4), QSize(2, 2)), QColor(255, 255, 255));
                p->fillRect(QRect(QPoint(pt.x() + 0 - x * 4, pt.y() + 0 - y * 4), QSize(2, 2)), d.m_clrToolbarSeparator);
            }
        }
    }
}

// for stausBar
/*! \internal */
bool RibbonPaintManager2013::drawPanelStatusBar(const QStyleOption* opt, QPainter* p, const QWidget* w) const
{
    return OfficePaintManager2013::drawPanelStatusBar(opt, p, w);
}

// for SizeGrip
/*! \internal */
bool RibbonPaintManager2013::drawSizeGrip(const QStyleOption* opt, QPainter* p, const QWidget* w) const
{
    return OfficePaintManager2013::drawSizeGrip(opt, p, w);
}

/*! \internal */
bool RibbonPaintManager2013::drawIndicatorArrow(QStyle::PrimitiveElement pe, const QStyleOption* opt, QPainter* p, const QWidget* w) const
{
    if (getParentWidget<RibbonGroup>(w) || getParentWidget<RibbonBar>(w) || qobject_cast<const RibbonBackstageButton*>(w))
    {
        switch(pe)
        {
            case QStyle::PE_IndicatorArrowDown :
                if (qobject_cast<const QToolButton*>(w))
                {
                    if (!qobject_cast<const QTabBar*>(w->parentWidget()))
                    {
                        bool enabled = opt->state & QStyle::State_Enabled;    
                        bool highlighted = opt->state & QStyle::State_Selected;
                        bool pressed = opt->state & QStyle::State_Sunken;

                        OfficePaintManager2013::ImageIcons index = OfficePaintManager2013::Icon_ArowDown;
                        OfficePaintManager2013::ImageState state = OfficePaintManager2013::Black2;

                        if (!enabled)
                            state = OfficePaintManager2013::LtGray;
                        else if (highlighted && pressed)
                        {
                            drawIconByColor(p, index, opt->rect, m_clrHighlightDn);
                            return true;
                        }
                        drawIcon(p, opt->rect, index, state);
                        return true;
                    }
                }
                break;
        default:
            break;
        }
    }

    return OfficePaintManager2013::drawIndicatorArrow(pe, opt, p, w);
}

/*! \internal */
void RibbonPaintManager2013::drawRectangle(QPainter* p, const QRect& rect, bool selected, bool pressed, bool enabled, bool checked, bool popuped,
    Qtitan::BarType barType, Qtitan::BarPosition barPos) const
{
    OfficePaintManager2013::drawRectangle(p, rect, selected, pressed, enabled, checked, popuped, barType, barPos);
}

/*! \internal */
void RibbonPaintManager2013::drawKeyTip(const QStyleOption* opt, QPainter* p, const QWidget* w) const
{
    Q_UNUSED(w);
    QTN_D_STYLE(const RibbonStyle)
    p->fillRect(opt->rect, m_clrBarHilite);
    DrawHelpers::draw3dRectEx(*p, opt->rect, d.m_clrRibbonSeparator, d.m_clrRibbonSeparator);
}

/*! \internal */
void RibbonPaintManager2013::drawBackstage(const QStyleOption* opt, QPainter* p, const QWidget* w) const
{
    Q_UNUSED(w);
    QTN_D_STYLE(const RibbonStyle)
    if (const StyleOptionRibbonBackstage* optBackstage = qstyleoption_cast<const StyleOptionRibbonBackstage*>(opt)) 
    {
        p->fillRect(opt->rect, m_clrBarLight);
        int top = (bool)baseStyle()->proxy()->styleHint((QStyle::StyleHint)RibbonStyle::SH_RibbonBackstageHideTabs) ? 0 : 2;
        QRect rc = QRect(QPoint(0, top), QPoint(optBackstage->menuWidth, opt->rect.bottom()));
        p->fillRect(rc, d.m_clrAccent);
    }
}

/*! \internal */
void RibbonPaintManager2013::drawRibbonBackstageMenu(const QStyleOption* opt, QPainter* p, const QWidget* widget) const
{
    QTN_D_STYLE(const RibbonStyle)
    if (const QStyleOptionMenuItem* menuitem = qstyleoption_cast<const QStyleOptionMenuItem*>(opt)) 
    {
        bool act = menuitem->state & QStyle::State_Selected;
        bool focus = menuitem->state & QStyle::State_HasFocus;
        bool dis = !(menuitem->state & QStyle::State_Enabled);

        QPalette pal = menuitem->palette;

        QColor clrText = dis ? QColor(192, 192, 192) : QColor(255, 255, 255);
        pal.setColor(QPalette::WindowText, clrText);

        QRect rcText(opt->rect);
        rcText.adjust(15, 2, 2, 2);

        if (menuitem->menuItemType == QStyleOptionMenuItem::Separator)
        {
            int x, y, w, h;
            menuitem->rect.getRect(&x, &y, &w, &h);
            // windows always has a check column, regardless whether we have an icon or not
            const int iconSize = baseStyle()->proxy()->pixelMetric(QStyle::PM_ToolBarIconSize, opt, widget);
            int yoff = (y - 1 + h / 2);
            QPen penSave = p->pen();
            OfficeStyle::Theme th = theme();
            QColor clr = th == OfficeStyle::Office2016Black || th == OfficeStyle::Office2016DarkGray ? m_clrBarLight : d.m_clrAccent.light(130);
            p->setPen(clr);
            p->drawLine(x + 2 + iconSize, yoff, x + w - 4, yoff);
            p->setPen(penSave);
            return;
        }
        if (menuitem->menuItemType == QStyleOptionMenuItem::Normal)
        {
            if (act || focus)
                p->fillRect(opt->rect, m_clrAccentText);
        }
        else if (menuitem->menuItemType == QStyleOptionMenuItem::SubMenu)
        {
            if (act || focus)
            {
                p->fillRect(opt->rect, m_clrAccentText);
                if (focus)
                    p->fillRect(opt->rect, m_clrAccentHilight);
            }
        }

        int iconWidth = 0;
        int indent = menuitem->menuItemType == QStyleOptionMenuItem::SubMenu ? 6 : 0;
        if (!menuitem->icon.isNull()) 
        {
            int iconSize = baseStyle()->pixelMetric(QStyle::PM_LargeIconSize, opt, widget);
            if (iconSize > qMin(opt->rect.height(), opt->rect.width()))
                iconSize = baseStyle()->pixelMetric(QStyle::PM_SmallIconSize, opt, widget);

            QIcon::Mode mode = dis ? QIcon::Disabled : QIcon::Normal;
            QPixmap pixmap = menuitem->icon.pixmap(iconSize, mode);
            QPoint pmr(rcText.left() + indent, (rcText.top() + rcText.bottom() - pixmap.height()) / 2);
            p->drawPixmap(pmr, pixmap);
            iconWidth = pixmap.width();
        }

        indent += !menuitem->icon.isNull() ? 7 : 0;
        rcText.setLeft(rcText.left() + iconWidth + indent);

        if (!menuitem->text.isEmpty())
        {
            int flags = 0;
            flags |= Qt::TextHideMnemonic | Qt::AlignVCenter | Qt::TextSingleLine;
            int index = menuitem->text.indexOf(QLatin1String("&"));
            if (index != -1)
            {
                index = menuitem->text.indexOf(QLatin1String(" "), index);
                if (index != -1)
                    flags &= ~Qt::TextHideMnemonic;
            }
            // draw text
            baseStyle()->proxy()->drawItemText(p, rcText, flags, pal, opt->state & QStyle::State_Enabled, menuitem->text, QPalette::WindowText);
        }
    }
}

/*! \internal */
void RibbonPaintManager2013::drawRibbonBackstageCloseButton(const QStyleOption* opt, QPainter* p, const QWidget* w) const
{
    Q_UNUSED(w);

    p->fillRect(opt->rect, backstageCloseButton());

    if (const QStyleOptionToolButton* optButton = qstyleoption_cast<const QStyleOptionToolButton*>(opt)) 
    {
        QList<QSize> sz = optButton->icon.availableSizes();
        if (!optButton->icon.isNull() && sz.count() > 0)
        {
            bool enabled  = optButton->state & QStyle::State_Enabled;
            bool highlighted = optButton->state & QStyle::State_MouseOver;
            QPixmap pxIcon = optButton->icon.pixmap(sz.at(0), enabled ? (highlighted ? QIcon::Selected : QIcon::Active) : QIcon::Disabled, QIcon::On);
            p->drawPixmap(0, 0, pxIcon);
        }
    }
}

/*! \internal */
bool RibbonPaintManager2013::drawRibbonBackstageSeparator(const QStyleOption* opt, QPainter* p, const QWidget* widget) const
{
    Q_UNUSED(widget);
    bool ret = false;
#if (QT_VERSION < QT_VERSION_CHECK(5, 7, 0))
    if (const QStyleOptionFrameV3* f = qstyleoption_cast<const QStyleOptionFrameV3*>(opt))
#else
    if (const QStyleOptionFrame* f = qstyleoption_cast<const QStyleOptionFrame*>(opt))
#endif
    {
        p->fillRect(opt->rect, QColor(255, 255, 255));

        if (f->frameShape == QFrame::HLine)
        {
            QRect rc(QPoint(opt->rect.left(), opt->rect.height() / 2), QSize(opt->rect.width(), 1));
            p->fillRect(rc, QColor(170, 170, 170));
            ret = true;
        }
        else if (f->frameShape == QFrame::VLine)
        {
            QRect rc(QPoint(opt->rect.width()/2, opt->rect.top()), QSize(1, opt->rect.height()));
            p->fillRect(rc, QColor(170, 170, 170));
            ret = true;
        }
    }
    return ret;
}

/*! \internal */
void RibbonPaintManager2013::setupPalette(QWidget* widget) const
{
    if (qobject_cast<RibbonKeyTip*>(widget))
    {
        QPalette palette = widget->palette();
        palette.setColor(QPalette::ToolTipText, m_clrBarText);
        palette.setColor(QPalette::Disabled, QPalette::ToolTipText, m_clrBarShadow);
        widget->setPalette(palette);
    }
}

/*! \internal */
void RibbonPaintManager2013::modifyColors()
{
    OfficePaintManager2013::modifyColors();
    QTN_D_STYLE(RibbonStyle)
    d.m_clrRibbonFace = m_clrBarFace;
    m_clrRibbonGroupsFill = m_clrBarLight;
    d.m_clrRibbonGroupCaptionText = m_clrBarText;
    d.m_clrControlGallerySelected = QColor(255, 255, 255);
    d.m_clrControlGalleryNormal   = QColor(255, 255, 255);
    d.m_clrControlGalleryBorder   = m_clrBarShadow;
}

/*! \internal */
QColor RibbonPaintManager2013::titleButtonHiliteColor(bool disabled, const QWidget* w) const
{
    Q_UNUSED(disabled);
    Q_UNUSED(w);
    return QColor();
}

/*! \internal */
QColor RibbonPaintManager2013::buttonHiliteColor(const QWidget* widget, bool highlightDn) const
{
    Q_UNUSED(widget);
    QTN_D_STYLE(RibbonStyle)
    return highlightDn ? m_clrHighlightDn : d.m_clrHighlight;
}

/*! \internal */
QColor RibbonPaintManager2013::ribbonBarColor(bool isBackstageVisible) const
{
    QTN_D_STYLE(RibbonStyle)
    return isBackstageVisible ? QColor(255, 255, 255) : d.m_clrRibbonFace;
}

/*! \internal */
QColor RibbonPaintManager2013::backstageCloseButton() const
{
    QTN_D_STYLE(RibbonStyle)
    return d.m_clrAccent;
}

/*! \internal */
void RibbonPaintManager2013::drawRibbonSliderButton(const QStyleOption* opt, QPainter* p, const QWidget* w) const
{
    Q_UNUSED(w);
    QTN_D_STYLE(const RibbonStyle)
    bool left = opt->direction != Qt::RightToLeft;

    bool highlighted = opt->state & QStyle::State_MouseOver;
    bool pressed     = opt->state & QStyle::State_Sunken;
    bool enabled     = opt->state & QStyle::State_Enabled;

    QPoint ptCenter = opt->rect.center();
    int radius = 7;
    QRect rectSign(QPoint(ptCenter.x() - radius / 2, ptCenter.y() - radius / 2), QSize(radius, radius));

    QColor clrLine;
    if (!enabled)
        clrLine = m_clrBarShadow;
    else if (pressed && highlighted)
        clrLine = d.m_clrMenubarBorder;
    else if (highlighted)
        clrLine = d.m_clrMenubarBorder;
    else
        clrLine = w && getParentWidget<const QStatusBar>(w) ? QColor(255, 255, 255) : m_clrBarText;


    QPen savePen = p->pen();
    p->setPen(clrLine);

    QPainterPath path;
    path.moveTo(rectSign.left(),  ptCenter.y());
    path.lineTo(rectSign.right(), ptCenter.y());
    if (left)
    {
        path.moveTo(ptCenter.x(), rectSign.top());
        path.lineTo(ptCenter.x(), rectSign.bottom());
    }
    p->drawPath(path);
    p->setPen(savePen);
}

/*! \internal */
bool RibbonPaintManager2013::isTopLevelMenuItemUpperCase(const QWidget* w) const 
{ 
    if (getParentWidget<RibbonTabBar>(w))
        return true; 
    else if (qobject_cast<const RibbonSystemButton*>(w))
        return true; 
    return false;
}

/*! \internal */
void RibbonPaintManager2013::fillRibbonButton(const QStyleOption* option, QPainter* p, const QWidget* w) const
{
    bool popuped  = false;
    if (const QStyleOptionToolButton* toolbutton = qstyleoption_cast<const QStyleOptionToolButton*>(option)) 
        popuped  = (toolbutton->activeSubControls & QStyle::SC_ToolButtonMenu) && (toolbutton->state & QStyle::State_Sunken);

    bool highlighted = option->state & QStyle::State_MouseOver;
    bool pressed = option->state & QStyle::State_Sunken;

    if (pressed || highlighted)
    {
        bool isHighlightDown = (pressed || popuped);
        QColor color = buttonHiliteColor(w, isHighlightDown/*highlightDn*/);
        QRect rectFill = option->rect;
        p->fillRect(rectFill, color);
    }
}

/*! \internal */
void RibbonPaintManager2013::fillRibbonTabControl(const QStyleOption* opt, QPainter* p, const QWidget* widget) const
{
    if (const RibbonTab* tab = qobject_cast<const RibbonTab*>(widget))
    {
        if (ContextHeader* contextHeader = tab->getContextHeader())
        {
            if (!contextHeader || !contextHeader->firstTab || !contextHeader->lastTab || contextHeader->color == RibbonPage::ContextColorNone)
                return;

            QColor color = qtn_getColorToRGB(contextHeader->color);

            QColor clrLight = DrawHelpers::colorMakePale(color.rgb());

            QRect rcFirst(contextHeader->firstTab->rect());
            QRect rcLast(contextHeader->lastTab->rect());

            QRect rc(rcFirst.left(), opt->rect.top(), rcLast.right()+1, rcLast.bottom());
            p->fillRect(rc, clrLight);
        }
    }
}

// for BackstageButton
/*! \internal */
void RibbonPaintManager2013::drawPanelBackstageButton(const QStyleOption* opt, QPainter* p, const QWidget* w) const
{
    Q_UNUSED(w);
    QTN_D_STYLE(RibbonStyle)
    if (const StyleOptionBackstageButton* btnStyle = qstyleoption_cast<const StyleOptionBackstageButton*>(opt)) 
    {
        // QRect rc = btnStyle->rect;
        bool enabled     = opt->state & QStyle::State_Enabled;
        bool checked     = opt->state & QStyle::State_On;
        bool highlighted = opt->state & QStyle::State_MouseOver;
        bool pressed     = opt->state & QStyle::State_Sunken;
        bool popuped     = (btnStyle->activeSubControls & QStyle::SC_ToolButtonMenu) && (opt->state & QStyle::State_Sunken);

        if (btnStyle->flatStyle && !highlighted && !pressed && !checked)
            return;

        QColor color(Qt::white);
        if ( !enabled ) color = QColor();

        else if (popuped) color = m_clrHighlightDn;
        else if (checked && !highlighted && !pressed) color = m_clrHighlightDn;
        else if (pressed) color = m_clrHighlightDn;
        else if (highlighted) color = d.m_clrHighlight;

        if ( color.isValid() )
            p->fillRect(opt->rect, color);

        if (!btnStyle->tabStyle)
        {
            const QPen oldPen = p->pen();
            p->setPen(QColor(170, 170, 170));
            p->drawRect(btnStyle->rect.adjusted(0, 0, -1, -1));
            p->setPen(oldPen);
        }
    }
}


/* RibbonPaintManager2016 */
/*! \internal */
RibbonPaintManager2016::RibbonPaintManager2016(CommonStyle* baseStyle)
    : RibbonPaintManager2013(baseStyle)
    , m_proxy(Q_NULL)
{
    m_proxy = new OfficePaintManager2016(baseStyle);
}

/*! \internal */
RibbonPaintManager2016::~RibbonPaintManager2016()
{
    delete m_proxy;
}

// shared
bool RibbonPaintManager2016::drawPanelTipLabel(const QStyleOption* opt, QPainter* p, const QWidget* wd) const
{
    if (theme() == OfficeStyle::Office2016Black)
    {
        p->fillRect(opt->rect, m_clrBarHilite);
        QPen oldPen = p->pen();
        p->setPen(QColor(98, 98, 98));
        p->drawRect(opt->rect.adjusted(0, 0, -1, -1));
        p->setPen(oldPen);
        return true;
    }
    else
        return RibbonPaintManager2013::drawPanelTipLabel(opt, p, wd);
}

QIcon RibbonPaintManager2016::qtnStandardIcon(QStyle::StandardPixmap px, const QStyleOption* opt, const QWidget* wd, bool& ret ) const
{
    ret = false;
    if (RibbonStyle* bStyle = qobject_cast<RibbonStyle*>(baseStyle()))
    {
        if (QStyle* style = bStyle->defaultStyle())
        {
            if (theme() == OfficeStyle::Office2016Black && wd && qobject_cast<const QDockWidget*>(wd) && 
                (px == QStyle::SP_TitleBarNormalButton || px == QStyle::SP_TitleBarCloseButton))
            {
                QIcon icon = style->standardIcon(px, opt, wd);
                if (!icon.isNull())
                {
                    ret = true;
                    int iconSize = style->pixelMetric(QStyle::PM_SmallIconSize, 0, wd);
                    QPixmap srcPx = icon.pixmap(QSize(iconSize, iconSize));
                    QImage imageMask = DrawHelpers::addaptColors(srcPx.toImage(), qRgb(0,0,0), qRgb(192, 192, 192), false);
                    return QIcon(QPixmap::fromImage(imageMask));
                }
            }
        }
    }
    return QIcon();
}

// for RibbonBar
/*! \internal */
void RibbonPaintManager2016::drawPixTitleBackground(QPainter* p, const QRect& rectDst, const QPixmap& pixTitleBackground, const QRect& rectSrc, const QWidget* widget) const
{
    QTN_D_STYLE(RibbonStyle);
    const RibbonBar* ribbonBar = qobject_cast<const RibbonBar*>(widget);
    Q_ASSERT(ribbonBar != Q_NULL);

    if (theme() == OfficeStyle::Office2016White)
    {
        RibbonPaintManager2013::drawPixTitleBackground(p, rectDst, pixTitleBackground, rectSrc, widget);
        return;
    }

    if (ribbonBar->isBackstageVisible())
    {
        if (theme() == OfficeStyle::Office2016Colorful)
        {
            RibbonPaintManager2013::drawPixTitleBackground(p, rectDst, pixTitleBackground, rectSrc, widget);
            return;
        }
    }
//    else
    {
        if (theme() != OfficeStyle::Office2016Colorful)
        {
            if (theme() == OfficeStyle::Office2016Black)
            {
                QImage image = pixTitleBackground.toImage();
                QColor color(0, 0, 0);
                QColor clrDest(80, 80, 80);
                QImage imageMask = DrawHelpers::addaptColors(image, color.rgb(), clrDest.rgb(), false);
                imageMask = DrawHelpers::updateImage(imageMask, color.rgb(), clrDest.rgb());
                image.setAlphaChannel(imageMask);
                p->drawImage(rectDst, image, rectSrc);
            }
            else
            {
                QImage image = pixTitleBackground.toImage();
                QColor color(0, 0, 0);
                QColor clrDest(120, 120, 120);
                QImage imageMask = DrawHelpers::addaptColors(image, color.rgb(), clrDest.rgb(), false);
                imageMask = DrawHelpers::updateImage(imageMask, color.rgb(), clrDest.rgb());
                image.setAlphaChannel(imageMask);
                p->drawImage(rectDst, image, rectSrc);
            }
            return;
        }
        
        QImage image = pixTitleBackground.toImage();
        QColor color(255, 255, 255);
        QColor clrDest = theme() == OfficeStyle::Office2016Colorful ? DrawHelpers::colorMakeLighter(d.m_clrAccent.rgb(), 0.05) : m_clrBarShadow;
        QImage imageMask = DrawHelpers::addaptColors(image, color.rgb(), clrDest.rgb(), false);
        image.setAlphaChannel(imageMask);
        p->drawImage(rectDst, image, rectSrc);
    }
}

// for TitleBar
/*! \internal */
bool RibbonPaintManager2016::drawToolBar(const QStyleOption* opt, QPainter* p, const QWidget* w) const
{
    if (qstyleoption_cast<const QStyleOptionToolBar*>(opt))
    {
        return OfficePaintManager2013::drawToolBar(opt, p, w);
    }
    else if (qstyleoption_cast<const StyleRibbonQuickAccessBar*>(opt))
    {
        if (const RibbonQuickAccessBar* quickAccessBar = qobject_cast<const RibbonQuickAccessBar*>(w))
        {
            if (const RibbonBar* ribbonBar = qobject_cast<const RibbonBar*>(quickAccessBar->parentWidget()))
            {
                if (ribbonBar->quickAccessBarPosition() == RibbonBar::QABottomPosition)
                    return OfficePaintManager2013::drawToolBar(opt, p, w);
            }
        }
        return true;
    }
    return OfficePaintManager2013::drawToolBar(opt, p, w);
}

// for ribbon tab
/*! \internal */
void RibbonPaintManager2016::fillRibbonTabControl(const QStyleOption* opt, QPainter* p, const QWidget* widget) const
{
    QTN_D_STYLE(RibbonStyle)
    if (const RibbonTab* tab = qobject_cast<const RibbonTab*>(widget))
    {
        if (const StyleRibbonOptionHeader* optTab = qstyleoption_cast<const StyleRibbonOptionHeader*>(opt)) 
        {
            if (tab->getContextHeader())
            {
                if (theme() == OfficeStyle::Office2016Black)
                {
                    bool highlighted = opt->state & QStyle::State_MouseOver;
                    p->fillRect(opt->rect.adjusted(0, 0, 0, -1), highlighted ? d.m_clrHighlight : d.m_clrDockBar);
                }
                else
                    RibbonPaintManager2013::fillRibbonTabControl(opt, p, widget);
            }
            else if (opt->state & QStyle::State_MouseOver && theme() != OfficeStyle::Office2016White)
            {
                QTN_D_STYLE(RibbonStyle);
                if (optTab->tabBarPosition == RibbonBar::TabBarTopPosition)
                    p->fillRect(opt->rect.adjusted(0, 0, 0, -1), theme() == OfficeStyle::Office2016Black ? d.m_clrHighlight : m_clrAccentHilight);
                else if (optTab->tabBarPosition == RibbonBar::TabBarBottomPosition)
                    p->fillRect(opt->rect.adjusted(0, 2, 0, 0), theme() == OfficeStyle::Office2016Black ? d.m_clrHighlight : m_clrAccentHilight);
            }
            else
                RibbonPaintManager2013::fillRibbonTabControl(opt, p, widget);
        }
    }
}

// for editors
/*! \internal */
bool RibbonPaintManager2016::drawComboBox(const QStyleOptionComplex* opt, QPainter* p, const QWidget* w) const
{
    return m_proxy->drawComboBox(opt, p, w);
}

// for menuBar
/*! \internal */
bool RibbonPaintManager2016::drawFrameMenu(const QStyleOption* opt, QPainter* p, const QWidget* w) const
{
    if ((qobject_cast<const RibbonSystemPopupBar*>(w) || qobject_cast<const RibbonPageSystemPopup*>(w)) && theme() != OfficeStyle::Office2016White)
    {
        QRect rect = opt->rect;
        if (theme() == OfficeStyle::Office2016Black)
        {
            p->fillRect(rect, m_clrBarFace);
            DrawHelpers::draw3dRectEx(*p, rect.adjusted(0, 0, -1, -1), m_clrBarDkShadow, m_clrBarDkShadow);
        }
        else
        {
            DrawHelpers::draw3dRectEx(*p, rect, m_clrBarFace, m_clrBarFace);
            rect.adjust(1, 1, -1, -1);
            DrawHelpers::draw3dRectEx(*p, rect, m_clrBarFace, m_clrBarFace);
        }
        return true;
    }
    return RibbonPaintManager2013::drawFrameMenu(opt, p, w);
}

void RibbonPaintManager2016::drawTabShapeLabel(const QStyleOption* opt, QPainter* p, const QWidget* w) const
{
    QTN_D_STYLE(RibbonStyle)
    if (theme() == OfficeStyle::Office2016White)
    {
        RibbonPaintManager2013::drawTabShapeLabel(opt, p, w);
        return;
    }

    if (const StyleRibbonOptionHeader* optTab = qstyleoption_cast<const StyleRibbonOptionHeader*>(opt))
    {
        bool selected = opt->state & QStyle::State_Selected;
        bool highlighted = opt->state & QStyle::State_MouseOver;
        int flags = optTab->textAlignment;

        flags |= Qt::TextShowMnemonic;
        if (!baseStyle()->proxy()->styleHint(QStyle::SH_UnderlineShortcut, opt, w))
            flags |= Qt::TextHideMnemonic;

        RibbonPage::ContextColor pageColor = RibbonPage::ContextColorNone;
        if (const RibbonTab* tab = qobject_cast<const RibbonTab*>(w))
            pageColor = tab->contextColor();

        QColor textColor = d.m_clrRibbonTabBarText;

        if (pageColor != RibbonPage::ContextColorNone)
        {
            if (selected)
            {
                if (theme() == OfficeStyle::Office2016Black)
                    textColor = QColor(Qt::white);
                else
                {
                    textColor = ::qtn_getColorToRGB(pageColor);
                    textColor = DrawHelpers::colorMakeDarker(textColor.rgb(), .4);
                }
            }
            else
            {
                if (!optTab->contextText.isEmpty())
                {
                    if (theme() == OfficeStyle::Office2016Black)
                        textColor = DrawHelpers::colorMakeDarker(m_clrBarText.rgba(), 0.3);
                    else
                        textColor = highlighted ? m_clrAccentText : m_clrBarText;
                }
            }
        }
        else if (selected) 
            textColor = d.m_clrTabSelectedText;
        else if (highlighted) 
            textColor = theme() == OfficeStyle::Office2016Colorful ? m_clrAccentLight : QColor(Qt::white);
        else if (theme() == OfficeStyle::Office2016Black)
            textColor = DrawHelpers::colorMakeDarker(m_clrBarText.rgba(), 0.3);

        if (d.m_isActiveTabAccented && selected)
            textColor = m_clrRibbonGroupsFill;

        QString text = optTab->text;
        if (d.ribbonPaintManager()->isTopLevelMenuItemUpperCase(w))
            text = text.toUpper();

        // draw text
        QPalette pal = optTab->palette;
        pal.setColor(QPalette::WindowText, textColor);
        baseStyle()->proxy()->drawItemText(p, optTab->rect, flags, pal, opt->state & QStyle::State_Enabled, text, QPalette::WindowText);
    }
}

/*! \internal */
void RibbonPaintManager2016::drawContextHeaders(const QStyleOption* opt, QPainter* p) const
{
    QTN_D_STYLE(RibbonStyle)
    if (theme() == OfficeStyle::Office2016Black)
    {
        if (const StyleOptionTitleBar* optTitleBar = qstyleoption_cast<const StyleOptionTitleBar*>(opt))
        {
            const QList<ContextHeader*>& listContextHeaders = *optTitleBar->listContextHeaders;
            Q_FOREACH (const ContextHeader* header, listContextHeaders)
            {
                QRect rc = header->rcRect;
                rc.translate(0, 4);
                QColor color = qtn_getColorToRGB(header->color);

                QColor clrDark = DrawHelpers::colorMakeDarker(color.rgb());
                QColor clrLight = d.m_clrDockBar;

                p->fillRect(rc, clrLight);

                QRect rectTop = header->rcRect;
                rectTop.setBottom(rectTop.top() + 2);

                p->fillRect(rectTop, clrDark);

                rc.setLeft(rc.left() + 4);

                int frameBorder = optTitleBar->frameHelper ? optTitleBar->frameHelper->frameBorder() : 0;

                rc.setTop(rc.top() + (frameBorder - 2));

                QFont saveFont = p->font();
                p->setFont(header->firstTab->font());
                QPen savePen = p->pen();
                p->setPen(DrawHelpers::colorMakeDarker(color.rgb(), .1));
                rc.translate(0, -6);
                p->drawText(rc, Qt::AlignCenter | Qt::TextSingleLine, p->fontMetrics().elidedText(header->strTitle, Qt::ElideRight, rc.width()));
                p->setPen(savePen);
                p->setFont(saveFont);
            }
        }
    }
    else
        RibbonPaintManager2013::drawContextHeaders(opt, p);
}

// for SizeGrip
/*! \internal */
bool RibbonPaintManager2016::drawSizeGrip(const QStyleOption* opt, QPainter* p, const QWidget* w) const
{
    return m_proxy->drawSizeGrip(opt, p, w);
}

/*! \internal */
void RibbonPaintManager2016::drawPopupResizeGripper(const QStyleOption* opt, QPainter* p, const QWidget* w) const
{
    Q_UNUSED(w);

    QTN_D_STYLE(const RibbonStyle)
    if (const QStyleOptionSizeGrip* sizeGrip = qstyleoption_cast<const QStyleOptionSizeGrip*>(opt)) 
    {
        p->fillRect(sizeGrip->rect, d.m_clrBackgroundLight);

        QPoint pt(sizeGrip->rect.right() - 3, sizeGrip->rect.bottom() - 3);
        for (int y = 0; y < 3; y++)
        {
            for (int x = 0; x < 3 - y; x++)
            {
//                p->fillRect(QRect(QPoint(pt.x() + 1 - x * 4, pt.y() + 1 - y * 4), QSize(2, 2)), QColor(255, 255, 255));
                p->fillRect(QRect(QPoint(pt.x() + 0 - x * 4, pt.y() + 0 - y * 4), QSize(2, 2)), d.m_clrToolbarSeparator);
            }
        }
    }
}

// for file menu
void RibbonPaintManager2016::drawQuickAccessButton(const QStyleOption* opt, QPainter* p, const QWidget* w) const
{
    if (theme() == OfficeStyle::Office2016White)
    {
        RibbonPaintManager2013::drawQuickAccessButton(opt, p, w);
        return;
    }

    fillRibbonButton(opt, p, w);
    ImageIcons index = Icon_CustomizeArowDown;
    QRect rect = opt->rect;
    QRect rectWhite = rect;
    rectWhite.adjust(0, 1, 0, 1);

    ImageState state = White;
    if (const RibbonQuickAccessBar* quickAccessBar = qobject_cast<const RibbonQuickAccessBar*>(w->parentWidget()))
    {
        if (const RibbonBar* ribbonBar = qobject_cast<const RibbonBar*>(quickAccessBar->parentWidget()))
        {
            if (ribbonBar->quickAccessBarPosition() == RibbonBar::QABottomPosition)
                state = Black;
        }
    }
    drawIcon(p, rectWhite, index, state);
}

/*! \internal */
bool RibbonPaintManager2016::drawIndicatorArrow(QStyle::PrimitiveElement pe, const QStyleOption* opt, QPainter* p, const QWidget* w) const
{
    if (theme() == OfficeStyle::Office2016White)
        return RibbonPaintManager2013::drawIndicatorArrow(pe, opt, p, w);

    if (qobject_cast<const RibbonQuickAccessBar*>(w->parentWidget()) || qobject_cast<const RibbonTabBar*>(w->parentWidget()))
    {
        switch (pe)
        {
            case QStyle::PE_IndicatorArrowDown:
                if (qobject_cast<const QToolButton*>(w))
                {
                    if (!qobject_cast<const QTabBar*>(w->parentWidget()))
                    {
                        bool enabled = opt->state & QStyle::State_Enabled;
                        bool highlighted = opt->state & QStyle::State_Selected;
                        bool pressed = opt->state & QStyle::State_Sunken;

                        OfficePaintManager2013::ImageIcons index = OfficePaintManager2013::Icon_ArowDown;
                        OfficePaintManager2013::ImageState state = OfficePaintManager2013::White;

                        if (!enabled)
                            state = OfficePaintManager2013::LtGray;
                        else if (highlighted && pressed)
                        {
                            drawIconByColor(p, index, opt->rect, m_clrHighlightDn);
                            return true;
                        }
                        drawIcon(p, opt->rect, index, state);
                        return true;
                    }
                }
                break;
            default:
                break;
        }
    }
    return RibbonPaintManager2013::drawIndicatorArrow(pe, opt, p, w);
}

/*! \internal */
void RibbonPaintManager2016::drawKeyTip(const QStyleOption* opt, QPainter* p, const QWidget* w) const
{
    Q_UNUSED(w);
    QTN_D_STYLE(const RibbonStyle)
    if (theme() == OfficeStyle::Office2016Black)
        p->fillRect(opt->rect, qRgb(255, 214, 51));
    else
        p->fillRect(opt->rect, m_clrBarHilite);
    DrawHelpers::draw3dRectEx(*p, opt->rect, d.m_clrRibbonSeparator, d.m_clrRibbonSeparator);
}

/*! \internal */
void RibbonPaintManager2016::drawRibbonSliderButton(const QStyleOption* opt, QPainter* p, const QWidget* w) const
{
    Q_UNUSED(w);
    bool left = opt->direction != Qt::RightToLeft;

    bool highlighted = opt->state & QStyle::State_MouseOver;
    bool pressed = opt->state & QStyle::State_Sunken;
    bool enabled = opt->state & QStyle::State_Enabled;

    QPoint ptCenter = opt->rect.center();
    int radius = 7;
    QRect rectSign(QPoint(ptCenter.x() - radius / 2, ptCenter.y() - radius / 2), QSize(radius, radius));

    QColor clrLine;
    if (!enabled)
        clrLine = m_clrBarShadow;
    else if (pressed && highlighted)
        clrLine = m_clrHighlightDnNC;
    else if (highlighted)
        clrLine = m_clrHighlightDnNC;
    else
        clrLine = w && getParentWidget<const QStatusBar>(w) ? m_clrBarDkShadow : m_clrBarText;


    QPen savePen = p->pen();
    p->setPen(clrLine);

    QPainterPath path;
    path.moveTo(rectSign.left(), ptCenter.y());
    path.lineTo(rectSign.right(), ptCenter.y());
    if (left)
    {
        path.moveTo(ptCenter.x(), rectSign.top());
        path.lineTo(ptCenter.x(), rectSign.bottom());
    }
    p->drawPath(path);
    p->setPen(savePen);
}

/*! \internal */
void RibbonPaintManager2016::drawBackstage(const QStyleOption* opt, QPainter* p, const QWidget* widget) const
{
    QTN_D_STYLE(const RibbonStyle)
    if (theme() == OfficeStyle::Office2016White)
    {
        RibbonPaintManager2013::drawBackstage(opt, p, widget);
        return;
    }

    if (const StyleOptionRibbonBackstage* optBackstage = qstyleoption_cast<const StyleOptionRibbonBackstage*>(opt))
    {
        int top = (bool)baseStyle()->proxy()->styleHint((QStyle::StyleHint)RibbonStyle::SH_RibbonBackstageHideTabs) ? 0 : 2;
        QRect rc = QRect(QPoint(0, top), QPoint(optBackstage->menuWidth, opt->rect.bottom()));
        if (theme() == OfficeStyle::Office2016Black)
            p->fillRect(rc, d.m_clrRibbonFace);
        else
            p->fillRect(rc, theme() == OfficeStyle::Office2016Colorful ? d.m_clrAccent : m_clrBarText);
    }
}

/*! \internal */
bool RibbonPaintManager2016::drawRibbonBackstageSeparator(const QStyleOption* opt, QPainter* p, const QWidget* widget) const
{
    QTN_D_STYLE(RibbonStyle);
    if (theme() == OfficeStyle::Office2016White)
        return RibbonPaintManager2013::drawRibbonBackstageSeparator(opt, p, widget);

    bool ret = false;
    #if (QT_VERSION < QT_VERSION_CHECK(5, 7, 0))
    if (const QStyleOptionFrameV3* f = qstyleoption_cast<const QStyleOptionFrameV3*>(opt))
    #else
    if (const QStyleOptionFrame* f = qstyleoption_cast<const QStyleOptionFrame*>(opt))
    #endif
    {
        QColor separatorColor = theme() == OfficeStyle::Office2016Colorful ? d.m_clrRibbonSeparator : m_clrBarLight;
        if (f->frameShape == QFrame::HLine)
        {
            QRect rc(QPoint(opt->rect.left(), opt->rect.height() * 0.5), QSize(opt->rect.width(), 1));
            p->fillRect(rc, separatorColor);
            ret = true;
        }
        else if (f->frameShape == QFrame::VLine)
        {
            QRect rc(QPoint(opt->rect.width() * 0.5, opt->rect.top()), QSize(1, opt->rect.height()));
            p->fillRect(rc, separatorColor);
            ret = true;
        }
    }
    return ret;
}

/*! \internal */
void RibbonPaintManager2016::setupPalette(QWidget* widget) const
{
    RibbonPaintManager2013::setupPalette(widget);
    QTN_D_STYLE(RibbonStyle);

    OfficeStyle::Theme themeId = theme();
    if (qobject_cast<QToolButton*>(widget) && (qobject_cast<RibbonTabBar*>(widget->parentWidget()) || qobject_cast<RibbonQuickAccessBar*>(widget->parentWidget())))
    {
        QPalette palette = widget->palette();
        palette.setColor(QPalette::HighlightedText, d.m_clrRibbonText);
        widget->setPalette(palette);
    }
    else if (qobject_cast<RibbonBackstagePage*>(widget))
    { 
        if (themeId == OfficeStyle::Office2016Black)
        {
            QPalette palette = widget->palette();
            palette.setColor(QPalette::Light, d.m_clrDockBar);
            palette.setColor(QPalette::WindowText, m_clrBarText);
            widget->setPalette(palette);
        }
        else
        {
            QPalette palette = widget->palette();
            palette.setColor(QPalette::Light, themeId == OfficeStyle::Office2016Colorful ? m_clrBarLight : d.m_clrDockBar);
            if (themeId == OfficeStyle::Office2016DarkGray)
                palette.setColor(QPalette::WindowText, QColor(Qt::white));
            widget->setPalette(palette);
        }
    }

    if (themeId == OfficeStyle::Office2016Black)
    {
        if (widget->inherits("QComboBoxListView"))
        {
            QPalette palette = widget->palette();
            palette.setColor(QPalette::Background, QColor(67, 67, 67));
            widget->setPalette(palette);
        }
        else if (qobject_cast<QLineEdit*>(widget) || qobject_cast<QComboBox*>(widget))
        {
            QPalette palette = widget->palette();
            palette.setColor(QPalette::Text, Qt::white);
            palette.setColor(QPalette::Base, QColor(67, 67, 67));
            widget->setPalette(palette);
        } 
        else if (qobject_cast<RibbonToolTip*>(widget))
        {
            QPalette palette = QToolTip::palette();
            palette.setColor(QPalette::All, QPalette::ToolTipText, d.m_clrRibbonText);
            palette.setColor(QPalette::Disabled, QPalette::ToolTipText, d.m_clrRibbonText);
            widget->setPalette(palette);
        }
        else if (qobject_cast<QMenu*>(widget))
        {
            QPalette palette = widget->palette();
            palette.setColor(QPalette::ButtonText, m_clrBarText);
            palette.setColor(QPalette::Button, d.m_clrBackgroundLight);
            widget->setPalette(palette);
        }
        else if (qobject_cast<RibbonBackstageButton*>(widget))
        {
            QPalette palette = widget->palette();
            palette.setColor(QPalette::ButtonText, m_clrBarText);
            widget->setPalette(palette);
        }
        else if (qobject_cast<RibbonToolBarControl*>(widget))
        {
            QPalette palette = widget->palette();
            palette.setBrush(QPalette::Background, helper().getColor("Window", "BarLight").light(114));
            widget->setPalette(palette);
        }
        else if (qobject_cast<QGroupBox*>(widget))
        {
            QPalette palette = widget->palette();
            palette.setBrush(QPalette::Dark, QColor(37,37,37));
            palette.setBrush(QPalette::Light, helper().getColor("Window", "BarLight"));
            palette.setColor(QPalette::Foreground, helper().getColor("Window", "BarText"));
            widget->setPalette(palette);
        }
        else if (qobject_cast<QMessageBox*>(widget))
        {
            QPalette palette = widget->palette();
            palette.setColor(QPalette::Foreground, Qt::black);
            widget->setPalette(palette);
        }
        if (qobject_cast<QTextEdit*>(widget))
        {
            QPalette palette = widget->palette();
            palette.setColor(QPalette::Base, QColor(54, 54, 54));
            palette.setColor(QPalette::Text, QColor(255, 255, 255));
            widget->setPalette(palette);
        }
        else if (qobject_cast<RibbonKeyTip*>(widget))
        {
            QPalette palette = widget->palette();
            palette.setColor(QPalette::ToolTipText, Qt::black);
            widget->setPalette(palette);
        }
        else if (qobject_cast<QCheckBox*>(widget))
        {
            QColor color = helper().getColor("Window", "BarText");
            if (color.isValid())
            {
                QPalette palette = widget->palette();
                palette.setBrush(QPalette::ButtonText, color);
                palette.setBrush(QPalette::Disabled, QPalette::ButtonText, color.dark());
                widget->setPalette(palette);
            }
        }
    }
    else if (themeId == OfficeStyle::Office2016DarkGray)
    {
        if (qobject_cast<QGroupBox*>(widget))
        {
            QPalette palette = widget->palette();
            palette.setBrush(QPalette::Dark, d.m_clrDockBar);
            palette.setBrush(QPalette::Light, helper().getColor("Window", "BarLight"));
            palette.setColor(QPalette::Foreground, QColor(240, 240, 240));
            widget->setPalette(palette);
        }
        else if (qobject_cast<QCheckBox*>(widget))
        {
            if (!qobject_cast<RibbonControl*>(widget->parentWidget()))
            {
                QColor color = helper().getColor("Dialog", "Foreground");
                QPalette palette = widget->palette();
                palette.setBrush(QPalette::ButtonText, color);
                palette.setBrush(QPalette::Disabled, QPalette::ButtonText, color.dark(150));
                widget->setPalette(palette);
            }
        }
    }
}

/*! \internal */
void RibbonPaintManager2016::modifyColors()
{
    RibbonPaintManager2013::modifyColors();
    m_proxy->modifyColors();

    QTN_D_STYLE(RibbonStyle);
    if (theme() == OfficeStyle::Office2016Colorful)
    {
        d.m_clrRibbonFace = d.m_clrAccent;
        d.m_clrRibbonTabBarBackground = d.m_clrAccent;

        d.m_clrFrameCaptionTextInActive = m_clrBarLight;
        d.m_clrFrameCaptionTextActive = m_clrBarHilite;

        d.m_clrHighlightNC = m_clrAccentHilight;
        m_clrHighlightDnNC = DrawHelpers::colorMakeDarker(d.m_clrAccent.rgb());

        d.m_clrRibbonTabBarText = QColor(255, 255, 255);
        d.m_clrStatusBarShadow = m_clrBarLight;

        d.m_clrHighlight = DrawHelpers::colorMakeDarker(m_clrBarFace.rgb(), 0.025);
        m_clrHighlightMenuItem = d.m_clrHighlight;
        m_clrHighlightDn = DrawHelpers::colorMakeDarker(d.m_clrHighlight.rgb(), 0.12);
        d.m_clrStatusBarText = m_clrBarText;
        d.m_clrDockBar = m_clrBarFace;
        m_ImagesBlack = mapTo3dColors(m_ImagesSrc, QColor(0, 0, 0), QColor(119, 119, 119));
    }
    else if (theme() == OfficeStyle::Office2016DarkGray)
    {
        m_clrAccentLight = DrawHelpers::colorMakeDarker(d.m_clrAccent.rgb(), .75);
        m_clrTextDisabled = DrawHelpers::colorMakeDarker(m_clrBarShadow.rgb(), 1.1);

        d.m_clrRibbonFace = m_clrBarShadow;
        d.m_clrRibbonTabBarBackground = m_clrBarShadow;
        m_clrRibbonGroupsFill = QColor(178, 178, 178);

        d.m_clrFrameCaptionTextInActive = m_clrBarLight;
        d.m_clrFrameCaptionTextActive = m_clrBarHilite;

        d.m_clrHighlightNC = QColor(100, 100, 100);
        d.m_clrBackgroundLight = QColor(240, 240, 240); 
        m_clrHighlightDnNC = m_clrBarDkShadow;

        d.m_clrRibbonTabBarText = QColor(255, 255, 255);
        d.m_clrStatusBarShadow = DrawHelpers::colorMakeDarker(d.m_clrHighlightNC.rgb(), .15);
        d.m_clrTabSelectedText = QColor(Qt::black);

        d.m_clrHighlight = DrawHelpers::colorMakeDarker(m_clrBarFace.rgb(), 0.12);
        m_clrHighlightMenuItem = QColor(205, 205, 205);
        m_clrHighlightDn = DrawHelpers::colorMakeDarker(d.m_clrHighlight.rgb(), 0.12);
        d.m_clrStatusBarText = QColor(Qt::white);
        d.m_clrDockBar = QColor(106, 106, 106);
    }
    else if (theme() == OfficeStyle::Office2016Black)
    {
        d.m_clrControlGallerySelected = QColor(48, 48, 48);
        d.m_clrControlGalleryNormal   = QColor(48, 48, 48);
        d.m_clrControlGalleryMenuBk   = QColor(48, 48, 48);
        d.m_clrControlGalleryBorder   = m_clrBarLight;
        d.m_clrToolbarSeparator       = QColor(106, 106, 106);

        d.m_clrFrameBorderActive0     = m_clrBarShadow;
        d.m_clrFrameBorderInactive0   = QColor(131, 131, 131);

        d.m_clrRibbonFace = QColor(10, 10, 10);
        d.m_clrRibbonTabBarBackground = d.m_clrRibbonFace;
        m_clrTextDisabled = DrawHelpers::colorMakeDarker(m_clrBarShadow.rgb(), 1.1);
        d.m_clrEditCtrlBorder = QColor(106, 106, 106);
        m_clrRibbonGroupsFill = m_clrBarFace;

        d.m_clrFrameCaptionTextInActive = m_clrBarLight;
        d.m_clrFrameCaptionTextActive = QColor(255, 255, 255);

        d.m_clrHighlightNC = QColor(100, 100, 100);
        d.m_clrBackgroundLight = m_clrRibbonGroupsFill;
        m_clrHighlightDnNC = QColor(70, 70, 70);

        d.m_clrRibbonTabBarText = QColor(255, 255, 255);
        d.m_clrStatusBarShadow = DrawHelpers::colorMakeDarker(d.m_clrHighlightNC.rgb(), .15);
        d.m_clrTabSelectedText = QColor(Qt::white);

        m_clrHighlightMenuItem = QColor(87, 87, 87);
        d.m_clrHighlight = QColor(64, 64, 64);
        m_clrHighlightDn = QColor(87, 87, 87);
        d.m_clrStatusBarText = QColor(Qt::white);
        d.m_clrDockBar = qRgb(37, 37, 37);

        m_ImagesDkGray = mapTo3dColors(m_ImagesSrc, QColor(0, 0, 0), QColor(128, 128, 128));
        m_ImagesBlack  = mapTo3dColors(m_ImagesSrc, QColor(0, 0, 0), QColor(192, 192, 192));
        m_ImagesBlack2  = mapTo3dColors(m_ImagesSrc, QColor(0, 0, 0), QColor(128, 128, 128));
        m_ImagesLtGray = mapTo3dColors(m_ImagesSrc, QColor(0, 0, 0), qRgb(153, 153, 153));
    }
}

QColor RibbonPaintManager2016::titleButtonHiliteColor(bool disabled, const QWidget* w) const
{
    if (theme() == OfficeStyle::Office2016White)
        return RibbonPaintManager2013::titleButtonHiliteColor(disabled, w);

    const RibbonBar* ribbonBar = qobject_cast<const RibbonBar*>(w->parentWidget());
    Q_ASSERT(ribbonBar != Q_NULL);
    if (ribbonBar->isBackstageVisible())
        return theme() == OfficeStyle::Office2016Colorful ? QColor(0, 0, 0) : QColor(255, 255, 255);
    else
        return disabled ? m_clrHighlightDn : m_clrBarHilite;
    return QColor();
}

/*! \internal */
QColor RibbonPaintManager2016::buttonHiliteColor(const QWidget* widget, bool highlightDn) const
{
    QTN_D_STYLE(RibbonStyle);

    if (qobject_cast<const QToolButton*>(widget))
    {
        if (const RibbonQuickAccessBar* quickAccessBar = qobject_cast<const RibbonQuickAccessBar*>(widget->parentWidget()))
        {
            if (const RibbonBar* ribbonBar = qobject_cast<const RibbonBar*>(quickAccessBar->parentWidget()))
            {
                if (ribbonBar->quickAccessBarPosition() == RibbonBar::QABottomPosition)
                    return RibbonPaintManager2013::buttonHiliteColor(widget, highlightDn);
            }
        }
    }

    if (qobject_cast<const RibbonTitleButton*>(widget))
    {
        if (const RibbonBar* ribbonBar = qobject_cast<const RibbonBar*>(widget->parentWidget()))
        {
            if (ribbonBar->isBackstageVisible() && theme() != OfficeStyle::Office2016DarkGray)
                return RibbonPaintManager2013::buttonHiliteColor(widget, highlightDn);
            else
                return highlightDn ? m_clrHighlightDnNC : d.m_clrHighlightNC;
        }
    }

    if (qobject_cast<const RibbonQuickAccessBar*>(widget->parentWidget()) || qobject_cast<const RibbonTabBar*>(widget->parentWidget()))
        return highlightDn ? m_clrHighlightDnNC : d.m_clrHighlightNC;
    else
        return RibbonPaintManager2013::buttonHiliteColor(widget, highlightDn);
}

/*! \internal */
QColor RibbonPaintManager2016::ribbonBarColor(bool isBackstageVisible) const
{
    QTN_D_STYLE(RibbonStyle)
    return isBackstageVisible ? (theme() == OfficeStyle::Office2016Colorful ? m_clrBarLight : d.m_clrDockBar) : d.m_clrRibbonFace;
}

/*! \internal */
QColor RibbonPaintManager2016::backstageCloseButton() const
{
    QTN_D_STYLE(RibbonStyle)
    if (theme() == OfficeStyle::Office2016White)
        return RibbonPaintManager2013::backstageCloseButton();
    else if (theme() == OfficeStyle::Office2016Colorful)
    {
        return d.m_clrAccent;
    }
    else if (theme() == OfficeStyle::Office2016Black)
    {
        return d.m_clrRibbonFace;
    }
    else
        return m_clrBarText;
}

/*! \internal */
QColor RibbonPaintManager2016::menuLabelColor() const
{
    return m_proxy->menuLabelColor();
}

/*! \internal */
bool RibbonPaintManager2016::isTopLevelMenuItemUpperCase(const QWidget* w) const
{
    Q_UNUSED(w);
    return false;
}

// for BackstageButton
/*! \internal */
void RibbonPaintManager2016::drawPanelBackstageButton(const QStyleOption* opt, QPainter* p, const QWidget* w) const
{
    if (theme() == OfficeStyle::Office2016White)
    {
        RibbonPaintManager2013::drawPanelBackstageButton(opt, p, w);
        return;
    }

    QTN_D_STYLE(RibbonStyle)
    if (const StyleOptionBackstageButton* btnStyle = qstyleoption_cast<const StyleOptionBackstageButton*>(opt))
    {
        bool enabled = opt->state & QStyle::State_Enabled;
        bool checked = opt->state & QStyle::State_On;
        bool highlighted = opt->state & QStyle::State_MouseOver;
        bool pressed = opt->state & QStyle::State_Sunken;
        bool popuped = (btnStyle->activeSubControls & QStyle::SC_ToolButtonMenu) && (opt->state & QStyle::State_Sunken);

        if (btnStyle->flatStyle && !highlighted && !pressed && !checked)
            return;

        QColor color(m_clrBarHilite);

        if (!enabled) color = QColor();
        else if (popuped) color = m_clrHighlightDn;
        else if (checked && !highlighted && !pressed) color = m_clrHighlightDn;
        else if (pressed) color = m_clrHighlightDn;
        else if (highlighted) color = d.m_clrHighlight;
        
        if (color.isValid())
            p->fillRect(opt->rect, color);

        if (!btnStyle->tabStyle)
        {
            const QPen oldPen = p->pen();
            p->setPen(m_clrBarDkShadow);
            p->drawRect(btnStyle->rect.adjusted(0, 0, -1, -1));
            p->setPen(oldPen);
        }
    }
}
