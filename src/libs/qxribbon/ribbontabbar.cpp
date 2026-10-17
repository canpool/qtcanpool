/**
 * Copyleft (C) 2023-2025 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MIT
 **/
#include "ribbontabbar.h"

QX_RIBBON_BEGIN_NAMESPACE

/**
 * @brief A tab bar that can also report "no page is selected"
 *
 * The ribbon shows no selected tab while it is minimized, and QTabBar has no state for that:
 * setCurrentIndex(-1) is a silent no-op, because Qt only stores an index it can resolve to a tab
 * (QTabBar::setCurrentIndex is "if (auto tab = d->at(index)) ...", and at(-1) is nullptr), so the
 * current index can be -1 only while the bar holds no tab at all.
 *
 * Hence the placeholder tab: an empty, disabled tab, always last, that can be made current without
 * anything appearing on screen. setCurrentIndex(-1) selects it, currentIndex() maps it back to -1,
 * and count() excludes it, so callers see an ordinary tab bar holding only real pages.
 */
RibbonTabBar::RibbonTabBar(QWidget *parent)
    : QTabBar(parent)
    , m_tabMargin(6, 0, 0, 0)
{
    setExpanding(false);

    QTabBar::addTab(QString());
    // Disabled, so a click cannot select it and the wheel cannot land on it: Qt moves the current
    // index with setCurrentNextEnabledIndex(), which only stops at tabs that are enabled and visible.
    setTabEnabled(nullTabIndex(), false);
}

const QMargins &RibbonTabBar::tabMargin() const
{
    return m_tabMargin;
}

void RibbonTabBar::setTabMargin(const QMargins &margin)
{
    m_tabMargin = margin;
}

int RibbonTabBar::addTab(const QString &text)
{
    return insertTab(-1, text);
}

int RibbonTabBar::insertTab(int index, const QString &text)
{
    int validCnt = count();
    if (index < 0 || index > validCnt) {
        index = validCnt;
    }
    int newIndex = QTabBar::insertTab(index, text);
    if (validCnt == 0) {
        setCurrentIndex(0);
    }
    return newIndex;
}

int RibbonTabBar::currentIndex() const
{
    const int index = QTabBar::currentIndex();
    // While the placeholder is current no page is selected, which is what -1 says.
    return index == nullTabIndex() ? -1 : index;
}

int RibbonTabBar::count() const
{
    // valid count does not include the placeholder tab
    return QTabBar::count() - 1;
}

int RibbonTabBar::nullTabIndex() const
{
    return QTabBar::count() - 1;
}

void RibbonTabBar::setCurrentIndex(int index)
{
    if (index != -1) {
        QTabBar::setCurrentIndex(index);
        return;
    }
    // Select the placeholder instead of asking Qt for -1. The signal is blocked because it would
    // carry the placeholder's index, which no page answers to.
    blockSignals(true);
    QTabBar::setCurrentIndex(nullTabIndex());
    blockSignals(false);
}

QSize RibbonTabBar::tabSizeHint(int index) const
{
    QSize sz = QTabBar::tabSizeHint(index);
    if (index == nullTabIndex()) {
        // The placeholder must take no room in the strip. Zero here rather than in a style sheet:
        // its width would otherwise be whatever padding the theme gives ::tab, 16 to 28 pixels.
        sz.setWidth(0);
    }
    sz.setHeight(this->height());

    return sz;
}

QSize RibbonTabBar::minimumTabSizeHint(int index) const
{
    QSize sz = QTabBar::minimumTabSizeHint(index);
    if (index == nullTabIndex()) {
        sz.setWidth(0);
    }

    return sz;
}

QX_RIBBON_END_NAMESPACE
