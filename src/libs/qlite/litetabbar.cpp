/***************************************************************************
 **
 **  Copyright (C) 2019-2020 MaMinJie <canpool@163.com>
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
#include "litetabbar.h"

#include <QToolButton>
#include <QMap>
#include <QBoxLayout>
#include <QAction>
#include <QResizeEvent>

QLITE_BEGIN_NAMESPACE

class LiteTabBarPrivate : public QObject
{
    Q_OBJECT
public:
    LiteTabBarPrivate();
    ~LiteTabBarPrivate();

    void init();

    QToolButton *at(int index);

    inline bool validIndex(int index) const { return index >= 0 && index < m_tabs.count(); }
    void setIconSize(QSize size);

public:
    QList<QToolButton *> m_tabs;
    QList<QToolButton *> m_actionButtons;
    QMap<QAction*, QToolButton*> m_actionTabMap;
    QMap<QToolButton*, int> m_tabHeights;

    int m_currentIndex;
    LiteTabBar::Direction m_direction;
    int m_spacing;
    int m_totalHeight;
    QSize m_iconSize;

    QBoxLayout *m_frontActionLayout;
    QBoxLayout *m_middleActionLayout;
    QBoxLayout *m_backActionLayout;
    QBoxLayout *m_tabLayout;
    QBoxLayout *m_mainLayout;
    LiteTabBar *q;

public slots:
    void switchTab(bool checked);
    void pressTab();
};

LiteTabBarPrivate::LiteTabBarPrivate()
    : m_currentIndex(-1),
      m_direction(LiteTabBar::Horizontal),
      m_spacing(2),
      m_totalHeight(0),
      m_iconSize(QSize(22, 22)),
      m_frontActionLayout(nullptr),
      m_middleActionLayout(nullptr),
      m_backActionLayout(nullptr),
      m_tabLayout(nullptr),
      m_mainLayout(nullptr),
      q(nullptr)
{
}

LiteTabBarPrivate::~LiteTabBarPrivate()
{
    qDeleteAll(m_tabs);
    m_tabs.clear();
    qDeleteAll(m_actionButtons);
    m_actionButtons.clear();
    m_actionTabMap.clear();
    m_tabHeights.clear();
}

void LiteTabBarPrivate::init()
{
    QBoxLayout::Direction direction = QBoxLayout::LeftToRight;

    m_frontActionLayout = new QBoxLayout(direction);
    m_frontActionLayout->setMargin(0);
    m_frontActionLayout->setSpacing(m_spacing);

    m_middleActionLayout = new QBoxLayout(direction);
    m_middleActionLayout->setMargin(0);
    m_middleActionLayout->setSpacing(m_spacing);

    m_backActionLayout = new QBoxLayout(direction);
    m_backActionLayout->setMargin(0);
    m_backActionLayout->setSpacing(m_spacing);

    m_tabLayout = new QBoxLayout(direction);
    m_tabLayout->setMargin(0);
    m_tabLayout->setSpacing(m_spacing);

    m_mainLayout = new QBoxLayout(direction);
    m_mainLayout->setMargin(0);
    m_mainLayout->setSpacing(1);
    m_mainLayout->addLayout(m_frontActionLayout);
    m_mainLayout->addLayout(m_tabLayout);
    m_mainLayout->addLayout(m_middleActionLayout);
    m_mainLayout->addStretch();
    m_mainLayout->addLayout(m_backActionLayout);

    q->setLayout(m_mainLayout);
}

QToolButton *LiteTabBarPrivate::at(int index)
{
    return validIndex(index) ? m_tabs[index] : nullptr;
}

void LiteTabBarPrivate::setIconSize(QSize size)
{
    QList<QToolButton *> tabButtons = q->findChildren<QToolButton *>();
    foreach (QToolButton *tab, tabButtons) {
        tab->setIconSize(size);
    }
}

void LiteTabBarPrivate::switchTab(bool checked)
{
    QToolButton *tab = qobject_cast<QToolButton*>(sender());
    if (tab == nullptr) return;
    if (!checked) {
        tab->setChecked(true);
        return;
    }
    int index = m_tabs.indexOf(tab);
    q->setCurrentIndex(index);
}

void LiteTabBarPrivate::pressTab()
{
    QToolButton *tab = qobject_cast<QToolButton*>(sender());
    if (tab == nullptr) return;
    int index = m_tabs.indexOf(tab);
    q->setCurrentIndex(index);
}

LiteTabBar::LiteTabBar(QWidget *parent)
    : LiteWidget(parent),d(new LiteTabBarPrivate())
{
    d->q = this;
    d->init();
}

LiteTabBar::~LiteTabBar()
{
    delete d;
}

int LiteTabBar::addTab(const QString &text)
{
    return insertTab(-1, text);
}

int LiteTabBar::addTab(const QIcon &icon, const QString &text)
{
    return insertTab(-1, icon, text);
}

int LiteTabBar::insertTab(int index, const QString &text)
{
    return insertTab(index, QIcon(), text);
}

int LiteTabBar::insertTab(int index, const QIcon &icon, const QString &text)
{
    QToolButton *tab = new QToolButton();
    tab->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    tab->setObjectName(QLatin1String("tab"));
    tab->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    tab->setText(text);
    tab->setToolTip(text);
    tab->setIcon(icon);
    tab->setIconSize(d->m_iconSize);
    tab->setCheckable(true);
    connect(tab, SIGNAL(clicked(bool)), d, SLOT(switchTab(bool)));
    connect(tab, SIGNAL(pressed()), d, SLOT(pressTab()));

    if (d->validIndex(index)){
        d->m_tabs.insert(index, tab);
        d->m_tabLayout->insertWidget(index, tab);
    } else {
        index = d->m_tabs.count();
        d->m_tabs.append(tab);
        d->m_tabLayout->addWidget(tab);
    }

    if (d->m_tabs.count() == 1) {
        setCurrentIndex(index);
    } else if (d->m_currentIndex >= index) {
        d->m_currentIndex++;
    }
    d->m_totalHeight += tab->sizeHint().height();
    d->m_tabHeights.insert(tab, tab->sizeHint().height());
    return index;
}

void LiteTabBar::removeTab(int index)
{
    if (!d->validIndex(index)) return;

    QToolButton *tab = d->m_tabs.takeAt(index);
    d->m_tabLayout->removeWidget(tab);
    d->m_totalHeight -= d->m_tabHeights.value(tab);
    d->m_tabHeights.remove(tab);
    delete tab;

    int count = d->m_tabs.count();
    if (count == 0) {
        d->m_currentIndex = -1;
    } else if (index < d->m_currentIndex) {
        d->m_currentIndex--;
    } else if (index == d->m_currentIndex) {
        d->m_currentIndex = -1;
        if(index == count) {
            index--;
        }
        setCurrentIndex(index);
    }
}

int LiteTabBar::currentIndex() const
{
    if (d->validIndex(d->m_currentIndex)) {
        return d->m_currentIndex;
    }
    return -1;
}

int LiteTabBar::count() const
{
    return d->m_tabs.count();
}

void LiteTabBar::addAction(QAction *action, LiteTabBar::ActionPosition position)
{
    QToolButton *pButton = new QToolButton();
    pButton->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    pButton->setObjectName(QLatin1String("button"));
    pButton->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    pButton->setText(action->text());
    pButton->setToolTip(action->toolTip());
    pButton->setIconSize(d->m_iconSize);
    pButton->setDefaultAction(action);
    d->m_actionButtons.append(pButton);
    d->m_actionTabMap.insert(action, pButton);

    switch (position) {
        case LiteTabBar::Front: {
            d->m_frontActionLayout->addWidget(pButton);
            break;
        }
        case LiteTabBar::Middle: {
            d->m_middleActionLayout->addWidget(pButton);
            break;
        }
        case LiteTabBar::Back: {
            d->m_backActionLayout->addWidget(pButton);
            break;
        }
    }
    d->m_totalHeight += pButton->sizeHint().height();
}

void LiteTabBar::setTabEnabled(int index, bool enable)
{
    if (QToolButton *tab = d->at(index)) {
        tab->setEnabled(enable);
        if (!enable && index == d->m_currentIndex) {
            setCurrentIndex(d->validIndex(index + 1) ? (index + 1) : 0);
        } else if (enable && !d->validIndex(d->m_currentIndex)) {
            setCurrentIndex(index);
        }
    }
}

void LiteTabBar::setTabVisible(int index, bool visible)
{
    if (QToolButton *tab = d->at(index)) {
        tab->setVisible(visible);
        if (!visible && index == d->m_currentIndex) {
            setCurrentIndex(d->validIndex(index + 1) ? (index + 1) : 0);
        } else if (visible && !d->validIndex(d->m_currentIndex)) {
            setCurrentIndex(index);
        }
    }
}

void LiteTabBar::setTabButtonStyle(Qt::ToolButtonStyle style)
{
    QList<QToolButton *> tabButtons = findChildren<QToolButton *>();
    foreach (QToolButton *tab, tabButtons) {
        tab->setToolButtonStyle(style);
    }
}

void LiteTabBar::setActionStyle(QAction *action, Qt::ToolButtonStyle style)
{
    QToolButton *pButton = d->m_actionTabMap.value(action);
    if (pButton) {
        pButton->setToolButtonStyle(style);
    }
}

void LiteTabBar::setTabSpacing(int spacing)
{
    if (d->m_spacing == spacing)
        return;
    d->m_spacing = spacing;
    d->m_frontActionLayout->setSpacing(spacing);
    d->m_middleActionLayout->setSpacing(spacing);
    d->m_backActionLayout->setSpacing(spacing);
    d->m_tabLayout->setSpacing(spacing);
}

void LiteTabBar::setIconSize(QSize size)
{
    d->setIconSize(size);
    d->m_iconSize = size;
}

void LiteTabBar::setDirection(LiteTabBar::Direction direction)
{
    QBoxLayout::Direction layoutDirection = QBoxLayout::LeftToRight;
    if (direction == LiteTabBar::Vertical) {
        layoutDirection = QBoxLayout::TopToBottom;
    }
    d->m_frontActionLayout->setDirection(layoutDirection);
    d->m_middleActionLayout->setDirection(layoutDirection);
    d->m_backActionLayout->setDirection(layoutDirection);
    d->m_tabLayout->setDirection(layoutDirection);
    d->m_mainLayout->setDirection(layoutDirection);
}

void LiteTabBar::setCurrentIndex(int index)
{
    if(index == d->m_currentIndex) return;
    if(d->m_currentIndex != -1){
        d->m_tabs.at(d->m_currentIndex)->setChecked(false);
    }
    d->m_currentIndex = index;
    d->m_tabs.at(d->m_currentIndex)->setChecked(true);
    emit currentChanged(index);
}

void LiteTabBar::resizeEvent(QResizeEvent *event)
{
    // Because of padding and border-width are set in qss,
    // so + 2 * icon.heigth
    if (event->size().height() <= d->m_totalHeight + d->m_iconSize.height() * 2) {
        d->setIconSize(QSize(0, 0));
    } else {
        d->setIconSize(d->m_iconSize);
    }
}

QLITE_END_NAMESPACE

#include "litetabbar.moc"
