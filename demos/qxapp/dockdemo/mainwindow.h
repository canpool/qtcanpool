/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "qxdock/qxdock_global.h"
#include <QMainWindow>

QX_DOCK_BEGIN_NAMESPACE
class DockWindow;
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
     */
    MainWindow(QWidget *parent = nullptr, bool screenshotMode = false);

    /*!
     * The demo's arrangement goes in and out of a settings file, and the two
     * are named for that rather than saveState()/restoreState(): those are
     * QMainWindow's own, they take arguments, and a same-named pair here
     * hides them - which is exactly why this class could not call its base's
     * without writing the class name in front of every call.
     */
    void saveToSettings();
    void restoreFromSettings();
protected:
    virtual void closeEvent(QCloseEvent *event) override;
private:
    DockWindow *m_dockWindow = nullptr;
    bool m_screenshotMode = false;
};
#endif   // MAINWINDOW_H
