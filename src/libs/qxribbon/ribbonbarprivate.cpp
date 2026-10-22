/**
 * Copyleft (C) 2023 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MIT
 **/
#include "ribbonbar.h"
#include "ribbonbar_p.h"
#include "ribbonapplicationbutton.h"
#include "ribbonbuttongroup.h"
#include "ribboncontrols.h"
#include "ribbongallery.h"
#include "ribbonkeytip.h"
#include "ribbonmanager.h"
#include "ribbontabbar.h"
#include "ribbonutils.h"

#include <QAction>
#include <QApplication>
#include <QColor>
#include <QDebug>
#include <QIcon>
#include <QKeyEvent>
#include <QMenu>
#include <QTimer>
#include <QWidget>

QX_RIBBON_BEGIN_NAMESPACE

// The theme sheet dresses the application button by this property as well as by
// its object name; see setApplicationButton(). The name is what a selector in
// tools/themes/*.css spells in square brackets.
static const char kApplicationButtonProperty[] = "qxRibbonApplicationButton";
RibbonBarPrivate::RibbonBarPrivate()
    : m_applicationButton(Q_NULLPTR)
    , m_tabBar(Q_NULLPTR)
    , m_stack(Q_NULLPTR)
    , m_iconRightBorderPosition(1)
    , m_minimumPageButton(Q_NULLPTR)
    , m_topLeftButtonGroup(Q_NULLPTR)
    , m_topRightButtonGroup(Q_NULLPTR)
    , m_bottomLeftButtonGroup(Q_NULLPTR)
    , m_bottomRightButtonGroup(Q_NULLPTR)
    , m_windowButtonGroup(Q_NULLPTR)
    , m_quickAccessBarPosition(RibbonBar::QABRightPosition)
    , m_tabBarPosition(RibbonBar::TBLeftPosition)
    , m_ribbonStyle(RibbonBar::TabBarBelowStyle)
    , m_lastShowStyle(RibbonBar::TabBarBelowStyle)
    , m_pageContextColorListIndex(-1)
    , m_titleAligment(Qt::AlignCenter)
    , m_minimized(true)
    , m_titleVisible(true)
    , m_pageContextCoverTab(false)
    , m_blockResize(false)
    , m_keyTipMode(false)
{
    m_pageContextColorList << QColor(201, 89, 156)   // Rose red
                           << QColor(242, 203, 29)   // Yellow
                           << QColor(255, 157, 0)    // Orange
                           << QColor(14, 81, 167)    // Blue
                           << QColor(228, 0, 69)     // Red
                           << QColor(67, 148, 0)     // Green
        ;
}

void RibbonBarPrivate::init()
{
    Q_Q(RibbonBar);
    // The private class derives QObject without Q_OBJECT, so a bare tr() here
    // resolves to QObject::tr() while lupdate files it under
    // RibbonBarPrivate - two different answers, and neither finds the other.
    // RibbonBar is the class the button belongs to, so name it.
    m_applicationButton = new RibbonApplicationButton(QCoreApplication::translate("RibbonBar", "app"), q);
    connect(m_applicationButton, &QAbstractButton::clicked, q, &RibbonBar::applicationButtonClicked);
    dressApplicationButton(m_applicationButton);
    m_tabBar = new RibbonTabBar(q);
    m_tabBar->setObjectName(QStringLiteral("qx_RibbonTabBar"));
    m_tabBar->setDrawBase(false);
    connect(m_tabBar, &QTabBar::currentChanged, this, &RibbonBarPrivate::onCurrentRibbonTabChanged);
    connect(m_tabBar, &QTabBar::tabBarClicked, this, &RibbonBarPrivate::onCurrentRibbonTabClicked);
    connect(m_tabBar, &QTabBar::tabBarDoubleClicked, this, &RibbonBarPrivate::onCurrentRibbonTabDoubleClicked);
    connect(m_tabBar, &QTabBar::tabMoved, this, &RibbonBarPrivate::onTabMoved);

    m_tabBarBaseLine = new QWidget(q);
    m_tabBarBaseLine->setObjectName(QStringLiteral("qx_RibbonTabBarBaseLine"));
    m_tabBarBaseLine->setFixedHeight(tabBarBaseLineHeight);
    m_tabBarBaseLine->setVisible(false);

    m_stack = new RibbonStackedWidget(q);
    m_stack->setObjectName(QStringLiteral("qx_RibbonStackedWidget"));
    connect(m_stack, &RibbonStackedWidget::aboutToHide, this, &RibbonBarPrivate::onStackWidgetHided);
    m_stack->installEventFilter(q);
    setMinimizedFlag(false);

    m_quickAccessBar = new RibbonQuickAccessBarContainer(q);
    m_quickAccessBar->setObjectName(QStringLiteral("qx_RibbonQuickAccessBar"));
    m_quickAccessBar->setIcon(q->windowIcon());
}

void RibbonBarPrivate::setApplicationButton(QAbstractButton *btn)
{
    Q_Q(RibbonBar);
    if (m_applicationButton) {
        delete m_applicationButton;
    }
    if (btn) {
        if (btn->parent() != q) {
            btn->setParent(q);
        }
        btn->move(0, q->titleBarHeight());
        connect(btn, &QAbstractButton::clicked, q, &RibbonBar::applicationButtonClicked);

        dressApplicationButton(btn);
        btn->setVisible(true);
    }
    m_applicationButton = btn;
}

void RibbonBarPrivate::dressApplicationButton(QAbstractButton *btn)
{
    if (!btn) {
        return;
    }
    if (btn->objectName().isEmpty()) {
        btn->setObjectName(QStringLiteral("qx_RibbonApplicationButton"));
    }
    // The theme sheet dresses the application button by this property as well
    // as by its object name, so one the application named itself is dressed too
    // - the name is the application's to choose, and a sheet that only answered
    // to ours would leave its button undressed. Polished again because a sheet
    // installed before this call has already been matched against the button
    // without the property.
    btn->setProperty(kApplicationButtonProperty, true);
    btn->style()->unpolish(btn);
    btn->style()->polish(btn);
}

bool RibbonBarPrivate::isPageContextVisible(RibbonPageContext *pageContext)
{
    for (int i = 0; i < m_currentShowingPageContextList.size(); ++i) {
        if (m_currentShowingPageContextList[i] == pageContext) {   // operator is overridden
            return true;
        }
    }
    return false;
}

void RibbonBarPrivate::setMinimizedFlag(bool flag)
{
    Q_Q(RibbonBar);
    if (m_minimized == flag) {
        return;
    }
    m_minimized = flag;

    if (flag) {
        m_stack->setPopup(true);
        m_stack->setFocusPolicy(Qt::NoFocus);
        m_stack->clearFocus();
        m_tabBar->setFocus();
        m_stack->hide();
        m_blockResize = true;
        q->setFixedHeight(m_tabBar->geometry().bottom());
        m_blockResize = false;
    } else {
        m_stack->setPopup(false);
        m_stack->setFocus();
        m_stack->show();
    }
}

void RibbonBarPrivate::updateMinimumButtonIcon()
{
    Q_Q(RibbonBar);
    if (m_minimumPageButton) {
        QAction *action = m_minimumPageButton->defaultAction();
        if (action) {
            // A mask painted in the palette's text colour, like the gallery's scroll
            // arrows and the group's option button. The stock SP_TitleBarShadeButton
            // this used to ask for is black whatever the palette says, so on a dark
            // sheet the button was the one glyph that stayed invisible; painted from
            // the palette it follows a theme switch, which the PaletteChange arm of
            // RibbonBar::event() re-runs this for.
            //
            // The bar's palette and not the button's: this runs from the bar's own
            // PaletteChange, where the bar already carries the new palette and the
            // button may not have inherited it yet.
            const QColor color = q->palette().color(QPalette::ButtonText);
            action->setIcon(tintedIcon(m_minimized ? QStringLiteral(":/qxribbon/res/arrow_down.png")
                                                   : QStringLiteral(":/qxribbon/res/arrow_up.png"),
                                       color));
            // The button is an icon and nothing else, so the tooltip is the only place
            // that says what it does.
            action->setToolTip(m_minimized ? RibbonBar::tr("Pin the Ribbon") : RibbonBar::tr("Minimize the Ribbon"));
        }
    }
}

QColor RibbonBarPrivate::getPageContextColor()
{
    if (m_pageContextColorList.isEmpty()) {
        m_pageContextColorListIndex = -1;
        return QColor();
    }
    ++m_pageContextColorListIndex;
    if ((m_pageContextColorListIndex >= m_pageContextColorList.size()) || (m_pageContextColorListIndex < 0)) {
        m_pageContextColorListIndex = 0;
    }
    return m_pageContextColorList.at(m_pageContextColorListIndex);
}

void RibbonBarPrivate::updateTabData()
{
    int tabcount = m_tabBar->count();

    for (int i = 0; i < tabcount; ++i) {
        QVariant var = m_tabBar->tabData(i);
        if (var.isValid()) {
            RibbonTabData data = var.value<RibbonTabData>();
            data.index = i;
            m_tabBar->setTabData(i, QVariant::fromValue(data));
        }
    }
    // After refreshing the tabdata, also refresh the ContextPage information
    updatePageContextManagerData();
}

/**
 * @brief Refreshes all ContextPageManagerData; called when a single Page is removed
 */
void RibbonBarPrivate::updatePageContextManagerData()
{
    const int c = m_tabBar->count();

    for (PageContextManagerData &cd : m_currentShowingPageContextList) {
        cd.tabPageIndex.clear();
        for (int i = 0; i < cd.pageContext->pageCount(); ++i) {
            RibbonPage *page = cd.pageContext->page(i);
            for (int t = 0; t < c; ++t) {
                QVariant v = m_tabBar->tabData(t);
                if (v.isValid()) {
                    RibbonTabData data = v.value<RibbonTabData>();
                    if (data.page == page) {
                        cd.tabPageIndex.append(t);
                    }
                } else {
                    cd.tabPageIndex.append(-1);
                }
            }
        }
    }
}

void RibbonBarPrivate::updateRibbonElementGeometry()
{
    Q_Q(RibbonBar);
    // Adjust the RibbonPage layout mode according to the style
    QList<RibbonPage *> pages = q->pages();
    for (RibbonPage *page : std::as_const(pages)) {
        page->setGroupLayoutMode(isTwoRowStyle() ? RibbonGroup::TwoRowMode : RibbonGroup::ThreeRowMode);
    }
    // Prevent a height change from triggering resizeEvent, which would run resizeXX
    m_blockResize = true;
    updateRibbonBarHeight();
    m_blockResize = false;
}

void RibbonBarPrivate::updateRibbonBarHeight()
{
    Q_Q(RibbonBar);
    // Adjust the bar height according to the style
    if (m_minimized) {
        // In minimized mode the bar height is the bottom of the tabbar
        // In minimized mode the bar height is already adjusted in setMinimizedFlag
        // q->setFixedHeight(m_tabBar->geometry().bottom());
    } else {
        q->setFixedHeight(mainBarHeight());
    }
}

/**
 * @brief Pops up the stack in minimized mode: finish the tab bar hover state, then position, focus and show it
 */
void RibbonBarPrivate::showPopupStack()
{
    if (m_stack->isVisible() || !m_stack->isPopup()) {
        return;
    }
    // Before popping up the StackedWidget, send the tabbar a QHoverEvent so it knows the mouse has left
#if QT_VERSION < QT_VERSION_CHECK(6, 3, 0)
    QHoverEvent ehl(QEvent::HoverLeave, m_tabBar->mapToGlobal(QCursor::pos()), m_tabBar->mapToGlobal(QCursor::pos()));
#else
    QHoverEvent ehl(QEvent::HoverLeave, m_tabBar->mapToParent(QCursor::pos()), m_tabBar->mapToGlobal(QCursor::pos()),
                    m_tabBar->mapToGlobal(QCursor::pos()));
#endif
    QApplication::sendEvent(m_tabBar, &ehl);
    // Adjust the position before showing to avoid a wrong position after moving
    resizeStackedWidget();
    m_stack->setFocus();
    m_stack->show();
}

void RibbonBarPrivate::onWindowTitleChanged(const QString &title)
{
    Q_Q(RibbonBar);
    Q_UNUSED(title);
    q->update();
}

void RibbonBarPrivate::onWindowIconChanged(const QIcon &icon)
{
    if (m_quickAccessBar) {
        m_quickAccessBar->setIcon(icon);
    }
}

void RibbonBarPrivate::onPageWindowTitleChanged(const QString &title)
{
    RibbonPage *w = qobject_cast<RibbonPage *>(sender());

    if (Q_NULLPTR == w) {
        return;
    }
    for (int i = 0; i < m_tabBar->count(); ++i) {
        // There are few tabs, so efficiency is not a concern
        QVariant var = m_tabBar->tabData(i);
        if (var.isValid()) {
            RibbonTabData data = var.value<RibbonTabData>();
            if (w == data.page) {
                m_tabBar->setTabText(i, title);
            }
        }
    }
}

void RibbonBarPrivate::onPageContextAdded(RibbonPage *page)
{
    Q_ASSERT_X(page != Q_NULLPTR, "onPageContextAdded", "add Q_NULLPTR page");
    m_stack->addWidget(page);   // here the stackedWidget appends; everywhere else should use insert
}

void RibbonBarPrivate::onCurrentRibbonTabChanged(int index)
{
    Q_Q(RibbonBar);
    QVariant var = m_tabBar->tabData(index);
    RibbonPage *page = Q_NULLPTR;

    if (var.isValid()) {
        RibbonTabData data = var.value<RibbonTabData>();
        page = data.page;
    }
    if (page) {
        if (m_stack->currentWidget() != page) {
            m_stack->setCurrentWidget(page);
        }
        if (m_minimized) {
            m_tabBar->clearFocus();
            // In minimized mode, set the tab index to -1 after each StackedWidget show,
            // so every click triggers onCurrentRibbonTabChanged
            showPopupStack();
        }
    }
    Q_EMIT q->currentRibbonTabChanged(index);
}

/**
 * @brief ribbon tab bar single click
 *
 * This implementation must forward the stackedwidget's QEvent::MouseButtonDblClick to the tabbar in the eventfilter,
 * otherwise a double click becomes two single clicks
 *
 * The single click event only responds when the same tab is clicked
 * @param index
 */
void RibbonBarPrivate::onCurrentRibbonTabClicked(int index)
{
    if (index != m_tabBar->currentIndex()) {
        // A different clicked tab is handled by the changed slot
        return;
    }
    if (index == -1) {
        return;
    }
    if (index == m_tabBar->nullTabIndex()) {
        return;   // the placeholder tab, which is not a page
    }
    if (m_minimized) {
        showPopupStack();
    }
}

void RibbonBarPrivate::onCurrentRibbonTabDoubleClicked(int index)
{
    Q_Q(RibbonBar);
    if (index == -1) {
        return;
    }
    if (index == m_tabBar->nullTabIndex()) {
        return;   // the placeholder tab, which is not a page
    }
    q->setMinimized(!m_minimized);
}

void RibbonBarPrivate::onTabMoved(int from, int to)
{
    m_stack->moveWidget(from, to);
}

void RibbonBarPrivate::onStackWidgetHided()
{
    m_tabBar->setCurrentIndex(-1);
}

void RibbonBarPrivate::onFontChanged()
{
    RibbonElementStyleOpt.recalc();
}

void RibbonBarPrivate::hideKeyTips()
{
    Q_Q(RibbonBar);
    m_keyTipMode = false;
    qDeleteAll(m_keyTips);
    m_keyTips.clear();
}

static QString keyTipMnemonic(const QString &title)
{
    // Extract the mnemonic letter from a "&File" style title; empty when absent
    const int idx = title.indexOf(QLatin1Char('&'));
    if (idx >= 0 && idx + 1 < title.size()) {
        const QChar c = title.at(idx + 1);
        if (!c.isSpace()) {
            return QString(c).toUpper();
        }
    }
    return QString();
}

void RibbonBarPrivate::renderKeyTips(const QList<KeyTipTarget> &targets)
{
    Q_Q(RibbonBar);
    hideKeyTips();
    if (targets.isEmpty() || !q->isVisible()) {
        return;
    }
    // Assign badge letters: mnemonics first, then auto-fill with A-Z
    QSet<QChar> used;
    QVector<QString> keys(targets.size());
    for (int i = 0; i < targets.size(); ++i) {
        const QString &pref = targets.at(i).preferred;
        if (!pref.isEmpty() && !used.contains(pref.at(0))) {
            keys[i] = pref;
            used.insert(pref.at(0));
        }
    }
    static const QString pool = QStringLiteral("ABCDEFGHIJKLMNOPQRSTUVWXYZ");
    for (int i = 0; i < targets.size(); ++i) {
        if (!keys.at(i).isEmpty()) {
            continue;
        }
        for (const QChar &c : pool) {
            if (!used.contains(c)) {
                keys[i] = QString(c);
                used.insert(c);
                break;
            }
        }
    }
    m_keyTipMode = true;
    for (int i = 0; i < targets.size(); ++i) {
        if (keys.at(i).isEmpty()) {
            continue;   // Letter pool exhausted: drop this target
        }
        const KeyTipTarget &tg = targets.at(i);
        QRect rect;
        if (tg.pageIndex >= 0) {
            const QRect tabRect = m_tabBar->tabRect(tg.pageIndex);
            rect = QRect(m_tabBar->mapTo(q, tabRect.topLeft()), tabRect.size());
        } else if (tg.widget) {
            rect = QRect(tg.widget->mapTo(q, QPoint(0, 0)), tg.widget->size());
        } else {
            continue;
        }
        RibbonKeyTip *kt = new RibbonKeyTip(q);
        kt->setKey(keys.at(i));
        kt->setTarget(tg.widget);
        kt->setProperty("pageIndex", tg.pageIndex);
        const QSize sz = kt->sizeHint();
        const QPoint pos(rect.center().x() - sz.width() / 2, rect.top() + (rect.height() - sz.height()) / 2);
        kt->setGeometry(QRect(pos, sz));
        kt->show();
        kt->raise();
        m_keyTips.append(kt);
    }
}

void RibbonBarPrivate::showKeyTips()
{
    Q_Q(RibbonBar);
    // Level-1 targets: application button + visible page tabs
    QList<KeyTipTarget> targets;
    if (m_applicationButton && m_applicationButton->isVisible()) {
        targets.append({m_applicationButton, -1, QString()});
    }
    // Enumerate by tab index (same coordinate space as tabRect/setCurrentIndex,
    // so hidden pages cannot desync the badge positions)
    const int tabCount = m_tabBar->count();
    for (int i = 0; i < tabCount; ++i) {
        RibbonPage *p = q->page(i);
        if (!p || !q->isPageVisible(p)) {
            continue;
        }
        targets.append({Q_NULLPTR, i, keyTipMnemonic(p->title())});
    }
    renderKeyTips(targets);
}

void RibbonBarPrivate::showControlKeyTips()
{
    Q_Q(RibbonBar);
    // Level-2 targets: RibbonButtons inside the current page (keep the application button)
    QList<KeyTipTarget> targets;
    if (m_applicationButton && m_applicationButton->isVisible()) {
        targets.append({m_applicationButton, -1, QString()});
    }
    RibbonPage *page = q->page(q->currentIndex());
    if (page) {
        const QList<RibbonGroup *> groups = page->groups();
        for (RibbonGroup *g : groups) {
            const QList<RibbonButton *> btns = g->ribbonButtons();
            for (RibbonButton *b : btns) {
                if (!b->isVisible()) {
                    continue;
                }
                targets.append({b, -1, keyTipMnemonic(b->text())});
            }
            // Galleries in the group carry a badge too: pressing it moves the focus
            // into the gallery, which is then driven from the keyboard (B9).
            const QList<RibbonGallery *> galleries = g->findChildren<RibbonGallery *>();
            for (RibbonGallery *gallery : galleries) {
                if (!gallery->isVisible()) {
                    continue;
                }
                targets.append({gallery, -1, QString()});
            }
        }
    }
    renderKeyTips(targets);
}

bool RibbonBarPrivate::handleKeyTipKeyEvent(QKeyEvent *ke)
{
    Q_Q(RibbonBar);
    if (m_keyTipMode) {
        if (ke->key() == Qt::Key_Escape || ke->key() == Qt::Key_Alt) {
            hideKeyTips();
            return true;
        }
        const QString t = ke->text().toUpper();
        if (!t.isEmpty()) {
            for (RibbonKeyTip *kt : m_keyTips) {
                if (kt->key().toUpper() != t) {
                    continue;
                }
                const int pageIndex = kt->property("pageIndex").toInt();
                QWidget *target = kt->target();
                if (target) {
                    // Control target: the application button and RibbonButtons fire on
                    // click; a gallery takes the focus instead, so it can then be
                    // driven from the keyboard (B9).
                    if (QAbstractButton *btn = qobject_cast<QAbstractButton *>(target)) {
                        btn->click();
                    } else {
                        target->setFocus(Qt::ShortcutFocusReason);
                    }
                    hideKeyTips();
                } else if (pageIndex >= 0) {
                    // Tab target: switch to that page and descend into level-2 (in-page badges)
                    q->setCurrentIndex(pageIndex);
                    showControlKeyTips();
                } else {
                    hideKeyTips();
                }
                return true;
            }
            return true;   // No match: stay in the mode, awaiting Esc/Alt or the next key
        }
        return true;
    }
    // Not in the mode: bare Alt enters keytip mode (no Ctrl/Meta, to avoid stealing system shortcuts)
    if (ke->key() == Qt::Key_Alt) {
        if (!(ke->modifiers() & (Qt::ControlModifier | Qt::MetaModifier))) {
            showKeyTips();
            return true;
        }
    }
    return false;
}

QX_RIBBON_END_NAMESPACE
