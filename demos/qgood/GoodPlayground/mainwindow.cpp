/**
 * Copyright (C) 2023 maminjie <canpool@163.com> (https://github.com/canpool)
 * Copyright (C) 2023 Antonio Dias (https://github.com/antonypro)
 * SPDX-License-Identifier: MIT
 **/

#include "mainwindow.h"
#include "qgood/qgoodmanager.h"
#include "qgood/qgoodstateholder.h"

#include <QEvent>
#include <QGuiApplication>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QScreen>
#include <QSettings>
#include <QTimer>

MainWindow::MainWindow(QWidget *parent) : QGoodWindow(parent)
{
    m_settings = new QSettings("settings.ini", QSettings::IniFormat, this);

    m_goodCentralWidget = new QGoodCentralWidget(this);

    // Contents of the window
    m_label = new QLabel("Hello world!", this);
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

#ifdef QGOOD_WINDOW_ENABLE
    m_goodCentralWidget->setUnifiedTitleBarAndCentralWidget(true);

    // Align title to the right side
    m_goodCentralWidget->setTitleAlignment(Qt::AlignRight);

    // Add widget to the left of the title bar
    m_goodCentralWidget->setLeftTitleBarWidget(new QPushButton("Hello 1", this));

    QWidget *widget = new QWidget(this);
    widget->setFixedWidth(300);
    QHBoxLayout *layout1 = new QHBoxLayout(widget);
    layout1->setContentsMargins(0, 0, 0, 0);
    layout1->addWidget(new QPushButton("Hello 2", this));
    layout1->addWidget(new QLineEdit(this));

    // Add widget to the center of the title bar
    m_goodCentralWidget->setCenterTitleBarWidget(widget, true);

    // Add widget to the right of the title bar
    m_goodCentralWidget->setRightTitleBarWidget(new QPushButton("Hello 3", this));
#endif
}

MainWindow::~MainWindow()
{
}

void MainWindow::adjustSizeLabel()
{
    QFont font = m_label->font();

    QFontMetrics metrics(font);
    QSize size = metrics.size(0, m_label->text());
    qreal scale_width = qreal(m_label->width()) / qreal(size.width());
    qreal scale_height = qreal(m_label->height()) / qreal(size.height());

    qreal scale = qMin(scale_width, scale_height);

    if (qFuzzyIsNull(scale))
        scale = qreal(1);

    font.setPointSizeF(font.pointSizeF() * scale);
    m_label->setFont(font);
}

bool MainWindow::event(QEvent *event)
{
    switch (event->type()) {
    case QEvent::Show:
    case QEvent::Resize: {
        QTimer::singleShot(0, this, &MainWindow::adjustSizeLabel);
        break;
    }
    default:
        break;
    }

    return QGoodWindow::event(event);
}

void MainWindow::show()
{
    QTimer::singleShot(0, this, [=] {
        // Restore window geometry
        if (!restoreGeometry(m_settings->value("geometry").toByteArray())) {
            // If there is no saved geometry, use a default geometry
            if (QWidget *central_widget = m_goodCentralWidget->centralWidget()) {
                resize(qMax(central_widget->minimumSizeHint().width(), 500),
                       qMax(central_widget->minimumSizeHint().height(), 200));
            }

            move(QGuiApplication::primaryScreen()->availableGeometry().center() - rect().center());
        }

        // Call parent show
        QGoodWindow::show();
    });
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    Q_UNUSED(event)

    // Save geometry for next open
    m_settings->setValue("geometry", saveGeometry());
}
