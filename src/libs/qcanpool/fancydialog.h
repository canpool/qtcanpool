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
#ifndef FANCYDIALOG_H
#define FANCYDIALOG_H

#include <QDialog>
#include "qcanpool_global.h"

QCANPOOL_BEGIN_NAMESPACE

class FancyDialogPrivate;
class QCANPOOL_SHARED_EXPORT FancyDialog : public QDialog
{
    Q_OBJECT
public:
    explicit FancyDialog(QWidget *parent = nullptr, Qt::WindowFlags f = Qt::WindowFlags());
    ~FancyDialog();

    void setCentralWidget(QWidget *w);
    void setWidgetResizable(bool resizable);
    void setTitleBarHeight(int height);

    void addAdditionalControl(QAction *action);

    void setFixedSize(const QSize &s);
    void setFixedSize(int w, int h);
    void setFixedWidth(int w);
    void setFixedHeight(int h);

    void setWindowFlags(Qt::WindowFlags type);

protected:
    virtual void paintEvent(QPaintEvent *event);

private:
    FancyDialogPrivate *d;
};

QCANPOOL_END_NAMESPACE

#endif // FANCYDIALOG_H
