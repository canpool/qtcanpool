/**
 * Copyright (C) 2018-2022 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
**/
#include "fancywindow.h"
#include "fancybar.h"
#include "fancytitlebar_p.h"

#include <QApplication>
#include <QStatusBar>

QCANPOOL_BEGIN_NAMESPACE

class FancyWindowPrivate
{
public:
    FancyWindowPrivate();

    FancyBar *fancyBar;
};


FancyWindowPrivate::FancyWindowPrivate()
{
    fancyBar = nullptr;
}

FancyWindow::FancyWindow(QWidget *parent)
    : QMainWindow(parent), d(new FancyWindowPrivate())
{
    setAttribute(Qt::WA_StyledBackground, true);
    setObjectName(QLatin1String("qtc_window"));
    // When using the native title bar, it needs to be set before creating the title bar
    QMainWindow::setWindowFlags(windowFlags() |
                Qt::FramelessWindowHint
                );

    d->fancyBar = new FancyBar(this);
    connect(d->fancyBar, SIGNAL(maximizationChanged(bool)), this, SIGNAL(resizable(bool)));
    setMenuWidget(d->fancyBar);
    setMouseTracking(true);
    QRect geom = FancyScreen::normalRect();
    resize(geom.width(), geom.height());
    raise();
    activateWindow();
}

FancyWindow::~FancyWindow()
{
    delete d;
}

FancyBar *FancyWindow::fancyBar() const
{
    return d->fancyBar;
}

void FancyWindow::setFixedSize(const QSize &s)
{
    setFixedSize(s.width(), s.height());
}

void FancyWindow::setFixedSize(int w, int h)
{
    d->fancyBar->setWidgetResizable(false);
    QWidget::setFixedSize(w, h);
}

void FancyWindow::setFixedWidth(int w)
{
    setFixedSize(w, this->height());
}

void FancyWindow::setFixedHeight(int h)
{
    setFixedSize(this->width(), h);
}

void FancyWindow::setWindowFlags(Qt::WindowFlags type)
{
    QMainWindow::setWindowFlags(Qt::FramelessWindowHint | Qt::Window | type);
    d->fancyBar->updateWidgetFlags();
}

QCANPOOL_END_NAMESPACE
