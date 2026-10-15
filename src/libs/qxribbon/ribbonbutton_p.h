/**
 * Copyleft (C) 2023 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MIT
 **/
#pragma once

#include "ribbonbutton.h"

#include <QPainter>
#include <QStyleOptionToolButton>
#include <QStyle>

QX_RIBBON_BEGIN_NAMESPACE

class RibbonButtonPrivate
{
public:
    QX_DECLARE_PUBLIC(RibbonButton)
public:
    RibbonButtonPrivate();
    virtual ~RibbonButtonPrivate();

    QStyle *style() const;

    void recalcSizeHint(QStyleOptionToolButton &opt, QSize s);
    // Computes the sizes of the icon and the text
    // Note the icon size depends on the text, and the icon height is known once the font size is known
    void calcIconAndTextRect(const QStyleOptionToolButton &opt);
    // Call this when the text must be drawn; some effects need text changes to take effect
    QRect adjustedTextRect(const QStyleOptionToolButton &opt, const QWidget *w);
    QRect calcIndicatorArrowDownRect(const QStyleOptionToolButton &opt);
    virtual QPixmap createIconPixmap(const QStyleOptionToolButton &opt, const QSize &iconsize);
    // Updates the button information based on the mouse position
    void updateButtonState(const QPoint &pos);

    void drawButton(QStyleOptionToolButton &opt, QPainter &p, const QWidget *w);
    void drawIconAndLabel(QStyleOptionToolButton &opt, QPainter &p, const QWidget *w);
    void drawArrow(const QStyle *style, const QStyleOptionToolButton *toolbutton, const QRect &rect, QPainter *painter,
                   const QWidget *widget = Q_NULLPTR);
public:
    RibbonButton::RibbonButtonType m_buttonType;
    RibbonButton::LargeButtonType m_largeButtonType;
    bool m_mouseOnSubControl;   ///< Marks that the mouse is over the text area in MenuButtonPopup mode
    bool m_menuButtonPressed;
    bool m_isWordWrap;            ///< Whether the text wraps, default false
    int m_spacing;                ///< Distance between the button and the border
    QSize m_sizeHint;             ///< The computed sizehint
    QRect m_iconRect;             ///< The icon drawing position
    QRect m_textRect;             ///< The text drawing position
    QRect m_indicatorArrowRect;   ///< The IndicatorArrow drawing position

    static bool
        s_isToolButtonTextShift;   ///< Whether RibbonButton text shifts slightly on click for a bounce-like effect
};

QX_RIBBON_END_NAMESPACE
