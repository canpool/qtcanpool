/**
 * Copyleft (C) 2023-2025 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MIT
**/
#include "ribbontabbar.h"

#include <QTimer>
#include <QWheelEvent>

QX_RIBBON_BEGIN_NAMESPACE

RibbonTabBar::RibbonTabBar(QWidget *parent)
    : QTabBar(parent)
    , m_tabMargin(6, 0, 0, 0)
{
    setExpanding(false);

    // sentry tab
    QTabBar::addTab(QString());
    setTabVisible(0, false);
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

void RibbonTabBar::wheelEvent(QWheelEvent *event)
{
    // prevent scrolling to the sentry tab
    if (currentIndex() == count() - 1) {
        if (event->angleDelta().y() <= 0) {
            return;
        }
    }
    QTabBar::wheelEvent(event);
}

QX_RIBBON_END_NAMESPACE
