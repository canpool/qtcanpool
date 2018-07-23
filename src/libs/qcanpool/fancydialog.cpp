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

#include <QVBoxLayout>
#include <QSettings>
#include <QPainter>

class FancyDialogPrivate : public QObject
{
public:
    FancyDialogPrivate(QObject *parent = 0);

    FancyBar *fancyBar;
    QWidget *centralWidget;
};

FancyDialogPrivate::FancyDialogPrivate(QObject *parent)
    : QObject(parent)
{
    fancyBar = 0;
    centralWidget = 0;
}

FancyDialog::FancyDialog(QWidget *parent, Qt::WindowFlags f)
    : QDialog(parent),d(new FancyDialogPrivate(this))
{
    setWindowFlags(Qt::FramelessWindowHint |
                   Qt::WindowSystemMenuHint |
                   Qt::WindowMinimizeButtonHint |
                   Qt::Dialog
                   );
    d->fancyBar = new FancyBar(this, Qt::Dialog | f);
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

}

void FancyDialog::setCentralWidget(QWidget *w)
{
    if(d->centralWidget){
        layout()->removeWidget(d->centralWidget);
        delete d->centralWidget;
        d->centralWidget = w;
        layout()->addWidget(d->centralWidget);
    }
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

void FancyDialog::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);

    QSettings settings("Canpool", "qtcanpool");
    QString skinName = settings.value("skin").toString();
    if(skinName.isEmpty()){
        skinName = QString(":/skin/default");
    }
    QPainter painter(this);
    painter.setPen(Qt::NoPen);
    painter.setBrush(Qt::gray);
    painter.drawPixmap(QRect(0,0,this->width(),this->height()),
                       QPixmap(skinName));
}
