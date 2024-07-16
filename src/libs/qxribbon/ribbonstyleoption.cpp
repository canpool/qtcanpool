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
    void calc();
    void calcBaseHeight();
    int calcRibbonBarHeight(RibbonBar::RibbonStyle s) const;
public:
    int m_lineSpacing;
    int m_tabBarHeight;
    int m_titleBarHeight;
    int m_ribbonBarHeightOfficeStyleThreeRow;
    int m_ribbonBarHeightWpsLiteStyleThreeRow;
    int m_ribbonBarHeightOfficeStyleTwoRow;
    int m_ribbonBarHeightWpsLiteStyleTwoRow;
};

RibbonStyleOptionPrivate::RibbonStyleOptionPrivate()
    : m_lineSpacing(17)
{
    calc();
}

void RibbonStyleOptionPrivate::calc()
{
    calcBaseHeight();
    m_ribbonBarHeightOfficeStyleThreeRow = calcRibbonBarHeight(RibbonBar::OfficeStyle);
    m_ribbonBarHeightWpsLiteStyleThreeRow = calcRibbonBarHeight(RibbonBar::WpsLiteStyle);
    m_ribbonBarHeightOfficeStyleTwoRow = calcRibbonBarHeight(RibbonBar::OfficeStyleTwoRow);
    m_ribbonBarHeightWpsLiteStyleTwoRow = calcRibbonBarHeight(RibbonBar::WpsLiteStyleTwoRow);
}

void RibbonStyleOptionPrivate::calcBaseHeight()
{
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    int m_lineSpacing = QApplication::fontMetrics().lineSpacing();
#else
    int m_lineSpacing = QFontMetricsF(QApplication::font()).lineSpacing();
#endif

    m_titleBarHeight = m_lineSpacing * 1.65;
    m_tabBarHeight = m_lineSpacing * 1.5;
}

int RibbonStyleOptionPrivate::calcRibbonBarHeight(RibbonBar::RibbonStyle s) const
{
    switch (s) {
    case RibbonBar::OfficeStyle:
        return m_titleBarHeight + m_tabBarHeight + (m_lineSpacing * 1.5) * 3 +
               RibbonGroupLayout::groupContentsMargins().top() + RibbonGroupLayout::groupContentsMargins().bottom() +
               (RibbonGroup::titleVisible() ? RibbonGroup::groupTitleHeight() : 0);
    case RibbonBar::WpsLiteStyle:
        return m_ribbonBarHeightOfficeStyleThreeRow - m_tabBarHeight;
    case RibbonBar::WpsLiteStyleTwoRow:
        return m_ribbonBarHeightOfficeStyleThreeRow - m_tabBarHeight - m_lineSpacing * 1.5;
    case RibbonBar::OfficeStyleTwoRow:
        return m_ribbonBarHeightOfficeStyleThreeRow - m_lineSpacing * 1.5;
    default:
        break;
    }
    return m_ribbonBarHeightOfficeStyleThreeRow;
}

RibbonStyleOption::RibbonStyleOption()
{
    QX_INIT_PRIVATE(RibbonStyleOption)
}

RibbonStyleOption::~RibbonStyleOption()
{
    QX_FINI_PRIVATE()
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
    d->calcBaseHeight();
    d->m_ribbonBarHeightOfficeStyleThreeRow = calcRibbonBarHeight(RibbonBar::OfficeStyle);
    d->m_ribbonBarHeightWpsLiteStyleThreeRow = calcRibbonBarHeight(RibbonBar::WpsLiteStyle);
    d->m_ribbonBarHeightOfficeStyleTwoRow = calcRibbonBarHeight(RibbonBar::OfficeStyleTwoRow);
    d->m_ribbonBarHeightWpsLiteStyleTwoRow = calcRibbonBarHeight(RibbonBar::WpsLiteStyleTwoRow);
}

int RibbonStyleOption::calcRibbonBarHeight(RibbonBar::RibbonStyle s) const
{
    Q_D(const RibbonStyleOption);
    return d->calcRibbonBarHeight(s);
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
