/***************************************************************************
 **
 **  Copyright (C) 2018-2020 MaMinJie <canpool@163.com>
 **  Contact: https://github.com/canpool
 **           https://gitee.com/icanpool
 **
 **  GNU Lesser General Public License Usage
 **  Alternatively, this file may be used under the terms of the GNU Lesser
 **  General Public License version 3 as published by the Free Software
 **  Foundation and appearing in the file LICENSE.LGPL3 included in the
 **  packaging of this file. Please review the following information to
 **  ensure the GNU Lesser General Public License version 3 requirements
 **  will be met: https://www.gnu.org/licenses/lgpl-3.0.html.
 **
 **  GNU General Public License Usage
 **  Alternatively, this file may be used under the terms of the GNU
 **  General Public License version 2.0 or (at your option) the GNU General
 **  Public license version 3 or any later version approved by the KDE Free
 **  Qt Foundation. The licenses are as published by the Free Software
 **  Foundation and appearing in the file LICENSE.GPL2 and LICENSE.GPL3
 **  included in the packaging of this file. Please review the following
 **  information to ensure the GNU General Public License requirements will
 **  be met: https://www.gnu.org/licenses/gpl-2.0.html and
 **  https://www.gnu.org/licenses/gpl-3.0.html.
 **
****************************************************************************/
#include "fancytabwidget.h"
#include "fancytabbar.h"
#include "minisplitter.h"
#include <QStackedWidget>
#include <QStatusBar>
#include <QBoxLayout>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QStackedLayout>
#include <QIcon>

QCANPOOL_BEGIN_NAMESPACE

class FancyTabWidgetPrivate : public QObject
{
public:
    FancyTabWidgetPrivate();
    ~FancyTabWidgetPrivate();
    void updateTabBarPosition();
    void init();

    FancyTabBar *m_tabBar;
    QStackedWidget *m_stack;
    QStatusBar *m_statusBar;
    QWidget *m_center;

    QHBoxLayout *m_topCorner;

    FancyTabWidget *q;

    QBoxLayout *m_layout;
    FancyTabWidget::TabPosition m_pos;

    MiniSplitter *m_rightSplitter;
    MiniSplitter *m_bottomSplitter;
    MiniSplitter *m_leftSplitter;
};

FancyTabWidgetPrivate::FancyTabWidgetPrivate()
    : m_tabBar(nullptr), m_stack(nullptr), m_statusBar(nullptr), m_center(nullptr),
      m_topCorner(nullptr), q(nullptr), m_layout(nullptr), m_pos(FancyTabWidget::West)
{}

FancyTabWidgetPrivate::~FancyTabWidgetPrivate()
{}

void FancyTabWidgetPrivate::updateTabBarPosition()
{
    switch (m_pos) {
        case FancyTabWidget::North:
            m_tabBar->setDirection(FancyTabBar::Horizontal);
            m_layout->setDirection(QBoxLayout::TopToBottom);
            break;

        case FancyTabWidget::South:
            m_tabBar->setDirection(FancyTabBar::Horizontal);
            m_layout->setDirection(QBoxLayout::BottomToTop);
            break;

        case FancyTabWidget::West:
            m_tabBar->setDirection(FancyTabBar::Vertical);
            m_layout->setDirection(QBoxLayout::LeftToRight);
            break;

        case FancyTabWidget::East:
            m_tabBar->setDirection(FancyTabBar::Vertical);
            m_layout->setDirection(QBoxLayout::RightToLeft);
            break;
            /*default:*/
    }
}

/*******************************************************************************************
 *
 *  TabBar + Center
 *
 *  -------------------------------------------------------------------------|
 *  |   |                               Top                                  |
 *  |   |--------------------------------------------------------------------|
 *  |   |           |                                           |            |
 *  |   |           |                                           |            |
 *  |   |           |                                           |            |
 *  |   |           |                                           |            |
 *  |   |           |                                           |            |
 *  | M |           |                                           |            |
 *  | O |           |                                           |            |
 *  | D |           |                  Stack                    |   Right    |
 *  | E |           |                                           |            |
 *  |   |           |                                           |            |
 *  | B |    Left   |                                           |            |
 *  | A |           |                                           |            |
 *  | R |           |                                           |            |
 *  |   |           |                                           |            |
 *  |   |           |                                           |            |
 *  |   |           |--------------------------------------------------------|
 *  |   |           |                                                        |
 *  |   |           |                  Bottom                                |
 *  |   |           |                                                        |
 *  |   |           |                                                        |
 *  |   |--------------------------------------------------------------------|
 *  |   |                           Status Bar                               |
 *  -------------------------------------------------------------------------|
 *
 ******************************************************************************************/

void FancyTabWidgetPrivate::init()
{
    m_tabBar = new FancyTabBar(q);
    m_tabBar->setObjectName(QLatin1String("qcanpool_modebar"));
    m_stack = new QStackedWidget(q);
    m_stack->setObjectName(QLatin1String("qcanpool_modestack"));
    m_stack->setLineWidth(0);
    QPalette palette;
    palette.setColor(QPalette::Background, QColor(240, 240, 240, 240));
    m_stack->setPalette(palette);
    m_stack->setAutoFillBackground(true);
    connect(m_tabBar, SIGNAL(currentChanged(int)), m_stack, SLOT(setCurrentIndex(int)));
    connect(m_tabBar, SIGNAL(menuTriggered(int, QPoint)), q, SIGNAL(menuTriggered(int, QPoint)));
    connect(m_stack, SIGNAL(currentChanged(int)), q, SIGNAL(currentChanged(int)));
    m_rightSplitter = new MiniSplitter(Qt::Horizontal, MiniSplitter::Light);
    m_bottomSplitter = new MiniSplitter(Qt::Vertical, MiniSplitter::Light);
    m_leftSplitter = new MiniSplitter(Qt::Horizontal, MiniSplitter::Light);
    m_leftSplitter->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    // top, bottom, left, right corner
    QWidget *topWidget = new QWidget();
    m_topCorner = new QHBoxLayout();
    m_topCorner->setMargin(0);
    m_topCorner->setSpacing(0);
    topWidget->setLayout(m_topCorner);
    topWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    // stack + right
    m_rightSplitter->addWidget(m_stack);
    m_rightSplitter->setStretchFactor(0, 1);
    // stack + right
    // + bottom
    m_bottomSplitter->addWidget(m_rightSplitter);
    m_bottomSplitter->setStretchFactor(0, 1);
    // left + ...
    m_leftSplitter->addWidget(m_bottomSplitter);
    QVBoxLayout *centralLayout = new QVBoxLayout();
    centralLayout->setMargin(0);
    centralLayout->setSpacing(1);
    centralLayout->addWidget(topWidget);
    centralLayout->addWidget(m_leftSplitter);
    m_center = new QWidget();
    m_center->setLayout(centralLayout);
    m_layout = new QBoxLayout(QBoxLayout::LeftToRight);
    m_layout->setMargin(0);
    m_layout->setSpacing(0);
    m_layout->addWidget(m_tabBar);
    m_layout->addWidget(m_center);
    q->setLayout(m_layout);
}

FancyTabWidget::FancyTabWidget(QWidget *parent)
    : QWidget(parent), d(new FancyTabWidgetPrivate())
{
    d->q = this;
    d->init();
}

FancyTabWidget::~FancyTabWidget()
{
    delete d;
}

int FancyTabWidget::addTab(QWidget *widget, const QString &label, bool hasMenu)
{
    return insertTab(-1, widget, label, hasMenu);
}

int FancyTabWidget::addTab(QWidget *widget, const QIcon &icon, const QString &label, bool hasMenu)
{
    return insertTab(-1, widget, icon, label, hasMenu);
}

int FancyTabWidget::insertTab(int index, QWidget *widget, const QString &label, bool hasMenu)
{
    return insertTab(index, widget, QIcon(), label, hasMenu);
}

int FancyTabWidget::insertTab(int index, QWidget *widget, const QIcon &icon,
                               const QString &label, bool hasMenu)
{
    if (!widget)
        return -1;
    index = d->m_stack->insertWidget(index, widget);
    d->m_tabBar->insertTab(index, icon, label, hasMenu);

    return index;
}

void FancyTabWidget::removeTab(int index)
{
    if (QWidget *w = d->m_stack->widget(index)) {
        d->m_stack->removeWidget(w);
        d->m_tabBar->removeTab(index);
    }
}

void FancyTabWidget::setTabEnabled(int index, bool enable)
{
    d->m_tabBar->setTabEnabled(index, enable);
}

bool FancyTabWidget::isTabEnabled(int index) const
{
    return d->m_tabBar->isTabEnabled(index);
}

void FancyTabWidget::setTabVisible(int index, bool visible)
{
    if (QWidget *w = d->m_stack->widget(index)) {
        w->setHidden(!visible);
        d->m_tabBar->setTabVisible(index, visible);
    }
}

bool FancyTabWidget::isTabVisible(int index) const
{
    return d->m_tabBar->isTabVisible(index);
}

void FancyTabWidget::setTabToolTip(int index, QString toolTip)
{
    d->m_tabBar->setTabToolTip(index, toolTip);
}

QString FancyTabWidget::tabToolTip(int index) const
{
    return d->m_tabBar->tabToolTip(index);
}

void FancyTabWidget::setTabShortcut(int index, const QString &shortcut)
{
    d->m_tabBar->setTabShortcut(index, shortcut);
}

FancyTabWidget::TabPosition FancyTabWidget::tabPosition() const
{
    return d->m_pos;
}

void FancyTabWidget::setTabPosition(FancyTabWidget::TabPosition pos)
{
    if (d->m_pos == pos) {
        return;
    }
    d->m_pos = pos;
    d->updateTabBarPosition();
}

int FancyTabWidget::currentIndex() const
{
    return d->m_tabBar->currentIndex();
}

QStatusBar *FancyTabWidget::statusBar() const
{
    if (d->m_statusBar == nullptr) {
        d->m_statusBar = new QStatusBar;
        d->m_center->layout()->addWidget(d->m_statusBar);
    }
    return d->m_statusBar;
}

FancyTabBar *FancyTabWidget::tabBar() const
{
    return d->m_tabBar;
}

void FancyTabWidget::setTabStyle(FancyTabWidget::TabType type, FancyTabWidget::TabStyle style)
{
    d->m_tabBar->setTabStyle(static_cast<FancyTabBar::TabType>(type),
                             static_cast<FancyTabBar::TabStyle>(style));
}

void FancyTabWidget::setTabFont(FancyTabWidget::TabType type, QFont &font)
{
    d->m_tabBar->setTabFont(static_cast<FancyTabBar::TabType>(type), font);
}

void FancyTabWidget::setTabSpace(FancyTabWidget::TabType type, int space)
{
    d->m_tabBar->setTabSpace(static_cast<FancyTabBar::TabType>(type), space);
}

void FancyTabWidget::setTabIconSize(QSize size)
{
    d->m_tabBar->setTabIconSize(size);
}

int FancyTabWidget::addAction(QAction *action, FancyTabWidget::ActionPosition position)
{
    return d->m_tabBar->addAction(action, static_cast<FancyTabBar::ActionPosition>(position));
}

void FancyTabWidget::removeAction(QAction *action)
{
    d->m_tabBar->removeAction(action);
}

void FancyTabWidget::setActionStyle(QAction *action, FancyTabWidget::TabStyle style)
{
    d->m_tabBar->setActionStyle(action, static_cast<FancyTabBar::TabStyle>(style));
}

QSplitter *FancyTabWidget::addCornerWidget(QWidget *widget, FancyTabWidget::CornerPosition position)
{
    QSplitter *splitter = nullptr;

    switch (position) {
        case Left: {
            int count = d->m_leftSplitter->count();
            d->m_leftSplitter->insertWidget(count - 1, widget);
            splitter = d->m_leftSplitter;
            break;
        }
        case Top:
            d->m_topCorner->addWidget(widget);
            break;
        case Right:
            d->m_rightSplitter->addWidget(widget);
            splitter = d->m_rightSplitter;
            break;
        case Bottom:
            d->m_bottomSplitter->addWidget(widget);
            splitter = d->m_bottomSplitter;
            break;
    }

    return splitter;
}

void FancyTabWidget::setHoverColor(const QColor &color)
{
    d->m_tabBar->setHoverColor(color);
}

QColor FancyTabWidget::hoverColor() const
{
    return d->m_tabBar->hoverColor();
}

void FancyTabWidget::setPressColor(const QColor &color)
{
    d->m_tabBar->setPressColor(color);
}

QColor FancyTabWidget::pressColor() const
{
    return d->m_tabBar->pressColor();
}

void FancyTabWidget::setTextColor(const QColor &color)
{
    d->m_tabBar->setTextColor(color);
}

void FancyTabWidget::setSelectedTextColor(const QColor &color)
{
    d->m_tabBar->setSelectedTextColor(color);
}

void FancyTabWidget::setBackgroundColor(const QColor &color)
{
    d->m_tabBar->setBackgroundColor(color);
}

void FancyTabWidget::setCurrentIndex(int index)
{
    d->m_tabBar->setCurrentIndex(index);
}

void FancyTabWidget::hideMenu(int index)
{
    d->m_tabBar->hideMenu(index);
}

QCANPOOL_END_NAMESPACE
