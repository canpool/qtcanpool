/**
 * Copyleft (C) 2023 maminjie <canpool@163.com>
 **/
#include "commonstyle_p.h"
#include "ribbonmainwindow.h"
#include "ribbonstyle_p.h"
#include "stylehelpers.h"
#include <QPainter>

QRIBBON_USE_NAMESPACE

// for QForm
/*! \internal */
bool RibbonPaintManager::drawFrame(const QStyleOption *opt, QPainter *p, const QWidget *w) const
{
#ifdef Q_OS_WIN
    Q_UNUSED(p);
    QTC_D_STYLE(RibbonStyle)
    if (qobject_cast<const RibbonMainWindow *>(w)) {
        if (const StyleOptionFrame *optFrame = qstyleoption_cast<const StyleOptionFrame *>(opt)) {
            HDC hdc = (HDC)optFrame->hdc;

            QRect rc = optFrame->rect;
            rc.adjust(0, 0, -1, -1);

            QRect rcBorders = optFrame->clientRect;
            int nRightBorder = rcBorders.left() - rc.left(), nLeftBorder = rcBorders.left() - rc.left(),
                nTopBorder = rcBorders.top() - rc.top();
            int nBottomBorder = rc.bottom() - rcBorders.bottom() + DrawHelpers::dpiScaled(2);

            HBRUSH hBrush = ::CreateSolidBrush(rgbcolorref(d->m_clrRibbonFace));
            Q_ASSERT(hBrush != Q_NULL);

            // draw top
            RECT rectTop = {0, 0, rc.width(), nTopBorder};
            ::FillRect(hdc, &rectTop, hBrush);

            // draw left
            RECT rectLeft = {0, 0, nLeftBorder, rc.height()};
            ::FillRect(hdc, &rectLeft, hBrush);

            // draw right
            RECT rectRight = {rc.width() - nRightBorder, 0, rc.width() + nRightBorder, rc.height()};
            ::FillRect(hdc, &rectRight, hBrush);

            // draw bottom
            RECT rectBottom = {0, rc.height() - nBottomBorder, rc.width(), rc.height() + nBottomBorder};
            ::FillRect(hdc, &rectBottom, hBrush);

            ::DeleteObject(hBrush);

            COLORREF clrBorder =
                optFrame->active ? rgbcolorref(d->m_clrFrameBorderActive0) : rgbcolorref(d->m_clrFrameBorderInactive0);

            HPEN hPen = ::CreatePen(PS_SOLID, 1, clrBorder);
            HGDIOBJ hOldPen = ::SelectObject(hdc, (HGDIOBJ)hPen);

            ::LineTo(hdc, 0, 0);
            ::LineTo(hdc, rc.width(), 0);
            ::LineTo(hdc, rc.width() - 1, 0);
            ::LineTo(hdc, rc.width() - 1, rc.height());
            ::LineTo(hdc, rc.width(), rc.height() - 1);
            ::LineTo(hdc, 0, rc.height() - 1);
            ::LineTo(hdc, 0, rc.height() - 1);
            ::LineTo(hdc, 0, 0);

            SelectObject(hdc, hOldPen);
            ::DeleteObject(hPen);

            if (optFrame->hasStatusBar && !optFrame->isBackstageVisible) {
                int statusHeight = optFrame->statusHeight;
                HBRUSH hBrushStatusBar = ::CreateSolidBrush(rgbcolorref(d->m_clrStatusBarShadow));
                Q_ASSERT(hBrushStatusBar != Q_NULL);

                RECT rectBottom = {0, rc.height() - nBottomBorder, rc.width(), rc.height() + nBottomBorder};
                ::FillRect(hdc, &rectBottom, hBrushStatusBar);

                RECT rectLeft = {0, rc.height() - (statusHeight + (int)DrawHelpers::dpiScaled(nTopBorder)), nLeftBorder,
                                 rc.height() - nBottomBorder};
                ::FillRect(hdc, &rectLeft, hBrushStatusBar);

                RECT rectRight = {rc.width() - nRightBorder,
                                  rc.height() - (statusHeight + (int)DrawHelpers::dpiScaled(nTopBorder)), rc.width(),
                                  rc.height() - nBottomBorder};
                ::FillRect(hdc, &rectRight, hBrushStatusBar);

                ::DeleteObject(hBrushStatusBar);
            }
        }
        return true;
    }
#endif   // Q_OS_WIN
    return false;
}
