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
#include "fancymodemanager.h"
#include "fancytabwidget.h"
#include "fancymode.h"

#include <QMap>
#include <QMouseEvent>
#include <QVector>

QCANPOOL_BEGIN_NAMESPACE

class ModeManagerPrivate : public QObject
{
    Q_OBJECT
public:
    FancyTabWidget *m_modeStack;
    QVector<FancyMode *> m_modes;
    int m_oldCurrent;
    int m_menuIndex;

    bool validIndex(int index) {
        return index >= 0 && index < m_modes.size();
    }

public slots:
    void showMenu(int index, QPoint pos);
    void hideMenu();
};

void ModeManagerPrivate::showMenu(int index, QPoint pos)
{
    if (m_modes.at(index)->menu()) {
        m_menuIndex = index;
        m_modes.at(index)->menu()->popup(pos);
    }
}

void ModeManagerPrivate::hideMenu()
{
    m_modeStack->hideMenu(m_menuIndex);
}

static ModeManagerPrivate *d = nullptr;

FancyModeManager::FancyModeManager(FancyTabWidget *modeStack, QObject *parent)
    : QObject(parent)
{
    d = new ModeManagerPrivate();
    d->m_modeStack = modeStack;
    d->m_oldCurrent = -1;
    d->m_menuIndex = -1;
    d->m_modes.clear();
    connect(d->m_modeStack, SIGNAL(currentChanged(int)), this, SLOT(currentTabChanged(int)));
    connect(d->m_modeStack, SIGNAL(menuTriggered(int, QPoint)), d, SLOT(showMenu(int, QPoint)));
}

FancyModeManager::~FancyModeManager()
{
    delete d;
    d = nullptr;
}

void FancyModeManager::addMode(FancyMode *mode)
{
    if (mode == nullptr) {
        return;
    }
    // Count the number of modes with a higher priority
    int index = 0;
    foreach (const FancyMode * m, d->m_modes) {
        if (m->priority() >= mode->priority()) {
            ++index;
        }
    }

    d->m_modes.insert(index, mode);
    d->m_modeStack->insertTab(index, mode->widget(), mode->icon(), mode->displayName(),
                              mode->menu() != nullptr);
    d->m_modeStack->setTabEnabled(index, mode->isEnabled());

    if (!mode->menu()) {
        d->m_modeStack->setTabToolTip(index, tr("Switch to <b>%1</b> mode\nCtrl+%2")
                                      .arg(mode->displayName()).arg(index + 1));
        d->m_modeStack->setTabShortcut(index, tr("Ctrl+%1").arg(index + 1));
    } else {
        d->m_modeStack->setTabToolTip(index, mode->menu()->toolTip());
    }

    if (mode->menu()) {
        connect(mode->menu(), SIGNAL(aboutToHide()), d, SLOT(hideMenu()));
    }
}

void FancyModeManager::removeMode(FancyMode *mode)
{
    if (mode == nullptr) {
        return;
    }
    int index = d->m_modes.indexOf(mode);
    if (index == -1) {
        return;
    }
    d->m_modes.remove(index);
    d->m_modeStack->removeTab(index);
}

void FancyModeManager::setCurrentMode(FancyMode *mode)
{
    int index = d->m_modes.indexOf(mode);
    d->m_modeStack->setCurrentIndex(index);
}

void FancyModeManager::setCurrentIndex(int index)
{
    if (d->validIndex(index)) {
        d->m_modeStack->setCurrentIndex(index);
    }
}

FancyMode *FancyModeManager::currentMode() const
{
    int currentIndex = d->m_modeStack->currentIndex();
    if (currentIndex < 0) {
        return nullptr;
    }
    return d->m_modes.at(currentIndex);
}

FancyMode *FancyModeManager::mode(int index) const
{
    if (d->validIndex(index)) {
        return d->m_modes.at(index);
    }
    return nullptr;
}

void FancyModeManager::setModeEnabled(FancyMode *mode, bool enable)
{
    int index = d->m_modes.indexOf(mode);
    d->m_modeStack->setTabEnabled(index, enable);
}

void FancyModeManager::setModeVisible(FancyMode *mode, bool visible)
{
    int index = d->m_modes.indexOf(mode);
    d->m_modeStack->setTabVisible(index, visible);
}

void FancyModeManager::objectAdded(QObject *obj)
{
    FancyMode *mode = qobject_cast<FancyMode *>(obj);
    addMode(mode);
}

void FancyModeManager::objectRemoved(QObject *obj)
{
    FancyMode *mode = qobject_cast<FancyMode *>(obj);
    removeMode(mode);
}

void FancyModeManager::currentTabChanged(int index)
{
    if (d->validIndex(index) && index != d->m_oldCurrent) {
        FancyMode *mode = d->m_modes.at(index);
        FancyMode *oldMode = nullptr;

        if (d->validIndex(d->m_oldCurrent)) {
            oldMode = d->m_modes.at(d->m_oldCurrent);
        }

        d->m_oldCurrent = index;
        emit currentModeChanged(mode ? mode : oldMode);
    }
}

QCANPOOL_END_NAMESPACE

#include "fancymodemanager.moc"
