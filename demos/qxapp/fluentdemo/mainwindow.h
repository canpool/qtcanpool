/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtCore/QVector>
#include <QtWidgets/QMainWindow>

#include "qxfluent/qxfluent_global.h"
#include "qxwindow/qxwindow_global.h"

class QAction;
class QStackedWidget;

QX_FLUENT_BEGIN_NAMESPACE
class FluentCommandBar;
class FluentNavigationView;
class FluentWindowMaterial;
QX_FLUENT_END_NAMESPACE

QX_WINDOW_BEGIN_NAMESPACE
class WindowAgentWidget;
QX_WINDOW_END_NAMESPACE

QX_FLUENT_USE_NAMESPACE

class DemoPage;
class PageTransition;
class SettingsPage;
class TitleBar;

/*!
 * A Fluent application's window: a caption of its own along the top - mark,
 * name, search, commands, system buttons - a navigation pane down the left, and
 * one screen at a time beside it.
 *
 * This is the shape a Fluent application has instead of a ribbon: the commands
 * that matter in one row, the places that can be gone to in a column, and
 * everything else on the screen the column opens. It is worth a demo of its own
 * because the two control sets are not skins for one another - an application
 * picks one, and what it gets is a whole shell, not a look.
 *
 * The window draws no system frame. qxwindow's agent takes the window over, the
 * bar is handed to it as the caption, and the three system buttons are named one
 * by one - which is what keeps them clickable, because everything else in a
 * caption is where a press starts a window move.
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
    /*! Opens the pane to its full width (\a open) or folds it to its icons, as
     *  the caption's command and the switch on Settings do. */
    void setPaneOpen(bool open);
    /*! Writes \a text into the caption's search field, as typing it does: what
     *  the pane lists is the field's answer, so this is a way to show a
     *  narrowed pane without a hand on the keyboard. */
    void setSearch(const QString &text);

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
    /*! Tells the pane how much room it has: the pane with its words while the
     *  window can hold a column beside a page, and folded to its icons when it
     *  cannot. */
    void resizeEvent(QResizeEvent *event) override;
private:
    /*! A place the pane can go to: what it is called, what it looks like in the
     *  pane, and the screen it opens. */
    struct Destination {
        QString text;
        /*! The glyph the pane draws beside the name, named rather than held as
         *  a picture: the picture has to be painted in the ink of the page it
         *  lands on, and that ink is only known when the row is made - and
         *  again when the theme changes. */
        QString icon;
        /*! The heading this destination is listed under. Empty while it is one
         *  of the destinations that stand on their own - the screen the window
         *  opens on is not one of them, it is above them. */
        QString group;
        /*! The destination this one is listed under, empty while it stands on
         *  its own. A parent is a destination as well - pressing it opens what
         *  is under it and goes there - so the pane lists nothing that cannot
         *  be reached. */
        QString parent;
        QWidget *page = Q_NULLPTR;
        /*! The row the pane made for it, empty while the pane is not listing
         *  it. A row is the pane's to destroy - narrowing the pane destroys
         *  every row it had - so this is a loan, cleared when the pane is
         *  emptied, and the one thing that lets a change of theme repaint the
         *  icons without rebuilding the rows the reader has opened or closed. */
        QAction *action = Q_NULLPTR;
    };

    void createTheme(const QString &themeId);
    void createNavigation();
    void buildTitleBar();
    void installWindowAgent();
    void createConnections();

    /*! Puts the pane into the shape a window \a across pixels wide has, and
     *  does nothing when it is already in that shape - a window that reset the
     *  pane on every resize would undo the fold the user had just asked for. */
    void applyWidth(int across);

    /*! Rebuilds the pane from the destinations whose names contain \a needle,
     *  keeping the screen in front if it is still one of them. */
    void filterDestinations(const QString &needle);

    /*! Repaints the icons the pane is listing in the ink of the theme now in
     *  force. The rows are left as they are: what a theme changes about a pane
     *  is the colour of what it draws, not which rows are open. */
    void redrawIcons();

    TitleBar *m_titleBar = Q_NULLPTR;
    QX_WINDOW_PREPEND_NAMESPACE(WindowAgentWidget) *m_agent = Q_NULLPTR;
    /*! The shape the window is in, as one of the three widths. -1 is "not yet
     *  worked out", which is what the first resize answers. */
    int m_shape = -1;
    /*! The caption's fold: one of two ways to ask the pane the same thing, and
     *  the one that has to be told when the other was used. */
    QAction *m_fold = Q_NULLPTR;
    FluentNavigationView *m_nav = Q_NULLPTR;
    QStackedWidget *m_stack = Q_NULLPTR;
    /*! How a screen arrives when the pane changes it: the page fades in and
     *  rises the last eight pixels, over the family's own motion. */
    PageTransition *m_transition = Q_NULLPTR;
    SettingsPage *m_settings = Q_NULLPTR;
    /*! Every destination in the order the pane shows them, and the ones it is
     *  showing now - a narrowed pane is the second list, and the index the view
     *  reports is a position in it. */
    QVector<Destination> m_destinations;
    QVector<const Destination *> m_shown;
    /*! Made when a material is asked for and not before: a window material is
     *  asked of a window, and the page is built before it belongs to one. */
    FluentWindowMaterial *m_material = Q_NULLPTR;
};

#endif   // MAINWINDOW_H
