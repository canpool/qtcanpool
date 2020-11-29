/***************************************************************************
 **
 **  Copyright (C) 2018-2020 MaMinJie <canpool@163.com>
 **  Contact: https://github.com/canpool
 **           https://gitee.com/icanpool
 **
 **  GNU Lesser General Public License Usage
 **  Alternatively, this file may be used under the terms of the GNU Lesser
 **  General Public License version 3 as published by the Free Software
 **  Foundation and appearing in the file LICENSE.LGPL3 included in the
 **  packaging of this file. Please review the following information to
 **  ensure the GNU Lesser General Public License version 3 requirements
 **  will be met: https://www.gnu.org/licenses/lgpl-3.0.html.
 **
 **  GNU General Public License Usage
 **  Alternatively, this file may be used under the terms of the GNU
 **  General Public License version 2.0 or (at your option) the GNU General
 **  Public license version 3 or any later version approved by the KDE Free
 **  Qt Foundation. The licenses are as published by the Free Software
 **  Foundation and appearing in the file LICENSE.GPL2 and LICENSE.GPL3
 **  included in the packaging of this file. Please review the following
 **  information to ensure the GNU General Public License requirements will
 **  be met: https://www.gnu.org/licenses/gpl-2.0.html and
 **  https://www.gnu.org/licenses/gpl-3.0.html.
 **
****************************************************************************/
#include "fancydialog.h"
#include "fancybar.h"
#include "qcanpool_p.h"

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

QCANPOOL_END_NAMESPACE
