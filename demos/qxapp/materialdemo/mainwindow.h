/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtWidgets/QMainWindow>

#include "qxmaterial/qxmaterial_global.h"
#include "qxwindow/qxwindow_global.h"

class QAction;
class QStackedWidget;

QX_MATERIAL_BEGIN_NAMESPACE
class MaterialDrawer;
class MaterialFAB;
class MaterialNavigationRail;
class MaterialSnackbar;
class MaterialTopAppBar;
QX_MATERIAL_END_NAMESPACE

QX_MATERIAL_USE_NAMESPACE
QX_WINDOW_USE_NAMESPACE

QX_WINDOW_BEGIN_NAMESPACE
class WindowAgentWidget;
QX_WINDOW_END_NAMESPACE

class DemoPage;
class FeedbackPage;
class SettingsPage;
class TitleBar;

/*!
 * A Material application's window: a top app bar along the top, a navigation
 * rail down the left, and one screen at a time beside it.
 *
 * This is the shape a Material application has instead of a ribbon: the name of
 * the screen and the things that can be done to the screen as a whole in one
 * bar, the places that can be gone to in a column, the one thing the screen is
 * for floating above them, and everything else on the screen the column opens.
 * It is worth a demo of its own because the control sets are not skins for one
 * another - an application picks one, and what it gets is a whole shell, not a
 * look.
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
    /*! Opens destination \a index, as clicking it in the rail does. */
    void setDestination(int index);

    /*!
     * Prints every widget in the window with its rectangle, in window
     * coordinates, one per line: what a visual regression test crops a capture
     * to. The rectangle is measured rather than read from any layout.
     */
    void printTree();
protected:
    /*! Keeps the caption's maximize button saying what the window is: the state
     *  changes without that button being touched - the window starts maximized,
     *  the system menu maximizes it, a double click on the frame does - and a
     *  glyph that only follows its own clicks would be guessing. */
    void changeEvent(QEvent *event) override;
    /*! Tells the destinations how much room they have: a rail beside a page, a
     *  rail folded to its icons, or no rail at all and a drawer that comes over
     *  the page. */
    void resizeEvent(QResizeEvent *event) override;
private:
    void createTheme(const QString &themeId);
    void createShell();
    void buildTitleBar();
    void installWindowAgent();
    void createNavigation();
    void createDrawer();
    void createConnections();

    /*! Puts the window into the shape a window \a across pixels wide has, and
     *  does nothing when it is already in that shape - a window that reset
     *  itself on every resize would undo what the user had just done. */
    void applyWidth(int across);
    /*! The way into the drawer, and back out of it: it stands in the app bar's
     *  leading place only while there is no rail to hold the destinations. */
    void giveMenuButton();
    void takeMenuButton();

    /*! The name of the screen that opens first, taken from the page itself:
     *  the caption has to say something before the rail has reported anything,
     *  and a literal here would be a second name for one screen. */
    QString m_firstTitle;
    /*! The shape the window is in, as one of the three widths. -1 is "not yet
     *  worked out", which is what the first resize answers. */
    int m_shape = -1;
    TitleBar *m_titleBar = Q_NULLPTR;
    QX_WINDOW_PREPEND_NAMESPACE(WindowAgentWidget) *m_agent = Q_NULLPTR;
    /*! The app bar the caption holds: the screen's name and the things that can
     *  be done to it. It is the library's own control, and the caption is only
     *  where the window puts it. */
    MaterialTopAppBar *m_bar = Q_NULLPTR;
    MaterialNavigationRail *m_rail = Q_NULLPTR;
    /*! The rail's answer for a window too narrow to hold a column: the same
     *  destinations in a panel over the page. It stands beside the rail rather
     *  than inside it, because a drawer belongs to the window and not to the
     *  column it replaces. */
    MaterialDrawer *m_drawer = Q_NULLPTR;
    QAction *m_menu = Q_NULLPTR;
    MaterialFAB *m_fab = Q_NULLPTR;
    MaterialSnackbar *m_snackbar = Q_NULLPTR;
    QStackedWidget *m_stack = Q_NULLPTR;
    SettingsPage *m_settings = Q_NULLPTR;
};

#endif   // MAINWINDOW_H
