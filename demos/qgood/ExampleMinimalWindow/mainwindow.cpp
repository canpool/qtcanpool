/**
 * Copyright (C) 2023 maminjie <canpool@163.com> (https://github.com/canpool)
 * Copyright (C) 2022-2023 Antonio Dias (https://github.com/antonypro)
 * SPDX-License-Identifier: MIT
 **/

#include "mainwindow.h"
#include "qgoodbye/qgoodmanager.h"
#include "qgoodbye/qgoodstateholder.h"
#include <QLabel>

MainWindow::MainWindow(QWidget *parent) : QGoodWindow(parent)
{
    m_goodCentralWidget = new QGoodCentralWidget(this);

    // Contents of the window
    m_label = new QLabel("Hello world!", this);
    QFont font = m_label->font();
    font.setPointSize(72);
    m_label->setFont(font);
    m_label->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
    m_label->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);

    // Function that sets the state holder theme
    auto func_theme = [=] {
        qGoodStateHolder->setCurrentThemeDark(QGoodWindow::isSystemThemeDark());
    };

    // React to state holder theme change and apply our dark or light theme to the whole app
    connect(qGoodStateHolder, &QGoodStateHolder::currentThemeChanged, this, [=] {
        if (qGoodStateHolder->isCurrentThemeDark())
            QGoodManager::setAppDarkTheme();
        else
            QGoodManager::setAppLightTheme();
    });

    // React to system theme change
    connect(this, &QGoodWindow::systemThemeChanged, this, func_theme);

    // Set the central widget of QGoodCentralWidget
    m_goodCentralWidget->setCentralWidget(m_label);

    // Set the central widget of QGoodWindow which is QGoodCentralWidget
    setCentralWidget(m_goodCentralWidget);

    // Set initial theme
    func_theme();
}

MainWindow::~MainWindow()
{
}
