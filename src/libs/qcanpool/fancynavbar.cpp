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
#include "fancynavbar.h"
#include "fancynavbutton.h"

#include <QStackedWidget>
#include <QHBoxLayout>
#include <QToolButton>
#include <QAction>
#include <QMenu>
#include <QToolBar>
#include <QLabel>
#include <QSplitter>

class FancyNavBarPrivate : public QObject
{
    Q_OBJECT
public:
    explicit FancyNavBarPrivate(QObject *parent = 0);
    ~FancyNavBarPrivate();
    void init();

    void toggle(FancyNavButton *button, bool checked);
    void updateToolTips();

private:
    void disconnectToggle(FancyNavButton *button);
    void connectToggle(FancyNavButton *button);

    void updateMaximizeButton();

signals:

public slots:
    void slotNavToggled(bool checked);
    void slotMenuItemToggled(bool checked);
    void slotButtonClicked();
    void slotSideExpand(bool expand);
    void slotToggleMaximized();
    void slotClosePanel();

public:
    FancyNavBar *q;
    QStackedWidget *m_stack;
    QStackedWidget *m_opToolBarWidgets;
    QMenu *m_menu;
    QToolBar *m_toolbar;
    QWidget *m_panel;

    QMap<FancyNavButton*, QWidget*> m_navMap;
    QList<QWidget*> m_widgets;
    QMap<QAction*, FancyNavButton*> m_toggleNavMap;
    FancyNavButton *m_pLastButton;

    FancyNavButton *m_pSideExpandButton;
    FancyNavButton *m_pMenuButton;

    QLabel *m_titleLabel;
    QAction *m_minMaxAction;
    QAction *m_closeAction;

    QSplitter *m_splitter;
    int m_nonMaximizedSize = 0;
    bool m_isMaximized = false;
};

FancyNavBarPrivate::FancyNavBarPrivate(QObject *parent)
    : QObject(parent)
{
    m_stack = 0;
    m_opToolBarWidgets = 0;
    m_menu = 0;
    m_toolbar = 0;
    m_panel = 0;

    m_navMap.clear();
    m_widgets.clear();
    m_toggleNavMap.clear();
    m_pLastButton = 0;
    m_pSideExpandButton = 0;
    m_pMenuButton = 0;

    m_titleLabel = 0;
    m_minMaxAction = 0;
    m_closeAction = 0;

    m_splitter = 0;
}

FancyNavBarPrivate::~FancyNavBarPrivate()
{

}

void FancyNavBarPrivate::init()
{
    m_titleLabel = new QLabel();
    QFont font = m_titleLabel->font();
    font.setPointSize(10);
    m_titleLabel->setFont(font);
    m_titleLabel->setMinimumWidth(80);

    m_minMaxAction = new QAction(tr("Maximize"), this);
    updateMaximizeButton();
    connect(m_minMaxAction, SIGNAL(triggered(bool)), this, SLOT(slotToggleMaximized()));
    m_closeAction = new QAction(QIcon(":/tools/close"), tr("Close"), this);
    connect(m_closeAction, SIGNAL(triggered(bool)), this, SLOT(slotClosePanel()));

    m_toolbar = new QToolBar();
    m_toolbar->setIconSize(QSize(18,18));
    m_toolbar->setToolButtonStyle(Qt::ToolButtonIconOnly);
    m_toolbar->setStyleSheet("margin:0px;");

    m_stack = new QStackedWidget();
    m_opToolBarWidgets = new QStackedWidget();

    QWidget *spacer1 = new QWidget();
    spacer1->setFixedWidth(10);
    spacer1->setFixedHeight(1);

    QWidget *spacer2 = new QWidget();
    spacer2->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

    m_toolbar->addWidget(m_titleLabel);
    m_toolbar->addWidget(spacer1);
    m_toolbar->addSeparator();
    m_toolbar->addWidget(m_opToolBarWidgets);
    m_toolbar->addWidget(spacer2);
    m_toolbar->addAction(m_minMaxAction);
    m_toolbar->addAction(m_closeAction);

    m_panel = new QWidget();
    m_panel->hide();
    QVBoxLayout *panelLayout = new QVBoxLayout();
    panelLayout->setSpacing(0);
    panelLayout->setContentsMargins(0,0,0,0);
    panelLayout->addWidget(m_toolbar);
    panelLayout->addWidget(m_stack);
    m_panel->setLayout(panelLayout);
    QPalette palette;
    palette.setColor(QPalette::Background, QColor(240,240,240,240));
    m_panel->setPalette(palette);
    m_panel->setAutoFillBackground(true);

    // nav
    m_menu = new QMenu(q);

    m_pSideExpandButton = new FancyNavButton();
    m_pSideExpandButton->setToolButtonStyle(Qt::ToolButtonIconOnly);
    m_pSideExpandButton->setCheckable(true);
    m_pSideExpandButton->setIcon(QIcon(":/tools/sidebar"));
    m_pSideExpandButton->setIconSize(QSize(16,16));
    m_pSideExpandButton->setShortcut(QString("Alt+0"));
    m_pSideExpandButton->setToolTip(tr("<b>Show Sidebar</b> Alt+0"));
    connect(m_pSideExpandButton, SIGNAL(toggled(bool)), q, SIGNAL(sigSideExpand(bool)));
    connect(m_pSideExpandButton, SIGNAL(toggled(bool)), this, SLOT(slotSideExpand(bool)));

    m_pMenuButton = new FancyNavButton();
    m_pMenuButton->setType(FancyNavButton::Action);
    m_pMenuButton->setIcon(QIcon(":/tools/navmenu"));
    m_pMenuButton->setToolButtonStyle(Qt::ToolButtonIconOnly);
    m_pMenuButton->setIconSize(QSize(16,16));
    connect(m_pMenuButton, SIGNAL(clicked(bool)), this, SLOT(slotButtonClicked()));

    QHBoxLayout *layout = new QHBoxLayout();
    layout->setSpacing(2);
    layout->setContentsMargins(0,0,0,0);
    q->setLayout(layout);

    layout->addWidget(m_pSideExpandButton);
    layout->addWidget(m_pMenuButton);
}

void FancyNavBarPrivate::toggle(FancyNavButton *button, bool checked)
{
    if(m_pLastButton && checked && m_pLastButton != button){
        disconnectToggle(m_pLastButton);
        m_pLastButton->setChecked(false);
        m_pLastButton->select(false);
        connectToggle(m_pLastButton);
    }
    m_pLastButton = button;

#if (QT_VERSION < QT_VERSION_CHECK(5,0,0))
    m_panel->setShown(checked);
#else
    m_panel->setHidden(!checked);
#endif
    QWidget *widget = m_navMap.value(button);
    if(widget){
        QString tag = button->text().split(" ").at(0);
        QString text = button->text().remove(0, tag.length());
        m_titleLabel->setText(text);
        int index = m_stack->indexOf(widget);
        m_stack->setCurrentWidget(widget);
        m_opToolBarWidgets->setCurrentIndex(index);
    }
}

void FancyNavBarPrivate::updateToolTips()
{
    for(int i = 0; i < m_widgets.count(); i++){
        FancyNavButton *button = m_navMap.key(m_widgets.at(i));
        button->setToolTip(tr("<b>%1</b>\nAlt+%2").arg(button->text().split(" ").at(1)).arg(i+1));
    }
}

void FancyNavBarPrivate::disconnectToggle(FancyNavButton *button)
{
    disconnect(button, SIGNAL(toggled(bool)), this, SLOT(slotNavToggled(bool)));
}

void FancyNavBarPrivate::connectToggle(FancyNavButton *button)
{
    connect(button, SIGNAL(toggled(bool)), this, SLOT(slotNavToggled(bool)));
}

void FancyNavBarPrivate::updateMaximizeButton()
{
    if(m_isMaximized){
        m_minMaxAction->setIcon(QIcon(":/tools/arrow_down"));
        m_minMaxAction->setText(tr("Minimize"));
    }else{
        m_minMaxAction->setIcon(QIcon(":/tools/arrow_up"));
        m_minMaxAction->setText(tr("Maximize"));
    }
}

void FancyNavBarPrivate::slotNavToggled(bool checked)
{
    FancyNavButton *button = qobject_cast<FancyNavButton *>(sender());
    toggle(button, checked);
    button->select(checked);
}

void FancyNavBarPrivate::slotMenuItemToggled(bool checked)
{
    QAction *toggleAction = qobject_cast<QAction *>(sender());
    FancyNavButton *button = m_toggleNavMap.value(toggleAction, 0);
    if(button){
        button->setVisible(checked);
    }
}

void FancyNavBarPrivate::slotButtonClicked()
{
    FancyNavButton *button = qobject_cast<FancyNavButton *>(sender());
    if(button == m_pMenuButton){
        m_menu->exec(QCursor::pos());
    }
}

void FancyNavBarPrivate::slotSideExpand(bool expand)
{
    if(expand){
        m_pSideExpandButton->setToolTip(tr("<b>Hide Sidebar</b> Alt+0"));
    }else{
        m_pSideExpandButton->setToolTip(tr("<b>Show Sidebar</b> Alt+0"));
    }
    m_pSideExpandButton->select(expand);
}

void FancyNavBarPrivate::slotToggleMaximized()
{
    if(!m_splitter){
        return;
    }
    int idx = m_splitter->indexOf(m_panel);
    if (idx < 0)
        return;
    m_isMaximized = !m_isMaximized;
    updateMaximizeButton();

    QList<int> sizes = m_splitter->sizes();

    if (m_isMaximized) {
        m_nonMaximizedSize = sizes[idx];
        int sum = 0;
        foreach (int s, sizes)
            sum += s;
        for (int i = 0; i < sizes.count(); ++i) {
            sizes[i] = 32;
        }
        sizes[idx] = sum - (sizes.count()-1) * 32;
    } else {
        int target = m_nonMaximizedSize > 0 ? m_nonMaximizedSize : m_panel->sizeHint().height();
        int space = sizes[idx] - target;
        if (space > 0) {
            for (int i = 0; i < sizes.count(); ++i) {
                sizes[i] += space / (sizes.count()-1);
            }
            sizes[idx] = target;
        }
    }

    m_splitter->setSizes(sizes);
}

void FancyNavBarPrivate::slotClosePanel()
{
    disconnectToggle(m_pLastButton);
    m_pLastButton->setChecked(false);
    m_pLastButton->select(false);
    connectToggle(m_pLastButton);
    toggle(m_pLastButton, false);
}

FancyNavBar::FancyNavBar(QWidget *parent)
    : QWidget(parent),d(new FancyNavBarPrivate(this))
{
    d->q = this;
    d->init();
}

void FancyNavBar::add(const QString &title, QWidget *widget, QWidget *appendix)
{
    d->m_stack->addWidget(widget);
    if(appendix){
        d->m_opToolBarWidgets->addWidget(appendix);
    }else{
        d->m_opToolBarWidgets->addWidget(new QWidget());
    }
    int index = d->m_stack->count();

    FancyNavButton *button = new FancyNavButton();
    button->setText(QString("%1 %2").arg(index).arg(title));
    button->setCheckable(true);
    button->setShortcut(tr("Alt+%1").arg(index));
    button->setToolTip(tr("<b>%1</b>\nAlt+%2").arg(title).arg(index));
    connect(button, SIGNAL(toggled(bool)), d, SLOT(slotNavToggled(bool)));
    d->m_navMap.insert(button, widget);
    d->m_widgets.append(widget);

    QAction *toggleAction = new QAction(tr("Toggle %1").arg(title), this);
    toggleAction->setCheckable(true);
    toggleAction->setChecked(true);
    connect(toggleAction, SIGNAL(toggled(bool)), d, SLOT(slotMenuItemToggled(bool)));

    d->m_toggleNavMap.insert(toggleAction, button);
    d->m_menu->addAction(toggleAction);

    layout()->removeWidget(d->m_pMenuButton);
    layout()->addWidget(button);
    layout()->addWidget(d->m_pMenuButton);
}

void FancyNavBar::remove(QWidget *widget)
{
    FancyNavButton *button = d->m_navMap.key(widget);
    if(!button) return;
    if(button == d->m_pLastButton){
        d->slotClosePanel();
        d->m_pLastButton = NULL;
    }
    int index = d->m_widgets.indexOf(widget);
    d->m_widgets.removeAll(widget);
    QAction *action = d->m_toggleNavMap.key(button);
    d->m_stack->removeWidget(widget);
    d->m_toggleNavMap.remove(action);
    d->m_menu->removeAction(action);
    d->m_navMap.remove(button);
    layout()->removeWidget(button);
    d->m_opToolBarWidgets->removeWidget(d->m_opToolBarWidgets->widget(index));
    delete button;
    delete action;
    d->updateToolTips();
}

void FancyNavBar::select(QWidget *widget)
{
    FancyNavButton *button = d->m_navMap.key(widget);
    if(button){
        button->setChecked(true);
        d->toggle(button, true);
    }
}

QWidget *FancyNavBar::panel() const
{
    return d->m_panel;
}

void FancyNavBar::setPanelColor(const QColor &color)
{
    QPalette palette;
    palette.setColor(QPalette::Background, color);
    d->m_panel->setPalette(palette);
    d->m_panel->setAutoFillBackground(true);
}

void FancyNavBar::setSplitter(QSplitter *splitter)
{
    d->m_splitter = splitter;
}

void FancyNavBar::setSideButtonVisable(bool visable)
{
    d->m_pSideExpandButton->setVisible(visable);
}

void FancyNavBar::setSideButtonEnable(bool enable)
{
    d->m_pSideExpandButton->setEnabled(enable);
}

void FancyNavBar::setSideExpand(bool expand)
{
    d->m_pSideExpandButton->setChecked(expand);
    d->m_pSideExpandButton->select(expand);
}

void FancyNavBar::setHoverColor(const QColor &color)
{
    foreach (FancyNavButton *button, d->m_navMap.keys()) {
        button->setHoverColor(color);
    }
}

void FancyNavBar::setPressColor(const QColor &color)
{
    foreach (FancyNavButton *button, d->m_navMap.keys()) {
        button->setPressColor(color);
    }
}

void FancyNavBar::setTextColor(const QColor &color)
{
    foreach (FancyNavButton *button, d->m_navMap.keys()) {
        button->setTextColor(color);
    }
}

#include "fancynavbar.moc"
