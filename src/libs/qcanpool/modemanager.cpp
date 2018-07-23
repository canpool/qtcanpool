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
#include "modemanager.h"
#include "fancytabwidget.h"
#include "imode.h"

#include <QMap>
#include <QMouseEvent>
#include <QVector>

class ModeManagerPrivate : public QObject
{
    Q_OBJECT
public:
    FancyTabWidget *m_modeStack;
    QVector<IMode*> m_modes;
    int m_oldCurrent;
    int m_menuIndex;

    bool validIndex(int index) { return index >=0 && index < m_modes.size();}

public slots:
    void showMenu(int index, QPoint pos);
    void hideMenu();
};

void ModeManagerPrivate::showMenu(int index, QPoint pos)
{
    if(m_modes.at(index)->menu()){
        m_menuIndex = index;
        m_modes.at(index)->menu()->popup(pos);
//        d->m_modes.at(index)->menu()->popup(event->globalPos());
    }
}

void ModeManagerPrivate::hideMenu()
{
    m_modeStack->hideMenu(m_menuIndex);
}

ModeManagerPrivate *d;

ModeManager::ModeManager(FancyTabWidget *modeStack, QObject *parent)
    : QObject(parent)
{
    d = new ModeManagerPrivate();
    d->m_modeStack = modeStack;
    d->m_oldCurrent = -1;
    d->m_menuIndex = -1;
    d->m_modes.clear();

    connect(d->m_modeStack, SIGNAL(currentChanged(int)), this, SLOT(currentTabChanged(int)));
    connect(d->m_modeStack, SIGNAL(menuTriggered(int,QPoint)), d, SLOT(showMenu(int,QPoint)));
}

ModeManager::~ModeManager()
{
    delete d;
    d = 0;
}

void ModeManager::setCurrentMode(IMode *mode)
{
    int index = d->m_modes.indexOf(mode);
    d->m_modeStack->setCurrentIndex(index);
}

void ModeManager::setCurrentIndex(int index)
{
    if(index < 0 || index >= d->m_modes.count()){
        return;
    }
    d->m_modeStack->setCurrentIndex(index);
}

IMode *ModeManager::currentMode() const
{
    int currentIndex = d->m_modeStack->currentIndex();
    if(currentIndex < 0){
        return 0;
    }
    return d->m_modes.at(currentIndex);
}

IMode *ModeManager::mode(int index) const
{
    if(index >= 0 && index < d->m_modes.count())
        return d->m_modes.at(index);
    return 0;
}

void ModeManager::setEnabled(IMode *mode, bool enable)
{
    int index = d->m_modes.indexOf(mode);
    d->m_modeStack->setTabEnabled(index, enable);
}

void ModeManager::setVisible(IMode *mode, bool visible)
{
    int index = d->m_modes.indexOf(mode);
    d->m_modeStack->setTabVisible(index, visible);
}

void ModeManager::objectAdded(QObject *obj)
{
    IMode *mode = qobject_cast<IMode *>(obj);
    if (!mode)
        return;

    // Count the number of modes with a higher priority
    int index = 0;
    foreach (const IMode *m, d->m_modes)
        if (m->priority() >= mode->priority())
            ++index;

    d->m_modes.insert(index, mode);
    d->m_modeStack->insertTab(index, mode->widget(), mode->icon(), mode->displayName(),
                              mode->menu() != nullptr);
    d->m_modeStack->setTabEnabled(index, mode->isEnabled());
    if(!mode->menu()){
        d->m_modeStack->setTabToolTip(index, tr("Switch to <b>%1</b> mode\nCtrl+%2")
                                      .arg(mode->displayName()).arg(index+1));
    }else{
        d->m_modeStack->setTabToolTip(index, mode->menu()->toolTip());
    }

    if(mode->menu()){
        connect(mode->menu(), SIGNAL(aboutToHide()), d, SLOT(hideMenu()));
    }
}

void ModeManager::objectRemoved(QObject *obj)
{
    IMode *mode = qobject_cast<IMode *>(obj);
    if (!mode){
        return;
    }
    int index = d->m_modes.indexOf(mode);
    if(index == -1) return;
    d->m_modes.remove(index);
    d->m_modeStack->removeTab(index);
}

void ModeManager::currentTabChanged(int index)
{
    if(d->validIndex(index) && index != d->m_oldCurrent){
        IMode *mode = d->m_modes.at(index);

        IMode *oldMode = 0;
        if(d->validIndex(d->m_oldCurrent))
            oldMode = d->m_modes.at(d->m_oldCurrent);
        d->m_oldCurrent = index;
        emit currentModeChanged(mode ? mode : oldMode);
    }
}

#include "modemanager.moc"
