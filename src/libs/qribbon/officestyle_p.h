/**
 * Copyleft (C) 2023 maminjie <canpool@163.com>
 **/
#pragma once

#include "commonstyle_p.h"
#include "officestyle.h"

QRIBBON_BEGIN_NAMESPACE

class OfficePaintManagerInterface
{
public:
    virtual ~OfficePaintManagerInterface() {}
    virtual void polish(QWidget *widget) = 0;
public:
    virtual QIcon standardIconEx(QStyle::StandardPixmap, const QStyleOption *, const QWidget *, bool &ret) const = 0;
    virtual bool drawWorkspace(const QStyleOption *, QPainter *, const QWidget *) const = 0;
    // for TitleBar
    virtual bool drawToolBar(const QStyleOption *, QPainter *, const QWidget *) const = 0;
    virtual bool drawTitleBar(const QStyleOptionComplex *, QPainter *, const QWidget *) const = 0;
    virtual bool drawIndicatorToolBarSeparator(const QStyleOption *, QPainter *, const QWidget *) const = 0;
    // for DockWidget
    virtual bool drawIndicatorDockWidgetResizeHandle(const QStyleOption *, QPainter *, const QWidget *) const = 0;
    // for menuBar
    virtual bool drawPanelMenu(const QStyleOption *, QPainter *, const QWidget *) const = 0;
    virtual bool drawFrameMenu(const QStyleOption *opt, QPainter *p, const QWidget *w) const = 0;
    virtual bool drawIndicatorMenuCheckMark(const QStyleOption *, QPainter *, const QWidget *) const = 0;
    // for Buttons
    virtual bool drawIndicatorCheckRadioButton(QStyle::PrimitiveElement element, const QStyleOption *, QPainter *,
                                               const QWidget *) const = 0;
    virtual bool drawIndicatorViewItemCheck(QStyle::PrimitiveElement, const QStyleOption *, QPainter *,
                                            const QWidget *) const = 0;
    virtual bool drawPanelButtonCommand(const QStyleOption *, QPainter *, const QWidget *) const = 0;
    // shared
    virtual bool drawPanelTipLabel(const QStyleOption *, QPainter *, const QWidget *) const = 0;
    // for editors
    virtual bool drawControlEdit(const QStyleOption *opt, QPainter *p, const QWidget *w) const = 0;
    virtual bool drawFrameLineEdit(const QStyleOption *, QPainter *, const QWidget *) const = 0;
    virtual bool drawComboBox(const QStyleOptionComplex *opt, QPainter *p, const QWidget *w) const = 0;
    // for SpinBox
    virtual bool drawSpinBox(const QStyleOptionComplex *, QPainter *, const QWidget *) const = 0;
    // for ToolBox
    virtual bool drawToolBoxTabShape(const QStyleOption *, QPainter *, const QWidget *) const = 0;
    // for TabBar
    virtual bool drawTabBarTabShape(const QStyleOption *, QPainter *, const QWidget *) const = 0;
    virtual bool drawIndicatorTabClose(const QStyleOption *opt, QPainter *p, const QWidget *w) const = 0;
    // for QForm
    virtual bool drawShapedFrame(const QStyleOption *, QPainter *, const QWidget *) const = 0;
    virtual bool drawFrameWindow(const QStyleOption *, QPainter *, const QWidget *) const = 0;
    // for ScrollBar
    virtual bool drawScrollBar(const QStyleOptionComplex *opt, QPainter *p, const QWidget *w) const = 0;
    virtual bool drawScrollBarLine(QStyle::ControlElement element, const QStyleOption *opt, QPainter *p,
                                   const QWidget *w) const = 0;
    virtual bool drawScrollBarSlider(const QStyleOption *, QPainter *, const QWidget *) const = 0;
    virtual bool drawScrollBarPage(QStyle::ControlElement element, const QStyleOption *, QPainter *,
                                   const QWidget *) const = 0;
    // for stausBar
    virtual bool drawPanelStatusBar(const QStyleOption *, QPainter *, const QWidget *) const = 0;
    // for SizeGrip
    virtual bool drawSizeGrip(const QStyleOption *, QPainter *, const QWidget *) const = 0;
    // for ViewItem
    virtual bool drawPanelItemViewItem(const QStyleOption *, QPainter *, const QWidget *) const = 0;
    //
    virtual bool drawHeaderSection(const QStyleOption *, QPainter *, const QWidget *) const = 0;
    virtual bool drawHeaderEmptyArea(const QStyleOption *opt, QPainter *p, const QWidget *w) const = 0;
    virtual bool drawIndicatorHeaderArrow(const QStyleOption *, QPainter *, const QWidget *) const = 0;

    virtual bool drawIndicatorArrow(QStyle::PrimitiveElement, const QStyleOption *, QPainter *,
                                    const QWidget *) const = 0;

    virtual bool drawGalleryToolButton(const QStyleOption *, QPainter *, const QWidget *) const = 0;
    virtual void drawMenuItemSeparator(const QStyleOption *, QPainter *, const QWidget *) const = 0;

    virtual void drawRectangle(QPainter *p, const QRect &rect, bool selected, bool pressed, bool enabled, bool checked,
                               bool popuped, QRibbon::BarType barType, QRibbon::BarPosition barPos) const = 0;

    virtual void setupPalette(QWidget *widget) const = 0;
public:
    virtual bool pixelMetric(QStyle::PixelMetric pm, const QStyleOption *opt, const QWidget *w, int &ret) const = 0;
    virtual bool styleHint(QStyle::StyleHint hint, const QStyleOption *opt, const QWidget *widget,
                           QStyleHintReturn *returnData, int &ret) const = 0;
public:
    virtual bool isTopLevelMenuItemUpperCase(const QWidget *) const = 0;
};

QRIBBON_END_NAMESPACE

Q_DECLARE_INTERFACE(QRibbon::OfficePaintManagerInterface, "canpool.QRibbon.OfficePaintManagerInterface")

QRIBBON_BEGIN_NAMESPACE

class OfficeStylePrivate : public CommonStylePrivate
{
public:
    QTC_DECLARE_EX_PUBLIC(OfficeStyle)
public:
    explicit OfficeStylePrivate();
    virtual ~OfficeStylePrivate();
public:
    void initialization();

    void setBrush(QWidget *widget);
    void unsetBrush(QWidget *widget);

    void updateColors();
    void refreshMetrics();

    QString theme(const QString &str) const;
    OfficePaintManagerInterface *officePaintManager() const;
    virtual void makePaintManager();

    QColor accentIndexToColor(OfficeStyle::AccentColor accentcolor) const;
    bool isScrollBarsIgnored() const;
public:
    QColor m_clrAccent;
    OfficeStyle::AccentColor m_curAccentColor;

    // [Window]
    QColor m_clrFrame;
    QColor m_clr3DFace;
    QColor m_clrFrameBorderActive0;
    QColor m_clrFrameBorderActive1;
    QColor m_clrFrameBorderActive2;
    QColor m_clrFrameBorderActive3;

    QColor m_clrFrameBorderInactive0;
    QColor m_clrFrameBorderInactive1;
    QColor m_clrFrameBorderInactive2;
    QColor m_clrFrameBorderInactive3;

    QColor m_clrHighlightBorder;
    QColor m_clrHighlightPushedBorder;
    QColor m_clrHighlightCheckedBorder;
    QColor m_clrHighlightPushed;
    QColor m_clrHighlightChecked;
    QColor m_clrHighlight;
    QColor m_clrHighlightNC;

    QColor m_clrFrameCaptionTextInActive;
    QColor m_clrFrameCaptionTextActive;

    QColor m_clrTabNormalText;
    QColor m_clrTabHighlightText;
    QColor m_clrHighlightText;
    QColor m_clrTabSelectedText;
    QColor m_clrToolBarText;
    QColor m_clrToolBarGrayText;
    QColor m_clrMenuPopupGripperShadow;

    QColor m_clrTooltipLight;
    QColor m_clrTooltipDark;
    QColor m_clrTooltipBorder;

    // [Workspace]
    QColor m_clrAppWorkspace;
    QColor m_clrWorkspaceClientTop;
    QColor m_clrWorkspaceClientMiddle;
    QColor m_clrWorkspaceClientBottom;

    // [Edit]
    QColor m_clrControlEditNormal;
    QColor m_clrMenuPopupText;
    QColor m_clrMenuGripper;
    QColor m_clrMenuPopupSeparator;

    QColor m_clrMenuBarGrayText;
    QColor m_clrHighlightDisabledBorder;
    QColor m_clrMenubarFace;

    // [DockingPane]
    QPalette m_palNormalCaption;
    QColor m_clrNormalCaptionText;
    QPalette m_palActiveCaption;
    QColor m_clrActiveCaptionText;
    QPalette m_palSplitter;

    // [StatusBar]
    QColor m_clrStatusBarText;
    QColor m_clrStatusBarShadow;
    QPalette m_palStatusBarTop;
    QPalette m_palStatusBarBottom;

    // [Controls]
    QColor m_crBorderShadow;
    QColor m_tickSlider;

    // [Toolbar]
    QColor m_clrEditCtrlBorder;
    QColor m_clrEditCtrlDisabledBorder;
    QColor m_clrMenuBarText;
    QColor m_ToolbarGripper;
    QColor m_clrControlGallerySelected;
    QColor m_clrControlGalleryNormal;
    QColor m_clrControlGalleryBorder;
    QColor m_clrControlGalleryMenuBk;
    QColor m_clrControlGalleryLabel;
    QColor m_clrDockBar;
    QColor m_clrMenubarBorder;
    QColor m_clrToolbarFace;
    QColor m_clrToolbarSeparator;

    // [ShortcutBar]
    QColor m_clrShortcutItemShadow;

    // [PopupControl]
    QColor m_clrBackgroundLight;
    QColor m_clrBackgroundDark;
    QColor m_clrCaptionLight;
    QColor m_clrCaptionDark;

    QPalette m_clrCommandBar;
    QPalette m_clrPopupControl;
    QPalette m_palLunaSelected;
    QPalette m_palLunaPushed;
    QPalette m_palLunaChecked;

    // [ReportControl]
    QPalette m_palGradientColumn;
    QPalette m_palGradientColumnPushed;
    QColor m_clrColumnSeparator;
    QColor m_clrGridLineColor;
    QColor m_clrSelectionBackground;

    bool m_dpiAware;
    bool m_animEnabled;
    bool m_ignoreDialogs;
    bool m_ignoreScrollBars;
    bool m_ignoreMDIWindowTitle;

    OfficeStyle::OfficePopupDecoration m_typePopupStyle;

    QStyle *m_popupProxy;
    uint m_refCountProxy;

    OfficeStyle::Theme m_themeType;
    QHash<const QWidget *, QBrush> m_customBrushWidgets;   // widgets whose brush we tampered
};

class OfficePaintManager : public CommonPaintManager, public OfficePaintManagerInterface
{
    Q_OBJECT
    Q_INTERFACES(QRibbon::OfficePaintManagerInterface)
public:
    explicit OfficePaintManager(CommonStyle *baseStyle);
    virtual ~OfficePaintManager();

    enum ImageIcons {
        Icon_ArowDown,
        Icon_ArowRight,
        Icon_Check,
        Icon_Minimize,
        Icon_Restore,
        Icon_Close,
        Icon_Maximize,
        Icon_ArowUp,
        Icon_ArowShowAll,
        Icon_ArowLeft,
        Icon_CloseSmall,
        Icon_MoreButtons,
        Icon_Radio,
        Icon_ArowDownLarge,
        Icon_ArowRightLarge,
        Icon_PinHorz,
        Icon_PinVert,
        Icon_ArowLeftLarge,
        Icon_ArowFirst,
        Icon_ArowLast,
        Icon_ArowRightTab3d,
        Icon_ArowLeftTab3d,
        Icon_ArowRightDsbldTab3d,
        Icon_ArowLeftDsbldTab3d,
        Icon_ArowUpLarge,
        Icon_ArowPageLeft,
        Icon_ArowPageRight,
        Icon_ArowBack,
        Icon_ArowForward,
        Icon_CustomizeArowDown,
        Icon_CustomizeArowLeft,
        Icon_CustomizeMoreButtonsHorz,
        Icon_CustomizeMoreButtonsVert,
        Icon_CustomizeArowDownBold,
        Icon_CloseBold,
        Icon_LaunchArrow,
        Icon_MaximizeRibbon,
        Icon_MinimizeRibbon,
    };

    enum ImageState {
        Black,
        Gray,
        LtGray,
        White,
        DkGray,
        Black2,
    };
public:
    OfficeStyle::Theme theme() const;
    QString theme(const QString &str) const;
    QColor textColor(bool selected, bool pressed, bool enabled, bool checked, bool popuped, BarType barType,
                     BarPosition barPosition) const;
public:
    virtual void polish(QWidget *widget);
public:
    virtual QIcon standardIconEx(QStyle::StandardPixmap, const QStyleOption *, const QWidget *, bool &ret) const;
    virtual bool drawWorkspace(const QStyleOption *, QPainter *, const QWidget *) const;
    // for TitleBar
    virtual bool drawToolBar(const QStyleOption *, QPainter *, const QWidget *) const;
    virtual bool drawTitleBar(const QStyleOptionComplex *, QPainter *, const QWidget *) const;
    virtual bool drawIndicatorToolBarSeparator(const QStyleOption *, QPainter *, const QWidget *) const;
    // for DockWidget
    virtual bool drawIndicatorDockWidgetResizeHandle(const QStyleOption *, QPainter *, const QWidget *) const;
    // for menuBar
    virtual bool drawPanelMenu(const QStyleOption *, QPainter *, const QWidget *) const;
    virtual bool drawFrameMenu(const QStyleOption *opt, QPainter *p, const QWidget *w) const;
    virtual bool drawIndicatorMenuCheckMark(const QStyleOption *, QPainter *, const QWidget *) const;

    // for Buttons
    virtual bool drawIndicatorCheckRadioButton(QStyle::PrimitiveElement element, const QStyleOption *, QPainter *,
                                               const QWidget *) const;
    virtual bool drawIndicatorViewItemCheck(QStyle::PrimitiveElement, const QStyleOption *, QPainter *,
                                            const QWidget *) const;
    virtual bool drawPanelButtonCommand(const QStyleOption *, QPainter *, const QWidget *) const;

    // shared
    virtual bool drawPanelTipLabel(const QStyleOption *, QPainter *, const QWidget *) const;
    // for editors
    virtual bool drawControlEdit(const QStyleOption *opt, QPainter *p, const QWidget *w) const;
    virtual bool drawFrameLineEdit(const QStyleOption *, QPainter *, const QWidget *) const;
    virtual bool drawComboBox(const QStyleOptionComplex *opt, QPainter *p, const QWidget *w) const;
    // for SpinBox
    virtual bool drawSpinBox(const QStyleOptionComplex *, QPainter *, const QWidget *) const;
    // for ToolBox
    virtual bool drawToolBoxTabShape(const QStyleOption *, QPainter *, const QWidget *) const;
    // for TabBar
    virtual bool drawTabBarTabShape(const QStyleOption *, QPainter *, const QWidget *) const;
    virtual bool drawIndicatorTabClose(const QStyleOption *opt, QPainter *p, const QWidget *w) const;
    // for QForm
    virtual bool drawShapedFrame(const QStyleOption *, QPainter *, const QWidget *) const;
    virtual bool drawFrameWindow(const QStyleOption *, QPainter *, const QWidget *) const;

    // for ScrollBar
    virtual bool drawScrollBar(const QStyleOptionComplex *opt, QPainter *p, const QWidget *w) const;
    virtual bool drawScrollBarLine(QStyle::ControlElement element, const QStyleOption *opt, QPainter *p,
                                   const QWidget *w) const;
    virtual bool drawScrollBarSlider(const QStyleOption *, QPainter *, const QWidget *) const;
    virtual bool drawScrollBarPage(QStyle::ControlElement element, const QStyleOption *, QPainter *,
                                   const QWidget *) const;
    // for stausBar
    virtual bool drawPanelStatusBar(const QStyleOption *, QPainter *, const QWidget *) const;
    // for SizeGrip
    virtual bool drawSizeGrip(const QStyleOption *, QPainter *, const QWidget *) const;
    // for ViewItem
    virtual bool drawPanelItemViewItem(const QStyleOption *, QPainter *, const QWidget *) const;
    //
    virtual bool drawHeaderSection(const QStyleOption *, QPainter *, const QWidget *) const;
    virtual bool drawHeaderEmptyArea(const QStyleOption *opt, QPainter *p, const QWidget *w) const;
    virtual bool drawIndicatorHeaderArrow(const QStyleOption *, QPainter *, const QWidget *) const;

    virtual bool drawIndicatorArrow(QStyle::PrimitiveElement, const QStyleOption *, QPainter *, const QWidget *) const;

    virtual bool drawGalleryToolButton(const QStyleOption *, QPainter *, const QWidget *) const;
    virtual void drawMenuItemSeparator(const QStyleOption *, QPainter *, const QWidget *) const;

    virtual void drawRectangle(QPainter *p, const QRect &rect, bool selected, bool pressed, bool enabled, bool checked,
                               bool popuped, BarType barType, BarPosition barPos) const;

    virtual void setupPalette(QWidget *widget) const;
public:
    virtual bool pixelMetric(QStyle::PixelMetric pm, const QStyleOption *opt, const QWidget *w, int &ret) const;
    virtual bool styleHint(QStyle::StyleHint hint, const QStyleOption *opt, const QWidget *widget,
                           QStyleHintReturn *returnData, int &ret) const;
public:
    virtual void modifyColors();
    virtual bool isTopLevelMenuItemUpperCase(const QWidget *) const { return false; }
    virtual QColor menuLabelColor() const;
protected:
    void drawSysButton(QPainter *p, const QRect &rect, bool down, bool highlighted) const;
    ImageState stateByColor(const QColor &color, bool backgroundColor) const;

    void drawIcon(QPainter *p, const QPoint &pos, ImageIcons index, ImageState state, const QSize &sz) const;
    void drawIcon(QPainter *p, const QRect &rect, ImageIcons index, ImageState state,
                  const QSize &sz = QSize(0, 0)) const;
    void drawIconByColor(QPainter *p, ImageIcons index, const QRect &rect, const QColor &color,
                         bool backgroundColor = true, const QSize &sizeImage = QSize(0, 0)) const
    {
        drawIcon(p, rect, index, stateByColor(color, backgroundColor), sizeImage);
    }
    void drawIconByColor(QPainter *p, ImageIcons index, const QPoint &pos, const QColor &color,
                         bool backgroundColor = true, const QSize &sizeImage = QSize(9, 9)) const
    {
        drawIcon(p, pos, index, stateByColor(color, backgroundColor), sizeImage);
    }

    QPixmap mapTo3dColors(const QPixmap &px, const QColor &clrSrc, const QColor &clrDest) const;
    void loadPixmap();
protected:
    QColor m_clrAccentLight;
    QColor m_clrAccentHilight;
    QColor m_clrAccentText;
    QColor m_clrHighlightDn;
    QColor m_clrHighlightDnNC;
    QColor m_clrBarShadow;
    QColor m_clrBarHilite;
    QColor m_clrBarText;
    QColor m_clrBarDkShadow;
    QColor m_clrBarFace;
    QColor m_clrControlEditDisabled;
    QColor m_clrBarLight;
    QColor m_clrTextDisabled;
    QColor m_clrControl;
    QColor m_clrEditBoxBorder;
    QColor m_clrHighlightMenuItem;
    QColor m_clrHighlightChecked;
    QColor m_clrMenuItemBorder;

    QPixmap m_ImagesSrc;
    QPixmap m_ImagesBlack;
    QPixmap m_ImagesDkGray;
    QPixmap m_ImagesGray;
    QPixmap m_ImagesLtGray;
    QPixmap m_ImagesWhite;
    QPixmap m_ImagesBlack2;
private:
    Q_DISABLE_COPY(OfficePaintManager)
};

QRIBBON_END_NAMESPACE
