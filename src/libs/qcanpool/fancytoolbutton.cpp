/**
 * Copyright (C) 2022-2023 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#include "fancytoolbutton.h"

#include <QMouseEvent>
#include <QPaintEvent>
#include <QStyleOption>
#include <QStyleOptionFocusRect>
#include <QStyleOptionToolButton>
#include <QStylePainter>
#include <QApplication>
#include <QAction>
#include <QMenu>

QCANPOOL_BEGIN_NAMESPACE

class FancyToolButtonPrivate
{
public:
    FancyToolButtonPrivate();

    QStyle *style() const;
    QStyle::PrimitiveElement indicatorArrowType() const;
    int pixelMetric(QStyle::PixelMetric metric, const QStyleOption *toolbutton,
                    const QWidget *widget) const;

    void drawArrow(const QStyleOptionToolButton *toolbutton, const QRect &rect,
                   QPainter *painter, const QWidget *w) const;

    void splitString(const QString &str, QString &strFirstRow, QString &strSecondRow) const;
    void drawToolButton(const QStyleOptionToolButton *opt, QPainter *p, const QWidget *widget);
    void drawToolButtonLabel(const QStyleOption *opt, QPainter *p, const QWidget *w);
    QRect subControlRect(QStyle::ComplexControl cc, const QStyleOptionComplex *opt, QStyle::SubControl sc,
                                      const QWidget *widget) const;

    bool hasMenu(const QStyleOptionToolButton *toolbutton) const;

public:
    Qt::ArrowType m_menuArrowType;
    FancyToolButton::MenuArea m_menuArea;
    QRect m_iconRect;
    QRect m_textRect;
    FancyToolButton *q;
    bool m_showMenu;
};

FancyToolButtonPrivate::FancyToolButtonPrivate()
    : m_menuArrowType(Qt::DownArrow)
    , m_menuArea(FancyToolButton::BottomMenuArea)
    , m_showMenu(false)
{
}

QStyle *FancyToolButtonPrivate::style() const
{
    return q->style();
}

QStyle::PrimitiveElement FancyToolButtonPrivate::indicatorArrowType() const
{
    switch (m_menuArrowType) {
    case Qt::LeftArrow:
        return QStyle::PE_IndicatorArrowLeft;
    case Qt::RightArrow:
        return QStyle::PE_IndicatorArrowRight;
    case Qt::UpArrow:
        return QStyle::PE_IndicatorArrowUp;
    case Qt::DownArrow:
    default:
        return QStyle::PE_IndicatorArrowDown;
    }
}

int FancyToolButtonPrivate::pixelMetric(QStyle::PixelMetric metric, const QStyleOption *opt,
                                           const QWidget *widget) const
{
    // When using qstylesheet, mbi may be 0
    int mbi = style()->pixelMetric(metric, opt, widget);
    if (mbi <= 0) {
        mbi = 10;
    }
    return mbi;
}

void FancyToolButtonPrivate::drawArrow(const QStyleOptionToolButton *toolbutton,
                                       const QRect &rect, QPainter *p, const QWidget *w) const
{
    QStyle::PrimitiveElement pe;

    switch (toolbutton->arrowType) {
    case Qt::LeftArrow:
        pe = QStyle::PE_IndicatorArrowLeft;
        break;
    case Qt::RightArrow:
        pe = QStyle::PE_IndicatorArrowRight;
        break;
    case Qt::UpArrow:
        pe = QStyle::PE_IndicatorArrowUp;
        break;
    case Qt::DownArrow:
        pe = QStyle::PE_IndicatorArrowDown;
        break;
    default:
        return;
    }

    QStyleOptionToolButton arrowOpt;
    arrowOpt.rect = rect;
    arrowOpt.palette = toolbutton->palette;
    arrowOpt.state = toolbutton->state;
    style()->drawPrimitive(pe, &arrowOpt, p, w);
}

void FancyToolButtonPrivate::splitString(const QString &str, QString &strFirstRow, QString &strSecondRow) const
{
    strFirstRow = str;
    strSecondRow.clear();

    QStringList strList = str.split('\n');
    if (strList.count() == 1) {
        return;
    }
    strFirstRow = strList.at(0);
    strSecondRow = strList.at(1);
}

void FancyToolButtonPrivate::drawToolButton(const QStyleOptionToolButton *opt,
                                            QPainter *p, const QWidget *widget)
{
    const QStyleOptionToolButton *toolbutton = opt;
    QRect menuarea;
    menuarea = subControlRect(QStyle::CC_ToolButton, toolbutton, QStyle::SC_ToolButtonMenu, widget);

    QStyle::State bflags = toolbutton->state & ~QStyle::State_Sunken;
    if (bflags & QStyle::State_AutoRaise) {
        if (!(bflags & QStyle::State_MouseOver) || !(bflags & QStyle::State_Enabled))
            bflags &= ~QStyle::State_Raised;
    }
    QStyle::State mflags = bflags;
    if (toolbutton->state & QStyle::State_Sunken) {
        if (toolbutton->activeSubControls & QStyle::SC_ToolButton)
            bflags |= QStyle::State_Sunken;
        mflags |= QStyle::State_Sunken;
    }

    // draw tool
    style()->drawPrimitive(QStyle::PE_PanelButtonTool, toolbutton, p, widget);

    if (toolbutton->state & QStyle::State_HasFocus) {
        QStyleOptionFocusRect fr;
        fr.QStyleOption::operator=(*toolbutton);
        fr.rect.adjust(3, 3, -3, -3);
        if (toolbutton->features & QStyleOptionToolButton::MenuButtonPopup)
            fr.rect.adjust(0, 0, -pixelMetric(QStyle::PM_MenuButtonIndicator, toolbutton, widget), 0);
        style()->drawPrimitive(QStyle::PE_FrameFocusRect, &fr, p, widget);
    }

    // draw label and icon
    QStyleOptionToolButton label = *toolbutton;
    label.state = bflags;
    drawToolButtonLabel(&label, p, widget);

    // draw arraw
    if (toolbutton->subControls & QStyle::SC_ToolButtonMenu) {
        QStyleOption tool(0);
        tool.palette = toolbutton->palette;

        QRect ir = menuarea;
        QRect rcArrow;

        if (toolbutton->toolButtonStyle == Qt::ToolButtonTextUnderIcon && m_menuArea == FancyToolButton::BottomMenuArea) {
            QString strFirstRow, strSecondRow;
            splitString(toolbutton->text, strFirstRow, strSecondRow);
            int height = toolbutton->fontMetrics.height();
            int mbi = 6;
            rcArrow = QRect(QPoint(strSecondRow.isEmpty() ? toolbutton->rect.width() / 2 - 2 : toolbutton->rect.right() - 7,
                                   m_textRect.top() + height * 3 / 2 - mbi / 2),
                            QSize(mbi, mbi));
        } else {
            int mbi = pixelMetric(QStyle::PM_MenuButtonIndicator, toolbutton, widget);
            rcArrow = QRect(ir.right() - mbi - 1,
                            toolbutton->rect.top() + (ir.height() - mbi) / 2 + ((ir.height() - mbi) % 2), mbi, mbi);
        }
        tool.rect = rcArrow;
        tool.state = mflags;
        style()->drawPrimitive(indicatorArrowType(), &tool, p, widget);
    } else if (toolbutton->features & QStyleOptionToolButton::HasMenu) {
        QStyleOptionToolButton newBtn = *toolbutton;
        if (toolbutton->toolButtonStyle == Qt::ToolButtonTextUnderIcon && m_menuArea == FancyToolButton::BottomMenuArea) {
            QString strFirstRow, strSecondRow;
            splitString(toolbutton->text, strFirstRow, strSecondRow);
            int height = toolbutton->fontMetrics.height();
            int mbi = 6;
            newBtn.rect = QRect(QPoint(strSecondRow.isEmpty() ? toolbutton->rect.width() / 2 - 2 : toolbutton->rect.right() - 8,
                                       m_textRect.top() + height * 3 / 2 - mbi / 2),
                                QSize(mbi, mbi));
        } else {
            QRect ir = menuarea;
            int mbi = pixelMetric(QStyle::PM_MenuButtonIndicator, toolbutton, widget);
            newBtn.rect = QRect(ir.right() - mbi - 1,
                                toolbutton->rect.top() + (ir.height() - mbi) / 2 + ((ir.height() - mbi) % 2), mbi, mbi);
        }
        style()->drawPrimitive(indicatorArrowType(), &newBtn, p, widget);
    }
}

void FancyToolButtonPrivate::drawToolButtonLabel(const QStyleOption *opt, QPainter *p, const QWidget *w)
{
    const QStyleOptionToolButton *toolbutton = qstyleoption_cast<const QStyleOptionToolButton *>(opt);
    QRect rect = toolbutton->rect;
    p->setFont(toolbutton->font);

    m_iconRect = QRect();
    m_textRect = QRect();

    // Arrow type always overrules and is always shown
    bool hasArrow = toolbutton->features & QStyleOptionToolButton::Arrow;
    if (((!hasArrow && toolbutton->icon.isNull()) && !toolbutton->text.isEmpty()) ||
            toolbutton->toolButtonStyle == Qt::ToolButtonTextOnly) {
        int alignment = Qt::AlignCenter | Qt::TextShowMnemonic;
        if (!style()->styleHint(QStyle::SH_UnderlineShortcut, opt, w))
            alignment |= Qt::TextHideMnemonic;
        QRect rectText = rect;
        if (hasMenu(toolbutton)) {
            int mbi = pixelMetric(QStyle::PM_MenuButtonIndicator, opt, w);
            rectText.adjust(0, 0, -mbi, 0);
        }
        m_textRect = rectText;
        style()->drawItemText(p, rect, alignment, toolbutton->palette, opt->state & QStyle::State_Enabled,
                              toolbutton->text, QPalette::ButtonText);
    } else { // !Qt::ToolButtonTextOnly
        QPixmap pm;
        QSize pmSize = toolbutton->iconSize;
        if (pmSize.width() > qMin(rect.width(), rect.height())) {
            const int iconExtent = style()->pixelMetric(QStyle::PM_SmallIconSize);
            pmSize = QSize(iconExtent, iconExtent);
        }

        if (!toolbutton->icon.isNull()) {
            QIcon::State state = toolbutton->state & QStyle::State_On ? QIcon::On : QIcon::Off;
            QIcon::Mode mode;
            if (!(toolbutton->state & QStyle::State_Enabled)) {
                mode = QIcon::Disabled;
            } else if ((opt->state & QStyle::State_MouseOver) && (opt->state & QStyle::State_AutoRaise)) {
                mode = QIcon::Active;
            } else {
                mode = QIcon::Normal;
            }
            pm = toolbutton->icon.pixmap(toolbutton->rect.size().boundedTo(pmSize), mode, state);
            pmSize = pm.size() / pm.devicePixelRatio();
        }

        if (toolbutton->toolButtonStyle != Qt::ToolButtonIconOnly) {
            p->setFont(toolbutton->font);
            QRect pr = rect, tr = rect;

            int alignment = Qt::TextShowMnemonic;
            if (!style()->styleHint(QStyle::SH_UnderlineShortcut, opt, w))
                alignment |= Qt::TextHideMnemonic;

            if (toolbutton->toolButtonStyle == Qt::ToolButtonTextUnderIcon) {
                pr.setHeight(pmSize.height() + 6);
                tr.adjust(0, pr.height() - 1, 0, -3);
                if (!hasArrow) {
                    style()->drawItemPixmap(p, pr, Qt::AlignCenter, pm);
                } else {
                    drawArrow(toolbutton, pr, p, w);
                }
                alignment |= Qt::AlignCenter;
            } else { // ToolButtonTextBesideIcon, ..
                pr.setWidth(pmSize.width() + 8);
                tr.adjust(pr.width(), 0, 0, 0);
                if (!hasArrow) {
                    style()->drawItemPixmap(p, QStyle::visualRect(opt->direction, rect, pr), Qt::AlignCenter, pm);
                } else {
                    drawArrow(toolbutton, pr, p, w);
                }
                alignment |= Qt::AlignLeft | Qt::AlignVCenter;
            }

            QRect rcText(QStyle::visualRect(opt->direction, rect, tr));
            if (toolbutton->toolButtonStyle == Qt::ToolButtonTextUnderIcon) {
                QString strFirstRow, strSecondRow;
                splitString(toolbutton->text, strFirstRow, strSecondRow);

                int height = toolbutton->fontMetrics.height();
                // text AlignTop
                QRect rcFirstRowText(QPoint(rcText.left(), rcText.top() + 1),
                                     QPoint(rcText.right(), rcText.top() + height + 1));

                // if FancyToolButton::BottomMenuArea, add height in sizeHint
                if (m_menuArea != FancyToolButton::BottomMenuArea && strSecondRow.isEmpty()) {
                    rcFirstRowText = rcText;
                }

                if (!strFirstRow.isEmpty()) {
                    style()->drawItemText(p, rcFirstRowText, alignment, toolbutton->palette,
                                          toolbutton->state & QStyle::State_Enabled, strFirstRow, QPalette::ButtonText);
                }

                if (!strSecondRow.isEmpty()) {
                    int left = rcText.left();
                    if (toolbutton->subControls & QStyle::SC_ToolButtonMenu ||
                            toolbutton->features & QStyleOptionToolButton::HasMenu)
                        left = opt->rect.left() - 5;

                    // text AlignTop
                    QRect rcSecondRowText(QPoint(left, rcText.top() + height + 1),
                                          QPoint(rcText.right(), rcText.top() + height * 2 + 2));
                    style()->drawItemText(p, rcSecondRowText, alignment, toolbutton->palette,
                                          toolbutton->state & QStyle::State_Enabled, strSecondRow, QPalette::ButtonText);
                }
            } else { // ToolButtonTextBesideIcon, ..
                style()->drawItemText(p, rcText, alignment,
                                      toolbutton->palette, toolbutton->state & QStyle::State_Enabled, toolbutton->text,
                                      QPalette::ButtonText);
            }
            m_iconRect = pr;
            m_textRect = rcText;
        } else { // ToolButtonIconOnly
            QRect pr = rect;
            if (hasMenu(toolbutton)) {
                int mbi = pixelMetric(QStyle::PM_MenuButtonIndicator, opt, w);
                pr.setWidth(pr.width() - mbi);
            }
            m_iconRect = pr;
            if (hasArrow) {
                drawArrow(toolbutton, rect, p, w);
            } else {
                style()->drawItemPixmap(p, rect, Qt::AlignCenter, pm);
            }
        }
    }
}

QRect FancyToolButtonPrivate::subControlRect(QStyle::ComplexControl cc, const QStyleOptionComplex *opt,
                                             QStyle::SubControl sc, const QWidget *widget) const
{
    switch (cc) {
    case QStyle::CC_ToolButton:
        if (const QStyleOptionToolButton *tb = qstyleoption_cast<const QStyleOptionToolButton *>(opt)) {
            int mbi = pixelMetric(QStyle::PM_MenuButtonIndicator, tb, widget);
            QRect ret = tb->rect;
            switch (sc) {
            case QStyle::SC_ToolButton:
                if ((tb->features & (QStyleOptionToolButton::MenuButtonPopup | QStyleOptionToolButton::PopupDelay)) ==
                    QStyleOptionToolButton::MenuButtonPopup)
                    ret.adjust(0, 0, -mbi, 0);
                break;
            case QStyle::SC_ToolButtonMenu:
                if ((tb->features & (QStyleOptionToolButton::MenuButtonPopup | QStyleOptionToolButton::PopupDelay)) ==
                    QStyleOptionToolButton::MenuButtonPopup)
                    ret.adjust(ret.width() - mbi - 2, 0, 0, 0);
                break;
            default:
                break;
            }
            return style()->visualRect(tb->direction, tb->rect, ret);
        }
        break;
    default:
        break;
    }
    return style()->subControlRect(cc, opt, sc, widget);
}

bool FancyToolButtonPrivate::hasMenu(const QStyleOptionToolButton *toolbutton) const
{
    if ((toolbutton->subControls & QStyle::SC_ToolButtonMenu) ||
            (toolbutton->features & QStyleOptionToolButton::HasMenu)) {
        return true;
    }
    return false;
}

FancyToolButton::FancyToolButton(QWidget *parent)
    : QToolButton(parent)
    , d(new FancyToolButtonPrivate())
{
    d->q = this;
    setAttribute(Qt::WA_StyledBackground, true);
    setAutoRaise(true);
    setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
}

FancyToolButton::FancyToolButton(const QString &text, QWidget *parent)
    : FancyToolButton(parent)
{
    setText(text);
}

FancyToolButton::FancyToolButton(const QIcon &icon, const QString &text, QWidget *parent)
    : FancyToolButton(parent)
{
    setIcon(icon);
    setText(text);
}

FancyToolButton::~FancyToolButton()
{
    delete d;
}

Qt::ArrowType FancyToolButton::menuArrowType() const
{
    return d->m_menuArrowType;
}

void FancyToolButton::setMenuArrowType(Qt::ArrowType type)
{
    if (d->m_menuArrowType == type) {
        return;
    }
    d->m_menuArrowType = type;
    if (isVisible()) {
        update();
    }
}

FancyToolButton::MenuArea FancyToolButton::menuArea() const
{
    return d->m_menuArea;
}

void FancyToolButton::setMenuArea(FancyToolButton::MenuArea area)
{
    if (d->m_menuArea == area || toolButtonStyle() != Qt::ToolButtonTextUnderIcon) {
        return;
    }
    d->m_menuArea = area;
    if (isVisible()) {
        update();
    }
}

QSize FancyToolButton::sizeHint() const
{
    QSize sz = QToolButton::sizeHint();
    QStyleOptionToolButton opt;
    initStyleOption(&opt);

    int w = 0, h = 0;

    QString textButton = opt.text;

    if (opt.toolButtonStyle != Qt::ToolButtonTextOnly) {
        QSize icon = opt.iconSize;
        QAction *action = defaultAction();
        if (action && !action->icon().isNull())
            icon = action->icon().actualSize(opt.iconSize);
        w = icon.width();
        h = icon.height();
    }

    bool indicatorCenter = false;
    bool isBottomMenuArea = (menuArea() == FancyToolButton::BottomMenuArea);
    QToolButton::ToolButtonPopupMode mode = QToolButton::popupMode();

    if (opt.toolButtonStyle != Qt::ToolButtonIconOnly) {
        if (opt.toolButtonStyle == Qt::ToolButtonTextUnderIcon) {
            QString strFirstRow, strSecondRow;
            d->splitString(textButton, strFirstRow, strSecondRow);

            QSize textFirstSize;
            if (!strFirstRow.isEmpty())
                textFirstSize = opt.fontMetrics.size(Qt::TextShowMnemonic, strFirstRow);

            if (!strSecondRow.isEmpty()) {
                QSize textSecondSize = opt.fontMetrics.size(Qt::TextShowMnemonic, strSecondRow);
                textFirstSize.setWidth(qMax(textFirstSize.width(), textSecondSize.width()));
            }

            h = sz.height();
            if (isBottomMenuArea && strSecondRow.isEmpty()) {
                indicatorCenter = opt.features & QStyleOptionToolButton::HasMenu;
                h += opt.fontMetrics.height(); // place an empty text row
            }

            // if the text is more than icon
            if (textFirstSize.width() > w)
                w = textFirstSize.width();
            // add pixel
            w += mode == QToolButton::MenuButtonPopup ? 4 : 10;
        } else {
            // ToolButtonTextOnly, ToolButtonTextBesideIcon, ToolButtonFollowStyle
            QSize textSize = opt.fontMetrics.size(Qt::TextShowMnemonic, textButton);
#if QT_VERSION < QT_VERSION_CHECK(5, 11, 0)
            textSize.setWidth(textSize.width() + opt.fontMetrics.width(QLatin1Char(' ')) * 2);
#else
            textSize.setWidth(textSize.width() + opt.fontMetrics.horizontalAdvance(QLatin1Char(' ')) * 2);
#endif
            h = sz.height();

            if (opt.toolButtonStyle == Qt::ToolButtonTextBesideIcon) {
                w += textSize.width() + 6;

                if (textSize.height() > h)
                    h = textSize.height();
            } else { // ToolButtonTextOnly, ToolButtonFollowStyle
                w = textSize.width() + 4;
            }
        }
    } else {
        // ToolButtonIconOnly
        h = sz.height();
        w = h;
        if (d->hasMenu(&opt)) {
            w += 1;
        }
    }

    opt.rect.setSize(QSize(w, h));   // PM_MenuButtonIndicator depends on the height

    if ((mode == QToolButton::MenuButtonPopup || d->hasMenu(&opt)) && !indicatorCenter) {
        int mbi = d->pixelMetric(QStyle::PM_MenuButtonIndicator, &opt, this);
        w += mbi;
        // increase width to center the display during vertical layout
        if ((opt.toolButtonStyle != Qt::ToolButtonTextBesideIcon &&
             opt.toolButtonStyle != Qt::ToolButtonTextUnderIcon) ||
                (!isBottomMenuArea && opt.toolButtonStyle == Qt::ToolButtonTextUnderIcon)) {
            w += mbi * 2 / 3;
        }
    }

#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    return QSize(w, h).expandedTo(QApplication::globalStrut());
#else
    return QSize(w, h);
#endif
}

void FancyToolButton::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    QPainter p(this);
    QStyleOptionToolButton opt;
    initStyleOption(&opt);
    if (d->m_showMenu) {
        opt.state |= QStyle::State_Sunken | QStyle::State_Raised;
    }
    d->drawToolButton(&opt, &p, this);
}

void FancyToolButton::mousePressEvent(QMouseEvent *e)
{
    if (e->button() == Qt::LeftButton && popupMode() == MenuButtonPopup) {
        QRect outRect = d->m_iconRect;
        if (toolButtonStyle() == Qt::ToolButtonTextOnly) {
            outRect = d->m_textRect;
        }
        if (outRect.isValid() && !outRect.contains(e->pos())) {
            showMenu();
            return;
        }
        // RightMenu popups menu on the right
        if (d->m_menuArea == RightMenuArea && d->m_menuArrowType == Qt::RightArrow) {
            QStyleOptionToolButton opt;
            initStyleOption(&opt);
            QRect popupr = d->subControlRect(QStyle::CC_ToolButton, &opt, QStyle::SC_ToolButtonMenu, this);
            if (popupr.isValid() && popupr.contains(e->pos())) {
                QAction *action = defaultAction();
                if (action && action->menu()) {
                    d->m_showMenu = true;
                    repaint();
                    QPoint pos = e->globalPos();
                    pos.setX(pos.x() - e->x() + width());
                    pos.setY(pos.y() - e->y());
                    action->menu()->exec(pos);
                    d->m_showMenu = false;
                    repaint();
                    return;
                }
            }
        }

    }
    QToolButton::mousePressEvent(e);
}

QCANPOOL_END_NAMESPACE
