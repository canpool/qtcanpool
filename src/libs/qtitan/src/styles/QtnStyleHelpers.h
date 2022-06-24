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
#ifndef QTN_STYLEHELPERS_H
#define QTN_STYLEHELPERS_H


#include "QtitanDef.h"
#include "QtnRibbonBar.h"

QTITAN_BEGIN_NAMESPACE

class QTITAN_EXPORT DrawHelpers
{
public:
    DrawHelpers();
    virtual ~DrawHelpers();
public:
    static void draw3DRect(QPainter& pnt, QColor& col, int x, int y, int w, int h, bool up);
    static void draw3DRect(QPainter& pnt, const QColor& colLight, const QColor& colDark, int x, int y, int w, int h, bool up);

    static void draw3dRectEx(QPainter& pnt, const QRect& rect, const QColor& clrTopLeft, const QColor& clrBottomRight);
    static void draw3dRectEx(QPainter& pnt, int x, int y, int cx, int cy, const QColor& clrTopLeft, const QColor& clrBottomRight);

    static void drawRectangle(QPainter& pnt, const QRect& rc, const QColor& pen, const QColor& brush);
    static void drawTriangle(QPainter& pnt, const QPoint& pt0, const QPoint& pt1, const QPoint& pt2, const QColor& color);

    static void drawGradientFill(QPainter& pnt, const QRect& rect, const QColor& crFrom, const QColor& crTo, bool horz);
    static void drawGradientFill4(QPainter& pnt, const QRect& rect, const QColor& crFrom1, const QColor& crTo1, 
        const QColor& crFrom2, const QColor& crTo2, bool horz = true, int percentage = 50);

    static QRgb lightColor(QRgb clr, int nTint);
    static QRgb darkColor(QRgb clr, int nShade);

    static void RGBtoHSL(QRgb clr, double& h, double& s, double& l);
    static QRgb HSLtoRGB(double h, double s, double l);
    static QRgb HLStoRGB_ONE( double h, double l, double s );
    static double MakeHue_ONE(double H);
    static double MakeHue_TWO(double H);

    static QRgb colorMakePale(const QRgb& clr, double lum = .97);
    static QRgb colorMakeDarker(const QRgb& clr, double ratio = .1);
    static QRgb colorMakeLighter(const QRgb& clr, double ratio = .1);

    static QRgb pixelAlpha(const QRgb& srcPixel, int percent);

    static QRgb blendColors(QRgb crA, QRgb crB, double fAmountA);

    static QImage updateImage(const QImage& imageSrc, QRgb clrBase, QRgb clrTone);
    static QImage addaptColors(const QImage& imageSrc, QRgb clrBase, QRgb clrTone, bool clampHue = true);
    static QImage invertColors(const QImage& pxSrc);

    static int defaultDpiX();

    static qreal dpiScaled(qreal value);
    static int getDPI();
    static int getDPIToPercent();

    static QIcon createIconStyleWidget(QWidget* widget);
    static QPixmap createIconExStyleWidget(QWidget* widget, int width, int height, bool& iconView);

private:
    static double HueToRGB(double temp1, double temp2, double temp3);
    static int HueToRGB(int m1, int m2, int h);
    static QLinearGradient generateGradient(const QRect& rect, const QColor& crFrom, const QColor& crTo, bool bHorz);

private:
    static double logPixel;
    static double m_colorTolerance; // Used by 'addaptColors' method
};

class OfficeFrameHelper;

/* StyleOptionFrame */
class QTITAN_EXPORT StyleOptionFrame : public QStyleOption
{
public:
    StyleOptionFrame();
    StyleOptionFrame(const StyleOptionFrame& other);
    ~StyleOptionFrame();

public:
    void* hdc;
    QRect clientRect;
    bool active;
    bool hasStatusBar;
    bool titleVisible;
    bool maximizeFlags;
    int  frameBorder;
    int  titleBarSize;
    int  tabBarSize;
    int  statusHeight;
    bool isBackstageVisible;
    RibbonBar::TabBarPosition tabBarPosition;
};

class QTITAN_EXPORT StyleOptionBackstageButton : public QStyleOptionToolButton
{
public:
    StyleOptionBackstageButton();
    StyleOptionBackstageButton(const StyleOptionBackstageButton& other);
public:
    bool tabStyle;
    bool flatStyle;
};

class QTITAN_EXPORT StyleOptionRibbonBackstage : public QStyleOption
{
public:
    StyleOptionRibbonBackstage();
    StyleOptionRibbonBackstage(const StyleOptionRibbonBackstage& other);
public:
    int menuWidth;
};

/* StyleOptionRibbon */
class QTITAN_EXPORT StyleOptionRibbon : public QStyleOption
{
public:
    StyleOptionRibbon();
    StyleOptionRibbon(const StyleOptionRibbon& other);
    ~StyleOptionRibbon();
public:
    QRect rectTabBar;
    int titleBarHeight;
    bool minimized;
    bool isBackstageVisible;
    OfficeFrameHelper* frameHelper;
    QRegion airRegion;
    QPixmap pixTitleBackground;
    RibbonBar::TabBarPosition tabBarPosition;
};

class ContextHeader;
/* StyleOptionTitleBar */
class QTITAN_EXPORT StyleOptionTitleBar : public QStyleOptionTitleBar
{
public:
    StyleOptionTitleBar();
    StyleOptionTitleBar(const StyleOptionTitleBar &other);

public:
    OfficeFrameHelper* frameHelper;
    QRect rcTitleText;
    QRect rectTabBar;
    QRect rcRibbonClient;
    QRect rcQuickAccess;
    bool drawIcon;
    int quickAccessVisibleCount;
    bool quickAccessOnTop;
    bool quickAccessVisible;
    bool existSysButton;
    bool normalSysButton;
    bool isBackstageVisible;
    QList<ContextHeader*>* listContextHeaders;
    QRegion airRegion;
    QColor clrText;

protected:
    StyleOptionTitleBar(int version);
};

/* StyleCaptionButton */
class QTITAN_EXPORT StyleCaptionButton : public QStyleOptionToolButton
{
public:
    StyleCaptionButton();
    StyleCaptionButton(const StyleCaptionButton& other);
};

/* StyleOptionFrame */
class QTITAN_EXPORT StyleSystemToolButton : public QStyleOptionToolButton
{
public:
    StyleSystemToolButton();
    StyleSystemToolButton(const StyleSystemToolButton& other);
};

/* StyleHintReturnThemeColor */
class QTITAN_EXPORT StyleHintReturnThemeColor : public QStyleHintReturn
{
public:
    StyleHintReturnThemeColor(const QString& secName, const QString& kName, QColor col = QColor());
    ~StyleHintReturnThemeColor();
public:
    QString sectionName;
    QString keyName;
    QColor color;
    bool isValid;
};

/* StyleHintReturnThemeColor */
class QTITAN_EXPORT StyleRibbonQuickAccessBar : public QStyleOption
{
public:
    StyleRibbonQuickAccessBar();
    StyleRibbonQuickAccessBar(const StyleRibbonQuickAccessBar& other);
public:
    RibbonBar::QuickAccessBarPosition quickAccessBarPosition;
};

/* StyleRibbonOptionHeader */
class QTITAN_EXPORT StyleRibbonOptionHeader : public QStyleOptionHeader
{
public:
    StyleRibbonOptionHeader();
    StyleRibbonOptionHeader(const StyleRibbonOptionHeader& other);
public:
    QString contextText;
    RibbonBar::TabBarPosition tabBarPosition;
};

QTITAN_END_NAMESPACE

#endif // QTN_STYLEHELPERS_H
