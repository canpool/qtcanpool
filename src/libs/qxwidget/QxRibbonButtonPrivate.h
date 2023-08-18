/**
 * Copyleft (C) 2023 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#pragma once

#include "QxRibbonButton.h"

#include <QPainter>
#include <QStyleOptionToolButton>
#include <QStyle>

QX_BEGIN_NAMESPACE

class RibbonButtonPrivate
{
    QX_DECLARE_PUBLIC(RibbonButton)
public:
    RibbonButtonPrivate();
    virtual ~RibbonButtonPrivate();

    QStyle *style() const;

    void recalcSizeHint(QStyleOptionToolButton &opt, QSize s);
    // 此函数用来计算icon和text的尺寸
    // 注意icon的尺寸依赖text，且只要知道字体大小就可以确定icon的高度
    void calcIconAndTextRect(const QStyleOptionToolButton &opt);
    // 在需要绘制文字的时候调用此函数，有些效果需要文字的文字改变
    QRect adjustedTextRect(const QStyleOptionToolButton &opt, const QWidget *w);
    QRect calcIndicatorArrowDownRect(const QStyleOptionToolButton &opt);
    virtual QPixmap createIconPixmap(const QStyleOptionToolButton &opt, const QSize &iconsize);
    // 根据鼠标位置更新按钮的信息
    void updateButtonState(const QPoint &pos);

    void drawButton(QStyleOptionToolButton &opt, QPainter &p, const QWidget *w);
    void drawIconAndLabel(QStyleOptionToolButton &opt, QPainter &p, const QWidget *w);
    void drawArrow(const QStyle *style, const QStyleOptionToolButton *toolbutton, const QRect &rect,
                   QPainter *painter, const QWidget *widget = Q_NULLPTR);

public:
    RibbonButton::RibbonButtonType m_buttonType;
    RibbonButton::LargeButtonType m_largeButtonType;
    bool m_mouseOnSubControl;   ///< 这个用于标记MenuButtonPopup模式下，鼠标在文本区域
    bool m_menuButtonPressed;
    bool m_isWordWrap;          ///< 标记是否文字换行 @default false
    int m_iconAndTextSpace;     ///< 按钮和边框的距离
    QSize m_sizeHint;           ///< 保存计算好的sizehint
    QRect m_iconRect;           ///< 记录icon的绘制位置
    QRect m_textRect;           ///< 记录text的绘制位置
    QRect m_indicatorArrowRect;   ///< 记录IndicatorArrow的绘制位置

    static bool s_isToolButtonTextShift;   ///< 配置RibbonButton文字在点击时是否会轻微改变位置而达到一种类似跳动的效果,
                                           ///< @default 默认为false
    static bool s_liteStyleEnableWordWrap; ///< 在lite模式下是否允许文字换行，如果允许，则图标相对比较小，默认不允许
};

QX_END_NAMESPACE
