/**
 * Copyright (C) 2023 maminjie <canpool@163.com> (https://github.com/canpool)
 * Copyright (C) 2022-2023 Antonio Dias (https://github.com/antonypro)
 * SPDX-License-Identifier: MIT
 **/

#include "qgoodcentralwidget.h"
#include "qgooddialog.h"

#ifdef QGOOD_WINDOW_ENABLE
#include "qgoodtitlebar.h"
#define BORDERCOLOR QColor(24, 131, 215)

#include <QFrame>
#include <QStackedLayout>
#include <QTimer>
#endif // QGOOD_WINDOW_ENABLE

#include <QDialog>
#include <QEvent>
#include <QVBoxLayout>
#include <QGridLayout>

QGoodCentralWidget::QGoodCentralWidget(QGoodWindow *gw) : QWidget(gw)
{
    m_gw = gw;
    m_gw->installEventFilter(this);

    m_centralWidgetPlaceHolder = new QWidget(this);

#ifdef QGOOD_WINDOW_ENABLE
    m_leftWidgetTransparentForMouse = false;
    m_rightWidgetTransparentForMouse = false;
    m_centerWidgetTransparentForMouse = false;

    m_frameStyle = QString("QFrame#GoodFrame {border: %0;}");

    m_activeBorderColor = BORDERCOLOR;

    m_captionButtonWidth = 36;

    m_titleBar = new QGoodTitleBar(m_gw, this);
    m_titleBar->setCaptionButtonWidth(m_captionButtonWidth);
    m_titleBar->installEventFilter(this);

    connect(m_gw, &QGoodWindow::captionButtonStateChanged, m_titleBar, &QGoodTitleBar::captionButtonStateChanged);

    connect(m_titleBar, &QGoodTitleBar::showMinimized, m_gw, &QGoodWindow::showMinimized);
    connect(m_titleBar, &QGoodTitleBar::showNormal, m_gw, &QGoodWindow::showNormal);
    connect(m_titleBar, &QGoodTitleBar::showMaximized, m_gw, &QGoodWindow::showMaximized);
    connect(m_titleBar, &QGoodTitleBar::closeWindow, m_gw, &QGoodWindow::close);

    connect(m_gw, &QGoodWindow::windowTitleChanged, m_titleBar, [=](const QString &title) {
        m_titleBar->setTitle(title);
    });

    connect(m_gw, &QGoodWindow::windowIconChanged, m_titleBar, [=](const QIcon &icon) {
        if (!icon.isNull()) {
            const int pix_size = 16;
            m_titleBar->setIcon(icon.pixmap(pix_size, pix_size));
        }
    });

    m_drawBorders = !QGoodWindow::shouldBordersBeDrawnBySystem();

    m_titleBarVisible = true;
    m_captionButtonsVisible = true;
    m_titleVisible = true;
    m_iconVisible = true;
    m_iconWidth = 0;

    m_frame = new QFrame(this);
    m_frame->setObjectName("GoodFrame");

    setUnifiedTitleBarAndCentralWidget(false);

    QVBoxLayout *centralLayout = new QVBoxLayout(this);
    centralLayout->setContentsMargins(0, 0, 0, 0);
    centralLayout->setSpacing(0);
    centralLayout->addWidget(m_frame);
#else
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);
    mainLayout->addWidget(m_centralWidgetPlaceHolder);
#endif
}

QGoodCentralWidget::~QGoodCentralWidget()
{
}

int QGoodCentralWidget::execDialogWithWindow(QDialog *dialog, QGoodWindow *parentGw, QGoodCentralWidget *baseGcw,
                                             QWidget *leftTitleBarWidget, QWidget *rightTitleBarWidget,
                                             bool titleVisible, bool iconVisible)
{
#ifdef QGOOD_WINDOW_ENABLE
    dialog->setWindowFlags(Qt::Widget);

    QGoodWindow *gw = new QGoodWindow(parentGw);
    gw->setAttribute(Qt::WA_DeleteOnClose);

    QGoodCentralWidget *gcw = new QGoodCentralWidget(gw);
    gcw->setCentralWidget(dialog);

    gcw->setLeftTitleBarWidget(leftTitleBarWidget);
    gcw->setRightTitleBarWidget(rightTitleBarWidget);

    if (baseGcw) {
        if (baseGcw->titleBarColor() != QColor(Qt::transparent))
            gcw->setTitleBarColor(baseGcw->titleBarColor());
        gcw->setActiveBorderColor(baseGcw->activeBorderColor());
        gcw->setTitleAlignment(baseGcw->titleAlignment());
    }

    gcw->setTitleVisible(titleVisible);
    gcw->setIconVisible(iconVisible);

    gw->setCentralWidget(gcw);

    return QGoodDialog::exec(dialog, gw, parentGw);
#else
    Q_UNUSED(parentGw)
    Q_UNUSED(baseGcw)
    Q_UNUSED(leftTitleBarWidget)
    Q_UNUSED(rightTitleBarWidget)
    Q_UNUSED(titleVisible)
    Q_UNUSED(iconVisible)
    return dialog->exec();
#endif
}

void QGoodCentralWidget::setUnifiedTitleBarAndCentralWidget(bool unified)
{
#ifdef QGOOD_WINDOW_ENABLE
    if (m_frame->layout())
        delete m_frame->layout();

    if (!unified) {
        m_unifiedTitleBarAndCentralWidget = false;
        setTitleBarColor(QColor());

        QVBoxLayout *mainLayout = new QVBoxLayout(m_frame);
        mainLayout->setContentsMargins(0, 0, 0, 0);
        mainLayout->setSpacing(0);
        mainLayout->addWidget(m_titleBar);
        mainLayout->addWidget(m_centralWidgetPlaceHolder);
    } else {
        setTitleBarColor(QColor(Qt::transparent));
        m_unifiedTitleBarAndCentralWidget = true;

        QStackedLayout *mainLayout = new QStackedLayout(m_frame);
        mainLayout->setStackingMode(QStackedLayout::StackAll);
        mainLayout->setContentsMargins(0, 0, 0, 0);
        mainLayout->setSpacing(0);
        mainLayout->addWidget(m_titleBar);
        mainLayout->addWidget(m_centralWidgetPlaceHolder);
    }
#else
    Q_UNUSED(unified)
#endif
}

void QGoodCentralWidget::setTitleBarMask(const QRegion &mask)
{
#ifdef QGOOD_WINDOW_ENABLE
    m_titleBarMask = mask;
    updateWindow();
#else
    Q_UNUSED(mask)
#endif
}

QWidget *QGoodCentralWidget::setLeftTitleBarWidget(QWidget *widget, bool transparentForMouse)
{
#ifdef QGOOD_WINDOW_ENABLE
    QWidget *return_widget = m_titleBarLeftWidget;

    m_titleBarLeftWidget = widget;

    m_leftWidgetTransparentForMouse = transparentForMouse;

    m_titleBar->setLeftTitleBarWidget(m_titleBarLeftWidget);

    updateWindow();

    return return_widget;
#else
    Q_UNUSED(widget)
    Q_UNUSED(transparentForMouse)
    return nullptr;
#endif
}

QWidget *QGoodCentralWidget::setRightTitleBarWidget(QWidget *widget, bool transparentForMouse)
{
#ifdef QGOOD_WINDOW_ENABLE
    QWidget *return_widget = m_titleBarRightWidget;

    m_titleBarRightWidget = widget;

    m_rightWidgetTransparentForMouse = transparentForMouse;

    m_titleBar->setRightTitleBarWidget(m_titleBarRightWidget);

    updateWindow();

    return return_widget;
#else
    Q_UNUSED(widget)
    Q_UNUSED(transparentForMouse)
    return nullptr;
#endif
}

QWidget *QGoodCentralWidget::setCenterTitleBarWidget(QWidget *widget, bool transparentForMouse)
{
#ifdef QGOOD_WINDOW_ENABLE
    QWidget *return_widget = m_titleBarCenterWidget;

    m_titleBarCenterWidget = widget;

    m_centerWidgetTransparentForMouse = transparentForMouse;

    m_titleBar->setCenterTitleBarWidget(m_titleBarCenterWidget);

    updateWindow();

    return return_widget;
#else
    Q_UNUSED(widget)
    Q_UNUSED(transparentForMouse)
    return nullptr;
#endif
}

void QGoodCentralWidget::setCentralWidget(QWidget *widget)
{
    if (!m_centralWidgetPlaceHolder)
        return;

    m_centralWidget = widget;

    if (m_centralWidgetPlaceHolder->layout())
        delete m_centralWidgetPlaceHolder->layout();

    QGridLayout *layout = new QGridLayout(m_centralWidgetPlaceHolder);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);
    layout->addWidget(m_centralWidget);
}

void QGoodCentralWidget::setTitleAlignment(const Qt::Alignment &alignment)
{
#ifdef QGOOD_WINDOW_ENABLE
    m_titleBar->setTitleAlignment(alignment);
#else
    Q_UNUSED(alignment)
#endif
}

void QGoodCentralWidget::setTitleBarColor(const QColor &color)
{
#ifdef QGOOD_WINDOW_ENABLE
    if (m_unifiedTitleBarAndCentralWidget)
        return;

    m_titleBarColor = color;
    m_titleBar->m_titleBarColor = m_titleBarColor;
    m_titleBar->setTheme();
#else
    Q_UNUSED(color)
#endif
}

void QGoodCentralWidget::setActiveBorderColor(const QColor &color)
{
#ifdef QGOOD_WINDOW_ENABLE
    if (color.isValid())
        m_activeBorderColor = color;
    else
        m_activeBorderColor = BORDERCOLOR;

    updateWindow();
#else
    Q_UNUSED(color)
#endif
}

void QGoodCentralWidget::setTitleBarVisible(bool visible)
{
#ifdef QGOOD_WINDOW_ENABLE
    m_titleBarVisible = visible;
    m_titleBar->setVisible(m_titleBarVisible);
    m_titleBar->setEnabled(m_titleBarVisible);
    updateWindow();
#else
    Q_UNUSED(visible)
#endif
}

void QGoodCentralWidget::setCaptionButtonsVisible(bool visible)
{
#ifdef QGOOD_WINDOW_ENABLE
    m_captionButtonsVisible = visible;
    m_titleBar->m_captionButtons->setVisible(m_captionButtonsVisible);
    m_titleBar->m_captionButtons->setEnabled(m_captionButtonsVisible);
    updateWindow();
#else
    Q_UNUSED(visible)
#endif
}

void QGoodCentralWidget::setTitleVisible(bool visible)
{
#ifdef QGOOD_WINDOW_ENABLE
    m_titleVisible = visible;
    m_titleBar->m_titleWidget->setVisible(m_titleVisible);
    m_titleBar->m_titleWidget->setEnabled(m_titleVisible);
    updateWindow();
#else
    Q_UNUSED(visible)
#endif
}

void QGoodCentralWidget::setIconVisible(bool visible)
{
#ifdef QGOOD_WINDOW_ENABLE
    m_iconVisible = visible;
    if (m_iconVisible)
        m_iconWidth = 0;
    m_titleBar->m_iconWidget->setVisible(m_iconVisible);
    m_titleBar->m_iconWidget->setEnabled(m_iconVisible);
    updateWindow();
#else
    Q_UNUSED(visible)
#endif
}

void QGoodCentralWidget::setIconWidth(int width)
{
#ifdef QGOOD_WINDOW_ENABLE
    m_iconWidth = width;
    m_iconVisible = (m_iconWidth == 0);
    m_titleBar->m_iconWidget->setVisible(m_iconVisible);
    m_titleBar->m_iconWidget->setEnabled(m_iconVisible);
    updateWindow();
#else
    Q_UNUSED(width)
#endif
}

void QGoodCentralWidget::setTitleBarHeight(int height)
{
#ifdef QGOOD_WINDOW_ENABLE
    m_titleBar->setFixedHeight(height);
    m_titleBar->setCaptionButtonWidth(m_captionButtonWidth);
    updateWindow();
#else
    Q_UNUSED(height)
#endif
}

void QGoodCentralWidget::setCaptionButtonWidth(int width)
{
#ifdef QGOOD_WINDOW_ENABLE
    m_captionButtonWidth = width;
    m_titleBar->setCaptionButtonWidth(m_captionButtonWidth);
    updateWindow();
#else
    Q_UNUSED(width)
#endif
}

bool QGoodCentralWidget::isUnifiedTitleBarAndCentralWidget() const
{
#ifdef QGOOD_WINDOW_ENABLE
    return m_unifiedTitleBarAndCentralWidget;
#else
    return false;
#endif
}

QWidget *QGoodCentralWidget::leftTitleBarWidget() const
{
#ifdef QGOOD_WINDOW_ENABLE
    return m_titleBarLeftWidget;
#else
    return nullptr;
#endif
}

QWidget *QGoodCentralWidget::rightTitleBarWidget() const
{
#ifdef QGOOD_WINDOW_ENABLE
    return m_titleBarRightWidget;
#else
    return nullptr;
#endif
}

QWidget *QGoodCentralWidget::centerTitleBarWidget() const
{
#ifdef QGOOD_WINDOW_ENABLE
    return m_titleBarCenterWidget;
#else
    return nullptr;
#endif
}

QWidget *QGoodCentralWidget::centralWidget() const
{
    return m_centralWidget;
}

Qt::Alignment QGoodCentralWidget::titleAlignment() const
{
#ifdef QGOOD_WINDOW_ENABLE
    return m_titleBar->titleAlignment();
#else
    return Qt::Alignment(0);
#endif
}

QColor QGoodCentralWidget::titleBarColor() const
{
#ifdef QGOOD_WINDOW_ENABLE
    return m_titleBarColor;
#else
    return QColor();
#endif
}

QColor QGoodCentralWidget::activeBorderColor() const
{
#ifdef QGOOD_WINDOW_ENABLE
    return m_activeBorderColor;
#else
    return QColor();
#endif
}

bool QGoodCentralWidget::isTitleBarVisible() const
{
#ifdef QGOOD_WINDOW_ENABLE
    return m_titleBar->isVisible();
#else
    return true;
#endif
}

bool QGoodCentralWidget::isCaptionButtonsVisible() const
{
#ifdef QGOOD_WINDOW_ENABLE
    return m_captionButtonsVisible;
#else
    return true;
#endif
}

bool QGoodCentralWidget::isTitleVisible() const
{
#ifdef QGOOD_WINDOW_ENABLE
    return m_titleVisible;
#else
    return true;
#endif
}

bool QGoodCentralWidget::isIconVisible() const
{
#ifdef QGOOD_WINDOW_ENABLE
    return m_iconVisible;
#else
    return true;
#endif
}

int QGoodCentralWidget::iconWidth() const
{
#ifdef QGOOD_WINDOW_ENABLE
    return m_iconWidth;
#else
    return 0;
#endif
}

int QGoodCentralWidget::titleBarHeight() const
{
#ifdef QGOOD_WINDOW_ENABLE
    return m_titleBar->height();
#else
    return 0;
#endif
}

int QGoodCentralWidget::captionButtonWidth() const
{
#ifdef QGOOD_WINDOW_ENABLE
    return m_captionButtonWidth;
#else
    return 0;
#endif
}

void QGoodCentralWidget::updateWindow()
{
#ifdef QGOOD_WINDOW_ENABLE
    if (!m_gw)
        return;

    if (!m_gw->isVisible() || m_gw->isMinimized())
        return;

    bool window_active = m_gw->isActiveWindow();
    bool window_no_state = m_gw->windowState().testFlag(Qt::WindowNoState);
    bool draw_borders = m_drawBorders;
    bool is_maximized = m_gw->isMaximized();
    bool is_full_screen = m_gw->isFullScreen();
    int title_bar_width = m_titleBar->width();
    int title_bar_height = m_titleBar->height();
    int icon_width = m_titleBar->m_iconWidget->width();

    QString border_str = "none";

    if (draw_borders && window_no_state) {
        if (window_active)
            border_str = QString("1px solid %0").arg(m_activeBorderColor.name());
        else
            border_str = "1px solid #AAAAAA";
    }

    m_frame->setStyleSheet(m_frameStyle.arg(border_str));

    m_titleBar->setMaximized(is_maximized && !is_full_screen);

    m_titleBar->setVisible(m_titleBarVisible && !is_full_screen);

    if (!is_full_screen) {
        m_gw->setTitleBarHeight(m_titleBarVisible ? title_bar_height : 0);

        if (m_iconWidth > 0)
            m_gw->setIconWidth(m_iconWidth);
        else if (m_iconVisible)
            m_gw->setIconWidth(icon_width);
        else
            m_gw->setIconWidth(0);

        QRegion left_mask;
        QRegion right_mask;
        QRegion center_mask;

        if (m_titleBarLeftWidget) {
            QWidgetList list;

            if (!m_leftWidgetTransparentForMouse)
                list.append(m_titleBarLeftWidget);

            list.append(m_titleBarLeftWidget->findChildren<QWidget *>());

            for (QWidget *widget : list) {
                if (!widget->testAttribute(Qt::WA_TransparentForMouseEvents)) {
                    if (!widget->mask().isNull()) {
                        left_mask += widget->mask().translated(m_titleBar->m_leftWidgetPlaceHolder->pos());
                    } else {
                        QRect geom = widget->geometry();

                        if (geom.width() > m_titleBarLeftWidget->width())
                            geom.setWidth(m_titleBarLeftWidget->width());

                        left_mask += geom.translated(m_titleBar->m_leftWidgetPlaceHolder->pos());
                    }
                }
            }
        }

        if (m_titleBarRightWidget) {
            QWidgetList list;

            if (!m_rightWidgetTransparentForMouse)
                list.append(m_titleBarRightWidget);

            list.append(m_titleBarRightWidget->findChildren<QWidget *>());

            for (QWidget *widget : list) {
                if (!widget->testAttribute(Qt::WA_TransparentForMouseEvents)) {
                    if (!widget->mask().isNull()) {
                        right_mask += widget->mask().translated(m_titleBar->m_rightWidgetPlaceHolder->pos());
                    } else {
                        QRect geom = widget->geometry();

                        if (geom.width() > m_titleBarRightWidget->width())
                            geom.setWidth(m_titleBarRightWidget->width());

                        right_mask += geom.translated(m_titleBar->m_rightWidgetPlaceHolder->pos());
                    }
                }
            }
        }

        if (m_titleBarCenterWidget) {
            QWidgetList list;

            if (!m_centerWidgetTransparentForMouse)
                list.append(m_titleBarCenterWidget);

            list.append(m_titleBarCenterWidget->findChildren<QWidget *>());

            for (QWidget *widget : list) {
                if (!widget->testAttribute(Qt::WA_TransparentForMouseEvents)) {
                    if (!widget->mask().isNull()) {
                        center_mask += widget->mask().translated(m_titleBar->m_centerWidgetPlaceHolder->pos());
                    } else {
                        QRect geom = widget->geometry();

                        if (geom.width() > m_titleBarCenterWidget->width())
                            geom.setWidth(m_titleBarCenterWidget->width());

                        center_mask += geom.translated(m_titleBar->m_centerWidgetPlaceHolder->pos());
                    }
                }
            }
        }

        QRegion title_bar_mask;

        if (!m_titleBarMask.isEmpty())
            title_bar_mask = m_titleBarMask;
        else
            title_bar_mask -= m_gw->titleBarRect();

        title_bar_mask += left_mask;
        title_bar_mask += center_mask;
        title_bar_mask += right_mask;

        m_gw->setTitleBarMask(title_bar_mask);

        if (m_captionButtonsVisible) {
            QRect min_rect = m_titleBar->minimizeButtonRect();
            QRect max_rect = m_titleBar->maximizeButtonRect();
            QRect cls_rect = m_titleBar->closeButtonRect();

            min_rect.moveLeft(title_bar_width - cls_rect.width() - max_rect.width() - min_rect.width());
            max_rect.moveLeft(title_bar_width - cls_rect.width() - max_rect.width());
            cls_rect.moveLeft(title_bar_width - cls_rect.width());

            m_gw->setMinimizeMask(min_rect);
            m_gw->setMaximizeMask(max_rect);
            m_gw->setCloseMask(cls_rect);
        }
    }

    m_titleBar->setActive(window_active);

    m_titleBar->updateWindow();
#endif
}

bool QGoodCentralWidget::eventFilter(QObject *watched, QEvent *event)
{
#ifdef QGOOD_WINDOW_ENABLE
    if (watched == m_gw) {
        switch (event->type()) {
        case QEvent::Show:
        case QEvent::Resize: {
            QTimer::singleShot(0, this, &QGoodCentralWidget::updateWindow);
            break;
        }
        case QEvent::WindowStateChange:
        case QEvent::WindowActivate:
        case QEvent::WindowDeactivate: {
            updateWindow();
            break;
        }
        default:
            break;
        }
    } else if (watched == m_titleBar) {
        switch (event->type()) {
        case QEvent::Show: {
            QTimer::singleShot(0, this, &QGoodCentralWidget::updateWindow);
            break;
        }
        default:
            break;
        }
    }
#endif
    return QWidget::eventFilter(watched, event);
}
