/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "qxapp/qxproperty.h"

#include <QList>
#include <QMainWindow>

namespace QxApp
{
class QxPropertyEditor;
class QxSettingsDialog;
}
namespace QxCore
{
class QxSettings;
}
class QLabel;
class QPushButton;

//! Property and settings family: descriptors, the form they build, the dialog.
class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
private Q_SLOTS:
    void applyEdits();
    void openSettings();
private:
    void buildEditor();
    void buildSettingsDialog();
    QxCore::QxSettings *m_settings = nullptr;
    QxApp::QxSettingsDialog *m_dialog = nullptr;
    QxApp::QxPropertyEditor *m_editor = nullptr;
    QPushButton *m_applyButton = nullptr;
    QLabel *m_stateLabel = nullptr;
    //! The descriptors, kept so the stored values can be read back by default.
    QList<QxApp::QxProperty> m_properties;
};

#endif   // MAINWINDOW_H
