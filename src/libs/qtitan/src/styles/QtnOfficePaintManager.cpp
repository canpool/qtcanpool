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
#include <QDesktopWidget>
#include <QMainWindow>
#include <QPainter>
#include <QStyle>
#include <QBitmap>
#include <QComboBox>
#include <QLineEdit>
#include <QCheckBox>
#include <QDialog>
#include <QToolBar>
#include <QToolButton>
#include <QListWidget>
#include <QMdiArea>
#include <QMdiSubWindow>
#include <QToolTip>
#include <QDockWidget>
#include <QGroupBox>
#include <QTextEdit>
#include <QPainterPath>

#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
#include <qdrawutil.h>
#endif 

#if (QT_VERSION < QT_VERSION_CHECK(5, 0, 0))
#include <QWorkspace>
#endif

#include "QtnOfficeStylePrivate.h"
#include "QtnStyleHelpers.h"
#include "QtnRibbonGallery.h"
#include "QtnRibbonToolTip.h"
#include "QtnRibbonControls.h"

#include "QtnOfficePopupMenu.h"
#ifdef DEBUG_MEMORY_ENABLED
#include "QtitanMSVSDebug.h"
#endif


QTITAN_USE_NAMESPACE

static const int iImageWidth = 9;
static const int iImageHeight = 9;
static const int iTextMargin = 3;
static const QColor clrTransparent = QColor(255, 0, 255);

static inline bool isAncestor(const QObject* child)
{
    while (child) 
    {
        if (qobject_cast<const RibbonBar*>(child))
            return true;
        child = child->parent();
    }
    return false;
}


/*!
\class Qtitan::OfficePaintManager
\internal
*/
OfficePaintManager::OfficePaintManager(CommonStyle* baseStyle)
    : CommonPaintManager(baseStyle)
{
}

OfficePaintManager::~OfficePaintManager()
{
}

QString OfficePaintManager::theme(const QString& str) const
{
    return ((OfficeStyle*)baseStyle())->qtn_d().theme(str);
}

OfficeStyle::Theme OfficePaintManager::getTheme() const
{
    return ((OfficeStyle*)baseStyle())->getTheme();
}

QColor OfficePaintManager::textColor(bool selected, bool pressed, bool enabled, bool checked, bool popuped, BarType barType, BarPosition barPosition) const
{
    return ((OfficeStyle*)baseStyle())->getTextColor(selected, pressed, enabled, checked, popuped, barType, barPosition);
}

/*! \internal */
bool OfficePaintManager::isStyle2010() const
{
    return ((OfficeStyle*)baseStyle())->isStyle2010();
}

/*! \internal */
void OfficePaintManager::polish(QWidget* widget)
{
    QTN_D_STYLE(OfficeStyle)
    if (qobject_cast<QAbstractSpinBox*>(widget))
    {
        QPalette pal = widget->palette();
        pal.setColor(QPalette::All, QPalette::Shadow, d.m_clrEditCtrlBorder);
        pal.setColor(QPalette::All, QPalette::AlternateBase, d.m_clrControlEditNormal);
        widget->setPalette(pal);
    }
}

QIcon OfficePaintManager::qtnStandardIcon(QStyle::StandardPixmap px, const QStyleOption* opt, const QWidget* wd, bool& ret ) const
{
    Q_UNUSED(px);
    Q_UNUSED(opt);
    Q_UNUSED(wd);
    ret = false;
    return QIcon();
}

/*! \internal */
bool OfficePaintManager::drawWorkspace(const QStyleOption* opt, QPainter* p, const QWidget* w) const
{
    Q_UNUSED(w);
    QTN_D_STYLE(const OfficeStyle)

    if (opt == Q_NULL)
        return false;

    QPixmap soWorkspace = cached("WorkspaceTopLeft.png");
    if (!soWorkspace.isNull())
    {
        QRect rc = opt->rect;
        QRect rcSrc = soWorkspace.rect();
        QRect rcTopLeft = opt->rect;
        rcTopLeft.setBottom(rcTopLeft.top() + rcSrc.height());
        rcTopLeft.setRight(rcTopLeft.left() + qMax(rcTopLeft.width(), rcSrc.width()));
        drawImage(soWorkspace, *p, rcTopLeft, rcSrc, QRect(QPoint(rcSrc.width() - 1, 0), QPoint(0, 0)));

        QRect rcFill(QPoint(rc.left(), rc.top() + rcSrc.height()), QPoint(rc.right(), rc.bottom()));
        QRect rcFillTop(QPoint(rcFill.left(), rcFill.top()), QPoint(rcFill.right() + 1, rcFill.top() + rcFill.height() * 2 / 3));
        QRect rcFillBottom(QPoint(rcFill.left(), rcFillTop.bottom()), QPoint(rcFill.right() + 1, rcFill.bottom()));
        DrawHelpers::drawGradientFill(*p, rcFillTop, d.m_clrWorkspaceClientTop, d.m_clrWorkspaceClientMiddle, true);
        DrawHelpers::drawGradientFill(*p, rcFillBottom, d.m_clrWorkspaceClientMiddle, d.m_clrWorkspaceClientBottom, true);
        return true;
    }
    return false;
}

// for TitleBar
/*! \internal */
bool OfficePaintManager::drawToolBar(const QStyleOption* opt, QPainter* p, const QWidget* w) const
{
    Q_UNUSED(w);
    QTN_D_STYLE(const OfficeStyle)
    if (const QStyleOptionToolBar* toolbar = qstyleoption_cast<const QStyleOptionToolBar*>(opt)) 
    {
        if (toolbar->state & QStyle::State_Horizontal)
        {
            if (w->parentWidget())
                p->fillRect(toolbar->rect, d.m_clrDockBar);

            QRect rcFill = toolbar->rect;
            rcFill.setTop(rcFill.top()+1);
            // [1]
            DrawHelpers::drawGradientFill(*p, rcFill, d.m_clrCommandBar.color(QPalette::Light), d.m_clrCommandBar.color(QPalette::Dark), true);

            p->fillRect(QRect(QPoint(rcFill.left(), rcFill.bottom()), QSize(rcFill.width(), rcFill.height() - rcFill.height() - 2)), d.m_clrCommandBar.color(QPalette::Dark));

            p->fillRect(QRect(QPoint(toolbar->rect.left() + 5, toolbar->rect.bottom() - 1), QSize(toolbar->rect.width()/* - 7*/, 1)), d.m_clrCommandBar.color(QPalette::Shadow));

            p->fillRect(QRect(QPoint(toolbar->rect.right(), toolbar->rect.top() - 1), QSize(1, toolbar->rect.height())), d.m_clrCommandBar.color(QPalette::Shadow));
        }
        else
        {
            if (w->parentWidget())
            {
                if (toolbar->toolBarArea & Qt::RightToolBarArea)
                    p->fillRect(toolbar->rect, d.m_clrCommandBar.color(QPalette::Dark));
                else
                    p->fillRect(toolbar->rect, d.m_clrDockBar);
            }
            QRect rcFill = toolbar->rect;
            rcFill.setLeft(rcFill.left() + 1);
            // [1]
            DrawHelpers::drawGradientFill(*p, rcFill, d.m_clrCommandBar.color(QPalette::Light), d.m_clrCommandBar.color(QPalette::Dark), false);

            p->fillRect(QRect(QPoint(toolbar->rect.right() - 1, toolbar->rect.top() + 1), QSize(1, toolbar->rect.height()/* - 7*/)), d.m_clrCommandBar.color(QPalette::Shadow));

            p->fillRect(QRect(QPoint(toolbar->rect.left(), toolbar->rect.bottom()), QSize(toolbar->rect.width(), 1)), d.m_clrCommandBar.color(QPalette::Shadow));
        }
        return true;
    }
    return false;
}

/*! \internal */
void OfficePaintManager::drawSysButton(QPainter* p, const QRect& rect, bool down, bool highlighted, bool closeButton) const
{
    if (down || highlighted)
    {
        QPixmap soImageButton;
        if (closeButton)
            soImageButton = cached("FrameTitleCloseButton.png");

        if (soImageButton.isNull())
            soImageButton = cached("FrameTitleButton.png");

        drawImage(soImageButton, *p, rect, sourceRectImage(soImageButton.rect(), down || highlighted ? 1 : 0, 2),
            QRect(QPoint(3, 3), QPoint(3, 3)), QColor(0xFF, 0, 0xFF));
    }
}

/*! \internal */
bool OfficePaintManager::drawTitleBar(const QStyleOptionComplex* opt, QPainter* p, const QWidget* wd) const
{
    QTN_D_STYLE(const OfficeStyle)
    if (!wd || !qobject_cast<const QMdiSubWindow*>(wd))
    {
#if (QT_VERSION < QT_VERSION_CHECK(5, 0, 0)) // test on QWorkspace
        QWidget* widget = (wd && wd->parentWidget()) ? wd->parentWidget()->parentWidget() : Q_NULL;
        if (!qobject_cast<const QWorkspace*>(widget))
            return false;
#else
        return false;
#endif
    }

    if (const QStyleOptionTitleBar* tb = qstyleoption_cast<const QStyleOptionTitleBar *>(opt)) 
    {
        bool active = tb->titleBarState & QStyle::State_Active;
        QRect ir;
        if (opt->subControls & QStyle::SC_TitleBarLabel) 
        {
            ir = baseStyle()->proxy()->subControlRect(QStyle::CC_TitleBar, tb, QStyle::SC_TitleBarLabel, wd);
            QPixmap soImage = cached("FrameTopCenter.png");
            if (!soImage.isNull())
            {
                QRect rc = opt->rect;
                int border = rc.left();
                rc.setTop(0); rc.setLeft(1);
                rc.setRight(rc.right() + border - 1);
                drawImage(soImage, *p, rc, sourceRectImage(soImage.rect(), active ? 0 : 1, 2));
            }
            QColor color = active ? d.m_clrFrameCaptionTextActive : d.m_clrFrameCaptionTextInActive;
            if (!color.isValid())
                color = tb->palette.text().color();
            p->setPen(color);
            p->drawText(ir.x() + 2, ir.y(), ir.width() - 2, ir.height(), Qt::AlignLeft | Qt::AlignVCenter | Qt::TextSingleLine, tb->text);
        }

        bool down = false;
        bool highlighted = false;
        QPixmap pm;

        QStyleOption tool(0);
        tool.palette = tb->palette;

        QString strGlyphSize = "17.png";
        if (ir.width() < 27) strGlyphSize = "17.png";
        if (ir.width() < 15) strGlyphSize = "13.png";

        if (tb->subControls & QStyle::SC_TitleBarCloseButton && tb->titleBarFlags & Qt::WindowSystemMenuHint) 
        {
            QString strButton = "FrameTitleClose" + strGlyphSize;

            ir = baseStyle()->proxy()->subControlRect(QStyle::CC_TitleBar, tb, QStyle::SC_TitleBarCloseButton, wd);
            down = tb->activeSubControls & QStyle::SC_TitleBarCloseButton && (opt->state & QStyle::State_Sunken);
            highlighted = tb->activeSubControls & QStyle::SC_TitleBarCloseButton && (opt->state & QStyle::State_MouseOver);

            if ((tb->titleBarFlags & Qt::WindowType_Mask) == Qt::Tool)
                pm = baseStyle()->standardIcon(QStyle::SP_DockWidgetCloseButton, &tool, wd).pixmap(10, 10);
            else
            {
                pm = cached(strButton);
            }

            tool.rect = ir;
            tool.state = down ? QStyle::State_Sunken : QStyle::State_Raised;
            baseStyle()->proxy()->drawPrimitive(QStyle::PE_PanelButtonTool, &tool, p, wd);

            p->save();
            drawSysButton(p, ir, down, highlighted, tb->activeSubControls == QStyle::SC_TitleBarCloseButton);
            int state = !active ? 3 : down && highlighted ? 2 : highlighted || down ? 1 : 0;
            QRect rcSrc = sourceRectImage(pm.rect(), state, 5);
            QRect rcGlyph(QPoint((ir.right() + ir.left() - rcSrc.width()+1) / 2, (ir.top() + ir.bottom() - rcSrc.height()+2) / 2), rcSrc.size());
            drawImage(pm, *p, rcGlyph, rcSrc, QRect(QPoint(0, 0), QPoint(0, 0)), QColor(0xFF, 0, 0xFF));
            p->restore();
        }

        if (tb->subControls & QStyle::SC_TitleBarMaxButton && 
            tb->titleBarFlags & Qt::WindowMaximizeButtonHint && !(tb->titleBarState & Qt::WindowMaximized)) 
        {
            QString strButton = "FrameTitleMaximize" + strGlyphSize;

            ir = baseStyle()->proxy()->subControlRect(QStyle::CC_TitleBar, tb, QStyle::SC_TitleBarMaxButton, wd);
            down = tb->activeSubControls & QStyle::SC_TitleBarMaxButton && (opt->state & QStyle::State_Sunken);
            highlighted = tb->activeSubControls & QStyle::SC_TitleBarMaxButton && (opt->state & QStyle::State_MouseOver);

            pm = cached(strButton);
            tool.rect = ir;
            tool.state = down ? QStyle::State_Sunken : QStyle::State_Raised;
            baseStyle()->proxy()->drawPrimitive(QStyle::PE_PanelButtonTool, &tool, p, wd);

            p->save();
            drawSysButton(p, ir, down, highlighted, tb->activeSubControls == QStyle::SC_TitleBarCloseButton);

            int state = !active ? 3 : down && highlighted ? 2 : highlighted || down ? 1 : 0;
            QRect rcSrc = sourceRectImage(pm.rect(), state, 5);
            QRect rcGlyph(QPoint((ir.right() + ir.left() - rcSrc.width()+1) / 2, (ir.top() + ir.bottom() - rcSrc.height()+2) / 2), rcSrc.size());
            drawImage(pm, *p, rcGlyph, rcSrc, QRect(QPoint(0, 0), QPoint(0, 0)), QColor(0xFF, 0, 0xFF));
            p->restore();
        }

        if (tb->subControls & QStyle::SC_TitleBarMinButton && 
            tb->titleBarFlags & Qt::WindowMinimizeButtonHint && !(tb->titleBarState & Qt::WindowMinimized)) 
        {
            QString strButton = "FrameTitleMinimize" + strGlyphSize;
            ir = baseStyle()->proxy()->subControlRect(QStyle::CC_TitleBar, tb, QStyle::SC_TitleBarMinButton, wd);
            down = tb->activeSubControls & QStyle::SC_TitleBarMinButton && (opt->state & QStyle::State_Sunken);
            highlighted = tb->activeSubControls & QStyle::SC_TitleBarMinButton && (opt->state & QStyle::State_MouseOver);

            pm = cached(strButton);
            tool.rect = ir;
            tool.state = down ? QStyle::State_Sunken : QStyle::State_Raised;
            baseStyle()->proxy()->drawPrimitive(QStyle::PE_PanelButtonTool, &tool, p, wd);

            p->save();
            drawSysButton(p, ir, down, highlighted, tb->activeSubControls == QStyle::SC_TitleBarCloseButton);
            int state = !active ? 3 : down && highlighted ? 2 : highlighted || down ? 1 : 0;
            QRect rcSrc = sourceRectImage(pm.rect(), state, 5);
            QRect rcGlyph(QPoint((ir.right() + ir.left() - rcSrc.width()+1) / 2, (ir.top() + ir.bottom() - rcSrc.height()+2) / 2), rcSrc.size());
            drawImage(pm, *p, rcGlyph, rcSrc, QRect(QPoint(0, 0), QPoint(0, 0)), QColor(0xFF, 0, 0xFF));
            p->restore();
        }

        bool drawNormalButton = (tb->subControls & QStyle::SC_TitleBarNormalButton)
            && (((tb->titleBarFlags & Qt::WindowMinimizeButtonHint)
            && (tb->titleBarState & Qt::WindowMinimized))
            || ((tb->titleBarFlags & Qt::WindowMaximizeButtonHint)
            && (tb->titleBarState & Qt::WindowMaximized)));

        if (drawNormalButton) 
        {
            QString strButton = "FrameTitleRestore" + strGlyphSize;
            ir = baseStyle()->proxy()->subControlRect(QStyle::CC_TitleBar, tb, QStyle::SC_TitleBarNormalButton, wd);
            down = tb->activeSubControls & QStyle::SC_TitleBarNormalButton && (opt->state & QStyle::State_Sunken);
            highlighted = tb->activeSubControls & QStyle::SC_TitleBarNormalButton && (opt->state & QStyle::State_MouseOver);

            pm = cached(strButton);
            tool.rect = ir;
            tool.state = down ? QStyle::State_Sunken : QStyle::State_Raised;
            baseStyle()->proxy()->drawPrimitive(QStyle::PE_PanelButtonTool, &tool, p, wd);

            p->save();
            drawSysButton(p, ir, down, highlighted, tb->activeSubControls == QStyle::SC_TitleBarCloseButton);
            int state = !active ? 3 : down && highlighted ? 2 : highlighted || down ? 1 : 0;
            QRect rcSrc = sourceRectImage(pm.rect(), state, 5);
            QRect rcGlyph(QPoint((ir.right() + ir.left() - rcSrc.width()+1) / 2, (ir.top() + ir.bottom() - rcSrc.height()+2) / 2), rcSrc.size());
            drawImage(pm, *p, rcGlyph, rcSrc, QRect(QPoint(0, 0), QPoint(0, 0)), QColor(0xFF, 0, 0xFF));
            p->restore();
        }
/*
        if (tb->subControls & SC_TitleBarShadeButton
            && tb->titleBarFlags & Qt::WindowShadeButtonHint
            && !(tb->titleBarState & Qt::WindowMinimized)) {
                ir = proxy()->subControlRect(CC_TitleBar, tb, SC_TitleBarShadeButton, widget);
                down = (tb->activeSubControls & SC_TitleBarShadeButton && (opt->state & State_Sunken));
                pm = standardIcon(SP_TitleBarShadeButton, &tool, widget).pixmap(10, 10);
                tool.rect = ir;
                tool.state = down ? State_Sunken : State_Raised;
                proxy()->drawPrimitive(PE_PanelButtonTool, &tool, p, widget);
                p->save();
                if (down)
                    p->translate(proxy()->pixelMetric(PM_ButtonShiftHorizontal, tb, widget),
                    proxy()->pixelMetric(PM_ButtonShiftVertical, tb, widget));
                proxy()->drawItemPixmap(p, ir, Qt::AlignCenter, pm);
                p->restore();
        }

        if (tb->subControls & SC_TitleBarUnshadeButton
            && tb->titleBarFlags & Qt::WindowShadeButtonHint
            && tb->titleBarState & Qt::WindowMinimized) {
                ir = proxy()->subControlRect(CC_TitleBar, tb, SC_TitleBarUnshadeButton, widget);

                down = tb->activeSubControls & SC_TitleBarUnshadeButton  && (opt->state & State_Sunken);
                pm = standardIcon(SP_TitleBarUnshadeButton, &tool, widget).pixmap(10, 10);
                tool.rect = ir;
                tool.state = down ? State_Sunken : State_Raised;
                proxy()->drawPrimitive(PE_PanelButtonTool, &tool, p, widget);
                p->save();
                if (down)
                    p->translate(proxy()->pixelMetric(PM_ButtonShiftHorizontal, tb, widget),
                    proxy()->pixelMetric(PM_ButtonShiftVertical, tb, widget));
                proxy()->drawItemPixmap(p, ir, Qt::AlignCenter, pm);
                p->restore();
        }
        if (tb->subControls & SC_TitleBarContextHelpButton && tb->titleBarFlags & Qt::WindowContextHelpButtonHint) 
            {
                ir = proxy()->subControlRect(CC_TitleBar, tb, SC_TitleBarContextHelpButton, widget);

                down = tb->activeSubControls & SC_TitleBarContextHelpButton  && (opt->state & State_Sunken);
                pm = standardIcon(SP_TitleBarContextHelpButton, &tool, widget).pixmap(10, 10);
                tool.rect = ir;
                tool.state = down ? State_Sunken : State_Raised;
                proxy()->drawPrimitive(PE_PanelButtonTool, &tool, p, widget);
                p->save();
                if (down)
                    p->translate(proxy()->pixelMetric(PM_ButtonShiftHorizontal, tb, widget),
                    proxy()->pixelMetric(PM_ButtonShiftVertical, tb, widget));
                proxy()->drawItemPixmap(p, ir, Qt::AlignCenter, pm);
                p->restore();
        }
*/
        if (tb->subControls & QStyle::SC_TitleBarSysMenu && tb->titleBarFlags & Qt::WindowSystemMenuHint) 
        {
            ir = baseStyle()->proxy()->subControlRect(QStyle::CC_TitleBar, tb, QStyle::SC_TitleBarSysMenu, wd);
            if (!tb->icon.isNull()) 
                tb->icon.paint(p, ir);
            else 
            {
                int iconSize = baseStyle()->proxy()->pixelMetric(QStyle::PM_SmallIconSize, tb, wd);
                pm = baseStyle()->standardIcon(QStyle::SP_TitleBarMenuButton, &tool, wd).pixmap(iconSize, iconSize);
                tool.rect = ir;
                p->save();
                baseStyle()->proxy()->drawItemPixmap(p, ir, Qt::AlignCenter, pm);
                p->restore();
            }
        }
        return true;
    }
    return false;
}

/*! \internal */
bool OfficePaintManager::drawIndicatorToolBarSeparator(const QStyleOption* opt, QPainter* p, const QWidget* w) const
{
    Q_UNUSED(w);
    Q_UNUSED(p);
    QTN_D_STYLE(OfficeStyle)

    QStyleOption* option = (QStyleOption*)opt;
    option->palette.setColor(QPalette::Dark, d.m_clrToolbarSeparator);
    option->palette.setColor(QPalette::Light, QColor(255, 255, 255));
    return true; 
}

// for DockWidget
/*! \internal */
bool OfficePaintManager::drawIndicatorDockWidgetResizeHandle(const QStyleOption* opt, QPainter* p, const QWidget* w) const
{
    Q_UNUSED(w);
    QTN_D_STYLE(OfficeStyle)

    QRect rcFrame = w->rect();
    float fHeight = (float)rcFrame.height();

    QRgb clr1 = DrawHelpers::blendColors(d.m_palSplitter.color(QPalette::Light).rgb(), 
        d.m_palSplitter.color(QPalette::Dark).rgb(), float(opt->rect.bottom() - rcFrame.top()) / fHeight);

    QRgb clr2 = DrawHelpers::blendColors(d.m_palSplitter.color(QPalette::Light).rgb(), 
        d.m_palSplitter.color(QPalette::Dark).rgb(), float(opt->rect.top() - rcFrame.top()) / fHeight);

    // [1]
    DrawHelpers::drawGradientFill(*p, opt->rect, clr1, clr2, true);
    return true;
}

// for menuBar
/*! \internal */
bool OfficePaintManager::drawPanelMenu(const QStyleOption* opt, QPainter* p, const QWidget* w) const
{
    QTN_D_STYLE(const OfficeStyle)
    QPixmap soPopupBarFrame = cached("PopupBarFrame.png");
    if (!soPopupBarFrame.isNull())
        drawImage(soPopupBarFrame, *p, opt->rect, soPopupBarFrame.rect(), QRect(4, 4, 4, 4));

    bool showGripper = true;
    // draw a Gripper or not
    QVariant var = w->property("showGripper");
    if (!var.isNull())
        showGripper = var.toBool();

    if(showGripper)
    {
        const int nMenuPanelWidth = baseStyle()->pixelMetric(QStyle::PM_MenuPanelWidth, opt, w);
        const int nIconSize = baseStyle()->pixelMetric(QStyle::PM_ToolBarIconSize, opt, w) + 2;
        QRect rcBorders(QPoint(nMenuPanelWidth, nMenuPanelWidth), QPoint(nMenuPanelWidth, nMenuPanelWidth));

        int x = rcBorders.left()-1;
        int y = rcBorders.top();
        int cx = nIconSize;
        int cy = opt->rect.bottom() - rcBorders.top() - rcBorders.bottom();
        // [1]
        DrawHelpers::drawGradientFill(*p, QRect(QPoint(x + 1, y), QSize(cx - 1, cy)), d.m_clrMenuGripper, d.m_clrMenuGripper, true);

        p->fillRect(x + cx - 1, y, 1, cy + 1, d.m_clrMenuPopupGripperShadow);
        p->fillRect(x + cx, y, 1, cy + 1, QColor(245, 245, 245));
    }
    return true;
}

/*! \internal */
bool OfficePaintManager::drawFrameMenu(const QStyleOption* opt, QPainter* p, const QWidget* w) const
{
    Q_UNUSED(opt);
    Q_UNUSED(p);
    if (qobject_cast<const QToolBar*>(w))
        return true;
    return false;
}

/*! \internal */
bool OfficePaintManager::drawIndicatorMenuCheckMark(const QStyleOption* opt, QPainter* p, const QWidget* w) const
{
    Q_UNUSED(w);
    bool dis = !(opt->state & QStyle::State_Enabled);
    QPixmap soMenuCheckedItem = cached("MenuCheckedItem.png");
    QRect rcSrc = sourceRectImage(soMenuCheckedItem.rect(), !dis ? 0 : 1, 2);

    const int iconSize = baseStyle()->proxy()->pixelMetric(QStyle::PM_SmallIconSize, opt, w) + 4;
    QRect rc(QPoint(opt->rect.left() + opt->rect.width()*0.5 - iconSize*0.5, 
    opt->rect.top() + opt->rect.height()*0.5 - iconSize*0.5), QSize(iconSize, iconSize));
    drawImage(soMenuCheckedItem, *p, rc, rcSrc, QRect(QPoint(2, 2), QPoint(2, 2)));

    QPixmap soMenuCheckedItemMark = cached("MenuCheckedItemMark.png");
    rcSrc = sourceRectImage(soMenuCheckedItemMark.rect(), !dis ? 0 : 1, 4);
    QRect rcDest(QPoint((rc.left() + rc.right()-rcSrc.width())*0.5 + 1, (rc.top() + rc.bottom() - rcSrc.height())*0.5 + 1), rcSrc.size() );
    drawImage(soMenuCheckedItemMark, *p, rcDest, rcSrc, QRect(QPoint(0, 0), QPoint(0, 0)), QColor(0xFF, 0, 0xFF));
    return true;
}

// for Buttons
/*! \internal */
bool OfficePaintManager::drawIndicatorCheckRadioButton(QStyle::PrimitiveElement element, const QStyleOption* option, 
                                                       QPainter* painter, const QWidget* widget) const
{
    Q_UNUSED(widget);
    bool isRadio = (element == QStyle::PE_IndicatorRadioButton);
    QPixmap soImage = cached(isRadio ? "ToolbarButtonRadioButton.png" : "ToolbarButtonCheckBox.png");
    int state = 0;
    bool enabled  = option->state & QStyle::State_Enabled;
    bool checked  = option->state & QStyle::State_On;
    bool noChange = option->state & QStyle::State_NoChange;
    bool selected = option->state & QStyle::State_MouseOver;
    bool pressed  = option->state & QStyle::State_Sunken;

    int stateChecked = checked ? 1 : 0;
    if (!isRadio && noChange)
        stateChecked = 2;

    if (!enabled)
        state = 3;
    else if (selected && pressed)
        state = 2;
    else if (selected)
        state = 1;

    if (stateChecked == 1)
        state += 4;

    if (stateChecked == 2)
        state += 8;

    QRect rc = option->rect;
//    rc.setY(rc.y() + (rc.height() - 12) / 2 + ((rc.height() - 12) % 2));
//    rc.setWidth(13); rc.setHeight(13);

    drawImage(soImage, *painter, rc, sourceRectImage(soImage.rect(), state, isRadio ? 8 : 12));
    return true;
}

/*! \internal */
bool OfficePaintManager::drawIndicatorViewItemCheck(QStyle::PrimitiveElement element, const QStyleOption* option, QPainter* painter, const QWidget* widget) const
{
    return drawIndicatorCheckRadioButton(element, option, painter, widget);
}

/*! \internal */
bool OfficePaintManager::drawPanelButtonCommand(const QStyleOption* opt, QPainter* p, const QWidget* w) const
{
    Q_UNUSED(w);
    if (const QStyleOptionButton* btn = qstyleoption_cast<const QStyleOptionButton*>(opt)) 
    {
        uint resolve_mask = btn->palette.resolve();
        if (resolve_mask & (1 << QPalette::Base)) 
            return false;

        QPixmap soImage = cached("PushButton.png");

        if (soImage.isNull())
        {
            Q_ASSERT(false);
            return false;
        }

        bool enabled  = opt->state & QStyle::State_Enabled;
        bool checked  = opt->state & QStyle::State_On;
        bool selected = opt->state & QStyle::State_MouseOver;
        bool pressed  = opt->state & QStyle::State_Sunken;
        bool defaultBtn = btn->features & QStyleOptionButton::DefaultButton;

        int state = defaultBtn ? 4 : 0;

        if (!enabled)
            state = 3;
        else if (checked && !selected && !pressed) state = 2;
        else if (checked && selected && !pressed) state = 1;
        else if ((selected && pressed) /*|| qobject_cast<const QPushButton*>(w)*/) state = 2;
        else if (selected || pressed) state = 1;

        drawImage(soImage, *p, opt->rect, sourceRectImage(soImage.rect(), state, 5), 
            QRect(QPoint(4, 4), QPoint(4, 4)), QColor(0xFF, 0, 0xFF));
        return true;
    }
    return false;
}

// shared
/*! \internal */
bool OfficePaintManager::drawPanelTipLabel(const QStyleOption* opt, QPainter* p, const QWidget* w) const
{
    Q_UNUSED(w);
    QTN_D_STYLE(OfficeStyle)

    QColor clrLight = d.m_clrTooltipLight;
    QColor clrDark = d.m_clrTooltipDark;
    if (!clrLight.isValid()) 
        clrLight = QColor(255, 255, 255);
    if (!clrDark.isValid()) 
        clrDark = QColor(201, 217, 239);
    // [1]
    DrawHelpers::drawGradientFill(*p, opt->rect, clrLight, clrDark, true);

    QPixmap soImage = cached("TooltipFrame.png");
    if (!soImage.isNull())
    {
        drawImage(soImage, *p, opt->rect, sourceRectImage(soImage.rect(), 0, 1), QRect(QPoint(3, 3), QPoint(3, 3)), QColor(0xFF, 0, 0xFF));
    }
    else
    {
        const QPen oldPen = p->pen();
        p->setPen(d.m_clrTooltipBorder);
        p->drawRect(opt->rect.adjusted(0, 0, -1, -1));
        p->setPen(oldPen);
    }
    return true; 
}

/*! \internal */
bool OfficePaintManager::drawControlEdit(const QStyleOption* opt, QPainter* p, const QWidget* widget) const
{
    QTN_D_STYLE(OfficeStyle)
    if (const QStyleOptionFrame* panel = qstyleoption_cast<const QStyleOptionFrame*>(opt)) 
    {
        bool usePalette = false;
        uint resolve_mask = panel->palette.resolve();

        if (widget) 
        {
            // Since spin box includes a line edit we need to resolve the palette mask also from
            // the parent, as while the color is always correct on the palette supplied by panel,
            // the mask can still be empty. If either mask specifies custom base color, use that.
            if (QAbstractSpinBox* spinbox = qobject_cast<QAbstractSpinBox*>(widget->parentWidget()))
                resolve_mask |= spinbox->palette().resolve();
        }

        if (resolve_mask & (1 << QPalette::Base)) 
            usePalette = true;

        if (!usePalette && widget && qobject_cast<QComboBox*>(widget->parentWidget()))
            return true;

        bool enabled   = opt->state & QStyle::State_Enabled;
        bool highlight = opt->state & QStyle::State_MouseOver;
        bool hasFocus  = opt->state & QStyle::State_HasFocus;
        QRect rect(panel->rect);

        if (usePalette)
            p->fillRect(rect, panel->palette.brush(QPalette::Base));
        else
            p->fillRect(rect, enabled && (highlight || hasFocus) ? panel->palette.brush(QPalette::Base) : 
                enabled ? d.m_clrControlEditNormal : panel->palette.brush(QPalette::Base));

        if ( panel->lineWidth > 0 )
            baseStyle()->proxy()->drawPrimitive(QStyle::PE_FrameLineEdit, panel, p, widget);
        return true;
    }
    return false;
}

/*! \internal */
bool OfficePaintManager::drawFrameLineEdit(const QStyleOption* opt, QPainter* p, const QWidget* w) const
{
    Q_UNUSED(w);
    QTN_D_STYLE(OfficeStyle)
    if (const QStyleOptionFrame* panel = qstyleoption_cast<const QStyleOptionFrame*>(opt))
    {
        bool usePalette = false;
        uint resolve_mask = panel->palette.resolve();
        if (resolve_mask & (1 << QPalette::Base)) 
            usePalette = true;

        if (usePalette)
        {
            // we try to check if this lineedit is a delegate on a derived class.
            QPen oldPen = p->pen();
            // Inner white border
            p->setPen(QPen(opt->palette.base().color(), 1));
            p->drawRect(opt->rect.adjusted(1, 1, -2, -2));
            // Outer dark border
            p->setPen(QPen(opt->palette.shadow().color(), 1));
            p->drawRect(opt->rect.adjusted(0, 0, -1, -1));
            p->setPen(oldPen);
        }
        else
        {
            p->save();
            QRegion clipRegion = opt->rect;
            clipRegion -= panel->rect.adjusted(2, 2, -2, -2);
            p->setClipRegion(clipRegion);
            p->setPen(d.m_clrEditCtrlBorder);
            p->drawRect(panel->rect.adjusted(0, 0, -1, -1));
            p->restore();
        }
        return true;
    }
    return false;
}

/*! \internal */
bool OfficePaintManager::drawComboBox(const QStyleOptionComplex* opt, QPainter* p, const QWidget* w) const
{
    QTN_D_STYLE(OfficeStyle)
    if (const QStyleOptionComboBox* cmb = qstyleoption_cast<const QStyleOptionComboBox*>(opt))
    {
        QStyle::State flags = opt->state;
        QStyle::SubControls sub = opt->subControls;
        QRect r = opt->rect;

        bool enabled  = flags & QStyle::State_Enabled;
        bool highlight = flags & QStyle::State_MouseOver;
        bool dropped  = cmb->activeSubControls == QStyle::SC_ComboBoxArrow && ((cmb->state & QStyle::State_On) || (cmb->state & QStyle::State_Sunken));
        bool dropButtonHot = cmb->activeSubControls == QStyle::SC_ComboBoxArrow && (cmb->state & QStyle::State_MouseOver);
        bool hasFocus = opt->state & QStyle::State_HasFocus;

        if (cmb->frame) 
        {
            r.adjust(0, 0, -1, -1);

            bool usePalette = false;
            uint resolve_mask = cmb->palette.resolve();
            if (resolve_mask & (1 << QPalette::Base)) 
                usePalette = true;

            p->fillRect(cmb->rect, !usePalette && enabled && (highlight || hasFocus) ? cmb->palette.brush(QPalette::Base) :
              enabled ? d.m_clrControlEditNormal : cmb->palette.brush(QPalette::Base));

//            p->fillRect(r, enabled && (highlight || hasFocus) ? cmb->palette.brush(QPalette::Base) : 
//                enabled ? d.m_clrControlEditNormal : cmb->palette.brush(QPalette::Base));

//            p->fillRect(r, enabled && (dropped || selected || hasFocus) ? cmb->palette.brush(QPalette::Normal, QPalette::Base) : 
//                enabled ? cmb->palette.brush(QPalette::Normal, QPalette::AlternateBase) : cmb->palette.brush(QPalette::Disabled, QPalette::Base));

            QPen savePen = p->pen();
            p->setPen(enabled ? d.m_clrEditCtrlBorder : d.m_clrEditCtrlDisabledBorder);
            p->drawRect(r);
            p->setPen(savePen);
        }

        if (sub & QStyle::SC_ComboBoxArrow)
        {
            QPixmap soImage = cached("ToolbarButtonsComboDropDown.png"); 
            QRect rcBtn = baseStyle()->proxy()->subControlRect(QStyle::CC_ComboBox, opt, QStyle::SC_ComboBoxArrow, w);

            if (!enabled)
                drawImage(soImage, *p, rcBtn, sourceRectImage(soImage.rect(), 4, 5), QRect(QPoint(2, 2), QPoint(2, 2)));
            else if (dropped)
                drawImage(soImage, *p, rcBtn, sourceRectImage(soImage.rect(), 3, 5), QRect(QPoint(2, 2), QPoint(2, 2)));
            else if (highlight)
            {
                int state = !cmb->editable || dropButtonHot || hasFocus ? 2 : highlight ? 1 : 2;
                drawImage(soImage, *p, rcBtn, sourceRectImage(soImage.rect(), state, 5), QRect(QPoint(2, 2), QPoint(2, 2)));
            }
            else // NORMAL
                drawImage(soImage, *p, rcBtn, sourceRectImage(soImage.rect(), hasFocus ? 2 : 0, 5), QRect(QPoint(2, 2), QPoint(2, 2)));

            QPoint pt = rcBtn.center();     
            QRect rc(QPoint(pt.x() - 2, pt.y() - 2), QPoint(pt.x() + 3, pt.y() + 2));
            drawDropDownGlyph(p, pt, highlight, dropped, enabled, false/*bVert*/);
        }
        return true;
    }
    return false;
}

// for SpinBox
/*! \internal */
bool OfficePaintManager::drawSpinBox(const QStyleOptionComplex* opt, QPainter* p, const QWidget* w) const
{
    Q_UNUSED(w);
    if (const QStyleOptionSpinBox* sb = qstyleoption_cast<const QStyleOptionSpinBox *>(opt))
    {
        bool selected = opt->state & QStyle::State_MouseOver;
        bool enabled  = opt->state & QStyle::State_Enabled;
        bool hasFocus = opt->state & QStyle::State_HasFocus;

        if (sb->frame && (sb->subControls & QStyle::SC_SpinBoxFrame)) 
        {
            QRect r = baseStyle()->proxy()->subControlRect(QStyle::CC_SpinBox, sb, QStyle::SC_SpinBoxFrame, w);

            p->fillRect(r, enabled && (selected || hasFocus) ? sb->palette.brush(QPalette::Normal, QPalette::Base) : 
                enabled ? sb->palette.brush(QPalette::Normal, QPalette::AlternateBase) : sb->palette.brush(QPalette::Disabled, QPalette::Base));

            QPen savePen = p->pen();
            p->setPen(sb->palette.brush(QPalette::Shadow).color());
            r.adjust(0, 0, -1, -1);
            p->drawRect(r);
            p->setPen(savePen);
        }

        QStyleOptionSpinBox copy = *sb;
        copy.state = QStyle::State_None;
        copy.subControls |= QStyle::SC_SpinBoxUp;
        QRect rcTop = baseStyle()->proxy()->subControlRect(QStyle::CC_SpinBox, &copy, QStyle::SC_SpinBoxUp, w);

        QPixmap soBackground = cached("ToolbarButtonSpinArrowsVertical.png"); 

        QPixmap pImage = rcTop.width() > 12 && rcTop.height() > 12 ? cached("ControlGalleryScrollArrowGlyphs.png") :
            cached("ToolbarButtonSpinArrowGlyphs.png");

        if (soBackground.isNull() || pImage.isNull())
        {
            Q_ASSERT(false);
            return false;
        }

        copy = *sb;

        if (sb->subControls & QStyle::SC_SpinBoxUp)
        {
            bool pressedButton = false;
            bool hotButton = false;
            bool enabledIndicator = true;

            rcTop = baseStyle()->proxy()->subControlRect(QStyle::CC_SpinBox, sb, QStyle::SC_SpinBoxUp, w);

            if (!(sb->stepEnabled & QAbstractSpinBox::StepUpEnabled) || !(sb->state & QStyle::State_Enabled))
                enabledIndicator = false;
            else if (sb->activeSubControls == QStyle::SC_SpinBoxUp && (sb->state & QStyle::State_Sunken))
                pressedButton = true;
            else if (sb->activeSubControls == QStyle::SC_SpinBoxUp && (sb->state & QStyle::State_MouseOver))
                hotButton = true;

            int state = !enabled ? 4 : (hasFocus || selected) && pressedButton ? 3 : hotButton ? 2 : (hasFocus || selected) ? 1 : 0;
            if (sb->state & QStyle::State_MouseOver)
                drawImage(soBackground, *p, rcTop, sourceRectImage(soBackground.rect(), state, 10), QRect(QPoint(2, 2), QPoint(2, 2)), QColor(0xFF, 0, 0xFF));

            state = !enabledIndicator || !enabled ? 3 : selected && pressedButton ? 2 : selected ? 2 : selected ? 1 :  0;
            QRect rcSrc = sourceRectImage(pImage.rect(), state, 16);
            QSize sz(rcSrc.size());
            QRect rcDraw(QPoint(rcTop.left() + (rcTop.width()/2 - sz.width()/2), rcTop.top() + (rcTop.height()/2 - sz.height()/2)), rcSrc.size());
            drawImage(pImage, *p, rcDraw, rcSrc, QRect(QPoint(0, 0), QPoint(0, 0)), QColor(0xFF, 0, 0xFF));
        }

        if (sb->subControls & QStyle::SC_SpinBoxDown) 
        {
            bool pressedButton = false;
            bool hotButton = false;
            bool enabledIndicator = true;

            QRect rcBottom = baseStyle()->proxy()->subControlRect(QStyle::CC_SpinBox, sb, QStyle::SC_SpinBoxDown, w);

            if (!(sb->stepEnabled & QAbstractSpinBox::StepDownEnabled) || !(sb->state & QStyle::State_Enabled))
                enabledIndicator = false;
            else if (sb->activeSubControls == QStyle::SC_SpinBoxDown && (sb->state & QStyle::State_Sunken))
                pressedButton = true;
            else if (sb->activeSubControls == QStyle::SC_SpinBoxDown && (sb->state & QStyle::State_MouseOver))
                hotButton = true;

            int state = !enabled ? 9 : (hasFocus || selected) && pressedButton ? 8 : hotButton ? 7 : (hasFocus || selected) ? 6 : 5;
            if (sb->state & QStyle::State_MouseOver)
                drawImage(soBackground, *p, rcBottom, sourceRectImage(soBackground.rect(), state, 10), QRect(QPoint(2, 2), QPoint(2, 2)), QColor(0xFF, 0, 0xFF));

            state = 4 + (!enabledIndicator || !enabled ? 3 : selected && pressedButton ? 2 : selected ? 2 : selected ? 1 :  0);
            QRect rcSrc = sourceRectImage(pImage.rect(), state, 16);
            QSize sz(rcSrc.size());
            QRect rcDraw(QPoint(rcBottom.left() + (rcBottom.width()/2 - sz.width()/2), rcBottom.top() + (rcBottom.height()/2 - sz.height()/2)), rcSrc.size());
            drawImage(pImage, *p, rcDraw, rcSrc, QRect(QPoint(0, 0), QPoint(0, 0)), QColor(0xFF, 0, 0xFF));
        }
        return true;
    }
    return false;
}

// for ToolBox
/*! \internal */
bool OfficePaintManager::drawToolBoxTabShape(const QStyleOption* opt, QPainter* p, const QWidget* w) const
{
    Q_UNUSED(w);
    QTN_D_STYLE(const OfficeStyle)
    QPixmap soImage = cached("ShortcutBarItems.png"); 
    if (soImage.isNull())
    {
        Q_ASSERT(false);
        return false;
    }
    int state = 0;
    if (opt->state & QStyle::State_Sunken)
        state = 3;
    else if (opt->state & QStyle::State_MouseOver)
        state = 2;
    drawImage(soImage, *p, opt->rect, sourceRectImage(soImage.rect(), state, 4), QRect(QPoint(2, 2), QPoint(2, 2)));

    p->fillRect(opt->rect.left(), opt->rect.bottom() - 1, opt->rect.width(), 1, d.m_clrShortcutItemShadow);

    return true; 
}


// for TabBar
/*! \internal */
bool OfficePaintManager::drawTabBarTabShape(const QStyleOption* opt, QPainter* p, const QWidget* w) const
{
    if (const QStyleOptionTab* tab = qstyleoption_cast<const QStyleOptionTab*>(opt))
    {
        QPixmap soImage = tab->position == QStyleOptionTab::Beginning || tab->position == QStyleOptionTab::OnlyOneTab ? 
            cached("TabItemTopLeft.png") : cached("TabItemTop.png"); 

        if (soImage.isNull())
        {
            Q_ASSERT(false);
            return false;
        }

        bool selected = tab->state & QStyle::State_Selected;
        bool highlight = tab->state & QStyle::State_MouseOver;
        bool focused = tab->state & QStyle::State_HasFocus;
        bool pressed = tab->state & QStyle::State_Sunken;

        QRect rect = opt->rect;
        int state = 0;
        if (selected && focused)
            state = 4;
        if (selected && highlight)
            state = 4;
        else if (selected)
            state = 2;
        else if (pressed)
            state = 4;
        else if (highlight)
            state = 1;

        if (state == 0)
            return true;

        bool correct = w != 0 ? qobject_cast<QMdiArea*>(w->parentWidget()) != 0 : true;
        int borderThickness = baseStyle()->proxy()->pixelMetric(QStyle::PM_DefaultFrameWidth, opt, w)/2;

        QSize sz;
        QTransform matrix;
        switch (tab->shape)
        {
            case QTabBar::RoundedNorth:
            case QTabBar::TriangularNorth:
                {
                    if (!selected)
                        rect.adjust(0, 0, 0, correct ? 0 : -borderThickness);

                    sz = QSize(rect.width(), rect.height());
                    matrix.rotate(0.);
                    break;
                }
            case QTabBar::RoundedSouth:
            case QTabBar::TriangularSouth:
                {
                    if (!selected)
                        rect.adjust(0 , correct ? -borderThickness : borderThickness, 0, 0);

                    sz = QSize(rect.width(), rect.height());
                    matrix.rotate(-180., Qt::XAxis);
                    break;
                }
            case QTabBar::RoundedWest:
            case QTabBar::TriangularWest:
                {
                    if (!selected)
                        rect.adjust(0, 0, correct ? 0 : -borderThickness, 0);

                    sz = QSize(rect.height(), rect.width());
                    matrix.rotate(-90.);
                    matrix.rotate(180., Qt::YAxis);
                    break;
                }
            case QTabBar::RoundedEast:
            case QTabBar::TriangularEast:
                {
                    if (!selected)
                        rect.adjust(correct ? 0 : borderThickness, 0, 0, 0);

                    sz = QSize(rect.height(), rect.width());
                    matrix.rotate(90.);
                    break;
                }
            default:
                break;
        }

        QPixmap soImageRotate(sz);
        soImageRotate.fill(Qt::transparent);

        QPainter painter(&soImageRotate);
        drawPixmap(soImage, painter, QRect(QPoint(0, 0), sz), sourceRectImage(soImage.rect(), state, 5), false, QRect(QPoint(6, 6), QPoint(6, 6)));

        QPixmap resultImage = soImageRotate.transformed(matrix);
        p->drawPixmap(rect, resultImage);

        return true;
    }
    return false;
}

/*! \internal */
bool OfficePaintManager::drawIndicatorTabClose(const QStyleOption* opt, QPainter* p, const QWidget* w) const
{
    Q_UNUSED(w);
    QString strButton = "FrameTitleClose17.png";
    QRect ir = opt->rect;
    QPixmap pm = cached(strButton);

    bool active = opt->state & QStyle::State_Raised;
    bool down = opt->state & QStyle::State_Sunken;
    bool highlighted = opt->state & QStyle::State_MouseOver;

    int state = !active ? 3 : down && highlighted ? 2 : highlighted || down ? 1 : 0;
    QRect rcSrc = sourceRectImage(pm.rect(), state, 5);
    QRect rcGlyph(QPoint((ir.right() + ir.left() - rcSrc.width()+1) / 2, (ir.top() + ir.bottom() - rcSrc.height()+2) / 2), rcSrc.size());
    drawSysButton(p, ir, down, highlighted, true);
    drawImage(pm, *p, rcGlyph, rcSrc, QRect(QPoint(0, 0), QPoint(0, 0)), QColor(0xFF, 0, 0xFF));

    return true;
}

// for QForm
/*! \internal */
bool OfficePaintManager::drawShapedFrame(const QStyleOption* opt, QPainter* p, const QWidget* w) const
{
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

                            p->fillRect(opt->rect, opt->palette.light().color());

                            switch (mdiArea->tabPosition()) 
                            {
                            case QTabWidget::North:
                                rect.adjust(0, tabBar->sizeHint().height(), 0, 0);
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

                            rect.adjust(1, 1, -1, -1);
                            qDrawShadeRect(p, rect, opt->palette, true, lw, mlw);
                        }
                        ret = true;
                    }
                    break;
                }
            default:
                break;
            }
        }
    }
    return ret;
}

/*! \internal */
bool OfficePaintManager::drawFrameWindow(const QStyleOption* opt, QPainter* p, const QWidget* wd) const
{
    QTN_D_STYLE(const OfficeStyle)

    if (!wd || !qobject_cast<const QMdiSubWindow*>(wd))
    {
#if (QT_VERSION < QT_VERSION_CHECK(5, 0, 0))
        QWidget* widget = (wd && wd->parentWidget()) ? wd->parentWidget() : Q_NULL;
        if (!qobject_cast<const QWorkspace*>(widget))
            return false;
#else
        return false;
#endif
    }
    if (const QStyleOptionFrame* frm = qstyleoption_cast<const QStyleOptionFrame *>(opt))
    {
        int fwidth = frm->lineWidth + frm->midLineWidth;
        if (fwidth > 0) 
        {
            QStyle::State flags = opt->state;
            bool active = flags & QStyle::State_Active;

            QRect rc = frm->rect;
            rc.translate(-rc.topLeft());

            p->fillRect(rc.left() + 0, rc.top(), 1, rc.height(), active ? d.m_clrFrameBorderActive0 : d.m_clrFrameBorderInactive0);
            p->fillRect(rc.right(), rc.top(), 1, rc.height(), active ? d.m_clrFrameBorderActive0 : d.m_clrFrameBorderInactive0);
            p->fillRect(rc.left(), rc.bottom(), rc.width(), 1, active ? d.m_clrFrameBorderActive0 : d.m_clrFrameBorderInactive0);
        }
        return true;
    }
    return false;
}

// for ScrollBar
/*! \internal */
bool OfficePaintManager::drawScrollBar(const QStyleOptionComplex* opt, QPainter* p, const QWidget* w) const
{
    QTN_D_STYLE(const OfficeStyle)
    if ( d.isScrollBarsIgnored() )
        return false;

    if (const QStyleOptionSlider* scrollbar = qstyleoption_cast<const QStyleOptionSlider*>(opt))
    {
        // Make a copy here and reset it for each primitive.
        QStyleOptionSlider newScrollbar(*scrollbar);
        if (scrollbar->minimum == scrollbar->maximum)
            newScrollbar.state &= ~QStyle::State_Enabled; //do not draw the slider.

        QStyle::State saveFlags = scrollbar->state;

        newScrollbar.state = saveFlags;
        newScrollbar.rect = opt->rect;

        bool light = w && (qobject_cast<const RibbonGallery*>(w->parentWidget()) ||
            qobject_cast<const QDialog*>(w->topLevelWidget()) || ::isAncestor(w));

        QPixmap soImage;
        if (opt->state & QStyle::State_Horizontal) 
        {
            soImage = light ? cached("ControlGalleryScrollHorizontalLight.png")
                : cached("ControlGalleryScrollHorizontalDark.png");
            drawImage(soImage, *p, newScrollbar.rect, sourceRectImage(soImage.rect(), 0, 2),
                QRect(QPoint(0, 1), QPoint(0, 1)));
        }
        else
        {
            soImage = light ? cached("ControlGalleryScrollVerticalLight.png")
                : cached("ControlGalleryScrollVerticalDark.png");
            drawImage(soImage, *p, newScrollbar.rect, sourceRectImage(soImage.rect(), 0, 2),
                QRect(QPoint(1, 0), QPoint(1, 0)));
        }

        if (scrollbar->subControls & QStyle::SC_ScrollBarSubLine) 
        {
            newScrollbar.rect = scrollbar->rect;
            newScrollbar.state = saveFlags;
            newScrollbar.rect = baseStyle()->proxy()->subControlRect(QStyle::CC_ScrollBar, &newScrollbar, QStyle::SC_ScrollBarSubLine, w);

            if (opt->state & QStyle::State_Horizontal) 
                newScrollbar.rect.adjust(0, 1, 0, -1);
            else
                newScrollbar.rect.adjust(1, 0, -1, 0);

            if (newScrollbar.rect.isValid()) 
                baseStyle()->proxy()->drawControl(QStyle::CE_ScrollBarSubLine, &newScrollbar, p, w);
        }

        if (scrollbar->subControls & QStyle::SC_ScrollBarAddLine) 
        {
            newScrollbar.rect = scrollbar->rect;
            newScrollbar.state = saveFlags;
            newScrollbar.rect = baseStyle()->proxy()->subControlRect(QStyle::CC_ScrollBar, &newScrollbar, QStyle::SC_ScrollBarAddLine, w);

            if (opt->state & QStyle::State_Horizontal) 
                newScrollbar.rect.adjust(0, 1, 0, -1);
            else
                newScrollbar.rect.adjust(1, 0, -1, 0);

            if (newScrollbar.rect.isValid()) 
                baseStyle()->proxy()->drawControl(QStyle::CE_ScrollBarAddLine, &newScrollbar, p, w);
        }

        if (scrollbar->subControls & QStyle::SC_ScrollBarSubPage) 
        {
            newScrollbar.rect = scrollbar->rect;
            newScrollbar.state = saveFlags;
            newScrollbar.rect = baseStyle()->proxy()->subControlRect(QStyle::CC_ScrollBar, &newScrollbar, QStyle::SC_ScrollBarSubPage, w);

            if (newScrollbar.rect.isValid()) 
            {
                if (!(scrollbar->activeSubControls & QStyle::SC_ScrollBarSubPage))
                    newScrollbar.state &= ~(QStyle::State_Sunken | QStyle::State_MouseOver);
                baseStyle()->proxy()->drawControl(QStyle::CE_ScrollBarSubPage, &newScrollbar, p, w);
            }
        }

        if (scrollbar->subControls & QStyle::SC_ScrollBarAddPage) 
        {
            newScrollbar.rect = scrollbar->rect;
            newScrollbar.state = saveFlags;
            newScrollbar.rect = baseStyle()->proxy()->subControlRect(QStyle::CC_ScrollBar, &newScrollbar, QStyle::SC_ScrollBarAddPage, w);

            if (newScrollbar.rect.isValid()) 
            {
                if (!(scrollbar->activeSubControls & QStyle::SC_ScrollBarAddPage))
                    newScrollbar.state &= ~(QStyle::State_Sunken | QStyle::State_MouseOver);
                baseStyle()->proxy()->drawControl(QStyle::CE_ScrollBarAddPage, &newScrollbar, p, w);
            }
        }

        if (scrollbar->subControls & QStyle::SC_ScrollBarSlider) 
        {
            newScrollbar.rect = scrollbar->rect;
            newScrollbar.state = saveFlags;
            newScrollbar.rect = baseStyle()->proxy()->subControlRect(QStyle::CC_ScrollBar, &newScrollbar, QStyle::SC_ScrollBarSlider, w);

            if (opt->state & QStyle::State_Horizontal) 
                newScrollbar.rect.adjust(0, 1, 0, -1);
            else
                newScrollbar.rect.adjust(1, 0, -1, 0);

            if (newScrollbar.rect.isValid()) 
                baseStyle()->proxy()->drawControl(QStyle::CE_ScrollBarSlider, &newScrollbar, p, w);
        }
        return true;
    }
    return false;
}

/*! \internal */
static QRect offsetSourceRect(QRect rc, int state)
{
    rc.translate(0, (state - 1) * rc.height());
    return rc;
}

/*! \internal */
bool OfficePaintManager::drawScrollBarLine(QStyle::ControlElement element, const QStyleOption* opt, QPainter* p, const QWidget* w) const
{
    QTN_D_STYLE(const OfficeStyle)
    if ( d.isScrollBarsIgnored() )
        return false;

    if (const QStyleOptionSlider* scrollbar = qstyleoption_cast<const QStyleOptionSlider*>(opt))
    {
        bool light = w && (qobject_cast<const RibbonGallery*>(w->parentWidget()) ||
            qobject_cast<const QDialog*>(w->topLevelWidget()) || ::isAncestor(w));

        QPixmap soImage;
        if (opt->state & QStyle::State_Horizontal)
            soImage = light ? cached("ControlGalleryScrollArrowsHorizontalLight.png")
            : cached("ControlGalleryScrollArrowsHorizontalDark.png"); 
        else
            soImage = light ? cached("ControlGalleryScrollArrowsVerticalLight.png")
            : cached("ControlGalleryScrollArrowsVerticalDark.png"); 

        bool enabled  = opt->state & QStyle::State_Enabled;
        bool selected = opt->state & QStyle::State_MouseOver;
        bool pressed  = opt->state & QStyle::State_Sunken;

        int state = -1;
        if (!enabled)
            state = 0;
        else if (selected && pressed)
        {
            if(((scrollbar->activeSubControls & QStyle::SC_ScrollBarAddLine) && (element == QStyle::CE_ScrollBarAddLine)) ||
                ((scrollbar->activeSubControls & QStyle::SC_ScrollBarSubLine) && (element == QStyle::CE_ScrollBarSubLine)) )
                state = 3;
            else
                state = 1;
        }
        else if(selected)
        {
            if(((scrollbar->activeSubControls & QStyle::SC_ScrollBarAddLine) && (element == QStyle::CE_ScrollBarAddLine)) ||
                ((scrollbar->activeSubControls & QStyle::SC_ScrollBarSubLine) && (element == QStyle::CE_ScrollBarSubLine)) )
                state = 2;
            else
                state = 1;
        }

        if (state != -1) 
            drawImage(soImage, *p, opt->rect, sourceRectImage(soImage.rect(), state, 4), QRect(QPoint(3, 3), QPoint(3, 3)), QColor(0xFF, 0, 0xFF));


        QPixmap soImageArrowGlyphs;
        if (!light)
            soImageArrowGlyphs = cached("ControlGalleryScrollArrowGlyphsDark.png");

        if (soImageArrowGlyphs.isNull())
            soImageArrowGlyphs = cached("ControlGalleryScrollArrowGlyphs.png");

        int number = -1;
        if (opt->state & QStyle::State_Horizontal) 
        {
            if (element == QStyle::CE_ScrollBarAddLine)
                number = !enabled ? 16 : state != 0 ? 14 : 13;
            else
                number = !enabled ? 12 : state != 0 ? 10 : 9;
        }
        else
        {
            if (element == QStyle::CE_ScrollBarAddLine)
                number = !enabled ? 8 : state != 0 ? 6 : 5;
            else
                number = !enabled ? 4 : state != 0 ? 2 : 1;
        }
        QRect rcArrowGripperSrc(0, 0, 9, 9);
        QRect rcSrc = ::offsetSourceRect(rcArrowGripperSrc, number);
        QRect rcArrowGripper(QPoint((opt->rect.left() + opt->rect.right() - 8) / 2, (opt->rect.top() + opt->rect.bottom() - 8) / 2), QSize(9,9));

        QSize sz = baseStyle()->proxy()->sizeFromContents(QStyle::CT_ScrollBar, opt, scrollbar->rect.size(), w);   
        if (opt->state & QStyle::State_Horizontal) 
        {
            if (sz.height() % 2 == 0) //Horizontal
                rcArrowGripper.setHeight(rcArrowGripper.height()+1);
        }
        else 
        {
            if (sz.width() % 2 == 0) //for vertical
                rcArrowGripper.setWidth(rcArrowGripper.width()+1);
        }

        drawImage(soImageArrowGlyphs, *p, rcArrowGripper, rcSrc, QRect(QPoint(0, 0), QPoint(0, 0)), QColor(255, 0, 255));
        return true;
    }
    return false;
}

/*! \internal */
bool OfficePaintManager::drawScrollBarSlider(const QStyleOption* opt, QPainter* p, const QWidget* w) const
{
    QTN_D_STYLE(const OfficeStyle)
    if ( d.isScrollBarsIgnored() )
        return false;

    if (const QStyleOptionSlider* scrollbar = qstyleoption_cast<const QStyleOptionSlider*>(opt)) 
    {
        if (!opt->rect.isEmpty())
        {
            bool light = w && (qobject_cast<const RibbonGallery*>(w->parentWidget()) ||
                qobject_cast<const QDialog*>(w->topLevelWidget()) || ::isAncestor(w));

            bool enabled  = opt->state & QStyle::State_Enabled;
            bool selected = opt->state & QStyle::State_MouseOver;
            bool pressed  = opt->state & QStyle::State_Sunken;

            int state = -1;
            if (!enabled)
                state = 0;
            else if(pressed && scrollbar->activeSubControls & QStyle::SC_ScrollBarSlider)
                state = 2;
            else if(selected && scrollbar->activeSubControls & QStyle::SC_ScrollBarSlider)
                state = 1;
            else if((selected || pressed) && scrollbar->activeSubControls & QStyle::SC_ScrollBarSlider)
                state = 2;
            else
                state = 0;


            QRect rc(opt->rect);
            QPixmap scrollThumb;
            if (opt->state & QStyle::State_Horizontal)
            {
                if (!light)
                    scrollThumb = cached("ControlGalleryScrollThumbHorizontalDark.png");

                if (scrollThumb.isNull())
                    scrollThumb = cached("ControlGalleryScrollThumbHorizontal.png");
            }
            else
            {
                if (!light)
                    scrollThumb = cached("ControlGalleryScrollThumbVerticalDark.png");

                if (scrollThumb.isNull())
                    scrollThumb = cached("ControlGalleryScrollThumbVertical.png");
            }

            Q_ASSERT(!scrollThumb.isNull());

            bool show = opt->state & QStyle::State_Horizontal ? rc.width() > 7 : rc.height() > 7;
            if( !rc.isEmpty() && show )
                drawImage(scrollThumb, *p, rc, sourceRectImage(scrollThumb.rect(), state, 3), QRect(QPoint(5, 5), QPoint(5, 5)));

            QRect rcGripper(QPoint(opt->rect.center().x() - 3, opt->rect.center().y() - 3), QSize(8, 8));
            if (opt->state & QStyle::State_Horizontal) 
            {
                if (opt->rect.width() > 10)
                {
                    QPixmap soImageScrollThumbGripperHorizontal = cached("ControlGalleryScrollThumbGripperHorizontal.png");
                    Q_ASSERT(!soImageScrollThumbGripperHorizontal.isNull());
                    drawImage(soImageScrollThumbGripperHorizontal, *p, rcGripper, sourceRectImage(soImageScrollThumbGripperHorizontal.rect(), state, 3));
                }
            }
            else
            {
                if (opt->rect.height() > 10)
                {
                    QPixmap soImageScrollThumbGripperVertical = cached("ControlGalleryScrollThumbGripperVertical.png");
                    Q_ASSERT(!soImageScrollThumbGripperVertical.isNull());
                    drawImage(soImageScrollThumbGripperVertical, *p, rcGripper, sourceRectImage(soImageScrollThumbGripperVertical.rect(), state, 3));
                }
            }

        }
        return true;
    }
    return false;
}

/*! \internal */
bool OfficePaintManager::drawScrollBarPage(QStyle::ControlElement element, const QStyleOption* opt, QPainter* p, const QWidget* w) const
{
    Q_UNUSED(element);
    Q_UNUSED(w);

    QTN_D_STYLE(const OfficeStyle)
    if ( d.isScrollBarsIgnored() )
        return false;

    if (!(opt->state & QStyle::State_Sunken))
        return true;

    bool light = w && (qobject_cast<const RibbonGallery*>(w->parentWidget()) ||
        qobject_cast<const QDialog*>(w->topLevelWidget()) || ::isAncestor(w));

    if (opt->state & QStyle::State_Horizontal) 
    {
        QPixmap soImage = light ? cached("ControlGalleryScrollHorizontalLight.png") : cached("ControlGalleryScrollHorizontalDark.png"); 
        if (!soImage.isNull())
            drawImage(soImage, *p, opt->rect, sourceRectImage(soImage.rect(), 1, 2), QRect(QPoint(0, 1), QPoint(0, 1)));
    }
    else
    {
        QPixmap soImage = light ? cached("ControlGalleryScrollVerticalLight.png") : cached("ControlGalleryScrollVerticalDark.png"); 
        if (!soImage.isNull())
            drawImage(soImage, *p, opt->rect, sourceRectImage(soImage.rect(), 1, 2), QRect(QPoint(1, 0), QPoint(1, 0)));
    }
    return true;
}

// for stausBar
/*! \internal */
bool OfficePaintManager::drawPanelStatusBar(const QStyleOption* opt, QPainter* p, const QWidget* w) const
{
    Q_UNUSED(w);
    QTN_D_STYLE(const OfficeStyle)
    QRect rc = opt->rect;
    p->fillRect(QRect(QPoint(rc.left(), rc.top()), QSize(rc.width(), 1)), d.m_clrStatusBarShadow);
    // [1]
    DrawHelpers::drawGradientFill(*p, QRect(QPoint(rc.left(), rc.top() + 1), QPoint(rc.right(), rc.top() + 9)),
        d.m_palStatusBarTop.color(QPalette::Light), d.m_palStatusBarTop.color(QPalette::Dark), true);

    // [2]
    DrawHelpers::drawGradientFill(*p, QRect(QPoint(rc.left(), rc.top() + 9), QPoint(rc.right(), rc.bottom())),
        d.m_palStatusBarBottom.color(QPalette::Light), d.m_palStatusBarBottom.color(QPalette::Dark), true);
    return true;
}

// for SizeGrip
/*! \internal */
bool OfficePaintManager::drawSizeGrip(const QStyleOption* opt, QPainter* p, const QWidget* w) const
{
    Q_UNUSED(w);
    QTN_D_STYLE(const OfficeStyle)

    QPoint pt(opt->rect.right() - 3, opt->rect.bottom() - 3);
    for (int y = 0; y < 3; y++)
    {
        for (int x = 0; x < 3 - y; x++)
        {
            p->fillRect(QRect(QPoint(pt.x() + 1 - x * 4, pt.y() + 1 - y * 4), QSize(2, 2)), QColor(255, 255, 255));
            p->fillRect(QRect(QPoint(pt.x() + 0 - x * 4, pt.y() + 0 - y * 4), QSize(2, 2)), d.m_clrToolbarSeparator);
        }
    }
    return true;
}

// for ViewItem
/*! \internal */
bool OfficePaintManager::drawPanelItemViewItem(const QStyleOption* opt, QPainter* p, const QWidget* widget) const
{
    #if (QT_VERSION < QT_VERSION_CHECK(5, 7, 0))
    if (const QStyleOptionViewItemV4* vopt = qstyleoption_cast<const QStyleOptionViewItemV4*>(opt))
    #else
    if (const QStyleOptionViewItem* vopt = qstyleoption_cast<const QStyleOptionViewItem*>(opt))
    #endif
    {
        const QAbstractItemView* view = qobject_cast<const QAbstractItemView*>(widget);
        if (!view)
            return false;

        QPixmap soImage = cached("ListBox.png");
        if (soImage.isNull())
        {
            Q_ASSERT(false);
            return false;
        }

        bool enabled  = vopt->state & QStyle::State_Enabled;
        bool selected = vopt->state & QStyle::State_Selected;
        bool hasFocus = vopt->state & QStyle::State_HasFocus;
        //        bool hasFocus = vopt->state & QStyle::State_Active;
        bool hover = vopt->state & QStyle::State_MouseOver;

        QRect itemRect = baseStyle()->subElementRect(QStyle::SE_ItemViewItemFocusRect, opt, widget).adjusted(-1, 0, 1, 0);
        itemRect.setTop(vopt->rect.top());
        itemRect.setBottom(vopt->rect.bottom());

        QSize sectionSize = itemRect.size();
        if (vopt->showDecorationSelected)
            sectionSize = vopt->rect.size();

        if (view->selectionBehavior() == QAbstractItemView::SelectRows)
            sectionSize.setWidth(vopt->rect.width());

        if (view->selectionMode() == QAbstractItemView::NoSelection)
            hover = false;

        int state = -1;
        if ((selected || hover) && enabled)
            state = selected && hover? 2 : hasFocus && selected ? 1 : !hasFocus && selected ? 3 : 0;

        if (vopt->backgroundBrush.style() != Qt::NoBrush) 
        {
            p->setBrushOrigin(vopt->rect.topLeft());
            p->fillRect(vopt->rect, vopt->backgroundBrush);
        }

        if (state != -1)
        {
            if (vopt->showDecorationSelected) 
            {
                bool reverse = vopt->direction == Qt::RightToLeft;
                bool leftSection = vopt->viewItemPosition == QStyleOptionViewItemV4::Beginning;
                bool rightSection = vopt->viewItemPosition == QStyleOptionViewItemV4::End;

                if (vopt->viewItemPosition == QStyleOptionViewItemV4::OnlyOne || vopt->viewItemPosition == QStyleOptionViewItemV4::Invalid)
                {
                    QRect rect(QPoint(vopt->rect.left(), vopt->rect.top()), sectionSize);
                    drawImage(soImage, *p, rect, sourceRectImage(soImage.rect(), state, 4), QRect(QPoint(4, 4), QPoint(4, 4)));
                }
                else if (reverse ? rightSection : leftSection)
                {
                    QRect rect(QPoint(vopt->rect.left(), vopt->rect.top()), sectionSize);
                    QRect rectImage = soImage.rect();
                    rectImage.adjust(0, 0, -4, 0);
                    drawImage(soImage, *p, rect, sourceRectImage(rectImage, state, 4), QRect(QPoint(4, 4), QPoint(4, 4)));
                } 
                else if (reverse ? leftSection : rightSection) 
                {
                    QRect rectImage = soImage.rect();
                    QPixmap copyPix = soImage.copy(4, 0, rectImage.width(), rectImage.height());
                    QRect rect(QPoint(vopt->rect.left(), vopt->rect.top()), sectionSize);
                    drawImage(copyPix, *p, rect, sourceRectImage(copyPix.rect(), state, 4), QRect(QPoint(4, 4), QPoint(4, 4)));
                }
                else if (vopt->viewItemPosition == QStyleOptionViewItemV4::Middle)
                {
                    QRect rectImage = soImage.rect();
                    QPixmap copyPix = soImage.copy(4, 0, rectImage.width()-8, rectImage.height());
                    QRect rect(QPoint(vopt->rect.left(), vopt->rect.top()), sectionSize);
                    drawImage(copyPix, *p, rect, sourceRectImage(copyPix.rect(), state, 4), QRect(QPoint(4, 4), QPoint(4, 4)));
                }
            }
            else
            {
                QRect rect(QPoint(vopt->rect.left(), vopt->rect.top()), sectionSize);
                drawImage(soImage, *p, rect, sourceRectImage(soImage.rect(), state, 4), QRect(QPoint(4, 4), QPoint(4, 4)));
            }
        }
        return true;
    }
    return false;
}

/*! \internal */
bool OfficePaintManager::drawHeaderSection(const QStyleOption* opt, QPainter* p, const QWidget* w) const
{
    Q_UNUSED(w);
    QTN_D_STYLE(const OfficeStyle)

    bool columnPressed  = opt->state & QStyle::State_Sunken;

    DrawHelpers::drawGradientFill(*p, opt->rect, columnPressed ? d.m_palGradientColumnPushed.color(QPalette::Light) : d.m_palGradientColumn.color(QPalette::Light), 
        columnPressed ? d.m_palGradientColumnPushed.color(QPalette::Dark) : d.m_palGradientColumn.color(QPalette::Dark), true);

    p->fillRect(QRect(QPoint(opt->rect.left(), opt->rect.bottom()), QSize(opt->rect.width(), 1)), 
        d.m_palGradientColumn.color(QPalette::Shadow));

    p->fillRect(QRect(QPoint(opt->rect.right(), opt->rect.top()), QSize(1, opt->rect.height())), d.m_clrColumnSeparator);

    return true;
}

/*! \internal */
bool OfficePaintManager::drawHeaderEmptyArea(const QStyleOption* opt, QPainter* p, const QWidget* w) const
{
    Q_UNUSED(w);
    QTN_D_STYLE(const OfficeStyle)

    DrawHelpers::drawGradientFill(*p, opt->rect, d.m_palGradientColumn.color(QPalette::Light), 
        d.m_palGradientColumn.color(QPalette::Dark), true);

    p->fillRect(QRect(QPoint(opt->rect.left(), opt->rect.bottom()), QSize(opt->rect.width(), 1)), 
        d.m_palGradientColumn.color(QPalette::Shadow));

    p->fillRect(QRect(QPoint(opt->rect.right(), opt->rect.top()), QSize(1, opt->rect.height())), 
        d.m_clrColumnSeparator);

    return true;
}

/*! \internal */
bool OfficePaintManager::drawIndicatorHeaderArrow(const QStyleOption* opt, QPainter* p, const QWidget* w) const
{
    Q_UNUSED(w);
    QTN_D_STYLE(const OfficeStyle)
    if (const QStyleOptionHeader* header = qstyleoption_cast<const QStyleOptionHeader*>(opt)) 
    {
        QPoint pt(opt->rect.center());
        if (header->sortIndicator == QStyleOptionHeader::SortUp) 
        {
            DrawHelpers::drawTriangle(*p, QPoint(pt.x() - 4, pt.y() - 2),
                QPoint(pt.x(), pt.y() + 2), QPoint(pt.x() + 4, pt.y()  - 2), d.m_clrColumnSeparator);
        } 
        else if (header->sortIndicator == QStyleOptionHeader::SortDown) 
        {
            DrawHelpers::drawTriangle(*p, QPoint(pt.x() - 4, pt.y() + 2),
                QPoint(pt.x(), pt.y() - 2), QPoint(pt.x() + 4, pt.y()  + 2), d.m_clrColumnSeparator);
        }
        return true;
    }
    return false;
}

/*! \internal */
bool OfficePaintManager::drawIndicatorArrow(QStyle::PrimitiveElement pe, const QStyleOption* opt, QPainter* p, const QWidget* w) const
{
    switch(pe)
    {
        case QStyle::PE_IndicatorArrowUp:
        case QStyle::PE_IndicatorArrowDown:
        case QStyle::PE_IndicatorArrowRight:
        case QStyle::PE_IndicatorArrowLeft:
            {
                if (pe == QStyle::PE_IndicatorArrowRight && qstyleoption_cast<const QStyleOptionMenuItem*>(opt))
                {
                    QPixmap soIndicator = cached("MenuPopupGlyph.png");
                    QRect rcSrc = sourceRectImage(soIndicator.rect(), opt->state & QStyle::State_Enabled ? 0 : 1, 2);
                    int size = rcSrc.height();
                    int xOffset = opt->rect.x() + (opt->rect.width() -  size)/2;
                    int yOffset = opt->rect.y() + (opt->rect.height() - size)/2;

                    QRect rcEntry(QPoint(xOffset, yOffset), rcSrc.size());
                    drawImage(soIndicator, *p, rcEntry, rcSrc, QRect(QPoint(0, 0), QPoint(0, 0)), QColor(0xFF, 0, 0xFF));
                    return true;
                }
                else if (pe == QStyle::PE_IndicatorArrowDown)
                {
                    if (qobject_cast<const QToolButton*>(w) && !qobject_cast<const QTabBar*>(w->parentWidget()))
                    {
                        QPixmap soIndicator = cached("ToolbarButtonDropDownGlyph.png");
                        bool enabled = opt->state & QStyle::State_Enabled;    
                        bool selected = opt->state & QStyle::State_Selected;
                        QRect rcSrc = sourceRectImage(soIndicator.rect(), !enabled ? 3 : selected ? 1 : 0, 4);
                        QRect rcEntry(QPoint(opt->rect.topLeft()), rcSrc.size());
                        rcEntry.translate(opt->rect.width()/2, 
                            (opt->rect.height() - rcSrc.height()) / 2 + ((opt->rect.height() - rcSrc.height()) % 2));
                        drawImage(soIndicator, *p, rcEntry, rcSrc, QRect(QPoint(0, 0), QPoint(0, 0)), QColor(0xFF, 0, 0xFF));
                        return true;
                    }
                }
            }
            break;
        default:
            return false;
    }
    return false;
}

/*! \internal */
bool OfficePaintManager::drawGalleryToolButton(const QStyleOption* opt, QPainter* p, const QWidget* widget) const
{
    if (const QStyleOptionToolButton* toolbutton = qstyleoption_cast<const QStyleOptionToolButton*>(opt))
    {
        if (widget)
        {
            QPixmap soImage;
            if(widget->property(_qtn_PopupButtonGallery).toBool())
                soImage = cached("ControlGalleryPopup.png");
            else if(widget->property(_qtn_ScrollUpButtonGallery).toBool())
                soImage = cached("ControlGalleryUp.png");
            else if(widget->property(_qtn_ScrollDownButtonGallery).toBool())
                soImage = cached("ControlGalleryDown.png");

            if (!soImage.isNull())
            {
                bool enabled  = opt->state & QStyle::State_Enabled;
                bool selected = opt->state & QStyle::State_MouseOver;
                bool pressed  = opt->state & QStyle::State_Sunken;
                //                bool popuped  = (toolbutton->activeSubControls & QStyle::SC_ToolButtonMenu) && (opt->state & State_Sunken);
            #define CALCSTATE \
                (!enabled ? 4 : pressed ? 3 : selected ? 2 : selected || pressed ? 1 : 0)

                drawImage(soImage, *p, toolbutton->rect, sourceRectImage(soImage.rect(), CALCSTATE, 5), QRect(QPoint(3, 3), QPoint(3, 3)), QColor(0xFF, 0, 0xFF));
                return true;
            }
        }
    }
    return false;
}

/*! \internal */
void OfficePaintManager::drawRectangle(QPainter* p, const QRect& rect, bool selected, bool pressed, bool enabled, bool checked, bool popuped,
                                       BarType barType, BarPosition barPos) const
{
    QTN_D_STYLE(const OfficeStyle);
    if (popuped)
    {
        // [1]
        DrawHelpers::drawGradientFill(*p, rect, d.m_clrPopupControl.color(QPalette::Light), d.m_clrPopupControl.color(QPalette::Dark),  true);
        DrawHelpers::draw3DRect(*p, d.m_clrMenubarBorder, d.m_clrMenubarBorder, rect.x(), rect.y(), rect.width()-1, rect.height()-1, true);
    }
    else if (barType != TypePopup)
    {
        if (!enabled)
        {
            if (/*isKeyboardSelected(selected)*/false)
            {
                // [1]
                DrawHelpers::drawGradientFill(*p, rect, checked ? d.m_palLunaPushed.color(QPalette::Light) : d.m_palLunaSelected.color(QPalette::Light), 
                    checked ? d.m_palLunaPushed.color(QPalette::Dark) : d.m_palLunaSelected.color(QPalette::Dark), true);
                DrawHelpers::draw3DRect(*p, d.m_clrHighlightPushedBorder, d.m_clrHighlightPushedBorder, rect.x(), rect.y(), rect.width()-1, rect.height()-1, true);
            }
            else if (checked)
            {
                // [1]
                DrawHelpers::drawGradientFill(*p, rect, d.m_palLunaChecked.color(QPalette::Light), d.m_palLunaChecked.color(QPalette::Dark), true);
                DrawHelpers::draw3DRect(*p, d.m_clrHighlightDisabledBorder, d.m_clrHighlightDisabledBorder, rect.x(), rect.y(), rect.width()-1, rect.height()-1, true);
            }
            return;
        }
        //        else if (checked && !selected && !pressed)
        //        {
        //            DrawHelpers::drawRectangle(*p, rect.adjusted(0, 0, -1, -1), m_clrHighlightPushedBorder, m_clr3DFace);
        //        }
        else if (checked && !selected && !pressed) 
        {
            // [1]
            DrawHelpers::drawGradientFill(*p, rect, d.m_palLunaChecked.color(QPalette::Light), d.m_palLunaChecked.color(QPalette::Dark), true);
            DrawHelpers::draw3DRect(*p, d.m_clrHighlightCheckedBorder, d.m_clrHighlightCheckedBorder, rect.x(), rect.y(), rect.width()-1, rect.height()-1, true);
        }
        else if (checked && selected && !pressed) 
        {
            // [1]
            DrawHelpers::drawGradientFill(*p, rect, d.m_palLunaPushed.color(QPalette::Light), d.m_palLunaPushed.color(QPalette::Dark),  true);
            DrawHelpers::draw3DRect(*p, d.m_clrHighlightPushedBorder, d.m_clrHighlightPushedBorder, rect.x(), rect.y(), rect.width()-1, rect.height()-1, true);
        }
        else if (/*isKeyboardSelected(pressed) ||*/ (selected && pressed)) 
        {
            // [1]
            DrawHelpers::drawGradientFill(*p, rect, d.m_palLunaPushed.color(QPalette::Light), d.m_palLunaPushed.color(QPalette::Dark), true);
            DrawHelpers::draw3DRect(*p, d.m_clrHighlightPushedBorder, d.m_clrHighlightPushedBorder, rect.x(), rect.y(), rect.width()-1, rect.height()-1, true);
        }
        else if (selected || pressed) 
        {
            // [1]
            DrawHelpers::drawGradientFill(*p, rect, d.m_palLunaSelected.color(QPalette::Light), d.m_palLunaSelected.color(QPalette::Dark), true);
            DrawHelpers::draw3DRect(*p, d.m_clrHighlightBorder, d.m_clrHighlightBorder, rect.x(), rect.y(), rect.width()-1, rect.height()-1, true);
        }
    }
    else //if (barPos == BarPopup && selected && barType == TypePopup)
    {
        QRect rc = rect;
        rc.adjust(0, 0, -1, -1);
        if (!enabled)
        {
            if (/*isKeyboardSelected(bSelected)*/false)
                DrawHelpers::drawRectangle(*p, rc, d.m_clrHighlightBorder, barPos != BarPopup ? (checked ? d.m_clrHighlightPushed : d.m_clrHighlight) : d.m_clrMenubarFace);
            else if (checked)
                DrawHelpers::drawRectangle(*p, rc, d.m_clrHighlightDisabledBorder, barPos != BarPopup ? d.m_clrHighlightChecked : QColor());
            return;
        }
        if (popuped) 
            DrawHelpers::drawRectangle(*p, rc, d.m_clrMenubarBorder, d.m_clrToolbarFace);
        else if (checked && !selected && !pressed) 
            DrawHelpers::drawRectangle(*p, rc, d.m_clrHighlightCheckedBorder, d.m_clrHighlightChecked);
        else if (checked && selected && !pressed) 
            DrawHelpers::drawRectangle(*p, rc, d.m_clrHighlightPushedBorder, d.m_clrHighlightPushed);
        else if (/*isKeyboardSelected(bPressed) ||*/ (selected && pressed)) 
            DrawHelpers::drawRectangle(*p, rc, d.m_clrHighlightPushedBorder, d.m_clrHighlightPushed);
        else if (selected || pressed) 
            DrawHelpers::drawRectangle(*p, rc, d.m_clrHighlightBorder, d.m_clrHighlight);
    }
}

/*! \internal */
void OfficePaintManager::setupPalette(QWidget* widget) const
{
    QTN_D_STYLE(const OfficeStyle);
    if (QMdiArea* mdiArea = qobject_cast<QMdiArea*>(widget))
    {
        QPalette palette = widget->palette();
        QColor color = helper().getColor("TabManager", "FrameBorder");
        QColor light = color.lighter(230);
        QColor dark = color.darker(160);
        dark.setAlpha(100);
        palette.setColor(QPalette::Dark, dark);
        palette.setColor(QPalette::Light, light);
        widget->setPalette(palette);
        mdiArea->setBackground(QBrush(d.m_clrAppWorkspace));
    }
}

/*! \internal */
bool OfficePaintManager::pixelMetric(QStyle::PixelMetric pm, const QStyleOption* opt, const QWidget* w, int& ret) const
{
    Q_UNUSED(opt);
    Q_UNUSED(w);
    switch(pm)
    {
        case QStyle::PM_MenuButtonIndicator:
            if (qobject_cast<const QToolButton*>(w) && !qobject_cast<const QTabBar*>(w->parentWidget()))
            {
                ret = int(DrawHelpers::dpiScaled(qreal(iImageWidth)));
                return true;
            }
            break;
        default:
            break;
    };
    return false;
}

bool OfficePaintManager::styleHint(QStyle::StyleHint /*hint*/, const QStyleOption* /*opt*/, const QWidget* /*widget*/, QStyleHintReturn* /*returnData*/, int& /*ret*/) const
{
    return false;
}

/*! \internal */
void OfficePaintManager::drawMenuItemSeparator(const QStyleOption* opt, QPainter* p, const QWidget* widget) const
{
    QTN_D_STYLE(const OfficeStyle);
    if (const QStyleOptionMenuItem* menuitem = qstyleoption_cast<const QStyleOptionMenuItem*>(opt)) 
    {
        if (menuitem->text.isEmpty())
        {
            int x, y, w, h;
            menuitem->rect.getRect(&x, &y, &w, &h);

            // windows always has a check column, regardless whether we have an icon or not
            const int iconSize = baseStyle()->proxy()->pixelMetric(QStyle::PM_ToolBarIconSize, opt, widget);

            int yoff = (y-1 + h / 2);
            int xIcon = iconSize;    
            if(qobject_cast<const OfficePopupMenu*>(widget))
                xIcon = 0;

            QPen penSave = p->pen();

            p->setPen(menuitem->palette.dark().color());
            p->drawLine(x + 2 + xIcon, yoff, x + w - 4, yoff);
            p->setPen(menuitem->palette.light().color());
            p->drawLine(x + 2 + xIcon, yoff + 1, x + w - 4, yoff + 1);

            p->setPen(penSave);
        }
        else
        {
            p->save();
            DrawHelpers::drawGradientFill(*p, menuitem->rect, d.m_clrMenuGripper, d.m_clrMenuGripper, true);
            QRect rectSeparator = menuitem->rect;
            rectSeparator.setTop(rectSeparator.bottom() - 1);

            QRect rect1 = rectSeparator;
            QRect rect2;
            rect1.setTop( rect1.top() + rectSeparator.height() / 2 - 1);
            rect1.setBottom(rect1.top());
            rect2 = rect1;
            rect2.translate(0, 1);

            p->fillRect(rect1, d.m_clrMenuPopupGripperShadow);
            p->fillRect(rect2, QColor(245, 245, 245));

            QRect rectText = menuitem->rect;
            rectText.adjust(iTextMargin, 0, -iTextMargin, -iTextMargin);
//            rectText.setBottom( rectText.bottom() - 2);

            int text_flags = Qt::AlignVCenter | Qt::TextSingleLine;
            QFont font = menuitem->font;
            font.setBold(true);
            p->setFont(font);

            p->setPen(opt->state & QStyle::State_Enabled ? d.m_clrMenuPopupText : d.m_clrMenuBarGrayText);
            p->drawText(rectText, text_flags, menuitem->text);

            p->restore();
        }
    }
}

/*! \internal */
void OfficePaintManager::drawDropDownGlyph(QPainter* p, QPoint pt, bool selected, bool popuped, bool enabled, bool vert) const
{
    Q_UNUSED(vert);
    Q_UNUSED(popuped);
    QPixmap soGlyphImage = cached("ToolbarButtonDropDownGlyph.png"); 
    if (soGlyphImage.isNull())
    {
        Q_ASSERT(false);
        return;
    }
    QRect rcSrc = sourceRectImage(soGlyphImage.rect(), !enabled ? 3 : selected ? 1 : 0, 4);
    QRect rc(QPoint(pt.x() - 2, pt.y() - 2), QSize(rcSrc.size()));

    drawImage(soGlyphImage, *p, rc, rcSrc, QRect(QPoint(0, 0), QPoint(0, 0)), QColor(0xFF, 0, 0xFF));
}

void OfficePaintManager::modifyColors()
{
}


/*!
\class Qtitan::OfficePaintManager2013
\internal
*/
OfficePaintManager2013::OfficePaintManager2013(CommonStyle* baseStyle)
    : CommonPaintManager(baseStyle)
{
    loadPixmap();

    m_ImagesBlack = m_ImagesSrc;
    m_ImagesGray   = mapTo3dColors(m_ImagesSrc, QColor(0, 0, 0), QColor(128, 128, 128));
    m_ImagesDkGray = mapTo3dColors(m_ImagesSrc, QColor(0, 0, 0), QColor(72, 72, 72));
    m_ImagesLtGray = mapTo3dColors(m_ImagesSrc, QColor(0, 0, 0), QColor(192, 192, 192));
    m_ImagesWhite  = mapTo3dColors(m_ImagesSrc, QColor(0, 0, 0), QColor(255, 255, 255));
    m_ImagesBlack2 = mapTo3dColors(m_ImagesSrc, QColor(0, 0, 0), QColor(0, 0, 0));
}

OfficePaintManager2013::~OfficePaintManager2013()
{
}

OfficeStyle::Theme OfficePaintManager2013::theme() const
{
  return ((OfficeStyle*)baseStyle())->getTheme();
}

QString OfficePaintManager2013::theme(const QString& str) const
{
    return ((OfficeStyle*)baseStyle())->qtn_d().theme(str);
}

QColor OfficePaintManager2013::textColor(bool selected, bool pressed, bool enabled, bool checked, bool popuped, BarType barType, BarPosition barPosition) const
{
    return ((OfficeStyle*)baseStyle())->getTextColor(selected, pressed, enabled, checked, popuped, barType, barPosition);
}

/*! \internal */
void OfficePaintManager2013::polish(QWidget* widget)
{
    QTN_D_STYLE(OfficeStyle);

    if (qobject_cast<QAbstractSpinBox*>(widget))
    {
        QPalette pal = widget->palette();
        pal.setColor(QPalette::Disabled, QPalette::Base, m_clrControlEditDisabled);
        pal.setColor(QPalette::Normal, QPalette::AlternateBase, d.m_clrControlEditNormal);
        widget->setPalette(pal);
    }
}

QIcon OfficePaintManager2013::qtnStandardIcon(QStyle::StandardPixmap px, const QStyleOption* opt, const QWidget* wd, bool& ret ) const
{
    Q_UNUSED(px);
    Q_UNUSED(opt);
    Q_UNUSED(wd);
    ret = false;
    return QIcon();
}

bool OfficePaintManager2013::drawWorkspace(const QStyleOption* opt, QPainter* p, const QWidget* w) const
{
    Q_UNUSED(w);
    p->fillRect(opt->rect, m_clrBarFace);
    return true;
}

// for TitleBar
/*! \internal */
bool OfficePaintManager2013::drawToolBar(const QStyleOption* opt, QPainter* p, const QWidget* w) const
{
    Q_UNUSED(w);
    p->fillRect(opt->rect, opt->palette.color(QPalette::Background));
    return true;
}

/*! \internal */
QColor OfficePaintManager2013::menuLabelColor() const
{
    return m_clrBarShadow;
}

/*! \internal */
void OfficePaintManager2013::drawSysButton(QPainter* p, const QRect& rect, bool down, bool highlighted) const
{
    QTN_D_STYLE(const OfficeStyle);
    QColor color;
    if (down) 
        color = m_clrHighlightDn;
    else if (highlighted)
        color = d.m_clrHighlight;

    if ( color.isValid() )
        p->fillRect(rect, color);

    if (down && highlighted)
        DrawHelpers::draw3dRectEx(*p, rect, m_clrHighlightDn, m_clrHighlightDn);
}

/*! \internal */
bool OfficePaintManager2013::drawTitleBar(const QStyleOptionComplex* opt, QPainter* p, const QWidget* wd) const
{
    if (!wd || !qobject_cast<const QMdiSubWindow*>(wd))
    {
#if (QT_VERSION < QT_VERSION_CHECK(5, 0, 0))
        QWidget* widget = (wd && wd->parentWidget()) ? wd->parentWidget()->parentWidget() : Q_NULL;
        if (!qobject_cast<const QWorkspace*>(widget))
            return false;
#else
        return false;
#endif
    }

    if (const QStyleOptionTitleBar* tb = qstyleoption_cast<const QStyleOptionTitleBar *>(opt)) 
    {
        QRect ir;
        if (opt->subControls & QStyle::SC_TitleBarLabel) 
        {
            ir = baseStyle()->proxy()->subControlRect(QStyle::CC_TitleBar, tb, QStyle::SC_TitleBarLabel, wd);
            p->setPen(tb->palette.text().color());
            p->drawText(ir.x() + 2, ir.y(), ir.width() - 2, ir.height(), Qt::AlignLeft | Qt::AlignVCenter | Qt::TextSingleLine, tb->text);
        }

        bool down = false;
        bool highlighted = false;
        QPixmap pm;

        QStyleOption tool(0);
        tool.palette = tb->palette;

        if (tb->subControls & QStyle::SC_TitleBarCloseButton && tb->titleBarFlags & Qt::WindowSystemMenuHint) 
        {
            ir = baseStyle()->proxy()->subControlRect(QStyle::CC_TitleBar, tb, QStyle::SC_TitleBarCloseButton, wd);
            down = tb->activeSubControls & QStyle::SC_TitleBarCloseButton && (opt->state & QStyle::State_Sunken);
            highlighted = tb->activeSubControls & QStyle::SC_TitleBarCloseButton && (opt->state & QStyle::State_MouseOver);

            if ((tb->titleBarFlags & Qt::WindowType_Mask) == Qt::Tool)
                pm = baseStyle()->standardIcon(QStyle::SP_DockWidgetCloseButton, &tool, wd).pixmap(10, 10);
            else
                pm = baseStyle()->standardIcon(QStyle::SP_TitleBarCloseButton, &tool, wd).pixmap(10, 10);
            tool.rect = ir;
            tool.state = down ? QStyle::State_Sunken : QStyle::State_Raised;
            baseStyle()->proxy()->drawPrimitive(QStyle::PE_PanelButtonTool, &tool, p, wd);

            p->save();
            drawSysButton(p, ir, down, highlighted);
            baseStyle()->proxy()->drawItemPixmap(p, ir, Qt::AlignCenter, pm);
            p->restore();
        }

        if (tb->subControls & QStyle::SC_TitleBarMaxButton && 
            tb->titleBarFlags & Qt::WindowMaximizeButtonHint && !(tb->titleBarState & Qt::WindowMaximized)) 
        {
            ir = baseStyle()->proxy()->subControlRect(QStyle::CC_TitleBar, tb, QStyle::SC_TitleBarMaxButton, wd);
            down = tb->activeSubControls & QStyle::SC_TitleBarMaxButton && (opt->state & QStyle::State_Sunken);
            highlighted = tb->activeSubControls & QStyle::SC_TitleBarMaxButton && (opt->state & QStyle::State_MouseOver);

            pm = baseStyle()->standardIcon(QStyle::SP_TitleBarMaxButton, &tool, wd).pixmap(10, 10);
            tool.rect = ir;
            tool.state = down ? QStyle::State_Sunken : QStyle::State_Raised;
            baseStyle()->proxy()->drawPrimitive(QStyle::PE_PanelButtonTool, &tool, p, wd);

            p->save();
            drawSysButton(p, ir, down, highlighted);
            baseStyle()->proxy()->drawItemPixmap(p, ir, Qt::AlignCenter, pm);
            p->restore();
        }

        if (tb->subControls & QStyle::SC_TitleBarMinButton && 
            tb->titleBarFlags & Qt::WindowMinimizeButtonHint && !(tb->titleBarState & Qt::WindowMinimized)) 
        {
            ir = baseStyle()->proxy()->subControlRect(QStyle::CC_TitleBar, tb, QStyle::SC_TitleBarMinButton, wd);
            down = tb->activeSubControls & QStyle::SC_TitleBarMinButton && (opt->state & QStyle::State_Sunken);
            highlighted = tb->activeSubControls & QStyle::SC_TitleBarMinButton && (opt->state & QStyle::State_MouseOver);

            pm = baseStyle()->standardIcon(QStyle::SP_TitleBarMinButton, &tool, wd).pixmap(10, 10);
            tool.rect = ir;
            tool.state = down ? QStyle::State_Sunken : QStyle::State_Raised;
            baseStyle()->proxy()->drawPrimitive(QStyle::PE_PanelButtonTool, &tool, p, wd);

            p->save();
            drawSysButton(p, ir, down, highlighted);
            baseStyle()->proxy()->drawItemPixmap(p, ir, Qt::AlignCenter, pm);
            p->restore();
        }

        bool drawNormalButton = (tb->subControls & QStyle::SC_TitleBarNormalButton)
            && (((tb->titleBarFlags & Qt::WindowMinimizeButtonHint)
            && (tb->titleBarState & Qt::WindowMinimized))
            || ((tb->titleBarFlags & Qt::WindowMaximizeButtonHint)
            && (tb->titleBarState & Qt::WindowMaximized)));

        if (drawNormalButton) 
        {
            ir = baseStyle()->proxy()->subControlRect(QStyle::CC_TitleBar, tb, QStyle::SC_TitleBarNormalButton, wd);
            down = tb->activeSubControls & QStyle::SC_TitleBarNormalButton && (opt->state & QStyle::State_Sunken);
            highlighted = tb->activeSubControls & QStyle::SC_TitleBarNormalButton && (opt->state & QStyle::State_MouseOver);

            pm = baseStyle()->standardIcon(QStyle::SP_TitleBarNormalButton, &tool, wd).pixmap(10, 10);
            tool.rect = ir;
            tool.state = down ? QStyle::State_Sunken : QStyle::State_Raised;
            baseStyle()->proxy()->drawPrimitive(QStyle::PE_PanelButtonTool, &tool, p, wd);

            p->save();
            drawSysButton(p, ir, down, highlighted);
            baseStyle()->proxy()->drawItemPixmap(p, ir, Qt::AlignCenter, pm);
            p->restore();
        }

        if (tb->subControls & QStyle::SC_TitleBarShadeButton && tb->titleBarFlags & Qt::WindowShadeButtonHint && !(tb->titleBarState & Qt::WindowMinimized)) 
            {
                ir = baseStyle()->proxy()->subControlRect(QStyle::CC_TitleBar, tb, QStyle::SC_TitleBarShadeButton, wd);
                down = (tb->activeSubControls & QStyle::SC_TitleBarShadeButton && (opt->state & QStyle::State_Sunken));
                pm = baseStyle()->standardIcon(QStyle::SP_TitleBarShadeButton, &tool, wd).pixmap(10, 10);
                tool.rect = ir;
                tool.state = down ? QStyle::State_Sunken : QStyle::State_Raised;
                baseStyle()->proxy()->drawPrimitive(QStyle::PE_PanelButtonTool, &tool, p, wd);
                p->save();
                drawSysButton(p, ir, down, highlighted);
                baseStyle()->proxy()->drawItemPixmap(p, ir, Qt::AlignCenter, pm);
                p->restore();
        }

        if (tb->subControls & QStyle::SC_TitleBarUnshadeButton && tb->titleBarFlags & Qt::WindowShadeButtonHint && tb->titleBarState & Qt::WindowMinimized) 
        {
                ir = baseStyle()->proxy()->subControlRect(QStyle::CC_TitleBar, tb, QStyle::SC_TitleBarUnshadeButton, wd);

                down = tb->activeSubControls & QStyle::SC_TitleBarUnshadeButton  && (opt->state & QStyle::State_Sunken);
                pm = baseStyle()->standardIcon(QStyle::SP_TitleBarUnshadeButton, &tool, wd).pixmap(10, 10);
                tool.rect = ir;
                tool.state = down ? QStyle::State_Sunken : QStyle::State_Raised;
                baseStyle()->proxy()->drawPrimitive(QStyle::PE_PanelButtonTool, &tool, p, wd);
                p->save();
                drawSysButton(p, ir, down, highlighted);
                baseStyle()->proxy()->drawItemPixmap(p, ir, Qt::AlignCenter, pm);
                p->restore();
        }
        if (tb->subControls & QStyle::SC_TitleBarContextHelpButton && tb->titleBarFlags & Qt::WindowContextHelpButtonHint) 
        {
                ir = baseStyle()->proxy()->subControlRect(QStyle::CC_TitleBar, tb, QStyle::SC_TitleBarContextHelpButton, wd);

                down = tb->activeSubControls & QStyle::SC_TitleBarContextHelpButton  && (opt->state & QStyle::State_Sunken);
                pm = baseStyle()->standardIcon(QStyle::SP_TitleBarContextHelpButton, &tool, wd).pixmap(10, 10);
                tool.rect = ir;
                tool.state = down ? QStyle::State_Sunken : QStyle::State_Raised;
                baseStyle()->proxy()->drawPrimitive(QStyle::PE_PanelButtonTool, &tool, p, wd);
                p->save();
                drawSysButton(p, ir, down, highlighted);
                baseStyle()->proxy()->drawItemPixmap(p, ir, Qt::AlignCenter, pm);
                p->restore();
        }

        if (tb->subControls & QStyle::SC_TitleBarSysMenu && tb->titleBarFlags & Qt::WindowSystemMenuHint) 
        {
            ir = baseStyle()->proxy()->subControlRect(QStyle::CC_TitleBar, tb, QStyle::SC_TitleBarSysMenu, wd);
            if (!tb->icon.isNull()) 
                tb->icon.paint(p, ir);
            else 
            {
                int iconSize = baseStyle()->proxy()->pixelMetric(QStyle::PM_SmallIconSize, tb, wd);
                pm = baseStyle()->standardIcon(QStyle::SP_TitleBarMenuButton, &tool, wd).pixmap(iconSize, iconSize);
                tool.rect = ir;
                p->save();
                baseStyle()->proxy()->drawItemPixmap(p, ir, Qt::AlignCenter, pm);
                p->restore();
            }
        }
        return true;
    }
    return false;
}

/*! \internal */
bool OfficePaintManager2013::drawIndicatorToolBarSeparator(const QStyleOption* opt, QPainter* p, const QWidget* w) const
{
    Q_UNUSED(w);
    Q_UNUSED(opt);
    Q_UNUSED(p);
    return false;
}

// for DockWidget
/*! \internal */
bool OfficePaintManager2013::drawIndicatorDockWidgetResizeHandle(const QStyleOption* opt, QPainter* p, const QWidget* w) const
{
    QRect rect = opt->rect;
    QColor clrSlider = m_clrBarShadow;
    if (!(opt->state & QStyle::State_Horizontal)|| rect.height() > rect.width() * 4)
    {
        p->fillRect(opt->rect, m_clrBarFace);

        rect.setLeft(rect.center().x() - 1);
        rect.setRight(rect.center().x() + 1);
        
        if (const QMainWindow* mainWindow = qobject_cast<const QMainWindow*>(w))
        {
            QRect rcFrame = mainWindow->rect();
            QRect screen = QApplication::desktop()->availableGeometry(mainWindow);

            rect.setTop(-rcFrame.top());
            rect.setBottom(rect.top() + screen.height() + 10);
        }
        DrawHelpers::drawGradientFill4(*p, rect, m_clrBarFace, clrSlider, clrSlider, m_clrBarFace);
    }
    else
        p->fillRect(opt->rect, clrSlider);

    return true;
}

// for menuBar
/*! \internal */
bool OfficePaintManager2013::drawPanelMenu(const QStyleOption* opt, QPainter* p, const QWidget* w) const
{
    QTN_D_STYLE(const OfficeStyle)

    p->fillRect(opt->rect, d.m_clrBackgroundLight);
    if (qobject_cast<const QToolBar*>(w))
        DrawHelpers::draw3dRectEx(*p, opt->rect, d.m_clrMenubarBorder, d.m_clrMenubarBorder);
    return true;
}

/*! \internal */
bool OfficePaintManager2013::drawFrameMenu(const QStyleOption* opt, QPainter* p, const QWidget* w) const
{
    Q_UNUSED(w);
    QTN_D_STYLE(const OfficeStyle)
    QRect rect = opt->rect;
    DrawHelpers::draw3dRectEx(*p, rect, d.m_clrMenubarBorder, d.m_clrMenubarBorder);
    rect.adjust(1, 1, -1, -1);
    DrawHelpers::draw3dRectEx(*p, rect, d.m_clrBackgroundLight, d.m_clrBackgroundLight);
    QRect rectLeft(1, 1, 2, rect.bottom() - 1);
    p->fillRect(rectLeft, m_clrBarHilite);
    return true;
}

/*! \internal */
bool OfficePaintManager2013::drawIndicatorMenuCheckMark(const QStyleOption* opt, QPainter* p, const QWidget* w) const
{
    Q_UNUSED(w);
    QRect rc = opt->rect;
    bool enabled  = opt->state & QStyle::State_Enabled;
    bool highlighted = opt->state & QStyle::State_MouseOver;

    ImageIcons iImage = /*isRadio*/false ? OfficePaintManager2013::Icon_Radio : OfficePaintManager2013::Icon_Check;

    rc.adjust(0, 1, 0, -1);

    OfficePaintManager2013::ImageState imageState = OfficePaintManager2013::Gray;

    if (!enabled)
        imageState = OfficePaintManager2013::LtGray;
    else if (highlighted)
        imageState = OfficePaintManager2013::DkGray;

    drawIcon(p, rc, iImage, imageState, QSize(9, 9));
    return true;
}

// for Buttons
/*! \internal */
bool OfficePaintManager2013::drawIndicatorCheckRadioButton(QStyle::PrimitiveElement element, const QStyleOption* option, 
                                                           QPainter* p, const QWidget* widget) const
{
    Q_UNUSED(widget);
    QTN_D_STYLE(OfficeStyle)

    bool isRadio = (element == QStyle::PE_IndicatorRadioButton);

    bool enabled  = option->state & QStyle::State_Enabled;
    bool checked  = option->state & QStyle::State_On;
    bool noChange = option->state & QStyle::State_NoChange;
    bool highlighted = option->state & QStyle::State_MouseOver;
    bool pressed  = option->state & QStyle::State_Sunken;

    int stateChecked = checked ? 1 : 0;
    if (!isRadio && noChange)
        stateChecked = 2;

    QRect rc = option->rect;
    bool themeBlack = theme() == OfficeStyle::Office2016Black;

    if ( isRadio )
    {
        p->save();

        p->setRenderHint(QPainter::Antialiasing, true);
        QPainterPath circle;
        const QPointF circleCenter = rc.center() + QPoint(1, 1);
        const qreal outlineRadius = (rc.width() + (rc.width() + 1) % 2) / 2.0 - 1;
        circle.addEllipse(circleCenter, outlineRadius, outlineRadius);

        if (pressed)
            p->setBrush(m_clrHighlightDn);
        else if (pressed || highlighted)
            p->setBrush(d.m_clrHighlight);
        else
            p->setBrush(themeBlack ? QColor(37, 37, 37) : QColor(255, 255, 255));

        QColor clrBorder = !enabled ? m_clrBarShadow : (highlighted || pressed) ? m_clrHighlightDn : m_clrBarDkShadow;
        p->setPen(clrBorder);
        p->drawPath(circle);

        if (option->state & (QStyle::State_Sunken | QStyle::State_On)) 
        {
            circle = QPainterPath();
            const qreal checkmarkRadius = outlineRadius / 2.32;
            circle.addEllipse(circleCenter, checkmarkRadius, checkmarkRadius);
            QColor checkMarkColor = highlighted && enabled ? m_clrAccentText : m_clrBarDkShadow;
            if (!themeBlack)
                checkMarkColor = checkMarkColor.darker(180);

            if (themeBlack && highlighted)
                checkMarkColor = checkMarkColor.lighter();
            checkMarkColor.setAlpha(200);
            p->setPen(checkMarkColor);
            checkMarkColor.setAlpha(180);
            p->setBrush(checkMarkColor);
            p->drawPath(circle);
        }
        p->restore();
    } 
    else
    {
        p->save();

        rc = rc.adjusted(0, 1, -2, -1);
        if (pressed)
            p->fillRect(rc, m_clrHighlightDn);
        else if (pressed || highlighted)
            p->fillRect(rc, d.m_clrHighlight);
        else
            p->fillRect(rc, themeBlack ? QColor(37, 37, 37) : QColor(255, 255, 255));

        QColor clrBorder = !enabled ? m_clrBarShadow : (highlighted || pressed) ? m_clrHighlightDn : m_clrBarDkShadow;

        p->setPen(clrBorder);
        p->drawRect(rc);

        if (stateChecked == 1)
        {
            p->setRenderHint(QPainter::Antialiasing, true);
            QColor checkMarkColor = highlighted && enabled ? m_clrAccentText : m_clrBarDkShadow;
            if (!themeBlack)
                checkMarkColor = checkMarkColor.darker(180);
            if (themeBlack && highlighted)
                checkMarkColor = checkMarkColor.lighter();

            QPen checkPen = QPen(checkMarkColor, DrawHelpers::dpiScaled(1.1));
            checkMarkColor.setAlpha(210);
            p->translate(-1, 0.5);
            p->setPen(checkPen);
            p->setBrush(Qt::NoBrush);
            p->translate(0.2, 0.0);

            const int checkMarkPadding = DrawHelpers::dpiScaled(3);
            // Draw checkmark
            QPainterPath path;
            path.moveTo(1 + checkMarkPadding, rc.height() / 2.0);
            path.lineTo(rc.width() / 2.0, rc.height() - checkMarkPadding);
            path.lineTo(rc.width() - checkMarkPadding + 1.5, checkMarkPadding);
            p->drawPath(path.translated(rc.topLeft()));
//            QRect rcIndicator(QPoint(rc.x() + (rc.width() - iImageWidth)/2, rc.y() + (rc.height() - iImageHeight)/2), QSize(iImageWidth, iImageHeight));
//            drawIcon(p, rcIndicator.topLeft(), Icon_Check, highlighted ? Black2 : Gray, QSize(9, 9));
        }
        else if (stateChecked == 2)
        {
            QBrush brushTmp(Qt::darkGray, Qt::Dense4Pattern);
            p->fillRect(rc.adjusted(1,1,0,0), brushTmp);
        }
        p->restore();
    }

    return true;
}

/*! \internal */
bool OfficePaintManager2013::drawIndicatorViewItemCheck(QStyle::PrimitiveElement element, const QStyleOption* option, QPainter* painter, const QWidget* widget) const
{
    return drawIndicatorCheckRadioButton(element, option, painter, widget);
}

/*! \internal */
bool OfficePaintManager2013::drawPanelButtonCommand(const QStyleOption* opt, QPainter* p, const QWidget* w) const
{
    Q_UNUSED(w);
    QTN_D_STYLE(OfficeStyle)
    if (const QStyleOptionButton* btn = qstyleoption_cast<const QStyleOptionButton*>(opt)) 
    {
        uint resolve_mask = btn->palette.resolve();
        if (resolve_mask & (1 << QPalette::Base)) 
            return false;

        bool enabled  = opt->state & QStyle::State_Enabled;
        bool checked  = opt->state & QStyle::State_On;
        bool selected = opt->state & QStyle::State_MouseOver;
        bool pressed  = opt->state & QStyle::State_Sunken;

        if (enabled)
        {
            if (pressed)
            {
                if (!checked)
                    p->fillRect(opt->rect, m_clrHighlightDn);
            }
            else if (selected || checked || (opt->state & QStyle::State_HasFocus))
                p->fillRect(opt->rect, d.m_clrHighlight);
            else
                p->fillRect(opt->rect, m_clrBarFace);
        }
        else
            p->fillRect(opt->rect, m_clrBarFace);

        QPen penSave = p->pen();
        p->setPen(m_clrBarDkShadow);
        p->drawRect(btn->rect.adjusted(0, 0, -1, -1));
        p->setPen(penSave);
        return true;
    }
    return false;
}

// shared
/*! \internal */
bool OfficePaintManager2013::drawPanelTipLabel(const QStyleOption* opt, QPainter* p, const QWidget* w) const
{
    Q_UNUSED(w)
    p->fillRect(opt->rect, m_clrBarHilite);
    QPen oldPen = p->pen();
    p->setPen(m_clrBarShadow);
    p->drawRect(opt->rect.adjusted(0, 0, -1, -1));
    p->setPen(oldPen);
    return true;
}

/*! \internal */
bool OfficePaintManager2013::drawControlEdit(const QStyleOption* opt, QPainter* p, const QWidget* widget) const
{
    QTN_D_STYLE(OfficeStyle)
    if (const QStyleOptionFrame* panel = qstyleoption_cast<const QStyleOptionFrame*>(opt)) 
    {
        bool usePalette = false;
        uint resolve_mask = panel->palette.resolve();

        if (widget) 
        {
            // Since spin box includes a line edit we need to resolve the palette mask also from
            // the parent, as while the color is always correct on the palette supplied by panel,
            // the mask can still be empty. If either mask specifies custom base color, use that.
            if (QAbstractSpinBox* spinbox = qobject_cast<QAbstractSpinBox*>(widget->parentWidget()))
                resolve_mask |= spinbox->palette().resolve();
        }

        if (resolve_mask & (1 << QPalette::Base)) 
            usePalette = true;

        if (!usePalette && widget && qobject_cast<QComboBox*>(widget->parentWidget()))
            return true;

        bool enabled   = opt->state & QStyle::State_Enabled;
        bool highlight = opt->state & QStyle::State_MouseOver;
        bool hasFocus  = opt->state & QStyle::State_HasFocus;
        QRect rect(panel->rect);

        if (usePalette)
            p->fillRect(rect, panel->palette.brush(QPalette::Base));
        else
            p->fillRect(rect, enabled && (highlight || hasFocus) ? panel->palette.brush(QPalette::Base) : 
                enabled ? d.m_clrControlEditNormal : m_clrControlEditDisabled);

        if ( panel->lineWidth > 0 )
            baseStyle()->proxy()->drawPrimitive(QStyle::PE_FrameLineEdit, panel, p, widget);
        return true;
    }
    return false;
}

/*! \internal */
bool OfficePaintManager2013::drawFrameLineEdit(const QStyleOption* opt, QPainter* p, const QWidget* w) const
{
    Q_UNUSED(w);
    QTN_D_STYLE(OfficeStyle)
    if (const QStyleOptionFrame* panel = qstyleoption_cast<const QStyleOptionFrame*>(opt))
    {
        bool usePalette = false;
        uint resolve_mask = panel->palette.resolve();
        if (resolve_mask & (1 << QPalette::Base)) 
            usePalette = true;

        if (usePalette)
        {
            // we try to check if this lineedit is a delegate on a derived class.
            QPen oldPen = p->pen();
            // Inner white border
            p->setPen(QPen(opt->palette.base().color(), 1));
            p->drawRect(opt->rect.adjusted(1, 1, -2, -2));
            // Outer dark border
            p->setPen(QPen(opt->palette.shadow().color(), 1));
            p->drawRect(opt->rect.adjusted(0, 0, -1, -1));
            p->setPen(oldPen);
        }
        else
        {
            bool enabled     = panel->state & QStyle::State_Enabled;
            bool highlighted = panel->state & QStyle::State_MouseOver;
            bool hasFocus    = panel->state & QStyle::State_HasFocus;

            p->save();
            QRegion clipRegion = opt->rect;
            clipRegion -= panel->rect.adjusted(2, 2, -2, -2);
            p->setClipRegion(clipRegion);

            QColor colorBorder = d.m_clrEditCtrlBorder;

            if (!enabled)
                colorBorder = m_clrTextDisabled;
            else if (highlighted || hasFocus)
                colorBorder = d.m_clrAccent;

            p->setPen(colorBorder);
            p->drawRect(panel->rect.adjusted(0, 0, -1, -1));
            p->restore();
        }
        return true;
    }
    return false;
}

/*! \internal */
bool OfficePaintManager2013::drawComboBox(const QStyleOptionComplex* opt, QPainter* p, const QWidget* w) const
{
    QTN_D_STYLE(const OfficeStyle)
    if (const QStyleOptionComboBox* cmb = qstyleoption_cast<const QStyleOptionComboBox*>(opt))
    {
        QStyle::State flags = cmb->state;
        QStyle::SubControls sub = cmb->subControls;

        bool enabled  = flags & QStyle::State_Enabled;
        bool highlighted = flags & QStyle::State_MouseOver;
        bool selected = flags & QStyle::State_Selected;
        bool dropped  = cmb->activeSubControls == QStyle::SC_ComboBoxArrow && ((flags & QStyle::State_On) || (flags & QStyle::State_Sunken));
        bool dropButtonHot = cmb->activeSubControls == QStyle::SC_ComboBoxArrow && (flags & QStyle::State_MouseOver);
        bool hasFocus = flags & QStyle::State_HasFocus;

        if (cmb->frame) 
        {
            QRect r = cmb->rect.adjusted(0, 0, -1, -1);
            bool usePalette = false;
            uint resolve_mask = cmb->palette.resolve();
            if (resolve_mask & (1 << QPalette::Base)) 
                usePalette = true;

            p->fillRect(r, !usePalette && enabled && (highlighted || selected || hasFocus) ? cmb->palette.brush(QPalette::Base) :
                enabled ? d.m_clrControlEditNormal : cmb->palette.brush(QPalette::Base));

            QColor colorBorder = d.m_clrEditCtrlBorder;

            if (!enabled)
                colorBorder = m_clrTextDisabled;
            else if (selected || dropped)
                colorBorder = d.m_clrAccent;
            else if (highlighted || hasFocus)
                colorBorder = d.m_clrAccent;

            QPen savePen = p->pen();
            p->setPen(colorBorder);
            p->drawRect(r);
            p->setPen(savePen);
        }

        if (sub & QStyle::SC_ComboBoxArrow)
        {
            QRect rcBtn = baseStyle()->proxy()->subControlRect(QStyle::CC_ComboBox, opt, QStyle::SC_ComboBoxArrow, w);
            rcBtn.adjust(0, 1, -1, -1);

            QColor clrFill = m_clrBarHilite;
            if (dropButtonHot || hasFocus)
            {
                p->fillRect(rcBtn, m_clrAccentLight);
                clrFill = m_clrAccentLight;
            }
            else if (hasFocus || dropped)
            {
                p->fillRect(rcBtn, m_clrHighlightDn);
                clrFill = m_clrHighlightDn;
            }
            else
                p->fillRect(rcBtn, clrFill);

            drawIconByColor(p, OfficePaintManager2013::Icon_ArowDown, rcBtn, clrFill);
        }
        return true;
    }
    return false;
}

// for SpinBox
/*! \internal */
bool OfficePaintManager2013::drawSpinBox(const QStyleOptionComplex* opt, QPainter* p, const QWidget* w) const
{
    Q_UNUSED(w);
    QTN_D_STYLE(const OfficeStyle)

    if (const QStyleOptionSpinBox* sb = qstyleoption_cast<const QStyleOptionSpinBox *>(opt))
    {
        bool highlighted = sb->state & QStyle::State_MouseOver;
        bool enabled  = sb->state & QStyle::State_Enabled;
        bool hasFocus = sb->state & QStyle::State_HasFocus;

        if (sb->frame && (sb->subControls & QStyle::SC_SpinBoxFrame)) 
        {
            QRect r = sb->rect.adjusted(0, 0, -1, -1);
            if (theme() == OfficeStyle::Office2016Black)
                p->fillRect(r, QColor(67, 67, 67));
            else
                p->fillRect(r, enabled && (highlighted || hasFocus) ? sb->palette.brush(QPalette::Normal, QPalette::Base) : 
                    enabled ? sb->palette.brush(QPalette::Normal, QPalette::AlternateBase) : sb->palette.brush(QPalette::Disabled, QPalette::Base));

            QColor colorBorder = d.m_clrEditCtrlBorder;

            if (!enabled)
                colorBorder = m_clrTextDisabled;
            else if (highlighted || hasFocus)
                colorBorder = theme() == OfficeStyle::Office2016Black ? qRgb(140, 140, 140) : d.m_clrAccent;

            QPen savePen = p->pen();
            p->setPen(colorBorder);
            p->drawRect(r);
            p->setPen(savePen);
        }

        QStyleOptionSpinBox copy = *sb;
        copy.state = QStyle::State_None;
        copy.subControls |= QStyle::SC_SpinBoxUp;
        QRect rcTop = baseStyle()->proxy()->subControlRect(QStyle::CC_SpinBox, &copy, QStyle::SC_SpinBoxUp, w);

        copy = *sb;

        OfficePaintManager2013::ImageIcons index[2][2] = {{OfficePaintManager2013::Icon_ArowUp, OfficePaintManager2013::Icon_ArowDown}, 
            {OfficePaintManager2013::Icon_ArowRight, OfficePaintManager2013::Icon_ArowLeft}};

        if (sb->subControls & QStyle::SC_SpinBoxUp)
        {
            bool pressedButton = false;
            bool hotButton = false;
            bool enabledIndicator = true;

            rcTop = baseStyle()->proxy()->subControlRect(QStyle::CC_SpinBox, sb, QStyle::SC_SpinBoxUp, w);

            if (!(sb->stepEnabled & QAbstractSpinBox::StepUpEnabled) || !(sb->state & QStyle::State_Enabled))
                enabledIndicator = false;
            else if (sb->activeSubControls == QStyle::SC_SpinBoxUp && (sb->state & QStyle::State_Sunken))
                pressedButton = true;
            else if (sb->activeSubControls == QStyle::SC_SpinBoxUp && (sb->state & QStyle::State_MouseOver))
                hotButton = true;

            if (pressedButton || hotButton)
            {
                p->fillRect(rcTop.adjusted(0, 1, -1, -1), pressedButton ? m_clrHighlightDn : d.m_clrHighlight);
            }
            drawIcon(p, rcTop, index[false ? 1 : 0][0], enabledIndicator ? OfficePaintManager2013::Black : OfficePaintManager2013::Gray, QSize(9, 9));
        }

        if (sb->subControls & QStyle::SC_SpinBoxDown) 
        {
            bool pressedButton = false;
            bool hotButton = false;
            bool enabledIndicator = true;

            QRect rcBottom = baseStyle()->proxy()->subControlRect(QStyle::CC_SpinBox, sb, QStyle::SC_SpinBoxDown, w);

            if (!(sb->stepEnabled & QAbstractSpinBox::StepDownEnabled) || !(sb->state & QStyle::State_Enabled))
                enabledIndicator = false;
            else if (sb->activeSubControls == QStyle::SC_SpinBoxDown && (sb->state & QStyle::State_Sunken))
                pressedButton = true;
            else if (sb->activeSubControls == QStyle::SC_SpinBoxDown && (sb->state & QStyle::State_MouseOver))
                hotButton = true;

            if (pressedButton || hotButton)
            {
                p->fillRect(rcBottom.adjusted(0, 0, -1, -2), pressedButton ? m_clrHighlightDn : d.m_clrHighlight);
            }
            drawIcon(p, rcBottom, index[false ? 1 : 0][1], enabledIndicator ? OfficePaintManager2013::Black : OfficePaintManager2013::Gray, QSize(9, 9));
        }

        return true;
    }
    return false;
}

// for ToolBox
/*! \internal */
bool OfficePaintManager2013::drawToolBoxTabShape(const QStyleOption*, QPainter*, const QWidget*) const
{
    return false;
}

// for TabBar
/*! \internal */
bool OfficePaintManager2013::drawTabBarTabShape(const QStyleOption* option, QPainter* p, const QWidget* widget) const
{
    if (const QStyleOptionTab *tab = qstyleoption_cast<const QStyleOptionTab *>(option))
    {
        QRect rect = option->rect;
        QRect fillRect = rect;
        const bool isHot = tab->state & QStyle::State_MouseOver;
        const bool selected = tab->state & QStyle::State_Selected;
        bool lastTab = tab->position == QStyleOptionTab::End;
        bool firstTab = tab->position == QStyleOptionTab::Beginning;
        const bool onlyOne = tab->position == QStyleOptionTab::OnlyOneTab;
        const int borderThickness = 1;//baseStyle()->proxy()->pixelMetric(QStyle::PM_DefaultFrameWidth, option, widget);
        const int tabOverlap = baseStyle()->proxy()->pixelMetric(QStyle::PM_TabBarTabOverlap, option, widget);

        if ( tab->direction == Qt::RightToLeft && (tab->shape == QTabBar::RoundedNorth || tab->shape == QTabBar::RoundedSouth)) 
        {
            bool temp = firstTab;
            firstTab = lastTab;
            lastTab = temp;
        }

        const bool begin = firstTab || onlyOne;
        const bool end = lastTab || onlyOne;
        switch (tab->shape) 
        {
            case QTabBar::RoundedNorth:
                    rect.adjust(begin ? tabOverlap : 0, tabOverlap, end ? -tabOverlap : 0, 0); fillRect = rect;
                    fillRect.adjust(1, 1, 0, 0);
                break;
            case QTabBar::RoundedSouth:
                    rect.adjust(begin ? tabOverlap : 0, -tabOverlap, end ? -tabOverlap : 0, -tabOverlap); fillRect = rect;
                    fillRect.adjust(1, 0, 0, -1);
                break;
            case QTabBar::RoundedEast:
                    rect.adjust(-1, begin ? tabOverlap : 0, -tabOverlap, end ? -tabOverlap : 0); fillRect = rect;
                    fillRect.adjust(1, 1, 0, 0);
                break;
            case QTabBar::RoundedWest:
                    rect.adjust(tabOverlap, begin ? tabOverlap : 0, 0, end ? -tabOverlap : 0); fillRect = rect;
                    fillRect.adjust(1, 1, 0, 0);
                break;
            default:
                break;
        }

        if (selected)
        {
            QPen savePen = p->pen();
            p->setPen(QPen(m_clrBarDkShadow, borderThickness));
            p->drawRect(rect);
            p->setPen(savePen);
        }

        if (isHot && !selected)
            p->fillRect(rect.adjusted(1, 1, -1, -1), m_clrHighlightMenuItem);
        else 
        if (selected)
            p->fillRect(fillRect, m_clrBarFace);

    }
    return true;
}

/*! \internal */
bool OfficePaintManager2013::drawIndicatorTabClose(const QStyleOption* opt, QPainter* p, const QWidget* w) const
{
    Q_UNUSED( w );
    if ((opt->state & QStyle::State_Enabled) && (opt->state & QStyle::State_MouseOver))
        p->fillRect(opt->rect.adjusted(1, 1, 0, 0), m_clrHighlightMenuItem);

    ImageIcons iImage = OfficePaintManager2013::Icon_Close;

    OfficePaintManager2013::ImageState imageState = OfficePaintManager2013::Gray;
    if (!(opt->state & QStyle::State_Enabled))
        imageState = OfficePaintManager2013::LtGray;
    else if (opt->state & QStyle::State_MouseOver)
        imageState = OfficePaintManager2013::DkGray;
    drawIcon(p, opt->rect, iImage, imageState, QSize(9, 9));

    return true;
}

// for QForm
/*! \internal */
bool OfficePaintManager2013::drawShapedFrame(const QStyleOption* opt, QPainter* p, const QWidget* w) const
{
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
                            p->fillRect(opt->rect, m_clrBarFace);

                            switch (mdiArea->tabPosition()) 
                            {
                            case QTabWidget::North:
                                rect.adjust(0, tabBar->sizeHint().height(), 0, 0);
                                break;
                            case QTabWidget::South:
                                rect.adjust(0, 0, 0, -tabBar->sizeHint().height());
                                break;
                            case QTabWidget::West:
                                rect.adjust(tabBar->sizeHint().width(), 0, 0, 0);
                                break;
                            case QTabWidget::East:
                                rect.adjust(0, 0, -tabBar->sizeHint().width(), 0);
                                break;
                            default:
                                break;
                            }
                            DrawHelpers::draw3dRectEx(*p, rect, m_clrBarDkShadow, m_clrBarDkShadow);
                        }
                        ret = true;
                    }
                    break;
                }
            default:
                break;
            }
        }
    }
    return ret;
}

/*! \internal */
bool OfficePaintManager2013::drawFrameWindow(const QStyleOption* opt, QPainter* p, const QWidget* wd) const
{
    QTN_D_STYLE(const OfficeStyle)
    if (!wd || !qobject_cast<const QMdiSubWindow*>(wd))
    {
#if (QT_VERSION < QT_VERSION_CHECK(5, 0, 0))
        QWidget* widget = (wd && wd->parentWidget()) ? wd->parentWidget() : Q_NULL;
        if (!qobject_cast<const QWorkspace*>(widget))
            return false;
#else
        return false;
#endif
    }

    QStyle::State flags = opt->state;
    QColor color;

    if (flags & QStyle::State_Active)
        color = d.m_clrFrameBorderActive0;
    else
        color = d.m_clrFrameBorderInactive0;

    int fwidth = baseStyle()->proxy()->pixelMetric(QStyle::PM_MdiSubWindowFrameWidth, 0, wd) - 1;
    // WP_FRAMELEFT;
    QRect rect = QRect(opt->rect.x(), opt->rect.y() + fwidth, opt->rect.x() + fwidth, opt->rect.height() - fwidth);
    p->fillRect(rect, color);

    // WP_FRAMERIGHT;
    rect = QRect(opt->rect.width() - fwidth, opt->rect.y() + fwidth, fwidth, opt->rect.height());
    p->fillRect(rect, color);


    // WP_FRAMEBOTTOM;
    rect = QRect(opt->rect.x(), opt->rect.height()-fwidth, opt->rect.width(), fwidth);
    p->fillRect(rect, color);


    // WP_CAPTION;
    rect = QRect(opt->rect.x(), opt->rect.y(), opt->rect.width(), opt->rect.y()+fwidth);
    p->fillRect(rect, color);

    return true;
}

// for ScrollBar
/*! \internal */
bool OfficePaintManager2013::drawScrollBar(const QStyleOptionComplex* opt, QPainter* p, const QWidget* w) const
{
    QTN_D_STYLE(const OfficeStyle)
    if ( d.isScrollBarsIgnored() )
        return false;

    if (const QStyleOptionSlider* scrollbar = qstyleoption_cast<const QStyleOptionSlider*>(opt))
    {
        // Make a copy here and reset it for each primitive.
        QStyleOptionSlider newScrollbar(*scrollbar);
        if (scrollbar->minimum == scrollbar->maximum)
            newScrollbar.state &= ~QStyle::State_Enabled; //do not draw the slider.

        QStyle::State saveFlags = scrollbar->state;

        newScrollbar.state = saveFlags;
        newScrollbar.rect = opt->rect;

        p->fillRect(scrollbar->rect, m_clrBarShadow);

        QPen savePen = p->pen();
        p->setPen(d.m_clrMenubarBorder);
        if (scrollbar->subControls & QStyle::SC_ScrollBarSubLine) 
        {
            QRect rc = baseStyle()->proxy()->subControlRect(QStyle::CC_ScrollBar, &newScrollbar, QStyle::SC_ScrollBarSubLine, w);
            if (opt->state & QStyle::State_Horizontal)
                p->drawRect(scrollbar->rect.adjusted(rc.width() + 1, 0, -1, -1));
            else
                p->drawRect(scrollbar->rect.adjusted(0, rc.height() + 1, -1, -1));
        }

        if (scrollbar->subControls & QStyle::SC_ScrollBarAddLine) 
        {
            QRect rc = baseStyle()->proxy()->subControlRect(QStyle::CC_ScrollBar, &newScrollbar, QStyle::SC_ScrollBarAddLine, w);
            if (opt->state & QStyle::State_Horizontal)
                p->drawRect(scrollbar->rect.adjusted(0, 0, -(rc.width() + 1), -1));
            else
                p->drawRect(scrollbar->rect.adjusted(0, 0, -1, -(rc.height() + 1)));
        }
        p->setPen(savePen);

        if (scrollbar->subControls & QStyle::SC_ScrollBarSubLine) 
        {
            newScrollbar.rect = scrollbar->rect;
            newScrollbar.state = saveFlags;
            newScrollbar.rect = baseStyle()->proxy()->subControlRect(QStyle::CC_ScrollBar, &newScrollbar, QStyle::SC_ScrollBarSubLine, w);

            if (opt->state & QStyle::State_Horizontal) 
                newScrollbar.rect.adjust(0, 0, 0, -1);
            else
                newScrollbar.rect.adjust(0, 0, -1, 0);

            if (newScrollbar.rect.isValid()) 
                baseStyle()->proxy()->drawControl(QStyle::CE_ScrollBarSubLine, &newScrollbar, p, w);
        }

        if (scrollbar->subControls & QStyle::SC_ScrollBarAddLine) 
        {
            newScrollbar.rect = scrollbar->rect;
            newScrollbar.state = saveFlags;
            newScrollbar.rect = baseStyle()->proxy()->subControlRect(QStyle::CC_ScrollBar, &newScrollbar, QStyle::SC_ScrollBarAddLine, w);

            if (opt->state & QStyle::State_Horizontal) 
                newScrollbar.rect.adjust(0, 0, -1, -1);
            else
                newScrollbar.rect.adjust(0, 0, -1, -1);

            if (newScrollbar.rect.isValid()) 
                baseStyle()->proxy()->drawControl(QStyle::CE_ScrollBarAddLine, &newScrollbar, p, w);
        }

        if (scrollbar->subControls & QStyle::SC_ScrollBarSlider) 
        {
            newScrollbar.rect = scrollbar->rect;
            newScrollbar.state = saveFlags;
            newScrollbar.rect = baseStyle()->proxy()->subControlRect(QStyle::CC_ScrollBar, &newScrollbar, QStyle::SC_ScrollBarSlider, w);

            if (opt->state & QStyle::State_Horizontal) 
                newScrollbar.rect.adjust(0, 0, 0, -1);
            else
                newScrollbar.rect.adjust(0, 0, -1, 0);

            if (newScrollbar.rect.isValid()) 
                baseStyle()->proxy()->drawControl(QStyle::CE_ScrollBarSlider, &newScrollbar, p, w);
        }

        return true;
    }
    return false;
}

/*! \internal */
bool OfficePaintManager2013::drawScrollBarLine(QStyle::ControlElement element, const QStyleOption* opt, QPainter* p, const QWidget* w) const
{
    Q_UNUSED(w);

    QTN_D_STYLE(const OfficeStyle)
    if ( d.isScrollBarsIgnored() )
        return false;

    if (const QStyleOptionSlider* scrollbar = qstyleoption_cast<const QStyleOptionSlider*>(opt))
    {
        bool enabled  = opt->state & QStyle::State_Enabled;
        bool highlighted = opt->state & QStyle::State_MouseOver;
        bool pressed  = opt->state & QStyle::State_Sunken;

        bool pressButton = false;
//        bool highlightedButton = false;
        if (highlighted && pressed)
        {
            if (((scrollbar->activeSubControls & QStyle::SC_ScrollBarAddLine) && (element == QStyle::CE_ScrollBarAddLine)) ||
                ((scrollbar->activeSubControls & QStyle::SC_ScrollBarSubLine) && (element == QStyle::CE_ScrollBarSubLine)) )
                pressButton = true;
        }
/*
        else if (highlighted)
        {
            if (((scrollbar->activeSubControls & QStyle::SC_ScrollBarAddLine) && (element == QStyle::CE_ScrollBarAddLine)) ||
                ((scrollbar->activeSubControls & QStyle::SC_ScrollBarSubLine) && (element == QStyle::CE_ScrollBarSubLine)) )
                highlightedButton = true;
        }
*/
        QColor clrFill = pressButton || !enabled ? m_clrBarShadow : m_clrControl;
        p->fillRect(scrollbar->rect, clrFill);

        QPen savePen = p->pen();
        p->setPen(highlighted ? m_clrEditBoxBorder : m_clrBarDkShadow);
        p->drawRect(scrollbar->rect);
        p->setPen(savePen);

        OfficePaintManager2013::ImageIcons index;
        if (opt->state & QStyle::State_Horizontal)
            index = element == QStyle::CE_ScrollBarAddLine ? OfficePaintManager2013::Icon_ArowRightTab3d : OfficePaintManager2013::Icon_ArowLeftTab3d;
        else
            index = element == QStyle::CE_ScrollBarAddLine ? OfficePaintManager2013::Icon_ArowDownLarge : OfficePaintManager2013::Icon_ArowUpLarge;

        OfficePaintManager2013::ImageState state = enabled ? stateByColor(clrFill, true) : OfficePaintManager2013::Gray;
        drawIcon(p, opt->rect, index, state, QSize(9, 9));
        return true;
    }
    return false;
}

/*! \internal */
bool OfficePaintManager2013::drawScrollBarSlider(const QStyleOption* opt, QPainter* p, const QWidget* w) const
{
    Q_UNUSED(w);

    QTN_D_STYLE(const OfficeStyle)
    if ( d.isScrollBarsIgnored() )
        return false;

    if (const QStyleOptionSlider* scrollbar = qstyleoption_cast<const QStyleOptionSlider*>(opt)) 
    {
        if (!scrollbar->rect.isEmpty())
        {
            bool enabled     = scrollbar->state & QStyle::State_Enabled;
            bool highlighted = scrollbar->state & QStyle::State_MouseOver;
            bool pressed     = scrollbar->state & QStyle::State_Sunken;

            QRect rc(scrollbar->rect);
            bool show = scrollbar->state & QStyle::State_Horizontal ? rc.width() > 7 : rc.height() > 7;
            if (show)
            {
                QColor clrFill = pressed || !enabled ? m_clrBarShadow : m_clrBarLight;
                p->fillRect(rc, clrFill);

                QPen savePen = p->pen();
                QPen newPen(QBrush(highlighted ? m_clrEditBoxBorder : m_clrBarDkShadow), 2);
                p->setPen(newPen);
                if (opt->state & QStyle::State_Horizontal)
                    p->drawRect(rc.adjusted(0, 0, 0, 1));
                else
                    p->drawRect(rc.adjusted(0, 0, 1, 0));
                p->setPen(savePen);
            }
        }
        return true;
    }
    return false;
}

/*! \internal */
bool OfficePaintManager2013::drawScrollBarPage(QStyle::ControlElement element, const QStyleOption* opt, QPainter* p, const QWidget* w) const
{
    Q_UNUSED(element);
    Q_UNUSED(opt);
    Q_UNUSED(p);
    Q_UNUSED(w);

    QTN_D_STYLE(const OfficeStyle)
    if ( d.isScrollBarsIgnored() )
        return false;

    return true;
}

// for stausBar
/*! \internal */
bool OfficePaintManager2013::drawPanelStatusBar(const QStyleOption* opt, QPainter* p, const QWidget* w) const
{
    Q_UNUSED(w);

    QTN_D_STYLE(const OfficeStyle);
    p->fillRect(opt->rect,  d.m_clrStatusBarShadow);
    return true;
}

// for SizeGrip
/*! \internal */
bool OfficePaintManager2013::drawSizeGrip(const QStyleOption* opt, QPainter* p, const QWidget* w) const
{
    Q_UNUSED(w);
    QPoint pt(opt->rect.right() - 3, opt->rect.bottom() - 3);
    for (int y = 0; y < 3; y++)
    {
        for (int x = 0; x < 3 - y; x++)
            p->fillRect(QRect(QPoint(pt.x() + 1 - x * 4, pt.y() + 1 - y * 4), QSize(2, 2)), QColor(255, 255, 255));
    }
    return true;
}

// for ViewItem
/*! \internal */
bool OfficePaintManager2013::drawPanelItemViewItem(const QStyleOption* opt, QPainter* p, const QWidget* w) const
{
    Q_UNUSED(opt);
    Q_UNUSED(p);
    Q_UNUSED(w);
    return false;
}

/*! \internal */
bool OfficePaintManager2013::drawHeaderSection(const QStyleOption* opt, QPainter* p, const QWidget* w) const
{
    Q_UNUSED(w);
    bool isPressed = opt->state & QStyle::State_Sunken;
    bool isHighlighted = opt->state & QStyle::State_MouseOver;

    QColor clrFill  = m_clrBarFace;
    QColor clrBorder = m_clrBarShadow;

    if (isPressed)
    {
        clrFill  = m_clrHighlightDn;
        clrBorder = QColor(255, 255, 255);
    }
    else if (isHighlighted)
    {
        clrFill  = m_clrBarLight;
        clrBorder = m_clrAccentLight;
    }

    p->fillRect(opt->rect, clrFill);

    QPen penSave = p->pen();
    p->setPen(clrBorder);
    p->drawRect(opt->rect.adjusted(0, 0, -1, -1));
    p->setPen(penSave);
    return true;
}

/*! \internal */
bool OfficePaintManager2013::drawHeaderEmptyArea(const QStyleOption* opt, QPainter* p, const QWidget* w) const
{
    Q_UNUSED(w);
    p->fillRect(opt->rect, m_clrBarFace);

    p->fillRect(QRect(QPoint(opt->rect.left(), opt->rect.bottom()), QSize(opt->rect.width(), 1)), 
        m_clrBarShadow);

    p->fillRect(QRect(QPoint(opt->rect.right(), opt->rect.top()), QSize(1, opt->rect.height())), 
        m_clrBarShadow);

    return true;
}

/*! \internal */
bool OfficePaintManager2013::drawIndicatorHeaderArrow(const QStyleOption*, QPainter*, const QWidget*) const
{
    return false;
}

/*! \internal */
bool OfficePaintManager2013::drawIndicatorArrow(QStyle::PrimitiveElement pe, const QStyleOption* opt, QPainter* p, const QWidget* w) const
{
    Q_UNUSED(w);
    switch(pe)
    {
        case QStyle::PE_IndicatorArrowDown:
            if (qobject_cast<const QToolButton*>(w) && !qobject_cast<const QTabBar*>(w->parentWidget()))
            {
                QRect rc = opt->rect;
                bool enabled  = opt->state & QStyle::State_Enabled;
                bool highlighted = opt->state & QStyle::State_MouseOver;

                ImageIcons iImage = OfficePaintManager2013::Icon_ArowDown;

                OfficePaintManager2013::ImageState imageState = OfficePaintManager2013::Gray;

                if (!enabled)
                    imageState = OfficePaintManager2013::LtGray;
                else if (highlighted)
                    imageState = OfficePaintManager2013::DkGray;

                rc.translate(rc.width()/2 - 3, 0);
                drawIcon(p, rc, iImage, imageState, QSize(9, 9));
                return true;
            }

            break;
        default:
            return false;
    }
    return false;
}

/*! \internal */
bool OfficePaintManager2013::drawGalleryToolButton(const QStyleOption* opt, QPainter* p, const QWidget* widget) const
{
    QTN_D_STYLE(const OfficeStyle)
    if (const QStyleOptionToolButton* toolbutton = qstyleoption_cast<const QStyleOptionToolButton*>(opt))
    {
        if (widget)
        {
            OfficePaintManager2013::ImageIcons id = OfficePaintManager2013::Icon_Check;
            if(widget->property(_qtn_PopupButtonGallery).toBool())
                id = OfficePaintManager2013::Icon_CustomizeArowDown;
            else if(widget->property(_qtn_ScrollUpButtonGallery).toBool())
                id = OfficePaintManager2013::Icon_ArowUp;
            else if(widget->property(_qtn_ScrollDownButtonGallery).toBool())
                id = OfficePaintManager2013::Icon_ArowDown;

            if (id != OfficePaintManager2013::Icon_Check)
            {
                bool enabled     = opt->state & QStyle::State_Enabled;
                bool highlighted = opt->state & QStyle::State_MouseOver;
                bool pressed     = opt->state & QStyle::State_Sunken;

                bool black = theme() == OfficeStyle::Office2016Black;

                QPen penSave = p->pen();
                p->setPen(black ? m_clrBarLight : m_clrBarShadow);
                p->drawRect(toolbutton->rect.adjusted(0, 0, -1, -1));
                p->setPen(penSave);

                QRect rect = toolbutton->rect.adjusted(1, 1, -1, -1);
                if (pressed && enabled)
                    p->fillRect(rect, m_clrHighlightDn);
                else if ((pressed || highlighted) && enabled)
                    p->fillRect(rect, d.m_clrHighlight);
                else
                    p->fillRect(rect, black ? QColor(48, 48, 48) : QColor(255, 255, 255));

                drawIcon(p, rect, id, OfficePaintManager2013::White);
                drawIcon(p, rect, id, enabled ? OfficePaintManager2013::Black : OfficePaintManager2013::Gray);

                return true;
            }
        }
    }
    return false;
}

/*! \internal */
void OfficePaintManager2013::drawMenuItemSeparator(const QStyleOption* opt, QPainter* p, const QWidget* widget) const
{
    QTN_D_STYLE(const OfficeStyle)
    if (const QStyleOptionMenuItem* menuitem = qstyleoption_cast<const QStyleOptionMenuItem*>(opt)) 
    {
        if (menuitem->text.isEmpty())
        {
            int x, y, w, h;
            menuitem->rect.getRect(&x, &y, &w, &h);

            // windows always has a check column, regardless whether we have an icon or not
            const int iconSize = baseStyle()->proxy()->pixelMetric(QStyle::PM_ToolBarIconSize, opt, widget);
            int yoff = (y-1 + h / 2);
            int xIcon = iconSize;    
            if(qobject_cast<const OfficePopupMenu*>(widget))
                xIcon = 0;

            QPen penSave = p->pen();
            p->setPen(d.m_clrMenuPopupSeparator);
            p->drawLine(x + 2 + xIcon, yoff, x + w - 4, yoff);
            p->setPen(penSave);
        }
        else
        {
            p->save();
            p->fillRect(menuitem->rect, menuLabelColor());

            QRect rectText = menuitem->rect;
            rectText.adjust(iTextMargin, 0, -iTextMargin, -iTextMargin);

            QFont font = menuitem->font;
            font.setBold(true);
            p->setFont(font);
            p->setPen(opt->state & QStyle::State_Enabled ? d.m_clrMenuPopupText : d.m_clrMenuBarGrayText);
            p->drawText(rectText, Qt::AlignVCenter | Qt::TextSingleLine, menuitem->text);
            p->restore();
        }
    }
}

/*! \internal */
void OfficePaintManager2013::drawRectangle(QPainter* p, const QRect& rect, bool selected, bool pressed, bool enabled, bool checked, bool popuped,
    BarType barType, BarPosition barPos) const
{
    Q_UNUSED(popuped);
    Q_UNUSED(enabled);
    Q_UNUSED(pressed);
    QTN_D_STYLE(OfficeStyle)
    if (barType != TypePopup)
    {
        QRect rc = rect;
        rc.adjust(1, 1, -1, -1);
        QColor color = QColor();
        if (checked)
            color = m_clrHighlightDn;
        else if (selected)
            color = d.m_clrHighlight;

        if (color.isValid())
            p->fillRect(rc, color);

        if (checked && selected)
        {
            QPen oldPen = p->pen();
            p->setPen(m_clrHighlightChecked);
            p->drawRect(rc.adjusted(-1, -1, 0, 0));
            p->setPen(oldPen);
        }
    }
    else if (barPos == BarPopup && selected && barType == TypePopup)
    {
        QRect rc = rect;
        rc.adjust(1, 0, -1, 0);
        p->fillRect(rc, m_clrHighlightMenuItem);
        if (theme() != OfficeStyle::Office2016Black)
            DrawHelpers::draw3dRectEx(*p, rc, m_clrMenuItemBorder, m_clrMenuItemBorder);
    }
    else if (enabled && barPos == BarPopup && checked && barType == TypePopup)
    {
        QRect rc = rect;
        rc.adjust(1, 0, -1, 0);
        p->fillRect(rc, m_clrHighlightDn);
    }
}

/*! \internal */
void OfficePaintManager2013::setupPalette(QWidget* widget) const
{
    QTN_D_STYLE(OfficeStyle)
    if (qobject_cast<QToolBar*>(widget))
    {
        QPalette palette = widget->palette();
        palette.setColor(QPalette::Window, d.m_clrDockBar);
        widget->setPalette(palette);
    }
}

/*! \internal */
bool OfficePaintManager2013::pixelMetric(QStyle::PixelMetric pm, const QStyleOption* opt, const QWidget* w, int& ret) const
{
    Q_UNUSED(opt);
    Q_UNUSED(w);
    switch(pm)
    {
        case QStyle::PM_MenuButtonIndicator:
            if (qobject_cast<const QToolButton*>(w))
            {
                ret = int(DrawHelpers::dpiScaled(qreal(iImageWidth + 1)));
                return true;
            }
            break;
        case QStyle::PM_MdiSubWindowFrameWidth:
#if (QT_VERSION < QT_VERSION_CHECK(5, 0, 0))
            {
                QWidget* widget = (w && w->parentWidget()) ? w->parentWidget() : Q_NULL;
                if (qobject_cast<const QWorkspace*>(widget))
                {
                    ret = 3;
                    return true;
                }
            }
#else
            if (w && qobject_cast<const QMdiSubWindow*>(w))
            {
                ret = 3;
                return true;
            }
#endif
            break;
        default:
            break;
    };
    return false;
}

/*! \internal */
bool OfficePaintManager2013::styleHint(QStyle::StyleHint hint, const QStyleOption* opt, const QWidget* w, QStyleHintReturn* returnData, int& ret) const
{
    Q_UNUSED(opt)
    Q_UNUSED(returnData)
    switch(hint)
    {
        case QStyle::SH_TitleBar_NoBorder:
            if (w && qobject_cast<const QMdiSubWindow*>(w))
            {
                ret = false;
                return true;
            }
            break;
        default:
            break;
    };
    return false;
}

/*! \internal */
void OfficePaintManager2013::modifyColors()
{
    QTN_D_STYLE(OfficeStyle)
    m_clrAccentLight   = DrawHelpers::colorMakePale(d.m_clrAccent.rgb(), .9);
    m_clrAccentHilight = DrawHelpers::pixelAlpha(d.m_clrAccent.rgb(), 115);
    m_clrAccentText    = DrawHelpers::pixelAlpha(d.m_clrAccent.rgb(), 75);

    d.m_clrFrameBorderActive0 = m_clrAccentText;
    d.m_clrFrameBorderInactive0 = DrawHelpers::colorMakeLighter(m_clrBarDkShadow.rgb());
    d.m_clrTabSelectedText = m_clrAccentText;

    d.m_clrHighlight = m_clrAccentLight;
    d.m_clrHighlightNC = d.m_clrHighlight;
    m_clrHighlightDn = DrawHelpers::colorMakeDarker(m_clrAccentLight.rgb(), .14);
    m_clrHighlightDnNC = m_clrHighlightDn;
    m_clrHighlightChecked = DrawHelpers::colorMakeDarker(m_clrAccentLight.rgb(), .05);
    
    m_clrBarShadow   = helper().getColor("Window", "BarShadow");
    m_clrBarText     = helper().getColor("Window", "BarText");
    m_clrBarHilite   = helper().getColor("Window", "BarHilite");
    m_clrBarDkShadow = helper().getColor("Window", "BarDkShadow");
    m_clrBarFace     = helper().getColor("Window", "BarFace"); 
    m_clrBarLight    = helper().getColor("Window", "BarLight"); 


    d.m_clrStatusBarShadow   = d.m_clrAccent;
    d.m_clrControlEditNormal = m_clrBarHilite; //m_clrBarLight;
    d.m_clrNormalCaptionText = m_clrBarText;
    d.m_clrActiveCaptionText = m_clrAccentText;
    d.m_clrEditCtrlBorder    = QColor(171, 171, 171);

    d.m_clrMenubarBorder     = DrawHelpers::colorMakeLighter(m_clrBarDkShadow.rgb()); // m_clrMenuBorder
    d.m_clrStatusBarText     = QColor(255, 255, 255);
    d.m_clrBackgroundLight   = QColor(255, 255, 255); // m_clrMenuLight
    d.m_clrDockBar           = QColor(255, 255, 255); 
    d.m_clrControlGalleryLabel = m_clrBarShadow;
    d.m_crBorderShadow = m_clrBarShadow;
    d.m_clrGridLineColor = m_clrBarShadow;
    d.m_clrMenuPopupText = m_clrBarText;

    m_clrHighlightMenuItem   = m_clrAccentLight; // m_clrHighlightMenuItem
    m_clrMenuItemBorder      = d.m_clrHighlight; // m_clrMenuItemBorder 

    m_clrTextDisabled        = m_clrBarDkShadow;
    m_clrControlEditDisabled = m_clrBarFace;
    m_clrControl             = m_clrBarHilite;
    m_clrEditBoxBorder       = QColor(171, 171, 171);

    m_ImagesBlack  = mapTo3dColors(m_ImagesSrc, QColor(0, 0, 0), QColor(119, 119, 119));
    m_ImagesBlack2 = mapTo3dColors(m_ImagesSrc, QColor(0, 0, 0), m_clrAccentText);
}

/*! \internal */
OfficePaintManager2013::ImageState OfficePaintManager2013::stateByColor(const QColor& color, bool backgroundColor) const
{
    if (!color.isValid())
        return Black;

    qreal h, l, s, a;
    color.getHslF(&h, &s, &l, &a);

    if (backgroundColor)
        return l < 0.7 ? White : Black;
    else
        return l > 0.7 ? White : Black;
}

/*! \internal */
void OfficePaintManager2013::drawIcon(QPainter* p, const QRect& rectImage, ImageIcons index, ImageState state, const QSize& sizeImageDest) const
{
    const QSize sizeImage = (sizeImageDest == QSize (0, 0)) ? QSize(9, 9) : sizeImageDest;

    QPoint ptImage(rectImage.left() + (rectImage.width()  - sizeImage.width()) / 2  + ((rectImage.width()  - sizeImage.width())  % 2), 
                   rectImage.top()  + (rectImage.height() - sizeImage.height()) / 2 + ((rectImage.height() - sizeImage.height()) % 2));

    drawIcon(p, ptImage, index, state, sizeImage);
}

/*! \internal */
void OfficePaintManager2013::drawIcon(QPainter* p, const QPoint& pos, ImageIcons index, ImageState state, const QSize& sz) const
{
    QPixmap images = (state == Black)  ? m_ImagesBlack  :
        (state == Gray)   ? m_ImagesGray   : 
        (state == DkGray) ? m_ImagesDkGray : 
        (state == LtGray) ? m_ImagesLtGray : 
        (state == White)  ? m_ImagesWhite  : m_ImagesBlack2;

    QRect rcSrc(QPoint((int)index * sz.width(), 0), sz);
    p->drawPixmap(pos, images, rcSrc);
}

/*! \internal */
QPixmap OfficePaintManager2013::mapTo3dColors(const QPixmap& px, const QColor& clrSrc, const QColor& clrDest) const
{
    QImage img = px.toImage();
    for (int y = 0; y < img.height(); ++y) 
    {
        QRgb *scanLine = (QRgb*)img.scanLine(y);
        for (int x = 0; x < img.width(); ++x) 
        {
            QRgb pixel = *scanLine;
            if (pixel == clrSrc.rgb()) 
                *scanLine = clrDest.rgb();
            ++scanLine;
        }
    }
    return QPixmap::fromImage(img);
}

/*! \internal */
void OfficePaintManager2013::loadPixmap()
{
    m_ImagesSrc = QPixmap(":/res/menuimg-pro24.png");
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
    QImage img = m_ImagesSrc.toImage();
    img.setAlphaChannel(img.createMaskFromColor(clrTransparent.rgb(), Qt::MaskOutColor));
    m_ImagesSrc = QPixmap::fromImage(img);
#else
    m_ImagesSrc.setAlphaChannel(m_ImagesSrc.createMaskFromColor(clrTransparent, Qt::MaskOutColor));
#endif
}


/*!
\class Qtitan::OfficePaintManager2016
\internal
*/
OfficePaintManager2016::OfficePaintManager2016(CommonStyle* baseStyle)
    : OfficePaintManager2013(baseStyle)
{
}

OfficePaintManager2016::~OfficePaintManager2016()
{
}

/*! \internal */
QIcon OfficePaintManager2016::qtnStandardIcon(QStyle::StandardPixmap px, const QStyleOption* opt, const QWidget* wd, bool& ret ) const
{
    ret = false;
    if (OfficeStyle* bStyle = qobject_cast<OfficeStyle*>(baseStyle()))
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

/*! \internal */
bool OfficePaintManager2016::drawPanelTipLabel(const QStyleOption* opt, QPainter* p, const QWidget* wd) const
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
        return OfficePaintManager2013::drawPanelTipLabel(opt, p, wd);
}

// for editors
/*! \internal */
bool OfficePaintManager2016::drawComboBox(const QStyleOptionComplex* opt, QPainter* p, const QWidget* w) const
{
    QTN_D_STYLE(const OfficeStyle)
    if (const QStyleOptionComboBox* cmb = qstyleoption_cast<const QStyleOptionComboBox*>(opt))
    {
        QStyle::State flags = cmb->state;
        QStyle::SubControls sub = cmb->subControls;

        bool enabled = flags & QStyle::State_Enabled;
        bool selected = flags & QStyle::State_Selected;
        bool highlighted = flags & QStyle::State_MouseOver;
        bool dropped = cmb->activeSubControls == QStyle::SC_ComboBoxArrow && ((flags & QStyle::State_On) || (flags & QStyle::State_Sunken));
        bool dropButtonHot = cmb->activeSubControls == QStyle::SC_ComboBoxArrow && (flags & QStyle::State_MouseOver);
        bool hasFocus = flags & QStyle::State_HasFocus;

        bool onlyClrAccent = theme() == OfficeStyle::Office2016White;

        QColor colorBorder = d.m_clrEditCtrlBorder;
        if (!enabled)
            colorBorder = m_clrTextDisabled;
        else if (selected || dropped)
        {
            if (theme() == OfficeStyle::Office2016Black)
                colorBorder = qRgb(140, 140, 140);
            else
                colorBorder = onlyClrAccent ? d.m_clrAccent : m_clrBarDkShadow.darker(150);
        }
        else if (highlighted || hasFocus)
        {
            if (theme() == OfficeStyle::Office2016Black)
                colorBorder = qRgb(140, 140, 140);
            else
                colorBorder = onlyClrAccent ? d.m_clrAccent : m_clrBarDkShadow.darker(150);
        }

        if (cmb->frame)
        {
            QRect r = cmb->rect.adjusted(0, 0, -1, -1);
            bool usePalette = false;
            uint resolve_mask = cmb->palette.resolve();
            if (resolve_mask & (1 << QPalette::Base))
                usePalette = true;

            if (onlyClrAccent)
            {
                p->fillRect(r, !usePalette && enabled && (highlighted || selected || hasFocus) ? cmb->palette.brush(QPalette::Base) :
                    enabled ? d.m_clrControlEditNormal : cmb->palette.brush(QPalette::Base));
            }
            else
            {
                p->fillRect(r, !usePalette && enabled && (highlighted || hasFocus) ? d.m_clrControlEditNormal :
                    enabled ? d.m_clrControlEditNormal : cmb->palette.brush(QPalette::Base));
            }
            p->save();
            p->setPen(colorBorder);
            p->drawRect(r);
            p->restore();
        }

        if (sub & QStyle::SC_ComboBoxArrow)
        {
            QRect rcBtn = baseStyle()->proxy()->subControlRect(QStyle::CC_ComboBox, opt, QStyle::SC_ComboBoxArrow, w);
            rcBtn.adjust(0, 1, -1, -1);

            bool backgroundColor = true;
            QColor clrFill = m_clrBarHilite;
            if (dropButtonHot || hasFocus)
            {
                p->fillRect(rcBtn, d.m_clrHighlight);

                p->save();
                p->setPen(colorBorder);
                p->drawRect(rcBtn.adjusted(0,-1,0,0));
                p->restore();

                clrFill = QColor(Qt::black);
                backgroundColor = false;
            }
            else if (hasFocus || dropped)
            {
                p->fillRect(rcBtn, m_clrHighlightDn);

                p->save();
                p->setPen(colorBorder);
                p->drawRect(rcBtn.adjusted(0, -1, 0, 0));
                p->restore();

                clrFill = QColor(Qt::black);
                backgroundColor = false;
            }
            else
                p->fillRect(rcBtn, clrFill);

            drawIconByColor(p, OfficePaintManager2013::Icon_ArowDown, rcBtn, clrFill, backgroundColor);
        }
        return true;
    }
    return false;
}

// for SizeGrip
/*! \internal */
bool OfficePaintManager2016::drawSizeGrip(const QStyleOption* opt, QPainter* p, const QWidget* w) const
{
    Q_UNUSED(w);
    QPoint pt(opt->rect.right() - 3, opt->rect.bottom() - 3);
    for (int y = 0; y < 3; y++)
    {
        for (int x = 0; x < 3 - y; x++)
            p->fillRect(QRect(QPoint(pt.x() + 1 - x * 4, pt.y() + 1 - y * 4), QSize(2, 2)), QColor(191, 191, 191));
    }
    return true;
}

// for ToolBox
bool OfficePaintManager2016::drawToolBoxTabShape(const QStyleOption*, QPainter*, const QWidget*) const
{
    return false;
}

/*! \internal */
void OfficePaintManager2016::setupPalette(QWidget* widget) const
{
    OfficePaintManager2013::setupPalette(widget);
    QTN_D_STYLE(OfficeStyle)

    if (theme() == OfficeStyle::Office2016Black)
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
            palette.setColor(QPalette::All, QPalette::ToolTipText, d.m_clrHighlightText);
            palette.setColor(QPalette::Disabled, QPalette::ToolTipText, d.m_clrHighlightText);
            widget->setPalette(palette);
        }
        else if (qobject_cast<QMenu*>(widget))
        {
            QPalette palette = widget->palette();
            palette.setColor(QPalette::ButtonText, m_clrBarText);
            palette.setColor(QPalette::Button, d.m_clrBackgroundLight);
            widget->setPalette(palette);
        }
        else if (qobject_cast<RibbonToolBarControl*>(widget))
        {
            QPalette palette = widget->palette();
            palette.setBrush(QPalette::Background, helper().getColor("Window", "BarLight").lighter(114));
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
        else if (qobject_cast<QCheckBox*>(widget))
        {
            QColor color = helper().getColor("Window", "BarText");
            if (color.isValid())
            {
                QPalette palette = widget->palette();
                palette.setBrush(QPalette::ButtonText, color);
                palette.setBrush(QPalette::Disabled, QPalette::ButtonText, color.darker());
                widget->setPalette(palette);
            }
        }
        if (qobject_cast<QTextEdit*>(widget))
        {
            QPalette palette = widget->palette();
            palette.setColor(QPalette::Base, QColor(54, 54, 54));
            palette.setColor(QPalette::Text, QColor(255, 255, 255));
            widget->setPalette(palette);
        }
    }
    else if (theme() == OfficeStyle::Office2016DarkGray)
    {
        if (qobject_cast<QMainWindow*>(widget))
        {
            QPalette palette = widget->palette();
            palette.setColor(QPalette::Background, QColor(171, 171, 171));
            widget->setPalette(palette);
        } 
        else if (qobject_cast<QToolBar*>(widget))
        {
            QPalette palette = widget->palette();
            palette.setColor(QPalette::Window, QColor(184, 184, 184));
            widget->setPalette(palette);
        }
        else if (qobject_cast<QGroupBox*>(widget))
        {
            QPalette palette = widget->palette();
            palette.setBrush(QPalette::Dark, d.m_clrDockBar);
            palette.setBrush(QPalette::Light, helper().getColor("Window", "BarLight"));
            palette.setColor(QPalette::Foreground, QColor(240, 240, 240));
            widget->setPalette(palette);
        }
        else if (qobject_cast<QCheckBox*>(widget))
        {
            QColor color = helper().getColor("Dialog", "Foreground");
            QPalette palette = widget->palette();
            palette.setBrush(QPalette::ButtonText, color);
            palette.setBrush(QPalette::Disabled, QPalette::ButtonText, color.darker(150));
            widget->setPalette(palette);
        }
    }
}

void OfficePaintManager2016::modifyColors()
{
    OfficePaintManager2013::modifyColors();
    QTN_D_STYLE(OfficeStyle)

    if (theme() == OfficeStyle::Office2016Colorful)
    {
        d.m_clrFrameCaptionTextInActive = m_clrBarLight;
        d.m_clrFrameCaptionTextActive = m_clrBarHilite;

        d.m_clrHighlightNC = m_clrAccentHilight;
        m_clrHighlightDnNC = DrawHelpers::colorMakeDarker(d.m_clrAccent.rgb());

        d.m_clrStatusBarShadow = m_clrBarLight;
        d.m_clrHighlight = DrawHelpers::colorMakeDarker(m_clrBarFace.rgb(), 0.025);

        m_clrHighlightMenuItem = d.m_clrHighlight;
        m_clrHighlightDn = DrawHelpers::colorMakeDarker(d.m_clrHighlight.rgb(), 0.12);

        d.m_clrStatusBarText = m_clrBarText;
        d.m_clrDockBar = m_clrBarFace;

        m_ImagesBlack = mapTo3dColors(m_ImagesSrc, QColor(0, 0, 0), QColor(119, 119, 119));
    }
    else if (theme() == OfficeStyle::Office2016DarkGray)// OfficeStyle::Office2016DarkGray
    {
        d.m_clrFrameBorderActive0 = m_clrBarShadow;
        d.m_clrFrameBorderInactive0 = QColor(131, 131, 131);

        m_clrAccentLight = DrawHelpers::colorMakeDarker(d.m_clrAccent.rgb(), .75);
        m_clrTextDisabled = DrawHelpers::colorMakeDarker(m_clrBarShadow.rgb(), 1.1);

        d.m_clrFrameCaptionTextInActive = m_clrBarLight;
        d.m_clrFrameCaptionTextActive = m_clrBarHilite;
        d.m_clrEditCtrlBorder = QColor(106, 106, 106);
        d.m_clrBackgroundLight = QColor(240, 240, 240); // m_clrMenuLight

        d.m_clrHighlightNC = QColor(100, 100, 100);
        m_clrHighlightDnNC = m_clrBarDkShadow;

        d.m_clrStatusBarShadow = m_clrBarLight;
        d.m_clrDockBar = QColor(106, 106, 106);
        d.m_clrControlGalleryLabel = QColor(227, 227, 227);

        d.m_clrHighlight = DrawHelpers::colorMakeDarker(m_clrBarFace.rgb(), 0.12);
        m_clrHighlightMenuItem = QColor(205, 205, 205);
        m_clrHighlightDn = DrawHelpers::colorMakeDarker(d.m_clrHighlight.rgb(), 0.12);
        d.m_clrStatusBarText = QColor(Qt::white);

        m_ImagesBlack = mapTo3dColors(m_ImagesSrc, QColor(0, 0, 0), QColor(48, 48, 48));
        m_ImagesBlack2 = mapTo3dColors(m_ImagesSrc, QColor(0, 0, 0), QColor(0, 0, 0));
    }
    else if (theme() == OfficeStyle::Office2016Black)
    {
        m_clrTextDisabled = DrawHelpers::colorMakeDarker(m_clrBarShadow.rgb(), 1.1);
        d.m_clrEditCtrlBorder = QColor(106, 106, 106);

        d.m_clrFrameCaptionTextInActive = m_clrBarLight;
        d.m_clrFrameCaptionTextActive = QColor(255, 255, 255);

        d.m_clrHighlightNC = QColor(100, 100, 100);
        d.m_clrBackgroundLight = m_clrBarFace;
        m_clrHighlightDnNC = QColor(70, 70, 70);

        d.m_clrStatusBarShadow = DrawHelpers::colorMakeDarker(d.m_clrHighlightNC.rgb(), .15);
        d.m_clrTabSelectedText = QColor(Qt::white);

        d.m_clrHighlight = m_clrHighlightMenuItem = QColor(87, 87, 87);
        m_clrHighlightDn = m_clrBarLight;
        d.m_clrStatusBarText = QColor(Qt::white);
        d.m_clrDockBar = QColor(37, 37, 37);

        m_ImagesDkGray = mapTo3dColors(m_ImagesSrc, QColor(0, 0, 0), QColor(128, 128, 128));
        m_ImagesBlack  = mapTo3dColors(m_ImagesSrc, QColor(0, 0, 0), QColor(192, 192, 192));
        m_ImagesBlack2  = mapTo3dColors(m_ImagesSrc, QColor(0, 0, 0), QColor(128, 128, 128));
        m_ImagesLtGray = mapTo3dColors(m_ImagesSrc, QColor(0, 0, 0), qRgb(153, 153, 153)/*m_clrTextDisabled*/);
    }

}

/*! \internal */
QColor OfficePaintManager2016::menuLabelColor() const
{
    OfficeStyle::Theme th = theme();
    if (th == OfficeStyle::Office2016Black)
        return QColor(74, 74, 74);
    return th == OfficeStyle::Office2016Colorful ? QColor(235, 235, 235) : QColor(225, 225, 225);
}
