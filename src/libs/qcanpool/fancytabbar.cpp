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
#include "fancytabbar.h"
#include "fancytab.h"

#include <QBoxLayout>
#include <QMap>
#include <QAction>
#include <QMenu>
#include <QMouseEvent>

QCANPOOL_BEGIN_NAMESPACE

class FancyTabBarPrivate : public QObject
{
    Q_OBJECT
public:
    FancyTabBarPrivate();
    ~FancyTabBarPrivate();

    void updateTabBarPosition();
    void init();

    inline bool validIndex(int index) const
    { return index >= 0 && index < m_modeTabs.count(); }

    inline bool validActionIndex(int index) const
    { return index >= 0 && index < m_actionTabs.count(); }

    void setTabAttribute(FancyTab *tab);
    void setCurrentIndex(int index);

    FancyTab *addAction(QAction *action);
    int addAction(QAction *action, FancyTabBar::ActionPosition position);
    int insertAction(FancyTab *before, QAction *action);

public:
    FancyTabBar::Direction m_direction;

    int m_currentIndex;
    QList<FancyTab *> m_modeTabs;
    QList<FancyTab *> m_actionTabs;
    QMap<QAction *, FancyTab *> m_actionTabMap;

    QBoxLayout *m_layout;
    QBoxLayout *m_modeLayout;
    QBoxLayout *m_menuModeLayout;
    QBoxLayout *m_frontActionLayout;
    QBoxLayout *m_middleActionLayout;
    QBoxLayout *m_backActionLayout;

    QWidget *m_headSpacer;
    QWidget *m_splitLine;

    FancyTabBar::TabStyle m_modeStyle;
    FancyTabBar::TabStyle m_actionStyle;

    FancyTabBar *q;

    QSize m_tabIconSize;
    QColor m_tabHoverColor;
    QColor m_tabPressColor;
    QColor m_tabTextColor;
    QColor m_tabSelectedTextColor;
    QColor m_backgroudColor;


public slots:
    void switchTab();
    void menuTriggered(QMouseEvent *e);
};

FancyTabBarPrivate::FancyTabBarPrivate()
    : m_direction(FancyTabBar::Vertical)
    , m_currentIndex(-1)
    , m_modeStyle(FancyTabBar::TextUnderIcon)
    , m_actionStyle(FancyTabBar::IconOnly)
    , m_tabIconSize(QSize(24, 24))
    , m_tabHoverColor(QColor(255, 255, 255, 50))
    , m_tabPressColor(QColor(0, 0, 0, 100))
    , m_tabTextColor(QColor(255, 255, 255))
    , m_tabSelectedTextColor(QColor(255, 255, 255))
    , m_backgroudColor(QColor())
{
}

FancyTabBarPrivate::~FancyTabBarPrivate()
{
    qDeleteAll(m_modeTabs);
    m_modeTabs.clear();
    qDeleteAll(m_actionTabs);
    m_actionTabs.clear();
    m_actionTabMap.clear();
}

void FancyTabBarPrivate::updateTabBarPosition()
{
    QBoxLayout::Direction direction;

    switch (m_direction) {
        case FancyTabBar::Horizontal: {
            direction = QBoxLayout::LeftToRight;
            m_headSpacer->setFixedWidth(5);
            m_splitLine->setFixedWidth(0);
            m_layout->setSpacing(5);
            q->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
            break;
        }
        case FancyTabBar::Vertical: {
            direction = QBoxLayout::TopToBottom;
            m_headSpacer->setFixedHeight(25);
            m_splitLine->setFixedHeight(1);
            m_layout->setSpacing(0);
            q->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
            break;
        }
    }

    m_layout->setDirection(direction);
    m_frontActionLayout->setDirection(direction);
    m_modeLayout->setDirection(direction);
    m_middleActionLayout->setDirection(direction);
    m_menuModeLayout->setDirection(direction);
    m_backActionLayout->setDirection(direction);
}

void FancyTabBarPrivate::init()
{
    QBoxLayout::Direction direction = QBoxLayout::TopToBottom;
    // mode layout
    m_modeLayout = new QBoxLayout(direction);
    m_modeLayout->setMargin(0);
    m_modeLayout->setSpacing(0);
    // menu mode layout
    m_menuModeLayout = new QBoxLayout(direction);
    m_menuModeLayout->setMargin(0);
    m_menuModeLayout->setSpacing(0);
    // front action layout
    m_frontActionLayout = new QBoxLayout(direction);
    m_frontActionLayout->setMargin(0);
    m_frontActionLayout->setSpacing(0);
    // middle action layout
    m_middleActionLayout = new QBoxLayout(direction);
    m_middleActionLayout->setMargin(0);
    m_middleActionLayout->setSpacing(0);
    // back action layout
    m_backActionLayout = new QBoxLayout(direction);
    m_backActionLayout->setMargin(0);
    m_backActionLayout->setSpacing(0);
    // head spacer
    m_headSpacer = new QWidget();

    if (m_direction == FancyTabBar::Horizontal) {
        m_headSpacer->setFixedWidth(5);
    } else {
        m_headSpacer->setFixedHeight(25);
    }

    // split line
    m_splitLine = new QWidget();

    if (m_direction == FancyTabBar::Horizontal) {
        m_splitLine->setFixedWidth(0);
    } else {
        m_splitLine->setFixedHeight(1);
    }

    m_splitLine->setAutoFillBackground(true);
    m_splitLine->setBackgroundRole(QPalette::Dark);
    // main layout
    m_layout = new QBoxLayout(direction);
    m_layout->setMargin(0);

    if (m_direction == FancyTabBar::Horizontal) {
        m_layout->setSpacing(5);
    } else {
        m_layout->setSpacing(0);
    }

    m_layout->addWidget(m_headSpacer);
    m_layout->addLayout(m_frontActionLayout);
    m_layout->addLayout(m_modeLayout);
    m_layout->addLayout(m_middleActionLayout);
    m_layout->addStretch();
    m_layout->addWidget(m_splitLine);
    m_layout->addSpacing(1);
    m_layout->addLayout(m_menuModeLayout);
    m_layout->addLayout(m_backActionLayout);
    m_layout->addSpacing(8);
    q->setLayout(m_layout);

    if (m_direction == FancyTabBar::Horizontal) {
        q->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    } else {
        q->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
    }
}

void FancyTabBarPrivate::setTabAttribute(FancyTab *tab)
{
    tab->setIconSize(m_tabIconSize);
    tab->setHoverColor(m_tabHoverColor);
    tab->setPressColor(m_tabPressColor);
    tab->setTextColor(m_tabTextColor);
    tab->setSelectedTextColor(m_tabSelectedTextColor);
}

void FancyTabBarPrivate::setCurrentIndex(int index)
{
    if (index == m_currentIndex && m_modeTabs.at(index)->isSelected()) {
        return;
    }
    if (index != m_currentIndex) {
        if (validIndex(m_currentIndex)) {
            m_modeTabs.at(m_currentIndex)->select(false);
        }
        m_currentIndex = index;
    }
    m_modeTabs.at(index)->select(true);
    emit q->currentChanged(index);
}

FancyTab *FancyTabBarPrivate::addAction(QAction *action)
{
    FancyTab *tab = new FancyTab();
    tab->setType(FancyTab::Action);
    tab->setText(action->text());
    tab->setIcon(action->icon());
    tab->setToolButtonStyle(static_cast<Qt::ToolButtonStyle>(m_actionStyle));
    tab->setToolTip(action->toolTip());
    tab->setDefaultAction(action);

    setTabAttribute(tab);

    if (m_direction == FancyTabBar::Vertical) {
        tab->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    } else if (m_direction == FancyTabBar::Horizontal) {
        tab->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
    }

    connect(tab, SIGNAL(clicked(bool)), action, SIGNAL(triggered(bool)));

    return tab;
}

int FancyTabBarPrivate::addAction(QAction *action, FancyTabBar::ActionPosition position)
{
    FancyTab *tab = addAction(action);

    m_actionTabs.append(tab);
    m_actionTabMap.insert(action, tab);

    if (position == FancyTabBar::Front) {
        m_frontActionLayout->addWidget(tab);
    } else if (position == FancyTabBar::Middle) {
        m_middleActionLayout->addWidget(tab);
    } else if (position == FancyTabBar::Back) {
        m_backActionLayout->addWidget(tab);
    }

    return m_actionTabs.count() - 1;
}

int FancyTabBarPrivate::insertAction(FancyTab *before, QAction *action)
{
    FancyTab *tab = addAction(action);

    int index = m_actionTabs.indexOf(before);
    m_actionTabs.insert(index, tab);
    m_actionTabMap.insert(action, tab);

    int idx = -1;
    if ((idx = m_frontActionLayout->indexOf(before)) != -1) {
        m_frontActionLayout->insertWidget(idx, tab);
    } else if ((idx = m_middleActionLayout->indexOf(before)) != -1) {
        m_middleActionLayout->insertWidget(idx, tab);
    } else if ((idx = m_backActionLayout->indexOf(before)) != -1) {
        m_backActionLayout->insertWidget(idx, tab);
    }

    return index;
}

void FancyTabBarPrivate::switchTab()
{
    FancyTab *tab = qobject_cast<FancyTab *>(sender());
    if (tab == nullptr) {
        return;
    }
    int index = m_modeTabs.indexOf(tab);
    setCurrentIndex(index);
}

void FancyTabBarPrivate::menuTriggered(QMouseEvent *e)
{
    FancyTab *tab = qobject_cast<FancyTab *>(sender());

    if (tab == nullptr) {
        return;
    }

    int index = m_modeTabs.indexOf(tab);
    int x = e->x();
    int y = e->y();
    QPoint pos = e->globalPos();

    if (m_direction == FancyTabBar::Vertical) {
        pos.setX(pos.x() - x + tab->width());
        pos.setY(pos.y() - y);
    } else if (m_direction == FancyTabBar::Horizontal) {
        pos.setX(pos.x() - x);
        pos.setY(pos.y() - y + tab->height());
    }

    emit q->menuTriggered(index, pos);
}

FancyTabBar::FancyTabBar(QWidget *parent)
    : QWidget(parent), d(new FancyTabBarPrivate())
{
    d->q = this;
    d->init();
}

FancyTabBar::~FancyTabBar()
{
    delete d;
}

void FancyTabBar::setDirection(FancyTabBar::Direction direction)
{
    if (d->m_direction == direction) {
        return;
    }

    d->m_direction = direction;
    d->updateTabBarPosition();
}

void FancyTabBar::setTabEnabled(int index, bool enable)
{
    if (d->validIndex(index)) {
        FancyTab *tab = d->m_modeTabs[index];
        tab->setEnabled(enable);
        QFont font = tab->font();
        font.setItalic(!enable);
        tab->setFont(font);
    }
}

bool FancyTabBar::isTabEnabled(int index) const
{
    if (d->validIndex(index)) {
        return d->m_modeTabs[index]->isEnabled();
    }

    return false;
}

void FancyTabBar::setTabVisible(int index, bool visible)
{
    if (d->validIndex(index)) {
        d->m_modeTabs[index]->setHidden(!visible);
    }
}

bool FancyTabBar::isTabVisible(int index) const
{
    if (d->validIndex(index)) {
        return d->m_modeTabs[index]->isVisible();
    }
    return false;
}

int FancyTabBar::addTab(const QString &label, bool hasMenu)
{
    return insertTab(-1, label, hasMenu);
}

int FancyTabBar::addTab(const QIcon &icon, const QString &label, bool hasMenu)
{
    return insertTab(-1, icon, label, hasMenu);
}

int FancyTabBar::insertTab(int index, const QString &label, bool hasMenu)
{
    return insertTab(index, QIcon(), label, hasMenu);
}

int FancyTabBar::insertTab(int index, const QIcon &icon, const QString &label, bool hasMenu)
{
    FancyTab *tab = new FancyTab(this);
    tab->setIcon(icon);
    tab->setText(label);
    tab->setHasMenu(hasMenu);

    d->setTabAttribute(tab);

    if (d->m_direction == Vertical) {
        tab->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    } else if (d->m_direction == Horizontal) {
        tab->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
    }

    connect(tab, SIGNAL(clicked(bool)), d, SLOT(switchTab()));
    connect(tab, SIGNAL(menuTriggered(QMouseEvent *)), d, SLOT(menuTriggered(QMouseEvent *)));

    if (d->validIndex(index)) {
        d->m_modeTabs.insert(index, tab);
    } else {
        index = d->m_modeTabs.count();
        d->m_modeTabs.append(tab);
    }

    if (d->m_modeTabs.count() == 1) {
        setCurrentIndex(index);
    } else if (d->m_currentIndex >= index) {
        d->m_currentIndex++;
    }

    if (hasMenu) {
        d->m_menuModeLayout->addWidget(tab);
    } else {
        d->m_modeLayout->insertWidget(index, tab);
    }
    return index;
}

void FancyTabBar::removeTab(int index)
{
    if (!d->validIndex(index)) return;

    FancyTab *tab = d->m_modeTabs.takeAt(index);

    if (tab->hasMenu()) {
        d->m_menuModeLayout->removeWidget(tab);
    } else {
        d->m_modeLayout->removeWidget(tab);
    }

    delete tab;

    // note: disable tab
    int count = d->m_modeTabs.count();
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

QWidget *FancyTabBar::tabWidget(int index) const
{
    if (d->validIndex(index)) {
        return d->m_modeTabs.at(index);
    }
    return nullptr;
}

void FancyTabBar::setCurrentIndex(int index)
{
    if (!d->validIndex(index) || !isTabEnabled(index)) {
        return;
    }
    d->setCurrentIndex(index);
}

int FancyTabBar::currentIndex() const
{
    return d->m_currentIndex;
}

void FancyTabBar::unselectCurrent()
{
    if (d->validIndex(d->m_currentIndex)) {
        d->m_modeTabs.at(d->m_currentIndex)->select(false);
    }
}

void FancyTabBar::setTabToolTip(int index, QString toolTip)
{
    if (d->validIndex(index)) {
        d->m_modeTabs[index]->setToolTip(toolTip);
    }
}

QString FancyTabBar::tabToolTip(int index) const
{
    if (d->validIndex(index)) {
        return d->m_modeTabs[index]->toolTip();
    }
    return QString("");
}

void FancyTabBar::setTabShortcut(int index, const QString &shortcut)
{
    if (d->validIndex(index)) {
        d->m_modeTabs[index]->setShortcut(shortcut);
    }
}

void FancyTabBar::setTabStyle(FancyTabBar::TabType type, FancyTabBar::TabStyle style)
{
    if (type == Mode) {
        d->m_modeStyle = style;
        foreach (FancyTab * tab, d->m_modeTabs) {
            tab->setToolButtonStyle(static_cast<Qt::ToolButtonStyle>(style));
        }
    } else if (type == Action) {
        d->m_actionStyle = style;
        foreach (FancyTab * tab, d->m_actionTabs) {
            tab->setToolButtonStyle(static_cast<Qt::ToolButtonStyle>(style));
        }
    }
}

void FancyTabBar::setTabFont(FancyTabBar::TabType type, QFont &font)
{
    if (type == Mode) {
        foreach (FancyTab * tab, d->m_modeTabs) {
            tab->setFont(font);
        }
    } else if (type == Action) {
        foreach (FancyTab * tab, d->m_actionTabs) {
            tab->setFont(font);
        }
    }
}

void FancyTabBar::setTabSpace(TabType type, int space)
{
    if (type == Mode) {
        d->m_modeLayout->setSpacing(space);
    } else if (type == Action) {
        d->m_frontActionLayout->setSpacing(space);
        d->m_middleActionLayout->setSpacing(space);
        d->m_backActionLayout->setSpacing(space);
    }
}

void FancyTabBar::setTabIconSize(QSize size)
{
    d->m_tabIconSize = size;
    foreach (FancyTab * tab, d->m_modeTabs) {
        tab->setIconSize(size);
    }
    foreach (FancyTab * tab, d->m_actionTabs) {
        tab->setIconSize(size);
    }
}

int FancyTabBar::addAction(QAction *action, FancyTabBar::ActionPosition position)
{
    if (action == nullptr) {
        return -1;
    }
    return d->addAction(action, position);
}

int FancyTabBar::insertAction(int index, QAction *action)
{
    if (!d->validActionIndex(index) || action == nullptr) {
        return -1;
    }
    FancyTab *tab = d->m_actionTabs.at(index);
    return d->insertAction(tab, action);
}

int FancyTabBar::insertAction(QAction *before, QAction *action)
{
    if (before == nullptr || action == nullptr) {
        return -1;
    }
    FancyTab *tab = d->m_actionTabMap.value(before, nullptr);
    if (tab == nullptr) {
        return -1;
    }
    return d->insertAction(tab, action);
}

void FancyTabBar::removeAction(QAction *action)
{
    FancyTab *tab = d->m_actionTabMap.value(action);

    if (!tab) {
        return;
    }

    d->m_actionTabs.removeAll(tab);
    d->m_actionTabMap.remove(action);
    d->m_frontActionLayout->removeWidget(tab);
    d->m_middleActionLayout->removeWidget(tab);
    d->m_backActionLayout->removeWidget(tab);
    delete tab;
}

void FancyTabBar::setActionStyle(QAction *action, FancyTabBar::TabStyle style)
{
    FancyTab *tab = d->m_actionTabMap.value(action);

    if (tab) {
        tab->setToolButtonStyle(static_cast<Qt::ToolButtonStyle>(style));
    }
}

QWidget *FancyTabBar::widgetForAction(QAction *action) const
{
    if (action == nullptr) {
        return nullptr;
    }
    return d->m_actionTabMap.value(action, nullptr);
}

void FancyTabBar::setHoverColor(const QColor &color)
{
    d->m_tabHoverColor = color;
    foreach (FancyTab * tab, d->m_modeTabs) {
        tab->setHoverColor(color);
    }
    foreach (FancyTab * tab, d->m_actionTabs) {
        tab->setHoverColor(color);
    }
}

QColor FancyTabBar::hoverColor() const
{
    return d->m_tabHoverColor;
}

void FancyTabBar::setPressColor(const QColor &color)
{
    d->m_tabPressColor = color;
    foreach (FancyTab * tab, d->m_modeTabs) {
        tab->setPressColor(color);
    }
    foreach (FancyTab * tab, d->m_actionTabs) {
        tab->setPressColor(color);
    }
}

QColor FancyTabBar::pressColor() const
{
    return d->m_tabPressColor;
}

void FancyTabBar::setTextColor(const QColor &color)
{
    d->m_tabTextColor = color;
    foreach (FancyTab * tab, d->m_modeTabs) {
        tab->setTextColor(color);
    }
    foreach (FancyTab * tab, d->m_actionTabs) {
        tab->setTextColor(color);
    }
}

void FancyTabBar::setSelectedTextColor(const QColor &color)
{
    d->m_tabSelectedTextColor = color;
    foreach (FancyTab * tab, d->m_modeTabs) {
        tab->setSelectedTextColor(color);
    }
    foreach (FancyTab * tab, d->m_actionTabs) {
        tab->setSelectedTextColor(color);
    }
}

void FancyTabBar::setBackgroundColor(const QColor &color)
{
    QPalette palette;
    palette.setColor(QPalette::Window, color);
    this->setPalette(palette);
    this->setAutoFillBackground(true);
}

void FancyTabBar::setHeadSpace(int space)
{
    if (space < 0 || space > 50) {
        return;
    }

    d->m_headSpacer->resize(QSize(1, 1));

    if (d->m_direction == Vertical) {
        d->m_headSpacer->setFixedHeight(space);
    } else {
        d->m_headSpacer->setFixedWidth(space);
    }
}

void FancyTabBar::hideMenu(int index)
{
    if (d->validIndex(index)) {
        d->m_modeTabs.at(index)->select(false);
    }
}

QCANPOOL_END_NAMESPACE

#include "fancytabbar.moc"
