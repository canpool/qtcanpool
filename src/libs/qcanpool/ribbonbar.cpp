/**
 * Copyright (C) 2020-2022 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
**/
#include "ribbonbar.h"
#include "ribbonbar_p.h"
#include "ribbonpage.h"
#include "quickaccessbar.h"
#include "fancytitlebar.h"

#include <QTabBar>
#include <QToolBar>
#include <QToolButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QAction>
#include <QList>
#include <QPoint>
#include <QMouseEvent>
#include <QApplication>
#include <QMenu>
#include <QStyle>
#include <QLabel>

QCANPOOL_BEGIN_NAMESPACE

/* RibbonStackedWidget */
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

/* RibbonBarPrivate */
RibbonBarPrivate::RibbonBarPrivate()
    : m_mainWidget(nullptr)
    , m_minimized(false)
    , m_frameless(false)
    , m_style(RibbonBar::ClassicStyle)
{

}

RibbonBarPrivate::~RibbonBarPrivate()
{

}

void RibbonBarPrivate::init()
{
    m_titleWidget = new QWidget();

    m_titleBar = new FancyTitleBar(m_mainWidget);
    m_titleBar->setDisabled(!m_frameless);
    m_titleBar->setTitleWidget(m_titleWidget);

    m_tabBar = new QTabBar(q);
    m_tabBar->setExpanding(false);
    m_tabBar->setDrawBase(false);

    m_baseToolBar = new QToolBar(q);
    m_baseToolBar->setIconSize(m_tabBar->iconSize());
    m_baseToolBar->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);

    m_minimizeIcon = QIcon(":/res/arrow_down.png");
    m_normalIcon = QIcon(":/res/arrow_up.png");
    m_minimizeAction = new QAction(m_normalIcon, "minmax", this);
    connect(m_minimizeAction, SIGNAL(triggered(bool)), this, SLOT(showMinMaximized()));
    m_baseToolBar->addAction(m_minimizeAction);
    m_minimizeButton = qobject_cast<QToolButton *>(m_baseToolBar->widgetForAction(m_minimizeAction));
    m_minimizeButton->setObjectName(QLatin1String("minmax"));

    m_stack = new RibbonStackedWidget(q);
#ifdef Q_OS_LINUX
    m_stack->setFixedHeight(102);
#else
    m_stack->setFixedHeight(94);
#endif
    m_stack->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    m_stack->installEventFilter(q);

    m_quickAccessBar = new QuickAccessBar();

    const int sz = q->style()->pixelMetric(QStyle::PM_SmallIconSize, nullptr, q);
    m_topToolBar = new QToolBar();
    m_topToolBar->setIconSize(QSize(sz, sz));
    m_bottomToolBar = new QToolBar();
    m_bottomToolBar->setIconSize(QSize(sz, sz));

    m_topLayout = new QHBoxLayout();
    m_topLayout->setContentsMargins(1, 0, 0, 0);
    m_topLayout->setSpacing(0);

    m_bottomLayout = new QHBoxLayout();
    m_bottomLayout->setContentsMargins(0, 0, 0, 0);
    m_bottomLayout->setSpacing(0);

    QVBoxLayout *titleLayout = new QVBoxLayout();
    titleLayout->setContentsMargins(0, 0, 0, 0);
    titleLayout->setSpacing(0);
    titleLayout->addLayout(m_topLayout);
    titleLayout->addLayout(m_bottomLayout);
    m_titleWidget->setLayout(titleLayout);

    m_mainLayout = new QVBoxLayout();
    m_mainLayout->setContentsMargins(0, 0, 0, 0);
    m_mainLayout->setSpacing(0);
    m_mainLayout->addWidget(m_titleWidget);
    m_mainLayout->addWidget(m_stack);
    q->setLayout(m_mainLayout);

    setRibbonStyle(m_style);

    connect(m_tabBar, SIGNAL(currentChanged(int)), this, SLOT(slotCurrentTabChanged(int)));
    connect(m_tabBar, SIGNAL(tabBarClicked(int)), this, SLOT(slotTabBarClicked(int)));
    connect(m_tabBar, SIGNAL(tabBarDoubleClicked(int)), this, SLOT(slotTabBarDoubleClicked(int)));
    connect(m_tabBar, SIGNAL(currentChanged(int)), m_stack, SLOT(setCurrentIndex(int)));
    connect(m_stack, SIGNAL(aboutToHide()), this, SLOT(slotPagePopupHide()));
}

/**
 * @brief RibbonBarPrivate::setRibbonStyle
 * @param style
 *
 * ClassicStyle:
 *  ------------------------------------------------------------------------------------
 *  | *Logo | QuickAccessBar |          *Title          | TopToolBar | *SysButtonGroup |
 *  ------------------------------------------------------------------------------------
 *  | AppButton |                        TabBar         | BottomToolBar | BaseToolBar  |
 *  ------------------------------------------------------------------------------------
 *  |                                    STACK                                         |
 *  |~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~|
 *
 * MergedStyle:
 *  ------------------------------------------------------------------------------------
 *  | *Logo | AppButton | TabBar | *Title | QuickAccessBar | ToolBar | *SysButtonGroup |
 *  ------------------------------------------------------------------------------------
 *  |                                    STACK                                         |
 *  |~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~------------------~~~|
 *
 * 1) *Logo,*Title,*SysButtonGroup -- Customize TitleBar elements
 * 2) ToolBar - TopToolBar, BottomToolBar, BaseToolBar
 *
 */
void RibbonBarPrivate::setRibbonStyle(RibbonBar::RibbonStyle style)
{
    cleanLayout(m_topLayout);
    cleanLayout(m_bottomLayout);

    if (style == RibbonBar::ClassicStyle) {
        if (m_frameless) {
            m_topLayout->addWidget(m_titleBar->logoButton());
        }
        m_topLayout->addWidget(m_quickAccessBar);
        m_topLayout->addStretch();
        if (m_frameless) {
            m_topLayout->addWidget(m_titleBar->titleLabel());
            m_topLayout->addStretch();
        }
        m_topLayout->addWidget(m_topToolBar);
        if (m_frameless) {
            m_topLayout->addWidget(m_titleBar->systemToolBar());
        }

        // m_bottomLayout->addWidget(m_appButton);
        m_bottomLayout->addWidget(m_tabBar);
        m_bottomLayout->addStretch();
        m_bottomLayout->addWidget(m_bottomToolBar);
        m_bottomLayout->addWidget(m_baseToolBar);
    } else if (style == RibbonBar::MergedStyle) {
        if (m_frameless) {
            m_topLayout->addWidget(m_titleBar->logoButton());
        }
        // m_topLayout->addWidget(m_appButton);
        m_topLayout->addWidget(m_tabBar);
        m_topLayout->addStretch();
        if (m_frameless) {
            m_topLayout->addWidget(m_titleBar->titleLabel());
            m_topLayout->addStretch();
        }
        m_topLayout->addWidget(m_quickAccessBar);
        m_topLayout->addWidget(m_topToolBar);
        m_topLayout->addWidget(m_bottomToolBar);
        m_topLayout->addWidget(m_baseToolBar);
        if (m_frameless) {
            m_topLayout->addWidget(m_titleBar->systemToolBar());
        }
    }

    if (!m_titleBar->logoButton()->icon().isNull()) {
        m_titleBar->logoButton()->setVisible(m_frameless);
    }
    m_titleBar->titleLabel()->setText(m_mainWidget->windowTitle());
    m_titleBar->titleLabel()->setVisible(m_frameless);
    m_titleBar->systemToolBar()->setVisible(m_frameless);
}

void RibbonBarPrivate::insertPage(int index, RibbonPage *page)
{
    if (validateIndex(index)) {
        m_listPages.insert(index, page);
        m_stack->insertWidget(index, page);
    } else {
        index = m_listPages.count();
        m_listPages.append(page);
        m_stack->addWidget(page);
    }
}

void RibbonBarPrivate::removePage(int index)
{
    if (validateIndex(index)) {
        RibbonPage *page = m_listPages[index];
        m_listPages.removeAt(index);
        m_tabBar->removeTab(index);
        m_stack->removeWidget(page);
        delete page;
    }
}

int RibbonBarPrivate::pageIndex(RibbonPage *page) const
{
    return m_listPages.indexOf(page);
}

int RibbonBarPrivate::addAction(QAction *action)
{
    m_baseToolBar->insertAction(m_minimizeAction, action);
    return 0;
}

void RibbonBarPrivate::showPagePopup()
{
    QPoint pos(q->mapToGlobal(QPoint(m_titleWidget->rect().left(), m_titleWidget->rect().bottom() + 1)));
    m_stack->setGeometry(pos.x(), pos.y(), q->width(), m_stack->height());
    m_stack->show();
}

void RibbonBarPrivate::showMinMaximized()
{
    m_minimized = !m_minimized;

    if (m_minimized) {
        // to be normal
        m_minimizeButton->setProperty("state", "max");
        m_minimizeAction->setIcon(m_minimizeIcon);
        m_minimizeAction->setToolTip(tr("Hide the Ribbon"));
        m_stack->hide();
        m_mainLayout->removeWidget(m_stack);
        m_stack->setPopup(true);
    } else {
        // to be minimized
        m_minimizeButton->setProperty("state", "min");
        m_minimizeAction->setIcon(m_normalIcon);
        m_minimizeAction->setToolTip(tr("Show the Ribbon"));
        m_stack->setPopup(false);
        m_mainLayout->addWidget(m_stack);
        m_stack->show();
        m_tabBar->setCurrentIndex(m_tabBar->currentIndex());
    }
    m_minimizeButton->style()->polish(m_minimizeButton);
}

void RibbonBarPrivate::cleanLayout(QLayout *layout)
{
    QLayoutItem *child;
    while ((child = layout->takeAt(0)) != nullptr) {
        if (child->widget()) {
            child->widget()->setParent(nullptr);
        } else if(child->layout()){
            cleanLayout(child->layout());
        }
    }
}

void RibbonBarPrivate::slotCurrentTabChanged(int index)
{
    Q_UNUSED(index);
    if (m_minimized) {
        showPagePopup();
    }
}

void RibbonBarPrivate::slotTabBarClicked(int index)
{
    if (index != m_tabBar->currentIndex()) {
        return;
    }
    if (m_minimized && m_stack->isHidden()) {
        showPagePopup();
    }
}

void RibbonBarPrivate::slotTabBarDoubleClicked(int index)
{
    Q_UNUSED(index);
    showMinMaximized();
}

void RibbonBarPrivate::slotPagePopupHide()
{

}

RibbonBar::RibbonBar(QWidget *parent)
    : QWidget(parent)
    , d(new RibbonBarPrivate())
{
    Q_ASSERT(parent);
    d->q = this;
    d->m_mainWidget = parent;
    d->init();
}

RibbonBar::~RibbonBar()
{
    delete d;
}

RibbonPage *RibbonBar::addPage(const QString &text)
{
    return insertPage(-1, text);
}

void RibbonBar::addPage(RibbonPage *page)
{
    return insertPage(-1, page);
}

RibbonPage *RibbonBar::insertPage(int index, const QString &text)
{
    return insertPage(index, QIcon(), text);
}

RibbonPage *RibbonBar::insertPage(int index, const QIcon &icon, const QString &text)
{
    int idxTab = d->m_tabBar->insertTab(index, icon, text);
    RibbonPage *page = new RibbonPage(this);
    page->setTitle(text);
    d->insertPage(idxTab, page);
    return page;
}

void RibbonBar::insertPage(int index, RibbonPage *page)
{
    Q_ASSERT(page != nullptr);
    if (d->pageIndex(page) != -1) {
        qWarning("eRibbonBar::insertPage: Can't add page twice.");
        return;
    }
    page->setParent(this);
    int idxTab = d->m_tabBar->insertTab(index, page->title());
    d->insertPage(idxTab, page);
}

void RibbonBar::removePage(RibbonPage *page)
{
    int index = d->pageIndex(page);
    if (index != -1) {
        removePage(index);
    }
}

void RibbonBar::removePage(int index)
{
    d->removePage(index);
}

void RibbonBar::clearPages()
{
    for (int i = pageCount() - 1; i >= 0; i--) {
        removePage(i);
    }
}

RibbonPage *RibbonBar::page(int index) const
{
    if (d->validateIndex(index)) {
        return d->m_listPages[index];
    }
    return nullptr;
}

int RibbonBar::pageCount() const
{
    return d->m_listPages.count();
}

int RibbonBar::currentPageIndex() const
{
    return d->m_tabBar->currentIndex();
}

QList<RibbonPage *> RibbonBar::pages() const
{
    return d->m_listPages;
}

int RibbonBar::addAction(QAction *action)
{
    if (action == nullptr) {
        return -1;
    }
    return d->addAction(action);
}

QuickAccessBar *RibbonBar::quickAccessBar() const
{
    return d->m_quickAccessBar;
}

RibbonBar::RibbonStyle RibbonBar::ribbonStyle() const
{
    return d->m_style;
}

void RibbonBar::setRibbonStyle(RibbonBar::RibbonStyle style)
{
    d->m_style = style;
    d->setRibbonStyle(style);
}

QToolBar *RibbonBar::toolBar(RibbonBar::ToolBarPosition position) const
{
    if (RibbonBar::ToolBarTopPosition == position) {
        return d->m_topToolBar;
    } else {
        return d->m_bottomToolBar;
    }
}

FancyTitleBar *RibbonBar::titleBar() const
{
    return d->m_titleBar;
}

void RibbonBar::updateParentFlags()
{
    QWidget *pw = d->m_mainWidget;
    if (pw == nullptr) {
        return;
    }
    d->m_frameless = (pw->windowFlags() & Qt::FramelessWindowHint);
    d->m_titleBar->updateWidgetFlags();
    d->m_titleBar->setDisabled(!d->m_frameless);
    d->setRibbonStyle(d->m_style);
}

bool RibbonBar::eventFilter(QObject *object, QEvent *event)
{
    if (object == d->m_stack && d->m_stack->isPopup()) {
        switch (event->type()) {
        case QEvent::MouseButtonDblClick:
        case QEvent::MouseButtonPress: {
            QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);
            if (d->m_stack->rect().contains(mouseEvent->pos())) {
                break;
            }
            QWidget *widget = QApplication::widgetAt(mouseEvent->globalPos());
            if (widget == d->m_tabBar) {
                const QPoint pos = widget->mapFromGlobal(mouseEvent->globalPos());
                QMouseEvent *me = new QMouseEvent(mouseEvent->type(), pos, mouseEvent->globalPos(),
                    mouseEvent->button(), mouseEvent->buttons(), mouseEvent->modifiers());
                QApplication::postEvent(widget, me);
                return true;
            }
            break;
        }
        default:
            break;
        }
    }
    return QWidget::eventFilter(object, event);
}

QCANPOOL_END_NAMESPACE
