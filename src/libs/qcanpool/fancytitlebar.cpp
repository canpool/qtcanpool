/**
 * Copyright (C) 2018-2022 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 * Notice: based on framelesshelper
**/
#include "fancytitlebar_p.h"
#include "fancytitlebar.h"

#include <QLabel>
#include <QHBoxLayout>
#include <QMenu>
#include <QAction>
#include <QMouseEvent>
#include <QEvent>
#include <QPoint>
#include <QApplication>
#include <QWidget>
#include <QStyle>

#if (QT_VERSION < QT_VERSION_CHECK(5,0,0))
#include <QApplication>
#include <QDesktopWidget>
#else
#include <QGuiApplication>
#include <QScreen>
#endif

QCANPOOL_BEGIN_NAMESPACE

/* FancyCursor */
FancyCursor::FancyCursor()
    : m_nBorderWidth(5)
{
    reset();
}

void FancyCursor::reset()
{
    m_bOnEdges              = false;
    m_bOnLeftEdge           = false;
    m_bOnRightEdge          = false;
    m_bOnTopEdge            = false;
    m_bOnBottomEdge         = false;
    m_bOnTopLeftEdge        = false;
    m_bOnBottomLeftEdge     = false;
    m_bOnTopRightEdge       = false;
    m_bOnBottomRightEdge    = false;
}

/**
 * @brief Update cursor status
 *                  Top
 *  TopLeft          |             TopRight
 *      \-------------------------/
 *      | ----------------------- |
 *      | |                     | |
 *      : : -- Left             : : -- Right
 *      | |                     | |
 *      | ----------------------- |
 *      /-------------------\-----\
 *  BottomLeft        |      \     BottomRight
 *                 Bottom     '-- BorderWidth
 *
 * @param gMousePos
 * @param frameRect
 */
void FancyCursor::update(const QPoint &gMousePos, const QRect &frameRect)
{
    int globalMouseX = gMousePos.x();
    int globalMouseY = gMousePos.y();
    int frameX       = frameRect.x();
    int frameY       = frameRect.y();
    int frameWidth   = frameRect.width();
    int frameHeight  = frameRect.height();

    m_bOnLeftEdge    = (globalMouseX >= frameX && globalMouseX <= frameX + m_nBorderWidth);
    m_bOnRightEdge   = (globalMouseX >= frameX + frameWidth - m_nBorderWidth &&
                        globalMouseX <= frameX + frameWidth);
    m_bOnTopEdge     = (globalMouseY >= frameY && globalMouseY <= frameY + m_nBorderWidth);
    m_bOnBottomEdge  = (globalMouseY >= frameY + frameHeight - m_nBorderWidth &&
                        globalMouseY <= frameY + frameHeight);

    m_bOnTopLeftEdge     = m_bOnTopEdge && m_bOnLeftEdge;
    m_bOnBottomLeftEdge  = m_bOnBottomEdge && m_bOnLeftEdge;
    m_bOnTopRightEdge    = m_bOnTopEdge && m_bOnRightEdge;
    m_bOnBottomRightEdge = m_bOnBottomEdge && m_bOnRightEdge;

    m_bOnEdges = m_bOnLeftEdge || m_bOnRightEdge || m_bOnTopEdge || m_bOnBottomEdge;
}

FancyScreen::FancyScreen()
{
    m_screenRects.clear();

#if (QT_VERSION < QT_VERSION_CHECK(5,0,0))
    QDesktopWidget *desktop = QApplication::desktop();
    int screenCnt = desktop->screenCount();

    for (int i = 0; i < screenCnt; i++) {
        QRect rect = desktop->availableGeometry(i);
        m_screenRects.append(rect);
    }
#else
    QList<QScreen *> screenList = QGuiApplication::screens();
    foreach (QScreen *screen, screenList) {
#if defined(Q_OS_WIN)
        m_screenRects.append(screen->availableGeometry());
#else
        m_screenRects.append(screen->virtualGeometry());
#endif
    }
#endif
}

int FancyScreen::currentScreen(const int x)
{
    int width;

    for (int i = 0; i < m_screenRects.count(); i++) {
        width = m_screenRects.at(i).x() + m_screenRects.at(i).width();
        if (x <= width) {
            return i;
        }
    }
    return 0;
}

QRect FancyScreen::screenRect(const int current)
{
    if (current >= m_screenRects.count()) {
        return m_screenRects.at(0);
    } else {
        return m_screenRects.at(current);
    }
}

QRect FancyScreen::normalRect()
{
    QRect geom;
#if (QT_VERSION < QT_VERSION_CHECK(5,0,0))
    geom = QApplication::desktop()->availableGeometry();
#else
#if defined(Q_OS_WIN)
    geom = QGuiApplication::primaryScreen()->availableGeometry();
#else
    geom = QGuiApplication::primaryScreen()->virtualGeometry();
#endif
#endif
    return QRect(geom.x() + 100, geom.y() + 100,
                 2 * geom.width() / 3, 2 * geom.height() / 3);
}

FancyTitleBarPrivate::FancyTitleBarPrivate()
    : m_mainWidget(nullptr)
    , m_titleWidget(nullptr)
    , m_normalRect(FancyScreen::normalRect())
    , m_currentScreen(0)
    , m_bEdgePressed(false)
    , m_bLeftButtonPressed(false)
    , m_bLeftButtonDbClicked(false)
    , m_bLeftButtonTitlePressed(true)
    , m_bCursorShapeChanged(false)
    , m_isMaximized(false)
    , m_isMinimized(false)
    , m_bWidgetMaximizable(true)
    , m_bWidgetResizable(true)
    , m_bWidgetMovable(true)
    , m_movePoint(QPoint(0, 0))
{
}

FancyTitleBarPrivate::~FancyTitleBarPrivate()
{}

void FancyTitleBarPrivate::init()
{
    const int sz = qApp->style()->pixelMetric(QStyle::PM_SmallIconSize, Q_NULLPTR, Q_NULLPTR);

    m_logoButton = new QToolButton(m_mainWidget);
    m_logoButton->setIcon(m_logoButton->style()->standardIcon(QStyle::SP_TitleBarMenuButton));
    m_logoButton->setIconSize(QSize(sz, sz));

    m_titleLabel = new QLabel(m_mainWidget);
    m_titleLabel->setAlignment(Qt::AlignCenter);

    m_toolBar = new QToolBar(m_mainWidget);
    m_toolBar->setObjectName(QLatin1String("qtc_sys_toolbar"));
//    m_toolBar->setStyleSheet("QToolBar{border:none; background:transparent;}");
    m_toolBar->setIconSize(QSize(sz, sz));

    m_minimizeAction = new QAction(m_mainWidget);
    m_minimizeAction->setText(tr("minimize"));

    m_maximizeAction = new QAction(m_mainWidget);
    m_maximizeTip = QString(tr("maximize"));
    m_normalTip = QString(tr("restore"));
    m_maximizeAction->setText(m_maximizeTip);

    m_closeAction = new QAction(m_mainWidget);
    m_closeAction->setText(tr("close"));

    QWidget *spacer = new QWidget();
    spacer->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    m_toolBar->addWidget(spacer);
    m_toolBar->addAction(m_minimizeAction);
    m_toolBar->addAction(m_maximizeAction);
    m_toolBar->addAction(m_closeAction);

    QToolButton *actionToolButton = nullptr;
    actionToolButton = qobject_cast<QToolButton *>(m_toolBar->widgetForAction(m_minimizeAction));
    m_minimizeAction->setIcon(actionToolButton->style()->standardIcon(QStyle::SP_TitleBarMinButton));
    actionToolButton->setObjectName(QLatin1String("minimize"));

    actionToolButton = qobject_cast<QToolButton *>(m_toolBar->widgetForAction(m_maximizeAction));
    m_maximizeIcon = actionToolButton->style()->standardIcon(QStyle::SP_TitleBarMaxButton);
    m_normalIcon = actionToolButton->style()->standardIcon(QStyle::SP_TitleBarNormalButton);
    m_maximizeAction->setIcon(m_maximizeIcon);
    actionToolButton->setObjectName(QLatin1String("maximize"));

    actionToolButton = qobject_cast<QToolButton *>(m_toolBar->widgetForAction(m_closeAction));
    m_closeAction->setIcon(actionToolButton->style()->standardIcon(QStyle::SP_TitleBarCloseButton));
    actionToolButton->setObjectName(QLatin1String("close"));

    connect(m_minimizeAction, SIGNAL(triggered()), this, SLOT(systemButtonClicked()));
    connect(m_maximizeAction, SIGNAL(triggered()), this, SLOT(systemButtonClicked()));
    connect(m_closeAction, SIGNAL(triggered()), this, SLOT(systemButtonClicked()));
}

void FancyTitleBarPrivate::installWidget(QWidget *w)
{
    m_mainWidget = w;
    m_windowFlags = w->windowFlags();
    m_mainWidget->setMouseTracking(true);
    m_mainWidget->setAttribute(Qt::WA_Hover, true);
}

void FancyTitleBarPrivate::setWidgetResizable(bool resizable)
{
    m_bWidgetResizable = resizable;
}

void FancyTitleBarPrivate::setWidgetMovable(bool movable)
{
    m_bWidgetMovable = movable;
}

void FancyTitleBarPrivate::setWidgetMaximizable(bool maximizable)
{
    m_bWidgetMaximizable = maximizable;
}

void FancyTitleBarPrivate::handleWidgetMouseEvent(QObject *obj, QEvent *event)
{
    Q_UNUSED(obj);

    switch (event->type()) {
        case QEvent::MouseButtonPress:
            handleMousePressEvent(static_cast<QMouseEvent *>(event));
            break;
        case QEvent::MouseButtonRelease:
            handleMouseReleaseEvent(static_cast<QMouseEvent *>(event));
            break;
        case QEvent::MouseMove:
            handleMouseMoveEvent(static_cast<QMouseEvent *>(event));
            break;
        case QEvent::Leave:
            handleLeaveEvent(static_cast<QMouseEvent *>(event));
            break;
        case QEvent::HoverMove:
            handleHoverMoveEvent(static_cast<QHoverEvent *>(event));
            break;
        case QEvent::MouseButtonDblClick:
            handleMouseDblClickEvent(static_cast<QMouseEvent *>(event));
            break;
        default:
            break;
    }
}

void FancyTitleBarPrivate::updateWindowButtons()
{
    m_maximizeAction->setVisible(m_windowFlags & Qt::WindowMaximizeButtonHint);
    m_minimizeAction->setVisible(m_windowFlags & Qt::WindowMinimizeButtonHint);

    setWidgetMaximizable(m_windowFlags & Qt::WindowMaximizeButtonHint);
}

bool FancyTitleBarPrivate::windowTitleChange(QObject *obj)
{
    QWidget *w = qobject_cast<QWidget *>(obj);
    if (w) {
        m_titleLabel->setText(w->windowTitle());
        return true;
    }
    return false;
}

bool FancyTitleBarPrivate::windowIconChange(QObject *obj)
{
    QWidget *w = qobject_cast<QWidget *>(obj);
    if (w) {
        QIcon icon = w->windowIcon();
        if (!icon.isNull()) {
            m_logoButton->setIcon(icon);
            emit q->windowIconChanged(icon);
        }
        return true;
    }
    return false;
}

void FancyTitleBarPrivate::windowSizeChange(QObject *obj)
{
    QWidget *w = qobject_cast<QWidget *>(obj);

    if (w) {
        if (w->isMaximized()) {
            // Mask the maximized state, because the frameless window
            // will cover the windows taskbar when maximized
            w->setWindowState(w->windowState() & ~Qt::WindowMaximized);
            this->maximizeWidget(w);
        }

        windowStateChange(obj);
    }
}

void FancyTitleBarPrivate::windowStateChange(QObject *obj)
{
    Q_UNUSED(obj);

    QToolButton *toolButton = qobject_cast<QToolButton *>(m_toolBar->widgetForAction(m_maximizeAction));
    if (m_isMaximized) {
        toolButton->setProperty("state", "maximize");
        m_maximizeAction->setToolTip(m_normalTip);
        m_maximizeAction->setIcon(m_normalIcon);
    } else {
        toolButton->setProperty("state", "restore");
        m_maximizeAction->setToolTip(m_maximizeTip);
        m_maximizeAction->setIcon(m_maximizeIcon);
    }
    toolButton->style()->polish(toolButton);
}

void FancyTitleBarPrivate::handleMousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton && !m_isMaximized) {
        m_bEdgePressed = true;
        QRect frameRect = m_mainWidget->frameGeometry();
        m_pressCursor.update(event->globalPos(), frameRect);
    }
    mousePressEvent(event);
    event->ignore();
}

void FancyTitleBarPrivate::handleMouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        m_bEdgePressed = false;
        m_pressCursor.reset();

        if (m_bWidgetResizable) {
            updateCursorShape(event->globalPos());
        }
    }
    mouseReleaseEvent(event);
    event->ignore();
}

void FancyTitleBarPrivate::handleMouseMoveEvent(QMouseEvent *event)
{
    if (m_bEdgePressed) {
        if (m_bWidgetResizable && m_pressCursor.m_bOnEdges) {
            resizeWidget(event->globalPos());
        }
    } else if (m_bWidgetResizable) {
        updateCursorShape(event->globalPos());
    }
    mouseMoveEvent(event);
}

void FancyTitleBarPrivate::handleLeaveEvent(QEvent *event)
{
    Q_UNUSED(event);

    if (!m_bEdgePressed) {
        m_mainWidget->unsetCursor();
    }
}

void FancyTitleBarPrivate::handleHoverMoveEvent(QHoverEvent *event)
{
    if (!m_bEdgePressed && m_bWidgetResizable) {
        updateCursorShape(m_mainWidget->mapToGlobal(event->pos()));
    }
}

void FancyTitleBarPrivate::handleMouseDblClickEvent(QMouseEvent *event)
{
    mouseDoubleClickEvent(event);
}

void FancyTitleBarPrivate::restoreWidget(QWidget *pWidget)
{
    m_isMaximized = false;
    pWidget->setGeometry(m_normalRect);
    emit q->windowResizable(true);
}

void FancyTitleBarPrivate::maximizeWidget(QWidget *pWidget)
{
    // Update the value before the window size changes,
    // because the event filter response may be random
    m_isMaximized = true;

    // support multi-screen
    int x = pWidget->frameGeometry().x() + pWidget->frameGeometry().width() / 2;
    // Use temporary variables, support screen hot plug
    FancyScreen sreen;
    m_currentScreen = sreen.currentScreen(x);
    QRect rect = sreen.screenRect(m_currentScreen);
    pWidget->setGeometry(rect);

    emit q->windowResizable(false);
}

void FancyTitleBarPrivate::resizeWidget(const QPoint &gMousePos)
{
    // original rect
    QRect oriRect = m_mainWidget->frameGeometry();
    int left, top, right, bottom;
    oriRect.getCoords(&left, &top, &right, &bottom);
    int minWidth = m_mainWidget->minimumWidth();
    int minHeight = m_mainWidget->minimumHeight();

    // update coords
    if (m_pressCursor.m_bOnTopLeftEdge) {
        left = gMousePos.x();
        top = gMousePos.y();
    } else if (m_pressCursor.m_bOnBottomLeftEdge) {
        left = gMousePos.x();
        bottom = gMousePos.y();
    } else if (m_pressCursor.m_bOnTopRightEdge) {
        right = gMousePos.x();
        top = gMousePos.y();
    } else if (m_pressCursor.m_bOnBottomRightEdge) {
        right = gMousePos.x();
        bottom = gMousePos.y();
    } else if (m_pressCursor.m_bOnLeftEdge) {
        left = gMousePos.x();
    } else if (m_pressCursor.m_bOnRightEdge) {
        right = gMousePos.x();
    } else if (m_pressCursor.m_bOnTopEdge) {
        top = gMousePos.y();
    } else if (m_pressCursor.m_bOnBottomEdge) {
        bottom = gMousePos.y();
    }

    QRect newRect(QPoint(left, top), QPoint(right, bottom));

    if (newRect.isValid()) {
        if (minWidth > newRect.width()) {
            if (left != oriRect.left()) {
                newRect.setLeft(oriRect.left());
            } else {
                newRect.setRight(oriRect.right());
            }
        }

        if (minHeight > newRect.height()) {
            if (top != oriRect.top()) {
                newRect.setTop(oriRect.top());
            } else {
                newRect.setBottom(oriRect.bottom());
            }
        }

        m_mainWidget->setGeometry(newRect);
    }
}

void FancyTitleBarPrivate::updateCursorShape(const QPoint &gMousePos)
{
    if (m_mainWidget->isFullScreen() || m_mainWidget->isMaximized() || m_isMaximized) {
        if (m_bCursorShapeChanged) {
            m_mainWidget->unsetCursor();
            m_bCursorShapeChanged = false;
        }
        return;
    }

    m_moveCursor.update(gMousePos, m_mainWidget->frameGeometry());

    if (m_moveCursor.m_bOnTopLeftEdge || m_moveCursor.m_bOnBottomRightEdge) {
        m_mainWidget->setCursor(Qt::SizeFDiagCursor);
        m_bCursorShapeChanged = true;
    } else if (m_moveCursor.m_bOnTopRightEdge || m_moveCursor.m_bOnBottomLeftEdge) {
        m_mainWidget->setCursor(Qt::SizeBDiagCursor);
        m_bCursorShapeChanged = true;
    } else if (m_moveCursor.m_bOnLeftEdge || m_moveCursor.m_bOnRightEdge) {
        m_mainWidget->setCursor(Qt::SizeHorCursor);
        m_bCursorShapeChanged = true;
    } else if (m_moveCursor.m_bOnTopEdge || m_moveCursor.m_bOnBottomEdge) {
        m_mainWidget->setCursor(Qt::SizeVerCursor);
        m_bCursorShapeChanged = true;
    } else {
        if (m_bCursorShapeChanged) {
            m_mainWidget->unsetCursor();
            m_bCursorShapeChanged = false;
        }
    }
}

/**
 * @brief Calculate the starting position of the window from maximized to normal size
 *
 *  |-------------------|       |---------|
 *  |                   |  ==>  |         |
 *  |                   |       |---------|
 *  |                   |
 *  |-------------------|
 *
 * as follows:
 * ****************************************************************************
 *
 *   (screenX, screenY)
 *         \        mouse
 *          +-------*---------------------
 *        / |                |           |
 *   point  |                |oriHeight  |
 *          |                |           |
 *          |----------------|           |   maxHeight
 *          |      oriWidth              |
 *          |                            |
 *          |                            |
 *          ------------------------------
 *                  maxWidth
 *
 *****************************************************************************
 *
 *   (screenX, screenY)
 *         \                        mouse
 *          +------------+----------*-----
 *          |          / |               |
 *          |      point |oriHeight      |
 *          |            |               |
 *          |            ----------------| maxHeight
 *          |               oriWidth     |
 *          |                            |
 *          |                            |
 *          ------------------------------
 *                  maxWidth
 *
 *****************************************************************************
 *
 * 1. When titleWidget was set, the x,y coordinate (constrained within the title bar area) as follows:
 *
 *  1) y coordinate is fixed to 0
 *  2) x coordinate is:
 *      a) screen x coordinate, when the drag point is left half (Align screen left)
 *          |--*-------------|
 *          |-------|
 *          `-> screenX
 *      b) maxwidth - oriWidth, when the drag point is right half (Align screen right)
 *          |-------------*--|
 *                   |-------|
 *                   `-> maxWidth - oriWidth
 *      c) mouseX - oriWidth/2
 *          |---------*------|
 *                |-------|
 *                `-> mouseX - oriWidth/2
 *
 * 2. When titleWidget was not set, the y coordinate is not constrained,
 * the method of calculating the y coordinate is the same as above
 *
 * @param pWindow
 * @param event
 * @return QPoint
 */
QPoint FancyTitleBarPrivate::windowStartPos(QWidget *pWindow, QMouseEvent *event) const
{
    int mouseX = event->globalX();
    int mouseY = event->globalY();
    FancyScreen screen;
    QRect rect = screen.screenRect(m_currentScreen);
    int maxWidth = rect.x() + rect.width();
    int maxHeight = rect.y() + rect.height();
    int screenX = rect.x();
    int screenY = rect.y();
    int oriWidth = m_normalRect.width();
    int oriHeight = m_normalRect.height();

    if (oriWidth == 0) {
        oriWidth = pWindow->minimumWidth();
    }
    if (oriHeight == 0) {
        oriHeight = pWindow->minimumHeight();
    }

    QPoint point;
    point.setY(0);

    if (mouseX - screenX < oriWidth / 2) {
        point.setX(screenX);                // Align screen left
    } else if (maxWidth - mouseX < oriWidth / 2) {
        point.setX(maxWidth - oriWidth);    // Align screen right
    } else {
        point.setX(mouseX - oriWidth / 2);
    }

    if (m_titleWidget == nullptr) {
        if (mouseY - screenY < oriHeight / 2) {
            point.setY(screenY);
        } else if (maxHeight - mouseY < oriHeight / 2) {
            point.setY(maxHeight - oriHeight);
        } else {
            point.setY(mouseY - oriHeight / 2);
        }
    }

    return point;
}

QRect FancyTitleBarPrivate::validDragRect()
{
    FancyScreen screen;
    QRect rect = screen.screenRect(m_currentScreen);

    int offset = 2 * m_moveCursor.m_nBorderWidth;
    rect.setWidth(rect.width() - rect.x() - offset);
    rect.setHeight(rect.height() - rect.y() - offset);
    rect.setX(offset);
    rect.setY(offset);

    return rect;
}

void FancyTitleBarPrivate::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        m_bLeftButtonPressed = true;
        m_bLeftButtonTitlePressed = m_titleWidget ? (m_titleWidget->sizeHint().height() > event->pos().y()) : true;
        QWidget *pWindow = m_mainWidget;

        if (pWindow->isTopLevel()) {
            if (m_isMaximized && m_bLeftButtonTitlePressed) {
                m_movePoint = event->globalPos() - windowStartPos(pWindow, event);
            } else {
                m_movePoint = event->globalPos() - pWindow->pos();
            }
        }
    }
}

void FancyTitleBarPrivate::mouseReleaseEvent(QMouseEvent *event)
{
    m_bLeftButtonPressed = false;

    // maximize on the top of the screen
    if (!m_isMaximized && !m_bLeftButtonDbClicked) {
        if (event->globalY() <= 3) {
            m_mainWidget->move(m_mainWidget->frameGeometry().x(), 10);

            if (m_bWidgetMaximizable) {
                emit m_maximizeAction->triggered();
            }
        } else {
            int y = m_mainWidget->frameGeometry().y();
            if (y < 0) {
                // Always show all titlebar on screen
                m_mainWidget->move(m_mainWidget->frameGeometry().x(), 0);
            }
        }
    }
    m_bLeftButtonDbClicked = false;
}

void FancyTitleBarPrivate::mouseMoveEvent(QMouseEvent *event)
{
    if (!m_bLeftButtonPressed || !m_bLeftButtonTitlePressed) {
        return;
    }

    QWidget *pWindow = m_mainWidget;
    if (pWindow->isTopLevel()) {
        if (m_bWidgetMaximizable && m_isMaximized) {
            // calculate the valid rect
            QRect rect = validDragRect();
            if (rect.contains(event->pos())) {
                m_movePoint = event->globalPos() - windowStartPos(pWindow, event);
                restoreWidget(pWindow);
            }
        } else {
            if (m_bWidgetResizable && m_pressCursor.m_bOnEdges) {
                event->ignore();
            } else {
                pWindow->move(event->globalPos() - m_movePoint);
            }
        }
    }
}

void FancyTitleBarPrivate::mouseDoubleClickEvent(QMouseEvent *event)
{
    if ( event->button() == Qt::LeftButton ) {
        m_bLeftButtonDbClicked = true;
        if (m_bWidgetMaximizable && m_bLeftButtonTitlePressed) {
            if (m_isMaximized) {
                m_movePoint = event->globalPos() - windowStartPos(m_mainWidget, event);
            }
            emit m_maximizeAction->triggered();
        }
    }
}

void FancyTitleBarPrivate::systemButtonClicked()
{
    QAction *pAction = qobject_cast<QAction *>(sender());
    QWidget *pWindow = m_mainWidget;

    if (pWindow) {
        if (pAction == m_minimizeAction) {
            pWindow->showMinimized();
            m_isMinimized = true;
        } else if (pAction == m_maximizeAction) {
            if (m_isMaximized) {
                restoreWidget(pWindow);
            } else {
                m_normalRect = pWindow->frameGeometry();
                maximizeWidget(pWindow);
            }
            m_isMinimized = false;
        } else if (pAction == m_closeAction) {
            pWindow->close();
        }
    }
}

/**
 * @brief FancyTitleBar::FancyTitleBar
 * @param mainWidget - the host widget
 */
FancyTitleBar::FancyTitleBar(QWidget *mainWidget)
    : QObject(mainWidget)
    , d(new FancyTitleBarPrivate())
{
    Q_ASSERT(mainWidget);

    d->q = this;
    d->installWidget(mainWidget);
    d->init();
    mainWidget->installEventFilter(this);
}

FancyTitleBar::~FancyTitleBar()
{
    delete d;
}

void FancyTitleBar::setTitleWidget(QWidget *widget)
{
    d->m_titleWidget = widget;
}

QWidget *FancyTitleBar::mainWidget() const
{
    return d->m_mainWidget;
}

QAction *FancyTitleBar::addAction(const QIcon &icon, const QString &text)
{
    QAction *action = new QAction(icon, text);
    d->m_toolBar->insertAction(d->m_minimizeAction, action);
    return action;
}

void FancyTitleBar::addAction(QAction *action)
{
    d->m_toolBar->insertAction(d->m_minimizeAction, action);
}

void FancyTitleBar::insertAction(QAction *before, QAction *action)
{
    d->m_toolBar->insertAction(before, action);
}

QAction *FancyTitleBar::addSeparator()
{
    return insertSeparator(d->m_minimizeAction);
}

QAction *FancyTitleBar::insertSeparator(QAction *before)
{
    return d->m_toolBar->insertSeparator(before);
}

QAction *FancyTitleBar::addWidget(QWidget *widget)
{
    return insertWidget(d->m_minimizeAction, widget);
}

QAction *FancyTitleBar::insertWidget(QAction *before, QWidget *widget)
{
    return d->m_toolBar->insertWidget(before, widget);
}

QLabel *FancyTitleBar::titleLabel() const
{
    return d->m_titleLabel;
}

QToolButton *FancyTitleBar::logoButton() const
{
    return d->m_logoButton;
}

QWidget *FancyTitleBar::systemToolBar() const
{
    return d->m_toolBar;
}

void FancyTitleBar::setButtonIcon(FancyTitleBar::ButtonRole role, const QIcon &icon)
{
    switch (role) {
        case FancyTitleBar::MinButton: {
            d->m_minimizeAction->setIcon(icon);
        } break;
        case FancyTitleBar::MaxButton: {
            d->m_maximizeIcon = icon;
            if (!d->m_isMaximized) {
                d->m_maximizeAction->setIcon(icon);
            }
        } break;
        case FancyTitleBar::NormalButton: {
            d->m_normalIcon = icon;
            if (d->m_isMaximized) {
                d->m_maximizeAction->setIcon(icon);
            }
        } break;
        case FancyTitleBar::CloseButton: {
            d->m_closeAction->setIcon(icon);
        } break;
    }
}

void FancyTitleBar::setButtonTip(FancyTitleBar::ButtonRole role, const QString &tip)
{
    switch (role) {
        case FancyTitleBar::MinButton: {
            d->m_minimizeAction->setToolTip(tip);
        } break;
        case FancyTitleBar::MaxButton: {
            d->m_maximizeTip = tip;
            if (!d->m_isMaximized) {
                d->m_maximizeAction->setToolTip(tip);
            }
        } break;
        case FancyTitleBar::NormalButton: {
            d->m_normalTip = tip;
            if (d->m_isMaximized) {
                d->m_maximizeAction->setToolTip(tip);
            }
        } break;
        case FancyTitleBar::CloseButton: {
            d->m_closeAction->setToolTip(tip);
        } break;
    }
}

void FancyTitleBar::setIconSize(const QSize &size)
{
    d->m_toolBar->setIconSize(size);
}

void FancyTitleBar::setWidgetResizable(bool resizable)
{
    d->m_bWidgetResizable = resizable;
}

void FancyTitleBar::setWidgetMovable(bool movable)
{
    d->m_bWidgetMovable = movable;
}

void FancyTitleBar::setWidgetMaximizable(bool maximizable)
{
    d->m_bWidgetMaximizable = maximizable;
}

void FancyTitleBar::updateWidgetFlags()
{
    d->m_windowFlags = d->m_mainWidget->windowFlags();
    d->updateWindowButtons();
}

bool FancyTitleBar::eventFilter(QObject *object, QEvent *event)
{
    switch (event->type()) {
        case QEvent::WindowTitleChange: {
            if (d->windowTitleChange(object)) {
                return true;
            }
        } break;
        case QEvent::WindowIconChange: {
            if (d->windowIconChange(object)) {
                return true;
            }
        } break;
        case QEvent::WindowStateChange: {
            d->windowStateChange(object);
            return true;
        }
        case QEvent::Resize: {
            d->windowSizeChange(object);
            return true;
        }
        case QEvent::MouseMove:
        case QEvent::HoverMove:
        case QEvent::MouseButtonPress:
        case QEvent::MouseButtonRelease:
        case QEvent::MouseButtonDblClick: {
            d->handleWidgetMouseEvent(object, event);
            return true;
        }
        default:
            break;
    }

    return QObject::eventFilter(object, event);
}

QCANPOOL_END_NAMESPACE
