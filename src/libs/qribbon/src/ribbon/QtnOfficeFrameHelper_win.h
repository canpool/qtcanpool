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
#ifndef QTN_OFFICEFRAMEHELPER_WIN_H
#define QTN_OFFICEFRAMEHELPER_WIN_H

#include <QObject>
#include <QSize>
#include <QRect>
#include <QTimer>
#include <qt_windows.h>

#include "QtnOfficeFrameHelper.h"
#include "QtitanDef.h"

QTITAN_BEGIN_NAMESPACE

class StyleOptionFrame;
class RibbonBar;
/* OfficeFrameHelperWin */
class OfficeFrameHelperWin : public QObject,
                             public OfficeFrameHelper
{
    Q_OBJECT
public:
    OfficeFrameHelperWin(QWidget* parent = Q_NULL);
    virtual ~OfficeFrameHelperWin();

    static UINT m_msgGetFrameHook;
public:
    virtual void enableOfficeFrame(QWidget* parent);
    void disableOfficeFrame();

    virtual bool isDwmEnabled() const;
    virtual bool isActive() const;
    virtual bool isMaximize() const;

    virtual int getFrameBorder() const;
    virtual int titleBarSize() const;
    virtual int tabBarHeight() const;

    virtual void setHeaderRect(const QRect& rcHeader);
    virtual void setContextHeader(QList<ContextHeader*>* listContextHeaders);

    virtual QPixmap getFrameSmallIcon() const;
    virtual QString getSystemMenuString(uint item) const;

    virtual void drawTitleBar(QPainter* painter, const StyleOptionTitleBar& opt);
    virtual void fillSolidRect(QPainter* painter, const QRect& rect, const QRegion& airRegion, QColor clr);
    virtual void drawDwmCaptionText(QPainter* painter, const QRect& rect, const QString& strWindowText, bool active);
    virtual QSize getSizeSystemIcon() const;
    virtual HICON getWindowIcon(const QIcon& icon) const;
    virtual void resetWindowIcon();
    virtual void recalcFrameLayout();
    virtual void postRecalcFrameLayout();
    virtual void processClickedSubControl(QStyle::SubControl subControl);

public:
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
    void collapseTopFrame(bool includesFrame = false);
#endif // QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
    virtual bool winEvent(MSG* message, long* result);

private:
    static int glowSize() { return 10; }
    static int frameSize() { return GetSystemMetrics(SM_CYSIZEFRAME); }
    static int captionSize() { return GetSystemMetrics(SM_CYCAPTION); }

protected:
    DWORD getStyle(bool exStyle = false) const;
    void refreshFrameStyle();
    void updateFrameRegion();
    void updateFrameRegion(const QSize& szFrameRegion, bool bUpdate);
    bool isMDIMaximized() const;
    bool isTitleVisible() const;
    void initStyleOption(StyleOptionFrame* option);
    void redrawFrame();
    bool modifyStyle(HWND hWnd, DWORD dwRemove, DWORD dwAdd, UINT nFlags);
    bool isFrameHasStatusBar(int* statusHeight = Q_NULL) const;
    HRGN calcFrameRegion(QSize sz);
    HFONT getCaptionFont(HANDLE hTheme);
    bool hitTestContextHeaders(const QPoint& point) const;
    bool isCompositionEnabled();
    bool resolveSymbols();
    void setFrameHook();

protected slots:
    void refreshFrameTimer();

protected:
    virtual bool event(QEvent* event);
    virtual bool eventFilter(QObject* obj, QEvent* event);

public:
    static bool m_allowDwm;

    bool m_lockNCPaint;
    bool m_inUpdateFrame;
    bool m_inLayoutRequest;
    bool m_postReclalcLayout;
    bool m_active;

private:
    HICON m_hIcon;
    QWidget* m_frame;
    RibbonBar* m_ribbonBar;
    QList<ContextHeader*>* m_listContextHeaders;
    QSize m_szFrameRegion;
    QRect m_rcHeader;
    int m_frameBorder;
    int m_borderSizeBotton;
    HWND m_hwndFrame;
    QTimer m_refreshFrame;
    bool m_dwmEnabled;
    bool m_skipNCPaint;
    bool m_officeFrameEnabled;
    bool m_wasFullScreen;

private:
    class FrameHelperEventHook;
    Q_DISABLE_COPY(OfficeFrameHelperWin)
};

QTITAN_END_NAMESPACE

#endif // QTN_OFFICEFRAMEHELPER_WIN_H

