/****************************************************************************
**
** Qtitan Library by Developer Machines (Microsoft-Ribbon implementation for Qt.C++)
** 
** Copyright (c) 2009-2013 Developer Machines (http://www.devmachines.com)
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
#include <QPainter>

#include "QtnPopupHelpers.h"
#include "QtnStyleHelpers.h"
#include "QtnCommonStylePrivate.h"

using namespace Qtitan;

/*!
    \class Qtitan::StyleOptionPopupTitleBar
    \internal
*/
StyleOptionPopupTitleBar::StyleOptionPopupTitleBar()
    : QStyleOptionTitleBar(Version)
{
}


/*!
    \class Qtitan::PopupDrawHelper
    \internal
*/
PopupDrawHelper::PopupDrawHelper()
{
    m_fontBold = true;
}

PopupDrawHelper::~PopupDrawHelper()
{
}

void PopupDrawHelper::polish(QWidget* widget)
{
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
    QCommonStyle::polish(widget);
#else
    QWindowsStyle::polish(widget);
#endif
}

QRect PopupDrawHelper::subControlRect(ComplexControl control, const QStyleOptionComplex* opt, SubControl subControl, const QWidget* widget) const
{
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
    QRect rect = QCommonStyle::subControlRect(control, opt, subControl, widget);
#else
    QRect rect = QWindowsStyle::subControlRect(control, opt, subControl, widget);
#endif

    switch (control) 
    {
        case CC_TitleBar:
            if (const StyleOptionPopupTitleBar* bar = qstyleoption_cast<const StyleOptionPopupTitleBar*>(opt)) 
            {
                const int controlMargin = 2;
                const int controlHeight = bar->rect.height() - controlMargin/* *2*/;
                const int delta = controlHeight + controlMargin;
                switch (subControl) 
                {
                    case SC_TitleBarLabel: 
                        {
                            QRect rc = opt->rect;
                            QRect labelRect = bar->fontMetrics.boundingRect(bar->text);
                            int titleBarHeight = proxy()->pixelMetric(PM_TitleBarHeight, opt, widget);
                            rc.setHeight(titleBarHeight); 
                            int width = labelRect.width();
                            labelRect.moveCenter(rc.center());
                            labelRect.setLeft(0);
                            labelRect.setWidth(qMax(rc.width(), width));

                            if (bar->subControls & SC_TitleBarCloseButton)
                            {
                              QRect ir = proxy()->subControlRect(CC_TitleBar, bar, SC_TitleBarCloseButton, widget);
                              labelRect.adjust(bar->subControls & SC_TitleBarSysMenu ? delta : controlMargin, 0, -ir.width(), controlMargin);
                            }
                            else
                              labelRect.adjust(bar->subControls & SC_TitleBarSysMenu ? delta : controlMargin, 0, controlMargin, controlMargin);
                            rect = labelRect;
                            break;
                        }
                    case SC_TitleBarCloseButton:
                        {
                            QRect rc = opt->rect;
                            QRect rcButton;
                            rcButton.setHeight(13);
                            rcButton.setWidth(13);
                            int titleBarHeight = proxy()->pixelMetric(PM_TitleBarHeight, opt, widget);
                            rc.setHeight(titleBarHeight); 
                            int offsetX = rc.right() - 13 - 2;
                            int offsetY = (rc.height() - rcButton.height())/2;
                            rcButton.translate(offsetX, offsetY);
                            rect = rcButton;
                            break;
                        }
                    case SC_TitleBarSysMenu :
                        {
                            QRect rc = opt->rect;
                            rc.setRect(bar->rect.left() + controlMargin, bar->rect.top() + controlMargin,
                                controlHeight, controlHeight);
                            rect = rc;
                            break;
                        }
                    default:
                        break;
                }
            }
            break;
        default:
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
            return QCommonStyle::subControlRect(control, opt, subControl, widget);
#else
            return QWindowsStyle::subControlRect(control, opt, subControl, widget);
#endif
    }

    return rect;
}

int PopupDrawHelper::pixelMetric(PixelMetric metric, const QStyleOption* option, const QWidget* widget) const
{
    Q_UNUSED(option);
    Q_UNUSED(widget);
    int value = 0;
    switch (metric) 
    {
        case PM_TitleBarHeight:
                value = 25;
            break;
        case PM_DefaultFrameWidth:
                value = 2;
            break;
        default:
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
                value = QCommonStyle::pixelMetric(metric);
#else
                value = QWindowsStyle::pixelMetric(metric);
#endif
    }
    return value;
}

QStyle::SubControl PopupDrawHelper::hitTestComplexControl(ComplexControl control, const QStyleOptionComplex* opt, const QPoint& pt, const QWidget* widget) const
{
    SubControl ret = SC_None;
    switch (control) 
    {
        case CC_TitleBar : 
            {
                const StyleOptionPopupTitleBar* bar = qstyleoption_cast<const StyleOptionPopupTitleBar*>(opt);
                QRect close = subControlRect(control, bar, QStyle::SC_TitleBarCloseButton, widget);
                if (close.contains(pt)) 
                    ret = SC_TitleBarCloseButton;
            }
            break;
        default:
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
            ret = QCommonStyle::hitTestComplexControl(control, opt, pt, widget);
#else
            ret = QWindowsStyle::hitTestComplexControl(control, opt, pt, widget);
#endif
    }
    return ret;
}

QRect getSource(QRect rcSrc, int state, int count)
{
    QRect rcImage(0, 0, rcSrc.width() / count, rcSrc.height());
    rcImage.translate(state * rcImage.width(), 0);
    return rcImage;
}

void PopupDrawHelper::drawComplexControl(ComplexControl cc, const QStyleOptionComplex* opt, QPainter* p, const QWidget* widget) const
{
    switch (cc) 
    {
        case CC_TitleBar:
            if (const StyleOptionPopupTitleBar* tb = qstyleoption_cast<const StyleOptionPopupTitleBar*>(opt)) 
            {
                QRect rect = tb->rect;

                if (tb->subControls & SC_TitleBarSysMenu/* && tb->titleBarFlags & Qt::WindowSystemMenuHint*/) 
                {
                    QRect ir = proxy()->subControlRect(CC_TitleBar, tb, SC_TitleBarSysMenu, widget);
                    if (!tb->icon.isNull())
                        tb->icon.paint(p, ir);
                }

                QRect labelRect = subControlRect(cc, tb, SC_TitleBarLabel, widget);
                
                QFont oldFont = p->font();
                QFont font = oldFont;
                if (m_fontBold)
                    font.setBold(true);
                p->setFont(font);

                p->setPen(m_clrText);
                p->drawText(labelRect, Qt::AlignLeft | Qt::AlignVCenter | Qt::TextSingleLine, 
                p->fontMetrics().elidedText(tb->text, Qt::ElideRight, qMin(rect.width(), labelRect.width()-2)));

                p->setFont(oldFont);

                if (tb->subControls & SC_TitleBarCloseButton)
                {
                    bool down = tb->activeSubControls & SC_TitleBarCloseButton && (opt->state & State_Sunken);
                    QStyleOption tool(0);
                    tool.palette = tb->palette;

                    QRect ir = proxy()->subControlRect(CC_TitleBar, tb, SC_TitleBarCloseButton, widget);
                    tool.rect = ir;
                    tool.state = down ? State_Sunken : State_Raised;

                    QPixmap pm;
                    if (!tb->pixmapCloseButton.isNull())
                    {
                        int state = 0;
                        if (down)
                            state = 2;
                        else if (tb->activeSubControls & SC_TitleBarCloseButton && (opt->state & State_MouseOver))
                            state = 1;
                        QRect rcImage = ::getSource(tb->pixmapCloseButton.rect(), state, 3);
                        pm = tb->pixmapCloseButton.copy(rcImage.left(), rcImage.top(), rcImage.width(), rcImage.height());
                    }
                    else
                    {
                        pm = standardIcon(SP_DockWidgetCloseButton, &tool, widget).pixmap(10, 10);
                        proxy()->drawPrimitive(PE_PanelButtonTool, &tool, p, widget);
                    }

                    p->save();
                    proxy()->drawItemPixmap(p, ir, Qt::AlignCenter, pm);
                    p->restore();
                }
            }
            break;
        default:
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
            QCommonStyle::drawComplexControl(cc, opt, p, widget);
#else
            QWindowsStyle::drawComplexControl(cc, opt, p, widget);
#endif
    }
}

void PopupDrawHelper::drawPrimitive(PrimitiveElement element, const QStyleOption* option, QPainter* p, const QWidget* widget) const
{
    switch (element) 
    {
        case PE_FrameWindow: 
            if (const QStyleOptionFrame* optFrame = qstyleoption_cast<const QStyleOptionFrame*>(option)) 
            {
                QRect rect = option->rect;
                int titleBarHeight = proxy()->pixelMetric(PM_TitleBarHeight, 0, 0);
                p->fillRect(rect, /*optFrame->palette*/m_palBackground.background().color());
                rect.adjust(3, titleBarHeight, -3, -3);
                p->fillRect(rect, /*optFrame->palette*/m_palBackground.foreground().color());
                rect.adjust(optFrame->lineWidth, optFrame->lineWidth, -optFrame->lineWidth, -optFrame->lineWidth); 
                rect.setBottom(rect.top() + 30);
                DrawHelpers::drawGradientFill(*p, rect, /*optFrame->palette*/m_palBackground.foreground().color().dark(111), /*optFrame->palette*/m_palBackground.foreground().color(), true);
                break;
            }
        default:
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
            QCommonStyle::drawPrimitive(element, option, p, widget);
#else
            QWindowsStyle::drawPrimitive(element, option, p, widget);
#endif
    }
}

void PopupDrawHelper::refreshPalette()
{
    m_fontBold = true;
    m_clrText = QColor(255, 255, 255);
    m_palBackground.setColor(QPalette::Background, QColor(116, 116, 116));
    m_palBackground.setColor(QPalette::Foreground, QColor(255, 255, 255));
}


/*!
    \class Qtitan::PopupOffice2000DrawHelper
    \internal
*/
PopupOffice2000DrawHelper::PopupOffice2000DrawHelper()
{
}

PopupOffice2000DrawHelper::~PopupOffice2000DrawHelper()
{
}
/*
void PopupOffice2000DrawHelper::drawComplexControl(ComplexControl cc, const QStyleOptionComplex *opt, QPainter *p, const QWidget *widget) const
{
}
*/
int PopupOffice2000DrawHelper::pixelMetric(PixelMetric metric, const QStyleOption* option, const QWidget* widget) const
{
    Q_UNUSED(option);
    Q_UNUSED(widget);
    int value = 0;
    switch (metric) 
    {
        case PM_TitleBarHeight:
            value = 22;
            break;
        default:
            value = PopupDrawHelper::pixelMetric(metric);
    }
    return value;
}

void PopupOffice2000DrawHelper::drawPrimitive(PrimitiveElement element, const QStyleOption* option, QPainter* p, const QWidget* widget) const
{
    switch (element) 
    {
        case PE_FrameWindow: 
            if (const QStyleOptionFrame* optFrame = qstyleoption_cast<const QStyleOptionFrame*>(option)) 
            {
                QRect rect = optFrame->rect;
                DrawHelpers::drawGradientFill(*p, rect, m_palBackground.background().color(), m_palBackground.background().color(), true);
                DrawHelpers::draw3DRect(*p, m_palFrame.shadow().color(), m_palFrame.shadow().color(), 
                    rect.x(), rect.y(), rect.width()-1, rect.height()-1, false);

                rect.adjust(1, 1, -1, -1);

                int titleBarHeight = proxy()->pixelMetric(PM_TitleBarHeight, 0, 0);
                QRect rcBackground = rect;
                rcBackground.setHeight(titleBarHeight);
                DrawHelpers::drawGradientFill(*p, rcBackground, QColor(0, 0, 128), QColor(24, 180, 192), false);

                rect.adjust(1, 1, -1, -1);

                rcBackground = rect;
                rcBackground.setTop(rcBackground.top() + titleBarHeight);

                DrawHelpers::draw3DRect(*p, m_palFrame.shadow().color(), m_palFrame.shadow().color(), 
                    rcBackground.x(), rcBackground.y(), rcBackground.width()-1, rcBackground.height()-1, false);

                rcBackground.adjust(1, 1, -1, -1);
                rcBackground.setRight(rcBackground.left() + 30);
                DrawHelpers::drawGradientFill(*p, rcBackground, QColor(0, 0, 128), QColor(0, 0, 128), true);

                break;
            }
        default:
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
            QCommonStyle::drawPrimitive(element, option, p, widget);
#else
            QWindowsStyle::drawPrimitive(element, option, p, widget);
#endif
    }
}

void PopupOffice2000DrawHelper::refreshPalette()
{
    m_clrText = QColor(255, 255, 255);
    m_fontBold = false;
}

/*!
    \class Qtitan::PopupOffice2003DrawHelper
    \internal
*/
PopupOffice2003DrawHelper::PopupOffice2003DrawHelper()
{
    setDecoration(OS_SYSTEMBLUE);
}

PopupOffice2003DrawHelper::~PopupOffice2003DrawHelper()
{
}

void PopupOffice2003DrawHelper::polish(QWidget* widget)
{
    PopupDrawHelper::polish(widget);
    widget->setAttribute(Qt::WA_Hover, true);
}

void PopupOffice2003DrawHelper::setDecoration(OptionsStyle style)
{
    if (m_styleType == style)
        return;

    m_styleType = style;
}

PopupOffice2003DrawHelper::OptionsStyle PopupOffice2003DrawHelper::getDecoration() const
{
    return m_styleType;
}

QRect PopupOffice2003DrawHelper::subControlRect(ComplexControl control, const QStyleOptionComplex* option, SubControl subControl, const QWidget* widget) const
{
    QRect rect = PopupDrawHelper::subControlRect(control, option, subControl, widget);

    switch (control) 
    {
    case CC_TitleBar:
        if (const StyleOptionPopupTitleBar* bar = qstyleoption_cast<const StyleOptionPopupTitleBar*>(option)) 
        {
            switch (subControl) 
            {
            case SC_TitleBarLabel: 
                {
                    if (!bar->text.isEmpty())
                    {
                        QRect rc = option->rect;
                        QRect labelRect = bar->fontMetrics.boundingRect(bar->text);
                        int lineWidth = proxy()->pixelMetric(QStyle::PM_DefaultFrameWidth, 0, 0);
                        int heightText = labelRect.height() + lineWidth;
                        labelRect.setTop(lineWidth);
                        labelRect.translate(0, rc.height());
                        labelRect.setHeight(heightText);
                        int width = labelRect.width();
                        labelRect.setLeft(0);
                        labelRect.setWidth(qMax(rc.width(), width));

                        if (bar->subControls & SC_TitleBarCloseButton)
                        {
                            QRect ir = proxy()->subControlRect(CC_TitleBar, bar, SC_TitleBarCloseButton, widget);
                            labelRect.adjust(5, 0, -ir.width(), 2);
                        }
                        else
                            labelRect.adjust(5, 0, 2, 2);
                        rect = labelRect;
                    }
                    else
                        rect = QRect();
                    break;
                }
            case SC_TitleBarCloseButton:
                {
                    QRect rc = option->rect;
                    QRect rcButton;
                    rcButton.setHeight(13);
                    rcButton.setWidth(13);
                    int titleBarHeight = proxy()->pixelMetric(PM_TitleBarHeight, option, widget);
                    int lineWidth = proxy()->pixelMetric(QStyle::PM_DefaultFrameWidth, 0, 0);
                    rc.setHeight(titleBarHeight); 
                    int offsetX = rc.right() - 13 - 2;
                    int offsetY = titleBarHeight + lineWidth*2;//(rc.height() - rcButton.height())/2;
                    rcButton.translate(offsetX, offsetY);
                    rect = rcButton;
                    break;
                }
            default:
                break;
            }
        }
        break;
    default:
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
        return QCommonStyle::subControlRect(control, option, subControl, widget);
#else
        return QWindowsStyle::subControlRect(control, option, subControl, widget);
#endif
    }

    return rect;
}

int PopupOffice2003DrawHelper::pixelMetric(PixelMetric metric, const QStyleOption* option, const QWidget* widget) const
{
    Q_UNUSED(option);
    Q_UNUSED(widget);
    int value = 0;
    switch (metric) 
    {
        case PM_TitleBarHeight:
            value = 7;
            break;
        case PM_DefaultFrameWidth:
            value = 2;
            break;
        default:
            value = PopupDrawHelper::pixelMetric(metric);
    }
    return value;
}

void PopupOffice2003DrawHelper::drawComplexControl(ComplexControl cc, const QStyleOptionComplex* opt, QPainter* p, const QWidget* widget) const
{
    switch (cc) 
    {
        case CC_TitleBar:
            if (const StyleOptionPopupTitleBar* tb = qstyleoption_cast<const StyleOptionPopupTitleBar*>(opt)) 
            {
                QRect rect = tb->rect;
                rect.adjust(1, 1, -1, -1);

                int titleBarHeight = proxy()->pixelMetric(PM_TitleBarHeight, opt, widget);
                QRect rcGripper = rect;
                rcGripper.setBottom(rcGripper.top() + titleBarHeight);
                DrawHelpers::drawGradientFill(*p, rcGripper, m_palGripper.color(QPalette::Light), m_palGripper.color(QPalette::Dark), true);

                int right = qMax(2, (rcGripper.width() - 4 * 9) / 2);
                for (int i = 0; i < 9; i++)
                {
                    p->fillRect(right, rcGripper.top() + 1, 2, 2, QColor(40, 50, 71));
                    p->fillRect(right+ 1, rcGripper.top() + 2, 2, 2, QColor(249, 249, 251));
                    p->fillRect(right+ 1, rcGripper.top() + 2, 1, 1, QColor(97, 116, 152));
                    right += 4;
                }

                QRect labelRect = subControlRect(cc, tb, SC_TitleBarLabel, widget);
                QFont oldFont = p->font();
                QFont font = oldFont;
                font.setBold(true);
                p->setFont(font);

                p->setPen(tb->palette.text().color());
                p->drawText(labelRect, Qt::AlignLeft | Qt::AlignVCenter | Qt::TextSingleLine, 
                    p->fontMetrics().elidedText(tb->text, Qt::ElideRight, qMin(rect.width(), labelRect.width()-2)));

                p->setFont(oldFont);

                if (tb->subControls & SC_TitleBarCloseButton)
                {
                    bool down = tb->activeSubControls & SC_TitleBarCloseButton && (opt->state & State_Sunken);
                    bool select = tb->activeSubControls & SC_TitleBarCloseButton && (opt->state & State_MouseOver);

                    QStyleOption tool(0);
                    tool.palette = tb->palette;

                    QRect ir = proxy()->subControlRect(CC_TitleBar, tb, SC_TitleBarCloseButton, widget);
                    tool.rect = ir;

                    if (down)
                        tool.state = State_Sunken;
                    else if (select)
                        tool.state = State_MouseOver;
                    else
                        tool.state = State_Raised;

                    QPixmap pm = standardIcon(SP_DockWidgetCloseButton, &tool, widget).pixmap(10, 10);

                    proxy()->drawPrimitive(PE_PanelButtonTool, &tool, p, widget);

                    p->save();
                    proxy()->drawItemPixmap(p, ir, Qt::AlignCenter, pm);
                    p->restore();
                }
            }
            break;
        case CC_ToolButton :
            if (const QStyleOptionToolButton* toolbutton = qstyleoption_cast<const QStyleOptionToolButton *>(opt))
            {
                QRect button, menuarea;
                button = proxy()->subControlRect(cc, toolbutton, SC_ToolButton, widget);
                menuarea = proxy()->subControlRect(cc, toolbutton, SC_ToolButtonMenu, widget);

                QStyleOption tool(0);
                tool.state = toolbutton->state;
                tool.palette = toolbutton->palette;
                tool.rect = button;
                if (toolbutton->subControls & SC_ToolButton) 
                    proxy()->drawPrimitive(PE_PanelButtonTool, &tool, p, widget);

                State bflags = toolbutton->state & ~State_Sunken;

                if (bflags & State_AutoRaise) 
                {
                    if (!(bflags & State_MouseOver) || !(bflags & State_Enabled)) 
                        bflags &= ~State_Raised;
                }

                QStyleOptionToolButton label = *toolbutton;
                label.state = bflags;//toolbutton->state & ~State_Sunken;
                int fw = proxy()->pixelMetric(PM_DefaultFrameWidth, opt, widget);
                label.rect = button.adjusted(fw, fw, -fw, -fw);
                proxy()->drawControl(CE_ToolButtonLabel, &label, p, widget);

                State mflags = bflags;
                if (toolbutton->state & State_Sunken) 
                {
                    if (toolbutton->activeSubControls & SC_ToolButton)
                        bflags |= State_Sunken;
                    mflags |= State_Sunken;
                }

                if (toolbutton->subControls & SC_ToolButtonMenu) 
                {
                    tool.rect = menuarea;
                    tool.state = mflags;
                    if (mflags & (State_Sunken | State_On | State_Raised))
                        proxy()->drawPrimitive(PE_IndicatorButtonDropDown, &tool, p, widget);
                    proxy()->drawPrimitive(PE_IndicatorArrowDown, &tool, p, widget);
                } 
                else if (toolbutton->features & QStyleOptionToolButton::HasMenu) 
                {
                    int mbi = proxy()->pixelMetric(PM_MenuButtonIndicator, toolbutton, widget);
                    QRect ir = toolbutton->rect;
                    QStyleOptionToolButton newBtn = *toolbutton;
                    newBtn.rect = QRect(ir.right() + 5 - mbi, ir.y() + ir.height() - mbi + 4, mbi - 6, mbi - 6);
                    proxy()->drawPrimitive(PE_IndicatorArrowDown, &newBtn, p, widget);
                }
            }
            break;
        default:
            PopupDrawHelper::drawComplexControl(cc, opt, p, widget);
    }
}

void PopupOffice2003DrawHelper::drawPrimitive(PrimitiveElement element, const QStyleOption* option, QPainter* p, const QWidget* widget) const
{
    switch (element) 
    {
        case PE_PanelButtonTool :
            {
                if (option->state & State_Sunken)
                    p->fillRect(option->rect.adjusted(0, 0, -2, -2), m_clrButtonPressed);
                else if (option->state & State_MouseOver)
                    p->fillRect(option->rect.adjusted(0, 0, -2, -2), m_clrButtonSelected);

                if ((option->state & State_Sunken) || (option->state & State_MouseOver))
                {
                    QPen savePen = p->pen();
                    p->setPen(option->state & State_Sunken ? m_clrButtonPressedBorder : m_clrButtonSelectedBorder);
                    p->drawRect(option->rect.adjusted(0, 0, -2, -2));
                    p->setPen(savePen);
                }
                break;
            }
        case PE_FrameWindow: 
            if (const QStyleOptionFrame* optFrame = qstyleoption_cast<const QStyleOptionFrame*>(option)) 
            {
                QRect rect = optFrame->rect;
                DrawHelpers::drawGradientFill(*p, rect, m_palBackground.color(QPalette::Light), m_palBackground.color(QPalette::Dark), true);
                DrawHelpers::draw3DRect(*p, m_palFrame.color(QPalette::Light), m_palFrame.color(QPalette::Dark), 
                    rect.x(), rect.y(), rect.width()-1, rect.height()-1, false);
                break;
            }
        default:
            PopupDrawHelper::drawPrimitive(element, option, p, widget);
    }
}

void PopupOffice2003DrawHelper::refreshPalette()
{
    PopupDrawHelper::refreshPalette();

    switch (m_styleType)
    {
        case OS_SYSTEMBLUE:
        case OS_SYSTEMROYALE:
        case OS_SYSTEMAERO:
            {
                m_palBackground.setColor(QPalette::Light, QColor(196, 218, 250));
                m_palBackground.setColor(QPalette::Dark, QColor(158, 190, 245));

                m_palGripper.setColor(QPalette::Light, QColor(89, 135, 214));
                m_palGripper.setColor(QPalette::Dark, QColor(0, 45, 150));

                m_palFrame.setColor(QPalette::Light, QColor(0, 45, 250));
                m_palFrame.setColor(QPalette::Dark, QColor(0, 45, 250));

                m_clrButtonSelected = QColor(255, 238, 194);
                m_clrButtonPressed = QColor(254, 128, 62);
                m_clrButtonSelectedBorder = QColor(0, 0, 128);
                m_clrButtonPressedBorder = QColor(0, 0, 128);
            }
            break;

        case OS_SYSTEMOLIVE :
            {
                m_palBackground.setColor(QPalette::Light, QColor(242, 241, 228));
                m_palBackground.setColor(QPalette::Dark, QColor(217, 217, 167));

                m_palGripper.setColor(QPalette::Light, QColor(120, 142, 111));
                m_palGripper.setColor(QPalette::Dark, QColor(73, 91, 67));

                m_palFrame.setColor(QPalette::Light, QColor(96, 128, 88));
                m_palFrame.setColor(QPalette::Dark, QColor(96, 128, 88));

                m_clrButtonSelected = QColor(255, 238, 194);
                m_clrButtonPressed = QColor(254, 128, 62);
                m_clrButtonSelectedBorder = QColor(63, 93, 56);
                m_clrButtonPressedBorder = QColor(63, 93, 56);
            }
            break;

        case OS_SYSTEMSILVER:
            {
                m_palBackground.setColor(QPalette::Light, QColor(243, 243, 247));
                m_palBackground.setColor(QPalette::Dark, QColor(215, 215, 229));

                m_palGripper.setColor(QPalette::Light, QColor(168, 167, 191));
                m_palGripper.setColor(QPalette::Dark, QColor(119, 118, 151));

                m_palFrame.setColor(QPalette::Light, QColor(124, 124, 148));
                m_palFrame.setColor(QPalette::Dark, QColor(124, 124, 148));

                m_clrButtonSelected = QColor(255, 238, 194);
                m_clrButtonPressed = QColor(254, 128, 62);
                m_clrButtonSelectedBorder = QColor(75, 75, 111);
                m_clrButtonPressedBorder = QColor(75, 75, 111);
            }
            break;
        default:
            break;
    }
}


/*!
    \class Qtitan::PopupOffice2007DrawHelper
    \internal
*/
PopupOffice2007DrawHelper::PopupOffice2007DrawHelper()
{
    m_helper = new StyleHelper(this);
}

PopupOffice2007DrawHelper::~PopupOffice2007DrawHelper()
{
}

void PopupOffice2007DrawHelper::refreshPalette()
{
    if (m_styleType == OS_SYSTEMBLUE)
    {
        m_helper->openIniFile(":/res/Office2007Blue/OfficeTheme.ini");

        m_palBackground.setColor(QPalette::Dark, m_helper->getColor(QObject::tr("PopupControl"), QObject::tr("BackgroundDark")));
        m_palBackground.setColor(QPalette::Light, m_helper->getColor(QObject::tr("PopupControl"), QObject::tr("BackgroundLight")));

        m_palGripper.setColor(QPalette::Dark, m_helper->getColor(QObject::tr("PopupControl"), QObject::tr("CaptionDark")));
        m_palGripper.setColor(QPalette::Light, m_helper->getColor(QObject::tr("PopupControl"), QObject::tr("CaptionLight")));

        m_palFrame.setColor(QPalette::Dark, m_helper->getColor(QObject::tr("PopupControl"), QObject::tr("FrameBorder")));
        m_palFrame.setColor(QPalette::Light, m_helper->getColor(QObject::tr("PopupControl"), QObject::tr("FrameBorder")));

        m_clrButtonSelected = m_helper->getColor(QObject::tr("PopupControl"), QObject::tr("ButtonSelected"));
        m_clrButtonPressed = m_helper->getColor(QObject::tr("PopupControl"), QObject::tr("ButtonPressed"));
        m_clrButtonSelectedBorder = m_helper->getColor(QObject::tr("PopupControl"), QObject::tr("ButtonBorder"));
        m_clrButtonPressedBorder = m_helper->getColor(QObject::tr("PopupControl"), QObject::tr("ButtonBorder"));
    }
    else
        PopupOffice2003DrawHelper::refreshPalette();
}

/*!
    \class Qtitan::PopupOffice2010DrawHelper
    \internal
*/
PopupOffice2010DrawHelper::PopupOffice2010DrawHelper()
{
}

PopupOffice2010DrawHelper::~PopupOffice2010DrawHelper()
{
}

/*!
    \class Qtitan::PopupMSNDrawHelper
    \internal
*/
PopupMSNDrawHelper::PopupMSNDrawHelper()
{
}

PopupMSNDrawHelper::~PopupMSNDrawHelper()
{
}

void PopupMSNDrawHelper::drawPrimitive(PrimitiveElement element, const QStyleOption* option, QPainter* p, const QWidget* widget) const
{
    switch (element) 
    {
/*
        case PE_PanelButtonTool :
            {
                if (option->state & State_Sunken)
                    p->fillRect(option->rect.adjusted(0, 0, -2, -2), m_clrButtonPressed);
                else if (option->state & State_MouseOver)
                    p->fillRect(option->rect.adjusted(0, 0, -2, -2), m_clrButtonSelected);

                if ((option->state & State_Sunken) || (option->state & State_MouseOver))
                {
                    QPen savePen = p->pen();
                    p->setPen(option->state & State_Sunken ? m_clrButtonPressedBorder : m_clrButtonSelectedBorder);
                    p->drawRect(option->rect.adjusted(0, 0, -2, -2));
                    p->setPen(savePen);
                }
                break;
            }
*/
        case PE_FrameWindow: 
            if (const QStyleOptionFrame* optFrame = qstyleoption_cast<const QStyleOptionFrame*>(option)) 
            {
                QRect rect = optFrame->rect;

                DrawHelpers::draw3DRect(*p, QColor(166, 180, 207), QColor(69, 86, 144), 
                    rect.x(), rect.y(), rect.width()-1, rect.height()-1, true);
                rect.adjust(1, 1, -1, -1);
                DrawHelpers::draw3DRect(*p, QColor(255, 255, 255), QColor(207, 222, 244), 
                    rect.x(), rect.y(), rect.width()-1, rect.height()-1, true);
                rect.adjust(1, 1, -1, -1);

                int titleBarHeight = proxy()->pixelMetric(PM_TitleBarHeight, option, widget);

                QRect rcBackground = rect;
                rcBackground.setHeight(12); 
                DrawHelpers::drawGradientFill(*p, rcBackground, QColor(207, 215, 236), QColor(255, 255, 255), true);

                rcBackground.setHeight(titleBarHeight + 1); 
                DrawHelpers::drawGradientFill(*p, rcBackground, QColor(255, 255, 255), QColor(207, 221, 244), true);

                rcBackground.setHeight(40); 
                DrawHelpers::drawGradientFill(*p, rcBackground, QColor(207, 221, 244), QColor(255, 255, 255), true);

//                rcBackground = CRect(rcClient.left, rcBackground.bottom, rcClient.right, rcClient.bottom);
                rcBackground.setTop(rcBackground.bottom());
                rcBackground.setBottom(rect.bottom());
                DrawHelpers::drawGradientFill(*p, rcBackground, QColor(255, 255, 255), QColor(207, 221, 244), true);

                QRect rcFrame = rect;//(rcClient.left, rcClient.top + 22, rcClient.right, rcClient.bottom);
                rcFrame.setTop(rect.top() + titleBarHeight); 
                DrawHelpers::draw3DRect(*p, QColor(114, 142, 184), QColor(185, 201, 239), 
                    rcFrame.x(), rcFrame.y(), rcFrame.width()-1, rcFrame.height()-1, true);
                break;
            }
        default:
            PopupOffice2000DrawHelper::drawPrimitive(element, option, p, widget);
    }
}

void PopupMSNDrawHelper::refreshPalette()
{
    m_fontBold = false;
    m_clrText = QColor(0, 0, 0);
}
