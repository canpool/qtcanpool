/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtWidgets/QMainWindow>

#include "qxmaterial/qxmaterial_global.h"

class QStackedWidget;

QX_MATERIAL_BEGIN_NAMESPACE
class MaterialFAB;
class MaterialNavigationRail;
class MaterialSnackbar;
class MaterialTopAppBar;
QX_MATERIAL_END_NAMESPACE

QX_MATERIAL_USE_NAMESPACE

class DemoPage;
class FeedbackPage;
class SettingsPage;

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
private:
    void createTheme(const QString &themeId);
    void createShell();
    void createNavigation();
    void createConnections();

    MaterialTopAppBar *m_bar = Q_NULLPTR;
    MaterialNavigationRail *m_rail = Q_NULLPTR;
    MaterialFAB *m_fab = Q_NULLPTR;
    MaterialSnackbar *m_snackbar = Q_NULLPTR;
    QStackedWidget *m_stack = Q_NULLPTR;
    SettingsPage *m_settings = Q_NULLPTR;
};

#endif   // MAINWINDOW_H
