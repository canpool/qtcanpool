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
#include "imode.h"

IMode::IMode(QObject *parent)
    : QObject(parent),
      m_isEnabled(true)
{

}

IMode::~IMode()
{
    if (m_menu)
        delete m_menu;
    if (m_widget)
        delete m_widget;
}

void IMode::setEnabled(bool enabled)
{
    if (m_isEnabled == enabled)
        return;
    m_isEnabled = enabled;
    emit enabledStateChanged(m_isEnabled);
}

bool IMode::isEnabled() const
{
    return m_isEnabled;
}

QWidget *IMode::widget()
{
    if(!m_widget){
        m_widget = new QWidget();
    }
    return m_widget;
}
