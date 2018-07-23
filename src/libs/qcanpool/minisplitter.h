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
#ifndef MINISPLITTER_H
#define MINISPLITTER_H

#include <QSplitter>
#include "qcanpool_global.h"

QT_BEGIN_NAMESPACE
class QSplitterHandle;
QT_END_NAMESPACE

class QCANPOOL_SHARED_EXPORT MiniSplitter : public QSplitter
{
    Q_OBJECT
public:
    enum SplitterStyle {Dark, Light};

    MiniSplitter(QWidget *parent = 0, SplitterStyle style = Dark);
    MiniSplitter(Qt::Orientation orientation, SplitterStyle style = Dark);

protected:
    QSplitterHandle *createHandle();

private:
    SplitterStyle m_style;
};

#endif // MINISPLITTER_H
