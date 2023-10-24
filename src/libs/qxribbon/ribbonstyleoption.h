/**
 * Copyleft (C) 2023 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
**/
#pragma once

#include "qxglobal.h"
#include "ribbonbar.h"
#include <QDebug>

QX_BEGIN_NAMESPACE

class RibbonStyleOptionPrivate;

/**
 * @brief 定义了 ribbon 所有尺寸相关信息，ribbon 的建立都基于此类的尺寸，如果想调整，
 * 可以通过 @ref RibbonFactory（通过 RibbonManager 单例管理） 的 @ref setRibbonStyleOption
 * 函数设置自己的 RibbonStyleOption
 *
 * @sa RibbonManager
 *
 * 一般 RibbonFactory::setRibbonStyleOption 函数最好在 ribbonbar 构建之前调用
 *
 * @note 此类定义了 ribbonbar 和 group 的高度信息，并通过字体提前计算好一些布局信息
 *
 * @todo 后续开发通过配置文件定义 ribbon 的尺寸布局
 */
class QX_WIDGET_EXPORT RibbonStyleOption
{
public:
    RibbonStyleOption();
    virtual ~RibbonStyleOption();
public:
    virtual int ribbonBarHeight(RibbonBar::RibbonStyle s) const;
    virtual int titleBarHeight() const;
    virtual int tabBarHeight() const;
    virtual void recalc();
protected:
    virtual int calcRibbonBarHeight(RibbonBar::RibbonStyle s) const;
private:
    QX_DECLARE_PRIVATE(RibbonStyleOption)
};

QX_WIDGET_EXPORT QDebug operator<<(QDebug debug, const RibbonStyleOption &c);

QX_END_NAMESPACE
