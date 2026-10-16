/**
 * Copyleft (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MIT
 **/
#pragma once
#include "qxribbon_global.h"

#include <QMdiArea>
#include <QHash>

QT_BEGIN_NAMESPACE
class QTabBar;
QT_END_NAMESPACE

QX_RIBBON_BEGIN_NAMESPACE

class RibbonPage;

/*!
 * \brief A tabbed-document MDI area that keeps the ribbon in step with the active document.
 *
 * A ribbon application normally shows its open documents as tabs, and swaps
 * the visible pages when the user switches document, the way the office
 * applications do. RibbonMdiArea is that document area. It builds on
 * QMdiArea, so every sub-window facility Qt provides stays available, and
 * adds the two pieces a ribbon host would otherwise wire by hand every time:
 *
 * - documents are presented as tabs by default (TabbedView plus document
 *   mode, closable and movable), which is what a ribbon application shows;
 *   setTabbedDocuments(false) goes back to classic sub-windows;
 * - each document may carry a RibbonPage, and that page is shown while the
 *   document is the active one and hidden while it is not, so switching
 *   tabs swaps the ribbon pages.
 *
 * A document added without a page takes no part in the swap, and a page
 * that is not on a RibbonBar (page->ribbonBar() is null) is left alone -
 * the area never talks to a bar it was not given. Pages must therefore be
 * added to a RibbonBar before they are handed to addDocument(); pages that
 * belong to a RibbonPageContext are not managed here.
 *
 * The area paints nothing of its own: the document tabs are the plain tab
 * bar QMdiArea drives, rendered through the window style sheet like any
 * other widget. Hosts that need a different page policy can ignore the page
 * argument and connect to the inherited subWindowActivated() signal.
 *
 * \note Qt's tabbed view selects the first document's tab but leaves the
 * area's active window null, and setActiveSubWindow() is then a no-op on
 * the tab that is already current (the tab bar cannot be deselected). The
 * first document therefore becomes active when the area is shown after the
 * document was added, or as soon as the user visits another tab and comes
 * back - both ordinary flows. Until then no registered page is up, which is
 * the same invariant the area keeps at every other point.
 */
class QX_RIBBON_EXPORT RibbonMdiArea : public QMdiArea
{
    Q_OBJECT
public:
    explicit RibbonMdiArea(QWidget *parent = Q_NULLPTR);
    ~RibbonMdiArea() Q_DECL_OVERRIDE;

    void setTabbedDocuments(bool on);
    bool isTabbedDocuments() const;

    //! The tab bar QMdiArea drives in tabbed view; it is a child of the area.
    QTabBar *documentTabBar() const;

    /*!
     * Adds a document and returns its sub-window, which owns the content and
     * is deleted when the document is closed (QMdiArea's contract). \a page
     * may be null, in which case the document leaves the ribbon alone.
     */
    QMdiSubWindow *addDocument(QWidget *content, const QString &title, RibbonPage *page = Q_NULLPTR);

    //! The page registered for \a subWindow, or null if it has none.
    RibbonPage *documentPage(QMdiSubWindow *subWindow) const;

private Q_SLOTS:
    void onSubWindowActivated(QMdiSubWindow *subWindow);
private:
    void showPage(RibbonPage *page);
    void hidePage(RibbonPage *page);

    QHash<QMdiSubWindow *, RibbonPage *> m_pageMap;
    QMdiSubWindow *m_activeSubWindow;
};

QX_RIBBON_END_NAMESPACE
