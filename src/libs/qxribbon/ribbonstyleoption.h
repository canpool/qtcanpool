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
 * @brief Defines all size-related information of the ribbon; the ribbon is built based on the sizes of this class. To
 * adjust them, use the @ref RibbonFactory::setRibbonStyleOption of @ref RibbonFactory (managed via the RibbonManager
 * singleton) to set your own RibbonStyleOption
 *
 * @sa RibbonManager
 *
 * It is generally best to call RibbonFactory::setRibbonStyleOption before the ribbonbar is built
 *
 * @note This class defines the height information of the ribbonbar and groups, and precomputes some layout information
 * from the font
 *
 * @note The size layout is replaceable: either approach works —
 *   - subclass this class, override a few virtuals, then pass it to RibbonFactory::setRibbonStyleOption;
 *   - or use this class directly and call setBaseHeight() / setTitleBarHeight() / setTabBarHeight()
 *     to fine-tune the sizes (reading/writing the config file belongs to the caller, not this layer — the caller just
 * maps its settings to these setters). The two approaches do not conflict: the setter changes the base height, and
 * recalc() recomputes the four ribbonbar heights accordingly; after title/tab are explicitly set, recalc() keeps your
 * values and only recomputes the derived items.
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
     * @brief Sets the primary scale of the size layout (base height in units of the font line spacing).
     * When set to a positive value, titleBar / tabBar are re-derived with fixed ratios (1.65 / 1.5), and the four
     * ribbonbar heights are recomputed accordingly. Passing 0 or not calling this keeps the font-derived values from
     * construction.
     */
    void setBaseHeight(int height);
    /**
     * @brief Directly overrides the title bar height (px). After overriding, recalc() keeps this value and only
     * recomputes the four ribbonbar heights.
     */
    void setTitleBarHeight(int height);
    /**
     * @brief Directly overrides the tab bar height (px). After overriding, recalc() keeps this value and only
     * recomputes the four ribbonbar heights.
     */
    void setTabBarHeight(int height);
private:
    QX_DECLARE_PRIVATE(RibbonStyleOption)
};

QX_RIBBON_EXPORT QDebug operator<<(QDebug debug, const RibbonStyleOption &c);

QX_RIBBON_END_NAMESPACE
