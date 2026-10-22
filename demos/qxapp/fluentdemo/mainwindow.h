/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtWidgets/QMainWindow>

#include "qxfluent/qxfluent_global.h"

class QStackedWidget;

QX_FLUENT_BEGIN_NAMESPACE
class FluentCommandBar;
class FluentNavigationView;
class FluentWindowMaterial;
QX_FLUENT_END_NAMESPACE

QX_FLUENT_USE_NAMESPACE

class DemoPage;
class SettingsPage;

/*!
 * A Fluent application's window: a command bar along the top, a navigation pane
 * down the left, and one screen at a time beside it.
 *
 * This is the shape a Fluent application has instead of a ribbon: the commands
 * that matter in one row, the places that can be gone to in a column, and
 * everything else on the screen the column opens. It is worth a demo of its own
 * because the two control sets are not skins for one another - an application
 * picks one, and what it gets is a whole shell, not a look.
 */
class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    /*!
     * @param screenshotMode holds still what moves on its own. Two controls on
     * these screens are animated by definition - a breathing skeleton and a
     * sweeping ring - and a capture has to render the same pixels every time.
     * @param themeId the theme to start on, empty to keep whichever one the
     * engine restored. A capture always names one.
     */
    MainWindow(QWidget *parent = Q_NULLPTR, bool screenshotMode = false, const QString &themeId = QString());

    /*! Switches the window to \a themeId, one of the registered theme ids. */
    void applyTheme(const QString &themeId);
    /*! Opens destination \a index, as clicking it in the pane does. */
    void setDestination(int index);

    /*!
     * Prints every widget in the window with its rectangle, in window
     * coordinates, one per line: what a visual regression test crops a capture
     * to. The rectangle is measured rather than read from any layout.
     */
    void printTree();
private:
    void createTheme(const QString &themeId);
    void createCommandBar();
    void createNavigation();
    void createConnections();

    FluentCommandBar *m_bar = Q_NULLPTR;
    FluentNavigationView *m_nav = Q_NULLPTR;
    QStackedWidget *m_stack = Q_NULLPTR;
    SettingsPage *m_settings = Q_NULLPTR;
    /*! Made when a material is asked for and not before: a window material is
     *  asked of a window, and the page is built before it belongs to one. */
    FluentWindowMaterial *m_material = Q_NULLPTR;
};

#endif   // MAINWINDOW_H
