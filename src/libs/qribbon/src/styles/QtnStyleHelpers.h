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
#ifndef QTN_STYLEHELPERS_H
#define QTN_STYLEHELPERS_H


#include "QtitanDef.h"
#include "QtnOfficeDefines.h"

namespace Qtitan
{
    /* DrawHelpers */
    class QTITAN_EXPORT DrawHelpers
    {
    public:
        DrawHelpers();
        virtual ~DrawHelpers();
    public:
        static void draw3DRect(QPainter& pnt, QColor& col, int x, int y, int w, int h, bool up);
        static void draw3DRect(QPainter& pnt, const QColor& colLight, const QColor& colDark, int x, int y, int w, int h, bool up);
        static void drawRectangle(QPainter& pnt, const QRect& rc, const QColor& pen, const QColor& brush);
        static void drawTriangle(QPainter& pnt, const QPoint& pt0, const QPoint& pt1, const QPoint& pt2, const QColor& color);

        static void drawGradientFill(QPainter& pnt, const QRect& rect, const QColor& crFrom, const QColor& crTo, bool bHorz);

        static QRgb lightColor(QRgb clr, int nTint);
        static QRgb darkColor(QRgb clr, int nShade);

        static void RGBtoHSL(QRgb clr, double& h, double& s, double& l);

        static QRgb HSLtoRGB(double h, double s, double l);

        static QRgb blendColors(QRgb crA, QRgb crB, double fAmountA);

        static qreal dpiScaled(qreal value);
        static int getDPI();
        static int getDPIToPercent();

    private:
        static double HueToRGB(double temp1, double temp2, double temp3);
        static int HueToRGB(int m1, int m2, int h);
        static QLinearGradient generateGradient(const QRect& rect, const QColor& crFrom, const QColor& crTo, bool bHorz);
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
        QRect clientRect;
        bool active;
        bool hasStatusBar;
        bool titleVisible;
        bool maximizeFlags;
        int  frameBorder;
        int  titleBarSize;
        int  statusHeight;
    };

    /* StyleOptionBackstageButton */
    class QTITAN_EXPORT StyleOptionBackstageButton : public QStyleOptionToolButton
    {
    public:
        StyleOptionBackstageButton();
        StyleOptionBackstageButton(const StyleOptionBackstageButton& other);
    public:
        bool tabStyle;
        bool flatStyle;
    };

    /* StyleOptionRibbon */
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
        OfficeFrameHelper* frameHelper;
        QRegion airRegion;
    };

    class ContextHeader;
    /* QStyleOptionTitleBar */
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
        QList<ContextHeader*>* listContextHeaders;
        QRegion airRegion;

    protected:
        StyleOptionTitleBar(int version);
    };

    /* StyleOptionFrame */
    class QTITAN_EXPORT StyleSystemToolButton : public QStyleOptionToolButton
    {
    public:
        StyleSystemToolButton();
        StyleSystemToolButton(const StyleSystemToolButton& other);

    public:
        QColor colorBackground;
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
    };

}; //namespace Qtitan

#endif // QTN_STYLEHELPERS_H
