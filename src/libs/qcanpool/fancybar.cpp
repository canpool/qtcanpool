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
#include "fancybar.h"
#include "fancybutton.h"
#include "quickaccessbar.h"
#include "cursor.h"
#include "screenhelper.h"
#include "qcanpool.h"

#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QMenuBar>
#include <QMouseEvent>
#include <QEvent>
#include <QDesktopWidget>
#include <QMainWindow>

#define TITLE_BAR_HEIGHT    25
#define MENU_BAR_HEIGHT     25

static QColor s_backgroundColor = QColor(0, 100, 200, 100);

class FancyBarPrivate : public QWidget
{
    Q_OBJECT
public:
    explicit FancyBarPrivate(QWidget *parent = 0);
    ~FancyBarPrivate();

    void showQuickAccessBar(bool show);
    void showMenuBar(bool show);

    void setWidgetResizable(bool resizable);
    void setWidgetMovable(bool movable);
    void setWidgetMaximizable(bool maximizable);

    void addAdditionalControl(QAction *action, FancyBar::AdditionalControlPosition position);
    void addAdditionalControl(QWidget *widget, FancyBar::AdditionalControlPosition position);

    void init();
    void setFancyStyle(FancyBar::FancyStyle style);
    void createTitleWidget();
    void createMenuWidget();
    void createWindowButtons();
    void updateMenuColor();

public:
    void registerWidget(QWidget *widget);

private:
    void restoreWidget(QWidget *pWidget);
    void maximizeWidget(QWidget *pWidget);

public:
    // window
    bool windowTitleChange(QObject *obj);
    bool windowIconChange(QObject *obj);
    void windowSizeChange(QObject *obj);
    void windowStateChange(QObject *obj);

    // mouse event
    void handleWidgetMouseEvent(QObject *obj, QEvent *event);
    // 更新鼠标样式
    void updateCursorShape(const QPoint &gMousePos);
    // 重置窗体大小
    void resizeWidget(const QPoint &gMousePos);
    // 移动窗体
    void moveWidget(const QPoint &gMousePos);
    // 处理鼠标按下
    void handleMousePressEvent(QMouseEvent *event);
    // 处理鼠标释放
    void handleMouseReleaseEvent(QMouseEvent *event);
    // 处理鼠标移动
    void handleMouseMoveEvent(QMouseEvent *event);
    // 处理鼠标离开
    void handleLeaveEvent(QEvent *event);
    // 处理鼠标进入
    void handleHoverMoveEvent(QHoverEvent *event);

    QPoint calcDragPoint(QWidget *pWindow, QMouseEvent *event) const;

protected:
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseDoubleClickEvent(QMouseEvent *event);

signals:
    void windowResizable(bool resizable);
    void windowMovable(bool movable);

public:
    FancyBar *q;

    QHBoxLayout     *m_logoLayout;
    // title widget
    QWidget         *m_titleWidget;
    FancyButton     *m_logoButton;
    QuickAccessBar  *m_quickAccessBar;
    QSpacerItem     *m_leftSpacerItem;
    QSpacerItem     *m_rightSpacerItem;
    QLabel          *m_titleLabel;
    QHBoxLayout     *m_titleAdditionalControlArea;
    FancyButton     *m_maximizeButton;
    FancyButton     *m_minimizeButton;
    FancyButton     *m_closeButton;
    QWidget         *m_systemGroup;
    bool m_bQuickAccessVisible;

    // menu widget
    QWidget         *m_menuWidget;
    FancyButton     *m_applicationButton;
    QHBoxLayout     *m_appButtonLayout;
    QMenuBar        *m_menuBar;
    QHBoxLayout     *m_menuAdditionalControlArea;
    QHBoxLayout     *m_menuBarArea;
    QSpacerItem     *m_middleSpacerItem;
    bool m_bMenuBarVisible;

    // screen
    int m_currentScreen;

    // main window
    QWidget *m_mainWidget;
    QPoint m_dragPos;
    Cursor m_pressCursor;
    Cursor m_moveCursor;
    bool m_bEdgePressed;
    bool m_bCursorShapeChanged;
    Qt::WindowFlags m_windowFlags;
    bool m_bWidgetResizable;
    bool m_bWidgetMovable;

    ///
    QRect m_normalRect;
    QPoint m_movePoint;
    QPoint m_dragPoint;
    bool m_bLeftButtonPressed;

    bool m_isMaximized;
    bool m_isMinimized;
    bool m_bWidgetMaximizable;

    FancyButton *m_menuButton;
    QMenu *m_menu;
    QAction *m_maximizeAction;
    QAction *m_minimizeAction;
    QAction *m_restoreAction;
    QAction *m_closeAction;

    QList<FancyButton*> m_additionalButtons;
    QWidget *m_applicationWidget;

    QColor m_menuHoverColor;
    QColor m_menuTextColor;
    QColor m_menuPressColor;

    // added on 2018-3-11 19:31:09
    int m_titleBarHeight;
    int m_menuBarHeight;

    // added on 2018-4-30 13:39:32
    FancyBar::FancyStyle m_style;

private slots:
    void slotWindowButtonClicked();
    void aboutToShowSystemMenu();
    void aboutToHideSystemMenu();
    void systemMenuTriggered(QMouseEvent* e);
    void aboutToShowMenu();
    void aboutToHideMenu();
    void menuTriggered(QMouseEvent* e);
    void applicationMenuTriggered(QMouseEvent* e);
};

FancyBarPrivate::FancyBarPrivate(QWidget *parent)
    : QWidget(parent)
{
    m_isMaximized = false;
    m_isMinimized = false;
    m_bLeftButtonPressed = false;

    m_currentScreen = 0;

    m_bQuickAccessVisible = false;
    m_bMenuBarVisible = false;

    m_bEdgePressed = false;
    m_bCursorShapeChanged = false;
    m_bWidgetMovable = true;
    m_bWidgetResizable = true;
    m_bWidgetMaximizable = true;

    q = 0;
    m_logoLayout = 0;
    m_titleWidget = 0;
    m_logoButton = 0;
    m_quickAccessBar = 0;
    m_leftSpacerItem = 0;
    m_titleLabel = 0;
    m_titleAdditionalControlArea = 0;
    m_maximizeButton = 0;
    m_minimizeButton = 0;
    m_closeButton = 0;
    m_systemGroup = 0;

    m_menuWidget = 0;
    m_applicationButton = 0;
    m_appButtonLayout = 0;
    m_menuBar = 0;
    m_menuAdditionalControlArea = 0;
    m_menuBarArea = 0;
    m_middleSpacerItem = 0;

    m_mainWidget = 0;

    m_menuButton = 0;
    m_additionalButtons.clear();
    m_applicationWidget = 0;

    m_menuTextColor = QColor(250,250,250);
    m_menuHoverColor = QColor(255,255,255,50);
    m_menuPressColor = QColor(255,255,255,50);

    m_titleBarHeight = TITLE_BAR_HEIGHT;
    m_menuBarHeight = MENU_BAR_HEIGHT;

    m_style = FancyBar::WindowStyle;

    QRect geom = QApplication::desktop()->availableGeometry();
    m_normalRect = QRect(100,100, 2*geom.width()/3, 2*geom.height()/3);
}

FancyBarPrivate::~FancyBarPrivate()
{

}

void FancyBarPrivate::showQuickAccessBar(bool show)
{
    if(m_bQuickAccessVisible == show)
        return;
    m_bQuickAccessVisible = show;
    if(show){
        m_quickAccessBar->show();
        m_leftSpacerItem->changeSize(0,0,QSizePolicy::Expanding, QSizePolicy::Minimum);
    }else{
        m_quickAccessBar->hide();
        m_leftSpacerItem->changeSize(0,0,QSizePolicy::Minimum, QSizePolicy::Minimum);
    }
}

void FancyBarPrivate::showMenuBar(bool show)
{
    if(m_bMenuBarVisible == show)
        return;
    m_bMenuBarVisible = show;
    m_menuWidget->setVisible(show);
    if(show){
        setFixedHeight(m_titleBarHeight + m_menuBarHeight);
    }else{
        setFixedHeight(m_titleBarHeight);
    }
}

void FancyBarPrivate::setWidgetResizable(bool resizable)
{
    m_bWidgetResizable = resizable;
}

void FancyBarPrivate::setWidgetMovable(bool movable)
{
    m_bWidgetMovable = movable;
}

void FancyBarPrivate::setWidgetMaximizable(bool maximizable)
{
    m_bWidgetMaximizable = maximizable;
}

void FancyBarPrivate::addAdditionalControl(QAction *action, FancyBar::AdditionalControlPosition position)
{
    FancyButton *button = new FancyButton();
    button->setText(action->text());
    button->setToolTip(action->toolTip());
    button->setIcon(action->icon());
    button->setDefaultAction(action);
    connect(button, SIGNAL(menuTriggered(QMouseEvent*)), this, SLOT(menuTriggered(QMouseEvent*)));
    connect(button, SIGNAL(clicked(bool)), action, SIGNAL(triggered(bool)));
    if(position == FancyBar::TitlePosition){
        m_titleAdditionalControlArea->addWidget(button);
    }else if(position == FancyBar::MenuPosition){
        m_menuAdditionalControlArea->addWidget(button);
    }
    m_additionalButtons.append(button);
}

void FancyBarPrivate::addAdditionalControl(QWidget *widget, FancyBar::AdditionalControlPosition position)
{
    if(position == FancyBar::TitlePosition){
        m_titleAdditionalControlArea->addWidget(widget);
    }else if(position == FancyBar::MenuPosition){
        m_menuAdditionalControlArea->addWidget(widget);
    }
}

void FancyBarPrivate::init()
{
    m_logoLayout = new QHBoxLayout();
    m_logoLayout->setContentsMargins(0,0,0,0);
    m_logoLayout->setSpacing(0);

    createTitleWidget();
    createMenuWidget();

    QVBoxLayout *rightLayout = new QVBoxLayout();
    rightLayout->setMargin(0);
    rightLayout->setSpacing(0);
    rightLayout->addWidget(m_titleWidget);
    rightLayout->addWidget(m_menuWidget);

    QHBoxLayout *layout = new QHBoxLayout();
    layout->setContentsMargins(1,1,1,0);
    layout->setSpacing(0);
    layout->addLayout(m_logoLayout);
    layout->addLayout(rightLayout);
    setLayout(layout);
}

void FancyBarPrivate::setFancyStyle(FancyBar::FancyStyle style)
{
    if(m_style == style)
        return;
    m_style = style;

    // clear layout
    m_logoLayout->removeItem(m_appButtonLayout);

    QHBoxLayout *titleLayout = qobject_cast<QHBoxLayout*>(m_titleWidget->layout());
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

    QHBoxLayout *menuLayout = qobject_cast<QHBoxLayout*>(m_menuWidget->layout());
    menuLayout->removeItem(m_appButtonLayout);
    menuLayout->removeItem(m_menuBarArea);
    menuLayout->removeItem(m_middleSpacerItem);
    menuLayout->removeItem(m_menuAdditionalControlArea);

    m_applicationButton->setToolButtonStyle(Qt::ToolButtonTextOnly);
    m_applicationButton->setIconSize(QSize(18,18));
    m_applicationButton->setRound(false);
    m_appButtonLayout->setContentsMargins(0, 0, 0, 0);
    m_logoButton->setVisible(false);
    showMenuBar(true);

    // set style
    if(style == FancyBar::WindowStyle){
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
    }else if(style == FancyBar::DialogStyle){
        m_logoButton->setVisible(true);

        titleLayout->addWidget(m_logoButton/*,0,Qt::AlignVCenter*/);
        titleLayout->addWidget(m_titleLabel);
        titleLayout->addItem(m_rightSpacerItem);
        titleLayout->addLayout(m_titleAdditionalControlArea);
        titleLayout->addWidget(m_systemGroup);
        showMenuBar(false);
    }else if(style == FancyBar::ClassicStyle){
        m_applicationButton->setToolButtonStyle(Qt::ToolButtonIconOnly);
        m_applicationButton->setIconSize(QSize(48,48));
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
    }else if(style == FancyBar::MergedStyle){
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

        showMenuBar(false);
    }

}

void FancyBarPrivate::createTitleWidget()
{
    m_titleWidget = new QWidget();
    m_titleWidget->setFixedHeight(m_titleBarHeight);

    createWindowButtons();
    m_leftSpacerItem = new QSpacerItem(0, 0, QSizePolicy::Minimum, QSizePolicy::Minimum);
    m_rightSpacerItem = new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Minimum);

    m_quickAccessBar = new QuickAccessBar();
    m_quickAccessBar->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    m_quickAccessBar->hide();

    m_titleAdditionalControlArea = new QHBoxLayout();
    m_titleAdditionalControlArea->setMargin(0);
    m_titleAdditionalControlArea->setSpacing(0);

    m_systemGroup = new QWidget();
    QHBoxLayout *systemButtonLayout = new QHBoxLayout();
    systemButtonLayout->setMargin(0);
    systemButtonLayout->setSpacing(0);
    systemButtonLayout->addWidget(m_minimizeButton/*, 0, Qt::AlignTop*/);
    systemButtonLayout->addWidget(m_maximizeButton/*, 0, Qt::AlignTop*/);
    systemButtonLayout->addWidget(m_closeButton/*, 0, Qt::AlignTop*/);
    m_systemGroup->setLayout(systemButtonLayout);

    QHBoxLayout *layout = new QHBoxLayout();
    layout->setContentsMargins(1,0,0,0);
    layout->setSpacing(0);
    layout->addWidget(m_logoButton/*,0,Qt::AlignVCenter*/);
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
    m_menuWidget->setFixedHeight(m_menuBarHeight);
    m_menuWidget->hide();

    m_applicationButton = new FancyButton();
    m_applicationButton->setHasMenu(true);
    m_applicationButton->setText(tr("Application"));
    m_applicationButton->setToolButtonStyle(Qt::ToolButtonTextOnly);
    m_applicationButton->setNormalColor(QColor(240,130,0));
    connect(m_applicationButton, SIGNAL(menuTriggered(QMouseEvent*)), this, SLOT(applicationMenuTriggered(QMouseEvent*)));

    m_applicationWidget = new QWidget(this);
    QVBoxLayout *applicationLayout = new QVBoxLayout();
    applicationLayout->setMargin(0);
    applicationLayout->setSpacing(0);
    m_applicationWidget->setLayout(applicationLayout);
    m_applicationWidget->setWindowFlags(Qt::Popup);

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
    layout->setContentsMargins(1,0,0,0);
    layout->setSpacing(0);
    layout->addLayout(m_appButtonLayout);
    layout->addLayout(m_menuBarArea);
    layout->addItem(m_middleSpacerItem);
    layout->addLayout(m_menuAdditionalControlArea);
    m_menuWidget->setLayout(layout);
}

void FancyBarPrivate::createWindowButtons()
{
    m_logoButton = new FancyButton();
    m_logoButton->setHasMenu(true);
    m_logoButton->setIcon(QIcon(":/main/logo"));
    connect(m_logoButton, SIGNAL(menuTriggered(QMouseEvent*)), this, SLOT(systemMenuTriggered(QMouseEvent*)));

    m_titleLabel = new QLabel();
    QFont font("Microsoft Yahei", 10);
    m_titleLabel->setFont(font);
    m_titleLabel->setAlignment(Qt::AlignCenter);
    m_titleLabel->setStyleSheet("color:white;");

    m_minimizeButton = new FancyButton();
    m_minimizeButton->setIcon(QIcon(":/main/min"));
    m_minimizeButton->setToolTip(tr("minimize"));
//    m_minimizeButton->setIconSize(QSize(22,22));
    connect(m_minimizeButton, SIGNAL(clicked(bool)), this, SLOT(slotWindowButtonClicked()));

    m_maximizeButton = new FancyButton();
    m_maximizeButton->setIcon(QIcon(":/main/max"));
    m_maximizeButton->setToolTip(tr("maximize"));
//    m_maximizeButton->setIconSize(QSize(22,22));
    connect(m_maximizeButton, SIGNAL(clicked(bool)), this, SLOT(slotWindowButtonClicked()));

    m_closeButton = new FancyButton();
    m_closeButton->setIcon(QIcon(":/main/close"));
    m_closeButton->setToolTip(tr("close"));
    m_closeButton->setIconSize(QSize(22,22));
    connect(m_closeButton, SIGNAL(clicked(bool)), this, SLOT(slotWindowButtonClicked()));
    m_closeButton->setHoverColor(QColor(207, 0, 0, 230));
    m_closeButton->setPressColor(QColor(207, 0, 0, 150));

    m_menu = new QMenu(this);
    connect(m_menu, SIGNAL(aboutToShow()), this, SLOT(aboutToShowSystemMenu()));
    connect(m_menu, SIGNAL(aboutToHide()), this, SLOT(aboutToHideSystemMenu()));
    m_maximizeAction = new QAction(QIcon(":/main/max2"), tr("maximize"), this);
    m_minimizeAction = new QAction(QIcon(":/main/min2"), tr("minimize"), this);
    m_restoreAction = new QAction(QIcon(":/main/restore2"), tr("restore"), this);
    m_closeAction = new QAction(QIcon(":/main/close2"), tr("close"), this);
    m_closeAction->setShortcut(QString("Alt+F4"));
    connect(m_maximizeAction, SIGNAL(triggered(bool)), m_maximizeButton, SIGNAL(clicked(bool)));
    connect(m_minimizeAction, SIGNAL(triggered(bool)), m_minimizeButton, SIGNAL(clicked(bool)));
    connect(m_restoreAction, SIGNAL(triggered(bool)), m_maximizeButton, SIGNAL(clicked(bool)));
    connect(m_closeAction, SIGNAL(triggered(bool)), m_closeButton, SIGNAL(clicked(bool)));
    m_menu->addAction(m_restoreAction);
    m_menu->addAction(m_minimizeAction);
    m_menu->addAction(m_maximizeAction);
    m_menu->addSeparator();
    m_menu->addAction(m_closeAction);
}

void FancyBarPrivate::updateMenuColor()
{
    if(m_menuBar == 0) return;
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
    m_mainWidget = widget;
    m_windowFlags = m_mainWidget->windowFlags();
    m_mainWidget->setMouseTracking(true);
    m_mainWidget->setAttribute(Qt::WA_Hover, true);
}

void FancyBarPrivate::restoreWidget(QWidget *pWidget)
{
    m_isMaximized = false;

    pWidget->setGeometry(m_normalRect);

    emit windowResizable(true);
}

void FancyBarPrivate::maximizeWidget(QWidget *pWidget)
{
    // 在窗体大小改变前更新值,因为事件过滤响应随机
    m_isMaximized = true;

    // 计算最大化所在屏幕
    int x = pWidget->frameGeometry().x() + pWidget->frameGeometry().width()/2;
    ScreenHelper sreen;
    m_currentScreen = sreen.currentScreen(x);

    // 创建QDesktopWidget对象
    QDesktopWidget * desktopWidget = QApplication::desktop();
    QRect rect = desktopWidget->availableGeometry(m_currentScreen);
    pWidget->setGeometry(rect);

    emit windowResizable(false);
}

bool FancyBarPrivate::windowTitleChange(QObject *obj)
{
    QWidget *pWidget = qobject_cast<QWidget *>(obj);
    if (pWidget)
    {
        QFont font = QApplication::font();
        font.setPointSize(11);
        m_titleLabel->setText(pWidget->windowTitle());
        m_titleLabel->setFont(font);
        return true;
    }
    return false;
}

bool FancyBarPrivate::windowIconChange(QObject *obj)
{
    QWidget *pWidget = qobject_cast<QWidget *>(obj);
    if (pWidget)
    {
        QIcon icon = pWidget->windowIcon();
//        m_pIconLabel->setPixmap(icon.pixmap(m_pIconLabel->size()).scaled(22,22));
        if(!icon.isNull()){
            m_logoButton->setIcon(icon);
            m_applicationButton->setIcon(icon);
        }
        return true;
    }
    return false;
}

void FancyBarPrivate::windowSizeChange(QObject *obj)
{
    QWidget *pWindow = qobject_cast<QWidget *>(obj);

    if ( pWindow )
    {
        bool bMaximize = pWindow->isMaximized();
        if (bMaximize) {
            // 消除无边框最大化遮挡windows任务栏
            pWindow->setWindowState(pWindow->windowState() & ~Qt::WindowMaximized);
            this->maximizeWidget(pWindow);
        }
        windowStateChange(obj);
//        m_maximizeButton->setStyle(QApplication::style());
    }
}

void FancyBarPrivate::windowStateChange(QObject *obj)
{
    Q_UNUSED(obj);
    if(m_isMaximized){
        m_maximizeButton->setToolTip(tr("restore"));
        m_maximizeButton->setProperty("maximizeProperty", "restore");
        m_maximizeButton->setIcon(QIcon(":/main/restore"));
    }else{
        m_maximizeButton->setProperty("maximizeProperty", "maximize");
        m_maximizeButton->setToolTip(tr("maximize"));
        m_maximizeButton->setIcon(QIcon(":/main/max"));
    }
    m_maximizeAction->setEnabled(!m_isMaximized);
    m_restoreAction->setEnabled(m_isMaximized);
}

void FancyBarPrivate::handleWidgetMouseEvent(QObject *obj, QEvent *event)
{
    Q_UNUSED(obj);
//    m_mainWidget = qobject_cast<QWidget *>(obj);
    switch (event->type())
    {
    default:
        break;
    case QEvent::MouseButtonPress:
        handleMousePressEvent(static_cast<QMouseEvent*>(event));
        break;
    case QEvent::MouseButtonRelease:
        handleMouseReleaseEvent(static_cast<QMouseEvent*>(event));
        break;
    case QEvent::MouseMove:
        handleMouseMoveEvent(static_cast<QMouseEvent*>(event));
        break;
    case QEvent::Leave:
        handleLeaveEvent(static_cast<QMouseEvent*>(event));
        break;
    case QEvent::HoverMove:
        handleHoverMoveEvent(static_cast<QHoverEvent*>(event));
        break;
    }
}

void FancyBarPrivate::updateCursorShape(const QPoint &gMousePos)
{
    if (m_mainWidget->isFullScreen() || m_mainWidget->isMaximized() || m_isMaximized)
    {
        if (m_bCursorShapeChanged)
        {
            m_mainWidget->unsetCursor();
        }
        return;
    }
    m_moveCursor.recalculate(gMousePos, m_mainWidget->frameGeometry());

    if(m_moveCursor.m_bOnTopLeftEdge || m_moveCursor.m_bOnBottomRightEdge)
    {
        m_mainWidget->setCursor( Qt::SizeFDiagCursor );
        m_bCursorShapeChanged = true;
    }
    else if(m_moveCursor.m_bOnTopRightEdge || m_moveCursor.m_bOnBottomLeftEdge)
    {
        m_mainWidget->setCursor( Qt::SizeBDiagCursor );
        m_bCursorShapeChanged = true;
    }
    else if(m_moveCursor.m_bOnLeftEdge || m_moveCursor.m_bOnRightEdge)
    {
        m_mainWidget->setCursor( Qt::SizeHorCursor );
        m_bCursorShapeChanged = true;
    }
    else if(m_moveCursor.m_bOnTopEdge || m_moveCursor.m_bOnBottomEdge)
    {
        m_mainWidget->setCursor( Qt::SizeVerCursor );
        m_bCursorShapeChanged = true;
    }
    else
    {
        if (m_bCursorShapeChanged)
        {
            m_mainWidget->unsetCursor();
            m_bCursorShapeChanged = false;
        }
    }
}

void FancyBarPrivate::resizeWidget(const QPoint &gMousePos)
{
    QRect origRect;

    origRect = m_mainWidget->frameGeometry();

    int left = origRect.left();
    int top = origRect.top();
    int right = origRect.right();
    int bottom = origRect.bottom();
    origRect.getCoords(&left, &top, &right, &bottom);

    int minWidth = m_mainWidget->minimumWidth();
    int minHeight = m_mainWidget->minimumHeight();

    if (m_pressCursor.m_bOnTopLeftEdge)
    {
        left = gMousePos.x();
        top = gMousePos.y();
    }
    else if (m_pressCursor.m_bOnBottomLeftEdge)
    {
        left = gMousePos.x();
        bottom = gMousePos.y();
    }
    else if (m_pressCursor.m_bOnTopRightEdge)
    {
        right = gMousePos.x();
        top = gMousePos.y();
    }
    else if (m_pressCursor.m_bOnBottomRightEdge)
    {
        right = gMousePos.x();
        bottom = gMousePos.y();
    }
    else if (m_pressCursor.m_bOnLeftEdge)
    {
        left = gMousePos.x();
    }
    else if (m_pressCursor.m_bOnRightEdge)
    {
        right = gMousePos.x();
    }
    else if (m_pressCursor.m_bOnTopEdge)
    {
        top = gMousePos.y();
    }
    else if (m_pressCursor.m_bOnBottomEdge)
    {
        bottom = gMousePos.y();
    }

    QRect newRect(QPoint(left, top), QPoint(right, bottom));

    if (newRect.isValid())
    {
        if (minWidth > newRect.width())
        {
            if (left != origRect.left())
                newRect.setLeft(origRect.left());
            else
                newRect.setRight(origRect.right());
        }
        if (minHeight > newRect.height())
        {
            if (top != origRect.top())
                newRect.setTop(origRect.top());
            else
                newRect.setBottom(origRect.bottom());
        }
        m_mainWidget->setGeometry(newRect);
    }
}

void FancyBarPrivate::moveWidget(const QPoint &gMousePos)
{
    m_mainWidget->move(gMousePos - m_dragPos);
}

void FancyBarPrivate::handleMousePressEvent(QMouseEvent *event)
{
    if(event->button() == Qt::LeftButton && !m_isMaximized){ // add !m_isMaximized 2017-7-27 22:52:39
        m_bEdgePressed = true;
        QRect frameRect = m_mainWidget->frameGeometry();
        m_pressCursor.recalculate(event->globalPos(), frameRect);

        m_dragPos = event->globalPos() - frameRect.topLeft();
    }
}

void FancyBarPrivate::handleMouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
    {
        m_bEdgePressed = false;
        m_pressCursor.reset();
        if (m_bWidgetResizable)
        {
            updateCursorShape(event->globalPos());
        }
    }
}

void FancyBarPrivate::handleMouseMoveEvent(QMouseEvent *event)
{
    if (m_bEdgePressed)
    {
        if (m_bWidgetResizable && m_pressCursor.m_bOnEdges)
        {
            resizeWidget(event->globalPos());
        }
//        else if (d->m_bWidgetMovable && m_bLeftButtonTitlePressed)
//        {
//            moveWidget(event->globalPos());
//        }
    }
    else if (m_bWidgetResizable)
    {
        updateCursorShape(event->globalPos());
    }
}

void FancyBarPrivate::handleLeaveEvent(QEvent *event)
{
    Q_UNUSED(event);
    if(!m_bEdgePressed)
    {
        m_mainWidget->unsetCursor();
    }
}

void FancyBarPrivate::handleHoverMoveEvent(QHoverEvent *event)
{
    if (!m_bEdgePressed && m_bWidgetResizable)
    {
        updateCursorShape(m_mainWidget->mapToGlobal(event->pos()));
    }
}

QPoint FancyBarPrivate::calcDragPoint(QWidget *pWindow, QMouseEvent *event) const
{
    // 最大化时，计算拖拽界面显示的坐标点
    int mouseX = event->globalX();
    //int maxWidth = QApplication::desktop()->availableGeometry().width();
    ScreenHelper screen;
    QRect rect = screen.screenRect(m_currentScreen);
    int maxWidth = rect.x() + rect.width();
    int screenX = rect.x();
    int orgWidth = m_normalRect.width();
    if(orgWidth == 0){ // 初始最大化显示时,orgWidth记录为0
        orgWidth = pWindow->minimumWidth();
    }
    QPoint point;
    point.setY(0);
    if(mouseX - screenX < orgWidth/2){
        point.setX(screenX);
    }else if(maxWidth - mouseX < orgWidth/2){
        point.setX(maxWidth - orgWidth);
    }else{
        point.setX(mouseX - orgWidth/2);
    }

    return point;
}

void FancyBarPrivate::mousePressEvent(QMouseEvent *event)
{
    if(event->button() == Qt::LeftButton)
    {
        m_bLeftButtonPressed = true;
//        QWidget *pWindow = this->window();
        QWidget *pWindow = m_mainWidget;
        if (pWindow->isTopLevel()){
            if(m_isMaximized){
                m_dragPoint = calcDragPoint(pWindow,event);
                m_movePoint = event->globalPos() - m_dragPoint;
            }else{
                m_movePoint = event->globalPos() - pWindow->pos();
            }
        }
    }
    event->ignore();
}

void FancyBarPrivate::mouseReleaseEvent(QMouseEvent *event)
{
    m_bLeftButtonPressed = false;

    // maximize on the top of the screen
    if(!m_isMaximized){
        if(event->globalY() == 0){
            m_mainWidget->move(m_mainWidget->frameGeometry().x(), 0);
            if(m_bWidgetMaximizable){
                emit m_maximizeButton->click();
            }
        }else{
            int y = m_mainWidget->frameGeometry().y();
            if(y < 0){
                m_mainWidget->move(m_mainWidget->frameGeometry().x(), 0);
            }
        }
    }

    event->ignore();
}

void FancyBarPrivate::mouseMoveEvent(QMouseEvent *event)
{
    if(m_bLeftButtonPressed)
    {
//        QWidget *pWindow = this->window();
        QWidget *pWindow = m_mainWidget;

        if (pWindow->isTopLevel()){
            if(m_bWidgetMaximizable && m_isMaximized){
//                restoreWidget(pWindow);
                // modified on 2017-7-27 22:54:15 , drag on miximization
                if(event->globalY() > 2*m_moveCursor.m_nBorderWidth){
                    m_dragPoint = calcDragPoint(pWindow,event);
                    m_movePoint = event->globalPos() - m_dragPoint;
                    restoreWidget(pWindow);
                }
            }else{
                if (m_bWidgetResizable && m_pressCursor.m_bOnEdges){
                    event->ignore();
                }else{
                    pWindow->move(event->globalPos() - m_movePoint);
                }
            }
        }
    }
}

void FancyBarPrivate::mouseDoubleClickEvent(QMouseEvent *event)
{
    if( event->button() == Qt::LeftButton ){
        if(m_bWidgetMaximizable){
            emit m_maximizeButton->click();
        }
    }
}

void FancyBarPrivate::slotWindowButtonClicked()
{
    FancyButton *pButton = qobject_cast<FancyButton *>(sender());
    //QWidget *pWindow = this->parentWidget()->parentWidget();
    QWidget *pWindow = m_mainWidget;

    if (pWindow){
        if (pButton == m_minimizeButton){
            pWindow->showMinimized();
            m_isMinimized = true;
        }else if (pButton == m_maximizeButton){
            if(m_isMaximized){
                restoreWidget(pWindow);
            }else{
                m_normalRect = pWindow->frameGeometry();
                maximizeWidget(pWindow);
            }
            m_isMinimized = false;
        }else if (pButton == m_closeButton) {
            pWindow->close();
        }
    }
}

void FancyBarPrivate::aboutToShowSystemMenu()
{
    m_logoButton->select(true);
}

void FancyBarPrivate::aboutToHideSystemMenu()
{
    m_logoButton->select(false);
}

void FancyBarPrivate::systemMenuTriggered(QMouseEvent *e)
{
    FancyButton *button = qobject_cast<FancyButton*>(sender());
    if(button == 0) return;

    int x = e->x();
    int y = e->y();
    QPoint pos = e->globalPos();
    pos.setX(pos.x()-x);
    pos.setY(pos.y()-y+button->height());
    m_menu->popup(pos);
}

void FancyBarPrivate::aboutToShowMenu()
{
    if(m_menuButton){
        m_menuButton->select(true);
    }
}

void FancyBarPrivate::aboutToHideMenu()
{
    if(m_menuButton){
        m_menuButton->select(false);
    }
}

void FancyBarPrivate::menuTriggered(QMouseEvent *e)
{
    FancyButton *button = qobject_cast<FancyButton*>(sender());
    if(button == 0) return;
    QAction *action = button->defaultAction();
    QMenu *menu = action->menu();
    if(menu == 0) return;

    connect(menu, SIGNAL(aboutToShow()), this, SLOT(aboutToShowMenu()));
    connect(menu, SIGNAL(aboutToHide()), this, SLOT(aboutToHideMenu()));
    m_menuButton = button;

    int x = e->x();
    int y = e->y();
    QPoint pos = e->globalPos();
    pos.setX(pos.x()-x);
    pos.setY(pos.y()-y+button->height());
    menu->popup(pos);
}

void FancyBarPrivate::applicationMenuTriggered(QMouseEvent *e)
{
    if(m_applicationWidget == 0) return;
    if(m_applicationWidget->layout()->itemAt(0) == NULL) return;
    FancyButton *button = qobject_cast<FancyButton*>(sender());
    if(button == 0) return;
    int x = e->x();
    int y = e->y();
    QPoint pos = e->globalPos();
    pos.setX(pos.x()-x);
    pos.setY(pos.y()-y+button->height());
    m_applicationWidget->move(pos);
    m_applicationWidget->setWindowModality(Qt::WindowModal);
    m_applicationWidget->show();
    m_applicationButton->select(false);
}

FancyBar::FancyBar(QWidget *parent, Qt::WindowFlags f)
    : QWidget(parent),d(new FancyBarPrivate(this))
{
    d->q = this;
    d->init();

    if(parent){
        d->registerWidget(parent);
        parent->installEventFilter(this);
    }

    QVBoxLayout *layout = new QVBoxLayout();
    layout->setMargin(0);
    layout->setSpacing(0);
    layout->addWidget(d);
    setLayout(layout);

    //
    if(f){
        d->m_maximizeButton->setHidden(!(f & Qt::WindowMaximizeButtonHint));
        d->m_minimizeButton->setHidden(!(f & Qt::WindowMinimizeButtonHint));
        d->setWidgetMaximizable(f & Qt::WindowMaximizeButtonHint);
//        d->m_logoButton->setEnabled(false);
        d->m_logoButton->setHasMenu(false);
    }
    if(s_backgroundColor.isValid()){
        setBackgroundColor(s_backgroundColor);
    }

    connect(d, SIGNAL(windowResizable(bool)), this, SIGNAL(maximizationChanged(bool)));
}

FancyBar::~FancyBar()
{

}

QMenuBar *FancyBar::menuBar() const
{
    if(d->m_menuBar == 0){
        d->m_menuBar = new QMenuBar();
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

QuickAccessBar *FancyBar::quickAccessBar() const
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
    foreach (FancyButton *button, d->m_additionalButtons) {
        button->setHoverColor(color);
    }
    d->m_menuHoverColor = color;
    d->updateMenuColor();
    d->m_quickAccessBar->setHoverColor(color);
}

void FancyBar::setPressColor(const QColor &color)
{
    foreach (FancyButton *button, d->m_additionalButtons) {
        button->setPressColor(color);
    }
    d->m_menuPressColor = color;
    d->updateMenuColor();
    d->m_quickAccessBar->setPressColor(color);
}

void FancyBar::setTextColor(const QColor &color)
{
    foreach (FancyButton *button, d->m_additionalButtons) {
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
    d->m_applicationWidget->layout()->addWidget(widget);
//    d->m_applicationWidget = widget;
//    if(widget){
//        widget->setWindowFlags(Qt::Popup);
//    }
}

void FancyBar::setApplicationButtonBkColor(const QColor &color)
{
    d->m_applicationButton->setNormalColor(color);
}

void FancyBar::setWidgetResizable(bool resizable)
{
    d->setWidgetResizable(resizable);
}

void FancyBar::setTitleBarHeight(int height)
{
    if(height > TITLE_BAR_HEIGHT && height <= TITLE_BAR_HEIGHT+15){
        d->m_titleBarHeight = height;
        d->m_titleWidget->setFixedHeight(d->m_titleBarHeight);
        if(isMenuBarVisible()){
            d->setFixedHeight(d->m_titleBarHeight + d->m_menuBarHeight);
        }
    }
}

void FancyBar::setFancyStyle(FancyBar::FancyStyle style)
{
    d->setFancyStyle(style);
}

bool FancyBar::eventFilter(QObject *object, QEvent *event)
{
    switch (event->type())
    {
    case QEvent::WindowTitleChange:
    {
        if(d->windowTitleChange(object))
            return true;
    }break;
    case QEvent::WindowIconChange:
    {
        if(d->windowIconChange(object))
            return true;
    }break;
    case QEvent::WindowStateChange:
    {
        d->windowStateChange(object);
        return true;
    }break;
    case QEvent::Resize:
    {
        d->windowSizeChange(object);
        return true;
    }break;
    case QEvent::MouseMove:
    case QEvent::HoverMove:
    case QEvent::MouseButtonPress:
    case QEvent::MouseButtonRelease:
    case QEvent::MouseButtonDblClick:
    {
        d->handleWidgetMouseEvent(object,event);
        return true;
    }break;
    default:
        break;
    }
    return QObject::eventFilter(object,event);
}

#include "fancybar.moc"


