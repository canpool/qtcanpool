/**
 * Copyleft (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MIT
 **/
#pragma once

#include "qxribbon_global.h"
#include "ribbonbackstageview.h"

#include <QFrame>
#include <QHash>
#include <QPointer>
#include <QToolButton>
#include <QVBoxLayout>

QX_RIBBON_BEGIN_NAMESPACE

class RibbonBackstageViewPrivate
{
    QX_DECLARE_PUBLIC(RibbonBackstageView)
public:
    RibbonBackstageViewPrivate();

    void init(RibbonBar *ribbonBar);

    // Places the view over its host and the entries and the active page inside it.
    // Every path that can change any of the three - show, add, activate, host resize -
    // lands here, so the geometry has one definition instead of four.
    void relayout();

    QAction *pageAction(QWidget *widget) const;
    QWidget *entryWidget(QAction *action) const;
    void addEntry(QAction *action);
    void insertEntry(QAction *action, QWidget *entry);
    void removeEntry(QAction *action);
    void activateFirstPage();
public:
    QPointer<RibbonBar> m_ribbonBar;
    QWidget *m_menu;                               ///< the left column that holds the entries
    QVBoxLayout *m_menuLayout;                     ///< close button, entries, stretch - in action order
    QToolButton *m_closeButton;                    ///< the back button at the top of the column
    QHash<QAction *, QToolButton *> m_buttons;     ///< entry action -> its button
    QHash<QAction *, QFrame *> m_separators;       ///< separator action -> its line
    QHash<QAction *, QPointer<QWidget>> m_pages;   ///< page entry action -> the page it shows
    QPointer<QWidget> m_activePage;                ///< the page currently shown, null when none
    bool m_closePrevented;
};

QX_RIBBON_END_NAMESPACE
