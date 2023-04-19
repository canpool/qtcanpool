/**
 * Copyleft (C) 2023 maminjie <canpool@163.com>
 **/
#include <QApplication>
#include <QDesktopWidget>
#include <QLayout>
#include <QLibrary>
#include <QPaintEngine>
#include <QPainter>
#include <QPixmap>
#include <QStatusBar>
#include <QStyleOption>
#include <QToolButton>
#include <QWidget>
#include <qevent.h>

#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
    #include <qpa/qplatformnativeinterface.h>
    #include <qwindow.h>
#endif

#include "officeframehelper_win.h"
#include "ribbonbackstageview.h"
#include "ribbonbar.h"
#include "ribbonbar_p.h"
#include "ribbonmainwindow.h"
#include "ribbonstyle.h"
#include "ribbonsystempopupbar.h"
#include "ribbontabbar.h"
#include "stylehelpers.h"

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

// DWM related
typedef struct   // MARGINS
{
    int cxLeftWidth;      // width of left border that retains its size
    int cxRightWidth;     // width of right border that retains its size
    int cyTopHeight;      // height of top border that retains its size
    int cyBottomHeight;   // height of bottom border that retains its size
} HLP_MARGINS;

typedef struct   // DTTOPTS
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

typedef struct {
    DWORD dwFlags;
    DWORD dwMask;
} HLP_WTA_OPTIONS;

#define HLP_WM_THEMECHANGED          0x031A
#define HLP_WM_DWMCOMPOSITIONCHANGED 0x031E

enum HLP_WINDOWTHEMEATTRIBUTETYPE {
    HLP_WTA_NONCLIENT = 1
};

#define HLP_WTNCA_NODRAWCAPTION 0x00000001
#define HLP_WTNCA_NODRAWICON    0x00000002

#define HLP_DT_CENTER     0x00000001   // DT_CENTER
#define HLP_DT_VCENTER    0x00000004
#define HLP_DT_SINGLELINE 0x00000020
#define HLP_DT_NOPREFIX   0x00000800

enum HLP_NAVIGATIONPARTS   // NAVIGATIONPARTS
{
    HLP_NAV_BACKBUTTON = 1,
    HLP_NAV_FORWARDBUTTON = 2,
    HLP_NAV_MENUBUTTON = 3,
};

enum HLP_NAV_BACKBUTTONSTATES   // NAV_BACKBUTTONSTATES
{
    HLP_NAV_BB_NORMAL = 1,
    HLP_NAV_BB_HOT = 2,
    HLP_NAV_BB_PRESSED = 3,
    HLP_NAV_BB_DISABLED = 4,
};

#define HLP_AUTOHIDE_LEFT   0x0001
#define HLP_AUTOHIDE_RIGHT  0x0002
#define HLP_AUTOHIDE_TOP    0x0004
#define HLP_AUTOHIDE_BOTTOM 0x0008

#define HLP_TMT_CAPTIONFONT (801)   // TMT_CAPTIONFONT

//---- bits used in dwFlags of DTTOPTS ----
#define HLP_DTT_TEXTCOLOR    (1UL << 0)    // crText has been specified
#define HLP_DTT_BORDERCOLOR  (1UL << 1)    // crBorder has been specified
#define HLP_DTT_SHADOWCOLOR  (1UL << 2)    // crShadow has been specified
#define HLP_DTT_SHADOWTYPE   (1UL << 3)    // iTextShadowType has been specified
#define HLP_DTT_SHADOWOFFSET (1UL << 4)    // ptShadowOffset has been specified
#define HLP_DTT_BORDERSIZE   (1UL << 5)    // iBorderSize has been specified
#define HLP_DTT_FONTPROP     (1UL << 6)    // iFontPropId has been specified
#define HLP_DTT_COLORPROP    (1UL << 7)    // iColorPropId has been specified
#define HLP_DTT_STATEID      (1UL << 8)    // IStateId has been specified
#define HLP_DTT_CALCRECT     (1UL << 9)    // Use pRect as and in/out parameter
#define HLP_DTT_APPLYOVERLAY (1UL << 10)   // fApplyOverlay has been specified
#define HLP_DTT_GLOWSIZE     (1UL << 11)   // iGlowSize has been specified
#define HLP_DTT_CALLBACK     (1UL << 12)   // pfnDrawTextCallback has been specified
#define HLP_DTT_COMPOSITED   (1UL << 13)   // Draws text with antialiased alpha (needs a DIB section)

#define HLP_WM_NCMOUSELEAVE 674   // WM_NCMOUSELEAVE

#define HLP_WP_CAPTION          1   // WP_CAPTION
#define HLP_WP_MAXCAPTION       5   // WP_MAXCAPTION
#define HLP_CS_ACTIVE           1   // CS_ACTIVE
#define HLP_FS_ACTIVE           1
#define HLP_FS_INACTIVE         2
#define HLP_TMT_FILLCOLORHINT   3821   // TMT_FILLCOLORHINT
#define HLP_TMT_BORDERCOLORHINT 3822   // TMT_BORDERCOLORHINT

typedef BOOL(WINAPI *PtrDwmDefWindowProc)(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam, LRESULT *plResult);
typedef HRESULT(WINAPI *PtrDwmIsCompositionEnabled)(BOOL *pfEnabled);
typedef HRESULT(WINAPI *PtrDwmSetWindowAttribute)(HWND, DWORD, LPCVOID, DWORD);
typedef HRESULT(WINAPI *PtrDwmExtendFrameIntoClientArea)(HWND hWnd, const HLP_MARGINS *pMarInset);
typedef HRESULT(WINAPI *PtrSetWindowThemeAttribute)(HWND hwnd, enum HLP_WINDOWTHEMEATTRIBUTETYPE eAttribute,
                                                    PVOID pvAttribute, DWORD cbAttribute);

static PtrDwmDefWindowProc pDwmDefWindowProc = 0;
static PtrDwmIsCompositionEnabled pDwmIsCompositionEnabled = 0;
static PtrDwmSetWindowAttribute pDwmSetWindowAttribute = 0;
static PtrDwmExtendFrameIntoClientArea pDwmExtendFrameIntoClientArea = 0;

// Theme related
typedef bool(WINAPI *PtrIsAppThemed)();
typedef HANDLE(WINAPI *PtrOpenThemeData)(HWND hwnd, LPCWSTR pszClassList);
typedef HRESULT(WINAPI *PtrGetThemeSysFont)(HANDLE hTheme, int iFontId, LOGFONTW *plf);
typedef HRESULT(WINAPI *PtrDrawThemeTextEx)(HANDLE hTheme, HDC hdc, int iPartId, int iStateId, LPCWSTR pszText,
                                            int cchText, DWORD dwTextFlags, LPRECT pRect, const HLP_DTTOPTS *pOptions);
typedef HRESULT(WINAPI *PtrDrawThemeIcon)(HANDLE hTheme, HDC hdc, int iPartId, int iStateId, const RECT *pRect,
                                          HIMAGELIST himl, int iImageIndex);

enum WIS_UX_BUFFERFORMAT {
    WIS_BPBF_COMPATIBLEBITMAP,   // Compatible bitmap
    WIS_BPBF_DIB,                // Device-independent bitmap
    WIS_BPBF_TOPDOWNDIB,         // Top-down device-independent bitmap
    WIS_BPBF_TOPDOWNMONODIB      // Top-down monochrome device-independent bitmap
};

// BP_PAINTPARAMS
struct WIS_UX_PAINTPARAMS {
    DWORD cbSize;
    DWORD dwFlags;   // BPPF_ flags
    const RECT *prcExclude;
    const BLENDFUNCTION *pBlendFunction;
};

static PtrIsAppThemed pIsAppThemed = 0;
static PtrOpenThemeData pOpenThemeData = 0;
static PtrGetThemeSysFont pGetThemeSysFont = 0;
static PtrDrawThemeTextEx pDrawThemeTextEx = 0;
static PtrDrawThemeIcon pDrawThemeIcon = 0;

typedef struct {
    OfficeFrameHelperWin *frameHelper;
} HLP_FRAMEHOOK;

/* OfficeFrameHelperWin */
class OfficeFrameHelperWin::FrameHelperEventHook
{
public:
    FrameHelperEventHook();
    ~FrameHelperEventHook();
public:
    void setEventHook();
    void removeEventHook();
protected:
    typedef void(CALLBACK *WINEVENTPROC)(HWINEVENTHOOK hWinEventHook, DWORD event, HWND hwnd, LONG idObject,
                                         LONG idChild, DWORD idEventThread, DWORD dwmsEventTime);
    typedef HWINEVENTHOOK(WINAPI *LPFNSETWINEVENTHOOK)(UINT eventMin, UINT eventMax, HMODULE hmodWinEventProc,
                                                       WINEVENTPROC lpfnWinEventProc, DWORD idProcess, DWORD idThread,
                                                       UINT dwflags);
    typedef BOOL(WINAPI *LPFNUNHOOKWINEVENT)(HWINEVENTHOOK hWinEventHook);

    static void CALLBACK WinEventProc(HWINEVENTHOOK /*hWinEventHook*/, DWORD event, HWND hWnd, LONG idObject,
                                      LONG /*idChild*/, DWORD /*dwEventThread*/, DWORD /*dwmsEventTime*/);
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

OfficeFrameHelperWin::FrameHelperEventHook::~FrameHelperEventHook() {}

void OfficeFrameHelperWin::FrameHelperEventHook::setEventHook()
{
    QLibrary dwmLib(QString::fromLatin1("user32"));
    m_pSetWinEventHook = (LPFNSETWINEVENTHOOK)dwmLib.resolve("SetWinEventHook");

    m_pUnhookWinEvent = (LPFNUNHOOKWINEVENT)dwmLib.resolve("UnhookWinEvent");
    if (m_pSetWinEventHook && m_pUnhookWinEvent)
        m_hWinEventHook =
            m_pSetWinEventHook(EVENT_OBJECT_REORDER, EVENT_OBJECT_REORDER, Q_NULL,
                               &OfficeFrameHelperWin::FrameHelperEventHook::WinEventProc, GetCurrentProcessId(), 0, 0);
}

void OfficeFrameHelperWin::FrameHelperEventHook::removeEventHook()
{
    if (m_hWinEventHook && m_pUnhookWinEvent)
        m_pUnhookWinEvent(m_hWinEventHook);
    m_hWinEventHook = Q_NULL;
}

void CALLBACK OfficeFrameHelperWin::FrameHelperEventHook::WinEventProc(HWINEVENTHOOK /*hWinEventHook*/, DWORD event,
                                                                       HWND hWnd, LONG idObject, LONG /*idChild*/,
                                                                       DWORD /*dwEventThread*/, DWORD /*dwmsEventTime*/)
{
    if (event == EVENT_OBJECT_REORDER && idObject == OBJID_WINDOW) {
        HLP_FRAMEHOOK framehook;
        framehook.frameHelper = Q_NULL;
        ::SendMessage(hWnd, OfficeFrameHelperWin::m_msgGetFrameHook, 0, (LPARAM)&framehook);
        OfficeFrameHelperWin *frameHelperWin = framehook.frameHelper;

        if (frameHelperWin) {
            if (hWnd != frameHelperWin->m_hwndFrame)
                return;

            if (frameHelperWin && !frameHelperWin->isDwmEnabled()) {
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
    \class QRibbon::OfficeFrameHelperWin
    \internal
*/
OfficeFrameHelperWin::OfficeFrameHelperWin(QWidget *parent) : QObject(parent)
{
    m_hIcon = Q_NULL;
    m_szIcon = QSize(0, 0);
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
    m_closeWindow = false;
    m_isTransparentCaption = false;
    m_shellAutohideBars = 0;
    m_shellAutohideBarsInitialized = false;

    m_changedSize = SIZE_RESTORED;
    m_isTheme2013 = false;

    m_oldMarginsleft = 0;
    m_oldMarginstop = 0;
    m_oldMarginsright = 0;
    m_oldMarginsbottom = 0;

    if (m_msgGetFrameHook == 0)
        m_msgGetFrameHook = RegisterWindowMessageW(L"WM_GETFRAMEHOOK");

    connect(&m_refreshFrame, SIGNAL(timeout()), this, SLOT(refreshFrameTimer()));
    resolveSymbols();
}

OfficeFrameHelperWin::~OfficeFrameHelperWin()
{
    if (m_frame)
        m_frame->removeEventFilter(this);

    if (RibbonMainWindow *mainWindow = qobject_cast<RibbonMainWindow *>(m_frame))
        mainWindow->setFrameHelper(Q_NULL);

    if (m_officeFrameEnabled)
        enableOfficeFrame(Q_NULL);

    resetWindowIcon();
}

void OfficeFrameHelperWin::enableOfficeFrame(QWidget *widget)
{
    bool enabled = widget != Q_NULL;
    if (m_officeFrameEnabled == enabled)
        return;

#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
    if (!enabled)
        restoreMargins();
#endif // QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)

    m_ribbonBar = qobject_cast<RibbonBar *>(parent());
    m_frame = widget;
    m_active = true;
    m_officeFrameEnabled = enabled;
    m_postReclalcLayout = false;

    if (RibbonMainWindow *mainWindow = qobject_cast<RibbonMainWindow *>(m_frame))
        mainWindow->setFrameHelper(this);

    if (m_frame) {
        m_hwndFrame = (HWND)m_frame->winId();
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
        saveMargins();
#endif // QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)

        enableWindowAero(m_hwndFrame, isCompositionEnabled());
        // enableWindowShadow(m_hwndFrame, isSmallSystemBorders() && (getStyle() & WS_CHILD) == 0);

        m_dwmEnabled = !m_allowDwm || getStyle() & WS_CHILD ? false : isCompositionEnabled();

        m_frame->installEventFilter(this);

        if (!m_dwmEnabled)
            refreshFrameStyle();
        else
            SetWindowRgn(m_hwndFrame, Q_NULL, true);

        updateFrameRegion();

        ::SetWindowPos(m_hwndFrame, 0, 0, 0, 0, 0,
                       SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED);
        setFrameHook();

#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
        // collapseTopFrame();
#endif // QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)

        QApplication::postEvent(m_ribbonBar, new QEvent(QEvent::LayoutRequest));
    } else {
        disableOfficeFrame();
    }
}

void OfficeFrameHelperWin::disableOfficeFrame()
{
    if (m_frame)
        m_frame->removeEventFilter(this);

    if (RibbonMainWindow *mainWindow = qobject_cast<RibbonMainWindow *>(m_frame))
        mainWindow->setFrameHelper(Q_NULL);

    m_frame = Q_NULL;
    m_ribbonBar = Q_NULL;
    m_officeFrameEnabled = false;
    m_postReclalcLayout = false;

    if (!m_closeWindow)
        enableWindowAero(m_hwndFrame, isCompositionEnabled());
    //    enableWindowShadow(m_hwndFrame, false);

    if (m_hwndFrame) {
        if (m_dwmEnabled) {
            HLP_MARGINS margins;
            margins.cxLeftWidth = 0;
            margins.cyTopHeight = 0;
            margins.cxRightWidth = 0;
            margins.cyBottomHeight = 0;
            pDwmExtendFrameIntoClientArea(m_hwndFrame, &margins);
        }

        ::SetWindowRgn(m_hwndFrame, (HRGN)Q_NULL, true);
        ::SetWindowPos(m_hwndFrame, 0, 0, 0, 0, 0,
                       SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED);
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

#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
Q_DECLARE_METATYPE(QMargins)
void OfficeFrameHelperWin::saveMargins()
{
    if (QWindow *window = m_frame->windowHandle()) {
        if (QPlatformWindow *platformWindow = window->handle()) {
            QMargins oldcustomMargins =
                qvariant_cast<QMargins>(QGuiApplication::platformNativeInterface()->windowProperty(
                    platformWindow, QStringLiteral("WindowsCustomMargins")));
            m_oldMarginsleft = oldcustomMargins.left();
            m_oldMarginstop = oldcustomMargins.top();
            m_oldMarginsright = oldcustomMargins.right();
            m_oldMarginsbottom = oldcustomMargins.bottom();
        }
    }
}

void OfficeFrameHelperWin::restoreMargins()
{
    if (QWindow *window = m_frame->windowHandle()) {
        if (QPlatformWindow *platformWindow = window->handle()) {
            RECT rc;
            ::GetWindowRect(m_hwndFrame, &rc);

            // Reduce top frame to zero since we paint it ourselves.
            const QMargins customMargins =
                QMargins(m_oldMarginsleft, m_oldMarginstop, m_oldMarginsright, m_oldMarginsbottom);
            const QVariant customMarginsV = QVariant::fromValue(customMargins);

            // The dynamic property takes effect when creating the platform window.
            window->setProperty("_q_windowsCustomMargins", customMarginsV);
            // If a platform window exists, change via native interface.
            QGuiApplication::platformNativeInterface()->setWindowProperty(
                platformWindow, QStringLiteral("WindowsCustomMargins"), customMarginsV);

            ::SetWindowPos(m_hwndFrame, Q_NULL, rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top,
                           SWP_NOZORDER | SWP_NOACTIVATE);
        }
    }
}
#endif   // QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)

bool OfficeFrameHelperWin::isDwmEnabled() const { return m_dwmEnabled; }

bool OfficeFrameHelperWin::isActive() const { return m_active; }

bool OfficeFrameHelperWin::isMaximize() const { return getStyle() & WS_MAXIMIZE; }

bool OfficeFrameHelperWin::canHideTitle() const { return isTheme2013(); }

int OfficeFrameHelperWin::frameBorder() const { return m_frameBorder; }

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

struct QTC_DLLVERSIONINFO {
    DWORD cbSize;
    DWORD dwMajorVersion;   // Major version
    DWORD dwMinorVersion;   // Minor version
    DWORD dwBuildNumber;    // Build number
    DWORD dwPlatformID;     // DLLVER_PLATFORM_*
};

typedef HRESULT(CALLBACK *QTC_DLLGETVERSIONPROC)(QTC_DLLVERSIONINFO *);

#define QTC_VERSION_WIN4 MAKELONG(0, 4)
static DWORD comCtlVersion()
{
    static DWORD dwComCtlVersion = (DWORD)-1;
    if (dwComCtlVersion != (DWORD)-1)
        return dwComCtlVersion;

    QLibrary ctlLib(QString::fromLatin1("COMCTL32"));

    QTC_DLLGETVERSIONPROC pfn;
    pfn = (QTC_DLLGETVERSIONPROC)ctlLib.resolve("DllGetVersion");
    Q_ASSERT(pfn != NULL);

    DWORD dwVersion = QTC_VERSION_WIN4;

    if (pfn != NULL) {
        QTC_DLLVERSIONINFO dvi;
        memset(&dvi, 0, sizeof(dvi));
        dvi.cbSize = sizeof(dvi);
        HRESULT hr = (*pfn)(&dvi);
        if (SUCCEEDED(hr)) {
            Q_ASSERT(dvi.dwMajorVersion <= 0xFFFF);
            Q_ASSERT(dvi.dwMinorVersion <= 0xFFFF);
            dwVersion = MAKELONG(dvi.dwMinorVersion, dvi.dwMajorVersion);
        }
    }

    dwComCtlVersion = dwVersion;
    return dwComCtlVersion;
}

bool OfficeFrameHelperWin::clientMetrics(int &iCaptionWidth, int &iCaptionHeight) const
{
    NONCLIENTMETRICS ncm;
    struct QTC_OLDNONCLIENTMETRICS {
        UINT cbSize;
        int iBorderWidth;
        int iScrollWidth;
        int iScrollHeight;
        int iCaptionWidth;
        int iCaptionHeight;
        LOGFONT lfCaptionFont;
        int iSmCaptionWidth;
        int iSmCaptionHeight;
        LOGFONT lfSmCaptionFont;
        int iMenuWidth;
        int iMenuHeight;
        LOGFONT lfMenuFont;
        LOGFONT lfStatusFont;
        LOGFONT lfMessageFont;
    };

    const UINT cbProperSize =
        (comCtlVersion() < MAKELONG(1, 6)) ? sizeof(QTC_OLDNONCLIENTMETRICS) : sizeof(NONCLIENTMETRICS);

    ZeroMemory(&ncm, sizeof(NONCLIENTMETRICS));
    ncm.cbSize = cbProperSize;

    if (::SystemParametersInfo(SPI_GETNONCLIENTMETRICS, cbProperSize, &ncm, 0)) {
        iCaptionWidth = ncm.iCaptionWidth;
        iCaptionHeight = ncm.iCaptionHeight;
        return true;
    }
    return false;
}

void OfficeFrameHelperWin::setHeaderRect(const QRect &rcHeader) { m_rcHeader = rcHeader; }

void OfficeFrameHelperWin::setContextHeader(QList<ContextHeader *> *listContextHeaders)
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

    UINT id = SC_CLOSE;
    switch (item) {
    case QStyle::SC_TitleBarCloseButton:
        id = SC_CLOSE;
        break;
    case QStyle::SC_TitleBarNormalButton:
        id = SC_RESTORE;
        break;
    case QStyle::SC_TitleBarMaxButton:
        id = SC_MAXIMIZE;
        break;
    case QStyle::SC_TitleBarMinButton:
        id = SC_MINIMIZE;
        break;
    default:
        break;
    }

    HMENU hMenu = ::GetSystemMenu(m_hwndFrame, 0);
    Q_ASSERT(::IsMenu(hMenu));
    if (hMenu) {
        // offer no buffer first
        int len = ::GetMenuStringW(hMenu, id, Q_NULL, 0, MF_BYCOMMAND);

        // use exact buffer length
        if (len > 0) {
            ushort *lpBuf = new ushort[len + 1];
            ::GetMenuStringW(hMenu, id, (LPWSTR)lpBuf, len + 1, MF_BYCOMMAND);

            QString str = QString::fromUtf16(lpBuf, len);
            int index = str.indexOf(QLatin1String("\t"));
            if (index > 0)
                str = str.left(index);

            delete[] lpBuf;
            return str;
        }
    }
    return QString();
}

QString OfficeFrameHelperWin::getWindowText() const
{
    QString str;
    int len = ::GetWindowTextLengthW(m_hwndFrame);
    ushort *text = new ushort[len + 1];
    ::GetWindowTextW(m_hwndFrame, (LPWSTR)text, len + 1);
    str = QString::fromUtf16(text);
    delete[] text;
    return str;
}

void OfficeFrameHelperWin::refreshFrameStyle()
{
    if (m_dwmEnabled)
        return;

    DWORD dwStyle = getStyle();
    DWORD dwStyleRemove = (WS_DLGFRAME | WS_VSCROLL | WS_HSCROLL);

    if (dwStyle & dwStyleRemove) {
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

void OfficeFrameHelperWin::updateFrameRegion(const QSize &szFrameRegion, bool bUpdate)
{
    if (m_inUpdateFrame)
        return;

    m_inUpdateFrame = true;
    m_isTransparentCaption = false;

    if (m_szFrameRegion != szFrameRegion || bUpdate) {
        RECT rc;
        rc.left = 0;
        rc.top = 0;
        rc.right = szFrameRegion.width();
        rc.bottom = szFrameRegion.height();
        AdjustWindowRectEx(&rc, getStyle(false), false, getStyle(true));

        m_frameBorder = -rc.left;
        m_borderSizeBotton = m_frameBorder;

        if (!m_dwmEnabled) {
            HRGN hRgn = 0;

            if (!isMDIMaximized()) {
                if (getStyle() & WS_MAXIMIZE) {
                    const int frameRegion = m_frameBorder - (getStyle(true) & WS_EX_CLIENTEDGE ? 2 : 0);
                    hRgn = CreateRectRgn(frameRegion, frameRegion, szFrameRegion.width() - frameRegion,
                                         szFrameRegion.height() - frameRegion);
                } else {
                    if (isTheme2013())
                        hRgn = ::CreateRectRgn(0, 0, szFrameRegion.width() + 1, szFrameRegion.height() + 1);
                    else
                        hRgn = calcFrameRegion(szFrameRegion);
                }
            }
            ::SetWindowRgn(m_hwndFrame, hRgn, true);
        } else {
            if (!(getStyle() & WS_MAXIMIZE))
                m_frameBorder = 4;

            const bool flatFrame = (bool)m_ribbonBar->style()->styleHint((QStyle::StyleHint)RibbonStyle::SH_FlatFrame);

            int cyTopHeight = isTitleVisible() ? 0 : titleBarSize() + (flatFrame ? 0 : 1);

            if (m_ribbonBar && !isTitleVisible() && flatFrame)
                cyTopHeight += m_ribbonBar->tabBarHeight() + 1;

            HLP_MARGINS margins;
            margins.cxLeftWidth = 0;
            margins.cyTopHeight = cyTopHeight;
            margins.cxRightWidth = 0;
            margins.cyBottomHeight = 0;
            HRESULT hres = pDwmExtendFrameIntoClientArea(m_hwndFrame, &margins);
            if (hres == S_OK)
                m_isTransparentCaption = true;
        }
        m_szFrameRegion = szFrameRegion;
    }
    m_inUpdateFrame = false;
}

void OfficeFrameHelperWin::updateShellAutohideBars()
{
    m_shellAutohideBars = 0;

    APPBARDATA abd;
    ZeroMemory(&abd, sizeof(APPBARDATA));
    abd.cbSize = sizeof(APPBARDATA);

    abd.uEdge = ABE_BOTTOM;
    if (SHAppBarMessage(ABM_GETAUTOHIDEBAR, &abd))
        m_shellAutohideBars |= HLP_AUTOHIDE_BOTTOM;

    abd.uEdge = ABE_TOP;
    if (SHAppBarMessage(ABM_GETAUTOHIDEBAR, &abd))
        m_shellAutohideBars |= HLP_AUTOHIDE_TOP;

    abd.uEdge = ABE_LEFT;
    if (SHAppBarMessage(ABM_GETAUTOHIDEBAR, &abd))
        m_shellAutohideBars |= HLP_AUTOHIDE_LEFT;

    abd.uEdge = ABE_RIGHT;
    if (SHAppBarMessage(ABM_GETAUTOHIDEBAR, &abd))
        m_shellAutohideBars |= HLP_AUTOHIDE_RIGHT;
}

int OfficeFrameHelperWin::shellAutohideBars()
{
    if (!m_shellAutohideBarsInitialized) {
        updateShellAutohideBars();
        m_shellAutohideBarsInitialized = true;
    }

    return m_shellAutohideBars;
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

bool OfficeFrameHelperWin::isTheme2013() const
{
    if (OfficeStyle *officeStyle = qobject_cast<OfficeStyle *>(qApp->style())) {
        OfficeStyle::Theme theme = officeStyle->getTheme();
        if (theme == OfficeStyle::Office2013White || theme == OfficeStyle::Office2013Gray ||
            theme == OfficeStyle::Office2013Dark)
            return true;
        return isTheme2016();
    }
    return false;
}

bool OfficeFrameHelperWin::isTheme2016() const { return false; }

void OfficeFrameHelperWin::initStyleOption(StyleOptionFrame *option)
{
    RECT rc;
    ::GetWindowRect(m_hwndFrame, &rc);
    RECT rcClient;
    ::GetClientRect(m_hwndFrame, &rcClient);
    ::ClientToScreen(m_hwndFrame, (LPPOINT)&rcClient);
    ::ClientToScreen(m_hwndFrame, ((LPPOINT)&rcClient) + 1);

    option->init(m_frame);
    option->rect = QRect(QPoint(rc.left, rc.top), QPoint(rc.right, rc.bottom));
    option->active = m_active;
    option->hasStatusBar = isFrameHasStatusBar();
    option->frameBorder = m_frameBorder;
    option->titleBarSize = titleBarSize();
    option->tabBarSize = m_ribbonBar->titleBarHeight() - m_frameBorder;
    isFrameHasStatusBar(&option->statusHeight);
    option->titleVisible = isTitleVisible();
    option->clientRect = QRect(QPoint(rcClient.left, rcClient.top), QPoint(rcClient.right, rcClient.bottom));
    option->maximizeFlags = getStyle() & WS_MAXIMIZE;
    option->isBackstageVisible =
        m_ribbonBar->isBackstageVisible() &&
        (bool)m_ribbonBar->style()->styleHint((QStyle::StyleHint)RibbonStyle::SH_RibbonBackstageHideTabs);
    option->tabBarPosition = m_ribbonBar->tabBarPosition();
}

static void drawPartFrame(QWidget *frame, HDC hdc, HDC cdc, const StyleOptionFrame &opt, const QRect &rect)
{
    QPixmap pixmap(rect.size());
    QPainter p(&pixmap);
    p.setWindow(rect);

    qApp->style()->drawPrimitive(QStyle::PE_Frame, &opt, &p, frame);

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
    if (!m_dwmEnabled) {
        if (isTheme2013()) {
            StyleOptionFrame opt;
            initStyleOption(&opt);
            opt.hdc = ::GetWindowDC(m_hwndFrame);
            qApp->style()->drawPrimitive(QStyle::PE_Frame, &opt, Q_NULL, m_frame);
            ::ReleaseDC(m_hwndFrame, (HDC)opt.hdc);
        } else {
            RECT rc;
            ::GetWindowRect(m_hwndFrame, &rc);

            int height = rc.bottom - rc.top;
            int width = rc.right - rc.left;

            HDC hdc = ::GetWindowDC(m_hwndFrame);
            HDC cdc = CreateCompatibleDC(hdc);

            StyleOptionFrame opt;
            initStyleOption(&opt);

            int fBorder = m_frameBorder != 0 ? m_frameBorder : 4;
            int borderSizeBotton = m_borderSizeBotton != 0 ? m_borderSizeBotton : 4;
            // draw top side
            ::drawPartFrame(m_frame, hdc, cdc, opt, QRect(0, 0, width, opt.titleVisible ? opt.titleBarSize : fBorder));
            // draw right side
            ::drawPartFrame(m_frame, hdc, cdc, opt, QRect(width - fBorder, 0, fBorder, height));
            // draw left side
            ::drawPartFrame(m_frame, hdc, cdc, opt, QRect(0, 0, fBorder, height));
            // draw bottom side
            ::drawPartFrame(m_frame, hdc, cdc, opt, QRect(0, height - borderSizeBotton, width, borderSizeBotton));
            ::DeleteDC(cdc);

            if (opt.titleVisible) {
                QRect rect(QPoint(rc.left, rc.top), QPoint(rc.right, rc.bottom));

                int nFrameBorder = frameBorder();
                int nTopBorder = isDwmEnabled() ? nFrameBorder : 0;
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
                if (HICON hIcon = qt_pixmapToWinHICON(getFrameSmallIcon()))
#else
                if (HICON hIcon = getFrameSmallIcon().toWinHICON())
#endif
                {
                    int nFrameBorder = frameBorder();

                    QSize szIcon = sizeSystemIcon(QIcon(), QRect());

                    int nTop = nFrameBorder;
                    int nLeft = nFrameBorder + 1;

                    QRect rect(QPoint(nLeft, nTop), szIcon);
                    ::DrawIconEx(hdc, rect.left(), rect.top(), hIcon, 0, 0, 0, Q_NULL, DI_NORMAL | DI_COMPAT);
                    ::DestroyIcon(hIcon);
                }

                ::SetBkMode(hdc, TRANSPARENT);

                HFONT hCaptionFont = getCaptionFont(Q_NULL);
                HFONT hOldFont = (HFONT)SelectObject(hdc, (HGDIOBJ)hCaptionFont);

                QRect rctext = rect;
                rctext.setHeight(opt.titleBarSize);

                QRgb clrFrameCaptionText = 0;
                if (!opt.active) {
                    QRibbon::StyleHintReturnThemeColor hintReturnInActive("Window", "CaptionTextInActive");
                    clrFrameCaptionText = static_cast<QRgb>(
                        m_frame->style()->styleHint(QStyle::SH_CustomBase, Q_NULL, Q_NULL, &hintReturnInActive));
                } else {
                    QRibbon::StyleHintReturnThemeColor hintReturnActive("Window", "CaptionTextActive");
                    clrFrameCaptionText = static_cast<QRgb>(
                        m_frame->style()->styleHint(QStyle::SH_CustomBase, Q_NULL, Q_NULL, &hintReturnActive));
                }

                COLORREF colorInActive =
                    RGB(qRed(clrFrameCaptionText), qGreen(clrFrameCaptionText), qBlue(clrFrameCaptionText));
                ::SetTextColor(hdc, colorInActive);

                int len = ::GetWindowTextLengthW(m_hwndFrame);
                wchar_t *text = new wchar_t[len + 1];
                ::GetWindowTextW(m_hwndFrame, text, len + 1);

                QSize szIcon = sizeSystemIcon(QIcon(), QRect());
                RECT rc = {nFrameBorder + szIcon.width() + 3, nTopBorder, opt.rect.width() - nFrameBorder /*-100*/,
                           opt.titleBarSize};

                UINT nFormat = DT_VCENTER | DT_LEFT | DT_END_ELLIPSIS | DT_SINGLELINE | DT_NOPREFIX;
                ::DrawTextW(hdc, text, len, &rc, nFormat);
                // #pragma warning(suppress: 6102)
                delete[] text;
                text = 0;
                SelectObject(hdc, (HGDIOBJ)hOldFont);
            }

            ::ReleaseDC(m_hwndFrame, hdc);
        }
    }
}

void OfficeFrameHelperWin::drawTitleBar(QPainter *painter, const StyleOptionTitleBar &opt)
{
    if (!m_ribbonBar)
        return;
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
    // QRasterPaintEngine* en = (QRasterPaintEngine *)painter->paintEngine();
    QPlatformNativeInterface *nativeInterface = QGuiApplication::platformNativeInterface();
    QBackingStore *backingStore = m_frame->backingStore();
    HDC hdc = static_cast<HDC>(nativeInterface->nativeResourceForBackingStore("getDC", backingStore));
#else
    QPaintEngine *en = painter->paintEngine();
    HDC hdc = en->getDC();
#endif   // (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))

    QRect rc = opt.rect;
    if ((bool)m_ribbonBar->style()->styleHint((QStyle::StyleHint)RibbonStyle::SH_FlatFrame))
        rc.adjust(0, 0, 0, 1);

    // fillSolidRect(painter, rc, opt.airRegion, QColor(0, 0, 0));

    int len = ::GetWindowTextLengthW(m_hwndFrame);
    wchar_t *text = new wchar_t[len + 1];
    ::GetWindowTextW(m_hwndFrame, text, len + 1);
    drawDwmCaptionText(painter, opt.rcTitleText, text[0] != 0 ? QString::fromWCharArray(text) : opt.text, opt.clrText,
                       opt.state & QStyle::State_Active, false);
    delete[] text;

    if (opt.drawIcon && !opt.icon.isNull()) {
        int nFrameBorder = frameBorder();
        int nTopBorder = isDwmEnabled() ? nFrameBorder : 0;

        QSize szIcon = sizeSystemIcon(opt.icon, opt.rect);

        int nTop = nTopBorder / 2 - 1 + (opt.rect.bottom() - szIcon.height()) * 0.5;
        int nLeft = opt.rect.left() + nFrameBorder + 1;

        QRect rect(QPoint(nLeft, nTop), szIcon);

        if (HICON hIcon = windowIcon(opt.icon, opt.rect)) {
            HANDLE hTheme = pOpenThemeData((HWND)QApplication::desktop()->winId(), L"WINDOW");
            if (m_isTransparentCaption && pDrawThemeIcon && hTheme) {
                RECT rcIcon = {rect.left(), rect.top(), rect.width() + rect.left(), rect.height() + rect.top()};
                HIMAGELIST hImageList =
                    ::ImageList_Create(szIcon.width(), szIcon.height(), ILC_COLOR32 | ILC_MASK, 0, 0);
                ::ImageList_AddIcon(hImageList, hIcon);
                pDrawThemeIcon(hTheme, hdc, 0, 0, &rcIcon, hImageList, 0);
                ::ImageList_Destroy(hImageList);
            } else
                ::DrawIconEx(hdc, rect.left(), rect.top(), hIcon, 0, 0, 0, Q_NULL, DI_NORMAL);
        }
    }

    if (hdc) {
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
        ::ReleaseDC(m_hwndFrame, hdc);
#else
        en->releaseDC(hdc);
#endif   // (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
    }
}

void OfficeFrameHelperWin::fillSolidRect(QPainter *painter, const QRect &rect, const QRegion &airRegion, QColor clr)
{
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
    Q_UNUSED(painter);
    QPlatformNativeInterface *nativeInterface = QGuiApplication::platformNativeInterface();
    QBackingStore *backingStore = m_frame->backingStore();
    HDC hdc = static_cast<HDC>(nativeInterface->nativeResourceForBackingStore("getDC", backingStore));
#else
    QPaintEngine *en = painter->paintEngine();
    HDC hdc = en->getDC();
#endif   // (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))

    if (hdc == 0)
        return;

#if QT_VERSION < QT_VERSION_CHECK(5, 11, 0)
    QVector<QRect> rects = airRegion.rects();
    for (QVector<QRect>::iterator it = rects.begin(); it != rects.end(); ++it) {
#else // since 5.8
    for (QRegion::const_iterator it = airRegion.begin(); it != airRegion.end(); ++it) {
#endif
        QRect r = rect.intersected(*it);
        RECT rc;
        rc.left = r.left();
        rc.top = r.top();
        rc.right = r.right() + 1;
        rc.bottom = r.bottom() + 1;
        ::SetBkColor(hdc, RGB(clr.red(), clr.green(), clr.blue()));
        ::ExtTextOutW(hdc, 0, 0, ETO_OPAQUE, &rc, Q_NULL, 0, Q_NULL);
    }
    if (hdc) {
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
        ::ReleaseDC(m_hwndFrame, hdc);
#else
        en->releaseDC(hdc);
#endif   // (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
    }
}

void OfficeFrameHelperWin::drawDwmCaptionText(QPainter *painter, const QRect &rc, const QString &strWindowText,
                                              const QColor &colText, bool active, bool contextTab)
{
    if (pDrawThemeTextEx == Q_NULL)
        return;

    if (strWindowText.isEmpty())
        return;

    QRect rect = rc;
    int glow = glowSize() * 2;

    const QFont font = QApplication::font("QMdiSubWindowTitleBar");
    const QFontMetrics fm(font);
    QString text = fm.elidedText(strWindowText, Qt::ElideRight, rect.width() - glow);

    if (!rect.isValid())
        return;

    HANDLE hTheme = pOpenThemeData((HWND)QApplication::desktop()->winId(), L"WINDOW");
    if (!hTheme)
        return;

    // Set up a memory DC and bitmap that we'll draw into
    HDC dcMem = Q_NULL;
    BITMAPINFO dib;
    ZeroMemory(&dib, sizeof(dib));
    dcMem = CreateCompatibleDC(/*hdc*/ 0);

    dib.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    dib.bmiHeader.biWidth = rect.width();
    dib.bmiHeader.biHeight = -rect.height();
    dib.bmiHeader.biPlanes = 1;
    dib.bmiHeader.biBitCount = 32;
    dib.bmiHeader.biCompression = BI_RGB;

    // Set up the DC
    HFONT hCaptionFont = getCaptionFont(contextTab || isTheme2013() ? hTheme : Q_NULL);
    // #pragma warning(suppress: 6387)
    HBITMAP bmp = CreateDIBSection(/*hdc*/ Q_NULL, &dib, DIB_RGB_COLORS, Q_NULL, Q_NULL, 0);
    HBITMAP hOldBmp = Q_NULL;
    if (bmp != Q_NULL)
        hOldBmp = (HBITMAP)SelectObject(dcMem, (HGDIOBJ)bmp);
    HFONT hOldFont = (HFONT)SelectObject(dcMem, (HGDIOBJ)hCaptionFont);

    // Draw the text!
    HLP_DTTOPTS dto;
    memset(&dto, 0, sizeof(HLP_DTTOPTS));
    dto.dwSize = sizeof(HLP_DTTOPTS);
    const UINT uFormat = DT_CENTER | DT_SINGLELINE | DT_VCENTER | DT_END_ELLIPSIS | DT_NOPREFIX;
    RECT rctext = {0, contextTab ? -rect.top() : -rect.top() /**2*/, rect.width(), rect.height()};

    dto.dwFlags = HLP_DTT_COMPOSITED;
    int nGlowSize = glowSize();
    if (nGlowSize > 0) {
        dto.dwFlags |= HLP_DTT_GLOWSIZE;
        dto.iGlowSize = nGlowSize;
    }

    if (colText.isValid()) {
        dto.iGlowSize = 0;
        dto.dwFlags |= HLP_DTT_TEXTCOLOR;
        dto.crText = RGB(colText.red(), colText.green(), colText.blue());
    }

    bool maximizeStyle = getStyle() & WS_MAXIMIZE;

    pDrawThemeTextEx(hTheme, dcMem, maximizeStyle ? HLP_WP_MAXCAPTION : HLP_WP_CAPTION,
                     active ? HLP_FS_ACTIVE : HLP_FS_INACTIVE, (LPCWSTR)text.utf16(), -1, uFormat, &rctext, &dto);

#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
    QPixmap pxx = ::qt_pixmapFromWinHBITMAP(bmp, HBitmapPremultipliedAlpha);
#else
    QPixmap pxx = QPixmap::fromWinHBITMAP(bmp, QPixmap::PremultipliedAlpha);
#endif
    painter->drawPixmap(rect.left(), rect.top() + (maximizeStyle ? m_frameBorder * 0.5 : m_frameBorder), pxx);

    if (hOldBmp != Q_NULL)
        SelectObject(dcMem, (HGDIOBJ)hOldBmp);
    SelectObject(dcMem, (HGDIOBJ)hOldFont);
    if (bmp != Q_NULL)
        DeleteObject(bmp);
    DeleteObject(hCaptionFont);
    DeleteDC(dcMem);
}

QSize OfficeFrameHelperWin::sizeSystemIcon(const QIcon &icon, const QRect &rect) const
{
    Q_UNUSED(icon)
    Q_UNUSED(rect)
    return isTheme2016() ? QSize() : QSize(GetSystemMetrics(SM_CXSMICON), GetSystemMetrics(SM_CYSMICON));
}

HICON OfficeFrameHelperWin::windowIcon(const QIcon &icon, const QRect &rect) const
{
    OfficeFrameHelperWin *that = (OfficeFrameHelperWin *)this;
    if (!m_hIcon && !icon.isNull()) {
        QSize szIcon = sizeSystemIcon(icon, rect);
        that->m_szIcon = szIcon;
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
    m_szIcon = QSize(0, 0);
}

void OfficeFrameHelperWin::recalcFrameLayout()
{
    m_postReclalcLayout = false;
    m_szFrameRegion = QSize();
    ::SetWindowPos(m_hwndFrame, 0, 0, 0, 0, 0,
                   SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED);

    if (QMainWindow *mainWindow = qobject_cast<QMainWindow *>(m_frame)) {
        if (QLayout *layout = mainWindow->layout())
            layout->invalidate();
    } else {
        RECT rcClient;
        ::GetClientRect(m_hwndFrame, &rcClient);
        ::SendMessage(m_hwndFrame, WM_SIZE, 0,
                      MAKELPARAM(rcClient.right - rcClient.left, rcClient.bottom - rcClient.top));
    }

    if (m_ribbonBar)
        m_ribbonBar->update();
}

void OfficeFrameHelperWin::postRecalcFrameLayout()
{
    m_postReclalcLayout = true;
    QApplication::postEvent(this, new QEvent(QEvent::LayoutRequest));
}

void OfficeFrameHelperWin::processClickedSubControl(QStyle::SubControl subControl)
{
    int idCommand = -1;
    switch (subControl) {
    case QStyle::SC_TitleBarCloseButton:
        idCommand = SC_CLOSE;
        break;
    case QStyle::SC_TitleBarNormalButton:
        idCommand = SC_RESTORE;
        break;
    case QStyle::SC_TitleBarMaxButton:
        idCommand = SC_MAXIMIZE;
        break;
    case QStyle::SC_TitleBarMinButton:
        idCommand = SC_MINIMIZE;
        break;
    default:
        break;
    }

    if (idCommand != -1)
        ::PostMessage(m_hwndFrame, WM_SYSCOMMAND, idCommand, 0);
}

static bool _qtcModifyStyle(HWND hWnd, int nStyleOffset, DWORD dwRemove, DWORD dwAdd, UINT nFlags)
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
    return _qtcModifyStyle(hWnd, GWL_STYLE, dwRemove, dwAdd, nFlags);
}

bool OfficeFrameHelperWin::isFrameHasStatusBar(int *statusHeight) const
{
    if (RibbonMainWindow *mainWindow = qobject_cast<RibbonMainWindow *>(m_frame)) {
        if (QWidget *statusBar = mainWindow->findChild<QStatusBar *>()) {
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

    RECT rgnTopFrame[] = {
        {4, 0, cx - 4, 1}, {2, 1, cx - 2, 2}, {1, 2, cx - 1, 3}, {1, 3, cx - 1, 4}, {0, 4, cx, cy - 4}};

    RECT rgnRibbonBottomFrame[] = {
        {1, cy - 4, cx - 1, cy - 2}, {2, cy - 2, cx - 2, cy - 1}, {4, cy - 1, cx - 4, cy - 0}};

    RECT rgnSimpleBottomFrame[] = {{0, cy - 4, cx, cy}};

    bool roundedCornersAlways = true;
    bool hasStatusBar = (roundedCornersAlways || isFrameHasStatusBar()) && m_frameBorder > 3;

    int sizeTopRect = sizeof(rgnTopFrame);
    int sizeBottomRect = hasStatusBar ? sizeof(rgnRibbonBottomFrame) : sizeof(rgnSimpleBottomFrame);

    int sizeData = sizeof(RGNDATAHEADER) + sizeTopRect + sizeBottomRect;

    RGNDATA *pRgnData = (RGNDATA *)malloc(sizeData);
    if (!pRgnData)
        return Q_NULL;

    memcpy(&pRgnData->Buffer, (void *)&rgnTopFrame, sizeTopRect);
    memcpy(&pRgnData->Buffer + sizeTopRect,
           hasStatusBar ? (void *)&rgnRibbonBottomFrame : (void *)&rgnSimpleBottomFrame, sizeBottomRect);

    pRgnData->rdh.dwSize = sizeof(RGNDATAHEADER);
    pRgnData->rdh.iType = RDH_RECTANGLES;
    pRgnData->rdh.nCount = (sizeTopRect + sizeBottomRect) / sizeof(RECT);
    pRgnData->rdh.nRgnSize = 0;
    pRgnData->rdh.rcBound.left = 0;
    pRgnData->rdh.rcBound.top = 0;
    pRgnData->rdh.rcBound.right = cx;
    pRgnData->rdh.rcBound.bottom = cy;

    HRGN rgh = ::ExtCreateRegion(Q_NULL, sizeData, pRgnData);

    free(pRgnData);
    return rgh;
}

static bool qntGetNonClientMetrics(NONCLIENTMETRICS &ncm)
{
    const UINT cbProperSize = sizeof(NONCLIENTMETRICS);
    ZeroMemory(&ncm, sizeof(NONCLIENTMETRICS));
    ncm.cbSize = sizeof(NONCLIENTMETRICS);
    return ::SystemParametersInfo(SPI_GETNONCLIENTMETRICS, cbProperSize, &ncm, 0);
}

HFONT OfficeFrameHelperWin::getCaptionFont(HANDLE hTheme)
{
    LOGFONT lf = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, {0}};

    if (!hTheme)
        pGetThemeSysFont(hTheme, HLP_TMT_CAPTIONFONT, &lf);
    else {
        NONCLIENTMETRICS ncm;
        qntGetNonClientMetrics(ncm);
        lf = ncm.lfMessageFont;
        /*
        NONCLIENTMETRICS ncm;
        ncm.cbSize = sizeof(NONCLIENTMETRICS);
        SystemParametersInfoW(SPI_GETNONCLIENTMETRICS, sizeof(NONCLIENTMETRICS), &ncm, false);
        lf = ncm.lfMessageFont;
        */
    }
    return ::CreateFontIndirectW(&lf);
}

#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
HDC OfficeFrameHelperWin::backingStoreDC(const QWidget *farame, QPoint *offset)
{
    HDC hdc = static_cast<HDC>(QGuiApplication::platformNativeInterface()->nativeResourceForBackingStore(
        QByteArrayLiteral("getDC"), farame->backingStore()));
    *offset = QPoint(0, 0);
    if (!farame->windowHandle())
        if (QWidget *nativeParent = farame->nativeParentWidget())
            *offset = farame->mapTo(nativeParent, *offset);
    return hdc;
}
#endif

bool OfficeFrameHelperWin::hitTestContextHeaders(const QPoint &point) const
{
    if (!m_listContextHeaders)
        return false;

    for (int i = 0, count = m_listContextHeaders->count(); i < count; i++) {
        ContextHeader *header = m_listContextHeaders->at(i);
        if (header->rcRect.contains(point))
            return true;
    }
    return false;
}

bool OfficeFrameHelperWin::hitTestBackstageHeaders(const QPoint &point) const
{
    Q_ASSERT(m_ribbonBar != Q_NULL);
    if (RibbonSystemButton *systemButton = m_ribbonBar->getSystemButton()) {
        if (RibbonBackstageView *backstageView = systemButton->backstage()) {
            if (backstageView->isVisible()) {
                QRect rect = backstageView->menuGeometry();
                if (rect.contains(point))
                    return true;
            }
        }
    }
    return false;
}

#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
void OfficeFrameHelperWin::collapseTopFrame()
{
    if (!m_frame->isVisible())
        return;

    const bool theme2013 = isTheme2013();

    const DWORD dStyle = getStyle();

    if ((dStyle & WS_MINIMIZE) && theme2013)
        return;

    if (QWindow *window = m_frame->windowHandle()) {
        if (QPlatformWindow *platformWindow = window->handle()) {
            int val = 0;
            int valBottom = 0;
            bool visible = theme2013 ? true : m_ribbonBar->qtc_d()->m_ribbonBarVisible;

            int valTitle = m_dwmEnabled ? (visible ? captionSize() + (m_frameBorder * 2) : 0)
                                        : (visible ? captionSize() + m_frameBorder / 2 : m_frameBorder / 2);

            if ((dStyle & WS_MAXIMIZE) == 0) {
                if (m_wasFullScreen)
                    valTitle = 0;
            }

            if (theme2013 && (dStyle & WS_MINIMIZE) == 0) {
                if (dStyle & WS_MAXIMIZE) {
                    if ((dStyle & WS_MAXIMIZE) == 0) {
                        valTitle -= m_frameBorder;
                        val = m_frameBorder - 3;
                        valBottom = m_frameBorder - 2;
                    } else {
                        valTitle -= m_frameBorder / 2;
                        valBottom = 0;
                    }
                }
            }

            // Reduce top frame to zero since we paint it ourselves.
            const QMargins customMargins = QMargins(-val, -valTitle, -val, -valBottom);
            const QVariant customMarginsV = QVariant::fromValue(customMargins);

            QMargins oldcustomMargins =
                qvariant_cast<QMargins>(QGuiApplication::platformNativeInterface()->windowProperty(
                    platformWindow, QStringLiteral("WindowsCustomMargins")));

            if (customMargins != oldcustomMargins) {
                // The dynamic property takes effect when creating the platform window.
                window->setProperty("_q_windowsCustomMargins", customMarginsV);
                // If a platform window exists, change via native interface.
                QGuiApplication::platformNativeInterface()->setWindowProperty(
                    platformWindow, QStringLiteral("WindowsCustomMargins"), customMarginsV);
            }
        }
    }
}
#endif   // QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)

#define qtc_GET_X_LPARAM(lp) ((int)(short)LOWORD(lp))
#define qtc_GET_Y_LPARAM(lp) ((int)(short)HIWORD(lp))
bool OfficeFrameHelperWin::winEvent(MSG *msg, long *result)
{
    if (m_hwndFrame && msg->hwnd != m_hwndFrame &&
        (m_ribbonBar && m_ribbonBar->testAttribute(Qt::WA_NativeWindow) && msg->hwnd == (HWND)m_ribbonBar->winId())) {
        if (msg->message == WM_NCHITTEST) {
            POINT point;
            point.x = (short)qtc_GET_X_LPARAM((DWORD)msg->lParam);
            point.y = (short)qtc_GET_Y_LPARAM((DWORD)msg->lParam);

            RECT rc;
            ::GetWindowRect(m_hwndFrame, &rc);
            rc.bottom = rc.top + titleBarSize();
            if (::PtInRect(&rc, point)) {
                LRESULT lResult = 0;
                lResult = DefWindowProcW(m_hwndFrame, msg->message, msg->wParam, msg->lParam);
                if (lResult == HTMINBUTTON || lResult == HTMAXBUTTON || lResult == HTCLOSE || lResult == HTHELP ||
                    lResult == HTCLIENT) {
                    *result = HTTRANSPARENT;
                    return true;
                } else if (lResult == HTSYSMENU) {
                    *result = HTSYSMENU;
                    return true;
                }
            }
        }
        return false;
    }

    if (!m_officeFrameEnabled)
        return false;

    if (!m_dwmEnabled && m_lockNCPaint &&
        (msg->message == WM_STYLECHANGING || msg->message == WM_STYLECHANGED || msg->message == WM_WINDOWPOSCHANGED ||
         msg->message == WM_WINDOWPOSCHANGING || msg->message == WM_NCPAINT)) {
        if (msg->message == WM_WINDOWPOSCHANGING)
            ((WINDOWPOS *)msg->lParam)->flags &= ~SWP_FRAMECHANGED;

        return true;
    } else if (msg->message == WM_SETTINGCHANGE) {
        UINT uFlags = msg->wParam;
        if (uFlags == SPI_SETNONCLIENTMETRICS || uFlags == SPI_SETWORKAREA || uFlags == SPI_SETICONTITLELOGFONT) {
            if (shellAutohideBars() != 0) {
                ::ShowWindow(m_hwndFrame, SW_RESTORE);
                ::ShowWindow(m_hwndFrame, SW_MAXIMIZE);
            }
        }

        m_shellAutohideBarsInitialized = false;
        return false;
    } else if (msg->message == WM_WINDOWPOSCHANGING) {
        WINDOWPOS *lpwndpos = (WINDOWPOS *)msg->lParam;

        QSize szFrameRegion(lpwndpos->cx, lpwndpos->cy);

        if (((lpwndpos->flags & SWP_NOSIZE) == 0) && (m_szFrameRegion != szFrameRegion) &&
            ((getStyle() & WS_CHILD) == 0)) {
            m_skipNCPaint = true;
        }
        return false;
    } else if (msg->message == WM_WINDOWPOSCHANGED) {
        WINDOWPOS *lpwndpos = (WINDOWPOS *)msg->lParam;

        if (lpwndpos->flags & SWP_FRAMECHANGED && !m_inUpdateFrame)
            updateFrameRegion();
        return false;
    } else if (m_hwndFrame != Q_NULL && (msg->message == WM_SIZE || msg->message == WM_STYLECHANGED)) {
        RECT rc;
        ::GetWindowRect(m_hwndFrame, &rc);

        QSize szFrameRegion(rc.right - rc.left, rc.bottom - rc.top);

        if ((m_szFrameRegion != szFrameRegion) || (msg->message == WM_STYLECHANGED)) {
            updateFrameRegion(szFrameRegion, (msg->message == WM_STYLECHANGED));
            redrawFrame();

            if (!m_dwmEnabled && (msg->message == WM_STYLECHANGED)) {
                ::SetWindowPos(m_hwndFrame, 0, 0, 0, 0, 0,
                               SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED);
                ::RedrawWindow(m_hwndFrame, &rc, 0, RDW_INVALIDATE | RDW_UPDATENOW | RDW_FRAME);
            }
        }

        if (msg->message == WM_SIZE && msg->wParam == SIZE_RESTORED && m_inLayoutRequest) {
            QResizeEvent e(m_ribbonBar->size(), m_ribbonBar->size());
            QApplication::sendEvent(m_ribbonBar, &e);
            m_inLayoutRequest = false;
        }
        if (msg->message == WM_SIZE && msg->wParam == SIZE_MINIMIZED) {
            if (getStyle(FALSE) & WS_MAXIMIZE)
                modifyStyle(m_hwndFrame, WS_MAXIMIZE, 0, 0);
        }

        return false;
    } else if (msg->message == WM_ENTERSIZEMOVE) {
        m_inLayoutRequest = true;
    } else if (m_hwndFrame != Q_NULL && msg->message == WM_NCRBUTTONUP) {
        // call the system menu
        ::SendMessageW(m_hwndFrame, 0x0313, (WPARAM)m_hwndFrame, msg->lParam);
        return true;
    } else if (msg->message == WM_SETTEXT) {
        *result = DefWindowProcW(msg->hwnd, msg->message, msg->wParam, msg->lParam);

        QEvent e(QEvent::WindowTitleChange);
        QApplication::sendEvent(m_ribbonBar, &e);
        return true;
    } else if (m_hwndFrame != Q_NULL && msg->message == WM_NCHITTEST && !m_dwmEnabled) {
        if (!isTitleVisible()) {
            *result = DefWindowProcW(msg->hwnd, msg->message, msg->wParam, msg->lParam);

            POINT point;
            point.x = (short)qtc_GET_X_LPARAM((DWORD)msg->lParam);
            point.y = (short)qtc_GET_Y_LPARAM((DWORD)msg->lParam);

            if (*result == HTCLIENT) {
                RECT rc;
                ::GetWindowRect(m_hwndFrame, &rc);

                rc.bottom = rc.top + m_frameBorder - (isTheme2013() ? 3 : 0);
                if (::PtInRect(&rc, point))
                    *result = HTTOP;

                if (*result == HTCLIENT) {
                    QPoint pos(m_ribbonBar->mapFromGlobal(QPoint(point.x, point.y)));
                    if (m_ribbonBar && m_rcHeader.isValid()) {
                        if (!hitTestContextHeaders(pos) && !hitTestBackstageHeaders(pos) && m_rcHeader.contains(pos))
                            *result = HTCAPTION;
                    } else {
                        rc.bottom = rc.top + titleBarSize();
                        if (::PtInRect(&rc, point))
                            *result = HTCAPTION;
                    }

                    bool isBackstageVisible = m_ribbonBar->isBackstageVisible() &&
                                              (bool)m_ribbonBar->style()->styleHint(
                                                  (QStyle::StyleHint)RibbonStyle::SH_RibbonBackstageHideTabs);
                    if (pos.y() < m_rcHeader.height() && m_ribbonBar->getSystemButton() && !isBackstageVisible &&
                        m_ribbonBar->getSystemButton()->toolButtonStyle() != Qt::ToolButtonFollowStyle) {
                        DWORD dwStyle = getStyle();
                        if (pos.x() < 7 + sizeSystemIcon(QIcon(), QRect()).width() && (dwStyle & WS_SYSMENU))
                            *result = HTSYSMENU;
                    }
                }
            }
        } else {
            *result = DefWindowProcW(msg->hwnd, msg->message, msg->wParam, msg->lParam);
            if (*result == HTCLOSE || *result == HTMAXBUTTON || *result == HTMINBUTTON || *result == HTHELP)
                *result = HTCAPTION;
        }

        return true;
    } else if (m_hwndFrame != Q_NULL && msg->message == WM_NCHITTEST && !isTitleVisible() && m_dwmEnabled) {
        POINT point, clientPoint;
        point.x = (short)qtc_GET_X_LPARAM((DWORD)msg->lParam);
        point.y = (short)qtc_GET_Y_LPARAM((DWORD)msg->lParam);
        clientPoint.x = point.x;
        clientPoint.y = point.y;
        ::ScreenToClient(m_hwndFrame, &clientPoint);

        LRESULT lResult;
        // Perform hit testing using DWM
        if (pDwmDefWindowProc(msg->hwnd, msg->message, msg->wParam, msg->lParam, &lResult)) {
            // DWM returned a hit, no further processing necessary
            *result = lResult;
        } else {
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

        if (*result == HTCLIENT || *result == HTSYSMENU) {
            RECT rc;
            ::GetWindowRect(m_hwndFrame, &rc);
            rc.left = rc.left + m_frameBorder * 2;
            rc.bottom = rc.top + captionSize() + m_frameBorder * 2;
            rc.right = rc.right - m_frameBorder * 2;

            if (::PtInRect(&rc, point)) {
                if (QWidget *quickAccessBar = (QWidget *)m_ribbonBar->quickAccessBar()) {
                    QRect rectAccessBar = quickAccessBar->rect();
                    QPoint pos(quickAccessBar->mapFromGlobal(QPoint(point.x, point.y)));
                    if (rectAccessBar.contains(pos))
                        return true;
                }

                if (QWidget *systemButton = (QWidget *)m_ribbonBar->getSystemButton()) {
                    QRect rectSystemButton = systemButton->rect();
                    QPoint pos(systemButton->mapFromGlobal(QPoint(point.x, point.y)));
                    if (rectSystemButton.contains(pos)) {
                        *result = HTCLIENT;
                        return true;
                    }
                }

                QPoint pos(m_ribbonBar->mapFromGlobal(QPoint(point.x, point.y)));
                if (hitTestContextHeaders(pos))
                    return true;

                DWORD dwStyle = getStyle();
                if ((point.y < rc.top + m_frameBorder) && (dwStyle & WS_SIZEBOX) && ((dwStyle & WS_MAXIMIZE) == 0)) {
                    *result = HTTOP;
                    return true;
                }
                if (*result != HTSYSMENU) {
                    *result = HTCAPTION;
                    DWORD dwStyle = getStyle();
                    if (pos.x() < m_frameBorder + sizeSystemIcon(QIcon(), QRect()).width() && (dwStyle & WS_SYSMENU))
                        *result = HTSYSMENU;
                }
            }
        }
        return true;
    } else if (m_hwndFrame != Q_NULL && msg->message == WM_NCACTIVATE && !m_dwmEnabled) {
        bool active = (bool)msg->wParam;

        if (!::IsWindowEnabled(m_hwndFrame))
            active = true;

        DWORD dwStyle = getStyle();

        if (dwStyle & WS_VISIBLE) {
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

            if (m_active != active) {
                m_active = active;
                redrawFrame();
            }
        } else {
            m_active = active;
            return false;
        }
        return true;
    } else if (m_hwndFrame != Q_NULL && msg->message == HLP_WM_DWMCOMPOSITIONCHANGED) {
        bool dwmEnabled = !m_allowDwm || getStyle() & WS_CHILD ? FALSE : isCompositionEnabled();

        if (dwmEnabled != m_dwmEnabled) {
            m_dwmEnabled = dwmEnabled;

            ::SetWindowRgn(m_hwndFrame, 0, true);

            if (!m_dwmEnabled)
                refreshFrameStyle();

            ::SetWindowPos(m_hwndFrame, 0, 0, 0, 0, 0,
                           SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED);
        }
        return false;
    } else if (msg->message == WM_NCCALCSIZE && !m_lockNCPaint) {
        NCCALCSIZE_PARAMS FAR *lpncsp = (NCCALCSIZE_PARAMS FAR *)msg->lParam;

        RECT rc;
        rc.left = lpncsp->rgrc[0].left;
        rc.top = lpncsp->rgrc[0].top;
        rc.right = lpncsp->rgrc[0].right;
        rc.bottom = lpncsp->rgrc[0].bottom;

        m_wasFullScreen = m_frame->windowState() & Qt::WindowFullScreen;

#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
        collapseTopFrame();
#endif   // QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)

        if (m_dwmEnabled)
            *result = DefWindowProcW(msg->hwnd, msg->message, msg->wParam, msg->lParam);
        else {
            lpncsp->rgrc[0].left += m_frameBorder;
            lpncsp->rgrc[0].top += m_frameBorder;
            lpncsp->rgrc[0].right -= m_frameBorder;
            lpncsp->rgrc[0].bottom -= m_frameBorder;
        }

        DWORD dwStyle = getStyle();
        if (m_dwmEnabled) {
            if (!isTitleVisible())
                lpncsp->rgrc[0].top = rc.top;
        } else {
            if (isTheme2013()) {
                if ((dwStyle & WS_MAXIMIZE) == 0) {
                    lpncsp->rgrc[0].left -= m_frameBorder - 3;
                    lpncsp->rgrc[0].right += m_frameBorder - 3;
                    lpncsp->rgrc[0].top -= m_frameBorder - 3;
                    lpncsp->rgrc[0].bottom += m_frameBorder - 3;
                }
            } else {
                lpncsp->rgrc[0].top += (isTitleVisible() ? titleBarSize() - m_frameBorder : 0);
                if (isFrameHasStatusBar() && ((dwStyle & WS_MAXIMIZE) == 0)) {
                    int borderSize = qMax(rc.bottom - lpncsp->rgrc[0].bottom - 3, 1L);
                    m_borderSizeBotton = borderSize;
                    lpncsp->rgrc[0].bottom = rc.bottom - borderSize;
                }
            }
        }

        if (((dwStyle & (WS_MAXIMIZE | WS_CHILD)) == WS_MAXIMIZE) && shellAutohideBars() != 0)
            lpncsp->rgrc[0].bottom -= 1;

        return true;
    } else if (msg->message == WM_SYSCOMMAND && !m_dwmEnabled && msg->wParam == SC_MINIMIZE && getStyle() & WS_CHILD) {
        *result = DefWindowProcW(msg->hwnd, msg->message, msg->wParam, msg->lParam);

        redrawFrame();
        return true;
    } else if (msg->message == WM_NCPAINT && !m_dwmEnabled) {
        if (m_skipNCPaint) {
            m_skipNCPaint = false;
            return true;
        }
        if (!isMDIMaximized())
            redrawFrame();

        return true;
    }
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
    else if (m_hwndFrame != Q_NULL && msg->message == WM_SHOWWINDOW) {
        m_skipNCPaint = false;
        static bool bShow = false;
        if (!bShow) {
            bShow = true;

            bool is2013 = isTheme2013();
            int val = is2013 ? m_frameBorder - 3 : 0;
            bool visible = is2013 ? true : m_ribbonBar->qtc_d()->m_ribbonBarVisible;
            int valTitle = m_dwmEnabled ? (visible ? captionSize() + (m_frameBorder * 2) : 0)
                                        : (visible ? captionSize() + m_frameBorder * 0.5 : m_frameBorder * 0.5);

            RECT rc;
            ::GetWindowRect(m_hwndFrame, &rc);
            ::SetWindowPos(m_hwndFrame, Q_NULL, rc.left - val, rc.top - valTitle, rc.right - rc.left,
                           rc.bottom - rc.top, SWP_NOZORDER | SWP_NOACTIVATE);
        }
    }
#else    // QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
    else if (msg->message == WM_SHOWWINDOW) {
        static bool bShow = false;
        if (!bShow) {
            bShow = true;

            bool is2013 = isTheme2013();
            int val = m_dwmEnabled ? 0 : m_frameBorder;
            int valLeft = m_dwmEnabled ? (is2013 ? 0 : m_frameBorder + m_frameBorder / 2) : m_frameBorder;
            bool visible = is2013 ? true : m_ribbonBar->qtc_d()->m_ribbonBarVisible;
            int valTitle = m_dwmEnabled ? (visible ? captionSize() + (m_frameBorder * 2) : 0)
                                        : (visible ? captionSize() + m_frameBorder / 2 - 1 : m_frameBorder / 2 - 1);

            RECT rc;
            ::GetWindowRect(m_hwndFrame, &rc);
            ::SetWindowPos(m_hwndFrame, Q_NULL, rc.left - valLeft, rc.top - val, rc.right - rc.left,
                           rc.bottom - rc.top - valTitle, SWP_NOZORDER | SWP_NOACTIVATE);
        }
    }
#endif   // QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
    if (msg->message == OfficeFrameHelperWin::m_msgGetFrameHook) {
        HLP_FRAMEHOOK FAR *framehook = (HLP_FRAMEHOOK FAR *)msg->lParam;
        framehook->frameHelper = this;
        return true;
    }
    return false;
}

void OfficeFrameHelperWin::refreshFrameTimer()
{
    refreshFrameStyle();
    m_refreshFrame.stop();
}

bool OfficeFrameHelperWin::event(QEvent *event)
{
    if (m_postReclalcLayout && event->type() == QEvent::LayoutRequest)
        recalcFrameLayout();
    return QObject::event(event);
}

bool OfficeFrameHelperWin::eventFilter(QObject *obj, QEvent *event)
{
    if (obj == m_frame) {
#if (QT_VERSION < QT_VERSION_CHECK(5, 0, 0))
        if (event->type() == QEvent::Resize) {
            QEvent e(QEvent::EmbeddingControl);
            QCoreApplication::sendEvent(m_frame, &e);
        } else
#endif   // QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
            if (event->type() == QEvent::Paint) {
                QPainter p(m_frame);
                p.fillRect(0, 0, m_frame->width(), m_frame->height(),
                           m_frame->palette().brush(m_frame->backgroundRole()));
            } else if (event->type() == QEvent::LayoutRequest) {
                if (!m_dwmEnabled && !isMDIMaximized()) {
                    redrawFrame();
                    m_skipNCPaint = true;
                }
            } else if (event->type() == QEvent::Close) {
                m_closeWindow = true;
            } else if (event->type() == QEvent::StyleChange) {
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
                bool m_hasIsTheme2013 = m_isTheme2013;
#endif   // QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
                m_isTheme2013 = isTheme2013();

#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
                bool hasMaximize = m_hasIsTheme2013 != m_isTheme2013 && ((getStyle() & WS_MAXIMIZE) != 0);
                if (hasMaximize) {
                    m_frame->setWindowState(m_frame->windowState() &
                                            ~(Qt::WindowMinimized | Qt::WindowMaximized | Qt::WindowFullScreen));
                }
#endif   // QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)

                resetWindowIcon();

                enableWindowAero(m_hwndFrame, isCompositionEnabled());
                // enableWindowShadow(m_hwndFrame, isSmallSystemBorders() && (getStyle() & WS_CHILD) == 0);

                m_dwmEnabled = !m_allowDwm || getStyle() & WS_CHILD ? false : isCompositionEnabled();

                if (!m_dwmEnabled)
                    refreshFrameStyle();
                else
                    SetWindowRgn(m_hwndFrame, (HRGN)Q_NULL, true);

                if (pDwmExtendFrameIntoClientArea) {
                    HLP_MARGINS margins;
                    margins.cxLeftWidth = 0;
                    margins.cyTopHeight = 0;
                    margins.cxRightWidth = 0;
                    margins.cyBottomHeight = 0;
                    pDwmExtendFrameIntoClientArea(m_hwndFrame, &margins);
                }
                updateFrameRegion();

#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
                collapseTopFrame();
#endif   // QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)

                QApplication::postEvent(m_ribbonBar, new QEvent(QEvent::LayoutRequest));
                m_skipNCPaint = false;

                redrawFrame();

#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
                if (hasMaximize) {
                    m_frame->setWindowState((m_frame->windowState() & ~(Qt::WindowMinimized | Qt::WindowFullScreen)) |
                                            Qt::WindowMaximized);
                }
#endif   // QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
            } else if (event->type() == QEvent::WindowStateChange) {
                QApplication::postEvent(m_ribbonBar, new QEvent(QEvent::LayoutRequest));
                m_skipNCPaint = false;
                // ::SetWindowPos(m_hwndFrame, 0, 0, 0, 0, 0,
                //                SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED);
            } else if (event->type() == QEvent::Show) {
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
                collapseTopFrame();
#endif   // QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)

#if (QT_VERSION < QT_VERSION_CHECK(5, 0, 0))
                if (!(m_frame->windowState() & Qt::WindowMaximized) &&
                    !(m_frame->windowState() & Qt::WindowMinimized)) {
                    ::SetWindowPos(m_hwndFrame, 0, 0, 0, 0, 0,
                                   SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED);
                }
#endif   // QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
            }
    }
    return QObject::eventFilter(obj, event);
}

bool OfficeFrameHelperWin::isCompositionEnabled()
{
    if (isTheme2013() && m_officeFrameEnabled)
        return false;

    if (pDwmIsCompositionEnabled) {
        BOOL enabled;
        HRESULT hr = pDwmIsCompositionEnabled(&enabled);
        return (SUCCEEDED(hr) && enabled);
    }
    return false;
}

bool OfficeFrameHelperWin::isSmallSystemBorders() { return isTheme2013(); }

void OfficeFrameHelperWin::enableWindowAero(HWND hwnd, bool enable)
{
    DWORD value = enable ? 2 /* DWMNCRP_ENABLED */ : 1 /* DWMNCRP_DISABLED */;
    setWindowAttribute(hwnd, 2 /* DWMWA_NCRENDERING_POLICY */, &value, sizeof(value));
}

void OfficeFrameHelperWin::enableWindowShadow(HWND hwnd, bool enable)
{
    if (hwnd == Q_NULL)
        return;

    OSVERSIONINFO osvi;
    ZeroMemory(&osvi, sizeof(OSVERSIONINFO));
    Q_ASSERT(false);
    /*
    #if _MSC_VER >= 1200 // check the version of the system
        osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
        ::GetVersionEx(&osvi);
    */
    /*
        OSVERSIONINFOEX osvi;
        ::ZeroMemory(&osvi, sizeof(OSVERSIONINFOEX));
        osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);
        ULONGLONG maskCondition = ::VerSetConditionMask(0, VER_MAJORVERSION, VER_EQUAL);
        ::VerifyVersionInfoW(&osvi, VER_MAJORVERSION, maskCondition);
    */
    bool isWindowsVista = (osvi.dwMajorVersion >= 6);

    if (!isWindowsVista)
        return;

    DWORD dwClassStyle = ::GetClassLong(hwnd, GCL_STYLE);
    bool hasShadow = (dwClassStyle & CS_DROPSHADOW) != 0;
    bool wasChanged = FALSE;

    if (enable) {
        if (!hasShadow) {
            ::SetClassLong(hwnd, GCL_STYLE, dwClassStyle | CS_DROPSHADOW);
            wasChanged = TRUE;
        }
    } else {
        if (hasShadow) {
            ::SetClassLong(hwnd, GCL_STYLE, dwClassStyle & (~CS_DROPSHADOW));
            wasChanged = TRUE;
        }
    }

    if (wasChanged && ::IsWindowVisible(hwnd)) {
        bool isZoomed = ::IsZoomed(hwnd);
        bool isIconic = ::IsIconic(hwnd);

        ::ShowWindow(hwnd, SW_HIDE);
        ::ShowWindow(hwnd, isZoomed ? SW_SHOWMAXIMIZED : isIconic ? SW_SHOWMINIMIZED : SW_SHOW);
        ::SetWindowPos(hwnd, NULL, 0, 0, 0, 0,
                       SWP_FRAMECHANGED | SWP_NOZORDER | SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);

        ::BringWindowToTop(hwnd);
    }
}

bool OfficeFrameHelperWin::setWindowAttribute(HWND hwnd, DWORD dwAttribute, LPCVOID pvAttribute, DWORD cbAttribute)
{
    if (pDwmSetWindowAttribute == Q_NULL)
        return false;

    HRESULT hres = (*pDwmSetWindowAttribute)(hwnd, dwAttribute, pvAttribute, cbAttribute);
    return hres == S_OK;
}

bool OfficeFrameHelperWin::resolveSymbols()
{
    static bool tried = false;
    if (!tried) {
        tried = true;

        QLibrary dwmLib(QString::fromLatin1("dwmapi"));
        pDwmIsCompositionEnabled = (PtrDwmIsCompositionEnabled)dwmLib.resolve("DwmIsCompositionEnabled");
        pDwmSetWindowAttribute = (PtrDwmSetWindowAttribute)dwmLib.resolve("DwmSetWindowAttribute");
        if (pDwmIsCompositionEnabled && pDwmSetWindowAttribute) {
            pDwmDefWindowProc = (PtrDwmDefWindowProc)dwmLib.resolve("DwmDefWindowProc");
            pDwmExtendFrameIntoClientArea =
                (PtrDwmExtendFrameIntoClientArea)dwmLib.resolve("DwmExtendFrameIntoClientArea");
        }
        QLibrary themeLib(QString::fromLatin1("uxtheme"));
        pIsAppThemed = (PtrIsAppThemed)themeLib.resolve("IsAppThemed");

        if (pIsAppThemed) {
            pOpenThemeData = (PtrOpenThemeData)themeLib.resolve("OpenThemeData");
            pGetThemeSysFont = (PtrGetThemeSysFont)themeLib.resolve("GetThemeSysFont");
            pDrawThemeTextEx = (PtrDrawThemeTextEx)themeLib.resolve("DrawThemeTextEx");
            pDrawThemeIcon = (PtrDrawThemeIcon)themeLib.resolve("DrawThemeIcon");
        }
    }
    return (pIsAppThemed != 0 && pOpenThemeData != 0 && pGetThemeSysFont != 0 && pDrawThemeTextEx != 0 &&
            pDrawThemeIcon != 0);
}
