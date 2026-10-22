/**
 * Copyleft (C) 2023 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MIT
 **/
#include "ribbonbar.h"
#include "ribbonbar_p.h"
#include "ribbonbuttongroup.h"
#include "ribboncontrols.h"
#include "ribbonmanager.h"
#include "ribbontabbar.h"
#include "ribbonquickaccessbar.h"
#include "ribbonutils.h"

#include <QAction>
#include <QContextMenuEvent>
#include <QMenu>
#include <QWidget>

QX_RIBBON_BEGIN_NAMESPACE

/* RibbonBar */
RibbonBar::RibbonBar(QWidget *parent)
    : QMenuBar(parent)
{
    QX_INIT_PRIVATE(RibbonBar)
    Q_D(RibbonBar);
    d->init();
    if (parent) {
        connect(parent, &QWidget::windowTitleChanged, d, &RibbonBarPrivate::onWindowTitleChanged);
        connect(parent, &QWidget::windowIconChanged, d, &RibbonBarPrivate::onWindowIconChanged);
    }
    setRibbonStyle(TabBarBelowStyle);
    // Accessibility (B2): a stable accessible name for the ribbon container.
    setAccessibleName(QStringLiteral("Ribbon"));
    installRibbonAccessibility();
}

RibbonBar::~RibbonBar()
{
    QX_FINI_PRIVATE();
}

/**
 * @brief Adds the ribbon's standard context-menu entries (B10)
 *
 * "Minimize the Ribbon" mirrors RibbonBar::isMinimized() and toggles it, and
 * "Customize Quick Access Toolbar..." triggers the quick access bar's own customize
 * entry, so both the menu and the bar emit customizeDialogRequested().
 */
void RibbonBar::addContextMenuActions(QMenu *menu)
{
    QAction *minimizeAction = menu->addAction(tr("Minimize the Ribbon"));
    minimizeAction->setObjectName(QStringLiteral("qx_RibbonMinimizeAction"));
    minimizeAction->setCheckable(true);
    minimizeAction->setChecked(isMinimized());
    connect(minimizeAction, &QAction::toggled, this, &RibbonBar::setMinimized);

    RibbonQuickAccessBar *qab = quickAccessBar();
    QAction *customizeAction = menu->addAction(tr("Customize Quick Access Toolbar..."));
    customizeAction->setObjectName(QStringLiteral("qx_RibbonCustomizeQabAction"));
    customizeAction->setEnabled(qab && qab->actionCustomizeDialog());
    connect(customizeAction, &QAction::triggered, this, [qab]() {
        if (qab) {
            if (QAction *action = qab->actionCustomizeDialog()) {
                action->trigger();
            }
        }
    });
}

void RibbonBar::contextMenuEvent(QContextMenuEvent *event)
{
    QMenu menu(this);
    addContextMenuActions(&menu);
    if (!menu.isEmpty()) {
        menu.exec(event->globalPos());
    }
    event->accept();
}

QAbstractButton *RibbonBar::applicationButton()
{
    Q_D(RibbonBar);
    return d->m_applicationButton;
}

/**
 * @brief Sets the applicationButton; pass Q_NULLPTR to hide it
 *
 * There is a RibbonApplicationButton by default; pass Q_NULLPTR to cancel it, or pass a custom button
 *
 * @note RibbonBar owns the applicationButton; do not delete it externally
 * @param btn the applicationButton pointer; @ref RibbonApplicationButton may be passed
 * RibbonApplicationButton already has style settings applied
 */
void RibbonBar::setApplicationButton(QAbstractButton *btn)
{
    Q_D(RibbonBar);
    d->setApplicationButton(btn);
    resizeRibbon();
}

RibbonPage *RibbonBar::addPage(const QString &title)
{
    return insertPage(-1, title);
}

void RibbonBar::addPage(RibbonPage *page)
{
    insertPage(-1, page);
}

RibbonPage *RibbonBar::insertPage(int index, const QString &title)
{
    RibbonPage *page = new RibbonPage(this);
    page->setObjectName(title);
    page->setPageName(title);
    insertPage(index, page);
    return page;
}

void RibbonBar::insertPage(int index, RibbonPage *page)
{
    Q_D(RibbonBar);
    if (Q_NULLPTR == page) {
        return;
    }
    page->setGroupLayoutMode(isTwoRowStyle() ? RibbonGroup::TwoRowMode : RibbonGroup::ThreeRowMode);
    page->setRibbonBar(this);

    int i = d->m_tabBar->insertTab(index, page->windowTitle());
    RibbonTabData tabdata;
    tabdata.page = page;
    tabdata.index = i;
    d->m_tabBar->setTabData(i, QVariant::fromValue(tabdata));
    if (index != -1) {
        d->updateTabData();
    }

    d->m_stack->insertWidget(i, page);

    connect(page, &QWidget::windowTitleChanged, d, &RibbonBarPrivate::onPageWindowTitleChanged);
    resizeRibbon();
}

/**
 * @brief Finds a page by index; returns Q_NULLPTR if the index is out of range
 * @param index index
 * @return Q_NULLPTR if the index is out of range
 * @note If context tabs exist, their pages are returned as well
 * @note A page found by index must be visible; a tab hidden with @ref hidePage cannot be found by index
 * @note @ref pageByObjectName finds all tabs that were ever added,
 * to get all tabs managed by the ribbonbar, use @ref pages
 * @see pageIndex pageByObjectName pageByName
 */
RibbonPage *RibbonBar::page(int index) const
{
    Q_D(const RibbonBar);
    QVariant var = d->m_tabBar->tabData(index);

    if (var.isValid()) {
        RibbonTabData data = var.value<RibbonTabData>();
        return data.page;
    }
    return Q_NULLPTR;
}

/**
 * @brief Finds a Page by name
 * @param title the Page name, i.e. the tab title
 * @return Q_NULLPTR if not found; if names are duplicated, the first match is returned, so avoid duplicate tab names
 * @note The title may change due to translation, so to find a page by fixed content use @ref pageByObjectName
 * @see pageByObjectName
 */
RibbonPage *RibbonBar::pageByName(const QString &title) const
{
    Q_D(const RibbonBar);
    int c = d->m_stack->count();

    for (int i = 0; i < c; ++i) {
        RibbonPage *w = qobject_cast<RibbonPage *>(d->m_stack->widget(i));
        if (w && w->windowTitle() == title) {
            return w;
        }
    }
    return Q_NULLPTR;
}

/**
 * @brief Finds a Page by ObjectName
 * @param objname
 * @return Q_NULLPTR if not found; if the ObjectName is duplicated, the first match is returned, so avoid duplicate
 * ObjectNames
 * @see pageByName
 */
RibbonPage *RibbonBar::pageByObjectName(const QString &objname) const
{
    Q_D(const RibbonBar);
    int c = d->m_stack->count();

    for (int i = 0; i < c; ++i) {
        RibbonPage *w = qobject_cast<RibbonPage *>(d->m_stack->widget(i));
        if (w && w->objectName() == objname) {
            return (w);
        }
    }
    return Q_NULLPTR;
}

/**
 * @brief Gets all currently visible RibbonPages; RibbonPages of hidden RibbonPageContexts are also returned
 *
 * @return
 */
QList<RibbonPage *> RibbonBar::pages(bool getAll) const
{
    Q_D(const RibbonBar);
    int c = d->m_stack->count();
    QList<RibbonPage *> res;

    for (int i = 0; i < c; ++i) {
        RibbonPage *w = qobject_cast<RibbonPage *>(d->m_stack->widget(i));
        if (w) {
            if (!getAll && w->isPageContext()) {
                // Skip context pages when getAll is false
                continue;
            }
            res.append(w);
        }
    }
    return res;
}

/**
 * @brief Checks whether this page is visible, i.e. the tabbar has this page
 * @param page
 * @return
 */
bool RibbonBar::isPageVisible(RibbonPage *page) const
{
    int tabindex = pageIndex(page);

    return (tabindex >= 0);
}

/**
 * @brief Shows a hidden page
 * @param page
 */
void RibbonBar::showPage(RibbonPage *page)
{
    Q_D(RibbonBar);
    for (auto i = d->m_hidedPage.begin(); i != d->m_hidedPage.end(); ++i) {
        if (i->page == page) {   // this is the one to show
            int index = d->m_tabBar->insertTab(i->index, i->page->windowTitle());
            i->index = index;
            d->m_tabBar->setTabData(index, QVariant::fromValue(*i));
            d->m_hidedPage.erase(i);   // remove
            d->updateTabData();        // update the index information
            break;
        }
    }
    raisePage(page);
}

/**
 * @brief Hides a page without deleting or removing it
 * @param page
 */
void RibbonBar::hidePage(RibbonPage *page)
{
    Q_D(RibbonBar);
    int tabcount = d->m_tabBar->count();

    for (int i = 0; i < tabcount; ++i) {
        QVariant var = d->m_tabBar->tabData(i);
        if (var.isValid()) {
            RibbonTabData p = var.value<RibbonTabData>();
            if (p.page == page) {
                d->m_hidedPage.append(p);
                d->m_tabBar->removeTab(i);   // only remove the tab
                // After hiding the Page the context position changes, so it must be updated
                d->updateTabData();
                return;
            }
        }
    }
}

///
/// \brief Ensures the tab is shown and switches to the corresponding page
/// \param page tab pointer
///
void RibbonBar::raisePage(RibbonPage *page)
{
    Q_D(RibbonBar);
    int index = d->m_stack->indexOf(page);

    if (index >= 0) {
        setCurrentIndex(index);
    }
}

/**
 * @brief Removes a RibbonPage
 *
 * RibbonBar does not delete RibbonPage*, but the RibbonPage leaves RibbonBar's management
 * the tab is removed from the tabbar and the panel is removed; after this call the RibbonPage can be deleted
 * @param page
 */
void RibbonBar::removePage(RibbonPage *page)
{
    Q_D(RibbonBar);
    int index = d->tabIndex(page);
    bool isupdate = false;
    if (index >= 0) {
        d->m_tabBar->removeTab(index);
        isupdate = true;
    }
    d->m_stack->removeWidget(page);
    // Also check whether this page belongs to a context page
    for (RibbonPageContext *c : std::as_const(d->m_pageContextList)) {
        c->takePage(page);
    }
    // Refresh the index information of all tabdata
    if (isupdate) {
        d->updateTabData();
    }
    // Repaint after removal
    repaint();
    resizeRibbon();
}

void RibbonBar::clearPages()
{
    for (RibbonPage *page : pages(false)) {
        removePage(page);
        delete page;
    }
}

void RibbonBar::movePage(int from, int to)
{
    Q_D(RibbonBar);
    d->m_tabBar->moveTab(from, to);
    // Refresh the index information of all tabdata
    d->updateTabData();
    // This emits the tabMoved signal, which adjusts the window position in the stacked widget
}

int RibbonBar::pageIndex(RibbonPage *page) const
{
    Q_D(const RibbonBar);
    return d->tabIndex(page);
}

/**
 * @brief Adds a context tab
 *
 * A context tab is triggered on special occasions and must be triggered manually by the user
 *
 * Call @ref RibbonPageContext::addPage to add a RibbonPage to the context tab;
 * a RibbonPage added to a context tab is shown only while the context tab is shown
 * @param title the context tab title, shown with an own tab row but not with the tab bar on the title row. By default
 * the RibbonPageContext's
 * object name is also set to title
 * @param color the context tab color; if an invalid QColor() is given, the RibbonBar default color set is used
 * @return the context tab pointer
 * @note RibbonBar owns the RibbonPageContext; do not delete it externally. To delete it, call @ref
 * destroyPageContext
 */
RibbonPageContext *RibbonBar::addPageContext(const QString &title, const QColor &color)
{
    Q_D(RibbonBar);
    RibbonPageContext *context = new RibbonPageContext(this);

    context->setObjectName(title);
    context->setContextTitle(title);
    context->setContextColor(color.isValid() ? color : d->getPageContextColor());
    addPageContext(context);
    return context;
}

void RibbonBar::addPageContext(RibbonPageContext *context)
{
    Q_D(RibbonBar);
    if (Q_NULLPTR == context) {
        return;
    }
    connect(context, &RibbonPageContext::pageAdded, d, &RibbonBarPrivate::onPageContextAdded);
    // remove is not connected: after remove the page is not shown in the stacked widget either, and remove followed by
    // delete also deletes it from the stacked widget
    if (currentRibbonStyle() == TabBarInlineStyle) {
        d->resizeInTabBarInlineStyle();
    }
    d->m_pageContextList.append(context);
}

void RibbonBar::showPageContext(RibbonPageContext *context)
{
    Q_D(RibbonBar);
    if (isPageContextVisible(context)) {
        return;
    }
    PageContextManagerData pageContextData;

    pageContextData.pageContext = context;
    int cnt = context->pageCount();
    for (int i = 0; i < context->pageCount(); ++i) {
        RibbonPage *page = context->page(i);
        // Setting the same mode again performs no extra work
        page->setGroupLayoutMode(isTwoRowStyle() ? RibbonGroup::TwoRowMode : RibbonGroup::ThreeRowMode);
        // Switching the mode changes the height; the context tab must be shown when it is displayed
        // page->setFixedHeight(pageHeight());
        int index = d->m_tabBar->addTab(page->windowTitle());
        pageContextData.tabPageIndex.append(index);

        RibbonTabData tabdata;
        tabdata.page = page;
        tabdata.index = index;
        d->m_tabBar->setTabData(index, QVariant::fromValue(tabdata));
        if (cnt > 1) {
            // When adding several pages in a row, update the size right after each page; relying on the final postEvent
            // briefly shows navigation arrows
            QRect r = d->m_tabBar->geometry();
            r.setWidth(d->calcMinTabBarWidth());
            d->m_tabBar->setGeometry(r);
        }
    }
    d->m_currentShowingPageContextList.append(pageContextData);
    // Since contexts are always appended last, updateTabData() is not needed;
    resizeRibbon();
}

void RibbonBar::hidePageContext(RibbonPageContext *context)
{
    Q_D(RibbonBar);
    bool needResize = false;

    for (int i = 0; i < d->m_currentShowingPageContextList.size(); ++i) {
        if (d->m_currentShowingPageContextList[i].pageContext == context) {
            const QList<int> &indexs = d->m_currentShowingPageContextList[i].tabPageIndex;
            for (int j = indexs.size() - 1; j >= 0; --j) {
                d->m_tabBar->removeTab(indexs[j]);
            }
            // After removing a ContextPage the tab indexes change, so all tabs after it must be renumbered
            needResize = true;
            d->m_currentShowingPageContextList.removeAt(i);
            break;
        }
    }
    if (needResize) {
        d->updateTabData();
        resizeRibbon();
    }
}

bool RibbonBar::isPageContextVisible(RibbonPageContext *context)
{
    Q_D(RibbonBar);
    return d->isPageContextVisible(context);
}

void RibbonBar::setPageContextVisible(RibbonPageContext *context, bool visible)
{
    if (Q_NULLPTR == context) {
        return;
    }
    if (visible) {
        showPageContext(context);
    } else {
        hidePageContext(context);
    }
}

bool RibbonBar::isPageContextCoverTab()
{
    Q_D(RibbonBar);
    return d->m_pageContextCoverTab;
}

void RibbonBar::setPageContextCoverTab(bool cover)
{
    Q_D(RibbonBar);
    d->m_pageContextCoverTab = cover;
}

QList<RibbonPageContext *> RibbonBar::pageContextList() const
{
    Q_D(const RibbonBar);
    return d->m_pageContextList;
}

/**
 * @brief Destroys a context tab; its RibbonPages are destroyed as well
 * @param context the context tab pointer to destroy
 */
void RibbonBar::destroyPageContext(RibbonPageContext *context)
{
    Q_D(RibbonBar);
    if (Q_NULLPTR == context) {
        return;
    }
    //! 1. If the context tab is shown, hide it first
    if (isPageContextVisible(context)) {
        hidePageContext(context);
    }
    //! 2. Remove the context tab's related content
    d->m_pageContextList.removeAll(context);

    QList<RibbonPage *> res = context->pageList();
    for (RibbonPage *page : std::as_const(res)) {
        page->hide();
        page->deleteLater();
    }
    context->deleteLater();
    resizeRibbon();
}

void RibbonBar::minimize()
{
    setMinimized(true);
}

bool RibbonBar::isMinimized() const
{
    Q_D(const RibbonBar);
    return d->m_minimized;
}

void RibbonBar::setMinimized(bool flag)
{
    Q_D(RibbonBar);
    d->setMinimizedFlag(flag);
    d->updateMinimumButtonIcon();
    resizeRibbonNow();
    Q_EMIT minimizationChanged(flag);
}

bool RibbonBar::haveShowMinimumButton() const
{
    Q_D(const RibbonBar);
    return (Q_NULLPTR != d->m_minimumPageButton);
}

void RibbonBar::showMinimumButton(bool isShow)
{
    Q_D(RibbonBar);
    if (isShow) {
        cornerButtonGroup(Qt::BottomRightCorner);   // create m_bottomRightButtonGroup
        if (Q_NULLPTR == d->m_minimumPageButton) {
            d->m_minimumPageButton = RibbonElementFactory->createHideGroupButton(this);
            d->m_minimumPageButton->ensurePolished();   // load the style icon
            QAction *action = new QAction(d->m_minimumPageButton);
            connect(action, &QAction::triggered, this, [=]() {
                this->setMinimized(!isMinimized());
            });
            d->m_minimumPageButton->setDefaultAction(action);
            d->m_bottomRightButtonGroup->addWidget(d->m_minimumPageButton);
            update();
        }
        d->updateMinimumButtonIcon();
    } else {
        if (Q_NULLPTR != d->m_minimumPageButton) {
            d->m_minimumPageButton->hide();
            d->m_minimumPageButton->deleteLater();
            d->m_minimumPageButton = Q_NULLPTR;
        }
    }
    resizeRibbonNow();
}

int RibbonBar::tabBarHeight() const
{
    return RibbonElementStyleOpt.tabBarHeight();
}

int RibbonBar::titleBarHeight() const
{
    return RibbonElementStyleOpt.titleBarHeight();
}

RibbonButtonGroup *RibbonBar::cornerButtonGroup(Qt::Corner corner)
{
    Q_D(RibbonBar);
    RibbonButtonGroup *buttonGroup = Q_NULLPTR;

    switch (corner) {
    case Qt::TopLeftCorner:
        if (Q_NULLPTR == d->m_topLeftButtonGroup) {
            d->m_topLeftButtonGroup = d->createButtonGroup();
        }
        buttonGroup = d->m_topLeftButtonGroup;
        break;
    case Qt::TopRightCorner:
        if (Q_NULLPTR == d->m_topRightButtonGroup) {
            d->m_topRightButtonGroup = d->createButtonGroup();
        }
        buttonGroup = d->m_topRightButtonGroup;
        break;
    case Qt::BottomLeftCorner:
        if (Q_NULLPTR == d->m_bottomLeftButtonGroup) {
            d->m_bottomLeftButtonGroup = d->createButtonGroup();
        }
        buttonGroup = d->m_bottomLeftButtonGroup;
        break;
    case Qt::BottomRightCorner:
        if (Q_NULLPTR == d->m_bottomRightButtonGroup) {
            d->m_bottomRightButtonGroup = d->createButtonGroup();
        }
        buttonGroup = d->m_bottomRightButtonGroup;
        break;
    default:
        break;
    }
    if (buttonGroup) {
        buttonGroup->show();
    }
    return buttonGroup;
}

void RibbonBar::setCornerWidget(QWidget *widget, Qt::Corner corner)
{
    Q_D(RibbonBar);
    QMenuBar::setCornerWidget(widget, corner);
    if (widget) {
        // QMenuBar reparents a widget that has no parent yet, but a widget created
        // without one is hidden and stays hidden: setParent() does not make it
        // visible. A hidden widget takes no part in the ribbon layout
        // (takesPartInLayout), so without this it would keep the position and size it
        // was born with. QMenuBar shows the widgets it installs itself - an MDI
        // child's buttons - which is why those appear on their own.
        widget->show();
    }
    d->resizeRibbon();
}

RibbonQuickAccessBar *RibbonBar::quickAccessBar() const
{
    Q_D(const RibbonBar);
    return d->m_quickAccessBar->quickAccessBar();
}

void RibbonBar::setQuickAccessBarPosition(QuickAccessBarPosition position)
{
    Q_D(RibbonBar);
    if (d->m_quickAccessBarPosition == position) {
        return;
    }
    d->m_quickAccessBarPosition = position;
    if (!d->hasOwnTabRow()) {
        resizeRibbon();
    }
}

RibbonBar::QuickAccessBarPosition RibbonBar::quickAccessBarPosition() const
{
    Q_D(const RibbonBar);
    return d->m_quickAccessBarPosition;
}

void RibbonBar::setTabBarPosition(RibbonBar::TabBarPosition position)
{
    Q_D(RibbonBar);
    if (d->m_tabBarPosition == position) {
        return;
    }
    d->m_tabBarPosition = position;
    resizeRibbon();
}

RibbonBar::TabBarPosition RibbonBar::tabBarPosition() const
{
    Q_D(const RibbonBar);
    return d->m_tabBarPosition;
}

RibbonBar::RibbonStyle RibbonBar::currentRibbonStyle() const
{
    Q_D(const RibbonBar);
    return d->m_ribbonStyle;
}

/**
 * @brief Sets the ribbonbar style; this resets all elements, including the button layout,
 * especially when switching from three rows to two rows, where much is reset
 * @note This function triggers ResizeEvent automatically; no manual call is needed
 * @param v the style, see @ref RibbonBar::RibbonStyle
 */
void RibbonBar::setRibbonStyle(RibbonBar::RibbonStyle v)
{
    Q_D(RibbonBar);
    d->m_ribbonStyle = v;
    d->m_lastShowStyle = v;
    d->m_quickAccessBar->setIconVisible(hasOwnTabRow() && d->m_titleVisible);
    d->updateRibbonElementGeometry();

    /*
    QSize oldSize = size();
    QSize newSize(oldSize.width(), d->mainBarHeight());
    QResizeEvent es(newSize, oldSize);
    QApplication::sendEvent(this, &es);
    */
    // sendEvent prevents RibbonQuickAccessBar from updating its size after a style switch, so use postEvent instead
    // resizeRibbon();

    // NOTE: resizeRibbon() defers its work with postEvent(), and that delay is
    // observable here. Coming from TabBarInlineStyle while the bar is minimized,
    // the switch to TabBarBelowStyle reads m_tabBar->geometry().bottom() for
    // setFixedHeight() below before the tab bar has been resized, so the height
    // stays at the inline value and the horizontal strip holding the tab bar does
    // not display. Calling the resize directly is the workaround: the widget
    // style change needs the new geometry now, not next event loop pass.
    // See doc/pages/limitations.md, "RibbonBar resize is deferred".
    d->resizeRibbon();
    update();

    if (isMinimized()) {
        // In minimized mode the bar height is the tabbar bottom; this adjustment must happen after the resize event
        d->m_blockResize = true;
        setFixedHeight(d->m_tabBar->geometry().bottom());
        d->m_blockResize = false;
    }
    Q_EMIT ribbonStyleChanged(d->m_ribbonStyle);
}

int RibbonBar::currentIndex()
{
    Q_D(RibbonBar);
    return d->m_tabBar->currentIndex();
}

void RibbonBar::setCurrentIndex(int index)
{
    Q_D(RibbonBar);
    d->m_tabBar->setCurrentIndex(index);
}

bool RibbonBar::hasOwnTabRow() const
{
    Q_D(const RibbonBar);
    return d->hasOwnTabRow();
}

bool RibbonBar::isTwoRowStyle() const
{
    Q_D(const RibbonBar);
    return d->isTwoRowStyle();
}

void RibbonBar::setWindowButtonGroup(QWidget *widget)
{
    Q_D(RibbonBar);
    d->m_windowButtonGroup = widget;
}

QX_RIBBON_END_NAMESPACE
