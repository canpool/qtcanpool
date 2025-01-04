/**
 * Copyleft (C) 2023 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MIT
**/
#include "ribbontabbar.h"

#include <QTimer>

QX_RIBBON_BEGIN_NAMESPACE

RibbonTabBar::RibbonTabBar(QWidget *parent)
    : QTabBar(parent)
    , m_tabMargin(6, 0, 0, 0)
{
    setExpanding(false);

    // sentry tab
    addTab(QString());
}

const QMargins &RibbonTabBar::tabMargin() const
{
    return m_tabMargin;
}

void RibbonTabBar::setTabMargin(const QMargins &margin)
{
    m_tabMargin = margin;
}

int RibbonTabBar::insertTab(int index, const QString &text)
{
    int validCnt = count();
    if (index < 0 || index > validCnt) {
        index = validCnt;
    }
    int newIndex = QTabBar::insertTab(index, text);
    if (validCnt == 0) {
        // Delay setting the current tab, making sure last in the style sheet is applied to the correct tab,
        // otherwise last will be applied to the second-to-last tab
        QTimer::singleShot(0, this, [this]() {
            blockSignals(true);
            setCurrentIndex(0);
            blockSignals(false);
        });
    }
    return newIndex;
}

int RibbonTabBar::currentIndex() const
{
    int index = QTabBar::currentIndex();
    if (index == count()) {
        return -1;
    } else {
        return index;
    }
}

int RibbonTabBar::count() const
{
    // valid count does not include sentry tab
    return QTabBar::count() - 1;
}

void RibbonTabBar::setCurrentIndex(int index)
{
    if (index == -1) {
        blockSignals(true);
        QTabBar::setCurrentIndex(count());
        blockSignals(false);
    } else {
        QTabBar::setCurrentIndex(index);
    }
}

QX_RIBBON_END_NAMESPACE
