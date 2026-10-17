/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "qxribbon/ribbonmainwindow.h"

QX_RIBBON_BEGIN_NAMESPACE
class RibbonPage;
class RibbonGroup;
QX_RIBBON_END_NAMESPACE

class MainWindow : public QxRibbon::RibbonMainWindow
{
    Q_OBJECT
public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
protected:
    void resizeEvent(QResizeEvent *event) override;
private:
    void updateReducedReadout();

    QList<QxRibbon::RibbonGroup *> m_groups;
    QStringList m_titles;
};

#endif   // MAINWINDOW_H
