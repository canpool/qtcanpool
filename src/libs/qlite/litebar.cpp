/***************************************************************************
 **
 **  Copyright (C) 2019 MaMinJie <canpool@163.com>
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
#include "litebar.h"
#include "litebar_p.h"

#include <QApplication>
#include <QToolButton>
#include <QLabel>
#include <QToolBar>

#include "litescreen.h"

using namespace QLite;

LiteCursor::LiteCursor()
{
    m_nBorderWidth = 5;
    reset();
}

void LiteCursor::reset()
{
    m_bOnEdges = false;
    m_bOnLeftEdge = false;
    m_bOnRightEdge = false;
    m_bOnTopEdge = false;
    m_bOnBottomEdge = false;
    m_bOnTopLeftEdge = false;
    m_bOnBottomLeftEdge = false;
    m_bOnTopRightEdge  = false;
    m_bOnBottomRightEdge = false;
}

void LiteCursor::recalculate(const QPoint &gMousePos, const QRect &frameRect)
{
    int globalMouseX = gMousePos.x();
    int globalMouseY = gMousePos.y();
    int frameX = frameRect.x();
    int frameY = frameRect.y();
    int frameWidth = frameRect.width();
    int frameHeight = frameRect.height();
    m_bOnLeftEdge = (globalMouseX >= frameX &&
                     globalMouseX <= frameX + m_nBorderWidth );
    m_bOnRightEdge = (globalMouseX >= frameX + frameWidth - m_nBorderWidth &&
                      globalMouseX <= frameX + frameWidth);
    m_bOnTopEdge = (globalMouseY >= frameY &&
                    globalMouseY <= frameY + m_nBorderWidth );
    m_bOnBottomEdge = (globalMouseY >= frameY + frameHeight - m_nBorderWidth &&
                       globalMouseY <= frameY + frameHeight);
    m_bOnTopLeftEdge = m_bOnTopEdge && m_bOnLeftEdge;
    m_bOnBottomLeftEdge = m_bOnBottomEdge && m_bOnLeftEdge;
    m_bOnTopRightEdge = m_bOnTopEdge && m_bOnRightEdge;
    m_bOnBottomRightEdge = m_bOnBottomEdge && m_bOnRightEdge;
    m_bOnEdges = m_bOnLeftEdge || m_bOnRightEdge || m_bOnTopEdge || m_bOnBottomEdge;
}

LiteBarPrivate::LiteBarPrivate()
{
    q = nullptr;

    m_logoButton = nullptr;
    m_titleLabel = nullptr;
    m_toolBar = nullptr;
    m_closeAction = nullptr;
    m_maximizeAction = nullptr;
    m_minimizeAction = nullptr;

    m_mainWidget = nullptr;
    m_bEdgePressed = false;
    m_bCursorShapeChanged = false;

    m_bWidgetResizable = true;
    m_bWidgetMovable = true;
    m_bWidgetMaximizable = true;

    m_isMaximized = false;
    m_isMinimized = false;

    m_currentScreen = 0;

    m_normalRect = LiteScreen::normalRect();

    m_mousePoint.setX(0);
    m_mousePoint.setY(0);
    m_bLeftButtonPressed = false;
    m_bLeftButtonDbClicked = false;
}

void LiteBarPrivate::init()
{
    m_logoButton = new QToolButton(m_mainWidget);
    m_titleLabel = new QLabel(m_mainWidget);

    m_toolBar = new QToolBar(m_mainWidget);
    m_toolBar->setObjectName(QLatin1String("qlite_sys_toolbar"));
    m_toolBar->setStyleSheet("QToolBar{border:none; background: transparent;}");
    m_toolBar->setIconSize(QSize(18, 18));

    m_minimizeAction = new QAction(m_mainWidget);
    m_minimizeAction->setIcon(QIcon(":/qlite/min"));
    m_minimizeAction->setText(tr("minimize"));
    m_minimizeAction->setToolTip(tr("minimize"));

    m_maximizeAction = new QAction(m_mainWidget);
    m_maximizeAction->setIcon(QIcon(":/qlite/max"));
    m_maximizeAction->setText(tr("maximize"));
    m_maximizeAction->setToolTip(tr("maximize"));

    m_closeAction = new QAction(m_mainWidget);
    m_closeAction->setIcon(QIcon(":/qlite/close"));
    m_closeAction->setText(tr("close"));
    m_closeAction->setToolTip(tr("close"));

    QWidget *spacer = new QWidget();
    spacer->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    m_toolBar->addWidget(spacer);
    m_toolBar->addAction(m_minimizeAction);
    m_toolBar->addAction(m_maximizeAction);
    m_toolBar->addAction(m_closeAction);

    QToolButton *actionToolButton = nullptr;
    actionToolButton = qobject_cast<QToolButton *>(m_toolBar->widgetForAction(m_minimizeAction));
    actionToolButton->setObjectName(QLatin1String("minimize"));
    actionToolButton = qobject_cast<QToolButton *>(m_toolBar->widgetForAction(m_maximizeAction));
    actionToolButton->setObjectName(QLatin1String("maximize"));
    actionToolButton = qobject_cast<QToolButton *>(m_toolBar->widgetForAction(m_closeAction));
    actionToolButton->setObjectName(QLatin1String("close"));

    connect(m_minimizeAction, SIGNAL(triggered()), this, SLOT(slotSysButtonClicked()));
    connect(m_maximizeAction, SIGNAL(triggered()), this, SLOT(slotSysButtonClicked()));
    connect(m_closeAction, SIGNAL(triggered()), this, SLOT(slotSysButtonClicked()));

    updateWindowButtons();
}

void LiteBarPrivate::installWidget(QWidget *w)
{
    m_mainWidget = w;
    m_windowFlags = w->windowFlags();
    m_mainWidget->setMouseTracking(true);
    m_mainWidget->setAttribute(Qt::WA_Hover, true);
}

void LiteBarPrivate::handleWidgetMouseEvent(QObject *obj, QEvent *event)
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

bool LiteBarPrivate::windowTitleChange(QObject *obj)
{
    QWidget *pWidget = qobject_cast<QWidget *>(obj);

    if (pWidget == nullptr) {
        return false;
    }
    QFont font = QApplication::font();
    font.setPointSize(11);
    m_titleLabel->setText(pWidget->windowTitle());
    m_titleLabel->setFont(font);

    return true;
}

bool LiteBarPrivate::windowIconChange(QObject *obj)
{
    QWidget *pWidget = qobject_cast<QWidget *>(obj);

    if (pWidget == nullptr) {
        return false;
    }
    QIcon icon = pWidget->windowIcon();
    if (!icon.isNull()) {
        m_logoButton->setIcon(icon);
        m_logoButton->setIconSize(QSize(20, 20));
    }
    return true;
}

void LiteBarPrivate::windowSizeChange(QObject *obj)
{
    QWidget *pWindow = qobject_cast<QWidget *>(obj);

     if (pWindow) {
         bool bMaximize = pWindow->isMaximized();
         if (bMaximize) {
             pWindow->setWindowState(pWindow->windowState() & ~Qt::WindowMaximized);
             this->maximizeWidget(pWindow);
         }

         windowStateChange(obj);
     }
}

void LiteBarPrivate::windowStateChange(QObject *obj)
{
    Q_UNUSED(obj);

    if (m_isMaximized) {
        m_maximizeAction->setToolTip(tr("restore"));
        m_maximizeAction->setIcon(QIcon(":/qlite/restore"));
    } else {
        m_maximizeAction->setToolTip(tr("maximize"));
        m_maximizeAction->setIcon(QIcon(":/qlite/max"));
    }
}

void LiteBarPrivate::updateWindowButtons()
{
    m_maximizeAction->setVisible(m_windowFlags & Qt::WindowMaximizeButtonHint);
    m_minimizeAction->setVisible(m_windowFlags & Qt::WindowMinimizeButtonHint);
    q->setWidgetMaximizable(m_windowFlags & Qt::WindowMaximizeButtonHint);
}

void LiteBarPrivate::handleMousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton && !m_isMaximized && m_bWidgetResizable) {
        m_bEdgePressed = true;
        QRect frameRect = m_mainWidget->frameGeometry();
        m_pressCursor.recalculate(event->globalPos(), frameRect);
        m_dragPos = event->globalPos() - frameRect.topLeft();
    }

    if (event->button() == Qt::LeftButton) {
        m_bLeftButtonPressed = true;
        QWidget *pWindow = m_mainWidget;

        if (pWindow->isTopLevel()) {
            if (m_isMaximized) {
                m_dragPoint = calcDragPoint(pWindow, event);
                m_movePoint = event->globalPos() - m_dragPoint;
            } else {
                m_movePoint = event->globalPos() - pWindow->pos();
            }
        }
    }

    event->ignore();
}

void LiteBarPrivate::handleMouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        m_bEdgePressed = false;

        if (m_bWidgetResizable) {
            updateCursorShape(event->globalPos());
        }
        m_pressCursor.reset();
    }

    m_bLeftButtonPressed = false;
    // maximize on the top of the screen
    if (!m_isMaximized && !m_bLeftButtonDbClicked) {
        if (event->globalY() == 0) {
            m_mainWidget->move(m_mainWidget->frameGeometry().x(), 10);

            if (m_bWidgetMaximizable) {
                emit m_maximizeAction->triggered();
            }
        } else {
            int y = m_mainWidget->frameGeometry().y();

            if (y < 0) {
                m_mainWidget->move(m_mainWidget->frameGeometry().x(), 0);
            }
        }
    }
    m_bLeftButtonDbClicked = false;

    event->ignore();
}

void LiteBarPrivate::handleMouseMoveEvent(QMouseEvent *event)
{
    if (m_bEdgePressed) {
        if (m_bWidgetResizable && m_pressCursor.m_bOnEdges) {
            resizeWidget(event->globalPos());
        }
//        else if (d->m_bWidgetMovable && m_bLeftButtonTitlePressed)
//        {
//            moveWidget(event->globalPos());
//        }
    } else if (m_bWidgetResizable) {
        updateCursorShape(event->globalPos());
    }

    if (m_bLeftButtonPressed) {
        QWidget *pWindow = m_mainWidget;

        if (pWindow->isTopLevel()) {
            if (m_bWidgetMaximizable && m_isMaximized) {
                // calculate the valid rect
                QRect rect = validDragRect();
                if (rect.contains(event->pos())) {
                    m_dragPoint = calcDragPoint(pWindow, event);
                    m_movePoint = event->globalPos() - m_dragPoint;
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
}

void LiteBarPrivate::handleLeaveEvent(QEvent *event)
{
    Q_UNUSED(event);
    if (!m_bEdgePressed) {
        m_mainWidget->unsetCursor();
    }
}

void LiteBarPrivate::handleHoverMoveEvent(QHoverEvent *event)
{
    if (!m_bEdgePressed && m_bWidgetResizable) {
        updateCursorShape(m_mainWidget->mapToGlobal(event->pos()));
    }
}

void LiteBarPrivate::handleMouseDblClickEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        m_bLeftButtonDbClicked = true;
        if (m_bWidgetMaximizable) {
            if (m_isMaximized) {
                m_dragPoint = calcDragPoint(m_mainWidget, event);
                m_mousePoint = event->globalPos();
                m_movePoint = m_mousePoint - m_dragPoint;
            }
            emit m_maximizeAction->triggered();
        }
    }
}

void LiteBarPrivate::restoreWidget(QWidget *pWidget)
{
    m_isMaximized = false;
    pWidget->setGeometry(m_normalRect);
    emit q->sigWidgetResizable(true);
}

void LiteBarPrivate::maximizeWidget(QWidget *pWidget)
{
    m_isMaximized = true;
    int x = pWidget->frameGeometry().x() + pWidget->frameGeometry().width() / 2;
    LiteScreen sreen;
    m_currentScreen = sreen.currentScreen(x);
    QRect rect = sreen.screenRect(m_currentScreen);
    pWidget->setGeometry(rect);
    emit q->sigWidgetResizable(false);
}

void LiteBarPrivate::resizeWidget(const QPoint &gMousePos)
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
            if (left != origRect.left()) {
                newRect.setLeft(origRect.left());
            } else {
                newRect.setRight(origRect.right());
            }
        }

        if (minHeight > newRect.height()) {
            if (top != origRect.top()) {
                newRect.setTop(origRect.top());
            } else {
                newRect.setBottom(origRect.bottom());
            }
        }

        m_mainWidget->setGeometry(newRect);
    }
}

void LiteBarPrivate::moveWidget(const QPoint &gMousePos)
{
    m_mainWidget->move(gMousePos - m_dragPos);
}

void LiteBarPrivate::doubleMoveWidget()
{
    if (m_mousePoint.isNull()) {
        return;
    }
    LiteScreen screen;
    QRect rect = screen.screenRect(m_currentScreen);

    int offset = m_moveCursor.m_nBorderWidth;
    rect.setWidth(rect.width() - offset);
    rect.setHeight(rect.height() - offset);
    rect.setX(rect.x() + offset);
    rect.setY(rect.y() + offset);

    QPoint point = m_mousePoint - m_movePoint;
    if (rect.contains(m_mousePoint)) {
        offset = 0;
    }

    point.setY(point.y() + 2 * offset);
    point.setX(point.x() + 2 * offset);
    m_mainWidget->move(point);

    m_mousePoint.setX(0);
    m_mousePoint.setY(0);
}

void LiteBarPrivate::updateCursorShape(const QPoint &gMousePos)
{
    if (m_mainWidget->isFullScreen() || m_mainWidget->isMaximized() || m_isMaximized) {
        if (m_bCursorShapeChanged) {
            m_mainWidget->unsetCursor();
        }
        return;
    }

    m_moveCursor.recalculate(gMousePos, m_mainWidget->frameGeometry());

    if (m_moveCursor.m_bOnTopLeftEdge || m_moveCursor.m_bOnBottomRightEdge) {
        m_mainWidget->setCursor( Qt::SizeFDiagCursor );
        m_bCursorShapeChanged = true;
    } else if (m_moveCursor.m_bOnTopRightEdge || m_moveCursor.m_bOnBottomLeftEdge) {
        m_mainWidget->setCursor( Qt::SizeBDiagCursor );
        m_bCursorShapeChanged = true;
    } else if (m_moveCursor.m_bOnLeftEdge || m_moveCursor.m_bOnRightEdge) {
        m_mainWidget->setCursor( Qt::SizeHorCursor );
        m_bCursorShapeChanged = true;
    } else if (m_moveCursor.m_bOnTopEdge || m_moveCursor.m_bOnBottomEdge) {
        m_mainWidget->setCursor( Qt::SizeVerCursor );
        m_bCursorShapeChanged = true;
    } else {
        if (m_bCursorShapeChanged) {
            m_mainWidget->unsetCursor();
            m_bCursorShapeChanged = false;
        }
    }
}

/******************************************************************************
 *   (screenX, screenY)
 *         \        mouse
 *          +-------*---------------------
 *        / |                |           |
 *   point  |                |orgHeight  |
 *          |                |           |
 *          |----------------|           |   maxHeight
 *          |      orgWidth              |
 *          |                            |
 *          |                            |
 *          ------------------------------
 *                  maxWidth
 *
 * ****************************************************************************
 *
 *   (screenX, screenY)
 *         \                        mouse
 *          +------------+----------*-----
 *          |          / |               |
 *          |      point |orgHeight      |
 *          |            |               |
 *          |            ----------------| maxHeight
 *          |               orgWidth     |
 *          |                            |
 *          |                            |
 *          ------------------------------
 *                  maxWidth
 *
 ******************************************************************************/
QPoint LiteBarPrivate::calcDragPoint(QWidget *pWindow, QMouseEvent *event) const
{
    int mouseX = event->globalX();
    int mouseY = event->globalY();
    LiteScreen screen;
    QRect rect = screen.screenRect(m_currentScreen);
    int maxWidth = rect.x() + rect.width();
    int maxHeight = rect.y() + rect.height();
    int screenX = rect.x();
    int screenY = rect.y();
    int orgWidth = m_normalRect.width();
    int orgHeight = m_normalRect.height();

    if (orgWidth == 0) {
        orgWidth = pWindow->minimumWidth();
    }
    if (orgHeight == 0) {
        orgHeight = pWindow->minimumHeight();
    }

    QPoint point;

    if (mouseX - screenX < orgWidth / 2) {
        point.setX(screenX);
    } else if (maxWidth - mouseX < orgWidth / 2) {
        point.setX(maxWidth - orgWidth);
    } else {
        point.setX(mouseX - orgWidth / 2);
    }

    if (mouseY - screenY < orgHeight / 2) {
        point.setY(screenY);
    } else if (maxHeight - mouseY < orgHeight / 2) {
        point.setY(maxHeight - orgHeight);
    } else {
        point.setY(mouseY - orgHeight / 2);
    }

    return point;
}

QRect LiteBarPrivate::validDragRect()
{
    LiteScreen screen;
    QRect rect = screen.screenRect(m_currentScreen);

    int offset = 2 * m_moveCursor.m_nBorderWidth;
    rect.setWidth(rect.width() - rect.x() - offset);
    rect.setHeight(rect.height() - rect.y() - offset);
    rect.setX(offset);
    rect.setY(offset);

    return rect;
}

void LiteBarPrivate::slotSysButtonClicked()
{
    QAction *pAction = qobject_cast<QAction *>(sender());
    QWidget *pWindow = m_mainWidget;

    if (pWindow == nullptr) {
        return;
    }
    if (pAction == m_minimizeAction) {
        pWindow->showMinimized();
        m_isMinimized = true;
    } else if (pAction == m_maximizeAction) {
        if (m_isMaximized) {
            restoreWidget(pWindow);
            // double-click on the maximized window, need to move it to the mouse
            doubleMoveWidget();
        } else {
            m_normalRect = pWindow->frameGeometry();
            maximizeWidget(pWindow);
        }

        m_isMinimized = false;
    } else if (pAction == m_closeAction) {
        pWindow->close();
    }
}

LiteBar::LiteBar(QWidget *parent)
    : QObject(parent), d(new LiteBarPrivate())
{
    Q_ASSERT(parent);

    d->q = this;
    d->installWidget(parent);
    d->init();
    parent->installEventFilter(this);
}

LiteBar::~LiteBar()
{
    delete d;
}

QWidget *LiteBar::mainWidget() const
{
    return d->m_mainWidget;
}

QLabel *LiteBar::titleLabel() const
{
    return d->m_titleLabel;
}

QToolButton *LiteBar::logoButton() const
{
    return d->m_logoButton;
}

QWidget *LiteBar::sysToolBar() const
{
    return d->m_toolBar;
}

void LiteBar::addAction(QAction *action)
{
    d->m_toolBar->insertAction(d->m_minimizeAction, action);
}

void LiteBar::setCloseIcon(QIcon &icon)
{
    d->m_closeAction->setIcon(icon);
}

void LiteBar::setMaximizeIcon(QIcon &icon)
{
    d->m_maximizeAction->setIcon(icon);
}

void LiteBar::setMinimizeIcon(QIcon &icon)
{
    d->m_minimizeAction->setIcon(icon);
}

void LiteBar::setWidgetResizable(bool resizable)
{
    d->m_bWidgetResizable = resizable;
}

void LiteBar::setWidgetMovable(bool movable)
{
    d->m_bWidgetMovable = movable;
}

void LiteBar::setWidgetMaximizable(bool maximizable)
{
    d->m_bWidgetMaximizable = maximizable;
}

void LiteBar::updateWidgetFlags()
{
    d->m_windowFlags = d->m_mainWidget->windowFlags();
    d->updateWindowButtons();
}

bool LiteBar::eventFilter(QObject *object, QEvent *event)
{
    switch (event->type()) {
        case QEvent::WindowTitleChange: {
            if (d->windowTitleChange(object)) {
                return true;
            }
            break;
        }
        case QEvent::WindowIconChange: {
            if (d->windowIconChange(object)) {
                return true;
            }
            break;
        }
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


