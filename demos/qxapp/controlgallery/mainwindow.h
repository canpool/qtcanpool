/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtCore/QHash>
#include <QtWidgets/QMainWindow>

class QTabWidget;

class FluentPage;
class MaterialPage;

/*!
 * One window holding both control sets, one tab each.
 *
 * Why one window and not two demos: the two sets are alternatives the user
 * picks between, and a gallery that made them two applications would let them
 * drift apart - different sizes, different themes, different switches - until a
 * comparison between them was a comparison between two demos. Here they share
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
private:
    void createTheme(const QString &themeId);
    void createThemeMenu();
    void createStyleMenu();

    QTabWidget *m_tabs = Q_NULLPTR;
    FluentPage *m_fluent = Q_NULLPTR;
    MaterialPage *m_material = Q_NULLPTR;
    /*! Which tab each style id lives in, so applyStyle() is a lookup and not a
     *  search through the tab widget's own widgets. */
    QHash<QString, int> m_styleTabs;
    /*! The same ids in the order the tabs were added; a hash walks in whatever
     *  order it likes and a menu must not. */
    QStringList m_styleOrder;
};

#endif   // MAINWINDOW_H
