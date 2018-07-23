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
#include "fancytabbar.h"
#include "fancytab.h"

#include <QBoxLayout>
#include <QMap>
#include <QAction>
#include <QMenu>
#include <QMouseEvent>

class FancyTabBarPrivate : public QObject
{
    Q_OBJECT
public:
    FancyTabBarPrivate();
    ~FancyTabBarPrivate();
    void updateTabBarPosition();
    void init();

    FancyTabBar::Direction m_direction;

    int m_currentIndex;
    QList<FancyTab*> m_modeTabs;
    QList<FancyTab*> m_actionTabs;
    QMap<QAction*, FancyTab*> m_actionTabMap;

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

signals:

public slots:
    void switchTab();
    void menuTriggered(QMouseEvent *e);
};

FancyTabBarPrivate::FancyTabBarPrivate()
{
    m_direction = FancyTabBar::Vertical;
    m_currentIndex = -1;

    m_modeTabs.clear();
    m_actionTabs.clear();
    m_actionTabMap.clear();

    m_layout = 0;
    m_modeLayout = 0;
    m_menuModeLayout = 0;
    m_frontActionLayout = 0;
    m_middleActionLayout = 0;
    m_backActionLayout = 0;

    m_headSpacer = 0;
    m_splitLine = 0;

    m_modeStyle = FancyTabBar::TextUnderIcon;
    m_actionStyle = FancyTabBar::IconOnly;

    q = 0;
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
    case FancyTabBar::Horizontal:
        direction = QBoxLayout::LeftToRight;
        m_headSpacer->setFixedWidth(5);
        m_splitLine->setFixedWidth(0);
        m_layout->setSpacing(5);
        q->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
        break;
    case FancyTabBar::Vertical:
    default:
        direction = QBoxLayout::TopToBottom;
        m_headSpacer->setFixedHeight(25);
        m_splitLine->setFixedHeight(1);
        m_layout->setSpacing(0);
        q->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
        break;
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
    if(m_direction == FancyTabBar::Horizontal){
        m_headSpacer->setFixedWidth(5);
    }else{
        m_headSpacer->setFixedHeight(25);
    }

    // split line
    m_splitLine = new QWidget();
    if(m_direction == FancyTabBar::Horizontal){
        m_splitLine->setFixedWidth(0);
    }else{
        m_splitLine->setFixedHeight(1);
    }
    m_splitLine->setAutoFillBackground(true);
    m_splitLine->setBackgroundRole(QPalette::Dark);

    // main layout
    m_layout = new QBoxLayout(direction);
    m_layout->setMargin(0);
    if(m_direction == FancyTabBar::Horizontal){
        m_layout->setSpacing(5);
    }else{
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

    if(m_direction == FancyTabBar::Horizontal){
        q->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    }else{
        q->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
    }
}

void FancyTabBarPrivate::switchTab()
{
    FancyTab *tab = qobject_cast<FancyTab*>(sender());
    if(tab == 0) return;
    int index = m_modeTabs.indexOf(tab);
    if(index == m_currentIndex) return;
    if(m_currentIndex != -1){
        m_modeTabs.at(m_currentIndex)->select(false);
    }
    m_currentIndex = index;
    m_modeTabs.at(m_currentIndex)->select(true);
    emit q->currentChanged(m_currentIndex);
}

void FancyTabBarPrivate::menuTriggered(QMouseEvent *e)
{
    FancyTab *tab = qobject_cast<FancyTab*>(sender());
    if(tab == 0) return;
    int index = m_modeTabs.indexOf(tab);

    int x = e->x();
    int y = e->y();
    QPoint pos = e->globalPos();
    if(m_direction == FancyTabBar::Vertical){
        pos.setX(pos.x()-x+tab->width());
        pos.setY(pos.y()-y);
    }else if(m_direction == FancyTabBar::Horizontal){
        pos.setX(pos.x()-x);
        pos.setY(pos.y()-y+tab->height());
    }
    emit q->menuTriggered(index, pos);
}

FancyTabBar::FancyTabBar(QWidget *parent)
    : QWidget(parent),d(new FancyTabBarPrivate())
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
    if(d->m_direction == direction)
        return;
    d->m_direction = direction;
    d->updateTabBarPosition();
}

bool FancyTabBar::validIndex(int index) const
{
    return index >= 0 && index < d->m_modeTabs.count();
}

void FancyTabBar::setTabEnabled(int index, bool enable)
{
    Q_ASSERT(index < d->m_modeTabs.size());
    Q_ASSERT(index >= 0);
    if (index < d->m_modeTabs.size() && index >= 0) {
        FancyTab *tab = d->m_modeTabs[index];
        tab->setEnabled(enable);
        QFont font = tab->font();
        font.setItalic(!enable);
        tab->setFont(font);
//        QIcon icon = tab->icon();
//        icon.setIsMask(!enable);
//        tab->setIcon(icon);
    }
}

bool FancyTabBar::isTabEnabled(int index) const
{
    Q_ASSERT(index < d->m_modeTabs.size());
    Q_ASSERT(index >= 0);

    if (index < d->m_modeTabs.size() && index >= 0)
        return d->m_modeTabs[index]->isEnabled();

    return false;
}

void FancyTabBar::setTabVisible(int index, bool visible)
{
    Q_ASSERT(index < d->m_modeTabs.size());
    Q_ASSERT(index >= 0);
    if (index < d->m_modeTabs.size() && index >= 0){
        d->m_modeTabs[index]->setHidden(!visible);
    }
}

bool FancyTabBar::isTabVisible(int index) const
{
    Q_ASSERT(index < d->m_modeTabs.size());
    Q_ASSERT(index >= 0);
    return d->m_modeTabs[index]->isVisible();
}

void FancyTabBar::insertTab(int index, const QIcon &icon, const QString &label, bool hasMenu)
{
    FancyTab *tab = new FancyTab(this);
    tab->setIcon(icon);
    tab->setText(label);
    tab->setHasMenu(hasMenu);
    if(d->m_direction == Vertical){
        tab->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
        tab->setShortcut(tr("Ctrl+%1").arg(index+1));
    }else if(d->m_direction == Horizontal){
        tab->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    }
    d->m_modeTabs.insert(index, tab);
    connect(tab, SIGNAL(clicked(bool)), d, SLOT(switchTab()));
    connect(tab, SIGNAL(menuTriggered(QMouseEvent*)), d, SLOT(menuTriggered(QMouseEvent*)));
    if(d->m_currentIndex >= index)
        d->m_currentIndex++;
    if(hasMenu){
        d->m_menuModeLayout->addWidget(tab);
    }else{
        d->m_modeLayout->insertWidget(index, tab);
    }
}

void FancyTabBar::removeTab(int index)
{
    FancyTab *tab = d->m_modeTabs.takeAt(index);
    if(tab->m_hasMenu){
        d->m_menuModeLayout->removeWidget(tab);
    }else{
        d->m_modeLayout->removeWidget(tab);
    }
    delete tab;
}

void FancyTabBar::setCurrentIndex(int index)
{
    if(!validIndex(index)) return;
    if(isTabEnabled(index) && index != d->m_currentIndex){
        if(validIndex(d->m_currentIndex)){
            d->m_modeTabs.at(d->m_currentIndex)->select(false);
        }
        d->m_currentIndex = index;
        // to do
        d->m_modeTabs.at(d->m_currentIndex)->select(true);
        emit currentChanged(d->m_currentIndex);
    }
}

int FancyTabBar::currentIndex() const
{
    return d->m_currentIndex;
}

void FancyTabBar::setTabToolTip(int index, QString toolTip)
{
    if(!validIndex(index))
        return;
    d->m_modeTabs[index]->setToolTip(toolTip);
}

QString FancyTabBar::tabToolTip(int index) const
{
    if(!validIndex(index))
        return QString("");
    return d->m_modeTabs[index]->toolTip();
}

void FancyTabBar::setTabStyle(FancyTabBar::TabType type, FancyTabBar::TabStyle style)
{
    if(type == Mode){
        d->m_modeStyle = style;
        foreach (FancyTab *tab, d->m_modeTabs) {
            tab->setToolButtonStyle((Qt::ToolButtonStyle)style);
        }
    }else if(type == Action){
        d->m_actionStyle = style;
        foreach (FancyTab *tab, d->m_actionTabs) {
            tab->setToolButtonStyle((Qt::ToolButtonStyle)style);
        }
    }
}

void FancyTabBar::setTabFont(FancyTabBar::TabType type, QFont &font)
{
    if(type == Mode){
        foreach (FancyTab *tab, d->m_modeTabs) {
            tab->setFont(font);
        }
    }else if(type == Action){
        foreach (FancyTab *tab, d->m_actionTabs) {
            tab->setFont(font);
        }
    }
}

void FancyTabBar::setTabSpace(TabType type, int space)
{
    if(type == Mode){
        d->m_modeLayout->setSpacing(space);
    }else if(type == Action){
        d->m_frontActionLayout->setSpacing(space);
        d->m_middleActionLayout->setSpacing(space);
        d->m_backActionLayout->setSpacing(space);
    }
}

void FancyTabBar::setTabIconSize(QSize size)
{
    foreach (FancyTab *tab, d->m_modeTabs) {
        tab->setIconSize(size);
    }
    foreach (FancyTab *tab, d->m_actionTabs) {
        tab->setIconSize(size);
    }
}

int FancyTabBar::addAction(QAction *action, FancyTabBar::ActionPosition position)
{
    FancyTab *tab = new FancyTab();
    tab->setType(FancyTab::Action);
    tab->setText(action->text());
    tab->setIcon(action->icon());
    tab->setToolButtonStyle((Qt::ToolButtonStyle)d->m_actionStyle);
    tab->setToolTip(action->toolTip());
    tab->setDefaultAction(action);
    if(d->m_direction == Vertical){
        tab->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    }else if(d->m_direction == Horizontal){
        tab->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    }
    connect(tab, SIGNAL(clicked(bool)), action, SIGNAL(triggered(bool)));
    d->m_actionTabs.append(tab);
    d->m_actionTabMap.insert(action, tab);
    if(position == Front){
        d->m_frontActionLayout->addWidget(tab);
    }else if(position == Middle){
        d->m_middleActionLayout->addWidget(tab);
    }else if(position == Back){
        d->m_backActionLayout->addWidget(tab);
    }
    return d->m_actionTabs.count() - 1;
}

void FancyTabBar::removeAction(QAction *action)
{
    FancyTab *tab = d->m_actionTabMap.value(action);
    if(!tab) return;
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
    if(tab){
        tab->setToolButtonStyle((Qt::ToolButtonStyle)style);
    }
}

void FancyTabBar::setHoverColor(const QColor &color)
{
    FancyTab::setHoverColor(color);
}

QColor FancyTabBar::hoverColor() const
{
    return FancyTab::hoverColor();
}

void FancyTabBar::setPressColor(const QColor &color)
{
    FancyTab::setPressColor(color);
}

QColor FancyTabBar::pressColor() const
{
    return FancyTab::pressColor();
}

void FancyTabBar::setTextColor(const QColor &color)
{
    FancyTab::setTextColor(color);
}

void FancyTabBar::setSelectedTextColor(const QColor &color)
{
    FancyTab::setSelectedTextColor(color);
}

void FancyTabBar::setBackgroundColor(const QColor &color)
{
    QPalette palette;
    palette.setColor(QPalette::Background, color);
    this->setPalette(palette);
    this->setAutoFillBackground(true);
}

void FancyTabBar::setHeadSpace(int space)
{
    if(space < 0 || space > 50) return;
    d->m_headSpacer->resize(QSize(1,1));
    if(d->m_direction == Vertical){
        d->m_headSpacer->setFixedHeight(space);
    }else{
        d->m_headSpacer->setFixedWidth(space);
    }
}

void FancyTabBar::hideMenu(int index)
{
    if(!validIndex(index)) return;
    d->m_modeTabs.at(index)->select(false);
}

#include "fancytabbar.moc"
