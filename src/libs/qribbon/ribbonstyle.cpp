/**
 * Copyleft (C) 2023 maminjie <canpool@163.com>
 **/
#include <QApplication>
#include <QBitmap>
#include <QLabel>
#include <QMdiArea>
#include <QPaintEngine>
#include <QPainter>
#include <QTabBar>
#include <qevent.h>

#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
    #include <qdrawutil.h>
#endif /* Q_OS_WIN*/

#ifdef Q_OS_WIN
    #include <qt_windows.h>
#endif /* Q_OS_WIN*/

#include "commonstyle_p.h"
#include "officeframehelper.h"
#include "officepopupmenu.h"
#include "ribbon_p.h"
#include "ribbonbackstageview.h"
#include "ribbonbar_p.h"
#include "ribbonbutton_p.h"
#include "ribbongallery.h"
#include "ribbongroup.h"
#include "ribbongroup_p.h"
#include "ribbonmainwindow.h"
#include "ribbonquickaccessbar.h"
#include "ribbonstatusbar.h"
#include "ribbonstyle.h"
#include "ribbonstyle_p.h"
#include "ribbonsystempopupbar.h"
#include "ribbontabbar.h"
#include "ribbontooltip.h"
#include "stylehelpers.h"

#if defined(__GNUC__)
    #pragma GCC diagnostic ignored "-Wswitch"
#endif

QRIBBON_USE_NAMESPACE

/*!
    \enum RibbonStyle::QPixelMetric

    This enum describes the various available pixel metrics of QRibbon RibbonStyle. A pixel
    metric is a style dependent size represented by a single pixel
    value.
    \value PM_RibbonReducedGroupWidth
    \value PM_RibbonHeightGroup
    \value PM_RibbonHeightCaptionGroup
    \value PM_RibbonTabsHeight
*/

/*!
    \enum RibbonStyle::QPrimitiveElement

    This enum describes the various primitive elements of QRibbon RibbonStyle. A
    primitive element is a common GUI element, such as a checkbox
    indicator or button bevel.

    \value PE_RibbonPopupBarButton
    \value PE_RibbonFileButton
    \value PE_RibbonOptionButton
    \value PE_RibbonGroupScrollButton
    \value PE_RibbonSliderButton
    \value PE_RibbonTab
    \value PE_RibbonContextHeaders
    \value PE_RibbonQuickAccessButton
    \value PE_RibbonFillRect
    \value PE_RibbonRect
    \value PE_RibbonKeyTip
    \value PE_Backstage
*/

/*!
    \enum RibbonStyle::QContentsType

    \value CT_RibbonSliderButton
    \value CT_RibbonGroupButton

*/

/*!
    \enum RibbonStyle::QControlElement

    \value CE_RibbonTabShapeLabel
    \value CE_RibbonFileButtonLabel
    \value CE_RibbonBar
    \value CE_RibbonGroups
    \value CE_Group
    \value CE_ReducedGroup
    \value CE_PopupSizeGrip
*/

/*!
    \enum RibbonStyle::QStyleHint

    \value SH_FlatFrame
    \value SH_RibbonBackstageHideTabs
*/

static const int windowsSepHeight = 9;         // height of the separator
static const int windowsItemFrame = 2;         // menu item frame width
static const int windowsItemHMargin = 3;       // menu item hor text margin
static const int windowsCheckMarkWidth = 16;   // checkmarks width on windows
static const int windowsItemVMargin = 2;       // menu item ver text margin
static const int windowsRightBorder = 15;      // right border on windows
static const int windowsTabSpacing = 12;       // space between text and tab
static const int windowsArrowHMargin = 6;      // arrow horizontal margin
static const int splitActionPopupWidth = 20;   // Split button drop down width in popups

#ifdef Q_OS_WIN
    #if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
QT_BEGIN_NAMESPACE
Q_GUI_EXPORT QPixmap qt_pixmapFromWinHICON(HICON icon);
QT_END_NAMESPACE
    #endif   // QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
#endif       // Q_OS_WIN

static bool use2000style = true;

/* RibbonStylePrivate */
RibbonStylePrivate::RibbonStylePrivate()
{
    m_fileButtonImageCount = 3;
    m_flatFrame = false;
    m_destroyKeyTips = false;
    m_completeKey = false;
    m_blockKeyTips = false;
    m_isActiveTabAccented = false;
}

#ifdef Q_OS_WIN
const double g_logPixel = 96.0;
static QFont qtc_LOGFONTtoQFont(LOGFONT &lf, bool dpiaware)
{
    QString family = QString::fromWCharArray(lf.lfFaceName);
    QFont qf(family);
    qf.setItalic(lf.lfItalic);
    if (lf.lfWeight != FW_DONTCARE) {
        int weight;
        if (lf.lfWeight < 400)
            weight = QFont::Light;
        else if (lf.lfWeight < 600)
            weight = QFont::Normal;
        else if (lf.lfWeight < 700)
            weight = QFont::DemiBold;
        else if (lf.lfWeight < 800)
            weight = QFont::Bold;
        else
            weight = QFont::Black;
        qf.setWeight(weight);
    }
    int lfh = qAbs(lf.lfHeight);

    double factor = 64.0;
    if (QSysInfo::windowsVersion() <= QSysInfo::WV_XP)
        factor = 72.0;

    HDC displayDC = GetDC(0);
    double currentlogPixel = (double)GetDeviceCaps(displayDC, LOGPIXELSY);
    double delta = g_logPixel / currentlogPixel;
    double scale = factor * (dpiaware ? 1 : delta);

    qf.setPointSizeF((double)lfh * scale / currentlogPixel);

    ReleaseDC(0, displayDC);

    qf.setUnderline(false);
    qf.setOverline(false);
    qf.setStrikeOut(false);

    return qf;
}
#endif

void RibbonStylePrivate::initialization()
{
    QTC_Q(RibbonStyle);
    updateColors();
    QFont fontRibbon = q->font(0);
    QApplication::setFont(fontRibbon, "QRibbon::RibbonBar");
}

void RibbonStylePrivate::updateColors()
{
    QTC_Q(RibbonStyle);

    m_clrFileButtonText = QColor(255, 255, 255);
    m_clrRibbonGrayText = m_clrMenuBarGrayText;

    StyleHelper &helper = q->helper();

    // [Ribbon]
    m_lineWidthTop = helper.getInteger("Ribbon", "LineWidthTop", 1);
    m_clrRibbonGroupCaptionText = helper.getColor("Ribbon", "GroupCaptionText");
    m_clrRibbonFace = helper.getColor("Ribbon", "RibbonFace");
    m_clrRibbonTabBarBackground = helper.getColor("Ribbon", "RibbonTabBarBackground");
    m_clrRibbonTabBarText = helper.getColor("Ribbon", "RibbonTabBarText");
    m_clrRibbonSeparator = helper.getColor("Ribbon", "RibbonSeparator");
    m_clrRibbonLight = helper.getColor("Ribbon", "RibbonLight");

    m_clrRibbonText = helper.getColor("Ribbon", "RibbonText");
    m_clrGroupClientText = helper.getColor("Ribbon", "GroupClientText");
    m_clrGroupClientGrayText = QColor(141, 141, 141);
    m_clrRibbonInactiveFace = helper.getColor("Ribbon", "RibbonInactiveFace", m_clrRibbonFace);

    m_clrMinimizedFrameEdgeHighLight = helper.getColor("Ribbon", "MinimizedFrameEdgeHighLight");
    m_clrMinimizedFrameEdgeShadow = helper.getColor("Ribbon", "MinimizedFrameEdgeShadow");
    m_flatFrame = (bool)helper.getInteger("Window", "FlatFrame", 0);
    m_fileButtonImageCount = helper.getInteger("Ribbon", "FileButtonImageCount", 3);

    m_paintManager->modifyColors();
}

#if (QT_VERSION < QT_VERSION_CHECK(5, 7, 0))
void RibbonStylePrivate::tabLayout(const QStyleOptionTabV3 *opt, const QWidget *widget, QRect *textRect,
                                   QRect *iconRect) const
#else
void RibbonStylePrivate::tabLayout(const QStyleOptionTab *opt, const QWidget *widget, QRect *textRect,
                                   QRect *iconRect) const
#endif
{
    QTC_Q(const RibbonStyle);
    const QStyle *proxyStyle = q->proxy();

    Q_ASSERT(textRect);
    Q_ASSERT(iconRect);
    QRect tr = opt->rect;
    bool verticalTabs = opt->shape == QTabBar::RoundedEast || opt->shape == QTabBar::RoundedWest ||
                        opt->shape == QTabBar::TriangularEast || opt->shape == QTabBar::TriangularWest;

    bool botton = opt->shape == QTabBar::RoundedSouth;

    if (verticalTabs)
        tr.setRect(0, 0, tr.height(), tr.width());   // 0, 0 as we will have a translate transform

    int verticalShift = proxyStyle->pixelMetric(QStyle::PM_TabBarTabShiftVertical, opt, widget);
    int horizontalShift = proxyStyle->pixelMetric(QStyle::PM_TabBarTabShiftHorizontal, opt, widget);

    int xShift = botton ? 0 : 5;
    int hpadding = proxyStyle->pixelMetric(QStyle::PM_TabBarTabHSpace, opt, widget) / 2 - xShift;
    int vpadding = proxyStyle->pixelMetric(QStyle::PM_TabBarTabVSpace, opt, widget) / 2;

    if (opt->shape == QTabBar::RoundedSouth || opt->shape == QTabBar::TriangularSouth)
        verticalShift = -verticalShift;

    tr.adjust(hpadding, verticalShift - vpadding, horizontalShift - hpadding, vpadding);

    bool selected = opt->state & QStyle::State_Selected;
    if (selected) {
        tr.setTop(tr.top() - verticalShift);
        tr.setRight(tr.right() - horizontalShift);
    }

    // left widget
    if (!opt->leftButtonSize.isEmpty()) {
        tr.setLeft(tr.left() + 4 + (verticalTabs ? opt->leftButtonSize.height() : opt->leftButtonSize.width()));
    }
    // right widget
    if (!opt->rightButtonSize.isEmpty()) {
        tr.setRight(tr.right() - 4 - (verticalTabs ? opt->rightButtonSize.height() : opt->rightButtonSize.width()));
    }

    // icon
    if (!opt->icon.isNull()) {
        QSize iconSize = opt->iconSize;
        if (!iconSize.isValid()) {
            int iconExtent = proxyStyle->pixelMetric(QStyle::PM_SmallIconSize);
            iconSize = QSize(iconExtent, iconExtent);
        }
        QSize tabIconSize =
            opt->icon.actualSize(iconSize, (opt->state & QStyle::State_Enabled) ? QIcon::Normal : QIcon::Disabled,
                                 (opt->state & QStyle::State_Selected) ? QIcon::On : QIcon::Off);

        *iconRect =
            QRect(tr.left(), tr.center().y() - tabIconSize.height() / 2, tabIconSize.width(), tabIconSize.height());

        xShift = botton ? 6 : 0;

        if (!verticalTabs && botton &&
            !((opt->position == QStyleOptionTab::OnlyOneTab || opt->position == QStyleOptionTab::Beginning)))
            xShift = 0;

        if (opt->shape == QTabBar::RoundedEast && opt->position != QStyleOptionTab::OnlyOneTab &&
            opt->position != QStyleOptionTab::Beginning) {
            //            iconRect->translate(xShift-11, 0);
        } else
            iconRect->translate(xShift, 0);

        if (!verticalTabs)
            *iconRect = proxyStyle->visualRect(opt->direction, opt->rect, *iconRect);

        xShift = botton ? /*2 +*/ tabIconSize.width() : 4;

        if (!verticalTabs && botton &&
            !((opt->position == QStyleOptionTab::OnlyOneTab || opt->position == QStyleOptionTab::Beginning)))
            xShift -= 7;

        tr.translate(xShift, 0);
    }

    if (!verticalTabs)
        tr = proxyStyle->visualRect(opt->direction, opt->rect, tr);

    *textRect = tr;
}

void RibbonStylePrivate::tabHoverUpdate(QTabBar *tabBar, QEvent *event)
{
    if (event->type() == QEvent::HoverMove || event->type() == QEvent::HoverEnter) {
        QHoverEvent *he = static_cast<QHoverEvent *>(event);
        int index = tabBar->tabAt(he->pos());
        if (index != -1) {
            QRect rect = tabBar->tabRect(index);
            switch (tabBar->shape()) {
            case QTabBar::RoundedNorth:
            case QTabBar::TriangularNorth:
                rect.adjust(0, 0, 11, 0);
                break;
            case QTabBar::RoundedSouth:
            case QTabBar::TriangularSouth:
                //                    if (tab->position != QStyleOptionTab::Beginning)
                rect.adjust(-11, 0, 0, 0);
                break;
            case QTabBar::RoundedWest:
            case QTabBar::TriangularWest:
                rect.adjust(0, -11, 0, 0);
                break;
            case QTabBar::RoundedEast:
            case QTabBar::TriangularEast:
                rect.adjust(0, -11, 0, 0);
                break;
            default:
                break;
            }

            m_oldHoverRectTab = rect;
            tabBar->update(rect);
        }
    } else if (event->type() == QEvent::HoverLeave) {
        tabBar->update(m_oldHoverRectTab);
        m_oldHoverRectTab = QRect();
    }
}

void RibbonStylePrivate::setMacSmallSize(QWidget *widget)
{
    m_macSmallSizeWidgets.insert(widget, widget->testAttribute(Qt::WA_MacSmallSize));
    widget->setAttribute(Qt::WA_MacSmallSize, true);
}

void RibbonStylePrivate::unsetMacSmallSize(QWidget *widget)
{
    if (m_macSmallSizeWidgets.contains(widget)) {
        bool testMacSmallSize = m_macSmallSizeWidgets.value(widget);
        widget->setAttribute(Qt::WA_MacSmallSize, testMacSmallSize);
        m_macSmallSizeWidgets.remove(widget);
    }
}

RibbonPaintManagerInterface *RibbonStylePrivate::ribbonPaintManager() const
{
    QTC_Q(const RibbonStyle);
    RibbonPaintManagerInterface *ribbonPaintManager = qobject_cast<RibbonPaintManagerInterface *>(q->paintManager());
    return ribbonPaintManager;
}

void RibbonStylePrivate::makePaintManager()
{
    QTC_Q(RibbonStyle)

    setPaintManager(*new RibbonPaintManager(q));
}

/*
\class RibbonStyle
\inmodule ribbon
*/

/*!
Constuctor of RibbonStyle class
*/
RibbonStyle::RibbonStyle() : OfficeStyle(new RibbonStylePrivate)
{
    QTC_D(RibbonStyle);
    d->initialization();
}

/*!
Constructor of RibbonStyle class. The application is usually required to use this constructor only once.
After this, the call QApplcation::setStyle(...) is not required. Parameter \a mainWindow is not used.
*/
RibbonStyle::RibbonStyle(QMainWindow *mainWindow) : OfficeStyle(mainWindow, new RibbonStylePrivate)
{
    QTC_D(RibbonStyle);
    d->initialization();
}

/*!
Destructor of the OfficeStyle.
*/
RibbonStyle::~RibbonStyle() {}

/*!
\property RibbonStyle::activeTabAccented
This property holds whether the painting of active tab with accent color is enabled. Accent color depends on the style
or theme and can't be changed. Implemented for Office 2013 theme only. \inmodule ribbon
*/
bool RibbonStyle::isActiveTabAccented() const
{
    QTC_D(const RibbonStyle);
    return d->m_isActiveTabAccented;
}

void RibbonStyle::setActiveTabAccented(bool accented)
{
    QTC_D(RibbonStyle);
    if (d->m_isActiveTabAccented == accented)
        return;
    d->m_isActiveTabAccented = accented;
    d->refreshMetrics();
}

/*!
Returns font for the given \a widget. In most cases it is equal to QApplication::font().
*/
QFont RibbonStyle::font(const QWidget *widget) const
{
    if (qobject_cast<const RibbonBar *>(widget) || widget == Q_NULL) {
#ifdef Q_OS_WIN
        NONCLIENTMETRICS ncm;
        ncm.cbSize = FIELD_OFFSET(NONCLIENTMETRICS, lfMessageFont) + sizeof(LOGFONT);
        ::SystemParametersInfo(SPI_GETNONCLIENTMETRICS, ncm.cbSize, &ncm, 0);
        return qtc_LOGFONTtoQFont(ncm.lfMenuFont, isDPIAware());
#else
        QMenuBar menu;
        menu.setAttribute(Qt::WA_MacSmallSize);
        QFont fontRegular = QApplication::font(&menu);

        double scale = isDPIAware() ? 1 / DrawHelpers::dpiScaled(1.0) : 1;
        fontRegular.setPointSizeF(fontRegular.pointSizeF() * scale);
        return fontRegular;
#endif
    } else
        return QApplication::font(widget);
}

/*! \reimp */
void RibbonStyle::polish(QApplication *application)
{
    OfficeStyle::polish(application);

    if (application)
        application->installEventFilter(this);

    QTC_D(RibbonStyle);
    d->updateColors();
}

/*! \reimp */
void RibbonStyle::unpolish(QApplication *application)
{
    OfficeStyle::unpolish(application);

    if (application)
        application->removeEventFilter(this);
}

/*! \reimp */
void RibbonStyle::polish(QWidget *widget)
{
    OfficeStyle::polish(widget);

    if (isNativeDialog(widget))
        return;

    QTC_D(RibbonStyle);
    if (qobject_cast<RibbonBar *>(widget)) {
        d->setMacSmallSize(widget);
        QPalette palette = widget->palette();
        palette.setColor(QPalette::Light, d->m_clrMenuBarGrayText);
        palette.setColor(QPalette::ButtonText, d->m_clrRibbonText);
        widget->setPalette(palette);
    }

    if (qobject_cast<QStatusBar *>(widget)) {
        QPalette palette = widget->palette();
        palette.setColor(QPalette::Light, d->m_clrMenuBarGrayText);
        palette.setColor(QPalette::ButtonText, d->m_clrStatusBarText);
        widget->setPalette(palette);
    }

    if (qobject_cast<RibbonGroup *>(widget)) {
        QPalette palette = widget->palette();

        palette.setColor(QPalette::Light, d->m_clrMenuBarGrayText);
        palette.setColor(QPalette::WindowText, d->m_clrRibbonGroupCaptionText);
        palette.setColor(QPalette::ButtonText, d->m_clrGroupClientText);
        palette.setColor(QPalette::Disabled, QPalette::ButtonText, d->m_clrGroupClientGrayText);

        widget->setPalette(palette);
    } else if (qobject_cast<RibbonGroupPopupButton *>(widget)) {
        QPalette palette = widget->palette();
        palette.setColor(QPalette::Light, d->m_clrToolBarGrayText);
        palette.setColor(QPalette::WindowText, d->m_clrToolBarText);
        widget->setPalette(palette);
    } else if ((qobject_cast<RibbonGroup *>(widget->parentWidget()) && qobject_cast<QToolButton *>(widget))) {
        QPalette palette = widget->palette();
        palette.setColor(QPalette::Light, d->m_clrToolBarGrayText);
        palette.setColor(QPalette::WindowText, d->m_clrToolBarText);
        widget->setPalette(palette);
    } else if (qobject_cast<RibbonSystemPopupBar *>(widget->parentWidget()) && qobject_cast<QToolButton *>(widget)) {
        QPalette palette = widget->palette();
        palette.setColor(QPalette::Light, d->m_clrMenuBarGrayText);
        palette.setColor(QPalette::ButtonText, d->m_clrMenuPopupText);
        widget->setPalette(palette);
    } else if (qobject_cast<RibbonSystemButton *>(widget)) {
        QPalette palette = widget->palette();
        palette.setColor(QPalette::ButtonText, d->m_clrFileButtonText);
        widget->setPalette(palette);
    } else if (qobject_cast<OfficePopupMenu *>(widget->parentWidget()) && qobject_cast<QLabel *>(widget)) {
        QPalette palette = widget->palette();
        palette.setColor(QPalette::WindowText, getTextColor(false, false, true, false, false, TypePopup, BarNone));
        widget->setPalette(palette);
    } else if (qobject_cast<QTabBar *>(widget) && qobject_cast<QMdiArea *>(widget->parentWidget())) {
        widget->setAutoFillBackground(true);

        QPalette palette = widget->palette();
        palette.setColor(QPalette::Background, helper().getColor("TabManager", "AccessHeader"));
        widget->setPalette(palette);

        ((QTabBar *)widget)->setExpanding(false);
        widget->installEventFilter(this);
    }
    d->ribbonPaintManager()->setupPalette(widget);
}

/*! \reimp */
void RibbonStyle::unpolish(QWidget *widget)
{
    OfficeStyle::unpolish(widget);
    QTC_D(RibbonStyle);

    if (getParentWidget<RibbonBar>(widget) || qobject_cast<RibbonBar *>(widget))
        d->unsetMacSmallSize(widget);

    if (widget == Q_NULL)
        return;

    if (static_cast<RibbonBar *>((RibbonBar *)widget))
        widget->removeEventFilter(this);

    if (qobject_cast<QTabBar *>(widget) && qobject_cast<QMdiArea *>(widget->parentWidget())) {
        widget->removeEventFilter(this);
        ((QTabBar *)widget)->setExpanding(true);
    }
}

/*! \reimp */
void RibbonStyle::drawPrimitive(PrimitiveElement element, const QStyleOption *option, QPainter *painter,
                                const QWidget *widget) const
{
    switch ((QPrimitiveElement)element) {
    case PE_RibbonPopupBarButton:
        drawPopupSystemButton(option, painter, widget);
        break;
    case PE_RibbonFileButton:
        drawSystemButton(option, painter, widget);
        break;
    case PE_RibbonOptionButton:
        drawOptionButton(option, painter, widget);
        break;
    case PE_RibbonGroupScrollButton:
        drawGroupScrollButton(option, painter, widget);
        break;
    case PE_RibbonQuickAccessButton:
        drawQuickAccessButton(option, painter, widget);
        break;
    case PE_RibbonTab:
        drawTabShape(option, painter, widget);
        break;
    case PE_RibbonContextHeaders:
        drawContextHeaders(option, painter);
        break;
    case PE_RibbonFillRect:
        drawFillRect(option, painter, widget);
        break;
    case PE_RibbonRect:
        drawRect(option, painter, widget);
        break;
    case PE_RibbonKeyTip:
        drawKeyTip(option, painter, widget);
        break;
    case PE_Backstage:
        drawBackstage(option, painter, widget);
        break;
    case PE_RibbonBackstageCloseButton:
        drawRibbonBackstageCloseButton(option, painter, widget);
        break;
    case PE_RibbonSliderButton:
        drawRibbonSliderButton(option, painter, widget);
        break;
    default:
        OfficeStyle::drawPrimitive(element, option, painter, widget);
    }
}

/*! \reimp */
void RibbonStyle::drawControl(ControlElement element, const QStyleOption *opt, QPainter *p, const QWidget *widget) const
{
    switch (element) {
    case CE_RibbonTabShapeLabel:
        drawTabShapeLabel(opt, p, widget);
        break;
    case CE_RibbonBar:
        drawRibbonBar(opt, p, widget);
        break;
    case CE_RibbonTabBar:
        drawRibbonTabBar(opt, p, widget);
        break;
    case CE_RibbonFileButtonLabel:
        drawFileButtonLabel(opt, p, widget);
        break;
    case CE_RadioButtonLabel:
    case CE_CheckBoxLabel: {
        if (const QStyleOptionButton *btn = qstyleoption_cast<const QStyleOptionButton *>(opt)) {
            uint alignment = visualAlignment(btn->direction, Qt::AlignLeft | Qt::AlignVCenter);
            if (!proxy()->styleHint(SH_UnderlineShortcut, btn, widget))
                alignment |= Qt::TextHideMnemonic;
            QPixmap pix;
            QRect textRect = btn->rect;
            if (!btn->icon.isNull()) {
                pix = btn->icon.pixmap(btn->iconSize, btn->state & State_Enabled ? QIcon::Normal : QIcon::Disabled);
                proxy()->drawItemPixmap(p, btn->rect, alignment, pix);
                if (btn->direction == Qt::RightToLeft)
                    textRect.setRight(textRect.right() - btn->iconSize.width() - 4);
                else
                    textRect.setLeft(textRect.left() + btn->iconSize.width() + 4);
            }
            if (!btn->text.isEmpty())
                proxy()->drawItemText(p, textRect, alignment | Qt::TextShowMnemonic, btn->palette,
                                      btn->state & State_Enabled, btn->text, QPalette::ButtonText);
        }
        break;
    }
    case CE_RibbonGroups:
        drawRibbonGroups(opt, p, widget);
        break;
    case CE_Group:
        drawGroup(opt, p, widget);
        break;
    case CE_ReducedGroup:
        drawReducedGroup(opt, p, widget);
        break;
    case CE_PopupSizeGrip:
        drawPopupResizeGripper(opt, p, widget);
        break;
    default:
        OfficeStyle::drawControl(element, opt, p, widget);
        break;
    }
}

/*! \reimp */
void RibbonStyle::drawComplexControl(ComplexControl cc, const QStyleOptionComplex *option, QPainter *painter,
                                     const QWidget *widget) const
{
    OfficeStyle::drawComplexControl(cc, option, painter, widget);
}

/*! \reimp */
int RibbonStyle::pixelMetric(PixelMetric pm, const QStyleOption *option, const QWidget *widget) const
{
    int ret = 0;
    switch (pm) {
    case PM_ButtonIconSize:
    case PM_SmallIconSize: {
        if (widget && (qobject_cast<const RibbonStatusBar *>(widget->parentWidget()) ||
                       qobject_cast<const RibbonStatusBarSwitchGroup *>(widget->parentWidget())))
            ret = int(DrawHelpers::dpiScaled(14.));
        else
            ret = OfficeStyle::pixelMetric(pm, option, widget);
    } break;
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
    case PM_MenuBarPanelWidth:
        ret = 0;
        break;
#endif
    case PM_MenuBarVMargin: {
        ret = OfficeStyle::pixelMetric(pm, option, widget);
        if (!option) {
            if (const RibbonBar *ribbonBar = qobject_cast<const RibbonBar *>(widget))
                ret += ribbonBar->qtc_d()->topBorder() + ribbonBar->titleBarHeight() + 2;
        }
    } break;

    case PM_MenuBarHMargin:
        ret = 2;
        break;

    case PM_TabBarTabHSpace: {
        ret = OfficeStyle::pixelMetric(pm, option, widget);

        if (widget && qobject_cast<QMdiArea *>(widget->parentWidget())) {
            if (const QStyleOptionTab *taOption = qstyleoption_cast<const QStyleOptionTab *>(option)) {
                bool verticalTabs =
                    taOption->shape == QTabBar::RoundedEast || taOption->shape == QTabBar::RoundedWest ||
                    taOption->shape == QTabBar::TriangularEast || taOption->shape == QTabBar::TriangularWest;

                bool botton = taOption->shape == QTabBar::RoundedSouth;
                if (!verticalTabs && botton) {
                    if (taOption->icon.isNull() && (taOption->position == QStyleOptionTab::OnlyOneTab ||
                                                    taOption->position == QStyleOptionTab::Beginning))
                        ret += 11;
                    else if (!taOption->icon.isNull() && !(taOption->position == QStyleOptionTab::OnlyOneTab ||
                                                           taOption->position == QStyleOptionTab::Beginning))
                        ret -= 11;
                }
            }
        }
    } break;
    case PM_RibbonReducedGroupWidth:
        if (const QStyleOptionToolButton *optBut = qstyleoption_cast<const QStyleOptionToolButton *>(option)) {
            QString strFirstRow, strSecondRow;
            CommonStyle::splitString(optBut->text, strFirstRow, strSecondRow);
            bool towRow = !strFirstRow.isEmpty() && !strSecondRow.isEmpty();
            if (strFirstRow.count() < strSecondRow.count())
                strFirstRow = strSecondRow;

#if QT_VERSION < QT_VERSION_CHECK(5, 11, 0)
            ret =
                optBut->fontMetrics.boundingRect(strFirstRow).width() + optBut->fontMetrics.width(QLatin1Char(' ')) * 4;
#else
            ret = optBut->fontMetrics.boundingRect(strFirstRow).width() +
                    optBut->fontMetrics.horizontalAdvance(QLatin1Char(' ')) * 4;
#endif
            if (towRow)
                ret = ret + 16;

            QPixmap soCenter = cached("RibbonGroupButton.png");
            QRect rcSrc = sourceRectImage(soCenter.rect(), 0, 3);
            ret = qMax(ret, rcSrc.width());
        }
        break;
    case PM_RibbonHeightCaptionGroup:
        if (const QStyleOptionGroupBox *optGroup = qstyleoption_cast<const QStyleOptionGroupBox *>(option)) {
            bool titleGroupsVisible = widget ? widget->property(_qtc_TitleGroupsVisible).toBool() : false;
            ret = titleGroupsVisible ? option->fontMetrics.height() + 3 : 0;
            if (titleGroupsVisible) {
                QSize textSize = optGroup->fontMetrics.size(Qt::TextShowMnemonic, optGroup->text);
                if (ret < textSize.height())
                    ret = textSize.height();
            }
            break;
        }
        // fallthrough
    case PM_TitleBarHeight: {
        ret = OfficeStyle::pixelMetric(pm, option, widget);
        break;
    }
    case PM_MenuHMargin: {
        if (qobject_cast<const RibbonSystemPopupBar *>(widget))
            ret = 6;
        else if (qobject_cast<const OfficePopupMenu *>(widget))
            ret = -1;
        else
            ret = OfficeStyle::pixelMetric(pm, option, widget);
        break;
    }
    case PM_MenuVMargin: {
        if (qobject_cast<const RibbonSystemPopupBar *>(widget))
            ret = 19;
        else if (qobject_cast<const RibbonBackstageView *>(widget))
            ret = 7;
        else if (qobject_cast<const OfficePopupMenu *>(widget))
            ret = 0;
        else
            ret = OfficeStyle::pixelMetric(pm, option, widget);
        break;
    }
    case PM_ToolBarIconSize: {
        if (isDPIAware() && getParentWidget<RibbonBar>(widget)) {
            ret = OfficeStyle::pixelMetric(pm, option, widget);
            ret = (double)ret / ((double)(DrawHelpers::getDPIToPercent() / 100.0));
        } else if (qobject_cast<const RibbonSystemPopupBar *>(widget) ||
                   qobject_cast<const RibbonPageSystemPopup *>(widget))
            ret = 32;
        else
            ret = OfficeStyle::pixelMetric(pm, option, widget);
        break;
    }
    case PM_ToolBarFrameWidth: {
        if (qobject_cast<const RibbonQuickAccessBar *>(widget))
            ret = 0;
        else
            ret = OfficeStyle::pixelMetric(pm, option, widget);
        break;
    }
    // Returns the number of pixels to use for the business part of the
    // slider (i.e., the non-tickmark portion). The remaining space is shared
    // equally between the tickmark regions.
    case PM_SliderControlThickness: {
        if (const QStyleOptionSlider *sl = qstyleoption_cast<const QStyleOptionSlider *>(option)) {
            int space = (sl->orientation == Qt::Horizontal) ? sl->rect.height() : sl->rect.width();
            int ticks = sl->tickPosition;
            int n = 0;
            if (ticks & QSlider::TicksAbove)
                ++n;
            if (ticks & QSlider::TicksBelow)
                ++n;
            if (!n) {
                ret = space;
                break;
            }

            int thick = 6;   // Magic constant to get 5 + 16 + 5
            if (ticks != QSlider::TicksBothSides && ticks != QSlider::NoTicks)
                thick += proxy()->pixelMetric(PM_SliderLength, sl, widget) / 4;

            space -= thick;
            if (space > 0)
                thick += (space * 2) / (n + 2);
            ret = thick;
        } else {
            ret = 0;
        }
    } break;
    case PM_ToolBarSeparatorExtent: {
        if (qobject_cast<const RibbonToolBarControl *>(widget))
            ret = int(DrawHelpers::dpiScaled(3.));
        else if (qobject_cast<const RibbonStatusBar *>(widget))
            ret = int(DrawHelpers::dpiScaled(3.));
        else
            ret = OfficeStyle::pixelMetric(pm, option, widget);
    } break;

    default:
        ret = OfficeStyle::pixelMetric(pm, option, widget);
        break;
    }
    return ret;
}

/*! \reimp */
int RibbonStyle::styleHint(StyleHint hint, const QStyleOption *opt, const QWidget *widget,
                           QStyleHintReturn *returnData) const
{
    QTC_D(const RibbonStyle);
    int ret = 0;
    if (hint == QStyle::SH_ToolButtonStyle) {
        if (const RibbonSystemButton *sysButton = qobject_cast<const RibbonSystemButton *>(widget))
            ret = sysButton->toolButtonStyle();
    } else if (hint == (StyleHint)SH_FlatFrame) {
        ret = d->m_flatFrame;
    } else if (hint == SH_UnderlineShortcut) {
        const RibbonBar *ribbonBar = ::getParentWidget<const RibbonBar>(widget);
        if (ribbonBar && !qobject_cast<const QMenu *>(widget))
            ret = 0;
        else if (ribbonBar && ribbonBar->qtc_d()->m_keyTips.size() > 0)
            ret = 1;
        else
            ret = OfficeStyle::styleHint(hint, opt, widget, returnData);
    } else if (hint == (StyleHint)SH_RibbonBackstageHideTabs) {
        return d->ribbonPaintManager()->isRibbonBackstageHideTabs();
    } else if (hint == (StyleHint)SH_RibbonItemUpperCase) {
        return d->ribbonPaintManager()->isTopLevelMenuItemUpperCase(widget);
    } else
        ret = OfficeStyle::styleHint(hint, opt, widget, returnData);
    return ret;
}

/*! \internal */
QPixmap RibbonStyle::standardPixmap(StandardPixmap standardPixmap, const QStyleOption *opt, const QWidget *widget) const
{
    return OfficeStyle::standardPixmap(standardPixmap, opt, widget);
}

/*! \reimp */
QSize RibbonStyle::sizeFromContents(ContentsType ct, const QStyleOption *opt, const QSize &csz,
                                    const QWidget *widget) const
{
    QSize sz(csz);
    switch (ct) {
    case CT_ToolButton: {
        if (qobject_cast<const RibbonSystemButton *>(widget)) {
            sz = OfficeStyle::sizeFromContents(ct, opt, csz, widget);
        } else if (widget && (qobject_cast<const RibbonStatusBar *>(widget->parentWidget()) ||
                              qobject_cast<const RibbonStatusBarSwitchGroup *>(widget->parentWidget()))) {
            if (const QStyleOptionToolButton *optToolButton = qstyleoption_cast<const QStyleOptionToolButton *>(opt)) {
                if (!optToolButton->icon.isNull())
                    sz += QSize(5, 4);
                else
                    sz.setHeight(sz.height() + 5);
            }
        } else
            sz = OfficeStyle::sizeFromContents(ct, opt, csz, widget);
    } break;
    case CT_MenuItem: {
        const QMenu *menu = qobject_cast<const RibbonSystemPopupBar *>(widget);
        if (!menu) {
            menu = qobject_cast<const RibbonPageSystemPopup *>(widget);
            if (menu) {
                if (const QStyleOptionMenuItem *mi = qstyleoption_cast<const QStyleOptionMenuItem *>(opt)) {
                    if (mi->text.count(QString(_qtc_PopupLable)) > 0) {
                        sz = OfficeStyle::sizeFromContents(ct, opt, csz, widget);
                        sz.setWidth(1);
                        break;
                    }
                }
            }
        }

        if (menu) {
            if (const QStyleOptionMenuItem *mi = qstyleoption_cast<const QStyleOptionMenuItem *>(opt)) {
                int w = sz.width();
                sz = OfficeStyle::sizeFromContents(ct, opt, csz, widget);

                if (mi->menuItemType == QStyleOptionMenuItem::Separator) {
                    sz = QSize(10, windowsSepHeight);
                } else if (mi->icon.isNull()) {
                    sz.setHeight(sz.height() - 2);

                    int maxheight = 0;
                    QList<QAction *> list = menu->actions();
                    for (int j = 0; j < list.count(); j++) {
                        QAction *action = list.at(j);
                        if (!action->icon().isNull()) {
                            int iconExtent = pixelMetric(PM_LargeIconSize, opt, widget);
                            maxheight =
                                qMax(sz.height(), action->icon().actualSize(QSize(iconExtent, iconExtent)).height() +
                                                      2 * windowsItemFrame);
                        }
                    }

                    if (maxheight > 0)
                        sz.setHeight(maxheight);

                    w -= 6;
                }

                if (mi->menuItemType != QStyleOptionMenuItem::Separator && !mi->icon.isNull()) {
                    int iconExtent = pixelMetric(PM_LargeIconSize, opt, widget);
                    sz.setHeight(qMax(sz.height(), mi->icon.actualSize(QSize(iconExtent, iconExtent)).height() +
                                                       2 * windowsItemFrame));
                }
                int maxpmw = mi->maxIconWidth;
                int tabSpacing = use2000style ? 20 : windowsTabSpacing;
                if (mi->text.contains(QLatin1Char('\t')))
                    w += tabSpacing;
                else if (mi->menuItemType == QStyleOptionMenuItem::SubMenu)
                    w += 2 * windowsArrowHMargin;
                else if (mi->menuItemType == QStyleOptionMenuItem::DefaultItem) {
                    // adjust the font and add the difference in size.
                    // it would be better if the font could be adjusted in the initStyleOption qmenu func!!
                    QFontMetrics fm(mi->font);
                    QFont fontBold = mi->font;
                    fontBold.setBold(true);
                    QFontMetrics fmBold(fontBold);
#if QT_VERSION < QT_VERSION_CHECK(5, 11, 0)
                    w += fmBold.width(mi->text) - fm.width(mi->text);
#else
                    w += fmBold.horizontalAdvance(mi->text) - fm.horizontalAdvance(mi->text);
#endif
                }

                int checkcol = qMax(maxpmw, windowsCheckMarkWidth);   // Windows always shows a check column
                w += checkcol;
                w += windowsRightBorder + 12;   // + 10;
                sz.setWidth(w);

                if (mi->menuItemType != QStyleOptionMenuItem::Separator &&
                    qobject_cast<const RibbonPageSystemPopup *>(widget)) {
                    QFontMetrics fm(mi->font);
                    sz.setHeight(qMax(sz.height(), fm.height() * 3 + fm.height() / 2 + windowsItemVMargin * 2));
                }
            }
        } else if (qobject_cast<const OfficePopupMenu *>(widget)) {
            if (const QStyleOptionMenuItem *mi = qstyleoption_cast<const QStyleOptionMenuItem *>(opt)) {
                sz = OfficeStyle::sizeFromContents(ct, opt, csz, widget);
                if (mi->menuItemType == QStyleOptionMenuItem::Separator)
                    sz.setHeight(csz.height());
            }
        } else
            sz = OfficeStyle::sizeFromContents(ct, opt, csz, widget);
    } break;
    case CT_RibbonSliderButton: {
    } break;
    case CT_MenuBar:
        if (const RibbonBar *ribbonBar = qobject_cast<const RibbonBar *>(widget)) {
            if (!ribbonBar->isVisible()) {
                sz = OfficeStyle::sizeFromContents(ct, opt, csz, widget);
                sz.setHeight(0);
            }
        }
        break;
    default:
        sz = OfficeStyle::sizeFromContents(ct, opt, csz, widget);
        break;
    }

    return sz;
}

/*! \reimp */
QRect RibbonStyle::subControlRect(ComplexControl cc, const QStyleOptionComplex *opt, SubControl sc,
                                  const QWidget *widget) const
{
    QRect ret;
    switch (cc) {
    case CC_ToolButton:
        if (const QStyleOptionToolButton *tb = qstyleoption_cast<const QStyleOptionToolButton *>(opt)) {
            int mbi = proxy()->pixelMetric(PM_MenuButtonIndicator, tb, widget);
            ret = tb->rect;
            switch (sc) {
            case SC_ToolButton:
                if ((tb->features & (QStyleOptionToolButton::MenuButtonPopup | QStyleOptionToolButton::PopupDelay)) ==
                    QStyleOptionToolButton::MenuButtonPopup) {
                    if (tb->toolButtonStyle == Qt::ToolButtonTextUnderIcon) {
                        QRect popupr = subControlRect(cc, opt, QStyle::SC_ToolButtonMenu, widget);
                        ret.adjust(0, 0, 0, -popupr.height());
                    } else
                        ret.adjust(0, 0, -mbi, 0);
                    break;
                }
                // fallthrough
            case SC_ToolButtonMenu:
                if ((tb->features & (QStyleOptionToolButton::MenuButtonPopup | QStyleOptionToolButton::PopupDelay)) ==
                    QStyleOptionToolButton::MenuButtonPopup) {
                    if (!qobject_cast<const RibbonGroupPopupButton *>(widget)) {
                        if (tb->toolButtonStyle == Qt::ToolButtonTextUnderIcon) {
                            QString str(tb->text);
                            QString strFirstRow, strSecondRow;
                            splitString(str, strFirstRow, strSecondRow);

                            QSize textSize;
                            if (!strFirstRow.isEmpty()) {
                                textSize = opt->fontMetrics.size(Qt::TextShowMnemonic, strFirstRow);
#if QT_VERSION < QT_VERSION_CHECK(5, 11, 0)
                                textSize.setWidth(textSize.width() + opt->fontMetrics.width(QLatin1Char(' ')) * 2);
#else
                                textSize.setWidth(textSize.width() +
                                                  opt->fontMetrics.horizontalAdvance(QLatin1Char(' ')) * 2);
#endif
                            }
                            if (!strSecondRow.isEmpty()) {
                                QSize textSize1 = opt->fontMetrics.size(Qt::TextShowMnemonic, strSecondRow);
#if QT_VERSION < QT_VERSION_CHECK(5, 11, 0)
                                textSize1.setWidth(textSize1.width() + opt->fontMetrics.width(QLatin1Char(' ')) * 2);
#else
                                textSize1.setWidth(textSize1.width() +
                                                   opt->fontMetrics.horizontalAdvance(QLatin1Char(' ')) * 2);
#endif
                                textSize.setWidth(qMax(textSize.width(), textSize1.width()));
                            }
                            ret.adjust(0, tb->iconSize.height() + 8, 0, 0);
                            ret.setWidth(qMax(textSize.width(), opt->rect.width()));
                        } else
                            ret.adjust(ret.width() - mbi, 0, 0, 0);
                    }
                    break;
                }
            default:
                break;
            }
            ret = visualRect(tb->direction, tb->rect, ret);
        }
        break;
    default:
        ret = OfficeStyle::subControlRect(cc, opt, sc, widget);
        break;
    }
    return ret;
}

/*! \reimp */
QRect RibbonStyle::subElementRect(SubElement sr, const QStyleOption *opt, const QWidget *widget) const
{
    QRect r;
    switch (sr) {
    case SE_TabBarTabText:
        if (const QStyleOptionTab *tab = qstyleoption_cast<const QStyleOptionTab *>(opt)) {
            if (qobject_cast<QMdiArea *>(widget->parentWidget())) {
                QTC_D(const RibbonStyle);
#if (QT_VERSION < QT_VERSION_CHECK(5, 7, 0))
                QStyleOptionTabV3 tabV3(*tab);
#else
                QStyleOptionTab tabV3(*tab);
#endif
                QRect dummyIconRect;
                d->tabLayout(&tabV3, widget, &r, &dummyIconRect);

                bool verticalTabs = tab->shape == QTabBar::RoundedEast || tab->shape == QTabBar::RoundedWest ||
                                    tab->shape == QTabBar::TriangularEast || tab->shape == QTabBar::TriangularWest;

                bool botton = tab->shape == QTabBar::RoundedSouth;
                if (!verticalTabs && botton && tab->icon.isNull() &&
                    (tab->position == QStyleOptionTab::OnlyOneTab || tab->position == QStyleOptionTab::Beginning))
                    r.translate(6, 0);
            } else
                r = OfficeStyle::subElementRect(sr, opt, widget);
        }
        break;
    case SE_TabBarTabLeftButton:
    case SE_TabBarTabRightButton: {
        r = OfficeStyle::subElementRect(sr, opt, widget);

#if (QT_VERSION < QT_VERSION_CHECK(5, 7, 0))
        if (const QStyleOptionTabV3 *tab = qstyleoption_cast<const QStyleOptionTabV3 *>(opt))
#else
        if (const QStyleOptionTab *tab = qstyleoption_cast<const QStyleOptionTab *>(opt))
#endif
        {
            QRect rc = r;
            if (sr != SE_TabBarTabLeftButton) {
                if (tab->shape == QTabBar::RoundedSouth &&
                    (tab->position == QStyleOptionTab::OnlyOneTab || tab->position == QStyleOptionTab::Beginning))
                    rc.setLeft(rc.left() + rc.width() / 2);
                else if (tab->shape == QTabBar::RoundedWest &&
                         (tab->position == QStyleOptionTab::OnlyOneTab || tab->position == QStyleOptionTab::Beginning))
                    rc.setTop(rc.top() + rc.width() / 2);
                else if (tab->shape == QTabBar::RoundedEast)
                    rc.setTop(rc.top() /* - rc.width()/2*/);
            }
            r = rc;
        }
    } break;
    default:
        r = OfficeStyle::subElementRect(sr, opt, widget);
        break;
    }
    return r;
}

// for QForm
/*! \internal */
bool RibbonStyle::drawFrame(const QStyleOption *opt, QPainter *p, const QWidget *w) const
{
    QTC_D(const RibbonStyle);
    return d->ribbonPaintManager()->drawFrame(opt, p, w);
}

// for QForm
/*! \internal */
bool RibbonStyle::drawShapedFrame(const QStyleOption *opt, QPainter *p, const QWidget *w) const
{
    QTC_D(const RibbonStyle)
    bool ret = false;
    if (qobject_cast<const QMdiArea *>(w))
        ret = d->ribbonPaintManager()->drawShapedFrame(opt, p, w);
    else if (qobject_cast<const RibbonBackstageSeparator *>(w))
        ret = d->ribbonPaintManager()->drawRibbonBackstageSeparator(opt, p, w);
    return ret;
}

// for stausBar
/*! \internal */
bool RibbonStyle::drawPanelStatusBar(const QStyleOption *opt, QPainter *p, const QWidget *w) const
{
    QTC_D(const RibbonStyle);
    return d->ribbonPaintManager()->drawPanelStatusBar(opt, p, w);
}

/*! \internal */
void RibbonStyle::drawRibbonBar(const QStyleOption *opt, QPainter *p, const QWidget *w) const
{
    QTC_D(const RibbonStyle);
    d->ribbonPaintManager()->drawRibbonBar(opt, p, w);
}

/*! \internal */
void RibbonStyle::drawRibbonTabBar(const QStyleOption *opt, QPainter *p, const QWidget *w) const
{
    QTC_D(const RibbonStyle);
    d->ribbonPaintManager()->drawRibbonTabBar(opt, p, w);
}

/*! \internal */
void RibbonStyle::drawRibbonGroups(const QStyleOption *option, QPainter *p, const QWidget *widget) const
{
    QTC_D(const RibbonStyle);
    d->ribbonPaintManager()->drawRibbonGroups(option, p, widget);
}

/*! \internal */
void RibbonStyle::drawGroup(const QStyleOption *opt, QPainter *p, const QWidget *widget) const
{
    if (paintAnimation(tp_ControlElement, CE_Group, opt, p, widget, 300))
        return;

    QTC_D(const RibbonStyle);
    d->ribbonPaintManager()->drawGroup(opt, p, widget);
}

/*! \internal */
void RibbonStyle::drawReducedGroup(const QStyleOption *opt, QPainter *p, const QWidget *widget) const
{
    // if (paintAnimation(tp_ControlElement, CE_ReducedGroup, opt, p, widget, 300))
    //     return;
    QTC_D(const RibbonStyle);
    d->ribbonPaintManager()->drawReducedGroup(opt, p, widget);
}

// for SizeGrip
/*! \internal */
bool RibbonStyle::drawSizeGrip(const QStyleOption *opt, QPainter *p, const QWidget *w) const
{
    QTC_D(const RibbonStyle);
    return d->ribbonPaintManager()->drawSizeGrip(opt, p, w);
}

/*! \internal */
void RibbonStyle::drawContextHeaders(const QStyleOption *opt, QPainter *p) const
{
    QTC_D(const RibbonStyle);
    return d->ribbonPaintManager()->drawContextHeaders(opt, p);
}

/*! \internal */
bool RibbonStyle::drawIndicatorToolBarSeparator(const QStyleOption *opt, QPainter *p, const QWidget *widget) const
{
    QTC_D(const RibbonStyle);
    if (d->ribbonPaintManager()->drawIndicatorToolBarSeparator(opt, p, widget))
        return true;
    return OfficeStyle::drawIndicatorToolBarSeparator(opt, p, widget);
}

// for TitleBar
/*! \internal */
bool RibbonStyle::drawTitleBar(const QStyleOptionComplex *opt, QPainter *p, const QWidget *w) const
{
    QTC_D(const RibbonStyle)
    if (!qobject_cast<const RibbonBar *>(w))
        return OfficeStyle::drawTitleBar(opt, p, w);

    if (const StyleOptionTitleBar *optTitleBar = qstyleoption_cast<const StyleOptionTitleBar *>(opt)) {
        if (optTitleBar->frameHelper) {
            QRect rcCaption = optTitleBar->rect;
            bool dwmEnabled = optTitleBar->frameHelper->isDwmEnabled();
            bool active = optTitleBar->state & State_Active;

            QRect rcText = optTitleBar->rcTitleText;

            if (dwmEnabled) {
                optTitleBar->frameHelper->fillSolidRect(p, rcCaption, optTitleBar->airRegion, QColor(0, 0, 0));
                StyleOptionTitleBar optTitle = *optTitleBar;
                optTitle.rcTitleText = rcText;

                optTitleBar->frameHelper->drawTitleBar(p, optTitle);
            }

            if ((!optTitleBar->isBackstageVisible) &&
                optTitleBar->listContextHeaders && optTitleBar->listContextHeaders->count() > 0)
                proxy()->drawPrimitive((QStyle::PrimitiveElement)RibbonStyle::PE_RibbonContextHeaders, opt, p, w);

            if (!dwmEnabled) {
                p->save();
                p->setPen(!active ? d->m_clrFrameCaptionTextInActive : d->m_clrFrameCaptionTextActive);
                p->setFont(QApplication::font("QApplication"));
                QString text = optTitleBar->frameHelper ? optTitleBar->frameHelper->getWindowText() : QString();
                text = p->fontMetrics().elidedText(text.isEmpty() ? optTitleBar->text : text, Qt::ElideRight,
                                                   rcText.width());
                Qt::Alignment fl = Qt::AlignCenter;   // optTitleBar->quickAccessVisible ? Qt::AlignCenter :
                                                      // Qt::AlignLeft | Qt::AlignVCenter;
                p->drawText(rcText, fl | Qt::TextSingleLine, text);
                p->restore();
            }

            if (optTitleBar->drawIcon && optTitleBar->frameHelper) {
                if (!dwmEnabled && optTitleBar->subControls & SC_TitleBarSysMenu &&
                    optTitleBar->titleBarFlags & Qt::WindowSystemMenuHint) {
                    QStyleOption tool(0);
                    tool.palette = optTitleBar->palette;

                    QRect ir = proxy()->subControlRect(CC_TitleBar, optTitleBar, SC_TitleBarSysMenu, w);

                    if (!optTitleBar->icon.isNull()) {
#ifdef Q_OS_WIN
                        if (HICON hIcon = optTitleBar->frameHelper->windowIcon(optTitleBar->icon, optTitleBar->rect)) {
    #if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
                            QPixmap px = qt_pixmapFromWinHICON(hIcon);
                            if (!px.isNull()) {
                                QSize iconSize = px.size();
                                int controlHeight = optTitleBar->rect.height();
                                int hPad = (controlHeight - iconSize.height()) / 2;
                                int vPad = (controlHeight - iconSize.width()) / 2;
                                int br = optTitleBar->frameHelper->frameBorder() / 2;
                                QRect r = QRect(hPad - br, vPad - br, iconSize.width(), iconSize.height());
                                r = visualRect(optTitleBar->direction, optTitleBar->rect, r);
                                p->drawPixmap(r.left(), r.top(), px);
                            }
    #else
                            ir.translate(optTitleBar->frameHelper->frameBorder(),
                                         optTitleBar->frameHelper->frameBorder());
                            ::DrawIconEx(p->paintEngine()->getDC(), ir.left(), ir.top(), hIcon, 0, 0, 0, Q_NULL,
                                         DI_NORMAL | DI_COMPAT);
    #endif
                        }
#else
                        ir.translate(4, 0);
                        optTitleBar->icon.paint(p, ir);
#endif   // Q_OS_WIN
                    } else {
                        int iconSize = proxy()->pixelMetric(PM_SmallIconSize, optTitleBar, w);
                        QPixmap pm = standardIcon(SP_TitleBarMenuButton, &tool, w).pixmap(iconSize, iconSize);
                        tool.rect = ir;
                        p->save();
                        proxy()->drawItemPixmap(p, ir, Qt::AlignCenter, pm);
                        p->restore();
                    }
                }
            }
            return true;
        } else {
            if (optTitleBar->listContextHeaders && optTitleBar->listContextHeaders->count() > 0)
                proxy()->drawPrimitive((QStyle::PrimitiveElement)RibbonStyle::PE_RibbonContextHeaders, opt, p, w);
            return true;
        }
    }
    return false;
}

/*! \internal */
bool RibbonStyle::drawFrameMenu(const QStyleOption *opt, QPainter *p, const QWidget *w) const
{
    QTC_D(const RibbonStyle);
    return d->ribbonPaintManager()->drawFrameMenu(opt, p, w);
}

/*! \internal */
bool RibbonStyle::drawSlider(const QStyleOptionComplex *opt, QPainter *p, const QWidget *w) const
{
    QTC_D(const RibbonStyle);
    return d->ribbonPaintManager()->drawSlider(opt, p, w);
}

/*! \internal */
void RibbonStyle::drawSystemButton(const QStyleOption *option, QPainter *p, const QWidget *widget) const
{
    if (paintAnimation(tp_PrimitiveElement, PE_RibbonFileButton, option, p, widget))
        return;

    QTC_D(const RibbonStyle);
    d->ribbonPaintManager()->drawSystemButton(option, p, widget);
}

/*! \internal */
void RibbonStyle::drawOptionButton(const QStyleOption *option, QPainter *p, const QWidget *widget) const
{
    QTC_D(const RibbonStyle);
    d->ribbonPaintManager()->drawOptionButton(option, p, widget);
}

/*! \internal */
void RibbonStyle::drawGroupScrollButton(const QStyleOption *option, QPainter *p, const QWidget *widget) const
{
    QTC_D(const RibbonStyle);
    d->ribbonPaintManager()->drawGroupScrollButton(option, p, widget);
}

/*! \internal */
void RibbonStyle::drawFileButtonLabel(const QStyleOption *option, QPainter *p, const QWidget *widget) const
{
    QTC_D(const RibbonStyle);
    if (const QStyleOptionToolButton *toolbutton = qstyleoption_cast<const QStyleOptionToolButton *>(option)) {
        int alignment = Qt::TextShowMnemonic;
        if (!proxy()->styleHint(SH_UnderlineShortcut, toolbutton, widget))
            alignment |= Qt::TextHideMnemonic;

        alignment |= Qt::AlignCenter;
        alignment |= Qt::TextSingleLine;

        QString text = toolbutton->text;
        if (d->ribbonPaintManager()->isTopLevelMenuItemUpperCase(widget))
            text = text.toUpper();

        proxy()->drawItemText(p, toolbutton->rect, alignment, toolbutton->palette, toolbutton->state & State_Enabled,
                              text, QPalette::ButtonText);
    }
}

/*! \internal */
void RibbonStyle::drawPopupSystemButton(const QStyleOption *option, QPainter *p, const QWidget *w) const
{
    Q_UNUSED(option);
    Q_UNUSED(p);
    Q_UNUSED(w);
}

/*! \internal */
void RibbonStyle::drawQuickAccessButton(const QStyleOption *opt, QPainter *p, const QWidget *w) const
{
    QTC_D(const RibbonStyle);
    d->ribbonPaintManager()->drawQuickAccessButton(opt, p, w);
}

/*! \internal */
void RibbonStyle::drawPopupResizeGripper(const QStyleOption *opt, QPainter *p, const QWidget *w) const
{
    QTC_D(const RibbonStyle);
    d->ribbonPaintManager()->drawPopupResizeGripper(opt, p, w);
}

/*! \internal */
bool RibbonStyle::drawMenuItem(const QStyleOption *opt, QPainter *p, const QWidget *widget) const
{
    QTC_D(const RibbonStyle)
    if (qobject_cast<const RibbonPageSystemPopup *>(widget)) {
        if (const QStyleOptionMenuItem *menuitem = qstyleoption_cast<const QStyleOptionMenuItem *>(opt)) {
            if (menuitem->text.count(QString(_qtc_PopupLable)) > 0) {
                p->fillRect(menuitem->rect.adjusted(-1, 0, 2, 0), d->m_clrControlGalleryLabel);
                p->fillRect(menuitem->rect.left(), menuitem->rect.bottom() - 1, menuitem->rect.width(), 1,
                            QColor(197, 197, 197));

                QFont fontSave = p->font();

                QFont font = fontSave;
                font.setBold(true);
                p->setFont(font);

                QRect rcText(menuitem->rect);
                rcText.adjust(7, 0, 0, 0);

                QPalette pal = menuitem->palette;
                pal.setColor(QPalette::WindowText, d->m_clrGroupClientText);

                int flags = 0;
                flags |= Qt::TextHideMnemonic | Qt::AlignVCenter | Qt::TextSingleLine;

                QString text = menuitem->text;
                text = p->fontMetrics().elidedText(text.remove(QString(_qtc_PopupLable)), Qt::ElideRight,
                                                   rcText.adjusted(2, 0, -2, 0).width());

                // draw text
                proxy()->drawItemText(p, rcText, flags, pal, opt->state & State_Enabled, text, QPalette::WindowText);
                p->setFont(fontSave);

                return true;
            } else {
                int x, y, w, h;
                menuitem->rect.getRect(&x, &y, &w, &h);
                int tab = menuitem->tabWidth;
                bool dis = !(menuitem->state & State_Enabled);
                bool checked = menuitem->checkType != QStyleOptionMenuItem::NotCheckable ? menuitem->checked : false;
                bool act = menuitem->state & State_Selected;

                // windows always has a check column, regardless whether we have an icon or not
                const int nIconSize = pixelMetric(PM_ToolBarIconSize, opt, widget);
                int checkcol = nIconSize;

                if (act)
                    drawRectangle(p, menuitem->rect, true /*selected*/, false /*pressed*/, !dis /*enabled*/,
                                  false /*checked*/, false /*popuped*/, TypePopup, BarPopup);

                if (menuitem->menuItemType == QStyleOptionMenuItem::Separator) {
                    int yoff = (y - 1 + h / 2);
                    int xIcon = nIconSize;
                    if (qobject_cast<const OfficePopupMenu *>(widget))
                        xIcon = 0;
                    p->setPen(menuitem->palette.dark().color());
                    p->drawLine(x + 2 + /*menuitem->maxIconWidth*/ xIcon, yoff, x + w - 4, yoff);
                    p->setPen(menuitem->palette.light().color());
                    p->drawLine(x + 2 + /*menuitem->maxIconWidth*/ xIcon, yoff + 1, x + w - 4, yoff + 1);
                    return true;
                }

                QRect vCheckRect =
                    visualRect(opt->direction, menuitem->rect,
                               QRect(menuitem->rect.x(), menuitem->rect.y(), checkcol, menuitem->rect.height()));

                // On Windows Style, if we have a checkable item and an icon we
                // draw the icon recessed to indicate an item is checked. If we
                // have no icon, we draw a checkmark instead.
                if (!menuitem->icon.isNull()) {
                    QIcon::Mode mode = dis ? QIcon::Disabled : QIcon::Normal;

                    int iconSize = pixelMetric(PM_LargeIconSize, opt, widget);
                    if (iconSize > qMin(opt->rect.height(), opt->rect.width()))
                        iconSize = pixelMetric(PM_SmallIconSize, opt, widget);

                    if (act && !dis)
                        mode = QIcon::Active;
                    QPixmap pixmap;
                    if (checked)
                        // pixmap = menuitem->icon.pixmap(pixelMetric(PM_LargeIconSize, opt, widget), mode, QIcon::On);
                        pixmap = menuitem->icon.pixmap(iconSize, mode, QIcon::On);
                    else
                        // pixmap = menuitem->icon.pixmap(pixelMetric(PM_LargeIconSize, opt, widget), mode);
                        pixmap = menuitem->icon.pixmap(iconSize, mode);

                    int pixw = pixmap.width();
                    int pixh = pixmap.height();

                    QRect pmr(0, 0, pixw, pixh);
                    pmr.moveCenter(vCheckRect.center());
                    p->setPen(menuitem->palette.text().color());

                    if (checked) {
                        QRect vIconRect = visualRect(opt->direction, opt->rect, pmr);
                        QRect rcChecked = menuitem->rect;
                        rcChecked.setLeft(vIconRect.left());
                        rcChecked.setWidth(vIconRect.width());
                        drawRectangle(p, rcChecked.adjusted(-2, 1, 2, -1), false /*selected*/, true /*pressed*/,
                                      !dis /*enabled*/, true /*checked*/, false /*popuped*/, TypePopup, BarPopup);
                    }

                    p->drawPixmap(pmr.topLeft() + QPoint(windowsItemHMargin, 0), pixmap);
                }

                int xm = windowsItemFrame + checkcol + (windowsItemHMargin * 2);
                int xpos = menuitem->rect.x() + xm;

                // draw text
                if (!menuitem->text.isEmpty()) {
                    int height = menuitem->fontMetrics.height();
                    QRect textRect(xpos, y + windowsItemVMargin + 2, w - xm - windowsRightBorder - tab + 1, height);
                    QRect vTextRect = visualRect(opt->direction, menuitem->rect, textRect);

                    p->save();
                    QStringList split = menuitem->text.split(QString("\n"));
                    QString s = split.count() > 0 ? split.at(0) : menuitem->text;

                    QFont font = menuitem->font;
                    font.setBold(true);
                    p->setFont(font);

                    int t = s.indexOf(QLatin1Char('\t'));
                    int text_flags = Qt::AlignVCenter | Qt::TextShowMnemonic | Qt::TextDontClip | Qt::TextSingleLine;

                    if (!styleHint(SH_UnderlineShortcut, menuitem, widget))
                        text_flags |= Qt::TextHideMnemonic;
                    text_flags |= Qt::AlignLeft;

                    // draw hotkeys
                    if (t >= 0) {
                        QRect vShortcutRect =
                            visualRect(opt->direction, menuitem->rect,
                                       QRect(textRect.topRight(), QPoint(menuitem->rect.right(), textRect.bottom())));

                        p->setPen(opt->state & State_Enabled ? d->m_clrMenuPopupText : d->m_clrMenuBarGrayText);
                        p->drawText(vShortcutRect, text_flags, s.mid(t + 1));
                        s = s.left(t);
                    }

                    p->setPen(opt->state & State_Enabled ? d->m_clrMenuPopupText : d->m_clrMenuBarGrayText);

                    p->drawText(vTextRect, text_flags, s.left(t));
                    p->restore();
                    if (split.count() > 1) {
                        text_flags |= Qt::TextWordWrap;
                        QRect textTipRect(xpos, y + vTextRect.height(), w - xm - 1, opt->rect.height());
                        QRect rc = menuitem->fontMetrics.boundingRect(textTipRect, text_flags, split.at(1));
                        QRect vTextTipRect = visualRect(opt->direction, menuitem->rect, rc);
                        vTextTipRect.setTop(y + vTextRect.height());
                        vTextTipRect.setWidth(textTipRect.width());
                        p->setPen(opt->state & State_Enabled ? d->m_clrMenuPopupText : d->m_clrMenuBarGrayText);
                        p->drawText(vTextTipRect, text_flags, split.at(1));
                    }
                }

                return true;
            }
        }
    }

    if (qobject_cast<const RibbonBackstageView *>(widget)) {
        d->ribbonPaintManager()->drawRibbonBackstageMenu(opt, p, widget);
        return true;
    } else
        return OfficeStyle::drawMenuItem(opt, p, widget);
}

/*! \internal */
void RibbonStyle::drawTabShape(const QStyleOption *opt, QPainter *p, const QWidget *widget) const
{
    if (paintAnimation(tp_PrimitiveElement, PE_RibbonTab, opt, p, widget))
        return;

    QTC_D(const RibbonStyle);
    d->ribbonPaintManager()->drawTabShape(opt, p, widget);
}

/*! \internal */
void RibbonStyle::drawTabShapeLabel(const QStyleOption *opt, QPainter *p, const QWidget *w) const
{
    QTC_D(const RibbonStyle);
    d->ribbonPaintManager()->drawTabShapeLabel(opt, p, w);
}

/*! \internal */
bool RibbonStyle::drawTabBarTabShape(const QStyleOption *opt, QPainter *p, const QWidget *widget) const
{
    QTC_D(const RibbonStyle);
    return d->ribbonPaintManager()->drawTabBarTabShape(opt, p, widget);
}

/*! \internal */
bool RibbonStyle::drawTabBarTabLabel(const QStyleOption *opt, QPainter *p, const QWidget *widget) const
{
    QTC_D(const RibbonStyle);

    if (!qobject_cast<QMdiArea *>(widget->parentWidget()))
        return OfficeStyle::drawTabBarTabLabel(opt, p, widget);

    if (const QStyleOptionTab *tab = qstyleoption_cast<const QStyleOptionTab *>(opt)) {
#if (QT_VERSION < QT_VERSION_CHECK(5, 7, 0))
        QStyleOptionTabV3 tabV2(*tab);
#else
        QStyleOptionTab tabV2(*tab);
#endif
        QRect tr = tabV2.rect;
        bool verticalTabs = tabV2.shape == QTabBar::RoundedEast || tabV2.shape == QTabBar::RoundedWest ||
                            tabV2.shape == QTabBar::TriangularEast || tabV2.shape == QTabBar::TriangularWest;

        int alignment = Qt::AlignCenter | Qt::TextShowMnemonic;
        if (!proxy()->styleHint(SH_UnderlineShortcut, opt, widget))
            alignment |= Qt::TextHideMnemonic;

        if (verticalTabs) {
            p->save();
            int newX, newY, newRot;
            if (tabV2.shape == QTabBar::RoundedEast || tabV2.shape == QTabBar::TriangularEast) {
                newX = tr.width() + tr.x();
                newY = tr.y();
                newRot = 90;
            } else {
                newX = tr.x();
                newY = tr.y() + tr.height();
                newRot = -90;
            }
            QTransform m = QTransform::fromTranslate(newX, newY);
            m.rotate(newRot);
            p->setTransform(m, true);
        }
        QRect iconRect;
        d->tabLayout(&tabV2, widget, &tr, &iconRect);
        // we compute tr twice because the style may override subElementRect
        tr = proxy()->subElementRect(SE_TabBarTabText, opt, widget);

        if (!tabV2.icon.isNull()) {
            QPixmap tabIcon =
                tabV2.icon.pixmap(tabV2.iconSize, (tabV2.state & State_Enabled) ? QIcon::Normal : QIcon::Disabled,
                                  (tabV2.state & State_Selected) ? QIcon::On : QIcon::Off);
            p->drawPixmap(iconRect.x(), iconRect.y(), tabIcon);
        }
        proxy()->drawItemText(p, tr, alignment, tab->palette, tab->state & State_Enabled, tab->text,
                              QPalette::WindowText);

        if (verticalTabs)
            p->restore();

        if (tabV2.state & State_HasFocus) {
            const int OFFSET = 1 + pixelMetric(PM_DefaultFrameWidth);

            int x1, x2;
            x1 = tabV2.rect.left();
            x2 = tabV2.rect.right() - 1;

            QStyleOptionFocusRect fropt;
            fropt.QStyleOption::operator=(*tab);
            fropt.rect.setRect(x1 + 1 + OFFSET, tabV2.rect.y() + OFFSET, x2 - x1 - 2 * OFFSET,
                               tabV2.rect.height() - 2 * OFFSET);
            drawPrimitive(PE_FrameFocusRect, &fropt, p, widget);
        }
        return true;
    }
    return false;
}

/*! \internal */
bool RibbonStyle::drawPanelButtonTool(const QStyleOption *opt, QPainter *p, const QWidget *w) const
{
    QTC_D(const RibbonStyle);
    return d->ribbonPaintManager()->drawPanelButtonTool(opt, p, w);
}

/*! \internal */
void RibbonStyle::calcRects(const QRect &rc, const QSize &pmSize, const QFontMetrics &fMetric, const QString &text,
                            bool hasMenu, QRect *pmRect, QRect *strFirsRect, QRect *strSecondRect) const
{
    QRect rect(QPoint(0, 0), pmSize);
    *pmRect = rect;

    QString strFirstRow, strSecondRow;
    splitString(text, strFirstRow, strSecondRow);

    if (!strFirstRow.isEmpty()) {
        QSize size = fMetric.size(Qt::TextSingleLine, strFirstRow);
        if (!size.isEmpty()) {
            if (hasMenu && strSecondRow.isEmpty())
                size.setWidth(size.width() + 4 + 5);

            *strFirsRect = QRect(QPoint(0, rect.bottom() + 2), size);
            rect = rect.united(*strFirsRect);
        }
    }
    if (!strSecondRow.isEmpty()) {
        QSize size = fMetric.size(Qt::TextSingleLine, strSecondRow);
        if (!size.isEmpty()) {
            if (hasMenu)
                size.setWidth(size.width() + 4 + 5);
            *strSecondRect = QRect(QPoint(0, rect.bottom() + 2), size);
            rect = rect.united(*strSecondRect);
        }
    }

    int dx = (rc.width() - rect.width()) / 2;
    int dy = (rc.height() - rect.height()) / 2;

    if (!pmRect->isEmpty()) {
        pmRect->translate(dx, dy);
        pmRect->translate((rect.width() - pmRect->width()) / 2, 0);
    }

    if (!strFirsRect->isEmpty()) {
        strFirsRect->translate(dx, dy);
        strFirsRect->translate((rect.width() - strFirsRect->width()) / 2, 0);
    }

    if (!strSecondRect->isEmpty()) {
        strSecondRect->translate(dx, dy);
        strSecondRect->translate((rect.width() - strSecondRect->width()) / 2, 0);
    }
}

/*! \internal */
bool RibbonStyle::drawToolButtonLabel(const QStyleOption *opt, QPainter *p, const QWidget *w) const
{
    if (qobject_cast<const RibbonBackstageButton *>(w)) {
        if (const StyleOptionBackstageButton *toolbutton = qstyleoption_cast<const StyleOptionBackstageButton *>(opt)) {
            QRect rect = toolbutton->rect;
            // QPalette pal = toolbutton->state & State_Enabled ? QPalette() : toolbutton->palette;
            QPalette pal = toolbutton->palette;

            // Arrow type always overrules and is always shown
            bool hasArrow = toolbutton->features & QStyleOptionToolButton::Arrow;
            if (((!hasArrow && toolbutton->icon.isNull()) && !toolbutton->text.isEmpty()) ||
                toolbutton->toolButtonStyle == Qt::ToolButtonTextOnly) {
                int alignment = Qt::AlignCenter | Qt::TextShowMnemonic;
                if (!styleHint(SH_UnderlineShortcut, opt, w))
                    alignment |= Qt::TextHideMnemonic;

                proxy()->drawItemText(p, rect, alignment, /*toolbutton->palette*/ pal,
                                      /*opt->state & State_Enabled*/ true, toolbutton->text, QPalette::ButtonText);
            } else {
                QPixmap pm;
                QSize pmSize = toolbutton->iconSize;
                if (pmSize.width() > qMin(rect.width(), rect.height())) {
                    const int iconExtent = proxy()->pixelMetric(PM_SmallIconSize);
                    pmSize = QSize(iconExtent, iconExtent);
                }

                if (!toolbutton->icon.isNull()) {
                    QIcon::State state = toolbutton->state & State_On ? QIcon::On : QIcon::Off;
                    QIcon::Mode mode;
                    if (!(toolbutton->state & State_Enabled))
                        mode = QIcon::Disabled;
                    else if ((opt->state & State_MouseOver) && (opt->state & State_AutoRaise))
                        mode = QIcon::Active;
                    else
                        mode = QIcon::Normal;
                    pm = toolbutton->icon.pixmap(toolbutton->rect.size().boundedTo(pmSize), mode, state);
                    pmSize = pm.size();
                }

                if (toolbutton->toolButtonStyle != Qt::ToolButtonIconOnly) {
                    int alignment = Qt::TextShowMnemonic;
                    if (!proxy()->styleHint(SH_UnderlineShortcut, opt, w))
                        alignment |= Qt::TextHideMnemonic;

                    p->setFont(toolbutton->font);

                    if (toolbutton->toolButtonStyle == Qt::ToolButtonTextUnderIcon) {
                        QRect pmRect, strFirsRect, strSecondRect;
                        calcRects(rect, pmSize, toolbutton->fontMetrics, toolbutton->text,
                                  toolbutton->features & QStyleOptionToolButton::HasMenu, &pmRect, &strFirsRect,
                                  &strSecondRect);

                        if (!hasArrow)
                            drawItemPixmap(p, QStyle::visualRect(opt->direction, rect, pmRect), Qt::AlignHCenter, pm);
                        // else
                        //     drawArrow(this, toolbutton, pr, p, w);

                        alignment |= Qt::AlignCenter;

                        QString strFirstRow, strSecondRow;
                        splitString(toolbutton->text, strFirstRow, strSecondRow);

                        if (!strFirstRow.isEmpty()) {
                            if (toolbutton->features & QStyleOptionToolButton::HasMenu && strSecondRow.isEmpty()) {
                                alignment &= ~Qt::AlignCenter;
                                alignment |= Qt::AlignVCenter | Qt::AlignLeft;
                            }
                            proxy()->drawItemText(p, strFirsRect, alignment, /*toolbutton->palette*/ pal,
                                                  /*toolbutton->state & State_Enabled*/ true, strFirstRow,
                                                  QPalette::ButtonText);
                        }

                        if (!strSecondRow.isEmpty()) {
                            if (toolbutton->features & QStyleOptionToolButton::HasMenu) {
                                alignment &= ~Qt::AlignCenter;
                                alignment |= Qt::AlignVCenter | Qt::AlignLeft;
                            }
                            proxy()->drawItemText(p, strSecondRect, alignment, /*toolbutton->palette*/ pal,
                                                  /*toolbutton->state & State_Enabled*/ true, strSecondRow,
                                                  QPalette::ButtonText);
                        }
                    } else {
                        QRect pmRect(QPoint(4, (rect.height() - pmSize.height()) / 2), pmSize);
                        if (!hasArrow)
                            drawItemPixmap(p, QStyle::visualRect(opt->direction, rect, pmRect), Qt::AlignCenter, pm);
                        // else
                        //     drawArrow(this, toolbutton, pr, p, w);
                        alignment |= Qt::AlignLeft | Qt::AlignVCenter;

                        QSize sizeText = toolbutton->fontMetrics.size(Qt::TextSingleLine, toolbutton->text);
                        QRect tcText(QPoint(pmRect.right() + 4, (rect.height() - sizeText.height()) / 2), sizeText);
                        proxy()->drawItemText(p, tcText, alignment, /*toolbutton->palette*/ pal,
                                              /*toolbutton->state & State_Enabled*/ true, toolbutton->text,
                                              QPalette::ButtonText);
                    }
                } else {
                    // if (hasArrow)
                    //     drawArrow(this, toolbutton, rect, p, w);
                    // else
                    {
                        QRect pr = rect;
                        if ((toolbutton->subControls & SC_ToolButtonMenu) ||
                            (toolbutton->features & QStyleOptionToolButton::HasMenu)) {
                            int mbi = proxy()->pixelMetric(PM_MenuButtonIndicator, opt, w);
                            pr.setWidth(pr.width() - mbi);
                        }
                        drawItemPixmap(p, pr, Qt::AlignCenter, pm);
                    }
                }
            }
        }
        return true;
    } else {
        return OfficeStyle::drawToolButtonLabel(opt, p, w);
    }
}

/*! \internal */
bool RibbonStyle::drawToolButton(const QStyleOption *opt, QPainter *p, const QWidget *w) const
{
    if (qobject_cast<const RibbonBackstageButton *>(w)) {
        if (const QStyleOptionToolButton *toolbutton = qstyleoption_cast<const QStyleOptionToolButton *>(opt)) {
            QRect button, menuarea;
            button = proxy()->subControlRect(CC_ToolButton, toolbutton, SC_ToolButton, w);
            menuarea = proxy()->subControlRect(CC_ToolButton, toolbutton, SC_ToolButtonMenu, w);

            State flags = toolbutton->state & ~State_Sunken;

            if (flags & State_AutoRaise) {
                if (!(flags & State_MouseOver) || !(flags & State_Enabled))
                    flags &= ~State_Raised;
            }
            State mflags = flags;
            if (toolbutton->state & State_Sunken) {
                if (toolbutton->activeSubControls & SC_ToolButton)
                    flags |= State_Sunken;
                mflags |= State_Sunken;
            }

            RibbonStyle *rs = (RibbonStyle *)(this);
            bool animation = isAnimationEnabled();
            rs->setAnimationEnabled(false);
            proxy()->drawPrimitive(PE_PanelButtonTool, toolbutton, p, w);
            rs->setAnimationEnabled(animation);

            QStyleOptionToolButton label = *toolbutton;
            label.state = flags;
            proxy()->drawControl(CE_ToolButtonLabel, &label, p, w);

            QPixmap pm;
            QSize pmSize = toolbutton->iconSize;
            if (pmSize.width() > qMin(toolbutton->rect.width(), toolbutton->rect.height())) {
                const int iconExtent = proxy()->pixelMetric(PM_SmallIconSize);
                pmSize = QSize(iconExtent, iconExtent);
            }

            if (!toolbutton->icon.isNull()) {
                QIcon::State state = toolbutton->state & State_On ? QIcon::On : QIcon::Off;
                QIcon::Mode mode;
                if (!(toolbutton->state & State_Enabled))
                    mode = QIcon::Disabled;
                else if ((opt->state & State_MouseOver) && (opt->state & State_AutoRaise))
                    mode = QIcon::Active;
                else
                    mode = QIcon::Normal;
                pm = toolbutton->icon.pixmap(toolbutton->rect.size().boundedTo(pmSize), mode, state);
                pmSize = pm.size();
            }

            if (toolbutton->subControls & SC_ToolButtonMenu) {
                QStyleOption tool(0);
                tool.palette = toolbutton->palette;

                QRect ir = menuarea, rcArrow;

                if (toolbutton->toolButtonStyle == Qt::ToolButtonTextUnderIcon) {
                    QString strFirstRow, strSecondRow;
                    splitString(toolbutton->text, strFirstRow, strSecondRow);
                    rcArrow = QRect(QPoint(strSecondRow.isEmpty() ? opt->rect.width() / 2 - 2 : opt->rect.right() - 7,
                                           opt->rect.bottom() - 8),
                                    QSize(5, 4));
                } else
                    rcArrow =
                        QRect(QPoint((ir.left() + ir.right() - 6) / 2, (ir.top() + ir.bottom() - 5) / 2), QSize(5, 4));

                tool.rect = rcArrow;
                tool.state = mflags;

                proxy()->drawPrimitive(PE_IndicatorArrowDown, &tool, p, w);
            } else if (toolbutton->features & QStyleOptionToolButton::HasMenu) {
                QStyleOptionToolButton newBtn = *toolbutton;
                if (toolbutton->toolButtonStyle == Qt::ToolButtonTextUnderIcon) {
                    QRect pmRect, strFirsRect, strSecondRect;
                    calcRects(toolbutton->rect, pmSize, toolbutton->fontMetrics, toolbutton->text, true, &pmRect,
                              &strFirsRect, &strSecondRect);

                    QRect r = strSecondRect.isEmpty() ? strFirsRect : strSecondRect;
                    newBtn.rect = QRect(QPoint(r.right() - 5, r.top() + (r.height()) / 2), QSize(5, 5));
                } else {
                    QRect ir = menuarea;
                    int mbi = proxy()->pixelMetric(PM_MenuButtonIndicator, toolbutton, w);
                    newBtn.rect =
                        QRect(QPoint(ir.right() + 4 - mbi, ir.y() + ir.height() - mbi), QSize(mbi - 5, mbi - 5));
                }
                proxy()->drawPrimitive(PE_IndicatorArrowDown, &newBtn, p, w);
            }
        }
        return true;
    } else {
        return OfficeStyle::drawToolButton(opt, p, w);
    }
}

/*! \internal */
bool RibbonStyle::drawToolBar(const QStyleOption *opt, QPainter *p, const QWidget *w) const
{
    QTC_D(const RibbonStyle);
    return d->ribbonPaintManager()->drawToolBar(opt, p, w);
}

/*! \internal */
bool RibbonStyle::drawGroupControlEntry(const QStyleOption *opt, QPainter *p, const QWidget *widget) const
{
    Q_UNUSED(opt);
    Q_UNUSED(p);
    Q_UNUSED(widget);
    return false;
}

/*! \internal */
bool RibbonStyle::drawIndicatorArrow(PrimitiveElement pe, const QStyleOption *opt, QPainter *p, const QWidget *w) const
{
    QTC_D(const RibbonStyle);
    return d->ribbonPaintManager()->drawIndicatorArrow(pe, opt, p, w);
}

/*! \internal */
void RibbonStyle::drawRectangle(QPainter *p, const QRect &rect, bool selected, bool pressed, bool enabled, bool checked,
                                bool popuped, BarType barType, BarPosition barPos) const
{
    QTC_D(const RibbonStyle);
    d->ribbonPaintManager()->drawRectangle(p, rect, selected, pressed, enabled, checked, popuped, barType, barPos);
}

/*! \internal */
void RibbonStyle::drawSplitButtonPopup(QPainter *p, const QRect &rect, bool selected, bool enabled, bool popuped) const
{
    Q_UNUSED(p);
    Q_UNUSED(rect);
    Q_UNUSED(selected);
    Q_UNUSED(enabled);
    Q_UNUSED(popuped);
}

/*! \internal */
void RibbonStyle::drawItemText(QPainter *painter, const QRect &rect, int alignment, const QPalette &pal, bool enabled,
                               const QString &text, QPalette::ColorRole textRole) const
{
    if (text.isEmpty())
        return;

    QPen savedPen;
    if (textRole != QPalette::NoRole) {
        savedPen = painter->pen();
        painter->setPen(pal.color(textRole));
    }
    if (!enabled) {
        QPen pen = painter->pen();
        painter->setPen(pal.light().color());
        painter->setPen(pen);
    }

    painter->drawText(rect, alignment, text);

    if (textRole != QPalette::NoRole)
        painter->setPen(savedPen);
}

/*! \internal */
void RibbonStyle::drawFillRect(const QStyleOption *opt, QPainter *p, const QWidget *widget) const
{
    QTC_D(const RibbonStyle)
    if (qobject_cast<const QMenu *>(widget ? widget->parentWidget() : Q_NULL)) {
        p->fillRect(opt->rect, d->m_clrControlGalleryMenuBk);
        return;
    }

    bool enabled = opt->state & State_Enabled;
    bool selected = opt->state & State_MouseOver;
    p->fillRect(opt->rect, selected && enabled ? d->m_clrControlGallerySelected : d->m_clrControlGalleryNormal);
}

/*! \internal */
void RibbonStyle::drawRect(const QStyleOption *opt, QPainter *p, const QWidget *widget) const
{
    Q_UNUSED(widget)
    QTC_D(const RibbonStyle)

    QPen savePen = p->pen();
    p->setPen(d->m_clrControlGalleryBorder);
    p->drawRect(opt->rect);
    p->setPen(savePen);
}

/*! \internal */
void RibbonStyle::drawKeyTip(const QStyleOption *opt, QPainter *p, const QWidget *widget) const
{
    QTC_D(const RibbonStyle);
    d->ribbonPaintManager()->drawKeyTip(opt, p, widget);
}

/*! \internal */
void RibbonStyle::drawBackstage(const QStyleOption *opt, QPainter *p, const QWidget *widget) const
{
    QTC_D(const RibbonStyle);
    d->ribbonPaintManager()->drawBackstage(opt, p, widget);
}

/*! \internal */
void RibbonStyle::drawRibbonBackstageCloseButton(const QStyleOption *opt, QPainter *p, const QWidget *w) const
{
    QTC_D(const RibbonStyle);
    d->ribbonPaintManager()->drawRibbonBackstageCloseButton(opt, p, w);
}

/*! \internal */
void RibbonStyle::drawRibbonSliderButton(const QStyleOption *opt, QPainter *p, const QWidget *w) const
{
    QTC_D(const RibbonStyle);
    d->ribbonPaintManager()->drawRibbonSliderButton(opt, p, w);
}

template <class T> static QWidget *getPrevParentWidget(QWidget *pWidget)
{
    if (qobject_cast<T *>(pWidget))
        return Q_NULL;

    QWidget *pPrevWidget = pWidget;
    while (pWidget) {
        pWidget = pWidget->parentWidget();
        if (qobject_cast<T *>(pWidget))
            return pPrevWidget;
        else
            pPrevWidget = pWidget;
    }
    return Q_NULL;
}

/*! \internal */
bool RibbonStyle::showToolTip(const QPoint &pos, QWidget *w)
{
    Q_UNUSED(pos);

    if (getParentWidget<RibbonSystemPopupBar>(w))
        return true;

    if (getParentWidget<RibbonBar>(w)) {
        QPoint p = pos;
        p += QPoint(2,
#ifdef Q_WS_WIN
                    21
#else
                    16
#endif
        );

        int posX = p.x();
        int posY = p.y();

        if (const RibbonGroup *group = getParentWidget<RibbonGroup>(w)) {
            Q_UNUSED(group);
            posX = w->mapToGlobal(w->rect().topLeft()).x();

            if (QWidget *prevWidget = getPrevParentWidget<RibbonGroup>(w)) {
                if (RibbonGallery *gallery = qobject_cast<RibbonGallery *>(w)) {
                    QRect rect = gallery->getDrawItemRect(gallery->selectedItem());
                    posX = prevWidget->mapToGlobal(rect.topLeft()).x();
                } else {
                    posX = prevWidget->mapToGlobal(prevWidget->rect().topLeft()).x();
                    if (qobject_cast<RibbonToolBarControl *>(prevWidget)) {
                        posX = w->mapToGlobal(w->rect().topLeft()).x();
                    }
                }
            }
            if (group->windowFlags() & Qt::Popup)
                posY = group->mapToGlobal(group->rect().bottomRight()).y() + int(DrawHelpers::dpiScaled(2.));
            else if (const RibbonPage *page = getParentWidget<RibbonPage>(w))
                posY = page->mapToGlobal(page->rect().bottomRight()).y() + int(DrawHelpers::dpiScaled(2.));
        }

        QIcon icon;
        QString strTitleText;
        QString strTipText = w->toolTip();

        if (RibbonGallery *gallery = qobject_cast<RibbonGallery *>(w)) {
            if (RibbonGalleryItem *item = gallery->item(gallery->selectedItem())) {
                QString str = item->toolTip();
                if (strTipText != str)
                    strTipText = str;
            }
        } else if (strTipText.isEmpty()) {
            QWidget *widget = w->parentWidget();

            if (!qobject_cast<RibbonGroup *>(widget)) {
                if (RibbonPageSystemRecentFileList *recentFile = qobject_cast<RibbonPageSystemRecentFileList *>(w)) {
                    if (QAction *currentAction = recentFile->getCurrentAction())
                        strTipText = currentAction->data().toString();
                } else {
                    while (widget) {
                        strTipText = widget->toolTip();
                        if (!strTipText.isEmpty())
                            break;
                        widget = widget->parentWidget();
                        if (qobject_cast<RibbonGroup *>(widget))
                            break;
                    }
                }
            }
        } else if (QToolButton *button = qobject_cast<QToolButton *>(w)) {
            if (!strTipText.isEmpty())
                strTipText.remove(QChar('&'));

            icon = button->icon();
            QString str = button->text();
            str.remove(QChar('&'));
            if (strTipText != str)
                strTitleText = str;

            if (QAction *defAction = button->defaultAction()) {
                QString strShortcuts;
                QKeySequence keySequence;
                QList<QKeySequence> lstShortcuts = defAction->shortcuts();

                foreach(keySequence, lstShortcuts)
                {
                    QString strShortcutString = keySequence.toString(QKeySequence::NativeText);
                    if (strShortcuts != "")
                        strShortcuts += ", ";
                    strShortcuts += strShortcutString;
                }
                if (strShortcuts != "") {
                    if (strTitleText != "")
                        strTitleText += " (" + strShortcuts + ")";
                    else
                        strTipText += " (" + strShortcuts + ")";
                }
            }
        }

        if (icon.isNull()) {
        }

        if (!strTitleText.isEmpty() || !strTipText.isEmpty() || !icon.isNull()) {
            QPoint globalPos(posX, posY);
            RibbonToolTip::showToolTip(globalPos, strTitleText, strTipText, icon, w);
            return true;
        }
    }
    return false;
}

static RibbonBar *findMainWindow()
{
    foreach(QWidget * widget, qApp->topLevelWidgets())
    {
        if (qobject_cast<QMainWindow *>(widget)) {
            QList<RibbonBar *> l = widget->findChildren<RibbonBar *>();
            if (l.size() > 0)
                return l.at(0);
        }
    }
    return Q_NULL;
}

/*! \reimp */
bool RibbonStyle::eventFilter(QObject *watched, QEvent *event)
{
    QTC_D(RibbonStyle);
    switch (event->type()) {
    case QEvent::HoverMove:
    case QEvent::HoverEnter:
    case QEvent::HoverLeave:
        if (QTabBar *tabBar = qobject_cast<QTabBar *>(watched)) {
            if (qobject_cast<QMdiArea *>(tabBar->parentWidget())) {
                QTC_D(RibbonStyle);
                d->tabHoverUpdate(tabBar, event);
            }
        }
        break;
    case QEvent::KeyPress:
        if (QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event)) {
            d->m_blockKeyTips = false;
            d->m_blockKeyTips =
                (!keyEvent->modifiers() || (keyEvent->modifiers() & (Qt::MetaModifier | Qt::AltModifier))) &&
                keyEvent->text().length() == 1;

#ifdef Q_OS_WIN
            if (!d->m_blockKeyTips) {
                quint32 virtualKey = keyEvent->nativeVirtualKey();
                if (keyEvent->key() != Qt::Key_Alt && ::MapVirtualKey(virtualKey, 2) == 0)
                    d->m_blockKeyTips = true;
            }
#endif   // Q_OS_WIN

            if (keyEvent->key() == Qt::Key_Alt) {
                d->m_completeKey = static_cast<QKeyEvent *>(event)->text().isEmpty();
                if (QMainWindow *widget = qobject_cast<QMainWindow *>(watched)) {
                    QList<RibbonBar *> l = widget->findChildren<RibbonBar *>();
                    for (int i = 0, count = l.count(); i < count; ++i) {
                        const RibbonBar *rb = l.at(i);
                        if (rb->keyTipsEnabled()) {
                            bool hasKeyTips = rb->qtc_d()->m_keyTips.count() > 0;
                            HideKeyTipEvent ktEvent;
                            QApplication::sendEvent(l.at(i), &ktEvent);
                            d->m_destroyKeyTips = hasKeyTips;
                        }
                    }
                }
            }
        }
        break;
    case QEvent::KeyRelease:
        if (QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event)) {
            if (keyEvent->key() == Qt::Key_Alt) {
                if (QMainWindow *widget = qobject_cast<QMainWindow *>(watched)) {
                    if (!d->m_destroyKeyTips) {
                        QList<RibbonBar *> l = widget->findChildren<RibbonBar *>();
                        for (int i = 0; i < l.size(); ++i) {
                            const RibbonBar *rb = l.at(i);
                            if (!rb->isBackstageVisible() && rb->keyTipsEnabled() && !d->m_blockKeyTips) {
                                ShowKeyTipEvent ktEvent(l.at(i));
                                QApplication::sendEvent(l.at(i), &ktEvent);
                                //                                    l.at(i)->setFocus();
                            }
                        }
                    } else
                        d->m_destroyKeyTips = false;
                }
                d->m_completeKey = false;
            }
        }
        break;
    case QEvent::Show:
        if (QMenu *menu = qobject_cast<QMenu *>(watched)) {
            if (RibbonBar *ribbonBar = ::findMainWindow()) {
                if (ribbonBar->qtc_d()->m_levels.size() > 0 || ((qobject_cast<RibbonPageSystemPopup *>(watched) ||
                                                                 qobject_cast<RibbonSystemPopupBar *>(watched)) &&
                                                                d->m_completeKey))
                    QApplication::postEvent(ribbonBar, new ShowKeyTipEvent(menu));
            }
            d->m_completeKey = false;
        }
        break;
        break;
    case QEvent::FontChange:
        if (qobject_cast<RibbonBar *>(watched)) {
            d->refreshMetrics();

            QWidgetList all = allWidgets();
            for (QWidgetList::ConstIterator it = all.constBegin(); it != all.constEnd(); ++it) {
                QWidget *w = *it;
                if (w->windowType() != Qt::Desktop && !w->testAttribute(Qt::WA_SetStyle))
                    QApplication::postEvent(w, new QEvent(QEvent::StyleChange));
            }
        }
        break;

    default:
        break;
    }
    return OfficeStyle::eventFilter(watched, event);
}

/*!
    \class RibbonStylePlugin
    \internal
*/
QStringList RibbonStylePlugin::keys() const { return QStringList() << "RibbonStyle"; }

QStyle *RibbonStylePlugin::create(const QString &key)
{
    if (key.toLower() == QLatin1String("ribbonstyle"))
        return new RibbonStyle();
    return Q_NULL;
}

#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
#else
QObject *qt_plugin_instance_ribbonstyle()
{
    static QObject *instance = Q_NULL;
    if (!instance)
        instance = new RibbonStylePlugin();
    return instance;
}
#endif
