/***************************************************************************
 **
 **  Copyright (C) 2018-2021 MaMinJie <canpool@163.com>
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
#include "fancybar.h"
#include "fancybutton.h"
#include "fancyquickaccessbar.h"
#include "fancycursor.h"
#include "fancyscreen.h"
#include "fancytitlebar.h"
#include "fancyapplicationwidget.h"
#include "qcanpool.h"

#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QMenuBar>
#include <QMouseEvent>
#include <QEvent>
#include <QMainWindow>
#include <QApplication>

QCANPOOL_BEGIN_NAMESPACE

#define TITLE_BAR_HEIGHT    25
#define MENU_BAR_HEIGHT     25

static QColor s_backgroundColor = QColor(0, 100, 200, 100);

class FancyBarPrivate : public QWidget
{
    Q_OBJECT
public:
    explicit FancyBarPrivate(QWidget *parent = nullptr);
    ~FancyBarPrivate();

    void showQuickAccessBar(bool show);
    void showMenuBar(bool show);
    void showMenuWidget(bool show);

    void addAdditionalControl(QAction *action, FancyBar::AdditionalControlPosition position);
    void addAdditionalControl(QWidget *widget, FancyBar::AdditionalControlPosition position);

    void init();
    void setFancyStyle(FancyBar::FancyStyle style);
    void createTitleWidget();
    void createMenuWidget();
    void updateMenuColor();

    void registerWidget(QWidget *widget);

protected:
    QSize sizeHint() const;

private slots:
    void aboutToShowMenu();
    void aboutToHideMenu();
    void menuTriggered(QMouseEvent *e);
    void slotWindowIconChange(QIcon &icon);

public:
    FancyBar            *q;

    QHBoxLayout         *m_logoLayout;
    // title widget
    QWidget             *m_titleWidget;
    FancyTitleBar       *m_titleBar;
    QToolButton         *m_logoButton;
    FancyQuickAccessBar *m_quickAccessBar;
    QSpacerItem         *m_leftSpacerItem;
    QSpacerItem         *m_rightSpacerItem;
    QLabel              *m_titleLabel;
    QHBoxLayout         *m_titleAdditionalControlArea;
    QWidget             *m_systemGroup;

    // menu widget
    QWidget             *m_menuWidget;
    FancyButton         *m_applicationButton;
    QHBoxLayout         *m_appButtonLayout;
    QMenuBar            *m_menuBar;
    QHBoxLayout         *m_menuAdditionalControlArea;
    QHBoxLayout         *m_menuBarArea;
    QSpacerItem         *m_middleSpacerItem;

    QList<FancyButton *> m_additionalButtons;
    FancyApplicationWidget *m_applicationWidget;

    FancyButton         *m_menuButton;

    FancyBar::FancyStyle m_style;

    QColor  m_menuTextColor;
    QColor  m_menuHoverColor;
    QColor  m_menuPressColor;

    int     m_titleBarHeight;

    bool    m_bQuickAccessVisible;
    bool    m_bMenuBarVisible;
    bool    m_bMenuWidgetVisible;
};

FancyBarPrivate::FancyBarPrivate(QWidget *parent)
    : QWidget(parent)
    , m_menuBar(nullptr)
    , m_menuButton(nullptr)
    , m_style(FancyBar::WindowStyle)
    , m_menuTextColor(QColor(250, 250, 250))
    , m_menuHoverColor(QColor(255, 255, 255, 50))
    , m_menuPressColor(QColor(255, 255, 255, 50))
    , m_titleBarHeight(TITLE_BAR_HEIGHT)
    , m_bQuickAccessVisible(false)
    , m_bMenuBarVisible(false)
    , m_bMenuWidgetVisible(false)
{
}

FancyBarPrivate::~FancyBarPrivate()
{}

void FancyBarPrivate::showQuickAccessBar(bool show)
{
    if (m_bQuickAccessVisible == show) {
        return;
    }
    m_bQuickAccessVisible = show;
    if (show) {
        m_quickAccessBar->show();
        m_leftSpacerItem->changeSize(0, 0, QSizePolicy::Expanding, QSizePolicy::Minimum);
    } else {
        m_quickAccessBar->hide();
        m_leftSpacerItem->changeSize(0, 0, QSizePolicy::Minimum, QSizePolicy::Minimum);
    }
}

void FancyBarPrivate::showMenuBar(bool show)
{
    if (m_bMenuBarVisible == show) {
        return;
    }
    m_bMenuBarVisible = show;
    if (m_menuBar) {
        m_menuBar->setVisible(show);
    }

    if (m_style == FancyBar::WindowStyle ||
            m_style == FancyBar::ClassicStyle) {
        showMenuWidget(show);
    }
}

void FancyBarPrivate::showMenuWidget(bool show)
{
    if (m_bMenuWidgetVisible == show) {
        return;
    }
    m_bMenuWidgetVisible = show;
    m_menuWidget->setVisible(show);
}

void FancyBarPrivate::addAdditionalControl(QAction *action, FancyBar::AdditionalControlPosition position)
{
    FancyButton *button = new FancyButton();
    button->setText(action->text());
    button->setToolTip(action->toolTip());
    button->setIcon(action->icon());
    button->setDefaultAction(action);
    connect(button, SIGNAL(menuTriggered(QMouseEvent *)), this, SLOT(menuTriggered(QMouseEvent *)));
    connect(button, SIGNAL(clicked(bool)), action, SIGNAL(triggered(bool)));

    if (position == FancyBar::TitlePosition) {
        m_titleAdditionalControlArea->addWidget(button);
    } else if (position == FancyBar::MenuPosition) {
        m_menuAdditionalControlArea->addWidget(button);
    }

    m_additionalButtons.append(button);
}

void FancyBarPrivate::addAdditionalControl(QWidget *widget, FancyBar::AdditionalControlPosition position)
{
    if (position == FancyBar::TitlePosition) {
        m_titleAdditionalControlArea->addWidget(widget);
    } else if (position == FancyBar::MenuPosition) {
        m_menuAdditionalControlArea->addWidget(widget);
    }
}

void FancyBarPrivate::init()
{
    m_logoLayout = new QHBoxLayout();
    m_logoLayout->setContentsMargins(0, 0, 0, 0);
    m_logoLayout->setSpacing(0);
    createTitleWidget();
    createMenuWidget();
    QVBoxLayout *rightLayout = new QVBoxLayout();
    rightLayout->setMargin(0);
    rightLayout->setSpacing(0);
    rightLayout->addWidget(m_titleWidget);
    rightLayout->addWidget(m_menuWidget);
    QHBoxLayout *layout = new QHBoxLayout();
    layout->setContentsMargins(1, 1, 1, 0);
    layout->setSpacing(0);
    layout->addLayout(m_logoLayout);
    layout->addLayout(rightLayout);
    setLayout(layout);

    QVBoxLayout *qlayout = new QVBoxLayout();
    qlayout->setMargin(0);
    qlayout->setSpacing(0);
    qlayout->addWidget(this);
    q->setLayout(qlayout);
}

void FancyBarPrivate::setFancyStyle(FancyBar::FancyStyle style)
{
    if (m_style == style) {
        return;
    }
    m_style = style;
    // clear layout
    m_logoLayout->removeItem(m_appButtonLayout);
    QHBoxLayout *titleLayout = qobject_cast<QHBoxLayout *>(m_titleWidget->layout());
    titleLayout->removeWidget(m_logoButton);
    titleLayout->removeWidget(m_quickAccessBar);
    titleLayout->removeItem(m_appButtonLayout);
    titleLayout->removeItem(m_menuBarArea);
    titleLayout->removeItem(m_leftSpacerItem);
    titleLayout->removeWidget(m_titleLabel);
    titleLayout->removeItem(m_rightSpacerItem);
    titleLayout->removeItem(m_titleAdditionalControlArea);
    titleLayout->removeItem(m_menuAdditionalControlArea);
    titleLayout->removeWidget(m_systemGroup);
    QHBoxLayout *menuLayout = qobject_cast<QHBoxLayout *>(m_menuWidget->layout());
    menuLayout->removeItem(m_appButtonLayout);
    menuLayout->removeItem(m_menuBarArea);
    menuLayout->removeItem(m_middleSpacerItem);
    menuLayout->removeItem(m_menuAdditionalControlArea);
    m_applicationButton->setToolButtonStyle(Qt::ToolButtonTextOnly);
    m_applicationButton->setIconSize(QSize(18, 18));
    m_applicationButton->setRound(false);
    m_appButtonLayout->setContentsMargins(0, 0, 0, 0);
    m_logoButton->setVisible(false);
    showMenuWidget(true);

    // set style
    if (style == FancyBar::WindowStyle) {
        m_logoButton->setVisible(true);
        titleLayout->addWidget(m_logoButton/*,0,Qt::AlignVCenter*/);
        titleLayout->addWidget(m_quickAccessBar);
        titleLayout->addItem(m_leftSpacerItem);
        titleLayout->addWidget(m_titleLabel);
        titleLayout->addItem(m_rightSpacerItem);
        titleLayout->addItem(m_titleAdditionalControlArea);
        titleLayout->addWidget(m_systemGroup);
        m_appButtonLayout->setContentsMargins(0, 2, 0, 2);
        menuLayout->addLayout(m_appButtonLayout);
        menuLayout->addLayout(m_menuBarArea);
        menuLayout->addItem(m_middleSpacerItem);
        menuLayout->addLayout(m_menuAdditionalControlArea);
    } else if (style == FancyBar::DialogStyle) {
        m_logoButton->setVisible(true);
        titleLayout->addWidget(m_logoButton/*,0,Qt::AlignVCenter*/);
        titleLayout->addWidget(m_titleLabel);
        titleLayout->addItem(m_rightSpacerItem);
        titleLayout->addLayout(m_titleAdditionalControlArea);
        titleLayout->addWidget(m_systemGroup);
        showMenuWidget(false);
    } else if (style == FancyBar::ClassicStyle) {
        m_applicationButton->setToolButtonStyle(Qt::ToolButtonIconOnly);
        m_applicationButton->setIconSize(QSize(48, 48));
        m_applicationButton->setRound(true);
        m_logoLayout->addLayout(m_appButtonLayout);
        titleLayout->addWidget(m_quickAccessBar);
        titleLayout->addItem(m_leftSpacerItem);
        titleLayout->addWidget(m_titleLabel);
        titleLayout->addItem(m_rightSpacerItem);
        titleLayout->addLayout(m_titleAdditionalControlArea);
        titleLayout->addWidget(m_systemGroup);
        menuLayout->addLayout(m_menuBarArea);
        menuLayout->addItem(m_middleSpacerItem);
        menuLayout->addLayout(m_menuAdditionalControlArea);
    } else if (style == FancyBar::MergedStyle) {
        m_applicationButton->setToolButtonStyle(Qt::ToolButtonIconOnly);
        titleLayout->addLayout(m_appButtonLayout);
        titleLayout->addWidget(m_quickAccessBar);
        titleLayout->addLayout(m_menuBarArea);
        titleLayout->addItem(m_leftSpacerItem);
        titleLayout->addWidget(m_titleLabel);
        titleLayout->addItem(m_rightSpacerItem);
        titleLayout->addLayout(m_menuAdditionalControlArea);
        titleLayout->addLayout(m_titleAdditionalControlArea);
        titleLayout->addWidget(m_systemGroup);
        showMenuWidget(false);
    }
}

void FancyBarPrivate::createTitleWidget()
{
    m_titleWidget = new QWidget();
    m_titleWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

    m_leftSpacerItem = new QSpacerItem(0, 0, QSizePolicy::Minimum, QSizePolicy::Minimum);
    m_rightSpacerItem = new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Minimum);

    m_quickAccessBar = new FancyQuickAccessBar();
    m_quickAccessBar->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    m_quickAccessBar->hide();

    m_titleAdditionalControlArea = new QHBoxLayout();
    m_titleAdditionalControlArea->setMargin(0);
    m_titleAdditionalControlArea->setSpacing(0);

    m_logoButton = m_titleBar->logoButton();
    m_titleLabel = m_titleBar->titleLabel();
    m_systemGroup = m_titleBar->systemGroup();

    QHBoxLayout *layout = new QHBoxLayout();
    layout->setContentsMargins(1, 0, 0, 0);
    layout->setSpacing(0);
    layout->addWidget(m_logoButton);
    layout->addWidget(m_quickAccessBar);
    layout->addItem(m_leftSpacerItem);
    layout->addWidget(m_titleLabel);
    layout->addItem(m_rightSpacerItem);
    layout->addLayout(m_titleAdditionalControlArea);
    layout->addWidget(m_systemGroup);
    m_titleWidget->setLayout(layout);
}

void FancyBarPrivate::createMenuWidget()
{
    m_menuWidget = new QWidget();
    m_menuWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    m_menuWidget->hide();

    m_applicationWidget = new FancyApplicationWidget(this);
    m_applicationButton = qobject_cast<FancyButton *>(m_applicationWidget->appButton());
    m_applicationButton->setText(tr("Application"));
    m_applicationButton->setNormalColor(QColor(240, 130, 0));

    m_menuBarArea = new QHBoxLayout();
    m_menuBarArea->setMargin(0);
    m_menuBarArea->setSpacing(0);

    m_middleSpacerItem = new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Minimum);

    m_menuAdditionalControlArea = new QHBoxLayout();
    m_menuAdditionalControlArea->setMargin(0);
    m_menuAdditionalControlArea->setSpacing(0);

    m_appButtonLayout = new QHBoxLayout();
    m_appButtonLayout->setContentsMargins(0, 2, 0, 2);
    m_appButtonLayout->setSpacing(0);
    m_appButtonLayout->addWidget(m_applicationButton);

    QHBoxLayout *layout = new QHBoxLayout();
    layout->setContentsMargins(1, 0, 0, 0);
    layout->setSpacing(0);
    layout->addLayout(m_appButtonLayout);
    layout->addLayout(m_menuBarArea);
    layout->addItem(m_middleSpacerItem);
    layout->addLayout(m_menuAdditionalControlArea);
    m_menuWidget->setLayout(layout);
}

void FancyBarPrivate::updateMenuColor()
{
    if (m_menuBar == nullptr) {
        return;
    }
    m_menuBar->setStyleSheet(QString(
                                 "QMenuBar{background-color: transparent;margin:1px;color: #%1;}"
                                 "QMenuBar::item{background-color: transparent;}"
                                 "QMenuBar::item:disabled{color: gray;}"
                                 "QMenuBar::item:pressed{background:#%2;}"
                                 "QMenuBar::item:hover{background:#%3;}"
                             )
                             .arg(QCanpool::colorToArgb(m_menuTextColor))
                             .arg(QCanpool::colorToArgb(m_menuPressColor))
                             .arg(QCanpool::colorToArgb(m_menuHoverColor)));
}

void FancyBarPrivate::registerWidget(QWidget *widget)
{
    m_titleBar = new FancyTitleBar(widget);
    m_titleBar->setTitleWidget(q);

    connect(m_titleBar, SIGNAL(windowResizable(bool)), q, SIGNAL(maximizationChanged(bool)));
    connect(m_titleBar, SIGNAL(windowIconChanged(QIcon &)), this, SLOT(slotWindowIconChange(QIcon &)));
}

QSize FancyBarPrivate::sizeHint() const
{
    int ht = 0;
    int hm = 0;
    if (m_menuBar && m_bMenuBarVisible) {
        hm = qMax(m_menuBar->sizeHint().height(), MENU_BAR_HEIGHT);
        if (m_bMenuWidgetVisible) {
            m_menuWidget->setFixedHeight(hm);
            ht = m_titleBarHeight;
        } else {
            ht = qMax(hm, m_titleBarHeight);
            hm = 0;
        }
    } else {
        ht = m_titleBarHeight;
    }
    m_titleWidget->setFixedHeight(ht);

    return QSize(width(), ht + hm);
}

void FancyBarPrivate::aboutToShowMenu()
{
    if (m_menuButton) {
        m_menuButton->select(true);
    }
}

void FancyBarPrivate::aboutToHideMenu()
{
    if (m_menuButton) {
        m_menuButton->select(false);
    }
}

void FancyBarPrivate::menuTriggered(QMouseEvent *e)
{
    FancyButton *button = qobject_cast<FancyButton *>(sender());
    if (button == nullptr) {
        return;
    }
    QAction *action = button->defaultAction();
    QMenu *menu = action->menu();
    if (menu == nullptr) {
        return;
    }
    connect(menu, SIGNAL(aboutToShow()), this, SLOT(aboutToShowMenu()));
    connect(menu, SIGNAL(aboutToHide()), this, SLOT(aboutToHideMenu()));
    m_menuButton = button;
    int x = e->x();
    int y = e->y();
    QPoint pos = e->globalPos();
    pos.setX(pos.x() - x);
    pos.setY(pos.y() - y + button->height());
    menu->popup(pos);
}

void FancyBarPrivate::slotWindowIconChange(QIcon &icon)
{
    if (m_applicationButton) {
        m_applicationButton->setIcon(icon);
    }
}

FancyBar::FancyBar(QWidget *parent)
    : QWidget(parent), d(new FancyBarPrivate(this))
{
    Q_ASSERT(parent);

    d->q = this;
    d->registerWidget(parent);
    d->init();

    if (s_backgroundColor.isValid()) {
        setBackgroundColor(s_backgroundColor);
    }
}

FancyBar::~FancyBar()
{
}

FancyTitleBar *FancyBar::titleBar() const
{
    return d->m_titleBar;
}

QMenuBar *FancyBar::menuBar() const
{
    if (d->m_menuBar == nullptr) {
        d->m_menuBar = new QMenuBar();
        d->m_menuBar->setNativeMenuBar(false);
        d->m_menuBar->setVisible(true);
        d->m_menuBar->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
        d->updateMenuColor();
        d->m_menuBarArea->addWidget(d->m_menuBar);
    }

    return d->m_menuBar;
}

void FancyBar::showMenuBar(bool show)
{
    d->showMenuBar(show);
}

bool FancyBar::isMenuBarVisible() const
{
    return d->m_bMenuBarVisible;
}

FancyQuickAccessBar *FancyBar::quickAccessBar() const
{
    return d->m_quickAccessBar;
}

void FancyBar::showQuickAccess(bool show)
{
    d->showQuickAccessBar(show);
}

bool FancyBar::isQuickAccessVisible() const
{
    return d->m_bQuickAccessVisible;
}

void FancyBar::setHoverColor(const QColor &color)
{
    foreach (FancyButton * button, d->m_additionalButtons) {
        button->setHoverColor(color);
    }
    d->m_menuHoverColor = color;
    d->updateMenuColor();
    d->m_quickAccessBar->setHoverColor(color);
}

void FancyBar::setPressColor(const QColor &color)
{
    foreach (FancyButton * button, d->m_additionalButtons) {
        button->setPressColor(color);
    }
    d->m_menuPressColor = color;
    d->updateMenuColor();
    d->m_quickAccessBar->setPressColor(color);
}

void FancyBar::setTextColor(const QColor &color)
{
    foreach (FancyButton * button, d->m_additionalButtons) {
        button->setTextColor(color);
    }
    d->m_titleLabel->setStyleSheet(QString("color:#%1;").arg(QCanpool::colorToArgb(color)));
    d->m_menuTextColor = color;
    d->updateMenuColor();
    d->m_quickAccessBar->setTextColor(color);
}

void FancyBar::setBackgroundColor(const QColor &color)
{
    QPalette palette;
    palette.setColor(QPalette::Background, color);
    setPalette(palette);
    setAutoFillBackground(true);
    s_backgroundColor = color;
}

void FancyBar::addAdditionalControl(QAction *action, FancyBar::AdditionalControlPosition position)
{
    d->addAdditionalControl(action, position);
}

void FancyBar::addAdditionalControl(QWidget *widget, FancyBar::AdditionalControlPosition position)
{
    d->addAdditionalControl(widget, position);
}

void FancyBar::setApplicationWidget(const QString &label, QWidget *widget)
{
    d->m_applicationButton->setText(label);
    d->m_applicationWidget->setAppWidget(widget);
}

void FancyBar::setApplicationButtonBkColor(const QColor &color)
{
    d->m_applicationButton->setNormalColor(color);
}

void FancyBar::setWidgetResizable(bool resizable)
{
    d->m_titleBar->setWidgetResizable(resizable);
}

void FancyBar::setTitleBarHeight(int height)
{
    if (height > TITLE_BAR_HEIGHT && height <= TITLE_BAR_HEIGHT + 15) {
        d->m_titleBarHeight = height;
    }
}

void FancyBar::setFancyStyle(FancyBar::FancyStyle style)
{
    d->setFancyStyle(style);
}

void FancyBar::updateWidgetFlags()
{
    d->m_titleBar->updateWidgetFlags();
}

QCANPOOL_END_NAMESPACE

#include "fancybar.moc"
