/**
 * Copyright (C) 2018-2022 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
**/
#include "fancybar_p.h"
#include "fancybar.h"
#include "fancytitlebar.h"

#include "quickaccessbar.h"

#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QMenuBar>
#include <QToolBar>
#include <QToolButton>
#include <QMouseEvent>
#include <QEvent>
#include <QApplication>
#include <QStyle>

QCANPOOL_BEGIN_NAMESPACE

#define TITLE_BAR_HEIGHT    25
#define MENU_BAR_HEIGHT     25


FancyBarPrivate::FancyBarPrivate(QWidget *parent)
    : QWidget(parent)
    , m_menuBar(nullptr)
    , m_style(FancyBar::WindowStyle)
    , m_titleBarHeight(TITLE_BAR_HEIGHT)
    , m_bQuickAccessVisible(false)
    , m_bMenuBarVisible(false)
    , m_bMenuWidgetVisible(false)
{
}

FancyBarPrivate::~FancyBarPrivate()
{}


void FancyBarPrivate::init()
{
    m_logoLayout = new QHBoxLayout();
    m_logoLayout->setContentsMargins(0, 0, 0, 0);
    m_logoLayout->setSpacing(0);

    createTitleWidget();
    createMenuWidget();

    QVBoxLayout *rightLayout = new QVBoxLayout();
    rightLayout->setMargin(0);
    rightLayout->setSpacing(0);
    rightLayout->addWidget(m_titleWidget);
    rightLayout->addWidget(m_menuWidget);

    QHBoxLayout *layout = new QHBoxLayout();
    layout->setContentsMargins(1, 1, 1, 0);
    layout->setSpacing(0);
    layout->addLayout(m_logoLayout);
    layout->addLayout(rightLayout);
    setLayout(layout);

    QVBoxLayout *qlayout = new QVBoxLayout();
    qlayout->setMargin(0);
    qlayout->setSpacing(0);
    qlayout->addWidget(this);
    q->setLayout(qlayout);
}

void FancyBarPrivate::createTitleWidget()
{
    m_titleWidget = new QWidget();
    m_titleWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

    m_leftSpacerItem = new QSpacerItem(0, 0, QSizePolicy::Minimum, QSizePolicy::Minimum);
    m_rightSpacerItem = new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Minimum);

    m_quickAccessBar = new QuickAccessBar();
    m_quickAccessBar->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    m_quickAccessBar->hide();
    // to resize menuBar when quickAccessBar size was changed
    connect(m_quickAccessBar, &QuickAccessBar::customizeActionChanged, [&] () {
        if (m_style == FancyBar::MergedStyle && m_bMenuBarVisible && m_menuBar) {
            q->update();
        }
    });

    m_logoButton = m_titleBar->logoButton();
    m_titleLabel = m_titleBar->titleLabel();
    m_systemToolBar = m_titleBar->systemToolBar();

    QHBoxLayout *layout = new QHBoxLayout();
    layout->setContentsMargins(1, 0, 0, 0);
    layout->setSpacing(0);
    layout->addWidget(m_logoButton);
    layout->addWidget(m_quickAccessBar);
    layout->addItem(m_leftSpacerItem);
    layout->addWidget(m_titleLabel);
    layout->addItem(m_rightSpacerItem);
    layout->addWidget(m_systemToolBar);
    m_titleWidget->setLayout(layout);
}

void FancyBarPrivate::createMenuWidget()
{
    m_menuWidget = new QWidget();
    m_menuWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    m_menuWidget->hide();

    m_menuBarArea = new QHBoxLayout();
    m_menuBarArea->setMargin(0);
    m_menuBarArea->setSpacing(0);

    m_middleSpacerItem = new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Minimum);

    const int sz = qApp->style()->pixelMetric(QStyle::PM_SmallIconSize, Q_NULLPTR, Q_NULLPTR);
    m_toolBar = new QToolBar();
    m_toolBar->setObjectName(QLatin1String("qtc_menu_toolbar"));
    m_toolBar->setIconSize(QSize(sz, sz));

    QHBoxLayout *layout = new QHBoxLayout();
    layout->setContentsMargins(1, 0, 0, 0);
    layout->setSpacing(0);
    layout->addLayout(m_menuBarArea);
    layout->addItem(m_middleSpacerItem);
    layout->addWidget(m_toolBar);
    m_menuWidget->setLayout(layout);
}

void FancyBarPrivate::showQuickAccessBar(bool show)
{
    if (m_bQuickAccessVisible == show) {
        return;
    }
    m_bQuickAccessVisible = show;
    if (show) {
        m_quickAccessBar->show();
        m_leftSpacerItem->changeSize(0, 0, QSizePolicy::Expanding, QSizePolicy::Minimum);
    } else {
        m_quickAccessBar->hide();
        m_leftSpacerItem->changeSize(0, 0, QSizePolicy::Minimum, QSizePolicy::Minimum);
    }
}

void FancyBarPrivate::showMenuBar(bool show)
{
    if (m_bMenuBarVisible == show) {
        return;
    }
    m_bMenuBarVisible = show;
    if (m_menuBar) {
        m_menuBar->setVisible(show);
    }

    if (m_style == FancyBar::WindowStyle) {
        showMenuWidget(show);
    }
}

void FancyBarPrivate::showMenuWidget(bool show)
{
    if (m_bMenuWidgetVisible == show) {
        return;
    }
    m_bMenuWidgetVisible = show;
    m_menuWidget->setVisible(show);
}

void FancyBarPrivate::setFancyStyle(int style)
{
    if (m_style == style) {
        return;
    }
    m_style = style;
    // clear layout
    QHBoxLayout *titleLayout = qobject_cast<QHBoxLayout *>(m_titleWidget->layout());
    titleLayout->removeWidget(m_logoButton);
    titleLayout->removeWidget(m_quickAccessBar);
    titleLayout->removeItem(m_menuBarArea);
    titleLayout->removeItem(m_leftSpacerItem);
    titleLayout->removeWidget(m_titleLabel);
    titleLayout->removeItem(m_rightSpacerItem);
    titleLayout->removeWidget(m_toolBar);
    titleLayout->removeWidget(m_systemToolBar);
    QHBoxLayout *menuLayout = qobject_cast<QHBoxLayout *>(m_menuWidget->layout());
    menuLayout->removeItem(m_menuBarArea);
    menuLayout->removeItem(m_middleSpacerItem);
    menuLayout->removeWidget(m_toolBar);
    m_logoButton->setVisible(false);
    showMenuWidget(true);

    // set style
    if (style == FancyBar::DialogStyle) {
        m_logoButton->setVisible(true);
        titleLayout->addWidget(m_logoButton, 0, Qt::AlignVCenter);
        titleLayout->addWidget(m_titleLabel);
        titleLayout->addItem(m_rightSpacerItem);
        titleLayout->addWidget(m_systemToolBar);
        showMenuWidget(false);
    } else if (style == FancyBar::MergedStyle) {
        m_logoButton->setVisible(true);
        titleLayout->addWidget(m_logoButton, 0, Qt::AlignVCenter);
        titleLayout->addWidget(m_quickAccessBar);
        titleLayout->addLayout(m_menuBarArea);
        titleLayout->addItem(m_leftSpacerItem);
        titleLayout->addWidget(m_titleLabel);
        titleLayout->addItem(m_rightSpacerItem);
        titleLayout->addWidget(m_toolBar);
        titleLayout->addWidget(m_systemToolBar);
        showMenuWidget(false);
    } else {
        // default is FancyBar::WindowStyle
        m_style = FancyBar::WindowStyle;
        m_logoButton->setVisible(true);
        titleLayout->addWidget(m_logoButton, 0, Qt::AlignVCenter);
        titleLayout->addWidget(m_quickAccessBar);
        titleLayout->addItem(m_leftSpacerItem);
        titleLayout->addWidget(m_titleLabel);
        titleLayout->addItem(m_rightSpacerItem);
        titleLayout->addWidget(m_systemToolBar);
        menuLayout->addLayout(m_menuBarArea);
        menuLayout->addItem(m_middleSpacerItem);
        menuLayout->addWidget(m_toolBar);
    }
}

void FancyBarPrivate::registerWidget(QWidget *widget)
{
    m_titleBar = new FancyTitleBar(widget);
    m_titleBar->setTitleWidget(q);

    connect(m_titleBar, SIGNAL(windowResizable(bool)), q, SIGNAL(maximizationChanged(bool)));
}

QSize FancyBarPrivate::sizeHint() const
{
    int ht = 0;
    int hm = 0;
    if (m_menuBar && m_bMenuBarVisible) {
        hm = qMax(m_menuBar->sizeHint().height(), MENU_BAR_HEIGHT);
        if (m_bMenuWidgetVisible) {
            m_menuWidget->setFixedHeight(hm);
            ht = m_titleBarHeight;
        } else {
            ht = qMax(hm, m_titleBarHeight);
            hm = 0;
        }
    } else {
        ht = m_titleBarHeight;
    }
    m_titleWidget->setFixedHeight(ht);

    return QSize(width(), ht + hm);
}

FancyBar::FancyBar(QWidget *parent)
    : QWidget(parent), d(new FancyBarPrivate(this))
{
    Q_ASSERT(parent);
    setAttribute(Qt::WA_StyledBackground, true);
    setObjectName(QLatin1String("qtc_titlebar"));

    d->q = this;
    d->registerWidget(parent);
    d->init();
}

FancyBar::~FancyBar()
{
}

FancyTitleBar *FancyBar::titleBar() const
{
    return d->m_titleBar;
}

QMenuBar *FancyBar::menuBar() const
{
    if (d->m_menuBar == nullptr) {
        d->m_menuBar = new QMenuBar();
        d->m_menuBar->setNativeMenuBar(false);
        d->m_menuBar->setVisible(true);
        d->m_menuBar->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);
        d->m_menuBarArea->addWidget(d->m_menuBar);
    }

    return d->m_menuBar;
}

void FancyBar::showMenuBar(bool show)
{
    d->showMenuBar(show);
}

bool FancyBar::isMenuBarVisible() const
{
    return d->m_bMenuBarVisible;
}

QToolBar *FancyBar::toolBar() const
{
    return d->m_toolBar;
}

QuickAccessBar *FancyBar::quickAccessBar() const
{
    return d->m_quickAccessBar;
}

void FancyBar::showQuickAccess(bool show)
{
    d->showQuickAccessBar(show);
}

bool FancyBar::isQuickAccessVisible() const
{
    return d->m_bQuickAccessVisible;
}

void FancyBar::setWidgetResizable(bool resizable)
{
    d->m_titleBar->setWidgetResizable(resizable);
}

void FancyBar::setTitleBarHeight(int height)
{
    if (height > TITLE_BAR_HEIGHT && height <= TITLE_BAR_HEIGHT + 15) {
        d->m_titleBarHeight = height;
    }
}

void FancyBar::setFancyStyle(FancyBar::FancyStyle style)
{
    d->setFancyStyle(style);
}

void FancyBar::updateWidgetFlags()
{
    d->m_titleBar->updateWidgetFlags();
}

QCANPOOL_END_NAMESPACE
