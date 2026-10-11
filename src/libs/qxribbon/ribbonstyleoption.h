/**
 * Copyleft (C) 2023 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MIT
 **/
#pragma once

#include "qxribbon_global.h"
#include "ribbonbar.h"
#include <QDebug>

QX_RIBBON_BEGIN_NAMESPACE

class RibbonStyleOptionPrivate;

/**
 * @brief 定义了 ribbon 所有尺寸相关信息，ribbon 的建立都基于此类的尺寸，如果想调整，
 * 可以通过 @ref RibbonFactory （通过 RibbonManager 单例管理） 的 @ref RibbonFactory::setRibbonStyleOption
 * 函数设置自己的 RibbonStyleOption
 *
 * @sa RibbonManager
 *
 * 一般 RibbonFactory::setRibbonStyleOption 函数最好在 ribbonbar 构建之前调用
 *
 * @note 此类定义了 ribbonbar 和 group 的高度信息，并通过字体提前计算好一些布局信息
 *
 * @note 尺寸布局是可替换的：两种做法都行——
 *   - 子类化本类、覆盖几个 virtual，再交给 RibbonFactory::setRibbonStyleOption；
 *   - 或直接用本类，调用 setBaseHeight() / setTitleBarHeight() / setTabBarHeight()
 *     微调尺寸（配置文件的读写属于调用方，不在这层——调用方把自己的配置映射到这些 setter 即可）。
 *   这两种做法不冲突：setter 改的是基础高度，recalc() 据此重算四个 ribbonbar 高度；
 *   显式 set 过 title/tab 之后，recalc() 会保留你设的值、只重算派生项。
 */
class QX_RIBBON_EXPORT RibbonStyleOption
{
public:
    RibbonStyleOption();
    virtual ~RibbonStyleOption();
public:
    virtual int ribbonBarHeight(RibbonBar::RibbonStyle s) const;
    virtual int titleBarHeight() const;
    virtual int tabBarHeight() const;
    virtual void recalc();
public:
    /**
     * @brief 设定尺寸布局的主尺度（以字体行高为单位的基准高度）。
     * 设为正数后，titleBar / tabBar 按固定比例（1.65 / 1.5）重新派生，四个 ribbonbar 高度跟随重算。
     * 传 0 或不调用则维持构造时的字体派生值。
     */
    void setBaseHeight(int height);
    /**
     * @brief 直接覆盖标题栏高度（px）。覆盖后 recalc() 会保留此值、只重算四个 ribbonbar 高度。
     */
    void setTitleBarHeight(int height);
    /**
     * @brief 直接覆盖标签栏高度（px）。覆盖后 recalc() 会保留此值、只重算四个 ribbonbar 高度。
     */
    void setTabBarHeight(int height);
private:
    QX_DECLARE_PRIVATE(RibbonStyleOption)
};

QX_RIBBON_EXPORT QDebug operator<<(QDebug debug, const RibbonStyleOption &c);

QX_RIBBON_END_NAMESPACE
