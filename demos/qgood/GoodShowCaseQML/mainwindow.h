/**
 * Copyright (C) 2023 maminjie <canpool@163.com> (https://github.com/canpool)
 * Copyright (C) 2018-2023 Antonio Dias (https://github.com/antonypro)
 * SPDX-License-Identifier: MIT
 **/

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "qgood/qgoodcentralwidget.h"
#include "qgood/qgoodwindow.h"

#include <QtQml>
#include <QtQuick>
#include <QtQuickWidgets>

#ifdef Q_OS_WIN
#ifdef QT_VERSION_QT5
#include <QtWinExtras>
#endif
#endif

class MainWindow : public QGoodWindow
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

public Q_SLOTS:
    QString loadPixmapToString(const QString &path);

private Q_SLOTS:
    QPixmap loadGrayedPixmap(const QPixmap &pix);
    QString loadPixmapAsBase64(const QPixmap &pix);
    void captionButtonStateChangedPrivate(const QGoodWindow::CaptionButtonState &state);
private:
    bool event(QEvent *event);
    void closeEvent(QCloseEvent *event);
private:
    QGoodCentralWidget *m_goodCentralWidget;
    QPointer<QQuickWidget> m_quick_widget;
    QHash<QString, QString> m_pix_str_hash;
};

#endif   // MAINWINDOW_H
