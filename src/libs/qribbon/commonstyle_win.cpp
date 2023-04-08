/**
 * Copyleft (C) 2023 maminjie <canpool@163.com>
 **/
#include "commonstyle_p.h"
#include "ribbonmainwindow.h"
#include "ribbonstyle_p.h"
#include "stylehelpers.h"
#include <QPainter>
#include <qt_windows.h>

QRIBBON_USE_NAMESPACE

#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
QT_BEGIN_NAMESPACE
Q_GUI_EXPORT QPixmap qt_pixmapFromWinHBITMAP(HBITMAP, int hbitmapFormat = 0);
Q_GUI_EXPORT HBITMAP qt_pixmapToWinHBITMAP(const QPixmap &p, int hbitmapFormat = 0);
Q_GUI_EXPORT HICON qt_pixmapToWinHICON(const QPixmap &p);
Q_GUI_EXPORT QPixmap qt_pixmapFromWinHICON(HICON icon);
QT_END_NAMESPACE

enum HBitmapFormat {
    HBitmapNoAlpha,
    HBitmapPremultipliedAlpha,
    HBitmapAlpha
};
#endif

static QImage qtc_fromWinHBITMAP(HDC hdc, HBITMAP bitmap, int w, int h)
{
    BITMAPINFO bmi;
    memset(&bmi, 0, sizeof(bmi));
    bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth = w;
    bmi.bmiHeader.biHeight = -h;
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biBitCount = 32;
    bmi.bmiHeader.biCompression = BI_RGB;
    bmi.bmiHeader.biSizeImage = w * h * 4;

    QImage image(w, h, QImage::Format_ARGB32_Premultiplied);
    image.fill(0);
    if (image.isNull())
        return image;

        // Get bitmap bits
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
    uchar *data = (uchar *)::malloc(bmi.bmiHeader.biSizeImage);
#else
    uchar *data = (uchar *)qMalloc(bmi.bmiHeader.biSizeImage);
#endif

    if (GetDIBits(hdc, bitmap, 0, h, data, &bmi, DIB_RGB_COLORS)) {
        // Create image and copy data into image.
        for (int y = 0; y < h; ++y) {
            void *dest = (void *)image.scanLine(y);
            void *src = data + y * image.bytesPerLine();
            if (src != Q_NULL)
                memcpy(dest, src, image.bytesPerLine());
        }
    } else {
        qWarning("qtc_fromWinHBITMAP(), failed to get bitmap bits");
    }
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
    Q_ASSERT(false);
#else
    qFree(data);
#endif

    return image;
}

QPixmap StyleHelper::alphaBlend(const QPixmap &src)
{
    //    HBITMAP winBitmap1 = src.toWinHBITMAP(QPixmap::Alpha);
    //    return QPixmap::fromWinHBITMAP(winBitmap1, QPixmap::PremultipliedAlpha);

    bool foundAlpha = false;
    HDC screenDevice = GetDC(0);
    HDC hdc = CreateCompatibleDC(screenDevice);
    ReleaseDC(0, screenDevice);

    int w = src.width();
    int h = src.height();

#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
    HBITMAP winBitmap = qt_pixmapToWinHBITMAP(src, HBitmapAlpha);
#else
    HBITMAP winBitmap = src.toWinHBITMAP(QPixmap::Alpha);
#endif

    HGDIOBJ oldhdc = (HBITMAP)SelectObject(hdc, winBitmap);

    QImage image = qtc_fromWinHBITMAP(hdc, winBitmap, w, h);

    for (int y = 0; y < h && !foundAlpha; y++) {
        QRgb *scanLine = reinterpret_cast<QRgb *>(image.scanLine(y));
        for (int x = 0; x < w; x++) {
            if (qAlpha(scanLine[x]) != 0) {
                foundAlpha = true;
                break;
            }
        }
    }

    if (!foundAlpha) {
        // If no alpha was found, we use the mask to set alpha values
        QImage mask = qtc_fromWinHBITMAP(hdc, winBitmap, w, h);

        for (int y = 0; y < h; y++) {
            QRgb *scanlineImage = reinterpret_cast<QRgb *>(image.scanLine(y));
            QRgb *scanlineMask = mask.isNull() ? 0 : reinterpret_cast<QRgb *>(mask.scanLine(y));
            for (int x = 0; x < w; x++) {
                if (scanlineMask && qRed(scanlineMask[x]) != 0)
                    scanlineImage[x] = 0;   // mask out this pixel
                else
                    scanlineImage[x] |= 0xff000000;   // set the alpha channel to 255
            }
        }
    }

    SelectObject(hdc, oldhdc);   // restore state
    DeleteObject(winBitmap);
    DeleteDC(hdc);

    return QPixmap::fromImage(image);
}

static inline COLORREF rgbcolorref(const QColor &col) { return RGB(col.red(), col.green(), col.blue()); }

void FillSolidRect(HDC hdc, int x, int y, int cx, int cy, COLORREF clr)
{
    ::SetBkColor(hdc, clr);

    RECT rect;
    rect.left = x;
    rect.top = y;
    rect.right = x + cx;
    rect.bottom = y + cy;

    ::ExtTextOut(hdc, 0, 0, ETO_OPAQUE, &rect, NULL, 0, NULL);
}

void Draw3dRect(HDC hdc, int x, int y, int cx, int cy, COLORREF clrTopLeft, COLORREF clrBottomRight)
{
    FillSolidRect(hdc, x, y, cx - 1, 1, clrTopLeft);
    FillSolidRect(hdc, x, y, 1, cy - 1, clrTopLeft);
    FillSolidRect(hdc, x + cx, y, -1, cy, clrBottomRight);
    FillSolidRect(hdc, x, y + cy, cx, -1, clrBottomRight);
}

void Draw3dRect(HDC hdc, LPCRECT lpRect, COLORREF clrTopLeft, COLORREF clrBottomRight)
{
    Draw3dRect(hdc, lpRect->left, lpRect->top, lpRect->right - lpRect->left, lpRect->bottom - lpRect->top, clrTopLeft,
               clrBottomRight);
}

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
