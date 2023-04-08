/**
 * Copyleft (C) 2023 maminjie <canpool@163.com>
 **/
#pragma once

#include <QStyleOption>
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
    #include <QCommonStyle>
#else
    #include <QWindowsStyle>
#endif

#include "ribbon_global.h"

QRIBBON_BEGIN_NAMESPACE

class StyleHelper;
class CommonStylePrivate;
class CommonPaintManager;

enum BarType {
    TypeMenuBar,
    TypeNormal,
    TypePopup,
    TypeRibbon,
    TypeSplitButtonPopup,
};

enum BarPosition {
    BarTop,
    BarBottom,
    BarLeft,
    BarRight,
    BarFloating,
    BarPopup,
    BarListBox,
    BarNone
};

/* CommonStyle */
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
class QRIBBON_EXPORT CommonStyle : public QCommonStyle
#else
class QRIBBON_EXPORT CommonStyle : public QWindowsStyle
#endif
{
    Q_OBJECT
public:
    enum QPrimitiveElement {
        // PE_CustomBase = 0xf0000000 - base
        PE_Workspace = PE_CustomBase + 1,
        PE_RibbonCustomBase = 0xff000000,
    };
protected:
    CommonStyle(CommonStylePrivate *d);
    virtual ~CommonStyle();
public:
    QColor getThemeColor(const QString &sectionName, const QString &keyName, QColor color = QColor()) const;
public:
    virtual void polish(QApplication *);
    virtual void polish(QWidget *widget);
    virtual void unpolish(QApplication *);
    virtual void unpolish(QWidget *widget);
public:
    virtual void drawPrimitive(PrimitiveElement pe, const QStyleOption *opt, QPainter *p, const QWidget *w = 0) const;
    virtual void drawControl(ControlElement element, const QStyleOption *opt, QPainter *p, const QWidget *w = 0) const;
    virtual void drawComplexControl(ComplexControl cc, const QStyleOptionComplex *opt, QPainter *p,
                                    const QWidget *w = 0) const;

    virtual QSize sizeFromContents(ContentsType ct, const QStyleOption *opt, const QSize &contentsSize,
                                   const QWidget *widget = 0) const;
    virtual QRect subControlRect(ComplexControl cc, const QStyleOptionComplex *opt, SubControl sc,
                                 const QWidget *widget) const;
    virtual int pixelMetric(PixelMetric pm, const QStyleOption *option = 0, const QWidget *widget = 0) const;
    virtual int styleHint(StyleHint hint, const QStyleOption *opt = Q_NULL, const QWidget *widget = Q_NULL,
                          QStyleHintReturn *returnData = Q_NULL) const;
    virtual QPixmap standardPixmap(StandardPixmap standardPixmap, const QStyleOption *opt,
                                   const QWidget *widget = Q_NULL) const;
    virtual QIcon standardIcon(StandardPixmap standardIcon, const QStyleOption *opt = Q_NULL,
                               const QWidget *widget = Q_NULL) const;
public:
    static void splitString(const QString &str, QString &strFirstRow, QString &strSecondRow);
protected:
    virtual QIcon standardIconEx(QStyle::StandardPixmap, const QStyleOption *, const QWidget *, bool &ret) const
    {
        ret = false;
        return QIcon();
    }

    virtual bool drawWorkspace(const QStyleOption *, QPainter *, const QWidget *) const { return false; }
    virtual bool drawWidget(const QStyleOption *, QPainter *, const QWidget *) const { return false; }
    // for QForm
    virtual bool drawFrame(const QStyleOption *, QPainter *, const QWidget *) const { return false; }
    virtual bool drawFrameTabWidget(const QStyleOption *, QPainter *, const QWidget *) const { return false; }
    virtual bool drawShapedFrame(const QStyleOption *, QPainter *, const QWidget *) const { return false; }
    virtual bool drawFrameWindow(const QStyleOption *, QPainter *, const QWidget *) const { return false; }

    // for stausBar
    virtual bool drawPanelStatusBar(const QStyleOption *, QPainter *, const QWidget *) const { return false; }

    // for menuBar
    virtual bool drawMenuBarEmptyArea(const QStyleOption *, QPainter *, const QWidget *) const { return false; }
    virtual bool drawPanelMenu(const QStyleOption *, QPainter *, const QWidget *) const { return false; }
    virtual bool drawMenuItem(const QStyleOption *, QPainter *, const QWidget *) const { return false; }
    virtual bool drawFrameMenu(const QStyleOption *, QPainter *, const QWidget *) const { return false; }
    virtual bool drawMenuBarItem(const QStyleOption *, QPainter *, const QWidget *) const { return false; }
    virtual bool drawIndicatorMenuCheckMark(const QStyleOption *, QPainter *, const QWidget *) const { return false; }
    virtual bool drawIndicatorToolBarHandle(const QStyleOption *, QPainter *, const QWidget *) const { return false; }

    // for toolBar
    virtual bool drawToolBar(const QStyleOption *, QPainter *, const QWidget *) const { return false; }
    virtual bool drawToolButtonLabel(const QStyleOption *, QPainter *, const QWidget *) const { return false; }
    virtual bool drawToolButton(const QStyleOption *, QPainter *, const QWidget *) const { return false; }
    virtual bool drawPanelButtonTool(const QStyleOption *, QPainter *, const QWidget *) const { return false; }
    virtual bool drawGroupControlEntry(const QStyleOption *, QPainter *, const QWidget *) const { return false; }

    // for Buttons
    virtual bool drawIndicatorToolBarSeparator(const QStyleOption *, QPainter *, const QWidget *) const
    {
        return false;
    }
    virtual bool drawPanelButtonCommand(const QStyleOption *, QPainter *, const QWidget *) const { return false; }
    virtual bool drawIndicatorCheckRadioButton(PrimitiveElement, const QStyleOption *, QPainter *,
                                               const QWidget *) const
    {
        return false;
    }
    virtual bool drawIndicatorViewItemCheck(PrimitiveElement, const QStyleOption *, QPainter *, const QWidget *) const
    {
        return false;
    }
    virtual bool drawFrameDefaultButton(const QStyleOption *, QPainter *, const QWidget *) const { return false; }

    virtual bool drawSizeGrip(const QStyleOption *, QPainter *, const QWidget *) const { return false; }
    virtual bool drawSlider(const QStyleOptionComplex *, QPainter *, const QWidget *) const { return false; }

    // for ScrollBar
    virtual bool drawScrollBar(const QStyleOptionComplex *, QPainter *, const QWidget *) const { return false; }
    virtual bool drawScrollBarLine(ControlElement, const QStyleOption *, QPainter *, const QWidget *) const
    {
        return false;
    }
    virtual bool drawScrollBarSlider(const QStyleOption *, QPainter *, const QWidget *) const { return false; }
    virtual bool drawScrollBarPage(ControlElement, const QStyleOption *, QPainter *, const QWidget *) const
    {
        return false;
    }

    // for editors
    virtual bool drawControlEdit(const QStyleOption *, QPainter *, const QWidget *) const { return false; }
    virtual bool drawFrameLineEdit(const QStyleOption *, QPainter *, const QWidget *) const { return false; }
    virtual bool drawComboBox(const QStyleOptionComplex *, QPainter *, const QWidget *) const { return false; }

    // for DockWidget
    virtual bool drawDockWidgetTitle(const QStyleOption *, QPainter *, const QWidget *) const { return false; }
    virtual bool drawIndicatorDockWidgetResizeHandle(const QStyleOption *, QPainter *, const QWidget *) const
    {
        return false;
    }

    // for MDI
    virtual bool drawMdiControls(const QStyleOptionComplex *, QPainter *, const QWidget *) const { return false; }

    // for TitleBar
    virtual bool drawTitleBar(const QStyleOptionComplex *, QPainter *, const QWidget *) const { return false; }
    // for TabBar
    virtual bool drawTabBarTab(const QStyleOption *, QPainter *, const QWidget *) const { return false; }
    virtual bool drawTabBarTabShape(const QStyleOption *, QPainter *, const QWidget *) const { return false; }
    virtual bool drawTabBarTabLabel(const QStyleOption *, QPainter *, const QWidget *) const { return false; }
    virtual bool drawIndicatorTabClose(const QStyleOption *, QPainter *, const QWidget *) const { return false; }

    // for SpinBox
    virtual bool drawSpinBox(const QStyleOptionComplex *, QPainter *, const QWidget *) const { return false; }
    // for ComboBox
    virtual bool drawComboBoxLabel(ControlElement, const QStyleOption *, QPainter *, const QWidget *) const
    {
        return false;
    }

    // for ProgressBar
    virtual bool drawProgressBarGroove(const QStyleOption *, QPainter *, const QWidget *) const { return false; }
    // for ToolBox
    virtual bool drawToolBoxTab(const QStyleOption *, QPainter *, const QWidget *) const { return false; }
    virtual bool drawToolBoxTabShape(const QStyleOption *, QPainter *, const QWidget *) const { return false; }
    virtual bool drawToolBoxTabLabel(const QStyleOption *, QPainter *, const QWidget *) const { return false; }
    // for ViewItem
    virtual bool drawItemViewItem(const QStyleOption *, QPainter *, const QWidget *) const { return false; }
    virtual bool drawPanelItemViewItem(const QStyleOption *, QPainter *, const QWidget *) const { return false; }
    virtual bool drawHeader(const QStyleOption *, QPainter *, const QWidget *) const { return false; }
    virtual bool drawHeaderSection(const QStyleOption *, QPainter *, const QWidget *) const { return false; }
    virtual bool drawHeaderEmptyArea(const QStyleOption *, QPainter *, const QWidget *) const { return false; }
    virtual bool drawIndicatorHeaderArrow(const QStyleOption *, QPainter *, const QWidget *) const { return false; }

    // shared
    virtual void drawDropDownGlyph(QPainter *, QPoint, bool, bool, bool, bool) const {}
    virtual bool drawIndicatorArrow(PrimitiveElement, const QStyleOption *, QPainter *, const QWidget *) const
    {
        return false;
    }
    virtual bool drawPanelTipLabel(const QStyleOption *, QPainter *, const QWidget *) const { return false; }
    virtual void drawRectangle(QPainter *, const QRect &, bool, bool, bool, bool, bool, BarType, BarPosition) const {}

    CommonPaintManager *paintManager() const;
protected:   // shared
    virtual QPixmap cached(const QString &img) const;
protected:
    QRect sourceRectImage(QRect rcSrc, int state = 0, int count = 1) const;
    void clearCache();
    void drawImagePart(const QPixmap &dcSrc, QPainter &p, const QRect &rcDest, const QRect &rcSrc, bool bAlpha) const;

    void drawImage(const QPixmap &soSrc, QPainter &p, const QRect &rcDest, const QRect &rcSrc, QRect rcSizingMargins,
                   QColor clrTransparent, bool alphaBlend = false) const;
    void drawImage(const QPixmap &dcSrc, QPainter &p, const QRect &rcDest, const QRect &rcSrc,
                   QRect rcSizingMargins = QRect(QPoint(0, 0), QPoint(0, 0)), bool alphaBlend = false) const;

    void drawPixmap(const QPixmap &dcSrc, QPainter &p, const QRect &rcDest, const QRect &rcSrc, bool alpha,
                    QRect rcSizingMargins = QRect(QPoint(0, 0), QPoint(0, 0)), bool alphaBlend = false) const;

    void setDefaultStyle(QStyle *pStyle);
    QStyle *defaultStyle() const;

    StyleHelper &helper() const;
protected:
    enum TypePaint {
        tp_PrimitiveElement,
        tp_ControlElement,
        tp_ComplexControl,
    };
    bool paintAnimation(TypePaint, int nPrim, const QStyleOption *, QPainter *, const QWidget *,
                        int iDurationOver = 40) const;
    virtual bool transitionsEnabled() const;
protected:
    virtual bool isNativeDialog(const QWidget *wid) const;
    virtual bool showToolTip(const QPoint &pos, QWidget *w);
protected:
    virtual bool event(QEvent *event);
    virtual bool eventFilter(QObject *watched, QEvent *event);
protected:
    QTC_DECLARE_PRIVATE(CommonStyle)
private:
    friend class CommonPaintManager;
    Q_DISABLE_COPY(CommonStyle)
};

QRIBBON_END_NAMESPACE
