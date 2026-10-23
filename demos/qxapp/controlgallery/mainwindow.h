/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtCore/QHash>
#include <QtWidgets/QMainWindow>

class QTabWidget;

class GalleryPage;

/*!
 * One window holding every control set, one tab each.
 *
 * Why one window and not four demos: the sets are alternatives the user picks
 * between, and a gallery that made them four applications would let them drift
 * apart - different sizes, different themes, different switches - until a
 * comparison between them was a comparison between four demos. Here they share
 * the window, the theme and the switches, so the only thing that changes when
 * the tab changes is the set of controls.
 */
class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    /*!
     * @param screenshotMode puts the window into the state a capture needs and
     * nothing else: the reduce-motion switch is asserted so that the two
     * controls on show that animate by definition - a breathing skeleton, a
     * sweeping ring - hold still for it. It is asserted here and not in main()
     * because of when it has to be said; see the constructor.
     * @param styleId the tab to open on, "fluent" or "material".
     * @param themeId the theme to start on, empty to keep whichever one the
     * engine restored. A capture always names one: the golden has to be the
     * same picture whatever the machine's last interactive run picked.
     */
    MainWindow(QWidget *parent = Q_NULLPTR, bool screenshotMode = false, const QString &styleId = QString(),
               const QString &themeId = QString());

    /*!
     * Opens the tab named \a id.
     *
     * Only main()'s --style sequence calls this; the Style menu goes straight to
     * the tab widget. It exists so that a capture can ask for "start here, then
     * switch there" in one run, which is the shape the ribbon demo's --theme
     * already has, and the only way to catch a control that kept the other
     * set's paint.
     */
    void applyStyle(const QString &id);

    /*! Switches the window to \a themeId, one of the registered theme ids. */
    void applyTheme(const QString &id);

    /*!
     * Prints every widget in the window with its rectangle, in window
     * coordinates, one per line.
     *
     * A capture answers "does this look right"; this answers "where is this and
     * how big did it come out", which is what the visual regression test crops
     * to. The rectangle is measured rather than read from any layout: a control
     * that came out two pixels narrower than yesterday is a change the eye
     * cannot see and a capture cannot name.
     */
    void printTree();

    /*!
     * The size a capture of this window is taken at.
     *
     * Fixed in width, and in height it is the tallest page's own layout that
     * decides, measured rather than guessed: a gallery is a column of rows, and
     * a row below the fold is a row the visual regression test cannot crop to.
     * The floor covers the common case; the page covers the day a control is
     * added and the floor stops being enough, which is the day that used to be
     * discovered by a test failing with a rectangle outside the window.
     *
     * Both answers are measured after the window has been laid out - ask too
     * early and the chrome is a guess and the pages are at their default size.
     */
    QSize captureSize() const;
private:
    void createTheme(const QString &themeId);
    void createThemeMenu();
    void createStyleMenu();

    QTabWidget *m_tabs = Q_NULLPTR;
    /*! Every page, by the id it answers to on the --style switch. The window
     *  holds no member per family: a fifth set would otherwise be a header to
     *  edit, a member to add and a list to extend, and the gallery's whole job
     *  is to be the place a set is shown with nothing else to do. */
    QHash<QString, GalleryPage *> m_pages;
    /*! Which tab each style id lives in, so applyStyle() is a lookup and not a
     *  search through the tab widget's own widgets. */
    QHash<QString, int> m_styleTabs;
    /*! The same ids in the order the tabs were added; a hash walks in whatever
     *  order it likes and a menu must not. */
    QStringList m_styleOrder;
};

#endif   // MAINWINDOW_H
