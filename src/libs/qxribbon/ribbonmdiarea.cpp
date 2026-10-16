/**
 * Copyleft (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MIT
 **/
#include "ribbonmdiarea.h"

#include "ribbonbar.h"
#include "ribbonpage.h"

#include <QMdiSubWindow>
#include <QTabBar>

QX_RIBBON_BEGIN_NAMESPACE

RibbonMdiArea::RibbonMdiArea(QWidget *parent)
    : QMdiArea(parent)
    , m_activeSubWindow(Q_NULLPTR)
{
    // Tabbed documents are what a ribbon application shows; document mode
    // drops the sub-window frames so the tab bar alone carries them.
    setViewMode(QMdiArea::TabbedView);
    setDocumentMode(true);
    setTabsClosable(true);
    setTabsMovable(true);

    connect(this, &QMdiArea::subWindowActivated, this, &RibbonMdiArea::onSubWindowActivated);
}

RibbonMdiArea::~RibbonMdiArea()
{
    // The base-class destructor deletes the sub-windows, and every deletion
    // runs the handler connected in addDocument() - against members that are
    // gone by then, because base-class destruction follows member
    // destruction. Cut the handlers loose here, while they can still reach
    // the map safely.
    for (auto it = m_pageMap.begin(); it != m_pageMap.end(); ++it) {
        if (it.key()) {
            QObject::disconnect(it.key(), nullptr, this, nullptr);
        }
    }
    m_pageMap.clear();
    m_activeSubWindow = Q_NULLPTR;
}

void RibbonMdiArea::setTabbedDocuments(bool on)
{
    if (on) {
        setViewMode(QMdiArea::TabbedView);
        setDocumentMode(true);
    } else {
        setDocumentMode(false);
        setViewMode(QMdiArea::SubWindowView);
    }
}

bool RibbonMdiArea::isTabbedDocuments() const
{
    return viewMode() == QMdiArea::TabbedView;
}

QTabBar *RibbonMdiArea::documentTabBar() const
{
    // QMdiArea creates the bar when it enters tabbed view and keeps it as a
    // child of the area; there is no accessor for it.
    return findChild<QTabBar *>();
}

QMdiSubWindow *RibbonMdiArea::addDocument(QWidget *content, const QString &title, RibbonPage *page)
{
    // The sub-window owns the content, and closing the document deletes it -
    // that is QMdiArea's contract, and the page swap below relies on it: a
    // destroyed sub-window always means a closed document.
    QMdiSubWindow *subWindow = addSubWindow(content);
    subWindow->setWindowTitle(title);
    if (!page) {
        return subWindow;
    }

    m_pageMap.insert(subWindow, page);
    // When the document being closed is the active one, its page must go with
    // it right here: there may be no other document left to activate, and
    // then no activation change would ever hide it.
    connect(subWindow, &QObject::destroyed, this, [this, subWindow, page]() {
        m_pageMap.remove(subWindow);
        if (m_activeSubWindow == subWindow) {
            m_activeSubWindow = Q_NULLPTR;
            hidePage(page);
        }
    });
    // A document that is not the active one starts with its page down, so the
    // invariant holds from the first document on: pages begin their life
    // visible, as tabs of the bar.
    if (activeSubWindow() != subWindow) {
        hidePage(page);
    }
    return subWindow;
}

RibbonPage *RibbonMdiArea::documentPage(QMdiSubWindow *subWindow) const
{
    return m_pageMap.value(subWindow);
}

void RibbonMdiArea::onSubWindowActivated(QMdiSubWindow *subWindow)
{
    if (subWindow == m_activeSubWindow) {
        return;
    }

    // The invariant: of the pages the area manages, only the active
    // document's stays up - and none at all when nothing is active. Walking
    // the whole map rather than only the previous document matters because
    // pages begin visible and a document that was never active has never had
    // its page brought down. When nothing is active every key differs from
    // the null sub-window, so the same loop takes them all down.
    for (auto it = m_pageMap.begin(); it != m_pageMap.end(); ++it) {
        if (it.key() != subWindow) {
            hidePage(it.value());
        }
    }
    if (subWindow) {
        showPage(m_pageMap.value(subWindow));
    }

    m_activeSubWindow = subWindow;
}

void RibbonMdiArea::showPage(RibbonPage *page)
{
    if (!page) {
        return;
    }
    RibbonBar *bar = page->ribbonBar();
    if (bar) {
        bar->showPage(page);
    }
}

void RibbonMdiArea::hidePage(RibbonPage *page)
{
    if (!page) {
        return;
    }
    RibbonBar *bar = page->ribbonBar();
    if (bar) {
        bar->hidePage(page);
    }
}

QX_RIBBON_END_NAMESPACE
