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
#include "imodebar.h"
#include "imodebar_p.h"
#include "imode.h"
#include "imodebutton.h"

#include <QAction>
#include <QBoxLayout>
#include <QStackedWidget>
#include <QScrollArea>

/********************************************************************************
 * class :
 *      IModeBar
 *
 *******************************************************************************/
IModeBarPrivate::IModeBarPrivate(QObject *parent)
    : QObject(parent)
{
    m_modeMap.clear();
    m_modes.clear();
//    m_modeButtons.clear();
    m_actionButtons.clear();
    m_actionMap.clear();

    m_modeStyle = Qt::ToolButtonTextUnderIcon;
    m_actionStyle = Qt::ToolButtonIconOnly;

    m_modeStack = 0;
    m_modeLayout = 0;
    m_frontActionLayout = 0;
    m_backActionLayout = 0;

    m_spacer = 0;
    m_line = 0;
}

IModeBarPrivate::~IModeBarPrivate()
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

void IModeBarPrivate::selectMode(IMode *mode)
{
    IModeButton *pButton = m_modeMap.key(mode);
    if(pButton){
        foreach (IModeButton *button, m_modeMap.keys()) {
            button->select(button == pButton);
        }
        m_modeStack->setCurrentWidget(mode->widget());
    }else if(m_modes.count() > 0){
        selectMode(m_modes.at(0));
    }
}

void IModeBarPrivate::setEnabled(IMode *mode, bool enable)
{
    IModeButton *pButton = m_modeMap.key(mode);
    if(pButton){
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

void IModeBarPrivate::setVisible(IMode *mode, bool visible)
{
    IModeButton *pButton = m_modeMap.key(mode);
    if(pButton){
        pButton->setVisible(visible);
        mode->widget()->setVisible(visible);
    }
}

// slots
void IModeBarPrivate::switchMode()
{
    IModeButton *pButton = qobject_cast<IModeButton *>(sender());
    if(pButton){
        foreach (IModeButton *button, m_modeMap.keys()) {
            button->select(button == pButton);
        }
        m_modeStack->setCurrentWidget(m_modeMap.value(pButton)->widget());
    }
}

/********************************************************************************
 * class :
 *      IModeBar
 *
 *******************************************************************************/
IModeBar::IModeBar(QStackedWidget *modeStack, Direction direction, QWidget *parent)
    : QWidget(parent),d(new IModeBarPrivate)
{
    d->m_modeStack = modeStack;
    m_direction = direction;

    QBoxLayout::Direction layoutDirection;
    if(direction == Horizontal){
        layoutDirection = QBoxLayout::LeftToRight;
    }else{
        layoutDirection = QBoxLayout::TopToBottom;
    }

    // mode layout
    d->m_modeLayout = new QBoxLayout(layoutDirection);
//    d->m_modeLayout->addStretch();
    d->m_modeLayout->setSpacing(0);
    d->m_modeLayout->setMargin(0);

//    QScrollArea *pScrollArea = new QScrollArea();
//    if(direction == Horizontal){
//        pScrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
//        pScrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
//        pScrollArea->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);
//    }else{
//        pScrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
//        pScrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
//        pScrollArea->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
//    }
//    pScrollArea->setStyleSheet("background: transparent;");

//    pScrollArea->setWidgetResizable(true);
//    pScrollArea->setFrameShape(QFrame::NoFrame);
//    QWidget *scrollWidget = new QWidget();
//    if(direction == Horizontal){
//        scrollWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);
//    }else{
//        scrollWidget->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
//    }
//    scrollWidget->setLayout(d->m_modeLayout);
//    pScrollArea->setWidget(scrollWidget);

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
    if(direction == Horizontal){
        d->m_spacer->setFixedWidth(5);
    }else{
        d->m_spacer->setFixedHeight(25);
    }

    d->m_line = new QWidget();
    if(direction == Horizontal){
        d->m_line->setFixedWidth(0);
    }else{
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
    if(direction == Horizontal){
        pMainLayout->setSpacing(5);
    }else{
        pMainLayout->setSpacing(0);
    }
    pMainLayout->setContentsMargins(0, 0, 0, 0);

    setLayout(pMainLayout);

    if(direction == Horizontal){
        setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    }else{
        setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
    }
    connect(d->m_modeStack, SIGNAL(currentChanged(int)), this, SIGNAL(currentChanged(int)));
}

IModeBar::~IModeBar()
{
    delete d;
}

void IModeBar::addMode(IMode *mode)
{
    IModeButton *pButton = new IModeButton(mode->icon());
    pButton->setText(mode->displayName());
    pButton->setToolButtonStyle(d->m_modeStyle);
    d->m_modeStack->addWidget(mode->widget());
    d->m_modeMap.insert(pButton, mode);
    d->m_modes.append(mode);
    connect(pButton, SIGNAL(clicked()), d, SLOT(switchMode()));
    int index = d->m_modeStack->count();
    if(index == 0) index++;
    if(m_direction == Vertical){
        pButton->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
        pButton->setShortcut(tr("Ctrl+%1").arg(index));
        pButton->setToolTip(tr("Switch to <b>%1</b> mode\nCtrl+%2").arg(pButton->text()).arg(index));
    }else if(m_direction == Horizontal){
        pButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
        pButton->setShortcut(tr("Alt+Ctrl+%1").arg(index));
        pButton->setToolTip(tr("Switch to <b>%1</b> mode\nAlt+Ctrl+%2").arg(pButton->text()).arg(index));
    }
    d->m_modeLayout->insertWidget(index-1, pButton);
}

void IModeBar::selectMode(IMode *mode)
{
    d->selectMode(mode);
}

void IModeBar::setEnabled(IMode *mode, bool enable)
{
    d->setEnabled(mode, enable);
}

void IModeBar::setVisible(IMode *mode, bool visible)
{
    d->setVisible(mode,visible);
    IMode *currentMode = d->m_modes.at(d->m_modeStack->currentIndex());
    if(!visible){
        if(mode == currentMode){
            for(int i = 0; i < d->m_modeStack->count(); i++){
                IMode *tmpMode = d->m_modes.at(i);
                IModeButton *button = d->m_modeMap.key(tmpMode);
                if(button && !button->isHidden()){
                    selectMode(tmpMode);
                }
            }
        }
    }else{
        if(mode != currentMode){
            mode->widget()->setVisible(false);
        }
    }
}

void IModeBar::setButtonStyle(IModeBar::ButtonType type, Qt::ToolButtonStyle style)
{
    if(type == Mode){
        d->m_modeStyle = style;
        foreach (IModeButton *button, d->m_modeMap.keys()) {
            button->setToolButtonStyle(style);
        }
    }else if(type == Action){
        d->m_actionStyle = style;
        foreach (IModeButton *button, d->m_actionButtons) {
            button->setToolButtonStyle(style);
        }
    }
}

void IModeBar::setButtonFont(IModeBar::ButtonType type, QFont &font)
{
    if(type == Mode){
        foreach (IModeButton *button, d->m_modeMap.keys()) {
            button->setFont(font);
        }
    }else if(type == Action){
        foreach (IModeButton *button, d->m_actionButtons) {
            button->setFont(font);
        }
    }
}

void IModeBar::setIconSize(QSize size)
{
    foreach (IModeButton *button, d->m_modeMap.keys()) {
        button->setIconSize(size);
    }
    foreach (IModeButton *button, d->m_actionButtons) {
        button->setIconSize(size);
    }
}

void IModeBar::setButtonSpace(int space)
{
    d->m_modeLayout->setSpacing(space);
}

void IModeBar::setActionStyle(QAction *action, Qt::ToolButtonStyle style)
{
    IModeButton *pButton = d->m_actionMap.value(action);
    if(pButton){
        pButton->setToolButtonStyle(style);
    }
}

void IModeBar::addAction(QAction *action, ActionPosition position)
{
    IModeButton *pButton = new IModeButton(action->icon());
    if(m_direction == Vertical){
        pButton->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    }else if(m_direction == Horizontal){
        pButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    }
    pButton->setType(IModeButton::Action);
    pButton->setText(action->text());
    pButton->setToolButtonStyle(d->m_actionStyle);
    pButton->setToolTip(action->toolTip());
    pButton->setDefaultAction(action);
    d->m_actionButtons.append(pButton);
    d->m_actionMap.insert(action, pButton);
//    connect(pButton, SIGNAL(clicked(bool)), action, SLOT(trigger()));
    connect(pButton, SIGNAL(clicked(bool)), action, SIGNAL(triggered(bool)));
    if(position == AP_Front){
        d->m_frontActionLayout->addWidget(pButton);
    }else if(position == AP_Middle){
        d->m_middleActionLayout->addWidget(pButton);
    }else if(position == AP_Back){
        d->m_backActionLayout->addWidget(pButton);
    }
}

void IModeBar::setHoverColor(const QColor &color)
{
    foreach (IModeButton *button, d->m_modeMap.keys()) {
        button->setHoverColor(color);
    }
    foreach (IModeButton *button, d->m_actionButtons) {
        button->setHoverColor(color);
    }
}

QColor IModeBar::hoverColor() const
{
    if(d->m_modeMap.count() > 0){
        return d->m_modeMap.keys().at(0)->hoverColor();
    }
    if(d->m_actionButtons.count() > 0){
        return d->m_actionButtons.at(0)->hoverColor();
    }
    return QColor();
}

void IModeBar::setPressColor(const QColor &color)
{
    foreach (IModeButton *button, d->m_modeMap.keys()) {
        button->setPressColor(color);
    }
    foreach (IModeButton *button, d->m_actionButtons) {
        button->setPressColor(color);
    }
}

QColor IModeBar::pressColor() const
{
    if(d->m_modeMap.count() > 0){
        return d->m_modeMap.keys().at(0)->pressColor();
    }
    if(d->m_actionButtons.count() > 0){
        return d->m_actionButtons.at(0)->pressColor();
    }
    return QColor();
}

IMode* IModeBar::currentMode() const
{
    IMode *mode = d->m_modes.at(d->m_modeStack->currentIndex());
    return mode;
}

IMode* IModeBar::mode(int index)
{
    if(index < 0 || index >= d->m_modes.count())
        return NULL;
    IMode *mode = d->m_modes.at(index);
    return mode;
}

QWidget *IModeBar::spacer() const
{
    return d->m_spacer;
}

QWidget *IModeBar::line() const
{
    return d->m_line;
}

void IModeBar::setTextColor(const QColor &color)
{
    foreach (IModeButton *button, d->m_modeMap.keys()) {
        button->setTextColor(color);
    }
    foreach (IModeButton *button, d->m_actionButtons) {
        button->setTextColor(color);
    }
}

void IModeBar::setSelectedTextColor(const QColor &color)
{
    foreach (IModeButton *button, d->m_modeMap.keys()) {
        button->setSelectedTextColor(color);
    }
    foreach (IModeButton *button, d->m_actionButtons) {
        button->setSelectedTextColor(color);
    }
}

