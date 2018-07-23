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
#ifndef MODEMANAGER_H
#define MODEMANAGER_H

#include <QObject>
#include "qcanpool_global.h"

class FancyTabWidget;
class IMode;

class QCANPOOL_SHARED_EXPORT ModeManager : public QObject
{
    Q_OBJECT
public:
    explicit ModeManager(FancyTabWidget *modeStack, QObject *parent = 0);
    ~ModeManager();

    void setCurrentMode(IMode *mode);
    void setCurrentIndex(int index);
    IMode* currentMode() const;
    IMode* mode(int index) const;

    void setEnabled(IMode *mode, bool enable);
    void setVisible(IMode *mode, bool visible);

signals:
    void currentModeChanged(IMode *mode);

public slots:
    void objectAdded(QObject *obj);
    void objectRemoved(QObject *obj);
    void currentTabChanged(int index);
};

#endif // MODEMANAGER_H
