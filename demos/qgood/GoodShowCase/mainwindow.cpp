/**
 * Copyright (C) 2023 maminjie <canpool@163.com> (https://github.com/canpool)
 * Copyright (C) 2022-2023 Antonio Dias (https://github.com/antonypro)
 * SPDX-License-Identifier: MIT
 **/

#ifdef _WIN32

#ifdef _WIN32_WINNT
#undef _WIN32_WINNT
#endif

#define _WIN32_WINNT _WIN32_WINNT_VISTA

#include <dwmapi.h>
#include <windows.h>

#endif

#include "mainwindow.h"
#include "qgoodbye/qgoodmanager.h"
#include "qgoodbye/qgoodstateholder.h"

#include <QCloseEvent>
#include <QGuiApplication>
#include <QMessageBox>
#include <QScreen>
#include <QShortcut>
#include <QTimer>

MainWindow::MainWindow(QWidget *parent) : QGoodWindow(parent)
{
    m_centralWidget = new CentralWidget(this);
    m_centralWidget->setWindowFlags(Qt::Widget);

    m_goodCentralWidget = new QGoodCentralWidget(this);

#ifdef QGOOD_WINDOW_ENABLE

    // macOS uses global menu bar
#ifndef Q_OS_MAC
    QMenuBar *menu_bar = m_centralWidget->menuBar();

    // Set font of menu bar
    QFont font = menu_bar->font();
    font.setPixelSize(12);
#ifdef Q_OS_WIN
    font.setFamily("Segoe UI");
#else
    font.setFamily(qApp->font().family());
#endif
    menu_bar->setFont(font);

    QTimer::singleShot(0, this, [=] {
        menu_bar->setFixedWidth(menu_bar->sizeHint().width());
        const int title_bar_height = m_goodCentralWidget->titleBarHeight();
        menu_bar->setStyleSheet(QString("QMenuBar {height: %0px;}").arg(title_bar_height));
    });

    m_goodCentralWidget->setLeftTitleBarWidget(menu_bar);
#endif

    QWidget *widget = new QWidget();

    QHBoxLayout *layout = new QHBoxLayout(widget);
    layout->setContentsMargins(0, 0, 0, 0);

    QPushButton *button = new QPushButton("Toggle All!", widget);

    connect(button, &QPushButton::clicked, this, [=] {
        m_goodCentralWidget->setTitleVisible(!m_goodCentralWidget->isTitleVisible());
        m_goodCentralWidget->setIconVisible(!m_goodCentralWidget->isIconVisible());
        m_goodCentralWidget->setTitleBarColor(!m_goodCentralWidget->isTitleVisible() ? Qt::red : QColor());
        m_goodCentralWidget->setActiveBorderColor(!m_goodCentralWidget->isTitleVisible() ? Qt::red : QColor());
    });

    QLabel *label = new QLabel("Label", widget);
    label->setAttribute(Qt::WA_TransparentForMouseEvents);

    layout->addWidget(button);
    layout->addWidget(label);

    m_goodCentralWidget->setRightTitleBarWidget(widget, true);
#endif

    connect(qGoodStateHolder, &QGoodStateHolder::currentThemeChanged, this, &MainWindow::themeChange);

    connect(this, &QGoodWindow::systemThemeChanged, this, [=] {
        qGoodStateHolder->setCurrentThemeDark(QGoodWindow::isSystemThemeDark());
    });

    QShortcut *shortcut1 = new QShortcut(QKeySequence(Qt::CTRL | Qt::Key_S), this);

    connect(shortcut1, &QShortcut::activated, this, [=] {
        qGoodStateHolder->setCurrentThemeDark(!qGoodStateHolder->isCurrentThemeDark());
    });

    QShortcut *shortcut2 = new QShortcut(QKeySequence(Qt::CTRL | Qt::Key_T), this);

    connect(shortcut2, &QShortcut::activated, this, [=] {
        m_goodCentralWidget->setTitleBarVisible(!m_goodCentralWidget->isTitleBarVisible());
    });

    qGoodStateHolder->setCurrentThemeDark(QGoodWindow::isSystemThemeDark());

    m_goodCentralWidget->setCentralWidget(m_centralWidget);
    setCentralWidget(m_goodCentralWidget);

    setWindowIcon(qApp->style()->standardIcon(QStyle::SP_DesktopIcon));
    setWindowTitle("Good Window - CTRL+S toggle theme - CTRL+T toggle title bar!");

    resize(800, 600);
    move(QGuiApplication::primaryScreen()->availableGeometry().center() - rect().center());

#ifdef QGOOD_WINDOW_ENABLE
    m_goodCentralWidget->setTitleAlignment(Qt::AlignCenter);
#endif
}

MainWindow::~MainWindow()
{
}

void MainWindow::themeChange()
{
    if (qGoodStateHolder->isCurrentThemeDark())
        QGoodManager::setAppDarkTheme();
    else
        QGoodManager::setAppLightTheme();
}

bool MainWindow::nativeEvent(const QByteArray &eventType, void *message, qgoodintptr *result)
{
#ifdef QGOOD_WINDOW_ENABLE
#ifdef Q_OS_WIN
#ifdef QT_VERSION_QT5
    MSG *msg = static_cast<MSG *>(message);

    switch (msg->message) {
    case WM_THEMECHANGED:
    case WM_DWMCOMPOSITIONCHANGED: {
        // Keep window theme on Windows theme change events.
        QTimer::singleShot(100, this, [=] {
            if (qGoodStateHolder->isCurrentThemeDark())
                QGoodManager::setAppDarkTheme();
            else
                QGoodManager::setAppLightTheme();
        });

        break;
    }
    default:
        break;
    }
#endif
#endif
#endif
    return QGoodWindow::nativeEvent(eventType, message, result);
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    QMessageBox msgbox(this);
    msgbox.setIcon(QMessageBox::Question);
    msgbox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    msgbox.setDefaultButton(QMessageBox::No);
    msgbox.setText("Are you sure to close?");

    int result = QGoodCentralWidget::execDialogWithWindow(&msgbox, this, m_goodCentralWidget);

    if (result != QMessageBox::Yes) {
        event->ignore();
        return;
    }
}
