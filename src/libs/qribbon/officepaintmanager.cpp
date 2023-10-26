/**
 * Copyleft (C) 2023 maminjie <canpool@163.com>
 **/
#include <QApplication>
#include <QBitmap>
#include <QCheckBox>
#include <QComboBox>
#include <QDesktopWidget>
#include <QDialog>
#include <QDockWidget>
#include <QGroupBox>
#include <QLineEdit>
#include <QListWidget>
#include <QMainWindow>
#include <QMdiArea>
#include <QMdiSubWindow>
#include <QPainter>
#include <QPainterPath>
#include <QStyle>
#include <QTextEdit>
#include <QToolBar>
#include <QToolButton>
#include <QToolTip>

#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
    #include <qdrawutil.h>
#endif

#if (QT_VERSION < QT_VERSION_CHECK(5, 0, 0))
    #include <QWorkspace>
#endif

#include "officepopupmenu.h"
#include "officestyle_p.h"
#include "ribboncontrols.h"
#include "ribbongallery.h"
#include "ribbontooltip.h"
#include "stylehelpers.h"

QRIBBON_USE_NAMESPACE

static const int iImageWidth = 9;
static const int iImageHeight = 9;
static const int iTextMargin = 3;
static const QColor clrTransparent = QColor(255, 0, 255);

static inline bool isAncestor(const QObject *child)
{
    while (child) {
        if (qobject_cast<const RibbonBar *>(child))
            return true;
        child = child->parent();
    }
    return false;
}

/*!
\class QRibbon::OfficePaintManager
\internal
*/
OfficePaintManager::OfficePaintManager(CommonStyle *baseStyle) : CommonPaintManager(baseStyle)
{
    loadPixmap();

    m_ImagesBlack = m_ImagesSrc;
    m_ImagesGray = mapTo3dColors(m_ImagesSrc, QColor(0, 0, 0), QColor(128, 128, 128));
    m_ImagesDkGray = mapTo3dColors(m_ImagesSrc, QColor(0, 0, 0), QColor(72, 72, 72));
    m_ImagesLtGray = mapTo3dColors(m_ImagesSrc, QColor(0, 0, 0), QColor(192, 192, 192));
    m_ImagesWhite = mapTo3dColors(m_ImagesSrc, QColor(0, 0, 0), QColor(255, 255, 255));
    m_ImagesBlack2 = mapTo3dColors(m_ImagesSrc, QColor(0, 0, 0), QColor(0, 0, 0));
}

OfficePaintManager::~OfficePaintManager() {}

OfficeStyle::Theme OfficePaintManager::theme() const { return ((OfficeStyle *)baseStyle())->getTheme(); }

QString OfficePaintManager::theme(const QString &str) const
{
    Q_D_STYLE(const OfficeStyle);
    return d->theme(str);
}

QColor OfficePaintManager::textColor(bool selected, bool pressed, bool enabled, bool checked, bool popuped,
                                     BarType barType, BarPosition barPosition) const
{
    return ((OfficeStyle *)baseStyle())
        ->getTextColor(selected, pressed, enabled, checked, popuped, barType, barPosition);
}

/*! \internal */
void OfficePaintManager::polish(QWidget *widget)
{
    Q_D_STYLE(OfficeStyle);

    if (qobject_cast<QAbstractSpinBox *>(widget)) {
        QPalette pal = widget->palette();
        pal.setColor(QPalette::Disabled, QPalette::Base, m_clrControlEditDisabled);
        pal.setColor(QPalette::Normal, QPalette::AlternateBase, d->m_clrControlEditNormal);
        widget->setPalette(pal);
    }
}

QIcon OfficePaintManager::standardIconEx(QStyle::StandardPixmap px, const QStyleOption *opt, const QWidget *wd,
                                         bool &ret) const
{
    Q_UNUSED(px);
    Q_UNUSED(opt);
    Q_UNUSED(wd);
    ret = false;
    return QIcon();
}

bool OfficePaintManager::drawWorkspace(const QStyleOption *opt, QPainter *p, const QWidget *w) const
{
    Q_UNUSED(w);
    p->fillRect(opt->rect, m_clrBarFace);
    return true;
}

// for TitleBar
/*! \internal */
bool OfficePaintManager::drawToolBar(const QStyleOption *opt, QPainter *p, const QWidget *w) const
{
    Q_UNUSED(w);
    p->fillRect(opt->rect, opt->palette.color(QPalette::Background));
    return true;
}

/*! \internal */
QColor OfficePaintManager::menuLabelColor() const { return m_clrBarShadow; }

/*! \internal */
void OfficePaintManager::drawSysButton(QPainter *p, const QRect &rect, bool down, bool highlighted) const
{
    Q_D_STYLE(const OfficeStyle);
    QColor color;
    if (down)
        color = m_clrHighlightDn;
    else if (highlighted)
        color = d->m_clrHighlight;

    if (color.isValid())
        p->fillRect(rect, color);

    if (down && highlighted)
        DrawHelpers::draw3dRectEx(*p, rect, m_clrHighlightDn, m_clrHighlightDn);
}

/*! \internal */
bool OfficePaintManager::drawTitleBar(const QStyleOptionComplex *opt, QPainter *p, const QWidget *wd) const
{
    if (!wd || !qobject_cast<const QMdiSubWindow *>(wd)) {
#if (QT_VERSION < QT_VERSION_CHECK(5, 0, 0))
        QWidget *widget = (wd && wd->parentWidget()) ? wd->parentWidget()->parentWidget() : Q_NULL;
        if (!qobject_cast<const QWorkspace *>(widget))
            return false;
#else
        return false;
#endif
    }

    if (const QStyleOptionTitleBar *tb = qstyleoption_cast<const QStyleOptionTitleBar *>(opt)) {
        QRect ir;
        if (opt->subControls & QStyle::SC_TitleBarLabel) {
            ir = baseStyle()->proxy()->subControlRect(QStyle::CC_TitleBar, tb, QStyle::SC_TitleBarLabel, wd);
            p->setPen(tb->palette.text().color());
            p->drawText(ir.x() + 2, ir.y(), ir.width() - 2, ir.height(),
                        Qt::AlignLeft | Qt::AlignVCenter | Qt::TextSingleLine, tb->text);
        }

        bool down = false;
        bool highlighted = false;
        QPixmap pm;

        QStyleOption tool(0);
        tool.palette = tb->palette;

        if (tb->subControls & QStyle::SC_TitleBarCloseButton && tb->titleBarFlags & Qt::WindowSystemMenuHint) {
            ir = baseStyle()->proxy()->subControlRect(QStyle::CC_TitleBar, tb, QStyle::SC_TitleBarCloseButton, wd);
            down = tb->activeSubControls & QStyle::SC_TitleBarCloseButton && (opt->state & QStyle::State_Sunken);
            highlighted =
                tb->activeSubControls & QStyle::SC_TitleBarCloseButton && (opt->state & QStyle::State_MouseOver);

            if ((tb->titleBarFlags & Qt::WindowType_Mask) == Qt::Tool)
                pm = baseStyle()->standardIcon(QStyle::SP_DockWidgetCloseButton, &tool, wd).pixmap(10, 10);
            else
                pm = baseStyle()->standardIcon(QStyle::SP_TitleBarCloseButton, &tool, wd).pixmap(10, 10);
            tool.rect = ir;
            tool.state = down ? QStyle::State_Sunken : QStyle::State_Raised;
            baseStyle()->proxy()->drawPrimitive(QStyle::PE_PanelButtonTool, &tool, p, wd);

            p->save();
            drawSysButton(p, ir, down, highlighted);
            baseStyle()->proxy()->drawItemPixmap(p, ir, Qt::AlignCenter, pm);
            p->restore();
        }

        if (tb->subControls & QStyle::SC_TitleBarMaxButton && tb->titleBarFlags & Qt::WindowMaximizeButtonHint &&
            !(tb->titleBarState & Qt::WindowMaximized)) {
            ir = baseStyle()->proxy()->subControlRect(QStyle::CC_TitleBar, tb, QStyle::SC_TitleBarMaxButton, wd);
            down = tb->activeSubControls & QStyle::SC_TitleBarMaxButton && (opt->state & QStyle::State_Sunken);
            highlighted =
                tb->activeSubControls & QStyle::SC_TitleBarMaxButton && (opt->state & QStyle::State_MouseOver);

            pm = baseStyle()->standardIcon(QStyle::SP_TitleBarMaxButton, &tool, wd).pixmap(10, 10);
            tool.rect = ir;
            tool.state = down ? QStyle::State_Sunken : QStyle::State_Raised;
            baseStyle()->proxy()->drawPrimitive(QStyle::PE_PanelButtonTool, &tool, p, wd);

            p->save();
            drawSysButton(p, ir, down, highlighted);
            baseStyle()->proxy()->drawItemPixmap(p, ir, Qt::AlignCenter, pm);
            p->restore();
        }

        if (tb->subControls & QStyle::SC_TitleBarMinButton && tb->titleBarFlags & Qt::WindowMinimizeButtonHint &&
            !(tb->titleBarState & Qt::WindowMinimized)) {
            ir = baseStyle()->proxy()->subControlRect(QStyle::CC_TitleBar, tb, QStyle::SC_TitleBarMinButton, wd);
            down = tb->activeSubControls & QStyle::SC_TitleBarMinButton && (opt->state & QStyle::State_Sunken);
            highlighted =
                tb->activeSubControls & QStyle::SC_TitleBarMinButton && (opt->state & QStyle::State_MouseOver);

            pm = baseStyle()->standardIcon(QStyle::SP_TitleBarMinButton, &tool, wd).pixmap(10, 10);
            tool.rect = ir;
            tool.state = down ? QStyle::State_Sunken : QStyle::State_Raised;
            baseStyle()->proxy()->drawPrimitive(QStyle::PE_PanelButtonTool, &tool, p, wd);

            p->save();
            drawSysButton(p, ir, down, highlighted);
            baseStyle()->proxy()->drawItemPixmap(p, ir, Qt::AlignCenter, pm);
            p->restore();
        }

        bool drawNormalButton =
            (tb->subControls & QStyle::SC_TitleBarNormalButton) &&
            (((tb->titleBarFlags & Qt::WindowMinimizeButtonHint) && (tb->titleBarState & Qt::WindowMinimized)) ||
             ((tb->titleBarFlags & Qt::WindowMaximizeButtonHint) && (tb->titleBarState & Qt::WindowMaximized)));

        if (drawNormalButton) {
            ir = baseStyle()->proxy()->subControlRect(QStyle::CC_TitleBar, tb, QStyle::SC_TitleBarNormalButton, wd);
            down = tb->activeSubControls & QStyle::SC_TitleBarNormalButton && (opt->state & QStyle::State_Sunken);
            highlighted =
                tb->activeSubControls & QStyle::SC_TitleBarNormalButton && (opt->state & QStyle::State_MouseOver);

            pm = baseStyle()->standardIcon(QStyle::SP_TitleBarNormalButton, &tool, wd).pixmap(10, 10);
            tool.rect = ir;
            tool.state = down ? QStyle::State_Sunken : QStyle::State_Raised;
            baseStyle()->proxy()->drawPrimitive(QStyle::PE_PanelButtonTool, &tool, p, wd);

            p->save();
            drawSysButton(p, ir, down, highlighted);
            baseStyle()->proxy()->drawItemPixmap(p, ir, Qt::AlignCenter, pm);
            p->restore();
        }

        if (tb->subControls & QStyle::SC_TitleBarShadeButton && tb->titleBarFlags & Qt::WindowShadeButtonHint &&
            !(tb->titleBarState & Qt::WindowMinimized)) {
            ir = baseStyle()->proxy()->subControlRect(QStyle::CC_TitleBar, tb, QStyle::SC_TitleBarShadeButton, wd);
            down = (tb->activeSubControls & QStyle::SC_TitleBarShadeButton && (opt->state & QStyle::State_Sunken));
            pm = baseStyle()->standardIcon(QStyle::SP_TitleBarShadeButton, &tool, wd).pixmap(10, 10);
            tool.rect = ir;
            tool.state = down ? QStyle::State_Sunken : QStyle::State_Raised;
            baseStyle()->proxy()->drawPrimitive(QStyle::PE_PanelButtonTool, &tool, p, wd);
            p->save();
            drawSysButton(p, ir, down, highlighted);
            baseStyle()->proxy()->drawItemPixmap(p, ir, Qt::AlignCenter, pm);
            p->restore();
        }

        if (tb->subControls & QStyle::SC_TitleBarUnshadeButton && tb->titleBarFlags & Qt::WindowShadeButtonHint &&
            tb->titleBarState & Qt::WindowMinimized) {
            ir = baseStyle()->proxy()->subControlRect(QStyle::CC_TitleBar, tb, QStyle::SC_TitleBarUnshadeButton, wd);

            down = tb->activeSubControls & QStyle::SC_TitleBarUnshadeButton && (opt->state & QStyle::State_Sunken);
            pm = baseStyle()->standardIcon(QStyle::SP_TitleBarUnshadeButton, &tool, wd).pixmap(10, 10);
            tool.rect = ir;
            tool.state = down ? QStyle::State_Sunken : QStyle::State_Raised;
            baseStyle()->proxy()->drawPrimitive(QStyle::PE_PanelButtonTool, &tool, p, wd);
            p->save();
            drawSysButton(p, ir, down, highlighted);
            baseStyle()->proxy()->drawItemPixmap(p, ir, Qt::AlignCenter, pm);
            p->restore();
        }
        if (tb->subControls & QStyle::SC_TitleBarContextHelpButton &&
            tb->titleBarFlags & Qt::WindowContextHelpButtonHint) {
            ir =
                baseStyle()->proxy()->subControlRect(QStyle::CC_TitleBar, tb, QStyle::SC_TitleBarContextHelpButton, wd);

            down = tb->activeSubControls & QStyle::SC_TitleBarContextHelpButton && (opt->state & QStyle::State_Sunken);
            pm = baseStyle()->standardIcon(QStyle::SP_TitleBarContextHelpButton, &tool, wd).pixmap(10, 10);
            tool.rect = ir;
            tool.state = down ? QStyle::State_Sunken : QStyle::State_Raised;
            baseStyle()->proxy()->drawPrimitive(QStyle::PE_PanelButtonTool, &tool, p, wd);
            p->save();
            drawSysButton(p, ir, down, highlighted);
            baseStyle()->proxy()->drawItemPixmap(p, ir, Qt::AlignCenter, pm);
            p->restore();
        }

        if (tb->subControls & QStyle::SC_TitleBarSysMenu && tb->titleBarFlags & Qt::WindowSystemMenuHint) {
            ir = baseStyle()->proxy()->subControlRect(QStyle::CC_TitleBar, tb, QStyle::SC_TitleBarSysMenu, wd);
            if (!tb->icon.isNull())
                tb->icon.paint(p, ir);
            else {
                int iconSize = baseStyle()->proxy()->pixelMetric(QStyle::PM_SmallIconSize, tb, wd);
                pm = baseStyle()->standardIcon(QStyle::SP_TitleBarMenuButton, &tool, wd).pixmap(iconSize, iconSize);
                tool.rect = ir;
                p->save();
                baseStyle()->proxy()->drawItemPixmap(p, ir, Qt::AlignCenter, pm);
                p->restore();
            }
        }
        return true;
    }
    return false;
}

/*! \internal */
bool OfficePaintManager::drawIndicatorToolBarSeparator(const QStyleOption *opt, QPainter *p, const QWidget *w) const
{
    Q_UNUSED(w);
    Q_UNUSED(opt);
    Q_UNUSED(p);
    return false;
}

// for DockWidget
/*! \internal */
bool OfficePaintManager::drawIndicatorDockWidgetResizeHandle(const QStyleOption *opt, QPainter *p,
                                                             const QWidget *w) const
{
    QRect rect = opt->rect;
    QColor clrSlider = m_clrBarShadow;
    if (!(opt->state & QStyle::State_Horizontal) || rect.height() > rect.width() * 4) {
        p->fillRect(opt->rect, m_clrBarFace);

        rect.setLeft(rect.center().x() - 1);
        rect.setRight(rect.center().x() + 1);

        if (const QMainWindow *mainWindow = qobject_cast<const QMainWindow *>(w)) {
            QRect rcFrame = mainWindow->rect();
            QRect screen = QApplication::desktop()->availableGeometry(mainWindow);

            rect.setTop(-rcFrame.top());
            rect.setBottom(rect.top() + screen.height() + 10);
        }
        DrawHelpers::drawGradientFill4(*p, rect, m_clrBarFace, clrSlider, clrSlider, m_clrBarFace);
    } else
        p->fillRect(opt->rect, clrSlider);

    return true;
}

// for menuBar
/*! \internal */
bool OfficePaintManager::drawPanelMenu(const QStyleOption *opt, QPainter *p, const QWidget *w) const
{
    Q_D_STYLE(const OfficeStyle);

    p->fillRect(opt->rect, d->m_clrBackgroundLight);
    if (qobject_cast<const QToolBar *>(w))
        DrawHelpers::draw3dRectEx(*p, opt->rect, d->m_clrMenubarBorder, d->m_clrMenubarBorder);
    return true;
}

/*! \internal */
bool OfficePaintManager::drawFrameMenu(const QStyleOption *opt, QPainter *p, const QWidget *w) const
{
    Q_UNUSED(w);
    Q_D_STYLE(const OfficeStyle);
    QRect rect = opt->rect;
    DrawHelpers::draw3dRectEx(*p, rect, d->m_clrMenubarBorder, d->m_clrMenubarBorder);
    rect.adjust(1, 1, -1, -1);
    DrawHelpers::draw3dRectEx(*p, rect, d->m_clrBackgroundLight, d->m_clrBackgroundLight);
    QRect rectLeft(1, 1, 2, rect.bottom() - 1);
    p->fillRect(rectLeft, m_clrBarHilite);
    return true;
}

/*! \internal */
bool OfficePaintManager::drawIndicatorMenuCheckMark(const QStyleOption *opt, QPainter *p, const QWidget *w) const
{
    Q_UNUSED(w);
    QRect rc = opt->rect;
    bool enabled = opt->state & QStyle::State_Enabled;
    bool highlighted = opt->state & QStyle::State_MouseOver;

    ImageIcons iImage = /*isRadio*/ false ? OfficePaintManager::Icon_Radio : OfficePaintManager::Icon_Check;

    rc.adjust(0, 1, 0, -1);

    OfficePaintManager::ImageState imageState = OfficePaintManager::Gray;

    if (!enabled)
        imageState = OfficePaintManager::LtGray;
    else if (highlighted)
        imageState = OfficePaintManager::DkGray;

    drawIcon(p, rc, iImage, imageState, QSize(9, 9));
    return true;
}

// for Buttons
/*! \internal */
bool OfficePaintManager::drawIndicatorCheckRadioButton(QStyle::PrimitiveElement element, const QStyleOption *option,
                                                       QPainter *p, const QWidget *widget) const
{
    Q_UNUSED(widget);
    Q_D_STYLE(OfficeStyle);

    bool isRadio = (element == QStyle::PE_IndicatorRadioButton);

    bool enabled = option->state & QStyle::State_Enabled;
    bool checked = option->state & QStyle::State_On;
    bool noChange = option->state & QStyle::State_NoChange;
    bool highlighted = option->state & QStyle::State_MouseOver;
    bool pressed = option->state & QStyle::State_Sunken;

    int stateChecked = checked ? 1 : 0;
    if (!isRadio && noChange)
        stateChecked = 2;

    QRect rc = option->rect;

    if (isRadio) {
        p->save();

        p->setRenderHint(QPainter::Antialiasing, true);
        QPainterPath circle;
        const QPointF circleCenter = rc.center() + QPoint(1, 1);
        const qreal outlineRadius = (rc.width() + (rc.width() + 1) % 2) / 2.0 - 1;
        circle.addEllipse(circleCenter, outlineRadius, outlineRadius);

        if (pressed)
            p->setBrush(m_clrHighlightDn);
        else if (pressed || highlighted)
            p->setBrush(d->m_clrHighlight);
        else
            p->setBrush(QColor(255, 255, 255));

        QColor clrBorder = !enabled ? m_clrBarShadow : (highlighted || pressed) ? m_clrHighlightDn : m_clrBarDkShadow;
        p->setPen(clrBorder);
        p->drawPath(circle);

        if (option->state & (QStyle::State_Sunken | QStyle::State_On)) {
            circle = QPainterPath();
            const qreal checkmarkRadius = outlineRadius / 2.32;
            circle.addEllipse(circleCenter, checkmarkRadius, checkmarkRadius);
            QColor checkMarkColor = highlighted && enabled ? m_clrAccentText : m_clrBarDkShadow;
            checkMarkColor = checkMarkColor.darker(180);
            checkMarkColor.setAlpha(200);
            p->setPen(checkMarkColor);
            checkMarkColor.setAlpha(180);
            p->setBrush(checkMarkColor);
            p->drawPath(circle);
        }
        p->restore();
    } else {
        p->save();

        rc = rc.adjusted(0, 1, -2, -1);
        if (pressed)
            p->fillRect(rc, m_clrHighlightDn);
        else if (pressed || highlighted)
            p->fillRect(rc, d->m_clrHighlight);
        else
            p->fillRect(rc, QColor(255, 255, 255));

        QColor clrBorder = !enabled ? m_clrBarShadow : (highlighted || pressed) ? m_clrHighlightDn : m_clrBarDkShadow;

        p->setPen(clrBorder);
        p->drawRect(rc);

        if (stateChecked == 1) {
            p->setRenderHint(QPainter::Antialiasing, true);
            QColor checkMarkColor = highlighted && enabled ? m_clrAccentText : m_clrBarDkShadow;
            checkMarkColor = checkMarkColor.darker(180);

            QPen checkPen = QPen(checkMarkColor, DrawHelpers::dpiScaled(1.1));
            checkMarkColor.setAlpha(210);
            p->translate(-1, 0.5);
            p->setPen(checkPen);
            p->setBrush(Qt::NoBrush);
            p->translate(0.2, 0.0);

            const int checkMarkPadding = DrawHelpers::dpiScaled(3);
            // Draw checkmark
            QPainterPath path;
            path.moveTo(1 + checkMarkPadding, rc.height() / 2.0);
            path.lineTo(rc.width() / 2.0, rc.height() - checkMarkPadding);
            path.lineTo(rc.width() - checkMarkPadding + 1.5, checkMarkPadding);
            p->drawPath(path.translated(rc.topLeft()));
            // QRect rcIndicator(QPoint(rc.x() + (rc.width() - iImageWidth)/2, rc.y() + (rc.height() -
            // iImageHeight)/2), QSize(iImageWidth, iImageHeight)); drawIcon(p, rcIndicator.topLeft(),
            // Icon_Check, highlighted ? Black2 : Gray, QSize(9, 9));
        } else if (stateChecked == 2) {
            QBrush brushTmp(Qt::darkGray, Qt::Dense4Pattern);
            p->fillRect(rc.adjusted(1, 1, 0, 0), brushTmp);
        }
        p->restore();
    }

    return true;
}

/*! \internal */
bool OfficePaintManager::drawIndicatorViewItemCheck(QStyle::PrimitiveElement element, const QStyleOption *option,
                                                    QPainter *painter, const QWidget *widget) const
{
    return drawIndicatorCheckRadioButton(element, option, painter, widget);
}

/*! \internal */
bool OfficePaintManager::drawPanelButtonCommand(const QStyleOption *opt, QPainter *p, const QWidget *w) const
{
    Q_UNUSED(w);
    Q_D_STYLE(OfficeStyle);
    if (const QStyleOptionButton *btn = qstyleoption_cast<const QStyleOptionButton *>(opt)) {
        uint resolve_mask = btn->palette.resolve();
        if (resolve_mask & (1 << QPalette::Base))
            return false;

        bool enabled = opt->state & QStyle::State_Enabled;
        bool checked = opt->state & QStyle::State_On;
        bool selected = opt->state & QStyle::State_MouseOver;
        bool pressed = opt->state & QStyle::State_Sunken;

        if (enabled) {
            if (pressed) {
                if (!checked)
                    p->fillRect(opt->rect, m_clrHighlightDn);
            } else if (selected || checked || (opt->state & QStyle::State_HasFocus))
                p->fillRect(opt->rect, d->m_clrHighlight);
            else
                p->fillRect(opt->rect, m_clrBarFace);
        } else
            p->fillRect(opt->rect, m_clrBarFace);

        QPen penSave = p->pen();
        p->setPen(m_clrBarDkShadow);
        p->drawRect(btn->rect.adjusted(0, 0, -1, -1));
        p->setPen(penSave);
        return true;
    }
    return false;
}

// shared
/*! \internal */
bool OfficePaintManager::drawPanelTipLabel(const QStyleOption *opt, QPainter *p, const QWidget *w) const
{
    Q_UNUSED(w)
    p->fillRect(opt->rect, m_clrBarHilite);
    QPen oldPen = p->pen();
    p->setPen(m_clrBarShadow);
    p->drawRect(opt->rect.adjusted(0, 0, -1, -1));
    p->setPen(oldPen);
    return true;
}

/*! \internal */
bool OfficePaintManager::drawControlEdit(const QStyleOption *opt, QPainter *p, const QWidget *widget) const
{
    Q_D_STYLE(OfficeStyle);
    if (const QStyleOptionFrame *panel = qstyleoption_cast<const QStyleOptionFrame *>(opt)) {
        bool usePalette = false;
        uint resolve_mask = panel->palette.resolve();

        if (widget) {
            // Since spin box includes a line edit we need to resolve the palette mask also from
            // the parent, as while the color is always correct on the palette supplied by panel,
            // the mask can still be empty. If either mask specifies custom base color, use that.
            if (QAbstractSpinBox *spinbox = qobject_cast<QAbstractSpinBox *>(widget->parentWidget()))
                resolve_mask |= spinbox->palette().resolve();
        }

        if (resolve_mask & (1 << QPalette::Base))
            usePalette = true;

        if (!usePalette && widget && qobject_cast<QComboBox *>(widget->parentWidget()))
            return true;

        bool enabled = opt->state & QStyle::State_Enabled;
        bool highlight = opt->state & QStyle::State_MouseOver;
        bool hasFocus = opt->state & QStyle::State_HasFocus;
        QRect rect(panel->rect);

        if (usePalette)
            p->fillRect(rect, panel->palette.brush(QPalette::Base));
        else
            p->fillRect(rect, enabled && (highlight || hasFocus) ? panel->palette.brush(QPalette::Base)
                              : enabled                          ? d->m_clrControlEditNormal
                                                                 : m_clrControlEditDisabled);

        if (panel->lineWidth > 0)
            baseStyle()->proxy()->drawPrimitive(QStyle::PE_FrameLineEdit, panel, p, widget);
        return true;
    }
    return false;
}

/*! \internal */
bool OfficePaintManager::drawFrameLineEdit(const QStyleOption *opt, QPainter *p, const QWidget *w) const
{
    Q_UNUSED(w);
    Q_D_STYLE(OfficeStyle);
    if (const QStyleOptionFrame *panel = qstyleoption_cast<const QStyleOptionFrame *>(opt)) {
        bool usePalette = false;
        uint resolve_mask = panel->palette.resolve();
        if (resolve_mask & (1 << QPalette::Base))
            usePalette = true;

        if (usePalette) {
            // we try to check if this lineedit is a delegate on a derived class.
            QPen oldPen = p->pen();
            // Inner white border
            p->setPen(QPen(opt->palette.base().color(), 1));
            p->drawRect(opt->rect.adjusted(1, 1, -2, -2));
            // Outer dark border
            p->setPen(QPen(opt->palette.shadow().color(), 1));
            p->drawRect(opt->rect.adjusted(0, 0, -1, -1));
            p->setPen(oldPen);
        } else {
            bool enabled = panel->state & QStyle::State_Enabled;
            bool highlighted = panel->state & QStyle::State_MouseOver;
            bool hasFocus = panel->state & QStyle::State_HasFocus;

            p->save();
            QRegion clipRegion = opt->rect;
            clipRegion -= panel->rect.adjusted(2, 2, -2, -2);
            p->setClipRegion(clipRegion);

            QColor colorBorder = d->m_clrEditCtrlBorder;

            if (!enabled)
                colorBorder = m_clrTextDisabled;
            else if (highlighted || hasFocus)
                colorBorder = d->m_clrAccent;

            p->setPen(colorBorder);
            p->drawRect(panel->rect.adjusted(0, 0, -1, -1));
            p->restore();
        }
        return true;
    }
    return false;
}

/*! \internal */
bool OfficePaintManager::drawComboBox(const QStyleOptionComplex *opt, QPainter *p, const QWidget *w) const
{
    Q_D_STYLE(const OfficeStyle);
    if (const QStyleOptionComboBox *cmb = qstyleoption_cast<const QStyleOptionComboBox *>(opt)) {
        QStyle::State flags = cmb->state;
        QStyle::SubControls sub = cmb->subControls;

        bool enabled = flags & QStyle::State_Enabled;
        bool highlighted = flags & QStyle::State_MouseOver;
        bool selected = flags & QStyle::State_Selected;
        bool dropped = cmb->activeSubControls == QStyle::SC_ComboBoxArrow &&
                       ((flags & QStyle::State_On) || (flags & QStyle::State_Sunken));
        bool dropButtonHot = cmb->activeSubControls == QStyle::SC_ComboBoxArrow && (flags & QStyle::State_MouseOver);
        bool hasFocus = flags & QStyle::State_HasFocus;

        if (cmb->frame) {
            QRect r = cmb->rect.adjusted(0, 0, -1, -1);
            bool usePalette = false;
            uint resolve_mask = cmb->palette.resolve();
            if (resolve_mask & (1 << QPalette::Base))
                usePalette = true;

            p->fillRect(r, !usePalette && enabled && (highlighted || selected || hasFocus)
                               ? cmb->palette.brush(QPalette::Base)
                           : enabled ? d->m_clrControlEditNormal
                                     : cmb->palette.brush(QPalette::Base));

            QColor colorBorder = d->m_clrEditCtrlBorder;

            if (!enabled)
                colorBorder = m_clrTextDisabled;
            else if (selected || dropped)
                colorBorder = d->m_clrAccent;
            else if (highlighted || hasFocus)
                colorBorder = d->m_clrAccent;

            QPen savePen = p->pen();
            p->setPen(colorBorder);
            p->drawRect(r);
            p->setPen(savePen);
        }

        if (sub & QStyle::SC_ComboBoxArrow) {
            QRect rcBtn = baseStyle()->proxy()->subControlRect(QStyle::CC_ComboBox, opt, QStyle::SC_ComboBoxArrow, w);
            rcBtn.adjust(0, 1, -1, -1);

            QColor clrFill = m_clrBarHilite;
            if (dropButtonHot || hasFocus) {
                p->fillRect(rcBtn, m_clrAccentLight);
                clrFill = m_clrAccentLight;
            } else if (hasFocus || dropped) {
                p->fillRect(rcBtn, m_clrHighlightDn);
                clrFill = m_clrHighlightDn;
            } else
                p->fillRect(rcBtn, clrFill);

            drawIconByColor(p, OfficePaintManager::Icon_ArowDown, rcBtn, clrFill);
        }
        return true;
    }
    return false;
}

// for SpinBox
/*! \internal */
bool OfficePaintManager::drawSpinBox(const QStyleOptionComplex *opt, QPainter *p, const QWidget *w) const
{
    Q_UNUSED(w);
    Q_D_STYLE(const OfficeStyle);

    if (const QStyleOptionSpinBox *sb = qstyleoption_cast<const QStyleOptionSpinBox *>(opt)) {
        bool highlighted = sb->state & QStyle::State_MouseOver;
        bool enabled = sb->state & QStyle::State_Enabled;
        bool hasFocus = sb->state & QStyle::State_HasFocus;

        if (sb->frame && (sb->subControls & QStyle::SC_SpinBoxFrame)) {
            QRect r = sb->rect.adjusted(0, 0, -1, -1);
            p->fillRect(r, enabled && (highlighted || hasFocus) ? sb->palette.brush(QPalette::Normal, QPalette::Base)
                           : enabled ? sb->palette.brush(QPalette::Normal, QPalette::AlternateBase)
                                     : sb->palette.brush(QPalette::Disabled, QPalette::Base));

            QColor colorBorder = d->m_clrEditCtrlBorder;

            if (!enabled)
                colorBorder = m_clrTextDisabled;
            else if (highlighted || hasFocus)
                colorBorder = d->m_clrAccent;

            QPen savePen = p->pen();
            p->setPen(colorBorder);
            p->drawRect(r);
            p->setPen(savePen);
        }

        QStyleOptionSpinBox copy = *sb;
        copy.state = QStyle::State_None;
        copy.subControls |= QStyle::SC_SpinBoxUp;
        QRect rcTop = baseStyle()->proxy()->subControlRect(QStyle::CC_SpinBox, &copy, QStyle::SC_SpinBoxUp, w);

        copy = *sb;

        OfficePaintManager::ImageIcons index[2][2] = {
            {OfficePaintManager::Icon_ArowUp, OfficePaintManager::Icon_ArowDown},
            {OfficePaintManager::Icon_ArowRight, OfficePaintManager::Icon_ArowLeft}};

        if (sb->subControls & QStyle::SC_SpinBoxUp) {
            bool pressedButton = false;
            bool hotButton = false;
            bool enabledIndicator = true;

            rcTop = baseStyle()->proxy()->subControlRect(QStyle::CC_SpinBox, sb, QStyle::SC_SpinBoxUp, w);

            if (!(sb->stepEnabled & QAbstractSpinBox::StepUpEnabled) || !(sb->state & QStyle::State_Enabled))
                enabledIndicator = false;
            else if (sb->activeSubControls == QStyle::SC_SpinBoxUp && (sb->state & QStyle::State_Sunken))
                pressedButton = true;
            else if (sb->activeSubControls == QStyle::SC_SpinBoxUp && (sb->state & QStyle::State_MouseOver))
                hotButton = true;

            if (pressedButton || hotButton) {
                p->fillRect(rcTop.adjusted(0, 1, -1, -1), pressedButton ? m_clrHighlightDn : d->m_clrHighlight);
            }
            drawIcon(p, rcTop, index[false ? 1 : 0][0],
                     enabledIndicator ? OfficePaintManager::Black : OfficePaintManager::Gray, QSize(9, 9));
        }

        if (sb->subControls & QStyle::SC_SpinBoxDown) {
            bool pressedButton = false;
            bool hotButton = false;
            bool enabledIndicator = true;

            QRect rcBottom = baseStyle()->proxy()->subControlRect(QStyle::CC_SpinBox, sb, QStyle::SC_SpinBoxDown, w);

            if (!(sb->stepEnabled & QAbstractSpinBox::StepDownEnabled) || !(sb->state & QStyle::State_Enabled))
                enabledIndicator = false;
            else if (sb->activeSubControls == QStyle::SC_SpinBoxDown && (sb->state & QStyle::State_Sunken))
                pressedButton = true;
            else if (sb->activeSubControls == QStyle::SC_SpinBoxDown && (sb->state & QStyle::State_MouseOver))
                hotButton = true;

            if (pressedButton || hotButton) {
                p->fillRect(rcBottom.adjusted(0, 0, -1, -2), pressedButton ? m_clrHighlightDn : d->m_clrHighlight);
            }
            drawIcon(p, rcBottom, index[false ? 1 : 0][1],
                     enabledIndicator ? OfficePaintManager::Black : OfficePaintManager::Gray, QSize(9, 9));
        }

        return true;
    }
    return false;
}

// for ToolBox
/*! \internal */
bool OfficePaintManager::drawToolBoxTabShape(const QStyleOption *, QPainter *, const QWidget *) const { return false; }

// for TabBar
/*! \internal */
bool OfficePaintManager::drawTabBarTabShape(const QStyleOption *option, QPainter *p, const QWidget *widget) const
{
    if (const QStyleOptionTab *tab = qstyleoption_cast<const QStyleOptionTab *>(option)) {
        QRect rect = option->rect;
        QRect fillRect = rect;
        const bool isHot = tab->state & QStyle::State_MouseOver;
        const bool selected = tab->state & QStyle::State_Selected;
        bool lastTab = tab->position == QStyleOptionTab::End;
        bool firstTab = tab->position == QStyleOptionTab::Beginning;
        const bool onlyOne = tab->position == QStyleOptionTab::OnlyOneTab;
        const int borderThickness = 1;
        // baseStyle()->proxy()->pixelMetric(QStyle::PM_DefaultFrameWidth, option, widget);
        const int tabOverlap = baseStyle()->proxy()->pixelMetric(QStyle::PM_TabBarTabOverlap, option, widget);

        if (tab->direction == Qt::RightToLeft &&
            (tab->shape == QTabBar::RoundedNorth || tab->shape == QTabBar::RoundedSouth)) {
            bool temp = firstTab;
            firstTab = lastTab;
            lastTab = temp;
        }

        const bool begin = firstTab || onlyOne;
        const bool end = lastTab || onlyOne;
        switch (tab->shape) {
        case QTabBar::RoundedNorth:
            rect.adjust(begin ? tabOverlap : 0, tabOverlap, end ? -tabOverlap : 0, 0);
            fillRect = rect;
            fillRect.adjust(1, 1, 0, 0);
            break;
        case QTabBar::RoundedSouth:
            rect.adjust(begin ? tabOverlap : 0, -tabOverlap, end ? -tabOverlap : 0, -tabOverlap);
            fillRect = rect;
            fillRect.adjust(1, 0, 0, -1);
            break;
        case QTabBar::RoundedEast:
            rect.adjust(-1, begin ? tabOverlap : 0, -tabOverlap, end ? -tabOverlap : 0);
            fillRect = rect;
            fillRect.adjust(1, 1, 0, 0);
            break;
        case QTabBar::RoundedWest:
            rect.adjust(tabOverlap, begin ? tabOverlap : 0, 0, end ? -tabOverlap : 0);
            fillRect = rect;
            fillRect.adjust(1, 1, 0, 0);
            break;
        default:
            break;
        }

        if (selected) {
            QPen savePen = p->pen();
            p->setPen(QPen(m_clrBarDkShadow, borderThickness));
            p->drawRect(rect);
            p->setPen(savePen);
        }

        if (isHot && !selected)
            p->fillRect(rect.adjusted(1, 1, -1, -1), m_clrHighlightMenuItem);
        else if (selected)
            p->fillRect(fillRect, m_clrBarFace);
    }
    return true;
}

/*! \internal */
bool OfficePaintManager::drawIndicatorTabClose(const QStyleOption *opt, QPainter *p, const QWidget *w) const
{
    Q_UNUSED(w);
    if ((opt->state & QStyle::State_Enabled) && (opt->state & QStyle::State_MouseOver))
        p->fillRect(opt->rect.adjusted(1, 1, 0, 0), m_clrHighlightMenuItem);

    ImageIcons iImage = OfficePaintManager::Icon_Close;

    OfficePaintManager::ImageState imageState = OfficePaintManager::Gray;
    if (!(opt->state & QStyle::State_Enabled))
        imageState = OfficePaintManager::LtGray;
    else if (opt->state & QStyle::State_MouseOver)
        imageState = OfficePaintManager::DkGray;
    drawIcon(p, opt->rect, iImage, imageState, QSize(9, 9));

    return true;
}

// for QForm
/*! \internal */
bool OfficePaintManager::drawShapedFrame(const QStyleOption *opt, QPainter *p, const QWidget *w) const
{
    bool ret = false;
    if (const QMdiArea *mdiArea = qobject_cast<const QMdiArea *>(w)) {
#if (QT_VERSION < QT_VERSION_CHECK(5, 7, 0))
        if (const QStyleOptionFrameV3 *f = qstyleoption_cast<const QStyleOptionFrameV3 *>(opt))
#else
        if (const QStyleOptionFrame *f = qstyleoption_cast<const QStyleOptionFrame *>(opt))
#endif
        {
            int frameShape = f->frameShape;
            int frameShadow = QFrame::Plain;
            if (f->state & QStyle::State_Sunken)
                frameShadow = QFrame::Sunken;
            else if (f->state & QStyle::State_Raised)
                frameShadow = QFrame::Raised;

            switch (frameShape) {
            case QFrame::Panel: {
                if (frameShadow == QFrame::Sunken) {
                    QRect rect = opt->rect;
                    if (QTabBar *tabBar = mdiArea->findChild<QTabBar *>()) {
                        p->fillRect(opt->rect, m_clrBarFace);

                        switch (mdiArea->tabPosition()) {
                        case QTabWidget::North:
                            rect.adjust(0, tabBar->sizeHint().height(), 0, 0);
                            break;
                        case QTabWidget::South:
                            rect.adjust(0, 0, 0, -tabBar->sizeHint().height());
                            break;
                        case QTabWidget::West:
                            rect.adjust(tabBar->sizeHint().width(), 0, 0, 0);
                            break;
                        case QTabWidget::East:
                            rect.adjust(0, 0, -tabBar->sizeHint().width(), 0);
                            break;
                        default:
                            break;
                        }
                        DrawHelpers::draw3dRectEx(*p, rect, m_clrBarDkShadow, m_clrBarDkShadow);
                    }
                    ret = true;
                }
                break;
            }
            default:
                break;
            }
        }
    }
    return ret;
}

/*! \internal */
bool OfficePaintManager::drawFrameWindow(const QStyleOption *opt, QPainter *p, const QWidget *wd) const
{
    Q_D_STYLE(const OfficeStyle);
    if (!wd || !qobject_cast<const QMdiSubWindow *>(wd)) {
#if (QT_VERSION < QT_VERSION_CHECK(5, 0, 0))
        QWidget *widget = (wd && wd->parentWidget()) ? wd->parentWidget() : Q_NULL;
        if (!qobject_cast<const QWorkspace *>(widget))
            return false;
#else
        return false;
#endif
    }

    QStyle::State flags = opt->state;
    QColor color;

    if (flags & QStyle::State_Active)
        color = d->m_clrFrameBorderActive0;
    else
        color = d->m_clrFrameBorderInactive0;

    int fwidth = baseStyle()->proxy()->pixelMetric(QStyle::PM_MdiSubWindowFrameWidth, 0, wd) - 1;
    // WP_FRAMELEFT;
    QRect rect = QRect(opt->rect.x(), opt->rect.y() + fwidth, opt->rect.x() + fwidth, opt->rect.height() - fwidth);
    p->fillRect(rect, color);

    // WP_FRAMERIGHT;
    rect = QRect(opt->rect.width() - fwidth, opt->rect.y() + fwidth, fwidth, opt->rect.height());
    p->fillRect(rect, color);

    // WP_FRAMEBOTTOM;
    rect = QRect(opt->rect.x(), opt->rect.height() - fwidth, opt->rect.width(), fwidth);
    p->fillRect(rect, color);

    // WP_CAPTION;
    rect = QRect(opt->rect.x(), opt->rect.y(), opt->rect.width(), opt->rect.y() + fwidth);
    p->fillRect(rect, color);

    return true;
}

// for ScrollBar
/*! \internal */
bool OfficePaintManager::drawScrollBar(const QStyleOptionComplex *opt, QPainter *p, const QWidget *w) const
{
    Q_D_STYLE(const OfficeStyle);
    if (d->isScrollBarsIgnored())
        return false;

    if (const QStyleOptionSlider *scrollbar = qstyleoption_cast<const QStyleOptionSlider *>(opt)) {
        // Make a copy here and reset it for each primitive.
        QStyleOptionSlider newScrollbar(*scrollbar);
        if (scrollbar->minimum == scrollbar->maximum)
            newScrollbar.state &= ~QStyle::State_Enabled;   // do not draw the slider.

        QStyle::State saveFlags = scrollbar->state;

        newScrollbar.state = saveFlags;
        newScrollbar.rect = opt->rect;

        p->fillRect(scrollbar->rect, m_clrBarShadow);

        QPen savePen = p->pen();
        p->setPen(d->m_clrMenubarBorder);
        if (scrollbar->subControls & QStyle::SC_ScrollBarSubLine) {
            QRect rc = baseStyle()->proxy()->subControlRect(QStyle::CC_ScrollBar, &newScrollbar,
                                                            QStyle::SC_ScrollBarSubLine, w);
            if (opt->state & QStyle::State_Horizontal)
                p->drawRect(scrollbar->rect.adjusted(rc.width() + 1, 0, -1, -1));
            else
                p->drawRect(scrollbar->rect.adjusted(0, rc.height() + 1, -1, -1));
        }

        if (scrollbar->subControls & QStyle::SC_ScrollBarAddLine) {
            QRect rc = baseStyle()->proxy()->subControlRect(QStyle::CC_ScrollBar, &newScrollbar,
                                                            QStyle::SC_ScrollBarAddLine, w);
            if (opt->state & QStyle::State_Horizontal)
                p->drawRect(scrollbar->rect.adjusted(0, 0, -(rc.width() + 1), -1));
            else
                p->drawRect(scrollbar->rect.adjusted(0, 0, -1, -(rc.height() + 1)));
        }
        p->setPen(savePen);

        if (scrollbar->subControls & QStyle::SC_ScrollBarSubLine) {
            newScrollbar.rect = scrollbar->rect;
            newScrollbar.state = saveFlags;
            newScrollbar.rect = baseStyle()->proxy()->subControlRect(QStyle::CC_ScrollBar, &newScrollbar,
                                                                     QStyle::SC_ScrollBarSubLine, w);

            if (opt->state & QStyle::State_Horizontal)
                newScrollbar.rect.adjust(0, 0, 0, -1);
            else
                newScrollbar.rect.adjust(0, 0, -1, 0);

            if (newScrollbar.rect.isValid())
                baseStyle()->proxy()->drawControl(QStyle::CE_ScrollBarSubLine, &newScrollbar, p, w);
        }

        if (scrollbar->subControls & QStyle::SC_ScrollBarAddLine) {
            newScrollbar.rect = scrollbar->rect;
            newScrollbar.state = saveFlags;
            newScrollbar.rect = baseStyle()->proxy()->subControlRect(QStyle::CC_ScrollBar, &newScrollbar,
                                                                     QStyle::SC_ScrollBarAddLine, w);

            if (opt->state & QStyle::State_Horizontal)
                newScrollbar.rect.adjust(0, 0, -1, -1);
            else
                newScrollbar.rect.adjust(0, 0, -1, -1);

            if (newScrollbar.rect.isValid())
                baseStyle()->proxy()->drawControl(QStyle::CE_ScrollBarAddLine, &newScrollbar, p, w);
        }

        if (scrollbar->subControls & QStyle::SC_ScrollBarSlider) {
            newScrollbar.rect = scrollbar->rect;
            newScrollbar.state = saveFlags;
            newScrollbar.rect = baseStyle()->proxy()->subControlRect(QStyle::CC_ScrollBar, &newScrollbar,
                                                                     QStyle::SC_ScrollBarSlider, w);

            if (opt->state & QStyle::State_Horizontal)
                newScrollbar.rect.adjust(0, 0, 0, -1);
            else
                newScrollbar.rect.adjust(0, 0, -1, 0);

            if (newScrollbar.rect.isValid())
                baseStyle()->proxy()->drawControl(QStyle::CE_ScrollBarSlider, &newScrollbar, p, w);
        }

        return true;
    }
    return false;
}

/*! \internal */
bool OfficePaintManager::drawScrollBarLine(QStyle::ControlElement element, const QStyleOption *opt, QPainter *p,
                                           const QWidget *w) const
{
    Q_UNUSED(w);

    Q_D_STYLE(const OfficeStyle);
    if (d->isScrollBarsIgnored())
        return false;

    if (const QStyleOptionSlider *scrollbar = qstyleoption_cast<const QStyleOptionSlider *>(opt)) {
        bool enabled = opt->state & QStyle::State_Enabled;
        bool highlighted = opt->state & QStyle::State_MouseOver;
        bool pressed = opt->state & QStyle::State_Sunken;

        bool pressButton = false;
        // bool highlightedButton = false;
        if (highlighted && pressed) {
            if (((scrollbar->activeSubControls & QStyle::SC_ScrollBarAddLine) &&
                 (element == QStyle::CE_ScrollBarAddLine)) ||
                ((scrollbar->activeSubControls & QStyle::SC_ScrollBarSubLine) &&
                 (element == QStyle::CE_ScrollBarSubLine)))
                pressButton = true;
        }
        /*
        else if (highlighted) {
            if (((scrollbar->activeSubControls & QStyle::SC_ScrollBarAddLine) && (element == QStyle::CE_ScrollBarAddLine)) ||
                ((scrollbar->activeSubControls & QStyle::SC_ScrollBarSubLine) && (element == QStyle::CE_ScrollBarSubLine)))
                highlightedButton = true;
        }
        */
        QColor clrFill = pressButton || !enabled ? m_clrBarShadow : m_clrControl;
        p->fillRect(scrollbar->rect, clrFill);

        QPen savePen = p->pen();
        p->setPen(highlighted ? m_clrEditBoxBorder : m_clrBarDkShadow);
        p->drawRect(scrollbar->rect);
        p->setPen(savePen);

        OfficePaintManager::ImageIcons index;
        if (opt->state & QStyle::State_Horizontal)
            index = element == QStyle::CE_ScrollBarAddLine ? OfficePaintManager::Icon_ArowRightTab3d
                                                           : OfficePaintManager::Icon_ArowLeftTab3d;
        else
            index = element == QStyle::CE_ScrollBarAddLine ? OfficePaintManager::Icon_ArowDownLarge
                                                           : OfficePaintManager::Icon_ArowUpLarge;

        OfficePaintManager::ImageState state = enabled ? stateByColor(clrFill, true) : OfficePaintManager::Gray;
        drawIcon(p, opt->rect, index, state, QSize(9, 9));
        return true;
    }
    return false;
}

/*! \internal */
bool OfficePaintManager::drawScrollBarSlider(const QStyleOption *opt, QPainter *p, const QWidget *w) const
{
    Q_UNUSED(w);

    Q_D_STYLE(const OfficeStyle);
    if (d->isScrollBarsIgnored())
        return false;

    if (const QStyleOptionSlider *scrollbar = qstyleoption_cast<const QStyleOptionSlider *>(opt)) {
        if (!scrollbar->rect.isEmpty()) {
            bool enabled = scrollbar->state & QStyle::State_Enabled;
            bool highlighted = scrollbar->state & QStyle::State_MouseOver;
            bool pressed = scrollbar->state & QStyle::State_Sunken;

            QRect rc(scrollbar->rect);
            bool show = scrollbar->state & QStyle::State_Horizontal ? rc.width() > 7 : rc.height() > 7;
            if (show) {
                QColor clrFill = pressed || !enabled ? m_clrBarShadow : m_clrBarLight;
                p->fillRect(rc, clrFill);

                QPen savePen = p->pen();
                QPen newPen(QBrush(highlighted ? m_clrEditBoxBorder : m_clrBarDkShadow), 2);
                p->setPen(newPen);
                if (opt->state & QStyle::State_Horizontal)
                    p->drawRect(rc.adjusted(0, 0, 0, 1));
                else
                    p->drawRect(rc.adjusted(0, 0, 1, 0));
                p->setPen(savePen);
            }
        }
        return true;
    }
    return false;
}

/*! \internal */
bool OfficePaintManager::drawScrollBarPage(QStyle::ControlElement element, const QStyleOption *opt, QPainter *p,
                                           const QWidget *w) const
{
    Q_UNUSED(element);
    Q_UNUSED(opt);
    Q_UNUSED(p);
    Q_UNUSED(w);

    Q_D_STYLE(const OfficeStyle);
    if (d->isScrollBarsIgnored())
        return false;

    return true;
}

// for stausBar
/*! \internal */
bool OfficePaintManager::drawPanelStatusBar(const QStyleOption *opt, QPainter *p, const QWidget *w) const
{
    Q_UNUSED(w);

    Q_D_STYLE(const OfficeStyle);
    p->fillRect(opt->rect, d->m_clrStatusBarShadow);
    return true;
}

// for SizeGrip
/*! \internal */
bool OfficePaintManager::drawSizeGrip(const QStyleOption *opt, QPainter *p, const QWidget *w) const
{
    Q_UNUSED(w);
    QPoint pt(opt->rect.right() - 3, opt->rect.bottom() - 3);
    for (int y = 0; y < 3; y++) {
        for (int x = 0; x < 3 - y; x++)
            p->fillRect(QRect(QPoint(pt.x() + 1 - x * 4, pt.y() + 1 - y * 4), QSize(2, 2)), QColor(255, 255, 255));
    }
    return true;
}

// for ViewItem
/*! \internal */
bool OfficePaintManager::drawPanelItemViewItem(const QStyleOption *opt, QPainter *p, const QWidget *w) const
{
    Q_UNUSED(opt);
    Q_UNUSED(p);
    Q_UNUSED(w);
    return false;
}

/*! \internal */
bool OfficePaintManager::drawHeaderSection(const QStyleOption *opt, QPainter *p, const QWidget *w) const
{
    Q_UNUSED(w);
    bool isPressed = opt->state & QStyle::State_Sunken;
    bool isHighlighted = opt->state & QStyle::State_MouseOver;

    QColor clrFill = m_clrBarFace;
    QColor clrBorder = m_clrBarShadow;

    if (isPressed) {
        clrFill = m_clrHighlightDn;
        clrBorder = QColor(255, 255, 255);
    } else if (isHighlighted) {
        clrFill = m_clrBarLight;
        clrBorder = m_clrAccentLight;
    }

    p->fillRect(opt->rect, clrFill);

    QPen penSave = p->pen();
    p->setPen(clrBorder);
    p->drawRect(opt->rect.adjusted(0, 0, -1, -1));
    p->setPen(penSave);
    return true;
}

/*! \internal */
bool OfficePaintManager::drawHeaderEmptyArea(const QStyleOption *opt, QPainter *p, const QWidget *w) const
{
    Q_UNUSED(w);
    p->fillRect(opt->rect, m_clrBarFace);

    p->fillRect(QRect(QPoint(opt->rect.left(), opt->rect.bottom()), QSize(opt->rect.width(), 1)), m_clrBarShadow);

    p->fillRect(QRect(QPoint(opt->rect.right(), opt->rect.top()), QSize(1, opt->rect.height())), m_clrBarShadow);

    return true;
}

/*! \internal */
bool OfficePaintManager::drawIndicatorHeaderArrow(const QStyleOption *, QPainter *, const QWidget *) const
{
    return false;
}

/*! \internal */
bool OfficePaintManager::drawIndicatorArrow(QStyle::PrimitiveElement pe, const QStyleOption *opt, QPainter *p,
                                            const QWidget *w) const
{
    Q_UNUSED(w);
    switch (pe) {
    case QStyle::PE_IndicatorArrowDown:
        if (qobject_cast<const QToolButton *>(w) && !qobject_cast<const QTabBar *>(w->parentWidget())) {
            QRect rc = opt->rect;
            bool enabled = opt->state & QStyle::State_Enabled;
            bool highlighted = opt->state & QStyle::State_MouseOver;

            ImageIcons iImage = OfficePaintManager::Icon_ArowDown;

            OfficePaintManager::ImageState imageState = OfficePaintManager::Gray;

            if (!enabled)
                imageState = OfficePaintManager::LtGray;
            else if (highlighted)
                imageState = OfficePaintManager::DkGray;

            rc.translate(rc.width() / 2 - 3, 0);
            drawIcon(p, rc, iImage, imageState, QSize(9, 9));
            return true;
        }

        break;
    default:
        return false;
    }
    return false;
}

/*! \internal */
bool OfficePaintManager::drawGalleryToolButton(const QStyleOption *opt, QPainter *p, const QWidget *widget) const
{
    Q_D_STYLE(const OfficeStyle);
    if (const QStyleOptionToolButton *toolbutton = qstyleoption_cast<const QStyleOptionToolButton *>(opt)) {
        if (widget) {
            OfficePaintManager::ImageIcons id = OfficePaintManager::Icon_Check;
            if (widget->property(_qrn_PopupButtonGallery).toBool())
                id = OfficePaintManager::Icon_CustomizeArowDown;
            else if (widget->property(_qrn_ScrollUpButtonGallery).toBool())
                id = OfficePaintManager::Icon_ArowUp;
            else if (widget->property(_qrn_ScrollDownButtonGallery).toBool())
                id = OfficePaintManager::Icon_ArowDown;

            if (id != OfficePaintManager::Icon_Check) {
                bool enabled = opt->state & QStyle::State_Enabled;
                bool highlighted = opt->state & QStyle::State_MouseOver;
                bool pressed = opt->state & QStyle::State_Sunken;

                QPen penSave = p->pen();
                p->setPen(m_clrBarShadow);
                p->drawRect(toolbutton->rect.adjusted(0, 0, -1, -1));
                p->setPen(penSave);

                QRect rect = toolbutton->rect.adjusted(1, 1, -1, -1);
                if (pressed && enabled)
                    p->fillRect(rect, m_clrHighlightDn);
                else if ((pressed || highlighted) && enabled)
                    p->fillRect(rect, d->m_clrHighlight);
                else
                    p->fillRect(rect, QColor(255, 255, 255));

                drawIcon(p, rect, id, OfficePaintManager::White);
                drawIcon(p, rect, id, enabled ? OfficePaintManager::Black : OfficePaintManager::Gray);

                return true;
            }
        }
    }
    return false;
}

/*! \internal */
void OfficePaintManager::drawMenuItemSeparator(const QStyleOption *opt, QPainter *p, const QWidget *widget) const
{
    Q_D_STYLE(const OfficeStyle);
    if (const QStyleOptionMenuItem *menuitem = qstyleoption_cast<const QStyleOptionMenuItem *>(opt)) {
        if (menuitem->text.isEmpty()) {
            int x, y, w, h;
            menuitem->rect.getRect(&x, &y, &w, &h);

            // windows always has a check column, regardless whether we have an icon or not
            const int iconSize = baseStyle()->proxy()->pixelMetric(QStyle::PM_ToolBarIconSize, opt, widget);
            int yoff = (y - 1 + h / 2);
            int xIcon = iconSize;
            if (qobject_cast<const OfficePopupMenu *>(widget))
                xIcon = 0;

            QPen penSave = p->pen();
            p->setPen(d->m_clrMenuPopupSeparator);
            p->drawLine(x + 2 + xIcon, yoff, x + w - 4, yoff);
            p->setPen(penSave);
        } else {
            p->save();
            p->fillRect(menuitem->rect, menuLabelColor());

            QRect rectText = menuitem->rect;
            rectText.adjust(iTextMargin, 0, -iTextMargin, -iTextMargin);

            QFont font = menuitem->font;
            font.setBold(true);
            p->setFont(font);
            p->setPen(opt->state & QStyle::State_Enabled ? d->m_clrMenuPopupText : d->m_clrMenuBarGrayText);
            p->drawText(rectText, Qt::AlignVCenter | Qt::TextSingleLine, menuitem->text);
            p->restore();
        }
    }
}

/*! \internal */
void OfficePaintManager::drawRectangle(QPainter *p, const QRect &rect, bool selected, bool pressed, bool enabled,
                                       bool checked, bool popuped, BarType barType, BarPosition barPos) const
{
    Q_UNUSED(popuped);
    Q_UNUSED(enabled);
    Q_UNUSED(pressed);
    Q_D_STYLE(OfficeStyle);
    if (barType != TypePopup) {
        QRect rc = rect;
        rc.adjust(1, 1, -1, -1);
        QColor color = QColor();
        if (checked)
            color = m_clrHighlightDn;
        else if (selected)
            color = d->m_clrHighlight;

        if (color.isValid())
            p->fillRect(rc, color);

        if (checked && selected) {
            QPen oldPen = p->pen();
            p->setPen(m_clrHighlightChecked);
            p->drawRect(rc.adjusted(-1, -1, 0, 0));
            p->setPen(oldPen);
        }
    } else if (barPos == BarPopup && selected && barType == TypePopup) {
        QRect rc = rect;
        rc.adjust(1, 0, -1, 0);
        p->fillRect(rc, m_clrHighlightMenuItem);
        DrawHelpers::draw3dRectEx(*p, rc, m_clrMenuItemBorder, m_clrMenuItemBorder);
    } else if (enabled && barPos == BarPopup && checked && barType == TypePopup) {
        QRect rc = rect;
        rc.adjust(1, 0, -1, 0);
        p->fillRect(rc, m_clrHighlightDn);
    }
}

/*! \internal */
void OfficePaintManager::setupPalette(QWidget *widget) const
{
    Q_D_STYLE(OfficeStyle);
    if (qobject_cast<QToolBar *>(widget)) {
        QPalette palette = widget->palette();
        palette.setColor(QPalette::Window, d->m_clrDockBar);
        widget->setPalette(palette);
    }
}

/*! \internal */
bool OfficePaintManager::pixelMetric(QStyle::PixelMetric pm, const QStyleOption *opt, const QWidget *w, int &ret) const
{
    Q_UNUSED(opt);
    Q_UNUSED(w);
    switch (pm) {
    case QStyle::PM_MenuButtonIndicator:
        if (qobject_cast<const QToolButton *>(w)) {
            ret = int(DrawHelpers::dpiScaled(qreal(iImageWidth + 1)));
            return true;
        }
        break;
    case QStyle::PM_MdiSubWindowFrameWidth:
#if (QT_VERSION < QT_VERSION_CHECK(5, 0, 0))
    {
        QWidget *widget = (w && w->parentWidget()) ? w->parentWidget() : Q_NULL;
        if (qobject_cast<const QWorkspace *>(widget)) {
            ret = 3;
            return true;
        }
    }
#else
        if (w && qobject_cast<const QMdiSubWindow *>(w)) {
            ret = 3;
            return true;
        }
#endif
    break;
    default:
        break;
    };
    return false;
}

/*! \internal */
bool OfficePaintManager::styleHint(QStyle::StyleHint hint, const QStyleOption *opt, const QWidget *w,
                                   QStyleHintReturn *returnData, int &ret) const
{
    Q_UNUSED(opt)
    Q_UNUSED(returnData)
    switch (hint) {
    case QStyle::SH_TitleBar_NoBorder:
        if (w && qobject_cast<const QMdiSubWindow *>(w)) {
            ret = false;
            return true;
        }
        break;
    default:
        break;
    };
    return false;
}

/*! \internal */
void OfficePaintManager::modifyColors()
{
    Q_D_STYLE(OfficeStyle);
    m_clrAccentLight = DrawHelpers::colorMakePale(d->m_clrAccent.rgb(), .9);
    m_clrAccentHilight = DrawHelpers::pixelAlpha(d->m_clrAccent.rgb(), 115);
    m_clrAccentText = DrawHelpers::pixelAlpha(d->m_clrAccent.rgb(), 75);

    d->m_clrFrameBorderActive0 = m_clrAccentText;
    d->m_clrFrameBorderInactive0 = DrawHelpers::colorMakeLighter(m_clrBarDkShadow.rgb());
    d->m_clrTabSelectedText = m_clrAccentText;

    d->m_clrHighlight = m_clrAccentLight;
    d->m_clrHighlightNC = d->m_clrHighlight;
    m_clrHighlightDn = DrawHelpers::colorMakeDarker(m_clrAccentLight.rgb(), .14);
    m_clrHighlightDnNC = m_clrHighlightDn;
    m_clrHighlightChecked = DrawHelpers::colorMakeDarker(m_clrAccentLight.rgb(), .05);

    m_clrBarShadow = helper().getColor("Window", "BarShadow");
    m_clrBarText = helper().getColor("Window", "BarText");
    m_clrBarHilite = helper().getColor("Window", "BarHilite");
    m_clrBarDkShadow = helper().getColor("Window", "BarDkShadow");
    m_clrBarFace = helper().getColor("Window", "BarFace");
    m_clrBarLight = helper().getColor("Window", "BarLight");

    d->m_clrStatusBarShadow = d->m_clrAccent;
    d->m_clrControlEditNormal = m_clrBarHilite;   // m_clrBarLight;
    d->m_clrNormalCaptionText = m_clrBarText;
    d->m_clrActiveCaptionText = m_clrAccentText;
    d->m_clrEditCtrlBorder = QColor(171, 171, 171);

    d->m_clrMenubarBorder = DrawHelpers::colorMakeLighter(m_clrBarDkShadow.rgb());   // m_clrMenuBorder
    d->m_clrStatusBarText = QColor(255, 255, 255);
    d->m_clrBackgroundLight = QColor(255, 255, 255);   // m_clrMenuLight
    d->m_clrDockBar = QColor(255, 255, 255);
    d->m_clrControlGalleryLabel = m_clrBarShadow;
    d->m_crBorderShadow = m_clrBarShadow;
    d->m_clrGridLineColor = m_clrBarShadow;
    d->m_clrMenuPopupText = m_clrBarText;

    m_clrHighlightMenuItem = m_clrAccentLight;   // m_clrHighlightMenuItem
    m_clrMenuItemBorder = d->m_clrHighlight;     // m_clrMenuItemBorder

    m_clrTextDisabled = m_clrBarDkShadow;
    m_clrControlEditDisabled = m_clrBarFace;
    m_clrControl = m_clrBarHilite;
    m_clrEditBoxBorder = QColor(171, 171, 171);

    m_ImagesBlack = mapTo3dColors(m_ImagesSrc, QColor(0, 0, 0), QColor(119, 119, 119));
    m_ImagesBlack2 = mapTo3dColors(m_ImagesSrc, QColor(0, 0, 0), m_clrAccentText);
}

/*! \internal */
OfficePaintManager::ImageState OfficePaintManager::stateByColor(const QColor &color, bool backgroundColor) const
{
    if (!color.isValid())
        return Black;

    qreal h, l, s, a;
    color.getHslF(&h, &s, &l, &a);

    if (backgroundColor)
        return l < 0.7 ? White : Black;
    else
        return l > 0.7 ? White : Black;
}

/*! \internal */
void OfficePaintManager::drawIcon(QPainter *p, const QRect &rectImage, ImageIcons index, ImageState state,
                                  const QSize &sizeImageDest) const
{
    const QSize sizeImage = (sizeImageDest == QSize(0, 0)) ? QSize(9, 9) : sizeImageDest;

    QPoint ptImage(rectImage.left() + (rectImage.width() - sizeImage.width()) / 2 +
                       ((rectImage.width() - sizeImage.width()) % 2),
                   rectImage.top() + (rectImage.height() - sizeImage.height()) / 2 +
                       ((rectImage.height() - sizeImage.height()) % 2));

    drawIcon(p, ptImage, index, state, sizeImage);
}

/*! \internal */
void OfficePaintManager::drawIcon(QPainter *p, const QPoint &pos, ImageIcons index, ImageState state,
                                  const QSize &sz) const
{
    QPixmap images = (state == Black)    ? m_ImagesBlack
                     : (state == Gray)   ? m_ImagesGray
                     : (state == DkGray) ? m_ImagesDkGray
                     : (state == LtGray) ? m_ImagesLtGray
                     : (state == White)  ? m_ImagesWhite
                                         : m_ImagesBlack2;

    QRect rcSrc(QPoint((int)index * sz.width(), 0), sz);
    p->drawPixmap(pos, images, rcSrc);
}

/*! \internal */
QPixmap OfficePaintManager::mapTo3dColors(const QPixmap &px, const QColor &clrSrc, const QColor &clrDest) const
{
    QImage img = px.toImage();
    for (int y = 0; y < img.height(); ++y) {
        QRgb *scanLine = (QRgb *)img.scanLine(y);
        for (int x = 0; x < img.width(); ++x) {
            QRgb pixel = *scanLine;
            if (pixel == clrSrc.rgb())
                *scanLine = clrDest.rgb();
            ++scanLine;
        }
    }
    return QPixmap::fromImage(img);
}

/*! \internal */
void OfficePaintManager::loadPixmap()
{
    m_ImagesSrc = QPixmap(":/res/menuimg-pro24.png");
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
    QImage img = m_ImagesSrc.toImage();
    img.setAlphaChannel(img.createMaskFromColor(clrTransparent.rgb(), Qt::MaskOutColor));
    m_ImagesSrc = QPixmap::fromImage(img);
#else
    m_ImagesSrc.setAlphaChannel(m_ImagesSrc.createMaskFromColor(clrTransparent, Qt::MaskOutColor));
#endif
}
