﻿/**
 * Copyleft (C) 2023 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MIT
**/
#pragma once

#include "qxribbon_global.h"
#include "ribbongroup.h"

#include <QToolButton>

QX_RIBBON_BEGIN_NAMESPACE

class RibbonButton;
class RibbonGroupItem;
class RibbonGroupLayout;

/**
 * @brief Group右下角的操作按钮
 *
 * 此按钮和一个action关联，使用 @ref RibbonGroup::addOptionAction 函数用于生成此按钮
 *
 */
class RibbonGroupOptionButton : public QToolButton
{
    Q_OBJECT
public:
    explicit RibbonGroupOptionButton(QWidget *parent = Q_NULLPTR);
    // 有别于setDefaultAction 此函数只关联action的响应，而不设置text，icon等
    void connectAction(QAction *action);
};

class RibbonGroupPrivate
{
    QX_DECLARE_PUBLIC(RibbonGroup)
public:
    RibbonGroupPrivate();

    void init();
    void resetLargeToolButtonStyle();

    // 返回最后一个添加的action对应的button，前提是最后一个是toolbutton，否则返回Q_NULLPTR
    RibbonButton *lastAddedButton();
    // RibbonGroup的友元类RibbonCustommizeWidget会通过该接口获取所有item
    const QList<RibbonGroupItem *> &ribbonGroupItems() const;
public:
    RibbonGroupLayout *m_layout;
    RibbonGroupOptionButton *m_optionActionButton;
    RibbonGroup::GroupLayoutMode m_groupLayoutMode;   ///< group的布局模式，默认为3行模式ThreeRowMode
    static int s_groupTitleHeight;                       ///< group标题栏的全局高度,默认为21
    static bool s_titleVisible;
    bool m_isCanCustomize;                                ///< 记录是否可自定义
};

/* RibbonSeparator */
class RibbonSeparator : public QWidget
{
    Q_OBJECT
public:
    RibbonSeparator(int height, QWidget *parent = Q_NULLPTR);
    RibbonSeparator(QWidget *parent = Q_NULLPTR);

    virtual QSize sizeHint() const Q_DECL_OVERRIDE;
    void setTopBottomMargins(int top, int bottom);
protected:
    void paintEvent(QPaintEvent *event) Q_DECL_OVERRIDE;
private:
    int m_topMargin;
    int m_bottomMargin;
};

QX_RIBBON_END_NAMESPACE
