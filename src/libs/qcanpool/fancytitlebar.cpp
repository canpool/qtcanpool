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
#include "fancytitlebar.h"
#include "fancybutton.h"
#include "fancycursor.h"
#include "fancyscreen.h"

#include <QLabel>
#include <QHBoxLayout>
#include <QMenu>
#include <QAction>
#include <QMouseEvent>
#include <QEvent>
#include <QPoint>
#include <QApplication>
#include <QWidget>

QCANPOOL_BEGIN_NAMESPACE

class FancyTitleBarPrivate : public QObject
{
    Q_OBJECT
public:
    explicit FancyTitleBarPrivate();
    ~FancyTitleBarPrivate();

    void init();
    void installWidget(QWidget *w);

    void setWidgetResizable(bool resizable);
    void setWidgetMovable(bool movable);
    void setWidgetMaximizable(bool maximizable);

    // mouse event
    void handleWidgetMouseEvent(QObject *obj, QEvent *event);

    void updateWindowButtons();

    void setIconDark(bool dark);

public:
    // window
    bool windowTitleChange(QObject *obj);
    bool windowIconChange(QObject *obj);
    void windowSizeChange(QObject *obj);
    void windowStateChange(QObject *obj);

private:
    // mouse event
    void handleMousePressEvent(QMouseEvent *event);
    void handleMouseReleaseEvent(QMouseEvent *event);
    void handleMouseMoveEvent(QMouseEvent *event);
    void handleLeaveEvent(QEvent *event);
    void handleHoverMoveEvent(QHoverEvent *event);
    void handleMouseDblClickEvent(QMouseEvent *event);

    // widget action
    void restoreWidget(QWidget *pWidget);
    void maximizeWidget(QWidget *pWidget);
    void resizeWidget(const QPoint &gMousePos);

    // mouse shape
    void updateCursorShape(const QPoint &gMousePos);
    QPoint calcStartPoint(QWidget *pWindow, QMouseEvent *event) const;

    QRect validDragRect();

public:
    // Simulate mouse events
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseDoubleClickEvent(QMouseEvent *event);

private slots:
    void slotSysButtonClicked();
    void aboutToShowSystemMenu();
    void aboutToHideSystemMenu();
    void systemMenuTriggered(QMouseEvent *e);

public:
    FancyTitleBar   *q;
    QWidget         *m_mainWidget;
    QWidget         *m_titleWidget;

    // title widget
    FancyButton     *m_logoButton;
    QLabel          *m_titleLabel;
    QWidget         *m_systemGroup;
    FancyButton     *m_maximizeButton;
    FancyButton     *m_minimizeButton;
    FancyButton     *m_closeButton;

    QMenu           *m_menu;
    QAction         *m_maximizeAction;
    QAction         *m_minimizeAction;
    QAction         *m_restoreAction;
    QAction         *m_closeAction;

    // 0:white, 1:black
    QIcon           m_closeIcon[2];
    QIcon           m_minimizeIcon[2];
    QIcon           m_maximizeIcon[2];
    QIcon           m_normalIcon[2];
    bool            m_bDarkIcon;

    // main window
    Qt::WindowFlags m_windowFlags;
    FancyCursor     m_pressCursor;
    FancyCursor     m_moveCursor;
    QRect           m_normalRect;
    int             m_currentScreen;
    bool            m_bEdgePressed;
    bool            m_bLeftButtonPressed;
    bool            m_bLeftButtonDbClicked;
    bool            m_bLeftButtonTitlePressed;
    bool            m_bCursorShapeChanged;
    bool            m_isMaximized;
    bool            m_isMinimized;
    bool            m_bWidgetMaximizable;
    bool            m_bWidgetResizable;
    bool            m_bWidgetMovable;

    QPoint          m_movePoint;
};

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
    m_closeIcon[0] = QIcon(":/main/close");
    m_minimizeIcon[0] = QIcon(":/main/min");
    m_maximizeIcon[0] = QIcon(":/main/max");
    m_normalIcon[0] = QIcon(":/main/restore");
    m_closeIcon[1] = QIcon(":/main/close2");
    m_minimizeIcon[1] = QIcon(":/main/min2");
    m_maximizeIcon[1] = QIcon(":/main/max2");
    m_normalIcon[1] = QIcon(":/main/restore2");
    m_bDarkIcon = false;
}

FancyTitleBarPrivate::~FancyTitleBarPrivate()
{}

void FancyTitleBarPrivate::init()
{
    m_logoButton = new FancyButton(m_mainWidget);
    m_logoButton->setHasMenu(true);
    m_logoButton->setIcon(QIcon(":/main/logo"));
    connect(m_logoButton, SIGNAL(menuTriggered(QMouseEvent *)), this, SLOT(systemMenuTriggered(QMouseEvent *)));

    m_titleLabel = new QLabel(m_mainWidget);
    QFont font("Microsoft Yahei", 10);
    m_titleLabel->setFont(font);
    m_titleLabel->setAlignment(Qt::AlignCenter);
    m_titleLabel->setStyleSheet("color:white;");

    m_minimizeButton = new FancyButton(m_mainWidget);
    m_minimizeButton->setIcon(m_minimizeIcon[0]);
    m_minimizeButton->setToolTip(tr("minimize"));
    connect(m_minimizeButton, SIGNAL(clicked(bool)), this, SLOT(slotSysButtonClicked()));

    m_maximizeButton = new FancyButton(m_mainWidget);
    m_maximizeButton->setIcon(m_maximizeIcon[0]);
    m_maximizeButton->setToolTip(tr("maximize"));
    connect(m_maximizeButton, SIGNAL(clicked(bool)), this, SLOT(slotSysButtonClicked()));

    m_closeButton = new FancyButton(m_mainWidget);
    m_closeButton->setIcon(m_closeIcon[0]);
    m_closeButton->setToolTip(tr("close"));
    connect(m_closeButton, SIGNAL(clicked(bool)), this, SLOT(slotSysButtonClicked()));
    m_closeButton->setHoverColor(QColor(207, 0, 0, 230));
    m_closeButton->setPressColor(QColor(207, 0, 0, 150));

    m_menu = new QMenu(m_mainWidget);
    connect(m_menu, SIGNAL(aboutToShow()), this, SLOT(aboutToShowSystemMenu()));
    connect(m_menu, SIGNAL(aboutToHide()), this, SLOT(aboutToHideSystemMenu()));

    m_maximizeAction = new QAction(m_maximizeIcon[1], tr("maximize"), this);
    m_minimizeAction = new QAction(m_minimizeIcon[1], tr("minimize"), this);
    m_restoreAction = new QAction(m_normalIcon[1], tr("restore"), this);
    m_closeAction = new QAction(m_closeIcon[1], tr("close"), this);
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

    m_systemGroup = new QWidget(m_mainWidget);
    QHBoxLayout *systemButtonLayout = new QHBoxLayout();
    systemButtonLayout->setMargin(0);
    systemButtonLayout->setSpacing(0);
    systemButtonLayout->addWidget(m_minimizeButton);
    systemButtonLayout->addWidget(m_maximizeButton);
    systemButtonLayout->addWidget(m_closeButton);
    m_systemGroup->setLayout(systemButtonLayout);

    setIconDark(false);
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
    m_maximizeButton->setVisible(m_windowFlags & Qt::WindowMaximizeButtonHint);
    m_minimizeButton->setVisible(m_windowFlags & Qt::WindowMinimizeButtonHint);

    m_maximizeAction->setEnabled(m_windowFlags & Qt::WindowMaximizeButtonHint);
    m_minimizeAction->setEnabled(m_windowFlags & Qt::WindowMinimizeButtonHint);

    m_restoreAction->setEnabled(false);

    setWidgetMaximizable(m_windowFlags & Qt::WindowMaximizeButtonHint);
}

void FancyTitleBarPrivate::setIconDark(bool dark)
{
    m_bDarkIcon = dark;

    int index = dark ? 1 : 0;

    m_minimizeButton->setIcon(m_minimizeIcon[index]);
    m_closeButton->setIcon(m_closeIcon[index]);
    if (m_isMaximized) {
        m_maximizeButton->setIcon(m_normalIcon[index]);
    } else {
        m_maximizeButton->setIcon(m_maximizeIcon[index]);
    }
}

bool FancyTitleBarPrivate::windowTitleChange(QObject *obj)
{
    QWidget *pWidget = qobject_cast<QWidget *>(obj);

    if (pWidget) {
        QFont font = QApplication::font();
        font.setPointSize(11);
        m_titleLabel->setText(pWidget->windowTitle());
        m_titleLabel->setFont(font);
        return true;
    }
    return false;
}

bool FancyTitleBarPrivate::windowIconChange(QObject *obj)
{
    QWidget *pWidget = qobject_cast<QWidget *>(obj);

    if (pWidget) {
        QIcon icon = pWidget->windowIcon();
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
    QWidget *pWindow = qobject_cast<QWidget *>(obj);

    if (pWindow) {
        if (pWindow->isMaximized()) {
            // Mask the maximized state, because the frameless window
            // will cover the windows taskbar when maximized
            pWindow->setWindowState(pWindow->windowState() & ~Qt::WindowMaximized);
            this->maximizeWidget(pWindow);
        }

        windowStateChange(obj);
    }
}

void FancyTitleBarPrivate::windowStateChange(QObject *obj)
{
    Q_UNUSED(obj);

    if (m_isMaximized) {
        m_maximizeButton->setToolTip(tr("restore"));
        m_maximizeButton->setProperty("maximizeProperty", "restore");
        m_maximizeButton->setIcon(m_bDarkIcon ? m_normalIcon[1] : m_normalIcon[0]);
    } else {
        m_maximizeButton->setProperty("maximizeProperty", "maximize");
        m_maximizeButton->setToolTip(tr("maximize"));
        m_maximizeButton->setIcon(m_bDarkIcon ? m_maximizeIcon[1] : m_maximizeIcon[0]);
    }

    if (m_windowFlags & Qt::WindowMaximizeButtonHint) {
        m_maximizeAction->setEnabled(!m_isMaximized);
        m_restoreAction->setEnabled(m_isMaximized);
    } else {
        m_maximizeAction->setEnabled(false);
        m_restoreAction->setEnabled(false);
    }
}

void FancyTitleBarPrivate::handleMousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton && !m_isMaximized) {
        m_bEdgePressed = true;
        QRect frameRect = m_mainWidget->frameGeometry();
        m_pressCursor.recalculate(event->globalPos(), frameRect);
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
    int left = oriRect.left();
    int top = oriRect.top();
    int right = oriRect.right();
    int bottom = oriRect.bottom();
    oriRect.getCoords(&left, &top, &right, &bottom);
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

    m_moveCursor.recalculate(gMousePos, m_mainWidget->frameGeometry());

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
 * Calculate the starting position of the window from maximized to normal size
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
 *   point  |                |orgHeight  |
 *          |                |           |
 *          |----------------|           |   maxHeight
 *          |      orgWidth              |
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
 *          |      point |orgHeight      |
 *          |            |               |
 *          |            ----------------| maxHeight
 *          |               orgWidth     |
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
 *      b) maxwidth - oriWidth, when the dragpoint is right half (Align screen right)
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
 */
QPoint FancyTitleBarPrivate::calcStartPoint(QWidget *pWindow, QMouseEvent *event) const
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
    int orgHeight = m_normalRect.height();

    if (oriWidth == 0) {
        oriWidth = pWindow->minimumWidth();
    }
    if (orgHeight == 0) {
        orgHeight = pWindow->minimumHeight();
    }

    QPoint point;
    point.setY(0);

    if (mouseX - screenX < oriWidth / 2) {
        point.setX(screenX); // Align screen left
    } else if (maxWidth - mouseX < oriWidth / 2) {
        point.setX(maxWidth - oriWidth); // Align screen right
    } else {
        point.setX(mouseX - oriWidth / 2);
    }

    if (m_titleWidget == nullptr) {
        if (mouseY - screenY < orgHeight / 2) {
            point.setY(screenY);
        } else if (maxHeight - mouseY < orgHeight / 2) {
            point.setY(maxHeight - orgHeight);
        } else {
            point.setY(mouseY - orgHeight / 2);
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
                m_movePoint = event->globalPos() - calcStartPoint(pWindow, event);
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
                emit m_maximizeButton->click();
            }
        } else {
            int y = m_mainWidget->frameGeometry().y();

            if (y < 0) {
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
                m_movePoint = event->globalPos() - calcStartPoint(pWindow, event);
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
                m_movePoint = event->globalPos() - calcStartPoint(m_mainWidget, event);
            }
            emit m_maximizeButton->click();
        }
    }
}

void FancyTitleBarPrivate::slotSysButtonClicked()
{
    FancyButton *pButton = qobject_cast<FancyButton *>(sender());
    QWidget *pWindow = m_mainWidget;

    if (pWindow) {
        if (pButton == m_minimizeButton) {
            pWindow->showMinimized();
            m_isMinimized = true;
        } else if (pButton == m_maximizeButton) {
            if (m_isMaximized) {
                restoreWidget(pWindow);
            } else {
                m_normalRect = pWindow->frameGeometry();
                maximizeWidget(pWindow);
            }
            m_isMinimized = false;
        } else if (pButton == m_closeButton) {
            pWindow->close();
        }
    }
}

void FancyTitleBarPrivate::aboutToShowSystemMenu()
{
    m_logoButton->select(true);
}

void FancyTitleBarPrivate::aboutToHideSystemMenu()
{
    m_logoButton->select(false);
}

void FancyTitleBarPrivate::systemMenuTriggered(QMouseEvent *e)
{
    FancyButton *button = qobject_cast<FancyButton *>(sender());
    if (button == nullptr) {
        return;
    }

    int x = e->x();
    int y = e->y();
    QPoint pos = e->globalPos();
    pos.setX(pos.x() - x);
    pos.setY(pos.y() - y + button->height());
    m_menu->popup(pos);
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

QLabel *FancyTitleBar::titleLabel() const
{
    return d->m_titleLabel;
}

QToolButton *FancyTitleBar::logoButton() const
{
    return d->m_logoButton;
}

QWidget *FancyTitleBar::systemGroup() const
{
    return d->m_systemGroup;
}

void FancyTitleBar::setCloseIcon(QIcon &icon)
{
    d->m_closeButton->setIcon(icon);
}

void FancyTitleBar::setMaximizeIcon(QIcon &icon)
{
    d->m_maximizeButton->setIcon(icon);
}

void FancyTitleBar::setMinimizeIcon(QIcon &icon)
{
    d->m_minimizeButton->setIcon(icon);
}

void FancyTitleBar::setNormalIcon(QIcon &icon)
{
    d->m_normalIcon[0] = icon;
    if (!d->m_isMaximized) {
        d->m_maximizeButton->setIcon(icon);
    }
}

void FancyTitleBar::setIconDark(bool dark)
{
    d->m_bDarkIcon = dark;
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

QCANPOOL_END_NAMESPACE

#include "fancytitlebar.moc"
