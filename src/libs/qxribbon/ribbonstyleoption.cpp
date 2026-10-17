/**
 * Copyleft (C) 2023 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MIT
 **/
#include "ribbonstyleoption.h"
#include "ribbongrouplayout.h"
#include <QApplication>
#include <QDebug>

QX_RIBBON_BEGIN_NAMESPACE

class RibbonStyleOptionPrivate
{
    QX_DECLARE_PUBLIC(RibbonStyleOption)
public:
    RibbonStyleOptionPrivate();

    int baseHeight();
    int groupOtherHeight();
    void calc(int baseHeight);
    void recalcRibbonHeights();
public:
    int m_baseHeight = 17;
    int m_titleBarHeight = 0;
    int m_tabBarHeight = 0;
    int m_heightTabBarBelowThreeRow = 0;
    int m_heightTabBarInlineThreeRow = 0;
    int m_heightTabBarBelowTwoRow = 0;
    int m_heightTabBarInlineTwoRow = 0;
    // true = title/tab are still derived from baseHeight (recomputed in recalc); false = explicitly overridden by a
    // setter
    bool m_titleTabAuto = true;
};

RibbonStyleOptionPrivate::RibbonStyleOptionPrivate()
{
    calc(baseHeight());
}

int RibbonStyleOptionPrivate::baseHeight()
{
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    int s = QApplication::fontMetrics().lineSpacing();
#else
    int s = QFontMetricsF(QApplication::font()).lineSpacing();
#endif
    return s;
}

int RibbonStyleOptionPrivate::groupOtherHeight()
{
    int h = RibbonGroupLayout::groupContentsMargins().top() + RibbonGroupLayout::groupContentsMargins().bottom();
    if (RibbonGroup::titleVisible()) {
        return h + RibbonGroup::groupTitleHeight();
    }
    return h;
}

void RibbonStyleOptionPrivate::calc(int baseHeight)
{
    m_baseHeight = baseHeight;

    m_titleBarHeight = static_cast<int>(m_baseHeight * 1.65);
    m_tabBarHeight = static_cast<int>(m_baseHeight * 1.5);
    m_titleTabAuto = true;

    recalcRibbonHeights();
}

void RibbonStyleOptionPrivate::recalcRibbonHeights()
{
    m_heightTabBarBelowThreeRow = m_titleBarHeight + m_tabBarHeight + m_tabBarHeight * 3 + groupOtherHeight();
    m_heightTabBarInlineThreeRow = m_heightTabBarBelowThreeRow - m_tabBarHeight;
    m_heightTabBarBelowTwoRow = m_heightTabBarBelowThreeRow - m_tabBarHeight;
    m_heightTabBarInlineTwoRow = m_heightTabBarBelowThreeRow - m_tabBarHeight - m_tabBarHeight;
}

RibbonStyleOption::RibbonStyleOption()
{
    QX_INIT_PRIVATE(RibbonStyleOption);
}

RibbonStyleOption::~RibbonStyleOption()
{
    QX_FINI_PRIVATE();
}

int RibbonStyleOption::ribbonBarHeight(RibbonBar::RibbonStyle s) const
{
    Q_D(const RibbonStyleOption);
    switch (s) {
    case RibbonBar::TabBarBelowStyle:
        return d->m_heightTabBarBelowThreeRow;
    case RibbonBar::TabBarInlineStyle:
        return d->m_heightTabBarInlineThreeRow;
    case RibbonBar::TabBarBelowTwoRowStyle:
        return d->m_heightTabBarBelowTwoRow;
    case RibbonBar::TabBarInlineTwoRowStyle:
        return d->m_heightTabBarInlineTwoRow;
    default:
        break;
    }
    return d->m_heightTabBarBelowThreeRow;
}

int RibbonStyleOption::titleBarHeight() const
{
    Q_D(const RibbonStyleOption);
    return d->m_titleBarHeight;
}

int RibbonStyleOption::tabBarHeight() const
{
    Q_D(const RibbonStyleOption);
    return d->m_tabBarHeight;
}

void RibbonStyleOption::setBaseHeight(int height)
{
    Q_D(RibbonStyleOption);
    if (height > 0) {
        d->calc(height);
    }
}

void RibbonStyleOption::setTitleBarHeight(int height)
{
    Q_D(RibbonStyleOption);
    if (height > 0) {
        d->m_titleBarHeight = height;
        d->m_titleTabAuto = false;
        d->recalcRibbonHeights();
    }
}

void RibbonStyleOption::setTabBarHeight(int height)
{
    Q_D(RibbonStyleOption);
    if (height > 0) {
        d->m_tabBarHeight = height;
        d->m_titleTabAuto = false;
        d->recalcRibbonHeights();
    }
}

void RibbonStyleOption::recalc()
{
    Q_D(RibbonStyleOption);
    if (d->m_titleTabAuto) {
        d->calc(d->baseHeight());
    } else {
        d->recalcRibbonHeights();
    }
}

QDebug operator<<(QDebug debug, const RibbonStyleOption &c)
{
    QDebugStateSaver saver(debug);
    Q_UNUSED(saver);
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    int lineSpacing = QApplication::fontMetrics().lineSpacing();
#else
    int lineSpacing = QFontMetricsF(QApplication::font()).lineSpacing();
#endif
    debug.nospace() << "fontMetrics.lineSpacing=" << lineSpacing
                    << ",RibbonStyleOption(titleBarHeight=" << c.titleBarHeight()
                    << ",tabBarHeight=" << c.tabBarHeight()
                    << "\n,ribbonBarHeight(TabBarBelowStyle)=" << c.ribbonBarHeight(RibbonBar::TabBarBelowStyle)
                    << "\n,ribbonBarHeight(TabBarBelowTwoRowStyle)="
                    << c.ribbonBarHeight(RibbonBar::TabBarBelowTwoRowStyle)
                    << "\n,ribbonBarHeight(TabBarInlineStyle)=" << c.ribbonBarHeight(RibbonBar::TabBarInlineStyle)
                    << "\n,ribbonBarHeight(TabBarInlineTwoRowStyle)="
                    << c.ribbonBarHeight(RibbonBar::TabBarInlineTwoRowStyle);
    return debug;
}

QX_RIBBON_END_NAMESPACE
