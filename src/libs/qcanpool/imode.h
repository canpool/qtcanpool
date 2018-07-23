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
#ifndef IMODE_H
#define IMODE_H

#include <QWidget>

#include <QIcon>
#include <QMenu>

#include "qcanpool_global.h"

class QCANPOOL_SHARED_EXPORT IMode : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool enabled READ isEnabled WRITE setEnabled)
public:
    explicit IMode(QObject *parent = 0);
    ~IMode();

    QString displayName() const { return m_displayName;}
    QIcon icon() const { return m_icon;}
    int priority() const { return m_priority; }
    bool isEnabled() const;
    QMenu *menu() const { return m_menu; }
    void setWidget(QWidget *widget) { m_widget = widget; }
    QWidget* widget();

    void setEnabled(bool enabled);
    void setDisplayName(const QString &displayName) { m_displayName = displayName;}
    void setIcon(const QIcon &icon) { m_icon = icon;}
    void setPriority(int priority) { m_priority = priority; }
    void setMenu(QMenu *menu) { m_menu = menu; }

signals:
    void enabledStateChanged(bool enabled);

private:
    QString m_displayName;
    QIcon m_icon;
    QMenu *m_menu = nullptr;
    QWidget *m_widget = nullptr;
    int m_priority = -1;
    bool m_isEnabled;

};

#endif // IMODE_H
