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
    int groupTitleHeight();
    void calc(int baseHeight);
public:
    int m_baseHeight = 17;
    int m_tabBarHeight;
    int m_titleBarHeight;
    int m_ribbonBarHeightOfficeStyleThreeRow;
    int m_ribbonBarHeightWpsLiteStyleThreeRow;
    int m_ribbonBarHeightOfficeStyleTwoRow;
    int m_ribbonBarHeightWpsLiteStyleTwoRow;
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

int RibbonStyleOptionPrivate::groupTitleHeight()
{
    if (RibbonGroup::titleVisible()) {
        return RibbonGroupLayout::groupContentsMargins().top() + RibbonGroupLayout::groupContentsMargins().bottom() +
               RibbonGroup::groupTitleHeight();
    }
    return 0;
}

void RibbonStyleOptionPrivate::calc(int baseHeight)
{
    m_baseHeight = baseHeight;

    m_titleBarHeight = m_baseHeight * 1.65;
    m_tabBarHeight = m_baseHeight * 1.5;

    m_ribbonBarHeightOfficeStyleThreeRow = m_titleBarHeight + m_tabBarHeight + m_tabBarHeight * 3 + groupTitleHeight();
    m_ribbonBarHeightWpsLiteStyleThreeRow = m_ribbonBarHeightOfficeStyleThreeRow - m_tabBarHeight;
    m_ribbonBarHeightOfficeStyleTwoRow = m_ribbonBarHeightOfficeStyleThreeRow - m_tabBarHeight;
    m_ribbonBarHeightWpsLiteStyleTwoRow = m_ribbonBarHeightOfficeStyleThreeRow - m_tabBarHeight - m_tabBarHeight;
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
    case RibbonBar::OfficeStyle:
        return d->m_ribbonBarHeightOfficeStyleThreeRow;
    case RibbonBar::WpsLiteStyle:
        return d->m_ribbonBarHeightWpsLiteStyleThreeRow;
    case RibbonBar::OfficeStyleTwoRow:
        return d->m_ribbonBarHeightOfficeStyleTwoRow;
    case RibbonBar::WpsLiteStyleTwoRow:
        return d->m_ribbonBarHeightWpsLiteStyleTwoRow;
    default:
        break;
    }
    return d->m_ribbonBarHeightOfficeStyleThreeRow;
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

void RibbonStyleOption::recalc()
{
    Q_D(RibbonStyleOption);
    d->calc(d->baseHeight());
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
                    << "\n,ribbonBarHeight(OfficeStyle)=" << c.ribbonBarHeight(RibbonBar::OfficeStyle)
                    << "\n,ribbonBarHeight(OfficeStyleTwoRow)=" << c.ribbonBarHeight(RibbonBar::OfficeStyleTwoRow)
                    << "\n,ribbonBarHeight(WpsLiteStyle)=" << c.ribbonBarHeight(RibbonBar::WpsLiteStyle)
                    << "\n,ribbonBarHeight(WpsLiteStyleTwoRow)=" << c.ribbonBarHeight(RibbonBar::WpsLiteStyleTwoRow);
    return debug;
}

QX_RIBBON_END_NAMESPACE
