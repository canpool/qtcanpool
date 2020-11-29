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
#include "fancymodebar.h"
#include "fancymode.h"

#include <QStackedWidget>

QCANPOOL_BEGIN_NAMESPACE

/* FancyModeBarPrivate */
class FancyModeBarPrivate : public QObject
{
    Q_OBJECT
public:
    FancyModeBarPrivate();
    virtual ~FancyModeBarPrivate();

    inline bool validIndex(int index)
    { return index >= 0 && index < m_modes.size(); }

    void init();

public slots:
    void showMenu(int index, QPoint pos);
    void hideMenu();

public:
    FancyModeBar *q;
    QStackedWidget *m_modeStack;
    QList<FancyMode *> m_modes;
    int m_menuIndex;
};

FancyModeBarPrivate::FancyModeBarPrivate()
    : m_menuIndex(-1)
{
}

FancyModeBarPrivate::~FancyModeBarPrivate()
{}

void FancyModeBarPrivate::init()
{
    connect(q, SIGNAL(currentChanged(int)), m_modeStack, SLOT(setCurrentIndex(int)));
    connect(q, SIGNAL(menuTriggered(int, QPoint)), this, SLOT(showMenu(int, QPoint)));
}

void FancyModeBarPrivate::showMenu(int index, QPoint pos)
{
    if (m_modes.at(index)->menu()) {
        m_menuIndex = index;
        m_modes.at(index)->menu()->popup(pos);
    }
}

void FancyModeBarPrivate::hideMenu()
{
    q->hideMenu(m_menuIndex);
}

/* FancyModeBar */
FancyModeBar::FancyModeBar(QStackedWidget *modeStack, QWidget *parent)
    : FancyTabBar(parent)
    , d(new FancyModeBarPrivate())
{
    Q_ASSERT(modeStack);

    d->q = this;
    d->m_modeStack = modeStack;
    d->init();
}

FancyModeBar::~FancyModeBar()
{
    delete d;
}

void FancyModeBar::addMode(FancyMode *mode)
{
    if (mode == nullptr) {
        return;
    }
    int index = 0;
    foreach (const FancyMode * m, d->m_modes) {
        if (m->priority() >= mode->priority()) {
            ++index;
        }
    }
    d->m_modes.insert(index, mode);
    d->m_modeStack->insertWidget(index, mode->widget());
    insertTab(index, mode->icon(), mode->displayName(), mode->menu() != nullptr);
    setTabEnabled(index, mode->isEnabled());

    if (mode->menu()) {
        connect(mode->menu(), SIGNAL(aboutToHide()), d, SLOT(hideMenu()));
    }
}

void FancyModeBar::removeMode(FancyMode *mode)
{
    if (mode == nullptr) {
        return;
    }
    int index = d->m_modes.indexOf(mode);
    if (index == -1) {
        return;
    }
    d->m_modes.removeAt(index);
    removeTab(index);
}

void FancyModeBar::setCurrentMode(FancyMode *mode)
{
    int index = d->m_modes.indexOf(mode);
    setCurrentIndex(index);
}

FancyMode *FancyModeBar::currentMode() const
{
    int currentIndex = d->m_modeStack->currentIndex();
    if (currentIndex < 0) {
        return nullptr;
    }
    return d->m_modes.at(currentIndex);
}

FancyMode *FancyModeBar::mode(int index) const
{
    if (d->validIndex(index)) {
        return d->m_modes.at(index);
    }
    return nullptr;
}

void FancyModeBar::setModeEnabled(FancyMode *mode, bool enable)
{
    int index = d->m_modes.indexOf(mode);
    setTabEnabled(index, enable);
}

void FancyModeBar::setModeVisible(FancyMode *mode, bool visible)
{
    int index = d->m_modes.indexOf(mode);
    setTabVisible(index, visible);
}

QCANPOOL_END_NAMESPACE

#include "fancymodebar.moc"
