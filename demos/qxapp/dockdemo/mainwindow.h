/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "qxdock/qxdock_global.h"
#include <QMainWindow>
#include <QEvent>
#include <QString>

QX_DOCK_BEGIN_NAMESPACE
class DockWindow;
class DockWidget;
QX_DOCK_END_NAMESPACE

QX_DOCK_USE_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    /*!
     * @param screenshotMode keeps the arrangement out of the settings file, in
     * both directions: nothing is restored on construction and nothing is
     * saved on close. A screenshot has to show the demo's own layout rather
     * than whatever arrangement the last interactive run happened to leave in
     * Settings.ini, and it must not write one into the working directory
     * either - the capture has to be the same on every machine for the golden
     * to stay comparable.
     * @param themeId the theme to start on, empty to keep whichever one the
     * engine restored. A screenshot always names one: the golden has to be
     * the same capture whatever the machine's last interactive run picked.
     */
    MainWindow(QWidget *parent = nullptr, bool screenshotMode = false, const QString &themeId = QString());

    /*!
     * The demo's arrangement goes in and out of a settings file, and the two
     * are named for that rather than saveState()/restoreState(): those are
     * QMainWindow's own, they take arguments, and a same-named pair here
     * hides them - which is exactly why this class could not call its base's
     * without writing the class name in front of every call.
     */
    void saveToSettings();
    void restoreFromSettings();

    /*!
     * Opens the auto hide panel the demo owns, as if its side tab had been
     * clicked. A capture needs it: the auto hide title bar is filled with the
     * highlight and the tabs sit inside it, and a screenshot that only ever
     * shows the collapsed state never looks at that arrangement.
     */
    void expandAutoHidePanel();
protected:
    virtual void closeEvent(QCloseEvent *event) override;
    virtual bool eventFilter(QObject *watched, QEvent *event) override;
private:
    /*!
     * Registers the bundled themes, puts the window on one of them and opens the
     * Theme menu over whatever the engine knows.
     */
    void createTheme(const QString &themeId);
    void createThemeMenu();
    void installApplicationStyleSheet();

    DockWindow *m_dockWindow = nullptr;
    DockWidget *m_autoHideDockWidget = nullptr;
    bool m_screenshotMode = false;
    QString m_dockSheet;
};
#endif   // MAINWINDOW_H
