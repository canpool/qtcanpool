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
#include <QApplication>
#include <QWidget>
#include <QPainter>
#include <QLayout>
#include <QPixmap>
#include <QPaintEngine>
#include <QStyleOption>
#include <QStatusBar>
#include <QToolButton>
#include <QLibrary>
#include <QDesktopWidget>
#include <qevent.h>

#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
#include <private/qpaintengine_raster_p.h>
#include <private/qwidget_p.h>
#include <QtGui/qwindow.h>
#include <qpa/qplatformwindow.h>
#include <private/qapplication_p.h>
#endif

#include "QtnRibbonSystemPopupBar.h"
#include "QtnRibbonMainWindow.h"
#include "QtnRibbonBar.h"
#include "QtnRibbonTabBar.h"
#include "QtnStyleHelpers.h"
#include "QtnRibbonStyle.h"
#include "QtnOfficeFrameHelper_win.h"

QTITAN_USE_NAMESPACE


#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
QT_BEGIN_NAMESPACE
    Q_GUI_EXPORT QPixmap qt_pixmapFromWinHBITMAP(HBITMAP, int hbitmapFormat = 0);
    Q_GUI_EXPORT HBITMAP qt_pixmapToWinHBITMAP(const QPixmap& p, int hbitmapFormat = 0);
    Q_GUI_EXPORT HICON qt_pixmapToWinHICON(const QPixmap& p);
    Q_GUI_EXPORT QPixmap qt_pixmapFromWinHICON(HICON icon);
QT_END_NAMESPACE

enum HBitmapFormat 
{
    HBitmapNoAlpha,
    HBitmapPremultipliedAlpha,
    HBitmapAlpha
};
#endif

//DWM related
typedef struct          //MARGINS       
{
    int cxLeftWidth;    // width of left border that retains its size
    int cxRightWidth;   // width of right border that retains its size
    int cyTopHeight;    // height of top border that retains its size
    int cyBottomHeight; // height of bottom border that retains its size
} HLP_MARGINS;

typedef struct //DTTOPTS
{
    DWORD dwSize;
    DWORD dwFlags;
    COLORREF crText;
    COLORREF crBorder;
    COLORREF crShadow;
    int eTextShadowType;
    POINT ptShadowOffset;
    int iBorderSize;
    int iFontPropId;
    int iColorPropId;
    int iStateId;
    BOOL fApplyOverlay;
    int iGlowSize;
} HLP_DTTOPTS;

typedef struct 
{
    DWORD dwFlags;
    DWORD dwMask;
} HLP_WTA_OPTIONS;

#define HLP_WM_THEMECHANGED                 0x031A
#define HLP_WM_DWMCOMPOSITIONCHANGED        0x031E
#define HLP_WIN_PTR                       _T("HLPWINPTR")

enum HLP_WINDOWTHEMEATTRIBUTETYPE 
{
    HLP_WTA_NONCLIENT = 1
};

#define HLP_WTNCA_NODRAWCAPTION 0x00000001
#define HLP_WTNCA_NODRAWICON    0x00000002

#define HLP_DT_CENTER           0x00000001 // DT_CENTER
#define HLP_DT_VCENTER          0x00000004
#define HLP_DT_SINGLELINE       0x00000020
#define HLP_DT_NOPREFIX         0x00000800

enum HLP_NAVIGATIONPARTS           // NAVIGATIONPARTS
{
    HLP_NAV_BACKBUTTON = 1,
    HLP_NAV_FORWARDBUTTON = 2,
    HLP_NAV_MENUBUTTON = 3,
};

enum HLP_NAV_BACKBUTTONSTATES      // NAV_BACKBUTTONSTATES
{
    HLP_NAV_BB_NORMAL = 1,
    HLP_NAV_BB_HOT = 2,
    HLP_NAV_BB_PRESSED = 3,
    HLP_NAV_BB_DISABLED = 4,
};

#define HLP_TMT_CAPTIONFONT (801)           //TMT_CAPTIONFONT
#define HLP_DTT_COMPOSITED  (1UL << 13)     //DTT_COMPOSITED
#define HLP_DTT_GLOWSIZE    (1UL << 11)     //DTT_GLOWSIZE

#define HLP_WM_NCMOUSELEAVE 674             //WM_NCMOUSELEAVE

#define HLP_WP_CAPTION             1 // WP_CAPTION
#define HLP_WP_MAXCAPTION          5 // WP_MAXCAPTION
#define HLP_CS_ACTIVE              1 // CS_ACTIVE
#define HLP_FS_ACTIVE              1
#define HLP_FS_INACTIVE            2
#define HLP_TMT_FILLCOLORHINT   3821 // TMT_FILLCOLORHINT
#define HLP_TMT_BORDERCOLORHINT 3822 // TMT_BORDERCOLORHINT


typedef BOOL (WINAPI *PtrDwmDefWindowProc)(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam, LRESULT *plResult);
typedef HRESULT (WINAPI *PtrDwmIsCompositionEnabled)(BOOL* pfEnabled);
typedef HRESULT (WINAPI *PtrDwmExtendFrameIntoClientArea)(HWND hWnd, const HLP_MARGINS* pMarInset);
typedef HRESULT (WINAPI *PtrSetWindowThemeAttribute)(HWND hwnd, enum HLP_WINDOWTHEMEATTRIBUTETYPE eAttribute, PVOID pvAttribute, DWORD cbAttribute);

static PtrDwmDefWindowProc pDwmDefWindowProc = 0;
static PtrDwmIsCompositionEnabled pDwmIsCompositionEnabled = 0;
static PtrDwmExtendFrameIntoClientArea pDwmExtendFrameIntoClientArea = 0;

//Theme related
typedef bool (WINAPI *PtrIsAppThemed)();
typedef bool (WINAPI *PtrIsThemeActive)();
typedef HANDLE (WINAPI *PtrOpenThemeData)(HWND hwnd, LPCWSTR pszClassList);
typedef HRESULT (WINAPI *PtrCloseThemeData)(HANDLE hTheme);
typedef HRESULT (WINAPI *PtrGetThemeSysFont)(HANDLE hTheme, int iFontId, LOGFONTW *plf);
typedef HRESULT (WINAPI *PtrDrawThemeTextEx)(HANDLE hTheme, HDC hdc, int iPartId, int iStateId, LPCWSTR pszText, int cchText, DWORD dwTextFlags, LPRECT pRect, const HLP_DTTOPTS *pOptions);
typedef HRESULT (WINAPI *PtrDrawThemeBackground)(HANDLE hTheme, HDC hdc, int iPartId, int iStateId, const RECT *pRect, OPTIONAL const RECT *pClipRect);
typedef HRESULT (WINAPI *PtrGetThemeColor)(HANDLE hTheme, int iPartId, int iStateId, int iPropId, OUT COLORREF *pColor);

enum WIS_UX_BUFFERFORMAT
{
    WIS_BPBF_COMPATIBLEBITMAP,    // Compatible bitmap
    WIS_BPBF_DIB,                 // Device-independent bitmap
    WIS_BPBF_TOPDOWNDIB,          // Top-down device-independent bitmap
    WIS_BPBF_TOPDOWNMONODIB       // Top-down monochrome device-independent bitmap
};

// BP_PAINTPARAMS
struct WIS_UX_PAINTPARAMS
{
    DWORD                       cbSize;
    DWORD                       dwFlags; // BPPF_ flags
    const RECT *                prcExclude;
    const BLENDFUNCTION *       pBlendFunction;
};

//typedef HANDLE (WINAPI *PtrBeginBufferedPaint)(HDC hdcTarget, const RECT* rect, WIS_UX_BUFFERFORMAT dwFormat, WIS_UX_PAINTPARAMS* pPaintParams, HDC* phdc);

static PtrIsAppThemed pIsAppThemed = 0;
//static PtrIsThemeActive pIsThemeActive = 0;
static PtrOpenThemeData pOpenThemeData = 0;
//static PtrCloseThemeData pCloseThemeData = 0;
static PtrGetThemeSysFont pGetThemeSysFont = 0;
static PtrDrawThemeTextEx pDrawThemeTextEx = 0;
//static PtrDrawThemeBackground pDrawThemeBackground = 0;
//static PtrGetThemeColor pGetThemeColor = 0;

class OfficeFrameHelperWin::FrameHelperEventHook
{
public:
    FrameHelperEventHook();
    ~FrameHelperEventHook();

public:
    void setEventHook();
    void removeEventHook();

protected:
    typedef void (CALLBACK* WINEVENTPROC) ( HWINEVENTHOOK hWinEventHook, DWORD event, HWND hwnd, LONG idObject, LONG idChild, DWORD idEventThread, DWORD dwmsEventTime);
    typedef HWINEVENTHOOK (WINAPI* LPFNSETWINEVENTHOOK)(UINT eventMin, UINT eventMax, HMODULE hmodWinEventProc,
        WINEVENTPROC lpfnWinEventProc, DWORD idProcess, DWORD idThread, UINT dwflags);
    typedef BOOL (WINAPI* LPFNUNHOOKWINEVENT)(HWINEVENTHOOK hWinEventHook);

    static void CALLBACK WinEventProc(HWINEVENTHOOK /*hWinEventHook*/,
        DWORD event, HWND hWnd, LONG idObject, LONG /*idChild*/, DWORD /*dwEventThread*/, DWORD /*dwmsEventTime*/);

protected:
    HWINEVENTHOOK m_hWinEventHook;
    LPFNSETWINEVENTHOOK m_pSetWinEventHook;
    LPFNUNHOOKWINEVENT m_pUnhookWinEvent;
};

uint OfficeFrameHelperWin::m_msgGetFrameHook = 0;
OfficeFrameHelperWin::FrameHelperEventHook::FrameHelperEventHook()
{
    m_pSetWinEventHook = Q_NULL;
    m_pUnhookWinEvent = Q_NULL;
    m_hWinEventHook = Q_NULL;
}

OfficeFrameHelperWin::FrameHelperEventHook::~FrameHelperEventHook()
{
}

void OfficeFrameHelperWin::FrameHelperEventHook::setEventHook()
{
    QLibrary dwmLib(QString::fromLatin1("user32"));
    m_pSetWinEventHook = (LPFNSETWINEVENTHOOK)dwmLib.resolve("SetWinEventHook");

    m_pUnhookWinEvent = (LPFNUNHOOKWINEVENT)dwmLib.resolve("UnhookWinEvent");
    if (m_pSetWinEventHook && m_pUnhookWinEvent)
        m_hWinEventHook = m_pSetWinEventHook(EVENT_OBJECT_REORDER, EVENT_OBJECT_REORDER, Q_NULL, 
        &OfficeFrameHelperWin::FrameHelperEventHook::WinEventProc, GetCurrentProcessId(), 0, 0);
}

void OfficeFrameHelperWin::FrameHelperEventHook::removeEventHook()
{
    if (m_hWinEventHook && m_pUnhookWinEvent)
        m_pUnhookWinEvent(m_hWinEventHook);
    m_hWinEventHook = Q_NULL;
}

void CALLBACK OfficeFrameHelperWin::FrameHelperEventHook::WinEventProc(HWINEVENTHOOK /*hWinEventHook*/,
    DWORD event, HWND hWnd, LONG idObject, LONG /*idChild*/, DWORD /*dwEventThread*/, DWORD /*dwmsEventTime*/)
{
    if (event == EVENT_OBJECT_REORDER && idObject == OBJID_WINDOW)
    {
        ::SendMessage(hWnd, OfficeFrameHelperWin::m_msgGetFrameHook, 0, 0);
        OfficeFrameHelperWin* frameHelperWin = (OfficeFrameHelperWin*)::GetProp(hWnd, HLP_WIN_PTR);
        if (frameHelperWin)
        {
            if (hWnd != frameHelperWin->m_hwndFrame)
                return;

            if (frameHelperWin && !frameHelperWin->isDwmEnabled())
            {
                DWORD dwStyle = frameHelperWin->getStyle();
                DWORD dwStyleRemove = (WS_CAPTION | WS_VSCROLL | WS_HSCROLL);

                if (dwStyle & dwStyleRemove)
                    frameHelperWin->m_refreshFrame.start(100);
            }
        }
    }
}





bool OfficeFrameHelperWin::m_allowDwm = true;

/*!
    \class Qtitan::OfficeFrameHelperWin
    \internal
*/
OfficeFrameHelperWin::OfficeFrameHelperWin(QWidget* parent)
    : QObject(parent)
{
    m_hIcon = Q_NULL;
    m_frame = Q_NULL;
    m_hwndFrame = Q_NULL;
    m_listContextHeaders = Q_NULL;
    m_dwmEnabled = false;
    m_lockNCPaint = false;
    m_inUpdateFrame = false;
    m_inLayoutRequest = false;
    m_postReclalcLayout = false;
    m_szFrameRegion = QSize();
    m_frameBorder = 0;
    m_borderSizeBotton = 0;
    m_skipNCPaint = false;
    m_active = true;
    m_officeFrameEnabled = false;
    m_wasFullScreen = false;

    if (m_msgGetFrameHook == 0) 
        m_msgGetFrameHook = RegisterWindowMessageW(L"WM_GETFRAMEHOOK");

    connect(&m_refreshFrame,  SIGNAL(timeout()), this, SLOT(refreshFrameTimer()));

    if (!resolveSymbols())
       Q_ASSERT(false);
}

OfficeFrameHelperWin::~OfficeFrameHelperWin()
{
    if (m_frame)
        m_frame->removeEventFilter(this);

    if (RibbonMainWindow* mainWindow = qobject_cast<RibbonMainWindow*>(m_frame))
        mainWindow->setFrameHelper(Q_NULL);

    if (m_officeFrameEnabled)
        enableOfficeFrame(Q_NULL);

    resetWindowIcon();
}

void OfficeFrameHelperWin::enableOfficeFrame(QWidget* widget)
{
    bool enabled = widget != Q_NULL;
    if (m_officeFrameEnabled == enabled)
        return;

    m_ribbonBar = qobject_cast<RibbonBar*>(parent());
    m_frame = widget;
    m_active = true;
    m_officeFrameEnabled = enabled;
    m_postReclalcLayout = false;

    if (RibbonMainWindow* mainWindow = qobject_cast<RibbonMainWindow*>(m_frame))
        mainWindow->setFrameHelper(this);

    if (m_frame)
    {
        m_hwndFrame = (HWND)m_frame->winId();
        m_dwmEnabled = !m_allowDwm || getStyle() & WS_CHILD ? false : isCompositionEnabled();

        m_frame->installEventFilter(this);

        if (!m_dwmEnabled)
            refreshFrameStyle();
        else
            SetWindowRgn(m_hwndFrame, Q_NULL, true);

        updateFrameRegion();

        ::SetWindowPos(m_hwndFrame, 0, 0, 0, 0, 0, SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED);
        setFrameHook();
        QApplication::postEvent(m_ribbonBar, new QEvent(QEvent::LayoutRequest));
    }
    else
    {
        disableOfficeFrame();
    }
}

void OfficeFrameHelperWin::disableOfficeFrame()
{
    if (m_frame)
        m_frame->removeEventFilter(this);

    if (RibbonMainWindow* mainWindow = qobject_cast<RibbonMainWindow*>(m_frame))
        mainWindow->setFrameHelper(Q_NULL);

    m_frame = Q_NULL;
    m_ribbonBar = Q_NULL;
    m_officeFrameEnabled = false;
    m_postReclalcLayout = false;

    if (m_hwndFrame)
    {
        if (m_dwmEnabled)
        {
            HLP_MARGINS margins;
            margins.cxLeftWidth = 0;
            margins.cyTopHeight = 0;
            margins.cxRightWidth = 0;
            margins.cyBottomHeight = 0;
            pDwmExtendFrameIntoClientArea(m_hwndFrame, &margins);
        }

        SetWindowRgn(m_hwndFrame, (HRGN)Q_NULL, true);
        ::SetWindowPos(m_hwndFrame, 0, 0, 0, 0, 0, SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED);
    }
    m_hwndFrame = Q_NULL;
    m_dwmEnabled = false;
    setFrameHook();
}

void OfficeFrameHelperWin::setFrameHook()
{
    static FrameHelperEventHook hook;
    if (m_hwndFrame)
        hook.setEventHook();
    else
        hook.removeEventHook();
};

bool OfficeFrameHelperWin::isDwmEnabled() const
{
    return m_dwmEnabled;
}

bool OfficeFrameHelperWin::isActive() const
{
    return m_active;
}

bool OfficeFrameHelperWin::isMaximize() const
{
    return getStyle() & WS_MAXIMIZE;
}

int OfficeFrameHelperWin::getFrameBorder() const
{
    return m_frameBorder;
}

DWORD OfficeFrameHelperWin::getStyle(bool exStyle) const
{
    return m_hwndFrame ? (DWORD)GetWindowLongPtrW(m_hwndFrame, exStyle ? GWL_EXSTYLE : GWL_STYLE) : 0;
}

int OfficeFrameHelperWin::titleBarSize() const
{ 
    if (isTitleVisible() || (!m_ribbonBar && m_frame))
        return m_frame->style()->pixelMetric(QStyle::PM_TitleBarHeight, 0) + m_frameBorder;

    if (m_ribbonBar)
        return m_ribbonBar->titleBarHeight() + m_frameBorder;

    return frameSize() + captionSize(); 
}

int OfficeFrameHelperWin::tabBarHeight() const
{
    if (m_ribbonBar)
        return m_ribbonBar->tabBarHeight();
    return 0;
}

void OfficeFrameHelperWin::setHeaderRect(const QRect& rcHeader)
{
    m_rcHeader = rcHeader;
}

void OfficeFrameHelperWin::setContextHeader(QList<ContextHeader*>* listContextHeaders)
{
    m_listContextHeaders = listContextHeaders;
}

QPixmap OfficeFrameHelperWin::getFrameSmallIcon() const
{
    if (!m_hwndFrame)
        return QPixmap();

    DWORD dwStyle = getStyle();
    if ((dwStyle & WS_SYSMENU) == 0)
        return QPixmap();

    HICON hIcon = (HICON)(DWORD_PTR)::SendMessageW(m_hwndFrame, WM_GETICON, ICON_SMALL, 0);
    if (!hIcon)
        hIcon = (HICON)(DWORD_PTR)::GetClassLongPtrW(m_hwndFrame, GCLP_HICONSM);
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
    return qt_pixmapFromWinHICON(hIcon);
#else
    return QPixmap::fromWinHICON(hIcon);
#endif
}

QString OfficeFrameHelperWin::getSystemMenuString(uint item) const
{
    if (!m_hwndFrame)
        return QString();
//    Q_ASSERT(m_hwndFrame != Q_NULL);
    UINT id = SC_CLOSE;
    switch (item)
    {
        case QStyle::SC_TitleBarCloseButton :
                id = SC_CLOSE;
            break;
        case QStyle::SC_TitleBarNormalButton :
                id = SC_RESTORE;
            break;
        case QStyle::SC_TitleBarMaxButton :
               id = SC_MAXIMIZE;
            break;
        case QStyle::SC_TitleBarMinButton :
               id = SC_MINIMIZE;
           break;
        default :
           break;
    }

    HMENU hMenu = ::GetSystemMenu(m_hwndFrame, 0);
    Q_ASSERT(::IsMenu(hMenu)); 
    if (hMenu)
    {
        // offer no buffer first
        int len = ::GetMenuStringW(hMenu, id, Q_NULL, 0, MF_BYCOMMAND);

        // use exact buffer length
        if (len > 0)
        {
            ushort* lpBuf = new ushort[len + 1];
            ::GetMenuStringW(hMenu, id, (LPTSTR)lpBuf, len+1, MF_BYCOMMAND);

            QString str = QString::fromUtf16(lpBuf, len);
            int index = str.indexOf(QLatin1String("\t"));
            if (index> 0)
                str = str.left(index);

            delete [] lpBuf;
            return str;
         }
    }
    return QString();
}

void OfficeFrameHelperWin::refreshFrameStyle()
{
    if (m_dwmEnabled)
        return;

    DWORD dwStyle = getStyle();
    DWORD dwStyleRemove = (WS_DLGFRAME | WS_VSCROLL | WS_HSCROLL);

    if (dwStyle & dwStyleRemove)
    {
        m_lockNCPaint = true;
        SetWindowLongPtrW(m_hwndFrame, GWL_STYLE, dwStyle & ~dwStyleRemove);

        RECT rc;
        ::GetWindowRect(m_hwndFrame, &rc);
        ::SendMessageW(m_hwndFrame, WM_NCCALCSIZE, false, (LPARAM)&rc);

        SetWindowLongPtrW(m_hwndFrame, GWL_STYLE, dwStyle);

        m_lockNCPaint = false;
        redrawFrame();
    }
}

void OfficeFrameHelperWin::updateFrameRegion()
{
    if (!::IsWindow(m_hwndFrame))
        return;

    RECT rc;
    ::GetWindowRect(m_hwndFrame, &rc);
    updateFrameRegion(QSize(rc.right - rc.left, rc.bottom - rc.top), true);
}

void OfficeFrameHelperWin::updateFrameRegion(const QSize& szFrameRegion, bool bUpdate)
{
    if (m_inUpdateFrame)
        return;

    m_inUpdateFrame = true;

    if (m_szFrameRegion != szFrameRegion || bUpdate)
    {
        RECT rc;
        rc.left = 0;
        rc.top = 0;
        rc.right = szFrameRegion.width();
        rc.bottom = szFrameRegion.height();
        AdjustWindowRectEx(&rc, getStyle(false), false, getStyle(true));

        m_frameBorder = -rc.left;
        m_borderSizeBotton = m_frameBorder;

        if (!m_dwmEnabled)
        {
            HRGN hRgn = 0;

            if (!isMDIMaximized())
            {
                if (getStyle() & WS_MAXIMIZE)
                {
                    int nFrameRegion = m_frameBorder - (getStyle(true) & WS_EX_CLIENTEDGE ? 2 : 0);
                    hRgn = CreateRectRgn(nFrameRegion, nFrameRegion, szFrameRegion.width() - nFrameRegion, szFrameRegion.height() - nFrameRegion);
                }
                else
                    hRgn = calcFrameRegion(szFrameRegion);
            }
            ::SetWindowRgn(m_hwndFrame, hRgn, true);
        }
        else
        {
            if (!(getStyle() & WS_MAXIMIZE))
                m_frameBorder = 4;

            bool flatFrame = (bool)m_ribbonBar->style()->styleHint((QStyle::StyleHint)RibbonStyle::SH_FlatFrame);

            int cyTopHeight = isTitleVisible() ? 0 : titleBarSize() + (flatFrame ? 0 : 1);

            if (m_ribbonBar && !isTitleVisible() && flatFrame)
                cyTopHeight += m_ribbonBar->tabBarHeight() + 1;

            HLP_MARGINS margins;
            margins.cxLeftWidth = 0;
            margins.cyTopHeight = cyTopHeight;
            margins.cxRightWidth = 0;
            margins.cyBottomHeight = 0;
            pDwmExtendFrameIntoClientArea(m_hwndFrame, &margins);

        }

        m_szFrameRegion = szFrameRegion;
    }

    m_inUpdateFrame = false;
}

bool OfficeFrameHelperWin::isMDIMaximized() const
{
    return (getStyle() & WS_MAXIMIZE) && (getStyle(TRUE) & WS_EX_MDICHILD);
}

bool OfficeFrameHelperWin::isTitleVisible() const
{
    if (!m_ribbonBar)
        return true;

    if (!m_ribbonBar->isVisible())
        return true;

    if ((getStyle() & (WS_CHILD | WS_MINIMIZE)) == (WS_CHILD | WS_MINIMIZE))
        return true;

    return false;
}

void OfficeFrameHelperWin::initStyleOption(StyleOptionFrame* option)
{
    RECT rc;
    ::GetWindowRect(m_hwndFrame, &rc);
    RECT rcClient;
    ::GetClientRect(m_hwndFrame, &rcClient);
    ::ClientToScreen(m_hwndFrame, (LPPOINT)&rcClient);
    ::ClientToScreen(m_hwndFrame, ((LPPOINT)&rcClient)+1);

    option->init(m_frame);
    option->rect = QRect(QPoint(rc.left, rc.top), QPoint(rc.right, rc.bottom)); 
    option->active = m_active;
    option->hasStatusBar = isFrameHasStatusBar();
    option->frameBorder = m_frameBorder;
    option->titleBarSize = titleBarSize();
    isFrameHasStatusBar(&option->statusHeight);
    option->titleVisible = isTitleVisible();
    option->clientRect = QRect(QPoint(rcClient.left, rcClient.top), QPoint(rcClient.right, rcClient.bottom));
    option->maximizeFlags = getStyle() & WS_MAXIMIZE;
}

static void drawPartFrame(QWidget* frame, HDC hdc, HDC cdc, const StyleOptionFrame& opt, const QRect& rect)
{
    QPixmap pixmap(rect.size());
    QPainter p(&pixmap);
    p.setWindow(rect);

    frame->style()->drawPrimitive(QStyle::PE_Frame, &opt, &p, frame);

#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
    if (HBITMAP newBmp = qt_pixmapToWinHBITMAP(pixmap))
#else
    if (HBITMAP newBmp = pixmap.toWinHBITMAP())
#endif
    {
        HBITMAP oldb = (HBITMAP)SelectObject(cdc, newBmp);
        ::BitBlt(hdc, rect.left(), rect.top(), rect.width(), rect.height(), cdc, 0, 0, SRCCOPY);
        ::SelectObject(cdc, oldb);
        ::DeleteObject(newBmp);
    }
}

void OfficeFrameHelperWin::redrawFrame()
{
    if (!m_dwmEnabled)
    {
        RECT rc;
        ::GetWindowRect(m_hwndFrame, &rc);

        int height = rc.bottom - rc.top;
        int width = rc.right - rc.left;

        HDC hdc = ::GetWindowDC(m_hwndFrame);
        HDC cdc = CreateCompatibleDC(hdc);

        StyleOptionFrame opt;
        initStyleOption(&opt);
        int frameBorder = m_frameBorder != 0 ? m_frameBorder : 4;
        int borderSizeBotton = m_borderSizeBotton != 0 ? m_borderSizeBotton : 4;
        // draw top side
        ::drawPartFrame(m_frame, hdc, cdc, opt, QRect(0, 0, width, opt.titleVisible ? opt.titleBarSize : frameBorder));
        // draw right side
        ::drawPartFrame(m_frame, hdc, cdc, opt, QRect(width - frameBorder, 0, frameBorder, height));
        // draw left side
        ::drawPartFrame(m_frame, hdc, cdc, opt, QRect(0, 0, frameBorder, height));
        // draw bottom side
        ::drawPartFrame(m_frame, hdc, cdc, opt, QRect(0, height - borderSizeBotton, width, borderSizeBotton));
        ::DeleteDC(cdc);

        if (opt.titleVisible)
        {
            QRect rect(QPoint(rc.left, rc.top), QPoint(rc.right, rc.bottom));

            int nFrameBorder = getFrameBorder();
            int nTopBorder = isDwmEnabled() ? nFrameBorder : 0;
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
            if (HICON hIcon = qt_pixmapToWinHICON(getFrameSmallIcon()))
#else
            if (HICON hIcon = getFrameSmallIcon().toWinHICON())
#endif
            {
                int nFrameBorder = getFrameBorder();

                QSize szIcon(GetSystemMetrics(SM_CXSMICON), GetSystemMetrics(SM_CYSMICON));

                int nTop = nFrameBorder;
                int nLeft = nFrameBorder + 1;

                QRect rect(QPoint(nLeft, nTop), szIcon);
                ::DrawIconEx(hdc, rect.left(), rect.top(), hIcon, 0, 0, 0, Q_NULL, DI_NORMAL | DI_COMPAT);
                ::DestroyIcon(hIcon);
            }

            ::SetBkMode(hdc, TRANSPARENT);

            HFONT hCaptionFont = getCaptionFont(Q_NULL);
            HFONT hOldFont = (HFONT)SelectObject(hdc, (HGDIOBJ) hCaptionFont);

            QRect rctext = rect;
            rctext.setHeight(opt.titleBarSize);

            QRgb clrFrameCaptionText = 0;
            if (!opt.active)
            {
                Qtitan::StyleHintReturnThemeColor hintReturnInActive(tr("Window"), tr("CaptionTextInActive"));
                clrFrameCaptionText = static_cast<QRgb>(m_frame->style()->styleHint(QStyle::SH_CustomBase, Q_NULL, Q_NULL, &hintReturnInActive));
            }
            else
            {
                Qtitan::StyleHintReturnThemeColor hintReturnActive(tr("Window"), tr("CaptionTextActive"));
                clrFrameCaptionText = static_cast<QRgb>(m_frame->style()->styleHint(QStyle::SH_CustomBase, Q_NULL, Q_NULL, &hintReturnActive));
            }

            COLORREF colorInActive = RGB(qRed(clrFrameCaptionText), qGreen(clrFrameCaptionText), qBlue(clrFrameCaptionText));
            ::SetTextColor(hdc, colorInActive);

            int len = ::GetWindowTextLengthW(m_hwndFrame);
            wchar_t* text = new wchar_t[++len];

            ::GetWindowTextW(m_hwndFrame, text, len + 1);

            QSize szIcon(GetSystemMetrics(SM_CXSMICON), GetSystemMetrics(SM_CYSMICON));
            RECT rc = {nFrameBorder + szIcon.width() + 3, nTopBorder, opt.rect.width() - nFrameBorder /*-100*/, opt.titleBarSize};

            UINT nFormat = DT_VCENTER | DT_LEFT| DT_END_ELLIPSIS | DT_SINGLELINE | DT_NOPREFIX;
            ::DrawTextW(hdc, text, len, &rc, nFormat);
            delete[] text;
            SelectObject(hdc, (HGDIOBJ) hOldFont);
        }

        ::ReleaseDC(m_hwndFrame, hdc);
    }
}

void OfficeFrameHelperWin::drawTitleBar(QPainter* painter, const StyleOptionTitleBar& opt)
{
    if (!m_ribbonBar)
        return;
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
    QRasterPaintEngine* en = (QRasterPaintEngine *)painter->paintEngine();
#else
    QPaintEngine* en = painter->paintEngine();
#endif
    HDC hdc = en->getDC();

    QRect rc = opt.rect;
    if ((bool)m_ribbonBar->style()->styleHint((QStyle::StyleHint)RibbonStyle::SH_FlatFrame))
        rc.adjust( 0, 0, 0, 1 );

    fillSolidRect(painter, rc, opt.airRegion, QColor(0, 0, 0));

    int len = ::GetWindowTextLengthW(m_hwndFrame);
    wchar_t* text = new wchar_t[++len];
    ::GetWindowTextW(m_hwndFrame, text, len + 1);
    drawDwmCaptionText(painter, opt.rcTitleText, text[0] != NULL ? QString::fromWCharArray(text) : opt.text, opt.state & QStyle::State_Active);
    delete[] text;

    if (opt.drawIcon && !opt.icon.isNull()) 
    {
        int nFrameBorder = getFrameBorder();
        int nTopBorder = isDwmEnabled() ? nFrameBorder : 0;

        QSize szIcon = getSizeSystemIcon();

        int nTop = nTopBorder / 2 - 1 + (opt.rect.bottom()  - szIcon.height())/2;
        int nLeft = opt.rect.left() + nFrameBorder + 1;

        QRect rect(QPoint(nLeft, nTop), szIcon);

        //m_hIcon = (HICON)(DWORD_PTR)::GetClassLongPtr(m_hwndFrame, GCLP_HICONSM);

        if (HICON hIcon = getWindowIcon(opt.icon))
            ::DrawIconEx(hdc, rect.left(), rect.top(), hIcon, 0, 0, 0, Q_NULL, DI_NORMAL | DI_COMPAT);
    }

    if (hdc)
        en->releaseDC(hdc);
}

void OfficeFrameHelperWin::fillSolidRect(QPainter* painter, const QRect& rect, const QRegion& airRegion, QColor clr)
{
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
    QRasterPaintEngine* en = (QRasterPaintEngine *)painter->paintEngine();
#else
    QPaintEngine* en = painter->paintEngine();
#endif
    HDC hdc = en->getDC();

    if (hdc == 0)
        return;

    QVector<QRect> rects = airRegion.rects();
    for (QVector<QRect>::iterator it = rects.begin(); it != rects.end(); ++it)
    {
        QRect r = rect.intersected(*it);
        RECT rc;
        rc.left    = r.left();
        rc.top     = r.top();
        rc.right   = r.right() + 1;
        rc.bottom  = r.bottom() + 1;
        ::SetBkColor(hdc, RGB(clr.red(), clr.green(), clr.blue()));
        ::ExtTextOutW(hdc, 0, 0, ETO_OPAQUE, &rc, Q_NULL, 0, Q_NULL);
    }
    en->releaseDC(hdc);
}

void OfficeFrameHelperWin::drawDwmCaptionText(QPainter* painter, const QRect& rc, const QString& strWindowText, bool active)
{
    QRect rect = rc;
    int glow = glowSize() * 2;

    const QFont font = QApplication::font("QMdiSubWindowTitleBar");
    const QFontMetrics fm(font);
    QString text = fm.elidedText(strWindowText, Qt::ElideRight, rect.width() - glow);

//    rect.setWidth(rect.width() + glow);

    if (!rect.isValid())
        return;

    HANDLE hTheme = pOpenThemeData((HWND)QApplication::desktop()->winId(), L"WINDOW");
    if (!hTheme) 
       return;

    // Set up a memory DC and bitmap that we'll draw into
    HDC dcMem;
    HBITMAP bmp;
    BITMAPINFO dib;
    ZeroMemory(&dib, sizeof(dib));
    dcMem = CreateCompatibleDC(/*hdc*/0);

    dib.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    dib.bmiHeader.biWidth = rect.width();
    dib.bmiHeader.biHeight = -rect.height();
    dib.bmiHeader.biPlanes = 1;
    dib.bmiHeader.biBitCount = 32;
    dib.bmiHeader.biCompression = BI_RGB;

    bmp = CreateDIBSection(/*hdc*/0, &dib, DIB_RGB_COLORS, Q_NULL, Q_NULL, 0);

    // Set up the DC
    HFONT hCaptionFont = getCaptionFont(hTheme);
    HBITMAP hOldBmp = (HBITMAP)SelectObject(dcMem, (HGDIOBJ) bmp);
    HFONT hOldFont = (HFONT)SelectObject(dcMem, (HGDIOBJ) hCaptionFont);

    // Draw the text!
    HLP_DTTOPTS dto;
    dto.dwSize = sizeof(HLP_DTTOPTS);
    const UINT uFormat = DT_VCENTER | DT_CENTER | DT_END_ELLIPSIS | DT_SINGLELINE | DT_NOPREFIX;
    RECT rctext = {0, 0, rect.width(), rect.height()};

    dto.dwFlags = HLP_DTT_COMPOSITED | HLP_DTT_GLOWSIZE;
    dto.iGlowSize = glowSize();

    bool maximizeStyle = getStyle() & WS_MAXIMIZE;

    pDrawThemeTextEx(hTheme, dcMem, maximizeStyle ? HLP_WP_MAXCAPTION : HLP_WP_CAPTION, active ? HLP_FS_ACTIVE : HLP_FS_INACTIVE, 
       (LPCWSTR)text.utf16(), -1, uFormat, &rctext, &dto);

#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
    QPixmap pxx = ::qt_pixmapFromWinHBITMAP(bmp, HBitmapPremultipliedAlpha);
#else
    QPixmap pxx = QPixmap::fromWinHBITMAP(bmp, QPixmap::PremultipliedAlpha);
#endif

//    painter->drawPixmap(rect.left(), rect.top() + (maximizeStyle ? 0 : m_frameBorder), pxx);
    painter->drawPixmap(rect.left(), rect.top() + (maximizeStyle ? m_frameBorder/2 : m_frameBorder), pxx);


/*
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
    QRasterPaintEngine* en = (QRasterPaintEngine *)painter->paintEngine();
#else
    QPaintEngine* en = painter->paintEngine();
#endif
    HDC hdc = en->getDC();

    ::BitBlt(hdc, rect.left(), rect.top() + (maximizeStyle ? m_frameBorder/2 : m_frameBorder), rect.width(), rect.height(), dcMem, 0, 0, SRCCOPY);

    en->releaseDC(hdc);
*/
    SelectObject(dcMem, (HGDIOBJ) hOldBmp);
    SelectObject(dcMem, (HGDIOBJ) hOldFont);
    DeleteObject(bmp);
    DeleteObject(hCaptionFont);
    DeleteDC(dcMem);
}

QSize OfficeFrameHelperWin::getSizeSystemIcon() const
{
    return QSize(GetSystemMetrics(SM_CXSMICON), GetSystemMetrics(SM_CYSMICON));
}

HICON OfficeFrameHelperWin::getWindowIcon(const QIcon& icon) const
{
    OfficeFrameHelperWin* that = (OfficeFrameHelperWin*)this;
    if (!m_hIcon && !icon.isNull())
    {
        QSize szIcon = getSizeSystemIcon();
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
        that->m_hIcon = qt_pixmapToWinHICON(icon.pixmap(szIcon));
#else
        that->m_hIcon = icon.pixmap(szIcon).toWinHICON();
#endif
    }
    return m_hIcon;
}

void OfficeFrameHelperWin::resetWindowIcon()
{
    if (m_hIcon)
        ::DestroyIcon(m_hIcon);
    m_hIcon = Q_NULL;
}

void OfficeFrameHelperWin::recalcFrameLayout()
{
    m_postReclalcLayout = false;
    m_szFrameRegion = QSize();
    ::SetWindowPos(m_hwndFrame, 0, 0, 0, 0, 0, SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED);

    if (QMainWindow* mainWindow = qobject_cast<QMainWindow*>(m_frame))
    {
        if (QLayout* layout = mainWindow->layout())
            layout->invalidate();
    }
    else
    {
        RECT rcClient;
        ::GetClientRect(m_hwndFrame, &rcClient);
        ::SendMessage(m_hwndFrame, WM_SIZE, 0, MAKELPARAM(rcClient.right - rcClient.left, rcClient.bottom - rcClient.top));
    }

    if (m_ribbonBar)
        m_ribbonBar->repaint();
}

void OfficeFrameHelperWin::postRecalcFrameLayout()
{
    m_postReclalcLayout = true;
    QApplication::postEvent(this, new QEvent(QEvent::LayoutRequest));
}

void OfficeFrameHelperWin::processClickedSubControl(QStyle::SubControl subControl)
{
    int idCommand = -1;
    switch(subControl)
    {
        case QStyle::SC_TitleBarCloseButton  :
                idCommand = SC_CLOSE;
            break;
        case QStyle::SC_TitleBarNormalButton :
                idCommand = SC_RESTORE;
            break;
        case QStyle::SC_TitleBarMaxButton    :
                idCommand = SC_MAXIMIZE;
            break;
        case QStyle::SC_TitleBarMinButton    :
                idCommand = SC_MINIMIZE;
            break;
        default:
            break;
    }

    if (idCommand != -1)
        ::PostMessage(m_hwndFrame, WM_SYSCOMMAND, idCommand, 0);
}

static bool _qtnModifyStyle(HWND hWnd, int nStyleOffset, DWORD dwRemove, DWORD dwAdd, UINT nFlags)
{
    Q_ASSERT(hWnd != Q_NULL);
    DWORD dwStyle = GetWindowLongPtrW(hWnd, nStyleOffset);
    DWORD dwNewStyle = (dwStyle & ~dwRemove) | dwAdd;
    if (dwStyle == dwNewStyle)
        return false;

    SetWindowLongPtrW(hWnd, nStyleOffset, dwNewStyle);
    if (nFlags != 0)
        ::SetWindowPos(hWnd, Q_NULL, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE | nFlags);
    return true;
}

bool OfficeFrameHelperWin::modifyStyle(HWND hWnd, DWORD dwRemove, DWORD dwAdd, UINT nFlags)
{
    return _qtnModifyStyle(hWnd, GWL_STYLE, dwRemove, dwAdd, nFlags);
}

bool OfficeFrameHelperWin::isFrameHasStatusBar(int* statusHeight) const
{
    if (RibbonMainWindow* mainWindow = qobject_cast<RibbonMainWindow*>(m_frame))
    {
        if (QWidget* statusBar = mainWindow->findChild<QStatusBar*>())
        {
            if (statusHeight)
                *statusHeight = statusBar->height();
            return statusBar->isVisible();
        }
    }
    return false;
}

HRGN OfficeFrameHelperWin::calcFrameRegion(QSize sz)
{
    if (getStyle() & WS_MAXIMIZE)
        return Q_NULL;

    int cx = sz.width(), cy = sz.height();

    RECT rgnTopFrame[] =
    {
        {4, 0, cx - 4, 1}, {2, 1, cx - 2, 2}, {1, 2, cx - 1, 3}, {1, 3, cx - 1, 4}, {0, 4, cx, cy - 4}
    };

    RECT rgnRibbonBottomFrame[] =
    {
        {1, cy - 4, cx - 1, cy - 2}, {2, cy - 2, cx - 2, cy - 1}, {4, cy - 1, cx - 4, cy - 0}
    };

    RECT rgnSimpleBottomFrame[] =
    {
        {0, cy - 4, cx, cy}
    };

    bool roundedCornersAlways = true;

    bool bHasStatusBar = (roundedCornersAlways || isFrameHasStatusBar()) && m_frameBorder > 3;

    int nSizeTopRect = sizeof(rgnTopFrame);
    int nSizeBottomRect = bHasStatusBar ? sizeof(rgnRibbonBottomFrame) : sizeof(rgnSimpleBottomFrame);

    int nSizeData = sizeof(RGNDATAHEADER) + nSizeTopRect + nSizeBottomRect;

    RGNDATA* pRgnData = (RGNDATA*)malloc(nSizeData);
    if (!pRgnData)
        return Q_NULL;

    memcpy(&pRgnData->Buffer, (void*)&rgnTopFrame, nSizeTopRect);
    memcpy(&pRgnData->Buffer + nSizeTopRect, bHasStatusBar ? (void*)&rgnRibbonBottomFrame : (void*)&rgnSimpleBottomFrame, nSizeBottomRect);

    pRgnData->rdh.dwSize = sizeof(RGNDATAHEADER);
    pRgnData->rdh.iType = RDH_RECTANGLES;
    pRgnData->rdh.nCount = (nSizeTopRect + nSizeBottomRect) / sizeof(RECT);
    pRgnData->rdh.nRgnSize = 0;
    pRgnData->rdh.rcBound.left = 0;
    pRgnData->rdh.rcBound.top = 0;
    pRgnData->rdh.rcBound.right = cx;
    pRgnData->rdh.rcBound.bottom = cy;

    HRGN rgh = ::ExtCreateRegion(Q_NULL, nSizeData, pRgnData);

    free(pRgnData);
    return rgh;
}

HFONT OfficeFrameHelperWin::getCaptionFont(HANDLE hTheme)
{
    LOGFONT lf = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, { 0 } };

    if (!hTheme)
       pGetThemeSysFont(hTheme, HLP_TMT_CAPTIONFONT, &lf);
    else
    {
       NONCLIENTMETRICS ncm;
       ncm.cbSize = sizeof(NONCLIENTMETRICS);
       SystemParametersInfoW(SPI_GETNONCLIENTMETRICS, sizeof(NONCLIENTMETRICS), &ncm, false);
       lf = ncm.lfMessageFont;
    }
    return ::CreateFontIndirectW(&lf);
}

bool OfficeFrameHelperWin::hitTestContextHeaders(const QPoint& point) const
{
    if (!m_listContextHeaders)
        return false;

    for ( int i = 0, count = m_listContextHeaders->count(); i < count; i++)
    {
        ContextHeader* header =  m_listContextHeaders->at(i);
        if (header->rcRect.contains(point))
           return true;
    }
    return false;
}

#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
#define PTR_ADDR long long
class QtnHackWidget : public QWidget
{
public:
    Q_DECLARE_PRIVATE(QWidget)
    QTLWExtra* topData() { return d_func()->topData(); }
    QWidgetData* data() { return qt_qwidget_data(this); }
};

void OfficeFrameHelperWin::collapseTopFrame(bool includesFrame)
{
    PTR_ADDR frameMargins_offset = 0;

    if (QSysInfo::WordSize == 32)
        frameMargins_offset = 0x00000024;
    else if (QSysInfo::WordSize == 64)
        frameMargins_offset = 0x00000030;

    if ( frameMargins_offset == 0 )
        return;

    if (QTLWExtra* top = ((QtnHackWidget*)m_frame)->topData())
    {
        if (QWindow* window = (QWindow*)top->window)
        {
            if (QPlatformWindow* platformWindow = window->handle())
            {
                int** topPtr = static_cast<int**>((void *)((PTR_ADDR)platformWindow + frameMargins_offset));
                int val = m_dwmEnabled ?  (m_ribbonBar->isVisible() ? 0 :  captionSize() + (m_frameBorder*2)) : (m_ribbonBar->isVisible() ? (m_frameBorder/2) : captionSize());
                memcpy(topPtr, &val, sizeof(int));
            }
        }

        if ( includesFrame )
        {
            QWidgetData* data = ((QtnHackWidget*)m_frame)->data();
            data->fstrut_dirty = true;
            top->posIncludesFrame = true;
            ((QtnHackWidget*)m_frame)->d_func()->fixPosIncludesFrame();
        }
        int x1, y1, x2, y2;
        top->frameStrut.getCoords(&x1, &y1, &x2, &y2);
        top->frameStrut.setCoords(x1, 0, x2, y2);
    }
}
#endif // QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)

#define qtn_GET_X_LPARAM(lp)    ((int)(short)LOWORD(lp))
#define qtn_GET_Y_LPARAM(lp)    ((int)(short)HIWORD(lp))
bool OfficeFrameHelperWin::winEvent(MSG* msg, long* result)
{
    if (m_hwndFrame && msg->hwnd != m_hwndFrame && m_ribbonBar && msg->hwnd == (HWND)m_ribbonBar->winId())
    {
        if (msg->message == WM_NCHITTEST)
        {
            POINT point;
            point.x = (short)qtn_GET_X_LPARAM((DWORD)msg->lParam);
            point.y = (short)qtn_GET_Y_LPARAM((DWORD)msg->lParam);

            RECT rc;
            ::GetWindowRect(m_hwndFrame, &rc);
            rc.bottom = rc.top + titleBarSize();
            if (::PtInRect(&rc, point))
            {
                LRESULT lResult = 0;
                lResult = DefWindowProcW(m_hwndFrame, msg->message, msg->wParam, msg->lParam);
                if (lResult == HTMINBUTTON || lResult == HTMAXBUTTON || lResult == HTCLOSE || lResult == HTHELP || lResult == HTCLIENT)
                {
                    *result = HTTRANSPARENT;
                    return true;
                }
            }
        }
        return false;
    }

    if (!m_officeFrameEnabled)
        return false;

    if (!m_dwmEnabled && m_lockNCPaint && (msg->message == WM_STYLECHANGING || msg->message == WM_STYLECHANGED
        || msg->message == WM_WINDOWPOSCHANGED || msg->message == WM_WINDOWPOSCHANGING || msg->message == WM_NCPAINT))
    {
        if (msg->message== WM_WINDOWPOSCHANGING)
            ((WINDOWPOS*)msg->lParam)->flags &= ~SWP_FRAMECHANGED;

        return true;
    }
    else if (msg->message == WM_WINDOWPOSCHANGING)
    {
        WINDOWPOS* lpwndpos = (WINDOWPOS*)msg->lParam;

        QSize szFrameRegion(lpwndpos->cx, lpwndpos->cy);

        if (((lpwndpos->flags & SWP_NOSIZE) ==  0) && (m_szFrameRegion != szFrameRegion) &&
            ((getStyle() & WS_CHILD) == 0))
        {
            m_skipNCPaint = true;
        }

        if ((lpwndpos->flags == (SWP_NOACTIVATE | SWP_NOZORDER)) && isTitleVisible())
        {
            if (m_dwmEnabled)
            {
                QWidgetData* data = qt_qwidget_data(m_frame);
                QRect qrect = data->crect;
                int collapseTop = qrect.y() - m_frame->y();
                lpwndpos->x -= frameSize()/2;
                lpwndpos->cy -= collapseTop;
            }
            else
            {
                lpwndpos->x -= frameSize()/2;
                lpwndpos->y -= frameSize()/2;
                lpwndpos->cy -= frameSize()/2 + captionSize();
            }
        }
        return false;
    }
    else if (msg->message == WM_WINDOWPOSCHANGED)
    {
        WINDOWPOS* lpwndpos = (WINDOWPOS*)msg->lParam;
        if (lpwndpos->flags & SWP_FRAMECHANGED && !m_inUpdateFrame)
            updateFrameRegion();

        if (!m_wasFullScreen)
            m_wasFullScreen = m_frame->windowState() & Qt::WindowFullScreen;
        return false;
    }
    else if (msg->message == WM_SIZE || msg->message == WM_STYLECHANGED)
    {
        RECT rc;
        ::GetWindowRect(m_hwndFrame, &rc);

        QSize szFrameRegion(rc.right - rc.left, rc.bottom - rc.top);

        if ((m_szFrameRegion != szFrameRegion) || (msg->message == WM_STYLECHANGED))
        {
            updateFrameRegion(szFrameRegion, (msg->message == WM_STYLECHANGED));
            redrawFrame();

            if (!m_dwmEnabled && (msg->message == WM_STYLECHANGED))
            {
                ::SetWindowPos(m_hwndFrame, 0, 0, 0, 0, 0, SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED);
                ::RedrawWindow(m_hwndFrame, &rc, 0, RDW_INVALIDATE | RDW_UPDATENOW | RDW_FRAME);
            }
        }

        if (msg->message == WM_SIZE && msg->wParam == SIZE_RESTORED && m_inLayoutRequest)
        {
            QResizeEvent e(m_ribbonBar->size(), m_ribbonBar->size());
            QApplication::sendEvent(m_ribbonBar, &e);
            m_inLayoutRequest = false;
        }

        if (msg->message == WM_SIZE && msg->wParam == SIZE_MINIMIZED)
        {
            if (getStyle(FALSE) & WS_MAXIMIZE)
                modifyStyle(m_hwndFrame, WS_MAXIMIZE, 0, 0);
        }

#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
        if (msg->message == WM_SIZE && (msg->wParam == SIZE_MAXIMIZED || msg->wParam == SIZE_MINIMIZED || m_wasFullScreen))
        {
//            QApplication::postEvent(m_ribbonBar, new QEvent(QEvent::LayoutRequest));
            m_wasFullScreen = false;
            return true;
        }
#endif // QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
        return false;
    }
    else if (msg->message == WM_ENTERSIZEMOVE)
    {
        m_inLayoutRequest = true;
    }
    else if (msg->message == WM_NCRBUTTONUP)
    {
        // call the system menu
        ::SendMessageW(m_hwndFrame, 0x0313, (WPARAM)m_hwndFrame, msg->lParam);
        return true;
    }
    else if (msg->message == WM_SETTEXT)
    {
        *result = DefWindowProcW(msg->hwnd, msg->message, msg->wParam, msg->lParam);

        QEvent e(QEvent::WindowTitleChange);
        QApplication::sendEvent(m_ribbonBar, &e);
        return true;
    }
    else if (msg->message == WM_NCHITTEST && !m_dwmEnabled)
    {
        if (!isTitleVisible())
        {
            *result = DefWindowProcW(msg->hwnd, msg->message, msg->wParam, msg->lParam);

            POINT point;
            point.x = (short)qtn_GET_X_LPARAM((DWORD)msg->lParam);
            point.y = (short)qtn_GET_Y_LPARAM((DWORD)msg->lParam);

            if (*result == HTCLIENT)
            {
                RECT rc;
                ::GetWindowRect(m_hwndFrame, &rc);

                rc.bottom = rc.top + m_frameBorder;
                if (::PtInRect(&rc, point))
                    *result = HTTOP;

                if (*result == HTCLIENT)
                {
                    QPoint pos(m_ribbonBar->mapFromGlobal(QPoint(point.x, point.y)));
                    if (m_ribbonBar && m_rcHeader.isValid())
                    {
                        if (!hitTestContextHeaders(pos) && m_rcHeader.contains(pos))
                            *result = HTCAPTION;
                    }
                    else
                    {
                        rc.bottom = rc.top + titleBarSize();
                        if (::PtInRect(&rc, point))
                            *result = HTCAPTION;
                    }

                    if (pos.y() < m_rcHeader.height() &&  m_ribbonBar->getSystemButton() && 
                        m_ribbonBar->getSystemButton()->toolButtonStyle() != Qt::ToolButtonFollowStyle)
                    {
                        DWORD dwStyle = getStyle();
                        if (pos.x() < 7 + GetSystemMetrics(SM_CXSMICON) && (dwStyle & WS_SYSMENU))
                            *result = HTSYSMENU;
                    }
                }
            }
        }
        else
        {
            *result = DefWindowProcW(msg->hwnd, msg->message, msg->wParam, msg->lParam);
            if (*result == HTCLOSE || *result == HTMAXBUTTON || *result == HTMINBUTTON || *result == HTHELP)
                *result = HTCAPTION;
        }

        return true;
    }
    else if (msg->message == WM_NCHITTEST  && !isTitleVisible() && m_dwmEnabled)
    {
        POINT point, clientPoint;
        point.x = (short)qtn_GET_X_LPARAM((DWORD)msg->lParam);
        point.y = (short)qtn_GET_Y_LPARAM((DWORD)msg->lParam);
        clientPoint.x = point.x;
        clientPoint.y = point.y;
        ::ScreenToClient(m_hwndFrame, &clientPoint);

        LRESULT lResult;
        // Perform hit testing using DWM
        if (pDwmDefWindowProc(msg->hwnd, msg->message, msg->wParam, msg->lParam, &lResult)) 
        {
            // DWM returned a hit, no further processing necessary
            *result = lResult;
        } 
        else 
        {
            // DWM didn't return a hit, process using DefWindowProc
            lResult = DefWindowProc(msg->hwnd, msg->message, msg->wParam, msg->lParam);
            // If DefWindowProc returns a window caption button, just return HTCLIENT (client area).
            // This avoid unnecessary hits to Windows NT style caption buttons which aren't visible but are
            // located just under the Aero style window close button.
            if (lResult == HTCLOSE || lResult == HTMAXBUTTON || lResult == HTMINBUTTON || lResult == HTHELP)
                *result = HTCLIENT;
            else
                *result = lResult;
        }

        if ( *result == HTCLIENT || *result == HTSYSMENU)
        {
            RECT rc;
            ::GetWindowRect(m_hwndFrame, &rc);
            rc.left = rc.left + m_frameBorder * 2;
            rc.bottom = rc.top + captionSize() + m_frameBorder * 2;
            rc.right = rc.right - m_frameBorder * 2;

            if (::PtInRect(&rc, point))
            {
                if (QWidget* quickAccessBar = (QWidget*)m_ribbonBar->getQuickAccessBar())
                {
                    QRect rectAccessBar = quickAccessBar->rect();
                    QPoint pos(quickAccessBar->mapFromGlobal(QPoint(point.x, point.y)));
                    if (rectAccessBar.contains(pos))
                        return true;
                }

                if (QWidget* systemButton = (QWidget*)m_ribbonBar->getSystemButton())
                {
                    QRect rectSystemButton = systemButton->rect();
                    QPoint pos(systemButton->mapFromGlobal(QPoint(point.x, point.y)));
                    if (rectSystemButton.contains(pos))
                    {
                        *result = HTCLIENT;
                        return true;
                    }
                }

                QPoint pos(m_ribbonBar->mapFromGlobal(QPoint(point.x, point.y)));
                if (hitTestContextHeaders(pos))
                    return true;

                DWORD dwStyle = getStyle();
                if ((point.y < rc.top + m_frameBorder) && (dwStyle & WS_SIZEBOX) && ((dwStyle & WS_MAXIMIZE) == 0))
                {
                    *result = HTTOP;
                    return true;
                }
                if (*result != HTSYSMENU)
                    *result = HTCAPTION;
            }
        }
        return true;
    }
    else if (msg->message == WM_NCACTIVATE && !m_dwmEnabled)
    {
        bool active = (bool)msg->wParam;

        if (!::IsWindowEnabled(m_hwndFrame))
            active = true;

        DWORD dwStyle = getStyle();

        if (dwStyle & WS_VISIBLE)
        {
            refreshFrameStyle();

            m_lockNCPaint = true;

            if (dwStyle & WS_SIZEBOX)
                modifyStyle(m_hwndFrame, WS_SIZEBOX, 0, 0);

            if (::IsWindowEnabled(m_hwndFrame))
                *result = DefWindowProcW(msg->hwnd, msg->message, active, 0);
            else
                *result = DefWindowProcW(msg->hwnd, msg->message, msg->wParam, msg->lParam);

            if (dwStyle & WS_SIZEBOX)
                modifyStyle(m_hwndFrame, 0, WS_SIZEBOX, 0);

            m_lockNCPaint = false;

            if (m_active != active)
            {
                m_active = active;
                redrawFrame();
            }
        }
        else
        {
            m_active = active;
            return false;
        }
        return true;
    }
    else if (msg->message == HLP_WM_DWMCOMPOSITIONCHANGED)
    {
        bool dwmEnabled = !m_allowDwm || getStyle() & WS_CHILD ? FALSE : isCompositionEnabled();

        if (dwmEnabled != m_dwmEnabled)
        {
            m_dwmEnabled = dwmEnabled;

            ::SetWindowRgn(m_hwndFrame, 0, true);

            if (!m_dwmEnabled)
                refreshFrameStyle();

            ::SetWindowPos(m_hwndFrame, 0, 0, 0, 0, 0, SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED);
        }
        return false;
    }
    else if (msg->message == WM_NCCALCSIZE && !m_lockNCPaint)
    {
        NCCALCSIZE_PARAMS FAR* lpncsp = (NCCALCSIZE_PARAMS FAR*)msg->lParam;

        RECT rc;
        rc.left    = lpncsp->rgrc[0].left;
        rc.top     = lpncsp->rgrc[0].top;
        rc.right   = lpncsp->rgrc[0].right;
        rc.bottom  = lpncsp->rgrc[0].bottom;

#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
        collapseTopFrame();
#endif // QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)

        if (m_dwmEnabled)
            *result = DefWindowProcW(msg->hwnd, msg->message, msg->wParam, msg->lParam);
        else
        {
            lpncsp->rgrc[0].left   += m_frameBorder;
            lpncsp->rgrc[0].top    += m_frameBorder;
            lpncsp->rgrc[0].right  -= m_frameBorder;
            lpncsp->rgrc[0].bottom -= m_frameBorder;
        }

        DWORD dwStyle = getStyle();
        if (m_dwmEnabled)
        {
            if (!isTitleVisible())
                lpncsp->rgrc[0].top = rc.top;
        }
        else
        {
            lpncsp->rgrc[0].top += (isTitleVisible() ? titleBarSize() - m_frameBorder :  0);
            if (isFrameHasStatusBar() && ((dwStyle & WS_MAXIMIZE) == 0))
            {
                int borderSize = qMax(rc.bottom - lpncsp->rgrc[0].bottom - 3, 1L);
                m_borderSizeBotton = borderSize;
                lpncsp->rgrc[0].bottom = rc.bottom - borderSize;
            }
        }

        if (((dwStyle & (WS_MAXIMIZE | WS_CHILD)) == WS_MAXIMIZE))
            lpncsp->rgrc[0].bottom -= 1;

        return true;
    }
    else if (msg->message == WM_SYSCOMMAND && !m_dwmEnabled && msg->wParam == SC_MINIMIZE && getStyle() & WS_CHILD)
    {
        *result = DefWindowProcW(msg->hwnd, msg->message, msg->wParam, msg->lParam);

        redrawFrame();
        return true;
    }

    else if (msg->message == WM_GETMINMAXINFO )
    {
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
        collapseTopFrame(true);
#endif // QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
        if (!m_dwmEnabled)
        {
            *result = DefWindowProcW(msg->hwnd, msg->message, msg->wParam, msg->lParam);

            MINMAXINFO* lpMMI = (MINMAXINFO*)msg->lParam;

            int yMin = m_frameBorder + titleBarSize();
            int xMin = (int)3 * yMin;

            xMin += GetSystemMetrics(SM_CYSIZE) + 2 * GetSystemMetrics(SM_CXEDGE);

            lpMMI->ptMinTrackSize.x = qMax(lpMMI->ptMinTrackSize.x, (LONG)xMin);
            lpMMI->ptMinTrackSize.y = qMax(lpMMI->ptMinTrackSize.y, (LONG)yMin);

            return true;
        }
    }
    else if (msg->message == WM_NCPAINT && !m_dwmEnabled)
    {
        if (m_skipNCPaint)
        {
            m_skipNCPaint = false;
            return true;
        }

        if (!isMDIMaximized())
            redrawFrame();

        return true;
    }

    if (msg->message == OfficeFrameHelperWin::m_msgGetFrameHook)
    {
        ::SetProp(msg->hwnd, HLP_WIN_PTR, this);
        LRESULT lResult = (LRESULT)this;
        *result = lResult;
        return true;
    }
    return false;
}

void OfficeFrameHelperWin::refreshFrameTimer()
{
    refreshFrameStyle();
    m_refreshFrame.stop();
}

bool OfficeFrameHelperWin::event(QEvent* event)
{
    if ( m_postReclalcLayout && event->type() == QEvent::LayoutRequest )
        recalcFrameLayout();
    return QObject::event(event);
}

bool OfficeFrameHelperWin::eventFilter(QObject* obj, QEvent* event)
{
    if (obj == m_frame)
    {
        if (event->type() == QEvent::Resize)
        {
            QEvent e(QEvent::EmbeddingControl);
            QCoreApplication::sendEvent(m_frame, &e);
        }
        else if (event->type() == QEvent::StyleChange)
        {
            updateFrameRegion();
            redrawFrame();
        }
        else if (event->type() == QEvent::WindowStateChange)
        {
            if (m_frame->windowState() & Qt::WindowFullScreen)
            {
                m_ribbonBar->setFrameThemeEnabled(false);
                return QObject::eventFilter(obj, event);
            }

            bool updates = m_frame->updatesEnabled();
            if (updates)
                m_frame->setUpdatesEnabled(false);

            QApplication::postEvent(m_ribbonBar, new QEvent(QEvent::LayoutRequest));

            if (updates)
                m_frame->setUpdatesEnabled(updates);
        }
        else if (event->type() == QEvent::Show)
        {
            if (!(m_frame->windowState() & Qt::WindowMaximized) && !(m_frame->windowState() & Qt::WindowMinimized))
                ::SetWindowPos(m_hwndFrame, 0, 0, 0, 0, 0, SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED);
        }
    }
    return QObject::eventFilter(obj, event);
}

bool OfficeFrameHelperWin::isCompositionEnabled()
{
    if (pDwmIsCompositionEnabled) 
    {
        BOOL enabled;
        HRESULT hr = pDwmIsCompositionEnabled(&enabled);

        OfficeStyle *pOfficeStyle = dynamic_cast<OfficeStyle *>(m_frame->style());
        if (pOfficeStyle != NULL && pOfficeStyle->m_ignoreGlass)
        {
            enabled = false;
        }
        return (SUCCEEDED(hr) && enabled);
    }
    return false;
}

bool OfficeFrameHelperWin::resolveSymbols()
{
    static bool tried = false;
    if (!tried) 
    {
        tried = true;
        QLibrary dwmLib(QString::fromLatin1("dwmapi"));
        pDwmIsCompositionEnabled = (PtrDwmIsCompositionEnabled)dwmLib.resolve("DwmIsCompositionEnabled");
        if (pDwmIsCompositionEnabled) 
        {
            pDwmDefWindowProc = (PtrDwmDefWindowProc)dwmLib.resolve("DwmDefWindowProc");
            pDwmExtendFrameIntoClientArea = (PtrDwmExtendFrameIntoClientArea)dwmLib.resolve("DwmExtendFrameIntoClientArea");
        }
        QLibrary themeLib(QString::fromLatin1("uxtheme"));
        pIsAppThemed = (PtrIsAppThemed)themeLib.resolve("IsAppThemed");

        if (pIsAppThemed) 
        {
//            pDrawThemeBackground = (PtrDrawThemeBackground)themeLib.resolve("DrawThemeBackground");
//            pGetThemeColor = (PtrGetThemeColor)themeLib.resolve("GetThemeColor");
//            pIsThemeActive = (PtrIsThemeActive)themeLib.resolve("IsThemeActive");
            pOpenThemeData = (PtrOpenThemeData)themeLib.resolve("OpenThemeData");
//            pCloseThemeData = (PtrCloseThemeData)themeLib.resolve("CloseThemeData");
            pGetThemeSysFont = (PtrGetThemeSysFont)themeLib.resolve("GetThemeSysFont");
            pDrawThemeTextEx = (PtrDrawThemeTextEx)themeLib.resolve("DrawThemeTextEx");
        }
    }
/*
    return (pDwmIsCompositionEnabled != 0 && pDwmDefWindowProc != 0 && pDwmExtendFrameIntoClientArea != 0 && 
            pIsAppThemed != 0 && pDrawThemeBackground != 0 && pGetThemePartSize != 0 && pGetThemeColor != 0 && 
            pIsThemeActive != 0 && pOpenThemeData != 0 && pCloseThemeData != 0 && pGetThemeSysFont != 0 && 
            pDrawThemeTextEx != 0);
*/
    return (pIsAppThemed != 0 && /*pDrawThemeBackground != 0 && pGetThemePartSize != 0 && pGetThemeColor != 0 &&
            pIsThemeActive != 0 &&*/ pOpenThemeData != 0 && /*pCloseThemeData != 0 &&*/ pGetThemeSysFont != 0);
}

