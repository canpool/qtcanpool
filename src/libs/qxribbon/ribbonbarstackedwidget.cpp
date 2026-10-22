/**
 * Copyleft (C) 2023 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MIT
 **/
#include "ribbonbar_p.h"
#include "ribboncontainers.h"
#include "ribbonquickaccessbar.h"

#include <QEvent>
#include <QLayout>
#include <QStackedWidget>
#include <QWidget>

QX_RIBBON_BEGIN_NAMESPACE

RibbonStackedWidget::RibbonStackedWidget(QWidget *parent)
    : QStackedWidget(parent)
    , m_isPopup(false)
{
}

RibbonStackedWidget::~RibbonStackedWidget()
{
}

bool RibbonStackedWidget::isPopup() const
{
    return m_isPopup;
}

void RibbonStackedWidget::setPopup(bool popup)
{
    if (m_isPopup == popup) {
        return;
    }
    m_isPopup = popup;
    if (popup) {
        setWindowFlags(Qt::Popup);
    } else {
        setWindowFlags(Qt::Widget);
    }
}

bool RibbonStackedWidget::event(QEvent *event)
{
    if (m_isPopup) {
        switch (event->type()) {
        case QEvent::Show:
            Q_EMIT aboutToShow();
            break;
        case QEvent::Hide:
            Q_EMIT aboutToHide();
            break;
        default:
            break;
        }
    }
    return QStackedWidget::event(event);
}

/**
 * @brief Like the tabbar moveTab function, swaps the indexes of two widgets
 * @param from
 * @param to
 * @note This operation emits the widgetRemoved(int index) signal
 */
void RibbonStackedWidget::moveWidget(int from, int to)
{
    QWidget *w = widget(from);

    removeWidget(w);
    insertWidget(to, w);
}

/* RibbonQuickAccessBarContainer */
RibbonQuickAccessBarContainer::RibbonQuickAccessBarContainer(QWidget *parent)
    : RibbonCtrlContainer(parent)
{
    m_quickAccessBar = new RibbonQuickAccessBar(this);
    // use QLayout::SetDefaultConstraint instread of QLayout::SetFixedSize.
    // Otherwise, the size cannot be set using setGeometry
    m_quickAccessBar->layout()->setSizeConstraint(QLayout::SetDefaultConstraint);
    m_quickAccessBar->setObjectName(QStringLiteral("qx_QuickAccessBar"));
    setWidget(m_quickAccessBar);
    setTitleVisible(false);
}

RibbonQuickAccessBarContainer::~RibbonQuickAccessBarContainer()
{
}

RibbonQuickAccessBar *RibbonQuickAccessBarContainer::quickAccessBar() const
{
    return m_quickAccessBar;
}

QX_RIBBON_END_NAMESPACE
