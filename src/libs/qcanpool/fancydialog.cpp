/**
 * Copyright (C) 2018-2022 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
**/
#include "fancydialog.h"
#include "fancybar.h"
#include "fancytitlebar.h"

#include <QVBoxLayout>
#include <QSettings>
#include <QPainter>

QCANPOOL_BEGIN_NAMESPACE

class FancyDialogPrivate
{
public:
    FancyDialogPrivate();

    FancyBar *fancyBar;
    QWidget *centralWidget;
};

FancyDialogPrivate::FancyDialogPrivate()
{
    fancyBar = nullptr;
    centralWidget = nullptr;
}

FancyDialog::FancyDialog(QWidget *parent, Qt::WindowFlags f)
    : QDialog(parent), d(new FancyDialogPrivate())
{
    setAttribute(Qt::WA_StyledBackground, true);
    setObjectName(QLatin1String("qtc_dialog"));

    d->fancyBar = new FancyBar(this);
    d->fancyBar->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    d->centralWidget = new QWidget();

    QVBoxLayout *layout = new QVBoxLayout();
    layout->setContentsMargins(1, 0, 1, 1);
    layout->setSpacing(0);
    layout->addWidget(d->fancyBar);
    layout->addWidget(d->centralWidget);
    QDialog::setLayout(layout);

    resize(600, 300);

    setWindowFlags(Qt::WindowSystemMenuHint |
                   Qt::WindowMinimizeButtonHint |
                   f);
}

FancyDialog::~FancyDialog()
{
    delete d;
}

void FancyDialog::setCentralWidget(QWidget *w)
{
    if (d->centralWidget == w || w == nullptr) {
        return;
    }
    if (d->centralWidget) {
        QDialog::layout()->removeWidget(d->centralWidget);
        d->centralWidget->setParent(nullptr);
        d->centralWidget->deleteLater();
    }
    d->centralWidget = w;
    QDialog::layout()->addWidget(d->centralWidget);
}

void FancyDialog::setWidgetResizable(bool resizable)
{
    d->fancyBar->setWidgetResizable(resizable);
}

void FancyDialog::setTitleBarHeight(int height)
{
    d->fancyBar->setTitleBarHeight(height);
}

QAction *FancyDialog::addAction(const QIcon &icon, const QString &text)
{
    return d->fancyBar->titleBar()->addAction(icon, text);
}

void FancyDialog::addAction(QAction *action)
{
    d->fancyBar->titleBar()->addAction(action);
}

QLayout *FancyDialog::layout() const
{
    return d->centralWidget->layout();
}

void FancyDialog::setLayout(QLayout *layout)
{
    d->centralWidget->setLayout(layout);
}

void FancyDialog::setFixedSize(const QSize &s)
{
    setFixedSize(s.width(), s.height());
}

void FancyDialog::setFixedSize(int w, int h)
{
    d->fancyBar->setWidgetResizable(false);
    QWidget::setFixedSize(w, h);
}

void FancyDialog::setFixedWidth(int w)
{
    setFixedSize(w, this->height());
}

void FancyDialog::setFixedHeight(int h)
{
    setFixedSize(this->width(), h);
}

void FancyDialog::setWindowFlags(Qt::WindowFlags type)
{
    QDialog::setWindowFlags(Qt::FramelessWindowHint | Qt::Dialog | type);
    d->fancyBar->updateWidgetFlags();
}

QCANPOOL_END_NAMESPACE
