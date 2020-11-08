/***************************************************************************
 **
 **  Copyright (C) 2018-2020 MaMinJie <canpool@163.com>
 **  Contact: https://github.com/canpool
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
#include "fancymodebar_p.h"
#include "fancymode.h"
#include "fancybar.h"
#include "fancytab.h"

#include <QAction>
#include <QBoxLayout>
#include <QStackedWidget>
#include <QScrollArea>

FancyModeBarPrivate::FancyModeBarPrivate(QObject *parent)
    : QObject(parent)
{
    m_modeMap.clear();
    m_modes.clear();
//    m_modeButtons.clear();
    m_actionButtons.clear();
    m_actionMap.clear();
    m_modeStyle = Qt::ToolButtonTextUnderIcon;
    m_actionStyle = Qt::ToolButtonIconOnly;
    m_modeStack = nullptr;
    m_modeLayout = nullptr;
    m_frontActionLayout = nullptr;
    m_backActionLayout = nullptr;
    m_spacer = nullptr;
    m_line = nullptr;
}

FancyModeBarPrivate::~FancyModeBarPrivate()
{
    qDeleteAll(m_modeMap);
    m_modeMap.clear();
//    qDeleteAll(m_modes);
    m_modes.clear();
//    qDeleteAll(m_modeButtons);
//    m_modeButtons.clear();
    qDeleteAll(m_actionButtons);
    m_actionButtons.clear();
    m_actionMap.clear();
}

void FancyModeBarPrivate::selectMode(FancyMode *mode)
{
    FancyTab *pButton = m_modeMap.key(mode);

    if (pButton) {
        foreach (FancyTab * button, m_modeMap.keys()) {
            button->select(button == pButton);
        }
        m_modeStack->setCurrentWidget(mode->widget());
    } else if (m_modes.count() > 0) {
        selectMode(m_modes.at(0));
    }
}

void FancyModeBarPrivate::setEnabled(FancyMode *mode, bool enable)
{
    FancyTab *pButton = m_modeMap.key(mode);

    if (pButton) {
        pButton->setEnabled(enable);
        QFont font = pButton->font();
        font.setItalic(!enable);
        pButton->setFont(font);
//        if(enable){
//            pButton->setIcon(mode->icon());
//        }else{
//            pButton->setIcon(mode->disIcon());
//        }
    }
}

void FancyModeBarPrivate::setVisible(FancyMode *mode, bool visible)
{
    FancyTab *pButton = m_modeMap.key(mode);

    if (pButton) {
        pButton->setVisible(visible);
        mode->widget()->setVisible(visible);
    }
}

// slots
void FancyModeBarPrivate::switchMode()
{
    FancyTab *pButton = qobject_cast<FancyTab *>(sender());

    if (pButton) {
        foreach (FancyTab * button, m_modeMap.keys()) {
            button->select(button == pButton);
        }
        m_modeStack->setCurrentWidget(m_modeMap.value(pButton)->widget());
    }
}

/********************************************************************************
 * class :
 *      FancyModeBar
 *
 *******************************************************************************/
FancyModeBar::FancyModeBar(QStackedWidget *modeStack, Direction direction, QWidget *parent)
    : QWidget(parent), d(new FancyModeBarPrivate)
{
    d->m_modeStack = modeStack;
    m_direction = direction;
    QBoxLayout::Direction layoutDirection;

    if (direction == Horizontal) {
        layoutDirection = QBoxLayout::LeftToRight;
    } else {
        layoutDirection = QBoxLayout::TopToBottom;
    }

    // mode layout
    d->m_modeLayout = new QBoxLayout(layoutDirection);
//    d->m_modeLayout->addStretch();
    d->m_modeLayout->setSpacing(0);
    d->m_modeLayout->setMargin(0);
    // front action layout
    d->m_frontActionLayout = new QBoxLayout(layoutDirection);
    d->m_frontActionLayout->setSpacing(0);
    d->m_frontActionLayout->setMargin(0);
    d->m_middleActionLayout = new QBoxLayout(layoutDirection);
    d->m_middleActionLayout->setSpacing(0);
    d->m_middleActionLayout->setMargin(0);
    // back action layout
    d->m_backActionLayout = new QBoxLayout(layoutDirection);
    d->m_backActionLayout->setSpacing(0);
    d->m_backActionLayout->setMargin(0);
    QBoxLayout *spacerLayout = new QBoxLayout(layoutDirection);
    spacerLayout->addLayout(d->m_backActionLayout);
    int sbh = 8;
    spacerLayout->addSpacing(sbh);
    spacerLayout->setMargin(0);
    spacerLayout->setSpacing(0);
    d->m_spacer = new QWidget();

    if (direction == Horizontal) {
        d->m_spacer->setFixedWidth(5);
    } else {
        d->m_spacer->setFixedHeight(25);
    }

    d->m_line = new QWidget();

    if (direction == Horizontal) {
        d->m_line->setFixedWidth(0);
    } else {
        d->m_line->setFixedHeight(1);
    }

    d->m_line->setAutoFillBackground(true);
    d->m_line->setBackgroundRole(QPalette::Dark);
    QBoxLayout *pMainLayout = new QBoxLayout(layoutDirection);
    pMainLayout->addWidget(d->m_spacer);
    pMainLayout->addLayout(d->m_frontActionLayout);
    pMainLayout->addLayout(d->m_modeLayout);
//    pMainLayout->addWidget(pScrollArea);
    pMainLayout->addLayout(d->m_middleActionLayout);
    pMainLayout->addStretch();
    pMainLayout->addWidget(d->m_line);
    pMainLayout->addLayout(spacerLayout);

    if (direction == Horizontal) {
        pMainLayout->setSpacing(5);
    } else {
        pMainLayout->setSpacing(0);
    }

    pMainLayout->setContentsMargins(0, 0, 0, 0);
    setLayout(pMainLayout);

    if (direction == Horizontal) {
        setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    } else {
        setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
    }

    connect(d->m_modeStack, SIGNAL(currentChanged(int)), this, SIGNAL(currentChanged(int)));
}

FancyModeBar::~FancyModeBar()
{
    delete d;
}

void FancyModeBar::addMode(FancyMode *mode)
{
    FancyTab *pButton = new FancyTab(mode->icon());
    pButton->setText(mode->displayName());
    pButton->setToolButtonStyle(d->m_modeStyle);
    d->m_modeStack->addWidget(mode->widget());
    d->m_modeMap.insert(pButton, mode);
    d->m_modes.append(mode);
    connect(pButton, SIGNAL(clicked()), d, SLOT(switchMode()));
    int index = d->m_modeStack->count();

    if (index == 0) {
        index++;
    }

    if (m_direction == Vertical) {
        pButton->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
        pButton->setShortcut(tr("Ctrl+%1").arg(index));
        pButton->setToolTip(tr("Switch to <b>%1</b> mode\nCtrl+%2").arg(pButton->text()).arg(index));
    } else if (m_direction == Horizontal) {
        pButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
        pButton->setShortcut(tr("Alt+Ctrl+%1").arg(index));
        pButton->setToolTip(tr("Switch to <b>%1</b> mode\nAlt+Ctrl+%2").arg(pButton->text()).arg(index));
    }

    d->m_modeLayout->insertWidget(index - 1, pButton);
}

void FancyModeBar::selectMode(FancyMode *mode)
{
    d->selectMode(mode);
}

void FancyModeBar::setEnabled(FancyMode *mode, bool enable)
{
    d->setEnabled(mode, enable);
}

void FancyModeBar::setVisible(FancyMode *mode, bool visible)
{
    d->setVisible(mode, visible);
    FancyMode *currentMode = d->m_modes.at(d->m_modeStack->currentIndex());

    if (!visible) {
        if (mode == currentMode) {
            for (int i = 0; i < d->m_modeStack->count(); i++) {
                FancyMode *tmpMode = d->m_modes.at(i);
                FancyTab *button = d->m_modeMap.key(tmpMode);

                if (button && !button->isHidden()) {
                    selectMode(tmpMode);
                }
            }
        }
    } else {
        if (mode != currentMode) {
            mode->widget()->setVisible(false);
        }
    }
}

void FancyModeBar::setButtonStyle(FancyModeBar::ButtonType type, Qt::ToolButtonStyle style)
{
    if (type == Mode) {
        d->m_modeStyle = style;
        foreach (FancyTab * button, d->m_modeMap.keys()) {
            button->setToolButtonStyle(style);
        }
    } else if (type == Action) {
        d->m_actionStyle = style;
        foreach (FancyTab * button, d->m_actionButtons) {
            button->setToolButtonStyle(style);
        }
    }
}

void FancyModeBar::setButtonFont(FancyModeBar::ButtonType type, QFont &font)
{
    if (type == Mode) {
        foreach (FancyTab * button, d->m_modeMap.keys()) {
            button->setFont(font);
        }
    } else if (type == Action) {
        foreach (FancyTab * button, d->m_actionButtons) {
            button->setFont(font);
        }
    }
}

void FancyModeBar::setIconSize(QSize size)
{
    foreach (FancyTab * button, d->m_modeMap.keys()) {
        button->setIconSize(size);
    }
    foreach (FancyTab * button, d->m_actionButtons) {
        button->setIconSize(size);
    }
}

void FancyModeBar::setButtonSpace(int space)
{
    d->m_modeLayout->setSpacing(space);
}

void FancyModeBar::setActionStyle(QAction *action, Qt::ToolButtonStyle style)
{
    FancyTab *pButton = d->m_actionMap.value(action);

    if (pButton) {
        pButton->setToolButtonStyle(style);
    }
}

void FancyModeBar::addAction(QAction *action, ActionPosition position)
{
    FancyTab *pButton = new FancyTab(action->icon());

    if (m_direction == Vertical) {
        pButton->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    } else if (m_direction == Horizontal) {
        pButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    }

    pButton->setType(FancyTab::Action);
    pButton->setText(action->text());
    pButton->setToolButtonStyle(d->m_actionStyle);
    pButton->setToolTip(action->toolTip());
    pButton->setDefaultAction(action);
    d->m_actionButtons.append(pButton);
    d->m_actionMap.insert(action, pButton);
//    connect(pButton, SIGNAL(clicked(bool)), action, SLOT(trigger()));
    connect(pButton, SIGNAL(clicked(bool)), action, SIGNAL(triggered(bool)));

    if (position == AP_Front) {
        d->m_frontActionLayout->addWidget(pButton);
    } else if (position == AP_Middle) {
        d->m_middleActionLayout->addWidget(pButton);
    } else if (position == AP_Back) {
        d->m_backActionLayout->addWidget(pButton);
    }
}

void FancyModeBar::setHoverColor(const QColor &color)
{
    foreach (FancyTab * button, d->m_modeMap.keys()) {
        button->setHoverColor(color);
    }
    foreach (FancyTab * button, d->m_actionButtons) {
        button->setHoverColor(color);
    }
}

QColor FancyModeBar::hoverColor() const
{
    if (d->m_modeMap.count() > 0) {
        return d->m_modeMap.keys().at(0)->hoverColor();
    }

    if (d->m_actionButtons.count() > 0) {
        return d->m_actionButtons.at(0)->hoverColor();
    }

    return QColor();
}

void FancyModeBar::setPressColor(const QColor &color)
{
    foreach (FancyTab * button, d->m_modeMap.keys()) {
        button->setPressColor(color);
    }
    foreach (FancyTab * button, d->m_actionButtons) {
        button->setPressColor(color);
    }
}

QColor FancyModeBar::pressColor() const
{
    if (d->m_modeMap.count() > 0) {
        return d->m_modeMap.keys().at(0)->pressColor();
    }

    if (d->m_actionButtons.count() > 0) {
        return d->m_actionButtons.at(0)->pressColor();
    }

    return QColor();
}

FancyMode *FancyModeBar::currentMode() const
{
    FancyMode *mode = d->m_modes.at(d->m_modeStack->currentIndex());
    return mode;
}

FancyMode *FancyModeBar::mode(int index)
{
    if (index < 0 || index >= d->m_modes.count()) {
        return nullptr;
    }

    FancyMode *mode = d->m_modes.at(index);
    return mode;
}

QWidget *FancyModeBar::spacer() const
{
    return d->m_spacer;
}

QWidget *FancyModeBar::line() const
{
    return d->m_line;
}

void FancyModeBar::setTextColor(const QColor &color)
{
    foreach (FancyTab * button, d->m_modeMap.keys()) {
        button->setTextColor(color);
    }
    foreach (FancyTab * button, d->m_actionButtons) {
        button->setTextColor(color);
    }
}

void FancyModeBar::setSelectedTextColor(const QColor &color)
{
    foreach (FancyTab * button, d->m_modeMap.keys()) {
        button->setSelectedTextColor(color);
    }
    foreach (FancyTab * button, d->m_actionButtons) {
        button->setSelectedTextColor(color);
    }
}

