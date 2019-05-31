/***************************************************************************
 **
 **  Copyright (C) 2018 MaMinJie <canpool@163.com>
 **  Contact: https://github.com/canpool
 **
 **  This program is free software: you can redistribute it and/or modify
 **  it under the terms of the GNU General Public License as published by
 **  the Free Software Foundation, either version 3 of the License, or
 **  (at your option) any later version.
 **
 **  This program is distributed in the hope that it will be useful,
 **  but WITHOUT ANY WARRANTY; without even the implied warranty of
 **  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 **  GNU General Public License for more details.
 **
 **  You should have received a copy of the GNU General Public License
 **  along with this program.  If not, see http://www.gnu.org/licenses/.
 **
****************************************************************************/
#include "fancydialog.h"
#include "fancybar.h"
#include "qcanpool_p.h"

#include <QVBoxLayout>
#include <QSettings>
#include <QPainter>

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
    QDialog::setWindowFlags(Qt::FramelessWindowHint |
                   Qt::WindowSystemMenuHint |
//                   Qt::WindowMinimizeButtonHint |
                   Qt::Dialog | f
                  );
    d->fancyBar = new FancyBar(this);
    d->fancyBar->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    d->centralWidget = new QWidget();
    QVBoxLayout *layout = new QVBoxLayout();
    layout->setContentsMargins(1, 0, 1, 1);
    layout->setSpacing(0);
    layout->addWidget(d->fancyBar);
    layout->addWidget(d->centralWidget);
    setLayout(layout);
    resize(800, 600);
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
        layout()->removeWidget(d->centralWidget);
        d->centralWidget->setParent(nullptr);
        d->centralWidget->deleteLater();
    }
    d->centralWidget = w;
    layout()->addWidget(d->centralWidget);
}

void FancyDialog::setWidgetResizable(bool resizable)
{
    d->fancyBar->setWidgetResizable(resizable);
}

void FancyDialog::setTitleBarHeight(int height)
{
    d->fancyBar->setTitleBarHeight(height);
}

void FancyDialog::addAdditionalControl(QAction *action)
{
    d->fancyBar->addAdditionalControl(action, FancyBar::TitlePosition);
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
    QDialog::setWindowFlags(type);
    d->fancyBar->updateWidgetFlags();
}

void FancyDialog::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    QSettings settings(QCanpoolPrivate::g_settingsOrganization,
                       QCanpoolPrivate::g_settingsApplication);
    QString skinName = settings.value("skin").toString();

    if (skinName.isEmpty()) {
        skinName = QString(":/skin/default");
    }

    QPainter painter(this);
    painter.setPen(Qt::NoPen);
    painter.setBrush(Qt::gray);
    painter.drawPixmap(QRect(0, 0, this->width(), this->height()),
                       QPixmap(skinName));
}
