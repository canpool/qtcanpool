/***************************************************************************
 **
 **  Copyright (C) 2020 MaMinJie <canpool@163.com>
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
#include "fancyribbon.h"
#include "fancytab.h"
#include "fancytabbar.h"
#include "fancypopupwidget.h"
#include "fancyribbonpage.h"

#include <QStackedWidget>
#include <QVBoxLayout>
#include <QAction>
#include <QList>
#include <QPoint>
#include <QMouseEvent>
#include <QApplication>
#include <QMenu>

QCANPOOL_BEGIN_NAMESPACE

/* FancyRibbonPopupWidget */
class FancyRibbonPopupWidget : public FancyPopupWidget
{
    Q_OBJECT
public:
    explicit FancyRibbonPopupWidget(FancyTabBar *tabBar, QWidget *parent = nullptr);
    virtual ~FancyRibbonPopupWidget() {}

    bool isChildren(FancyTabBar *bar, FancyTab *tab);

protected:
    virtual bool event(QEvent* event);

private:
    FancyTabBar *m_tabBar;
};

FancyRibbonPopupWidget::FancyRibbonPopupWidget(FancyTabBar *tabBar, QWidget *parent)
    : FancyPopupWidget(parent)
    , m_tabBar(tabBar)
{}

bool FancyRibbonPopupWidget::isChildren(FancyTabBar *bar, FancyTab *tab)
{
    QList<FancyTab *> tabs = bar->findChildren<FancyTab *>();
    foreach (auto t, tabs) {
        if (t->type() == FancyTab::Mode && t == tab) {
            return true;
        }
    }
    return false;
}

bool FancyRibbonPopupWidget::event(QEvent *event)
{
    switch (event->type()) {
        case QEvent::MouseButtonDblClick:
        case QEvent::MouseButtonPress: {
            QMouseEvent* mouseEvent = static_cast<QMouseEvent*>(event);
            QWidget* widget = QApplication::widgetAt(mouseEvent->globalPos());
            FancyTab *tab = qobject_cast<FancyTab *>(widget);
            if (tab != nullptr && isChildren(m_tabBar, tab)) {
                if (mouseEvent->type() == QEvent::MouseButtonDblClick)
                    QMetaObject::invokeMethod(tab, "doubleClicked", Qt::QueuedConnection);
                else
                    QMetaObject::invokeMethod(tab, "clicked", Qt::QueuedConnection);
                return true;
            }
        }
        break;
        default:
        break;
    }
    return FancyPopupWidget::event(event);
}

/* FancyRibbonPrivate */
class FancyRibbonPrivate : public QObject
{
    Q_OBJECT
public:
    explicit FancyRibbonPrivate();
    virtual ~FancyRibbonPrivate();

    void init(void);

    inline bool validIndex(int index) const
    { return index >= 0 && index < m_listPages.count(); }

    void insertPage(int index, FancyRibbonPage *page);
    void removePage(int index);
    int pageIndex(FancyRibbonPage *page) const;

    int addAction(QAction *action);

public slots:
    void slotPageShow();
    void slotCurrentTabChanged(int index);
    void slotTabDoubleClicked();
    void slotPagePopupHide();
    void slotAboutToShowMenu();
    void slotAboutToHideMenu();
    void slotMenuTriggered(QMouseEvent *e);

public:
    FancyRibbon             *q;

    FancyTabBar             *m_tabBar;
    QStackedWidget          *m_pageStack;
    FancyRibbonPopupWidget  *m_pagePopupWidget;
    QAction                 *m_pageShowAction;
    FancyButton             *m_menuButton;

    QList<FancyRibbonPage *> m_listPages;

    bool m_pageShown;
};


FancyRibbonPrivate::FancyRibbonPrivate()
    : m_menuButton(nullptr)
    , m_pageShown(true)
{

}

FancyRibbonPrivate::~FancyRibbonPrivate()
{

}

void FancyRibbonPrivate::init()
{
    m_tabBar = new FancyTabBar(q);
    m_tabBar->setDirection(FancyTabBar::Horizontal);
    m_tabBar->setTabIconSize(QSize(18, 18));
    m_tabBar->setHeadSpace(0);

    m_pageShowAction = new QAction(QIcon(":/main/hide"), tr("show"), this);
    connect(m_pageShowAction, SIGNAL(triggered(bool)), this, SLOT(slotPageShow()));
    m_tabBar->addAction(m_pageShowAction);

    m_pageStack = new QStackedWidget(q);
    m_pageStack->setFixedHeight(100);
    m_pageStack->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    m_pagePopupWidget = new FancyRibbonPopupWidget(m_tabBar, q);
    QVBoxLayout *popupLayout = new QVBoxLayout();
    popupLayout->setSpacing(0);
    popupLayout->setMargin(0);
    m_pagePopupWidget->setLayout(popupLayout);

    QVBoxLayout *mainLayout = new QVBoxLayout(q);
    mainLayout->setSpacing(0);
    mainLayout->setMargin(0);
    mainLayout->addWidget(m_tabBar);
    mainLayout->addWidget(m_pageStack);

    q->setLayout(mainLayout);

    connect(m_tabBar, SIGNAL(currentChanged(int)), this, SLOT(slotCurrentTabChanged(int)));
    connect(m_tabBar, SIGNAL(currentChanged(int)), m_pageStack, SLOT(setCurrentIndex(int)));
    connect(m_pagePopupWidget, SIGNAL(aboutToHide()), this, SLOT(slotPagePopupHide()));
}

void FancyRibbonPrivate::insertPage(int index, FancyRibbonPage *page)
{
    if (validIndex(index)) {
        m_listPages.insert(index, page);
        m_pageStack->insertWidget(index, page);
    } else {
        index = m_listPages.count();
        m_listPages.append(page);
        m_pageStack->addWidget(page);
    }

    int idx = m_tabBar->insertTab(index, page->title());
    FancyTab *tab = qobject_cast<FancyTab *>(m_tabBar->tabWidget(idx));
    connect(tab, SIGNAL(doubleClicked()), this, SLOT(slotTabDoubleClicked()));
    page->setBackgroundColor(m_tabBar->pressColor());
}

void FancyRibbonPrivate::removePage(int index)
{
    if (validIndex(index)) {
        FancyRibbonPage *page = m_listPages[index];
        m_listPages.removeAt(index);
        m_tabBar->removeTab(index);
        m_pageStack->removeWidget(page);
        delete page;
    }
}

int FancyRibbonPrivate::pageIndex(FancyRibbonPage *page) const
{
    return m_listPages.indexOf(page);
}

int FancyRibbonPrivate::addAction(QAction *action)
{
    int index = m_tabBar->insertAction(m_pageShowAction, action);
    if (index != -1) {
        QMenu *menu = action->menu();
        if (menu) {
            connect(menu, SIGNAL(aboutToShow()), this, SLOT(slotAboutToShowMenu()));
            connect(menu, SIGNAL(aboutToHide()), this, SLOT(slotAboutToHideMenu()));
            FancyTab *tab = qobject_cast<FancyTab *>(m_tabBar->widgetForAction(action));
            tab->setHasMenu(true);
            connect(tab, SIGNAL(menuTriggered(QMouseEvent *)), this, SLOT(slotMenuTriggered(QMouseEvent *)));
        }
    }
    return index;
}

void FancyRibbonPrivate::slotPageShow()
{
    m_pageShown = !m_pageShown;

    if (m_pageShown) {
        m_pageShowAction->setIcon(QIcon(":/main/hide"));
        m_pageShowAction->setToolTip(tr("Hide the Ribbon"));
        m_pagePopupWidget->layout()->removeWidget(m_pageStack);
        q->layout()->addWidget(m_pageStack);
        m_tabBar->setCurrentIndex(m_tabBar->currentIndex());
    } else {
        m_pageShowAction->setIcon(QIcon(":/main/show"));
        m_pageShowAction->setToolTip(tr("Show the Ribbon"));
        q->layout()->removeWidget(m_pageStack);
        m_pagePopupWidget->layout()->addWidget(m_pageStack);
        m_tabBar->unselectCurrent();
    }
}

void FancyRibbonPrivate::slotCurrentTabChanged(int index)
{
    Q_UNUSED(index);
    if (!m_pageShown) {
        QPoint pos(q->mapToGlobal(QPoint(m_tabBar->rect().left(), m_tabBar->rect().bottom() + 1)));
        m_pagePopupWidget->setGeometry(pos.x(), pos.y(), q->width(), m_pagePopupWidget->height());
        m_pagePopupWidget->show();
    }
}

void FancyRibbonPrivate::slotTabDoubleClicked()
{
    if (!m_pageShown) {
        m_pagePopupWidget->hide();
        slotPageShow();
    }
}

void FancyRibbonPrivate::slotPagePopupHide()
{
    m_tabBar->unselectCurrent();
}

void FancyRibbonPrivate::slotAboutToShowMenu()
{
    if (m_menuButton) {
        m_menuButton->select(true);
    }
}

void FancyRibbonPrivate::slotAboutToHideMenu()
{
    if (m_menuButton) {
        m_menuButton->select(false);
    }
}

void FancyRibbonPrivate::slotMenuTriggered(QMouseEvent *e)
{
    FancyButton *button = qobject_cast<FancyButton *>(sender());
    if (button == nullptr) {
        return;
    }
    QAction *action = button->defaultAction();
    QMenu *menu = action->menu();
    if (menu == nullptr) {
        return;
    }
    m_menuButton = button;

    int x = e->x();
    int y = e->y();
    QPoint pos = e->globalPos();
    pos.setX(pos.x() - x);
    pos.setY(pos.y() - y + button->height());
    menu->popup(pos);
}

FancyRibbon::FancyRibbon(QWidget *parent)
    : QWidget(parent)
    , d(new FancyRibbonPrivate())
{
    d->q = this;
    d->init();
}

FancyRibbon::~FancyRibbon()
{
    delete d;
}

FancyRibbonPage *FancyRibbon::addPage(const QString &text)
{
    return insertPage(-1, text);
}

void FancyRibbon::addPage(FancyRibbonPage *page)
{
    return insertPage(-1, page);
}

FancyRibbonPage *FancyRibbon::insertPage(int index, const QString &text)
{
    FancyRibbonPage *page = new FancyRibbonPage(this);
    page->setTitle(text);

    insertPage(index, page);

    return page;
}

void FancyRibbon::insertPage(int index, FancyRibbonPage *page)
{
    d->insertPage(index, page);
}

void FancyRibbon::removePage(FancyRibbonPage *page)
{
    int index = d->pageIndex(page);
    if (index != -1) {
        removePage(index);
    }
}

void FancyRibbon::removePage(int index)
{
    d->removePage(index);
}

void FancyRibbon::clearPages()
{
    for (int i = pageCount() - 1; i >= 0; i--) {
        removePage(i);
    }
}

FancyRibbonPage *FancyRibbon::page(int index) const
{
    if (d->validIndex(index)) {
        return d->m_listPages[index];
    }
    return nullptr;
}

int FancyRibbon::pageCount() const
{
    return d->m_listPages.count();
}

int FancyRibbon::currentPageIndex() const
{
    return d->m_tabBar->currentIndex();
}

QList<FancyRibbonPage *> FancyRibbon::pages() const
{
    return d->m_listPages;
}

int FancyRibbon::addAction(QAction *action)
{
    if (action == nullptr) {
        return -1;
    }
    return d->addAction(action);
}

void FancyRibbon::setHoverColor(const QColor &color)
{
    d->m_tabBar->setHoverColor(color);
}

void FancyRibbon::setPressColor(const QColor &color)
{
    d->m_tabBar->setPressColor(color);
}

void FancyRibbon::setTextColor(const QColor &color)
{
    d->m_tabBar->setTextColor(color);
}

void FancyRibbon::setSelectedTextColor(const QColor &color)
{
    d->m_tabBar->setSelectedTextColor(color);
}

void FancyRibbon::setBackgroundColor(const QColor &color)
{
    d->m_tabBar->setBackgroundColor(color);
}

QCANPOOL_END_NAMESPACE

#include "fancyribbon.moc"
