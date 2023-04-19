/**
 * Copyleft (C) 2023 maminjie <canpool@163.com>
 **/
#include <QApplication>
#include <QBitmap>
#include <QCheckBox>
#include <QComboBox>
#include <QDockWidget>
#include <QGroupBox>
#include <QLineEdit>
#include <QMdiArea>
#include <QMessageBox>
#include <QPainter>
#include <QPainterPath>
#include <QTextEdit>
#include <QToolTip>

#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
    #include <qdrawutil.h>
#endif /* Q_OS_WIN*/

#include "commonstyle.h"
#include "commonstyle_p.h"
#include "officeframehelper.h"
#include "ribbon_p.h"
#include "ribbonbackstageview.h"
#include "ribbonbar.h"
#include "ribbonbutton_p.h"
#include "ribbongallerycontrols.h"
#include "ribbongroup.h"
#include "ribbongroup_p.h"
#include "ribbonmainwindow.h"
#include "ribbonpage.h"
#include "ribbonquickaccessbar.h"
#include "ribbonsliderpane.h"
#include "ribbonstatusbar.h"
#include "ribbonstyle.h"
#include "ribbonstyle_p.h"
#include "ribbonsystempopupbar.h"
#include "ribbontabbar.h"
#include "ribbontooltip.h"
#include "stylehelpers.h"

QRIBBON_USE_NAMESPACE

/*
static QString qtc_getColorName(RibbonPage::ContextColor color)
{
    switch (color) {
    case RibbonPage::ContextColorBlue:
        return QString("Blue");
    case RibbonPage::ContextColorYellow:
        return QString("Yellow");
    case RibbonPage::ContextColorGreen:
        return QString("Green");
    case RibbonPage::ContextColorRed:
        return QString("Red");
    case RibbonPage::ContextColorPurple:
        return QString("Purple");
    case RibbonPage::ContextColorCyan:
        return QString("Cyan");
    case RibbonPage::ContextColorOrange:
        return QString("Orange");
    default:
        break;
    }
    return QString("");
}
*/

static QColor qtc_getColorToRGB(RibbonPage::ContextColor color)
{
    switch (color) {
    case RibbonPage::ContextColorRed:
        return QColor(255, 160, 160);

    case RibbonPage::ContextColorOrange:
        return QColor(239, 189, 55);

    case RibbonPage::ContextColorYellow:
        return QColor(253, 229, 27);

    case RibbonPage::ContextColorGreen:
        return QColor(113, 190, 89);

    case RibbonPage::ContextColorBlue:
        return QColor(128, 181, 196);

    case RibbonPage::ContextColorCyan:
        return QColor(114, 163, 224);

    case RibbonPage::ContextColorPurple:
        return QColor(214, 178, 209);
    default:
        break;
    }
    return QColor();
}

/* RibbonPaintManager */
/*! \internal */
RibbonPaintManager::RibbonPaintManager(CommonStyle *baseStyle) : OfficePaintManager(baseStyle) {}

/*! \internal */
RibbonPaintManager::~RibbonPaintManager() {}

/*! \internal */
QIcon RibbonPaintManager::standardIconEx(QStyle::StandardPixmap px, const QStyleOption *opt, const QWidget *wd,
                                         bool &ret) const
{
    Q_UNUSED(px);
    Q_UNUSED(opt);
    Q_UNUSED(wd);
    ret = false;
    return QIcon();
}

// for RibbonBar
/*! \internal */
void RibbonPaintManager::drawRibbonBar(const QStyleOption *option, QPainter *p, const QWidget *widget) const
{
    Q_UNUSED(widget)
    if (const StyleOptionRibbon *optRibbonBar = qstyleoption_cast<const StyleOptionRibbon *>(option)) {
        p->fillRect(optRibbonBar->rect, ribbonBarColor(optRibbonBar->isBackstageVisible));

        if (!optRibbonBar->pixTitleBackground.isNull()) {
            QRect rect = optRibbonBar->rect;
            QRect rectSrc = optRibbonBar->pixTitleBackground.rect();
            QRect rectDst = rect;
            rectDst.setLeft(rectDst.right() - rectSrc.width() + 1);
            {
                QRect rt(rectDst);
                rectDst.intersects(rect);

                if (0 < rectDst.width() && rectDst.width() != rectSrc.width()) {
                    rectSrc.setLeft(rectSrc.left() + (rectDst.left() - rt.left()));
                    rectSrc.setRight(rectSrc.left() + qMin(rectDst.width(), rectSrc.width()));
                }
            }

            rectDst.setBottom(rectDst.top() + rectSrc.height() - 1);
            {
                QRect rt(rectDst);
                rectDst.intersects(rect);

                if (0 < rectDst.height() && rectDst.height() != rectSrc.height()) {
                    rectSrc.setTop(rectSrc.top() + (rectDst.top() - rt.top()));
                    rectSrc.setBottom(rectSrc.top() + qMin(rectDst.height(), rectSrc.height()));
                }
            }
            drawPixTitleBackground(p, rectDst, optRibbonBar->pixTitleBackground, rectSrc, widget);
        }
    }
}

/*! \internal */
void RibbonPaintManager::drawPixTitleBackground(QPainter *p, const QRect &rectDst, const QPixmap &pixTitleBackground,
                                                const QRect &rectSrc, const QWidget *widget) const
{
    Q_UNUSED(widget);

    if (theme() == OfficeStyle::Office2013Dark) {
        QImage image = pixTitleBackground.toImage();
        QColor color(0, 0, 0);
        QColor clrDest(240, 240, 240);
        QImage imageMask = DrawHelpers::addaptColors(image, color.rgb(), clrDest.rgb(), false);
        imageMask = DrawHelpers::updateImage(imageMask, color.rgb(), clrDest.rgb());
        image.setAlphaChannel(imageMask);
        p->drawImage(rectDst, image, rectSrc);
    } else
        p->drawPixmap(rectDst, pixTitleBackground, rectSrc);
}

/*! \internal */
void RibbonPaintManager::drawRibbonTabBar(const QStyleOption *opt, QPainter *p, const QWidget *w) const
{
    Q_UNUSED(w);
    QTC_D_STYLE(RibbonStyle)

    if (d->m_clrRibbonTabBarBackground != d->m_clrRibbonFace)
        p->fillRect(opt->rect,
                    d->m_clrRibbonTabBarBackground.isValid() ? d->m_clrRibbonTabBarBackground : d->m_clrRibbonFace);
}

/*! \internal */
void RibbonPaintManager::drawRibbonGroups(const QStyleOption *option, QPainter *p, const QWidget *widget) const
{
    Q_UNUSED(widget);
    QTC_D_STYLE(RibbonStyle)
    if (const StyleOptionRibbon *optGroups = qstyleoption_cast<const StyleOptionRibbon *>(option)) {
        p->fillRect(optGroups->rect, m_clrRibbonGroupsFill);

        QRect rectGroups = optGroups->rect;
        QRect rectActiveTab;
        if (const RibbonBar *rb = qobject_cast<const RibbonBar *>(widget)) {
            QList<RibbonTabBar *> l = widget->findChildren<RibbonTabBar *>();
            if (l.size() > 0) {
                RibbonTabBar *tabBar = l[0];
                if (RibbonTab *tab = tabBar->getTab(tabBar->currentIndex())) {
                    rectActiveTab = tab->rect();
                    QPoint pntrb(tab->mapTo((QWidget *)rb, rectActiveTab.topLeft()));
                    rectActiveTab.translate(pntrb);
                }
            }
        }

        if (d->m_isActiveTabAccented) {
            QPen saveTopPen = p->pen();
            p->setPen(QPen(d->m_clrAccent, d->m_lineWidthTop));

            QPainterPath pathTop;
            pathTop.moveTo(rectGroups.left(), rectGroups.top() + 1);
            pathTop.lineTo(rectActiveTab.left() + 1, rectGroups.top() + 1);
            pathTop.lineTo(rectActiveTab.right() - 2, rectGroups.top() + 1);
            pathTop.lineTo(rectGroups.right() - 1, rectGroups.top() + 1);
            p->drawPath(pathTop);
            p->setPen(saveTopPen);

            QPainterPath path;
            path.moveTo(rectGroups.right() - 1, rectGroups.top() + 1);
            path.lineTo(rectGroups.right() - 1, rectGroups.bottom() - 1);
            path.lineTo(rectGroups.left(), rectGroups.bottom() - 1);
            path.lineTo(rectGroups.left(), rectGroups.top());

            QPen savePen = p->pen();
            p->setPen(d->m_clrRibbonSeparator);
            p->drawPath(path);
            p->setPen(savePen);
        } else {
            QPainterPath path;
            if (optGroups->tabBarPosition == RibbonBar::TabBarTopPosition) {
                path.moveTo(rectGroups.left(), rectGroups.top());
                path.lineTo(rectActiveTab.left() + 1, rectGroups.top());
                path.lineTo(rectActiveTab.right() - 2, rectGroups.top());
                path.lineTo(rectGroups.right() - 1, rectGroups.top());
                path.lineTo(rectGroups.right() - 1, rectGroups.bottom() - 1);
                path.lineTo(rectGroups.left(), rectGroups.bottom() - 1);
                path.lineTo(rectGroups.left(), rectGroups.top());
            } else if (optGroups->tabBarPosition == RibbonBar::TabBarBottomPosition) {
                path.moveTo(rectGroups.left(), rectGroups.bottom() - 1);
                path.lineTo(rectActiveTab.left(), rectGroups.bottom() - 1);
                path.moveTo(rectActiveTab.right(), rectGroups.bottom() - 1);
                path.lineTo(rectGroups.right() - 1, rectGroups.bottom() - 1);
                path.lineTo(rectGroups.right() - 1, rectGroups.top());
                path.lineTo(rectGroups.left(), rectGroups.top());
                path.lineTo(rectGroups.left(), rectGroups.bottom());
            }
            QPen savePen = p->pen();
            p->setPen(d->m_clrRibbonSeparator);
            p->drawPath(path);
            p->setPen(savePen);
        }
    }
}

/*! \internal */
void RibbonPaintManager::drawGroup(const QStyleOption *opt, QPainter *p, const QWidget *widget) const
{
    Q_UNUSED(widget)
    QTC_D_STYLE(RibbonStyle)
    QRect rect = opt->rect;
    QPen savePen = p->pen();
    p->setPen(d->m_clrRibbonSeparator);
    p->drawLine(QPoint(rect.right() - 1, rect.top()), QPoint(rect.right() - 1, rect.bottom()));
    p->setPen(savePen);
}

/*! \internal */
void RibbonPaintManager::drawReducedGroup(const QStyleOption *opt, QPainter *p, const QWidget *widget) const
{
    QTC_D_STYLE(RibbonStyle)
    if (const QStyleOptionToolButton *optGroup = qstyleoption_cast<const QStyleOptionToolButton *>(opt)) {
        QRect rcEntry(opt->rect);
        if (optGroup->features & QStyleOptionToolButton::HasMenu)
            p->fillRect(rcEntry.adjusted(0, 0, -1, 0), m_clrBarShadow);
        else if (opt->state & QStyle::State_MouseOver)
            p->fillRect(rcEntry, DrawHelpers::pixelAlpha(m_clrBarShadow.rgb(), 103));

        QPen savePen = p->pen();
        p->setPen(d->m_clrRibbonSeparator);
        p->drawLine(QPoint(rcEntry.right() - 1, rcEntry.top()), QPoint(rcEntry.right() - 1, rcEntry.bottom()));
        p->setPen(savePen);

        // ----
        QFontMetrics fm(optGroup->fontMetrics);
        int flags = Qt::TextSingleLine;

        flags |= Qt::TextShowMnemonic;
        if (!baseStyle()->proxy()->styleHint(QStyle::SH_UnderlineShortcut, optGroup, widget))
            flags |= Qt::TextHideMnemonic;

        if (!optGroup->icon.isNull()) {
            QPixmap pm;
            QSize pmSize = optGroup->iconSize;

            QIcon::State stateIcon = optGroup->state & QStyle::State_On ? QIcon::On : QIcon::Off;
            QIcon::Mode mode;
            if (!(optGroup->state & QStyle::State_Enabled))
                mode = QIcon::Disabled;
            else if ((opt->state & QStyle::State_MouseOver) && (opt->state & QStyle::State_AutoRaise))
                mode = QIcon::Active;
            else
                mode = QIcon::Normal;

            pm = optGroup->icon.pixmap(optGroup->rect.size().boundedTo(optGroup->iconSize), mode, stateIcon);
            pmSize = pm.size();

            QRect rectImage = opt->rect;
            if (pmSize.width() < 20) {
                rectImage.setTop(rectImage.top() + 10);
                rectImage.setBottom(rectImage.top() + pmSize.height());

                rectImage.setLeft(rectImage.center().x() - pmSize.width() / 2);
                rectImage.setRight(rectImage.left() + pmSize.width());
                rectImage.adjust(-5, -5, 5, 5);
                p->fillRect(rectImage, m_clrBarFace);

                QPen savePen = p->pen();
                p->setPen(d->m_clrMenuPopupSeparator);
                p->drawRect(rectImage);
                p->setPen(savePen);
            }
            baseStyle()->proxy()->drawItemPixmap(p, rectImage, Qt::AlignCenter, pm);
        }

        //===========================================================================================
        QString strFirstRow, strSecondRow;
        CommonStyle::splitString(optGroup->text, strFirstRow, strSecondRow);

        int textHeightTotal = optGroup->rect.height() * 0.5;

        QRect rcText = rcEntry;
        rcText.adjust(4, textHeightTotal - 3, -4, 0);

        QRect rectFirstRow = rcText;
        if (!strFirstRow.isEmpty()) {
            int textHeight = optGroup->fontMetrics.boundingRect(strFirstRow).height() + optGroup->fontMetrics.descent();
            rectFirstRow.setHeight(textHeight + 3);
            baseStyle()->proxy()->drawItemText(p, rectFirstRow, flags | Qt::AlignHCenter, optGroup->palette,
                                               optGroup->state & QStyle::State_Enabled, strFirstRow,
                                               QPalette::WindowText);
        }

        QRect rectSecondRow = rectFirstRow;
        if (!strSecondRow.isEmpty()) {
            rectSecondRow.setTop(rectFirstRow.bottom());
            int textHeight =
                optGroup->fontMetrics.boundingRect(strSecondRow).height() + optGroup->fontMetrics.descent();
            rectSecondRow.setHeight(textHeight);
            baseStyle()->proxy()->drawItemText(p, rectSecondRow, flags | Qt::AlignLeft, optGroup->palette,
                                               optGroup->state & QStyle::State_Enabled, strSecondRow,
                                               QPalette::WindowText);
        }
        //===========================================================================================
        OfficePaintManager::ImageIcons index = OfficePaintManager::Icon_ArowDown;
        OfficePaintManager::ImageState state = OfficePaintManager::Black2;

#if QT_VERSION < QT_VERSION_CHECK(5, 11, 0)
        int width = opt->fontMetrics.width(strSecondRow);
#else
        int width = opt->fontMetrics.horizontalAdvance(strSecondRow);
#endif
        QPoint pt = QPoint(strSecondRow.isEmpty() ? rcText.center().x() - 4 : rectSecondRow.left() + width + 1,
                           strSecondRow.isEmpty() ? rectFirstRow.bottom()
                                                  : (rectSecondRow.top() + (rectSecondRow.height() - 9) / 2) + 1);

        bool enabled = opt->state & QStyle::State_Enabled;
        if (!enabled)
            state = OfficePaintManager::LtGray;
        else if (optGroup->features & QStyleOptionToolButton::HasMenu) {
            drawIconByColor(p, index, pt, m_clrHighlightDn);
            return;
        }
        drawIcon(p, pt, index, state, QSize(9, 9));
    }
}

// for TitleBar
/*! \internal */
bool RibbonPaintManager::drawToolBar(const QStyleOption *opt, QPainter *p, const QWidget *w) const
{
    if (qstyleoption_cast<const QStyleOptionToolBar *>(opt)) {
        return OfficePaintManager::drawToolBar(opt, p, w);
    } else if (qstyleoption_cast<const StyleRibbonQuickAccessBar *>(opt)) {
        return true;
    }
    return OfficePaintManager::drawToolBar(opt, p, w);
}

/*! \internal */
bool RibbonPaintManager::drawIndicatorToolBarSeparator(const QStyleOption *opt, QPainter *p,
                                                       const QWidget *widget) const
{
    if (qobject_cast<const RibbonGroup *>(widget)) {
        if (const QStyleOptionGroupBox *optGroup = qstyleoption_cast<const QStyleOptionGroupBox *>(opt)) {
            QTC_D_STYLE(RibbonStyle)
            QRect rect = opt->rect;

            if (optGroup->text.isEmpty()) {
                QPainterPath path;
                path.moveTo(rect.center().x(), rect.top());
                path.lineTo(rect.center().x(), rect.bottom());

                QPen savePen = p->pen();
                p->setPen(d->m_clrRibbonSeparator);
                p->drawPath(path);
                p->setPen(savePen);
            } else {
                QString titleText = optGroup->text;
                QRect rectText = optGroup->fontMetrics.boundingRect(titleText);
                if (optGroup->textAlignment == Qt::AlignBottom) {
                    QPainterPath path;
                    path.moveTo(rect.center().x(), rect.top());
                    path.lineTo(rect.center().x(), rect.bottom() - rectText.width() - 4);

                    p->save();
                    p->setPen(d->m_clrRibbonSeparator);
                    p->drawPath(path);
                    QSize s = rect.size();
                    s.transpose();
                    rect.setSize(s);
                    p->translate(rect.left() - 1, rect.top() + rect.width());
                    p->rotate(-90);
                    p->translate(-rect.left() + 1, -rect.top());
                    int indent = 0;
                    QPalette pal;
                    pal.setColor(QPalette::WindowText, d->m_clrRibbonSeparator.darker());
                    baseStyle()->proxy()->drawItemText(p, rect.adjusted(indent + 1, 1, -indent - 1, -1),
                                                       Qt::AlignLeft | Qt::AlignVCenter | Qt::TextShowMnemonic, pal,
                                                       optGroup->state & QStyle::State_Enabled, titleText,
                                                       QPalette::WindowText);
                    p->restore();
                } else if (optGroup->textAlignment == Qt::AlignTop) {
                    QPainterPath path;
                    path.moveTo(rect.center().x(), rect.top() + rectText.width());
                    path.lineTo(rect.center().x(), rect.bottom());

                    p->save();
                    p->setPen(d->m_clrRibbonSeparator);
                    p->drawPath(path);
                    QSize s = rect.size();
                    s.transpose();
                    rect.setSize(s);
                    p->translate(rect.left() - 1, rect.top() + rect.width());
                    p->rotate(-90);
                    p->translate(-rect.left() + 1, -rect.top());
                    int indent = 0;
                    QPalette pal;
                    pal.setColor(QPalette::WindowText, d->m_clrRibbonSeparator.darker());
                    baseStyle()->proxy()->drawItemText(
                        p, rect.adjusted(indent + rect.width() - rectText.width() - 2, 1, -indent - 1, -1),
                        Qt::AlignLeft | Qt::AlignVCenter | Qt::TextShowMnemonic, pal,
                        optGroup->state & QStyle::State_Enabled, titleText, QPalette::WindowText);
                    p->restore();
                } else if (optGroup->textAlignment == Qt::AlignVCenter) {
                    p->save();
                    p->setPen(d->m_clrRibbonSeparator);
                    p->drawLine(rect.center().x(), rect.top() + rectText.width() + rectText.width() + 4,
                                rect.center().x(), rect.bottom());
                    p->drawLine(rect.center().x(), rect.top(), rect.center().x(),
                                rect.bottom() - (rectText.width() + rectText.width() + 4));

                    QSize s = rect.size();
                    s.transpose();
                    rect.setSize(s);
                    p->translate(rect.left() - 1, rect.top() + rect.width());
                    p->rotate(-90);
                    p->translate(-rect.left() + 1, -rect.top());
                    int indent = 0;
                    QPalette pal;
                    pal.setColor(QPalette::WindowText, d->m_clrRibbonSeparator.darker());
                    baseStyle()->proxy()->drawItemText(
                        p, rect.adjusted(indent + (rect.width() - rectText.width()) / 2 - 2, 1, -indent - 1, -1),
                        Qt::AlignLeft | Qt::AlignVCenter | Qt::TextShowMnemonic, pal,
                        optGroup->state & QStyle::State_Enabled, titleText, QPalette::WindowText);
                    p->restore();
                }
            }
            return true;
        }
    } else if (qobject_cast<const RibbonStatusBar *>(widget)) {
        return true;
    }
    return false;
}

bool RibbonPaintManager::drawPanelButtonTool(const QStyleOption *opt, QPainter *p, const QWidget *w) const
{
    if (!w)
        return false;

    if (w && qobject_cast<const RibbonBackstageButton *>(w)) {
        drawPanelBackstageButton(opt, p, w);
        return true;
    }

    if (w && !qobject_cast<const RibbonTitleButton *>(w)) {
        if (const QStyleOptionToolButton *toolbutton = qstyleoption_cast<const QStyleOptionToolButton *>(opt)) {
            bool smallSize = opt->rect.height() < 33;

            RibbonToolBarControl *toolBarControl = qobject_cast<RibbonToolBarControl *>(w->parentWidget());
            if (toolBarControl)
                smallSize = true;

            if (w && qobject_cast<const RibbonButton *>(w)) {
                if (!opt->rect.isEmpty()) {
                    int minIcon = qMin(opt->rect.height(), opt->rect.width());
                    int actualArea = minIcon * minIcon;

                    int index = -1;
                    int curArea = 0;
                    QList<QSize> lSz = toolbutton->icon.availableSizes();
                    if (lSz.count() > 1) {
                        for (int i = 0, count = lSz.count(); count > i; i++) {
                            QSize curSz = lSz[i];
                            int area = curSz.height() * curSz.width();
                            if (actualArea > area) {
                                if (area > curArea)
                                    index = i;
                                curArea = area;
                            }
                        }
                        int iconExtent = index != -1 ? toolbutton->icon.actualSize(lSz[index]).height() : smallSize;
                        smallSize =
                            toolbutton->toolButtonStyle == Qt::ToolButtonTextUnderIcon ? iconExtent < 32 : iconExtent;
                    }
                }
            }

            bool enabled = opt->state & QStyle::State_Enabled;
            bool checked = opt->state & QStyle::State_On;
            bool highlighted = opt->state & QStyle::State_MouseOver;
            bool mouseInSplit =
                opt->state & QStyle::State_MouseOver && toolbutton->activeSubControls & QStyle::SC_ToolButton;
            bool mouseInSplitDropDown =
                opt->state & QStyle::State_MouseOver && toolbutton->activeSubControls & QStyle::SC_ToolButtonMenu;
            bool pressed = opt->state & QStyle::State_Sunken;
            bool popuped =
                (toolbutton->activeSubControls & QStyle::SC_ToolButtonMenu) && (opt->state & QStyle::State_Sunken);

            if (!(toolbutton->features & QStyleOptionToolButton::MenuButtonPopup)) {
                bool autoRaise = toolbutton->state & QStyle::State_AutoRaise;
                if (!autoRaise && !qobject_cast<const QStatusBar *>(w->parentWidget())) {
                    QStyleOptionButton tmpBtn;   // = *btn;
                    tmpBtn.state = toolbutton->state;
                    tmpBtn.rect = toolbutton->rect;
                    baseStyle()->proxy()->drawPrimitive(QStyle::PE_PanelButtonCommand, &tmpBtn, p, w);
                } else {
                    QColor color;
                    if (!enabled)
                        color = QColor();

                    else if (popuped)
                        color = buttonHiliteColor(w, true /*highlightDn*/);
                    else if (checked && !highlighted && !pressed)
                        color = buttonHiliteColor(w, true /*highlightDn*/);
                    else if (pressed)
                        color = buttonHiliteColor(w, true /*highlightDn*/);
                    else if (highlighted)
                        color = buttonHiliteColor(w, false /*highlight*/);

                    if (color.isValid())
                        p->fillRect(opt->rect, color);

                    if (checked && highlighted) {
                        QColor colorDn = buttonHiliteColor(w, true /*highlightDn*/);
                        DrawHelpers::draw3dRectEx(*p, opt->rect, colorDn, colorDn);
                    }
                }
                return true;
            }

            QRect rcButton = opt->rect;
            QRect popupr = baseStyle()->subControlRect(QStyle::CC_ToolButton, toolbutton, QStyle::SC_ToolButtonMenu, w);

            if (highlighted) {
                QPen savePen = p->pen();
                p->setPen(buttonHiliteColor(w, pressed /*highlightDn*/));
                p->drawRect(rcButton.adjusted(0, 0, -1, -1));
                p->setPen(savePen);
            }

            if (mouseInSplit || checked) {
                QColor colorSplit;
                if (!enabled) {
                    colorSplit = QColor();
                } else if (checked) {
                    if (popuped)
                        colorSplit = buttonHiliteColor(w, true /*highlightDn*/);
                    else if (highlighted && !pressed && !checked)
                        colorSplit = buttonHiliteColor(w, false /*highlight*/);
                    else if (highlighted && pressed)
                        colorSplit = buttonHiliteColor(w, true /*highlightDn*/);
                    else if (pressed || checked)
                        colorSplit = buttonHiliteColor(w, true /*highlightDn*/);
                    else if (highlighted || pressed)
                        colorSplit = !mouseInSplit ? buttonHiliteColor(w, true /*highlightDn*/) : QColor();
                } else {
                    if (popuped)
                        colorSplit = buttonHiliteColor(w, true /*highlightDn*/);
                    else if (pressed)
                        colorSplit = buttonHiliteColor(w, true /*highlightDn*/);
                    else if (highlighted)
                        colorSplit = buttonHiliteColor(w, false /*highlight*/);
                }
                if (colorSplit.isValid()) {
                    QRect rcSplitUp = smallSize
                                          ? QRect(QPoint(rcButton.left(), rcButton.top()),
                                                  QPoint(rcButton.right() - popupr.width() - 2, rcButton.bottom()))
                                          : QRect(QPoint(rcButton.left(), rcButton.top()),
                                                  QPoint(rcButton.right(), rcButton.bottom() - popupr.height() - 2));
                    p->fillRect(rcSplitUp, colorSplit);
                }
            }

            if (mouseInSplitDropDown || popuped || checked) {
                if (!popuped) {
                    QRect rcSplitDown = smallSize
                                            ? QRect(QPoint(rcButton.right() - popupr.width() - 1, rcButton.top()),
                                                    QPoint(rcButton.right(), rcButton.bottom()))
                                            : QRect(QPoint(rcButton.left(), rcButton.bottom() - popupr.height() - 1),
                                                    QPoint(rcButton.right(), rcButton.bottom()));

                    QColor colorSplit = buttonHiliteColor(w, pressed /*highlightDn*/);
                    p->fillRect(rcSplitDown, colorSplit);
                } else if (popuped || checked)
                    p->fillRect(rcButton, buttonHiliteColor(w, true /*highlightDn*/));
            }
            return true;
        }
    } else if (w && qobject_cast<const RibbonTitleButton *>(w)) {
        if (const StyleCaptionButton *toolbutton = qstyleoption_cast<const StyleCaptionButton *>(opt)) {
            bool down = toolbutton->state & QStyle::State_Sunken;
            bool highlighted = toolbutton->state & QStyle::State_MouseOver;

            if (down || highlighted)
                p->fillRect(toolbutton->rect, buttonHiliteColor(w, down /*highlightDn*/));

            QRect rectImage = toolbutton->rect;
            QSize sizeImage = QSize(9, 9);
            QPoint ptImage(rectImage.left() + (rectImage.width() - sizeImage.width()) / 2 +
                               ((rectImage.width() - sizeImage.width()) % 2),
                           rectImage.top() + (rectImage.height() - sizeImage.height()) / 2 +
                               ((rectImage.height() - sizeImage.height()) % 2));

            QColor colText = titleButtonHiliteColor(!(toolbutton->state & QStyle::State_Enabled), w);

            ImageState state = highlighted ? Black2 : Black;
            if (!(toolbutton->state & QStyle::State_Enabled))
                state = Gray;
            else if (colText.isValid()) {
                if (colText.red() <= 128 || colText.green() <= 128 || colText.blue() <= 128)
                    state = highlighted ? Black2 : Black;
                else
                    state = White;
            }

            ImageIcons index;
            if (toolbutton->activeSubControls == QStyle::SC_TitleBarCloseButton)
                index = Icon_Close;
            else if (toolbutton->activeSubControls == QStyle::SC_TitleBarMaxButton)
                index = Icon_Maximize;
            else if (toolbutton->activeSubControls == QStyle::SC_TitleBarMinButton)
                index = Icon_Minimize;
            else if (toolbutton->activeSubControls == QStyle::SC_TitleBarNormalButton)
                index = Icon_Restore;
            else {
                Q_ASSERT(false);
                return false;
            }

            drawIcon(p, ptImage, index, state, sizeImage);

            return true;
        }
    }

    return false;
}

/*! \internal */
void RibbonPaintManager::drawContextHeaders(const QStyleOption *opt, QPainter *p) const
{
    if (const StyleOptionTitleBar *optTitleBar = qstyleoption_cast<const StyleOptionTitleBar *>(opt)) {
        const QList<ContextHeader *> &listContextHeaders = *optTitleBar->listContextHeaders;
        Q_FOREACH(const ContextHeader *header, listContextHeaders)
        {
            QRect rc = header->rcRect;
            rc.translate(0, 2);
            QColor color = qtc_getColorToRGB(header->color);

            QColor clrDark = DrawHelpers::colorMakeDarker(color.rgb());
            QColor clrLight = DrawHelpers::colorMakePale(color.rgb());

            p->fillRect(rc, clrLight);

            QRect rectTop = header->rcRect;
            rectTop.setBottom(rectTop.top() + 6);

            p->fillRect(rectTop, clrDark);

            rc.setLeft(rc.left() + 4);

            int frameBorder = optTitleBar->frameHelper ? optTitleBar->frameHelper->frameBorder() : 0;

            rc.setTop(rc.top() + (frameBorder - 2));

            QString text = header->strTitle.toUpper();

            QFont saveFont = p->font();
            p->setFont(header->firstTab->font());
            QPen savePen = p->pen();
            p->setPen(DrawHelpers::colorMakeDarker(color.rgb(), .4));
            p->drawText(rc, Qt::AlignCenter | Qt::TextSingleLine,
                        p->fontMetrics().elidedText(text, Qt::ElideRight, rc.width()));
            p->setPen(savePen);
            p->setFont(saveFont);
        }
    }
}

// for Slider
/*! \internal */
bool RibbonPaintManager::drawSlider(const QStyleOptionComplex *opt, QPainter *p, const QWidget *w) const
{
    if (!qobject_cast<RibbonSliderPane *>(w->parentWidget()))
        return false;

    if (const QStyleOptionSlider *slider = qstyleoption_cast<const QStyleOptionSlider *>(opt)) {
        if (slider->orientation == Qt::Horizontal) {
            QRect groove = baseStyle()->proxy()->subControlRect(QStyle::CC_Slider, opt, QStyle::SC_SliderGroove, w);
            QRect handle = baseStyle()->proxy()->subControlRect(QStyle::CC_Slider, opt, QStyle::SC_SliderHandle, w);

            QRect rcTrackDest(QPoint(groove.left(), groove.center().y()), QSize(groove.width(), 2));
            rcTrackDest.adjust(3, 0, -2, 0);
            p->fillRect(rcTrackDest, m_clrBarFace);
            DrawHelpers::draw3dRectEx(*p, rcTrackDest, m_clrBarDkShadow, m_clrBarDkShadow);

            if ((opt->subControls & QStyle::SC_SliderHandle)) {
                bool enabled = opt->state & QStyle::State_Enabled;
                bool selected =
                    slider->activeSubControls & QStyle::SC_SliderHandle && opt->state & QStyle::State_MouseOver;
                bool pressetHt = slider->state & QStyle::State_Sunken;

                QColor clrLine = enabled ? m_clrBarDkShadow : m_clrBarShadow;
                QColor clrFill = m_clrBarFace;

                if (enabled && (pressetHt || selected)) {
                    clrFill = m_clrAccentLight;
                    clrLine = m_clrHighlightDn;
                }

                handle.adjust(2, 0, -2, -2);
                handle.setLeft(handle.center().x() - 2);
                handle.setRight(handle.center().x() + 2);

                p->fillRect(handle, clrFill);

                QPen savePen = p->pen();
                p->setPen(clrLine);
                p->drawRect(handle);
                p->setPen(savePen);
            }
            return true;
        }
    }
    return false;
}

/*! \internal */
void RibbonPaintManager::drawGroupScrollButton(const QStyleOption *option, QPainter *p, const QWidget *widget) const
{
    Q_UNUSED(widget)
    if (const QStyleOptionToolButton *toolbutton = qstyleoption_cast<const QStyleOptionToolButton *>(option)) {
        QTC_D_STYLE(const RibbonStyle)
        QRect rc = toolbutton->rect;

        bool enabled = toolbutton->state & QStyle::State_Enabled;
        bool highlighted = toolbutton->state & QStyle::State_MouseOver;
        bool pressed = toolbutton->state & QStyle::State_Sunken;

        if (pressed)
            p->fillRect(rc, m_clrHighlightDn);
        else if (pressed || highlighted)
            p->fillRect(rc, d->m_clrHighlight);
        else
            p->fillRect(rc, QColor(255, 255, 255));

        QColor clrBorder = !enabled ? m_clrBarShadow : (highlighted || pressed) ? m_clrHighlightDn : m_clrBarDkShadow;

        QPen oldPen = p->pen();
        p->setPen(clrBorder);
        p->drawRect(rc.adjusted(0, -1, -1, -2));
        p->setPen(oldPen);

        ImageIcons iImage = toolbutton->arrowType == Qt::LeftArrow ? OfficePaintManager::Icon_ArowLeft
                                                                   : OfficePaintManager::Icon_ArowRight;

        OfficePaintManager::ImageState imageState = OfficePaintManager::Gray;
        if (!enabled)
            imageState = OfficePaintManager::LtGray;
        else if (highlighted)
            imageState = OfficePaintManager::DkGray;

        QRect rcIndicator(QPoint(rc.x() + (rc.width() - 9) / 2, rc.y() + (rc.height() - 9) / 2), QSize(9, 9));
        drawIcon(p, rcIndicator, iImage, imageState, QSize(9, 9));
    }
}

// for menuBar
/*! \internal */
bool RibbonPaintManager::drawFrameMenu(const QStyleOption *opt, QPainter *p, const QWidget *w) const
{
    QTC_D_STYLE(const RibbonStyle)
    if (qobject_cast<const RibbonSystemPopupBar *>(w) || qobject_cast<const RibbonPageSystemPopup *>(w)) {
        QRect rect = opt->rect;
        DrawHelpers::draw3dRectEx(*p, rect, d->m_clrMenubarBorder, d->m_clrMenubarBorder);
        rect.adjust(1, 1, -1, -1);
        DrawHelpers::draw3dRectEx(*p, rect, d->m_clrBackgroundLight, d->m_clrBackgroundLight);
        QRect rectLeft(1, 1, 2, rect.bottom() - 1);
        p->fillRect(rectLeft, m_clrBarHilite);

        QRect rectFill = opt->rect;
        rectFill.adjust(1, 1, -1, -1);
        p->fillRect(rectFill, m_clrBarLight);
        return true;
    }
    return OfficePaintManager::drawFrameMenu(opt, p, w);
}

/*! \internal */
void RibbonPaintManager::drawTabShape(const QStyleOption *opt, QPainter *p, const QWidget *w) const
{
    QTC_D_STYLE(RibbonStyle)
    if (const StyleRibbonOptionHeader *optTab = qstyleoption_cast<const StyleRibbonOptionHeader *>(opt)) {
        if (optTab->state & QStyle::State_Selected) {
            QRect rectTab = optTab->rect;
            p->fillRect(rectTab, d->m_isActiveTabAccented
                                     ? (optTab->state & QStyle::State_MouseOver ? m_clrAccentHilight : d->m_clrAccent)
                                     : m_clrRibbonGroupsFill);

            QPainterPath path;
            if (optTab->tabBarPosition == RibbonBar::TabBarTopPosition) {
                path.moveTo(rectTab.left(), rectTab.bottom());
                path.lineTo(rectTab.left(), rectTab.top());
                path.lineTo(rectTab.right(), rectTab.top());
                path.lineTo(rectTab.right(), rectTab.bottom());
            } else if (optTab->tabBarPosition == RibbonBar::TabBarBottomPosition) {
                path.moveTo(rectTab.left(), rectTab.top());
                path.lineTo(rectTab.left(), rectTab.bottom());
                path.lineTo(rectTab.right(), rectTab.bottom());
                path.lineTo(rectTab.right(), rectTab.top());
            } else {
                return;
            }

            QPen savePen = p->pen();
            p->setPen(d->m_clrRibbonSeparator);
            p->drawPath(path);
            p->setPen(savePen);
        } else
            fillRibbonTabControl(optTab, p, w);

        if (optTab->state & QStyle::State_Small) {
            QRect rectTab = optTab->rect;
            QPainterPath path;
            path.moveTo(rectTab.right(), rectTab.top());
            path.lineTo(rectTab.right(), rectTab.bottom());
            QPen savePen = p->pen();
            p->setPen(d->m_clrRibbonSeparator);
            p->drawPath(path);
            p->setPen(savePen);
        }
    }
}

/*! \internal */
void RibbonPaintManager::drawTabShapeLabel(const QStyleOption *opt, QPainter *p, const QWidget *w) const
{
    QTC_D_STYLE(RibbonStyle)
    if (const QStyleOptionHeader *optTab = qstyleoption_cast<const QStyleOptionHeader *>(opt)) {
        bool selected = opt->state & QStyle::State_Selected;
        bool highlighted = opt->state & QStyle::State_MouseOver;
        int flags = optTab->textAlignment;

        flags |= Qt::TextShowMnemonic;
        if (!baseStyle()->proxy()->styleHint(QStyle::SH_UnderlineShortcut, opt, w))
            flags |= Qt::TextHideMnemonic;

        RibbonPage::ContextColor pageColor = RibbonPage::ContextColorNone;
        if (const RibbonTab *tab = qobject_cast<const RibbonTab *>(w))
            pageColor = tab->contextColor();

        QColor textColor = d->m_clrTabNormalText;
        if (d->m_clrRibbonTabBarText.isValid())
            textColor = d->m_clrRibbonTabBarText;

        if (selected && pageColor != RibbonPage::ContextColorNone) {
            textColor = ::qtc_getColorToRGB(pageColor);
            textColor = DrawHelpers::colorMakeDarker(textColor.rgb(), .4);
        } else if (selected || highlighted)
            textColor = d->m_clrTabSelectedText;

        if (d->m_isActiveTabAccented && selected)
            textColor = m_clrRibbonGroupsFill;

        QString text = optTab->text;
        if (d->ribbonPaintManager()->isTopLevelMenuItemUpperCase(w))
            text = text.toUpper();

        // draw text
        QPalette pal = optTab->palette;
        pal.setColor(QPalette::WindowText, textColor);
        baseStyle()->proxy()->drawItemText(p, optTab->rect, flags, pal, opt->state & QStyle::State_Enabled, text,
                                           QPalette::WindowText);
    }
}

// for TabBar
/*! \internal */
bool RibbonPaintManager::drawTabBarTabShape(const QStyleOption *opt, QPainter *p, const QWidget *w) const
{
    return OfficePaintManager::drawTabBarTabShape(opt, p, w);
}

/*! \internal */
bool RibbonPaintManager::drawShapedFrame(const QStyleOption *opt, QPainter *p, const QWidget *w) const
{
    return OfficePaintManager::drawShapedFrame(opt, p, w);
}

/*! \internal */
void RibbonPaintManager::drawSystemButton(const QStyleOption *option, QPainter *p, const QWidget *widget) const
{
    QTC_D_STYLE(const RibbonStyle)
    if (const StyleSystemToolButton *toolbutton = qstyleoption_cast<const StyleSystemToolButton *>(option)) {
        if (qobject_cast<const RibbonBar *>(widget->parentWidget())) {
            if (toolbutton->toolButtonStyle != Qt::ToolButtonFollowStyle) {
                bool popuped = (toolbutton->activeSubControls & QStyle::SC_ToolButtonMenu) &&
                               (option->state & QStyle::State_Sunken);
                bool isHighlighted = option->state & QStyle::State_MouseOver;
                bool enabled = widget->isEnabled();

                bool usePalette = false;
                uint resolve_mask = toolbutton->palette.resolve();
                if (resolve_mask & (1 << QPalette::Button))
                    usePalette = true;

                if (usePalette) {
                    QBrush brButton = toolbutton->palette.brush(QPalette::Active, QPalette::Button);
                    QBrush brButtonDisabled = toolbutton->palette.brush(QPalette::Disabled, QPalette::Button);
                    QBrush brHighlightButton = (resolve_mask & (1 << QPalette::Highlight))
                                                   ? toolbutton->palette.brush(QPalette::Active, QPalette::Highlight)
                                                   : brButton;
                    QBrush brPopupButton = (resolve_mask & (1 << QPalette::Dark))
                                               ? toolbutton->palette.brush(QPalette::Active, QPalette::Dark)
                                               : brButton;

                    QBrush brush = !enabled        ? brButtonDisabled
                                   : popuped       ? (isHighlighted ? brHighlightButton : brPopupButton)
                                   : false         ? brButton
                                   : isHighlighted ? brHighlightButton
                                                   : brButton;
                    p->fillRect(toolbutton->rect, brush);
                } else {
                    p->fillRect(toolbutton->rect, isHighlighted ? m_clrAccentHilight : d->m_clrAccent);
                }

                if (toolbutton->toolButtonStyle == Qt::ToolButtonIconOnly && !toolbutton->icon.isNull()) {
                    bool enabled = toolbutton->state & QStyle::State_Enabled;
                    QRect rc = toolbutton->rect;
                    QPixmap pxIcon =
                        toolbutton->icon.pixmap(toolbutton->iconSize, enabled ? QIcon::Normal : QIcon::Disabled,
                                                isHighlighted ? QIcon::On : QIcon::Off);
                    QPoint ptIcon((rc.left() + rc.right() - pxIcon.width()) / 2,
                                  (rc.top() + rc.bottom() + 1 - pxIcon.height()) / 2);
                    p->drawPixmap(ptIcon, pxIcon);
                }
                return;
            }
        }

        bool isHighlighted = option->state & QStyle::State_MouseOver;
        if ((toolbutton->activeSubControls & QStyle::SC_ToolButtonMenu) && (toolbutton->state & QStyle::State_Sunken))
            isHighlighted = true;

        p->fillRect(toolbutton->rect.adjusted(0, 0, -1, -1), isHighlighted ? m_clrAccentHilight : d->m_clrAccent);
        if (!toolbutton->icon.isNull()) {
            QRect rc = toolbutton->rect;
            bool enabled = toolbutton->state & QStyle::State_Enabled;
            bool selected = toolbutton->state & QStyle::State_MouseOver;
            QPixmap pxIcon = toolbutton->icon.pixmap(toolbutton->iconSize, enabled ? QIcon::Normal : QIcon::Disabled,
                                                     selected ? QIcon::On : QIcon::Off);
            QPoint ptIcon((rc.left() + rc.right() - pxIcon.width()) / 2,
                          (rc.top() + rc.bottom() - pxIcon.height()) / 2);
            p->drawPixmap(ptIcon.x(), ptIcon.y(), pxIcon);
        }
    }
}

void RibbonPaintManager::drawQuickAccessButton(const QStyleOption *opt, QPainter *p, const QWidget *w) const
{
    Q_UNUSED(w);
    fillRibbonButton(opt, p, w);
    ImageIcons index = Icon_CustomizeArowDown;
    QRect rect = opt->rect;
    QRect rectWhite = rect;
    rectWhite.adjust(0, 1, 0, 1);
    drawIcon(p, rectWhite, index, LtGray);
    drawIcon(p, rect, index, Black2);
}

/*! \internal */
void RibbonPaintManager::drawOptionButton(const QStyleOption *option, QPainter *p, const QWidget *widget) const
{
    Q_UNUSED(widget);

    fillRibbonButton(option, p, widget);

    bool highlighted = option->state & QStyle::State_MouseOver;
    bool pressed = option->state & QStyle::State_Sunken;
    bool enabled = option->state & QStyle::State_Enabled;
    QRect rect = option->rect;

    OfficePaintManager::ImageState state = OfficePaintManager::Black2;
    OfficePaintManager::ImageIcons index = OfficePaintManager::Icon_LaunchArrow;

    if (!enabled)
        state = OfficePaintManager::LtGray;
    else if (highlighted && pressed) {
        drawIconByColor(p, index, rect, m_clrHighlightDn);
        return;
    }
    drawIcon(p, rect, index, state);
}

/*! \internal */
void RibbonPaintManager::drawPopupResizeGripper(const QStyleOption *opt, QPainter *p, const QWidget *w) const
{
    Q_UNUSED(w);

    QTC_D_STYLE(const RibbonStyle)
    if (const QStyleOptionSizeGrip *sizeGrip = qstyleoption_cast<const QStyleOptionSizeGrip *>(opt)) {
        p->fillRect(sizeGrip->rect, d->m_clrBackgroundLight);

        QPoint pt(sizeGrip->rect.right() - 3, sizeGrip->rect.bottom() - 3);
        for (int y = 0; y < 3; y++) {
            for (int x = 0; x < 3 - y; x++) {
                p->fillRect(QRect(QPoint(pt.x() + 1 - x * 4, pt.y() + 1 - y * 4), QSize(2, 2)), QColor(255, 255, 255));
                p->fillRect(QRect(QPoint(pt.x() + 0 - x * 4, pt.y() + 0 - y * 4), QSize(2, 2)),
                            d->m_clrToolbarSeparator);
            }
        }
    }
}

// for stausBar
/*! \internal */
bool RibbonPaintManager::drawPanelStatusBar(const QStyleOption *opt, QPainter *p, const QWidget *w) const
{
    return OfficePaintManager::drawPanelStatusBar(opt, p, w);
}

// for SizeGrip
/*! \internal */
bool RibbonPaintManager::drawSizeGrip(const QStyleOption *opt, QPainter *p, const QWidget *w) const
{
    return OfficePaintManager::drawSizeGrip(opt, p, w);
}

/*! \internal */
bool RibbonPaintManager::drawIndicatorArrow(QStyle::PrimitiveElement pe, const QStyleOption *opt, QPainter *p,
                                            const QWidget *w) const
{
    if (getParentWidget<RibbonGroup>(w) || getParentWidget<RibbonBar>(w) ||
        qobject_cast<const RibbonBackstageButton *>(w)) {
        switch (pe) {
        case QStyle::PE_IndicatorArrowDown:
            if (qobject_cast<const QToolButton *>(w)) {
                if (!qobject_cast<const QTabBar *>(w->parentWidget())) {
                    bool enabled = opt->state & QStyle::State_Enabled;
                    bool highlighted = opt->state & QStyle::State_Selected;
                    bool pressed = opt->state & QStyle::State_Sunken;

                    OfficePaintManager::ImageIcons index = OfficePaintManager::Icon_ArowDown;
                    OfficePaintManager::ImageState state = OfficePaintManager::Black2;

                    if (!enabled)
                        state = OfficePaintManager::LtGray;
                    else if (highlighted && pressed) {
                        drawIconByColor(p, index, opt->rect, m_clrHighlightDn);
                        return true;
                    }
                    drawIcon(p, opt->rect, index, state);
                    return true;
                }
            }
            break;
        default:
            break;
        }
    }

    return OfficePaintManager::drawIndicatorArrow(pe, opt, p, w);
}

/*! \internal */
void RibbonPaintManager::drawRectangle(QPainter *p, const QRect &rect, bool selected, bool pressed, bool enabled,
                                       bool checked, bool popuped, QRibbon::BarType barType,
                                       QRibbon::BarPosition barPos) const
{
    OfficePaintManager::drawRectangle(p, rect, selected, pressed, enabled, checked, popuped, barType, barPos);
}

/*! \internal */
void RibbonPaintManager::drawKeyTip(const QStyleOption *opt, QPainter *p, const QWidget *w) const
{
    Q_UNUSED(w);
    QTC_D_STYLE(const RibbonStyle)
    p->fillRect(opt->rect, m_clrBarHilite);
    DrawHelpers::draw3dRectEx(*p, opt->rect, d->m_clrRibbonSeparator, d->m_clrRibbonSeparator);
}

/*! \internal */
void RibbonPaintManager::drawBackstage(const QStyleOption *opt, QPainter *p, const QWidget *w) const
{
    Q_UNUSED(w);
    QTC_D_STYLE(const RibbonStyle)
    if (const StyleOptionRibbonBackstage *optBackstage = qstyleoption_cast<const StyleOptionRibbonBackstage *>(opt)) {
        p->fillRect(opt->rect, m_clrBarLight);
        int top =
            (bool)baseStyle()->proxy()->styleHint((QStyle::StyleHint)RibbonStyle::SH_RibbonBackstageHideTabs) ? 0 : 2;
        QRect rc = QRect(QPoint(0, top), QPoint(optBackstage->menuWidth, opt->rect.bottom()));
        p->fillRect(rc, d->m_clrAccent);
    }
}

/*! \internal */
void RibbonPaintManager::drawRibbonBackstageMenu(const QStyleOption *opt, QPainter *p, const QWidget *widget) const
{
    QTC_D_STYLE(const RibbonStyle)
    if (const QStyleOptionMenuItem *menuitem = qstyleoption_cast<const QStyleOptionMenuItem *>(opt)) {
        bool act = menuitem->state & QStyle::State_Selected;
        bool focus = menuitem->state & QStyle::State_HasFocus;
        bool dis = !(menuitem->state & QStyle::State_Enabled);

        QPalette pal = menuitem->palette;

        QColor clrText = dis ? QColor(192, 192, 192) : QColor(255, 255, 255);
        pal.setColor(QPalette::WindowText, clrText);

        QRect rcText(opt->rect);
        rcText.adjust(15, 2, 2, 2);

        if (menuitem->menuItemType == QStyleOptionMenuItem::Separator) {
            int x, y, w, h;
            menuitem->rect.getRect(&x, &y, &w, &h);
            // windows always has a check column, regardless whether we have an icon or not
            const int iconSize = baseStyle()->proxy()->pixelMetric(QStyle::PM_ToolBarIconSize, opt, widget);
            int yoff = (y - 1 + h / 2);
            QPen penSave = p->pen();
            QColor clr = d->m_clrAccent.lighter(130);
            p->setPen(clr);
            p->drawLine(x + 2 + iconSize, yoff, x + w - 4, yoff);
            p->setPen(penSave);
            return;
        }
        if (menuitem->menuItemType == QStyleOptionMenuItem::Normal) {
            if (act || focus)
                p->fillRect(opt->rect, m_clrAccentText);
        } else if (menuitem->menuItemType == QStyleOptionMenuItem::SubMenu) {
            if (act || focus) {
                p->fillRect(opt->rect, m_clrAccentText);
                if (focus)
                    p->fillRect(opt->rect, m_clrAccentHilight);
            }
        }

        int iconWidth = 0;
        int indent = menuitem->menuItemType == QStyleOptionMenuItem::SubMenu ? 6 : 0;
        if (!menuitem->icon.isNull()) {
            int iconSize = baseStyle()->pixelMetric(QStyle::PM_LargeIconSize, opt, widget);
            if (iconSize > qMin(opt->rect.height(), opt->rect.width()))
                iconSize = baseStyle()->pixelMetric(QStyle::PM_SmallIconSize, opt, widget);

            QIcon::Mode mode = dis ? QIcon::Disabled : QIcon::Normal;
            QPixmap pixmap = menuitem->icon.pixmap(iconSize, mode);
            QPoint pmr(rcText.left() + indent, (rcText.top() + rcText.bottom() - pixmap.height()) / 2);
            p->drawPixmap(pmr, pixmap);
            iconWidth = pixmap.width();
        }

        indent += !menuitem->icon.isNull() ? 7 : 0;
        rcText.setLeft(rcText.left() + iconWidth + indent);

        if (!menuitem->text.isEmpty()) {
            int flags = 0;
            flags |= Qt::TextHideMnemonic | Qt::AlignVCenter | Qt::TextSingleLine;
            int index = menuitem->text.indexOf(QLatin1String("&"));
            if (index != -1) {
                index = menuitem->text.indexOf(QLatin1String(" "), index);
                if (index != -1)
                    flags &= ~Qt::TextHideMnemonic;
            }
            // draw text
            baseStyle()->proxy()->drawItemText(p, rcText, flags, pal, opt->state & QStyle::State_Enabled,
                                               menuitem->text, QPalette::WindowText);
        }
    }
}

/*! \internal */
void RibbonPaintManager::drawRibbonBackstageCloseButton(const QStyleOption *opt, QPainter *p, const QWidget *w) const
{
    Q_UNUSED(w);

    p->fillRect(opt->rect, backstageCloseButton());

    if (const QStyleOptionToolButton *optButton = qstyleoption_cast<const QStyleOptionToolButton *>(opt)) {
        QList<QSize> sz = optButton->icon.availableSizes();
        if (!optButton->icon.isNull() && sz.count() > 0) {
            bool enabled = optButton->state & QStyle::State_Enabled;
            bool highlighted = optButton->state & QStyle::State_MouseOver;
            QPixmap pxIcon = optButton->icon.pixmap(
                sz.at(0), enabled ? (highlighted ? QIcon::Selected : QIcon::Active) : QIcon::Disabled, QIcon::On);
            p->drawPixmap(0, 0, pxIcon);
        }
    }
}

/*! \internal */
bool RibbonPaintManager::drawRibbonBackstageSeparator(const QStyleOption *opt, QPainter *p, const QWidget *widget) const
{
    Q_UNUSED(widget);
    bool ret = false;
#if (QT_VERSION < QT_VERSION_CHECK(5, 7, 0))
    if (const QStyleOptionFrameV3 *f = qstyleoption_cast<const QStyleOptionFrameV3 *>(opt))
#else
    if (const QStyleOptionFrame *f = qstyleoption_cast<const QStyleOptionFrame *>(opt))
#endif
    {
        p->fillRect(opt->rect, QColor(255, 255, 255));

        if (f->frameShape == QFrame::HLine) {
            QRect rc(QPoint(opt->rect.left(), opt->rect.height() / 2), QSize(opt->rect.width(), 1));
            p->fillRect(rc, QColor(170, 170, 170));
            ret = true;
        } else if (f->frameShape == QFrame::VLine) {
            QRect rc(QPoint(opt->rect.width() / 2, opt->rect.top()), QSize(1, opt->rect.height()));
            p->fillRect(rc, QColor(170, 170, 170));
            ret = true;
        }
    }
    return ret;
}

/*! \internal */
void RibbonPaintManager::setupPalette(QWidget *widget) const
{
    if (qobject_cast<RibbonKeyTip *>(widget)) {
        QPalette palette = widget->palette();
        palette.setColor(QPalette::ToolTipText, m_clrBarText);
        palette.setColor(QPalette::Disabled, QPalette::ToolTipText, m_clrBarShadow);
        widget->setPalette(palette);
    }
}

/*! \internal */
void RibbonPaintManager::modifyColors()
{
    OfficePaintManager::modifyColors();
    QTC_D_STYLE(RibbonStyle)
    d->m_clrRibbonFace = m_clrBarFace;
    m_clrRibbonGroupsFill = m_clrBarLight;
    d->m_clrRibbonGroupCaptionText = m_clrBarText;
    d->m_clrControlGallerySelected = QColor(255, 255, 255);
    d->m_clrControlGalleryNormal = QColor(255, 255, 255);
    d->m_clrControlGalleryBorder = m_clrBarShadow;
}

/*! \internal */
QColor RibbonPaintManager::titleButtonHiliteColor(bool disabled, const QWidget *w) const
{
    Q_UNUSED(disabled);
    Q_UNUSED(w);
    return QColor();
}

/*! \internal */
QColor RibbonPaintManager::buttonHiliteColor(const QWidget *widget, bool highlightDn) const
{
    Q_UNUSED(widget);
    QTC_D_STYLE(RibbonStyle)
    return highlightDn ? m_clrHighlightDn : d->m_clrHighlight;
}

/*! \internal */
QColor RibbonPaintManager::ribbonBarColor(bool isBackstageVisible) const
{
    QTC_D_STYLE(RibbonStyle)
    return isBackstageVisible ? QColor(255, 255, 255) : d->m_clrRibbonFace;
}

/*! \internal */
QColor RibbonPaintManager::backstageCloseButton() const
{
    QTC_D_STYLE(RibbonStyle)
    return d->m_clrAccent;
}

/*! \internal */
void RibbonPaintManager::drawRibbonSliderButton(const QStyleOption *opt, QPainter *p, const QWidget *w) const
{
    Q_UNUSED(w);
    QTC_D_STYLE(const RibbonStyle)
    bool left = opt->direction != Qt::RightToLeft;

    bool highlighted = opt->state & QStyle::State_MouseOver;
    bool pressed = opt->state & QStyle::State_Sunken;
    bool enabled = opt->state & QStyle::State_Enabled;

    QPoint ptCenter = opt->rect.center();
    int radius = 7;
    QRect rectSign(QPoint(ptCenter.x() - radius / 2, ptCenter.y() - radius / 2), QSize(radius, radius));

    QColor clrLine;
    if (!enabled)
        clrLine = m_clrBarShadow;
    else if (pressed && highlighted)
        clrLine = d->m_clrMenubarBorder;
    else if (highlighted)
        clrLine = d->m_clrMenubarBorder;
    else
        clrLine = w && getParentWidget<const QStatusBar>(w) ? QColor(255, 255, 255) : m_clrBarText;

    QPen savePen = p->pen();
    p->setPen(clrLine);

    QPainterPath path;
    path.moveTo(rectSign.left(), ptCenter.y());
    path.lineTo(rectSign.right(), ptCenter.y());
    if (left) {
        path.moveTo(ptCenter.x(), rectSign.top());
        path.lineTo(ptCenter.x(), rectSign.bottom());
    }
    p->drawPath(path);
    p->setPen(savePen);
}

/*! \internal */
bool RibbonPaintManager::isTopLevelMenuItemUpperCase(const QWidget *w) const
{
    if (getParentWidget<RibbonTabBar>(w))
        return true;
    else if (qobject_cast<const RibbonSystemButton *>(w))
        return true;
    return false;
}

/*! \internal */
void RibbonPaintManager::fillRibbonButton(const QStyleOption *option, QPainter *p, const QWidget *w) const
{
    bool popuped = false;
    if (const QStyleOptionToolButton *toolbutton = qstyleoption_cast<const QStyleOptionToolButton *>(option))
        popuped =
            (toolbutton->activeSubControls & QStyle::SC_ToolButtonMenu) && (toolbutton->state & QStyle::State_Sunken);

    bool highlighted = option->state & QStyle::State_MouseOver;
    bool pressed = option->state & QStyle::State_Sunken;

    if (pressed || highlighted) {
        bool isHighlightDown = (pressed || popuped);
        QColor color = buttonHiliteColor(w, isHighlightDown /*highlightDn*/);
        QRect rectFill = option->rect;
        p->fillRect(rectFill, color);
    }
}

/*! \internal */
void RibbonPaintManager::fillRibbonTabControl(const QStyleOption *opt, QPainter *p, const QWidget *widget) const
{
    if (const RibbonTab *tab = qobject_cast<const RibbonTab *>(widget)) {
        if (ContextHeader *contextHeader = tab->getContextHeader()) {
            if (!contextHeader || !contextHeader->firstTab || !contextHeader->lastTab ||
                contextHeader->color == RibbonPage::ContextColorNone)
                return;

            QColor color = qtc_getColorToRGB(contextHeader->color);

            QColor clrLight = DrawHelpers::colorMakePale(color.rgb());

            QRect rcFirst(contextHeader->firstTab->rect());
            QRect rcLast(contextHeader->lastTab->rect());

            QRect rc(rcFirst.left(), opt->rect.top(), rcLast.right() + 1, rcLast.bottom());
            p->fillRect(rc, clrLight);
        }
    }
}

// for BackstageButton
/*! \internal */
void RibbonPaintManager::drawPanelBackstageButton(const QStyleOption *opt, QPainter *p, const QWidget *w) const
{
    Q_UNUSED(w);
    QTC_D_STYLE(RibbonStyle)
    if (const StyleOptionBackstageButton *btnStyle = qstyleoption_cast<const StyleOptionBackstageButton *>(opt)) {
        // QRect rc = btnStyle->rect;
        bool enabled = opt->state & QStyle::State_Enabled;
        bool checked = opt->state & QStyle::State_On;
        bool highlighted = opt->state & QStyle::State_MouseOver;
        bool pressed = opt->state & QStyle::State_Sunken;
        bool popuped = (btnStyle->activeSubControls & QStyle::SC_ToolButtonMenu) && (opt->state & QStyle::State_Sunken);

        if (btnStyle->flatStyle && !highlighted && !pressed && !checked)
            return;

        QColor color(Qt::white);
        if (!enabled)
            color = QColor();

        else if (popuped)
            color = m_clrHighlightDn;
        else if (checked && !highlighted && !pressed)
            color = m_clrHighlightDn;
        else if (pressed)
            color = m_clrHighlightDn;
        else if (highlighted)
            color = d->m_clrHighlight;

        if (color.isValid())
            p->fillRect(opt->rect, color);

        if (!btnStyle->tabStyle) {
            const QPen oldPen = p->pen();
            p->setPen(QColor(170, 170, 170));
            p->drawRect(btnStyle->rect.adjusted(0, 0, -1, -1));
            p->setPen(oldPen);
        }
    }
}
