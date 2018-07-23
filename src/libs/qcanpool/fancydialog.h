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
#ifndef FANCYDIALOG_H
#define FANCYDIALOG_H

#include <QDialog>
#include "qcanpool_global.h"

class FancyDialogPrivate;
class QCANPOOL_SHARED_EXPORT FancyDialog : public QDialog
{
    Q_OBJECT
public:
    explicit FancyDialog(QWidget *parent = 0, Qt::WindowFlags f = 0);
    ~FancyDialog();

    void setCentralWidget(QWidget *w);
    void setWidgetResizable(bool resizable);
    void setTitleBarHeight(int height);

    void addAdditionalControl(QAction *action);

protected:
    virtual void paintEvent(QPaintEvent *event);

private:
    FancyDialogPrivate *d;
};

#endif // FANCYDIALOG_H
