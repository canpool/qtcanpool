/**
 * Copyright (C) 2024 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "qxribbon/ribbonmainwindow.h"

QX_RIBBON_BEGIN_NAMESPACE
class RibbonActionsManager;
QX_RIBBON_END_NAMESPACE

class MainWindow : public QxRibbon::RibbonMainWindow
{
    Q_OBJECT
public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
private slots:
    void onActionCustomizeTriggered();
private:
    void createActionsManager();

    QAction *createAction(const QIcon &icon, const QString &text, QObject *parent = nullptr);
private:
    QxRibbon::RibbonActionsManager *m_actMgr;
    int m_actionTagText;
    int m_actionTagWithIcon;
    QString m_customizeXml;
};
#endif   // MAINWINDOW_H
