/**
 * Copyright (C) 2022 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#include "fancytoolbutton.h"

#include <QMouseEvent>
#include <QPaintEvent>
#include <QStyleOption>
#include <QStyleOptionFocusRect>
#include <QStyleOptionToolButton>
#include <QStylePainter>

#define FANCY_INDICATOR_ARROW_WIDTH 8
#define FANCY_WIDTH_HEIGHT_RATIO    1.2

QCANPOOL_BEGIN_NAMESPACE

class FancyToolButtonPrivate
{
public:
    FancyToolButtonPrivate();

    QStyle *style() const;
    QStyle::PrimitiveElement indicatorArrowType() const;
    QRect indicatorArrowRect(const QStyleOptionToolButton &opt, const QWidget *w) const;
    QPixmap createIconPixmap(const QStyleOptionToolButton &opt,
                             const QSize &iconsize, const QWidget *w) const;

    void drawControl(QStyleOptionToolButton &opt, QPainter &p, const QWidget *w) const;
    void drawArrow(const QStyleOptionToolButton *toolbutton, const QRect &rect,
                   QPainter *painter, const QWidget *w) const;

    QSize calcSizeHint(const QStyleOptionToolButton &opt, QSize s, const QWidget *w);
    void calcIconAndTextRect(const QStyleOptionToolButton &opt, const QWidget *w);

public:
    Qt::ArrowType m_menuArrowType;
    FancyToolButton::MenuArea m_menuArea;
    bool  m_mouseOnSubControl;
    bool  m_menuButtonPressed;
    QRect m_iconRect;
    QRect m_textRect;
    bool  m_isWordWrap;
    int   m_space;
    FancyToolButton *q;
};

FancyToolButtonPrivate::FancyToolButtonPrivate()
    : m_menuArrowType(Qt::DownArrow)
    , m_menuArea(FancyToolButton::BottomMenuArea)
    , m_mouseOnSubControl(false)
    , m_menuButtonPressed(false)
    , m_isWordWrap(false)
    , m_space(2)
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

QRect FancyToolButtonPrivate::indicatorArrowRect(const QStyleOptionToolButton &opt, const QWidget *w) const
{
    QRect rect = opt.rect;
    int fmls = w->fontMetrics().lineSpacing();
    int pos = FANCY_INDICATOR_ARROW_WIDTH;
    if (fmls > FANCY_INDICATOR_ARROW_WIDTH) {
        pos = fmls - (fmls - FANCY_INDICATOR_ARROW_WIDTH) / 2;
    }

    if (m_menuArea == FancyToolButton::BottomMenuArea) {
        if (opt.toolButtonStyle == Qt::ToolButtonTextUnderIcon && m_isWordWrap) {
            rect.setRect(m_textRect.right(), rect.bottom() - pos - m_space,
                         rect.right() - m_space - m_textRect.right(), FANCY_INDICATOR_ARROW_WIDTH);

        } else {
            rect.setRect(rect.left(), rect.bottom() - pos - m_space,
                         rect.width(), FANCY_INDICATOR_ARROW_WIDTH);
        }
    } else { /* RightMenuArea */
        rect.setRect(rect.width() - pos - m_space, m_space,
                     FANCY_INDICATOR_ARROW_WIDTH, rect.height() - 2 * m_space);
    }
    return rect;
}

QPixmap FancyToolButtonPrivate::createIconPixmap(const QStyleOptionToolButton &opt,
                                                 const QSize &iconsize, const QWidget *w) const
{
    if (opt.icon.isNull()) {
        return QPixmap();
    }

    QIcon::State state = opt.state & QStyle::State_On ? QIcon::On : QIcon::Off;
    QIcon::Mode  mode;
    if (!(opt.state & QStyle::State_Enabled)) {
        mode = QIcon::Disabled;
    } else if ((opt.state & QStyle::State_MouseOver) && (opt.state & QStyle::State_AutoRaise)) {
        mode = QIcon::Active;
    } else {
        mode = QIcon::Normal;
    }
    return opt.icon.pixmap(w->window()->windowHandle(), iconsize, mode, state);
}

void FancyToolButtonPrivate::drawControl(QStyleOptionToolButton &opt,
                                         QPainter &p, const QWidget *w) const
{
    // has menu
    if (opt.features & QStyleOptionToolButton::MenuButtonPopup || opt.features & QStyleOptionToolButton::HasMenu) {
        if (!w->rect().contains(w->mapFromGlobal(QCursor::pos()))) {
            // the widget is not under the mouse.
            opt.state &= ~QStyle::State_MouseOver;
        }
    }

    bool autoRaise = opt.state & QStyle::State_AutoRaise;
    QStyle::State bflags = opt.state & ~QStyle::State_Sunken;

    if (autoRaise) {
        if (!(bflags & QStyle::State_MouseOver) || !(bflags & QStyle::State_Enabled)) {
            bflags &= ~QStyle::State_Raised;
        }
    }

    if (opt.state & QStyle::State_Sunken) {
        if (opt.activeSubControls & QStyle::SC_ToolButton) {
            bflags |= QStyle::State_Sunken;
        } else if (opt.activeSubControls & QStyle::SC_ToolButtonMenu) {
            bflags |= QStyle::State_MouseOver;
        }
    }

    // draw panel
    QStyleOption tool = opt;
    tool.state = bflags;
    QStyle::PrimitiveElement pannelPe = autoRaise ? QStyle::PE_PanelButtonTool : QStyle::PE_PanelButtonBevel;
    if ((opt.subControls & QStyle::SC_ToolButton) && (opt.features & QStyleOptionToolButton::MenuButtonPopup)) {
        style()->drawPrimitive(QStyle::PE_PanelButtonTool, &tool, &p, w);
        if (opt.activeSubControls & QStyle::SC_ToolButtonMenu) {
            // menu is active, do nothing
        } else {
            if (tool.state & QStyle::State_MouseOver) {
                if (m_mouseOnSubControl) { // raise icon
                    tool.rect  = m_iconRect;
                } else { // raise text
                    tool.rect.setRect(m_textRect.x(), m_textRect.y(),
                                      tool.rect.width() - 2 * m_space, m_textRect.height());
                }
                tool.state = QStyle::State_Raised;
                style()->drawPrimitive(pannelPe, &tool, &p, w);
            }
        }
    } else if ((opt.subControls & QStyle::SC_ToolButton) && (opt.features & QStyleOptionToolButton::HasMenu)) {
        style()->drawPrimitive(pannelPe, &tool, &p, w);
    } else if (opt.subControls & QStyle::SC_ToolButton) {
        if (opt.state & QStyle::State_Sunken) {
            tool.state &= ~QStyle::State_MouseOver;
        }
        style()->drawPrimitive(pannelPe, &tool, &p, w);
    }

    // draw focus
    if (opt.state & QStyle::State_HasFocus) {
        QStyleOptionFocusRect fr;
        fr.QStyleOption::operator=(opt);
        fr.rect.adjust(m_space, m_space, -m_space, -m_space);
        if (opt.features & QStyleOptionToolButton::MenuButtonPopup) {
            fr.rect.adjust(0, 0, -style()->pixelMetric(QStyle::PM_MenuButtonIndicator, &opt, w), 0);
        }
        style()->drawPrimitive(QStyle::PE_FrameFocusRect, &fr, &p, w);
    }

    // draw icon, text, arrow
    bool  isArrow = opt.features & QStyleOptionToolButton::Arrow;
    if (opt.toolButtonStyle == Qt::ToolButtonTextOnly || ((!isArrow && opt.icon.isNull()) && !opt.text.isEmpty())) {
        int alignment = Qt::AlignCenter | Qt::TextShowMnemonic | Qt::TextWordWrap;
        if (!style()->styleHint(QStyle::SH_UnderlineShortcut, &opt, w)) {
            alignment |= Qt::TextHideMnemonic;
        }
        p.setFont(opt.font);
        style()->drawItemText(&p, m_textRect, alignment, opt.palette, opt.state & QStyle::State_Enabled,
                              opt.text, QPalette::ButtonText);
    } else { // draw icon + text
        if (isArrow) {
            drawArrow(&opt, opt.rect, &p, w);
        } else {
            QPixmap pm = createIconPixmap(opt, m_iconRect.size(), w);
            style()->drawItemPixmap(&p, m_iconRect, Qt::AlignCenter, pm);
        }
        if (opt.toolButtonStyle != Qt::ToolButtonIconOnly) {
            p.setFont(opt.font);
            int alignment = Qt::TextShowMnemonic | Qt::TextWordWrap;
            if (!style()->styleHint(QStyle::SH_UnderlineShortcut, &opt, w)) {
                alignment |= Qt::TextHideMnemonic;
            }
            if (opt.toolButtonStyle == Qt::ToolButtonTextUnderIcon && m_menuArea == FancyToolButton::BottomMenuArea) {
                alignment |= Qt::AlignHCenter | Qt::AlignTop;
            } else {
                alignment |= Qt::AlignCenter;
            }
            QRect vr = QStyle::visualRect(opt.direction, opt.rect, m_textRect);
            style()->drawItemText(&p, vr, alignment, opt.palette, opt.state & QStyle::State_Enabled,
                                  opt.text, QPalette::ButtonText);
        }
    }
    // draw arrow
    if (opt.features & QStyleOptionToolButton::MenuButtonPopup || opt.features & QStyleOptionToolButton::HasMenu) {
        opt.rect = indicatorArrowRect(opt, w);
        style()->drawPrimitive(indicatorArrowType(), &opt, &p, w);
    }
}

void FancyToolButtonPrivate::drawArrow(const QStyleOptionToolButton *toolbutton,
                                       const QRect &rect, QPainter *p, const QWidget *w) const
{    QStyle::PrimitiveElement pe;

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
    QStyleOption arrowOpt = *toolbutton;
    arrowOpt.rect = rect;
    w->style()->drawPrimitive(pe, &arrowOpt, p, w);
}


QSize FancyToolButtonPrivate::calcSizeHint(const QStyleOptionToolButton &opt, QSize s, const QWidget *w)
{
    QFontMetrics fm = w->fontMetrics();
    m_isWordWrap = opt.text.contains('\n');

    if (opt.toolButtonStyle == Qt::ToolButtonTextUnderIcon) {
        if (m_menuArea == FancyToolButton::BottomMenuArea) {
            if (opt.features & QStyleOptionToolButton::MenuButtonPopup ||
                    opt.features & QStyleOptionToolButton::HasMenu) {
                if (m_isWordWrap) {
                    s.rwidth() += fm.lineSpacing();
                }
            }
            if (m_isWordWrap) {
                s.rheight() -= fm.lineSpacing();
            }
        }
    } else if (opt.toolButtonStyle == Qt::ToolButtonTextOnly) {
        if (m_menuArea == FancyToolButton::BottomMenuArea) {
            s.rheight() += fm.lineSpacing();
        }
    }
    if (opt.toolButtonStyle != Qt::ToolButtonTextBesideIcon) {
        if (m_menuArea == FancyToolButton::RightMenuArea) {
            s.rwidth() += 2 * FANCY_INDICATOR_ARROW_WIDTH;
        }
    }
    return s;
}

void FancyToolButtonPrivate::calcIconAndTextRect(const QStyleOptionToolButton &opt, const QWidget *w)
{
    QRect rect = opt.rect;
    int fmls = w->fontMetrics().lineSpacing();
    if (opt.toolButtonStyle == Qt::ToolButtonTextOnly || opt.icon.isNull()) {
        m_iconRect = QRect();
        m_textRect = rect.adjusted(m_space, m_space, -m_space, -m_space);
        if (opt.features & QStyleOptionToolButton::MenuButtonPopup || opt.features & QStyleOptionToolButton::HasMenu) {
            if (m_menuArea == FancyToolButton::BottomMenuArea) {
                m_textRect.adjust(0, 0, 0, -fmls);
            }
        }
    } else if (opt.toolButtonStyle == Qt::ToolButtonIconOnly) {
        m_iconRect = rect.adjusted(m_space, m_space, -m_space, -m_space);
        if (opt.features & QStyleOptionToolButton::MenuButtonPopup || opt.features & QStyleOptionToolButton::HasMenu) {
            if (m_menuArea == FancyToolButton::BottomMenuArea) {
                m_iconRect.adjust(0, 0, 0, -fmls);
            }
        }
        m_textRect = QRect();
    } else if (opt.toolButtonStyle == Qt::ToolButtonTextUnderIcon) {
        m_iconRect = rect.adjusted(m_space, m_space, -m_space, -m_space);
        if (m_menuArea == FancyToolButton::BottomMenuArea) {
            int height = rect.height() - (fmls * 2) - 2 * m_space - m_space;
            m_iconRect.setHeight(height);
        } else {
            m_iconRect.setHeight(opt.iconSize.height());
        }
        m_textRect.setRect(m_space, m_iconRect.bottom() + m_space,
                           m_iconRect.width(), rect.bottom() - m_iconRect.bottom() - 2 * m_space);
        if (opt.features & QStyleOptionToolButton::MenuButtonPopup || opt.features & QStyleOptionToolButton::HasMenu) {
            if (m_menuArea == FancyToolButton::BottomMenuArea) {
                if (m_isWordWrap) {
                    m_textRect.adjust(0, 0, -fmls, 0);
                }
            }
        }
    } else { /* Qt::ToolButtonTextBesideIcon */
        m_iconRect = QRect(m_space, m_space, qMax(rect.height(), opt.iconSize.width()) - 2 * m_space,
                           rect.height() - 2 * m_space);
        m_textRect.setRect(m_iconRect.right() + m_space, m_space,
                           rect.right() - m_iconRect.right() - 2 * m_space, rect.height() - 2 * m_space);
        if (opt.features & QStyleOptionToolButton::MenuButtonPopup || opt.features & QStyleOptionToolButton::HasMenu) {
            if (m_menuArea == FancyToolButton::RightMenuArea) {
                m_textRect.adjust(0, 0, -fmls, 0);
            } else {
                m_iconRect.adjust(0, 0, 0, -fmls);
                m_textRect.adjust(0, 0, 0, -fmls);
            }
        }
    }
    // adjust the icon rect
    if (m_iconRect.isValid()) {
        int wa = 0, ha = 0;
        if (m_iconRect.size().width() > opt.iconSize.width()) {
            wa = (m_iconRect.size().width() - opt.iconSize.width()) / 2;
        }
        if (m_iconRect.size().height() > opt.iconSize.height()) {
            ha = (m_iconRect.size().height() - opt.iconSize.height()) / 2;
        }
        m_iconRect.adjust(wa, ha, -wa, -ha);
    }
}

FancyToolButton::FancyToolButton(QWidget *parent)
    : QToolButton(parent)
    , d(new FancyToolButtonPrivate())
{
    d->q = this;
    setAttribute(Qt::WA_StyledBackground, true);
    setAutoRaise(true);
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
    if (d->m_menuArea == area) {
        return;
    }
    d->m_menuArea = area;
    if (isVisible()) {
        update();
    }
}

void FancyToolButton::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    QPainter p(this);
    QStyleOptionToolButton opt;
    initStyleOption(&opt);
    d->drawControl(opt, p, this);
}

void FancyToolButton::mouseMoveEvent(QMouseEvent *e)
{
    bool isMouseOnSubControl(false);

    if (d->m_iconRect.isValid()) {
        isMouseOnSubControl = !d->m_iconRect.contains(e->pos());
    }

    if (d->m_mouseOnSubControl != isMouseOnSubControl) {
        d->m_mouseOnSubControl = isMouseOnSubControl;
        update();
    }
    QToolButton::mouseMoveEvent(e);
}

void FancyToolButton::mousePressEvent(QMouseEvent *e)
{
    if (e->button() == Qt::LeftButton && popupMode() == MenuButtonPopup) {
        QRect outRect = d->m_iconRect;
        if (toolButtonStyle() == Qt::ToolButtonTextOnly) {
            outRect = d->m_textRect;
        }
        if (outRect.isValid() && !outRect.contains(e->pos())) {
            d->m_menuButtonPressed = true;
            showMenu();
            return;
        }
    }
    d->m_menuButtonPressed = false;
    QToolButton::mousePressEvent(e);
}

void FancyToolButton::mouseReleaseEvent(QMouseEvent *e)
{
    QToolButton::mouseReleaseEvent(e);
    d->m_menuButtonPressed = false;
}

void FancyToolButton::focusOutEvent(QFocusEvent *e)
{
    QToolButton::focusOutEvent(e);
    d->m_mouseOnSubControl = false;
}

void FancyToolButton::leaveEvent(QEvent *e)
{
    d->m_mouseOnSubControl = false;
    QToolButton::leaveEvent(e);
}

bool FancyToolButton::hitButton(const QPoint &pos) const
{
    if (QAbstractButton::hitButton(pos)) {
        return !d->m_menuButtonPressed;
    }
    return false;
}

void FancyToolButton::resizeEvent(QResizeEvent *e)
{
    QToolButton::resizeEvent(e);
    QStyleOptionToolButton opt;
    initStyleOption(&opt);
    d->calcIconAndTextRect(opt, this);
}

QSize FancyToolButton::sizeHint() const
{
    QSize s = QToolButton::sizeHint();
    QStyleOptionToolButton opt;
    initStyleOption(&opt);
    return d->calcSizeHint(opt, s, this);
}

void FancyToolButton::changeEvent(QEvent *e)
{
    if (e && e->type() == QEvent::FontChange) {
        QStyleOptionToolButton opt;
        initStyleOption(&opt);
        d->calcIconAndTextRect(opt, this);
    }
    QToolButton::changeEvent(e);
}

void FancyToolButton::actionEvent(QActionEvent *e)
{
    QToolButton::actionEvent(e);
}

QSize FancyToolButton::minimumSizeHint() const
{
    return sizeHint();
}

bool FancyToolButton::event(QEvent *e)
{
    switch (e->type()) {
    case QEvent::WindowDeactivate:
    case QEvent::ActionChanged:
    case QEvent::ActionRemoved:
    case QEvent::ActionAdded:
        d->m_mouseOnSubControl = false;
        break;
    default:
        break;
    }
    return QToolButton::event(e);
}

QCANPOOL_END_NAMESPACE
