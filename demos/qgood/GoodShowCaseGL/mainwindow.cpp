/**
 * Copyright (C) 2023 maminjie <canpool@163.com> (https://github.com/canpool)
 * Copyright (C) 2018-2023 Antonio Dias (https://github.com/antonypro)
 * SPDX-License-Identifier: MIT
 **/

#include "mainwindow.h"
#include "qgoodbye/qgoodmanager.h"
#include "qgoodbye/qgoodstateholder.h"

#include <QTimer>
#include <QShortcut>
#include <QMessageBox>
#include <QGuiApplication>
#include <QScreen>
#include <QCloseEvent>

#ifdef Q_OS_WIN
#ifdef QT_VERSION_QT5
#include <QWinTaskbarButton>
#include <QWinTaskbarProgress>
#endif
#endif

MainWindow::MainWindow(QWidget *parent) : QGoodWindow(parent, QColor("#002640"))
{
    m_goodCentralWidget = new QGoodCentralWidget(this);

    GLWidget *gl = new GLWidget(this);

    QTimer *gl_timer = new QTimer(this);
    connect(gl_timer, &QTimer::timeout, gl, &GLWidget::rotate);
    gl_timer->setInterval(10);

#ifdef QGOOD_WINDOW_ENABLE
    m_goodCentralWidget->setUnifiedTitleBarAndCentralWidget(true);

    qGoodStateHolder->setCurrentThemeDark(true);

    QGoodManager::setAppDarkTheme();
#endif

    QShortcut *shortcut1 = new QShortcut(QKeySequence(Qt::Key_F), this);

    connect(shortcut1, &QShortcut::activated, this, [=] {
        if (!isFullScreen())
            showFullScreen();
        else
            showNormal();
    });

    m_goodCentralWidget->setCentralWidget(gl);
    setCentralWidget(m_goodCentralWidget);

    QPixmap p = QPixmap(1, 1);
    p.fill(Qt::red);

    setWindowIcon(p);
    setWindowTitle("Good Window - Press F to toggle fullscreen!");

    resize(640, 480);
    move(QGuiApplication::primaryScreen()->availableGeometry().center() - rect().center());

#ifdef Q_OS_WIN
#ifdef QT_VERSION_QT5
    QTimer::singleShot(0, this, [=] {
        QPixmap p = QPixmap(1, 1);
        p.fill(Qt::yellow);

        QWinTaskbarButton *button = new QWinTaskbarButton(this);
        button->setWindow(windowHandle());
        button->setOverlayIcon(p);

        QWinTaskbarProgress *progress = button->progress();
        progress->setVisible(true);
        progress->setValue(50);
    });
#endif
#endif
    gl_timer->start();

#ifdef QGOOD_WINDOW_ENABLE
    m_goodCentralWidget->setTitleAlignment(Qt::AlignCenter);
#endif
}

MainWindow::~MainWindow()
{
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    QMessageBox msgbox(this);
    msgbox.setIcon(QMessageBox::Question);
    msgbox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    msgbox.setDefaultButton(QMessageBox::No);
    msgbox.setText("Are you sure to close?");

    int result = QGoodCentralWidget::execDialogWithWindow(&msgbox, this, m_goodCentralWidget);

    if (result != QMessageBox::Yes)
        event->ignore();
}
