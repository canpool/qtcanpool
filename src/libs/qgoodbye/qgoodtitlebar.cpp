/**
 * Copyright (C) 2023 maminjie <canpool@163.com> (https://github.com/canpool)
 * Copyright (C) 2018-2023 Antonio Dias (https://github.com/antonypro)
 * SPDX-License-Identifier: MIT
 **/

#include "qgoodtitlebar.h"

#include <QApplication>
#include <QStyle>
#include <QHBoxLayout>
#include <QStackedLayout>
#include <QGridLayout>
#include <QTimer>
#include <QStyleFactory>

QGoodTitleBar::QGoodTitleBar(QGoodWindow *gw, QWidget *parent) : QFrame(parent)
{
    m_layoutSpacing = 0;

    // Get Layout Spacing
    {
        QString old_style;

        if (!qApp->style()->objectName().startsWith("fusion")) {
            old_style = qApp->style()->objectName();
            qApp->setStyle(QStyleFactory::create("fusion"));
        }

        QWidget widget;

        QHBoxLayout *layout = new QHBoxLayout(&widget);

        m_layoutSpacing = layout->spacing();

        if (!old_style.isEmpty()) {
            qApp->setStyle(QStyleFactory::create(old_style));
        }
    }

    m_gw = gw;

    style = QString("QGoodTitleBar {background-color: %0; border: none;}");

    connect(qGoodStateHolder, &QGoodStateHolder::currentThemeChanged, this, &QGoodTitleBar::setTheme);

    setFixedHeight(29);

    m_iconWidget = new QGoodIconWidget(this);
    m_iconWidget->setFixedWidth(29);

    m_titleWidget = new QGoodTitleWidget(this, this);

    m_captionButtons = new QWidget(this);
    m_captionButtons->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);

    m_minBtn = new QGoodCaptionButton(QGoodCaptionButton::IconType::Minimize, m_captionButtons);

    m_restoreBtn = new QGoodCaptionButton(QGoodCaptionButton::IconType::Restore, m_captionButtons);

    m_maxBtn = new QGoodCaptionButton(QGoodCaptionButton::IconType::Maximize, m_captionButtons);

    m_clsBtn = new QGoodCaptionButton(QGoodCaptionButton::IconType::Close, m_captionButtons);

    connect(m_minBtn, &QGoodCaptionButton::clicked, this, &QGoodTitleBar::showMinimized);
    connect(m_restoreBtn, &QGoodCaptionButton::clicked, this, &QGoodTitleBar::showNormal);
    connect(m_maxBtn, &QGoodCaptionButton::clicked, this, &QGoodTitleBar::showMaximized);
    connect(m_clsBtn, &QGoodCaptionButton::clicked, this, &QGoodTitleBar::closeWindow);

    QHBoxLayout *hlayout = new QHBoxLayout(m_captionButtons);
    hlayout->setContentsMargins(0, 0, 0, 0);
    hlayout->setSpacing(0);

    hlayout->addWidget(m_minBtn);
    hlayout->addWidget(m_restoreBtn);
    hlayout->addWidget(m_maxBtn);
    hlayout->addWidget(m_clsBtn);

    m_captionButtons->adjustSize();

    QStackedLayout *stacked_layout = new QStackedLayout(this);
    stacked_layout->setStackingMode(QStackedLayout::StackAll);

    QWidget *widget = new QWidget(this);

    QHBoxLayout *layout = new QHBoxLayout(widget);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);

    stacked_layout->addWidget(widget);
    stacked_layout->addWidget(m_titleWidget);

    m_leftWidgetPlaceHolder = new QWidget(this);
    m_leftWidgetPlaceHolder->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
    m_leftWidgetPlaceHolder->setVisible(false);
    m_leftWidgetPlaceHolder->setEnabled(false);

    m_rightWidgetPlaceHolder = new QWidget(this);
    m_rightWidgetPlaceHolder->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
    m_rightWidgetPlaceHolder->setVisible(false);
    m_rightWidgetPlaceHolder->setEnabled(false);

    m_centerWidgetPlaceHolder = new QWidget(this);
    m_centerWidgetPlaceHolder->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
    m_centerWidgetPlaceHolder->setVisible(false);
    m_centerWidgetPlaceHolder->setEnabled(false);

    m_centerSpacerItemLeft = new QSpacerItem(0, 0);
    m_centerSpacerItemRight = new QSpacerItem(0, 0);

    layout->addWidget(m_iconWidget);
    layout->addWidget(m_leftWidgetPlaceHolder);
    layout->addStretch();
    layout->addSpacerItem(m_centerSpacerItemLeft);
    layout->addWidget(m_centerWidgetPlaceHolder);
    layout->addSpacerItem(m_centerSpacerItemRight);
    layout->addStretch();
    layout->addWidget(m_rightWidgetPlaceHolder);
    layout->addWidget(m_captionButtons);

    QTimer::singleShot(0, this, [=] {
        // All caption button starts in hidden state,
        // showing them as needed.

        if (isMinimizedButtonEnabled()) {
            m_minBtn->setVisible(true);
        }

        if (isMaximizeButtonEnabled()) {
            if (!isMaximized())
                m_maxBtn->setVisible(true);
            else
                m_restoreBtn->setVisible(true);
        }

        m_clsBtn->setVisible(true);

        m_captionButtons->adjustSize();
    });

    m_active = true;

    m_isMaximized = false;

    setTheme();
}

void QGoodTitleBar::setTitle(const QString &title)
{
    m_titleWidget->setText(title);
}

void QGoodTitleBar::setIcon(const QPixmap &icon)
{
    m_iconWidget->setPixmap(icon);
}

void QGoodTitleBar::setActive(bool active)
{
    m_active = active;

    m_iconWidget->setActive(active);
    m_titleWidget->setActive(active);
    m_minBtn->setActive(active);
    m_restoreBtn->setActive(active);
    m_maxBtn->setActive(active);
    m_clsBtn->setActive(active);
}

void QGoodTitleBar::setTitleAlignment(const Qt::Alignment &alignment)
{
    m_titleWidget->setTitleAlignment(alignment);
}

void QGoodTitleBar::setMaximized(bool maximized)
{
    m_isMaximized = maximized;

    if (!isMaximizeButtonEnabled())
        return;

    if (maximized) {
        m_maxBtn->setVisible(false);
        m_restoreBtn->setVisible(true);
    } else {
        m_restoreBtn->setVisible(false);
        m_maxBtn->setVisible(true);
    }

    m_captionButtons->adjustSize();
}

void QGoodTitleBar::setTheme()
{
    bool dark = qGoodStateHolder->isCurrentThemeDark();

    setAttribute(Qt::WA_TranslucentBackground, false);

    if (dark) {
        QTimer::singleShot(0, this, [=] {
            if (m_titleBarColor == QColor(Qt::transparent))
                setAttribute(Qt::WA_TranslucentBackground, true);
            else if (m_titleBarColor.isValid())
                setStyleSheet(style.arg(m_titleBarColor.name()));
            else if (qApp->style()->objectName().startsWith("fusion"))
                setStyleSheet(style.arg(qApp->palette().base().color().name()));
            else
                setStyleSheet(style.arg("#000000"));
        });

        // Light mode to contrast
        m_minBtn->setIconMode(false);
        m_maxBtn->setIconMode(false);
        m_restoreBtn->setIconMode(false);
        m_clsBtn->setIconMode(false);

        m_titleWidget->setTitleColor(QColor(255, 255, 255), QColor(150, 150, 150));

        setActive(m_active);
    } else {
        QTimer::singleShot(0, this, [=] {
            if (m_titleBarColor == QColor(Qt::transparent))
                setAttribute(Qt::WA_TranslucentBackground, true);
            else if (m_titleBarColor.isValid())
                setStyleSheet(style.arg(m_titleBarColor.name()));
            else if (qApp->style()->objectName().startsWith("fusion"))
                setStyleSheet(style.arg(qApp->palette().base().color().name()));
            else
                setStyleSheet(style.arg("#FFFFFF"));
        });

        // Dark mode to contrast
        m_minBtn->setIconMode(true);
        m_maxBtn->setIconMode(true);
        m_restoreBtn->setIconMode(true);
        m_clsBtn->setIconMode(true);

        m_titleWidget->setTitleColor(QColor(0, 0, 0), QColor(150, 150, 150));

        setActive(m_active);
    }
}

void QGoodTitleBar::setLeftTitleBarWidget(QWidget *widget)
{
    if (m_leftWidgetPlaceHolder->layout())
        delete m_leftWidgetPlaceHolder->layout();

    m_leftWidget = widget;

    if (m_leftWidget) {
        QGridLayout *layout = new QGridLayout(m_leftWidgetPlaceHolder);
        layout->setContentsMargins(0, 0, m_layoutSpacing, 0);
        layout->setSpacing(0);
        layout->addWidget(m_leftWidget);

        m_leftWidgetPlaceHolder->setVisible(true);
        m_leftWidgetPlaceHolder->setEnabled(true);
    } else {
        m_leftWidgetPlaceHolder->setVisible(false);
        m_leftWidgetPlaceHolder->setEnabled(false);
    }
}

void QGoodTitleBar::setRightTitleBarWidget(QWidget *widget)
{
    if (m_rightWidgetPlaceHolder->layout())
        delete m_rightWidgetPlaceHolder->layout();

    m_rightWidget = widget;

    if (m_rightWidget) {
        QGridLayout *layout = new QGridLayout(m_rightWidgetPlaceHolder);
        layout->setContentsMargins(0, 0, m_layoutSpacing, 0);
        layout->setSpacing(0);
        layout->addWidget(m_rightWidget);

        m_rightWidgetPlaceHolder->setVisible(true);
        m_rightWidgetPlaceHolder->setEnabled(true);
    } else {
        m_rightWidgetPlaceHolder->setVisible(false);
        m_rightWidgetPlaceHolder->setEnabled(false);
    }
}

void QGoodTitleBar::setCenterTitleBarWidget(QWidget *widget)
{
    if (m_centerWidgetPlaceHolder->layout())
        delete m_centerWidgetPlaceHolder->layout();

    m_centerWidget = widget;

    if (m_centerWidget) {
        QGridLayout *layout = new QGridLayout(m_centerWidgetPlaceHolder);
        layout->setContentsMargins(0, 0, m_layoutSpacing, 0);
        layout->setSpacing(0);
        layout->addWidget(m_centerWidget);

        m_centerWidgetPlaceHolder->setVisible(true);
        m_centerWidgetPlaceHolder->setEnabled(true);
    } else {
        m_centerWidgetPlaceHolder->setVisible(false);
        m_centerWidgetPlaceHolder->setEnabled(false);
    }
}

void QGoodTitleBar::setCaptionButtonWidth(int width)
{
    m_minBtn->setFixedSize(width, height());
    m_restoreBtn->setFixedSize(width, height());
    m_maxBtn->setFixedSize(width, height());
    m_clsBtn->setFixedSize(width, height());

    m_captionButtons->adjustSize();
}

Qt::Alignment QGoodTitleBar::titleAlignment()
{
    return m_titleWidget->titleAlignment();
}

int QGoodTitleBar::captionButtonsWidth()
{
    return m_captionButtons->width();
}

int QGoodTitleBar::leftWidth()
{
    QRect rect;
    rect = rect.united(m_iconWidget->geometry());
    rect = rect.united(m_leftWidgetPlaceHolder->geometry());
    return rect.width();
}

int QGoodTitleBar::rightWidth()
{
    QRect rect;
    rect = rect.united(m_captionButtons->geometry());
    rect = rect.united(m_rightWidgetPlaceHolder->geometry());
    int width = rect.width() + layoutSpacing();
    return width;
}

int QGoodTitleBar::layoutSpacing()
{
    return m_layoutSpacing;
}

bool QGoodTitleBar::isMinimizedButtonEnabled()
{
    if (!m_gw)
        return false;

    return (m_gw->windowFlags() & Qt::WindowMinimizeButtonHint);
}

bool QGoodTitleBar::isMaximizeButtonEnabled()
{
    if (!m_gw)
        return false;

    return (m_gw->windowFlags() & Qt::WindowMaximizeButtonHint);
}

bool QGoodTitleBar::isCloseButtonEnabled()
{
    if (!m_gw)
        return false;

    return (m_gw->windowFlags() & Qt::WindowCloseButtonHint);
}

QRect QGoodTitleBar::minimizeButtonRect()
{
    if (m_minBtn->isVisible())
        return m_minBtn->geometry();

    return QRect();
}

QRect QGoodTitleBar::maximizeButtonRect()
{
    if (m_maxBtn->isVisible())
        return m_maxBtn->geometry();
    if (m_restoreBtn->isVisible())
        return m_restoreBtn->geometry();

    return QRect();
}

QRect QGoodTitleBar::closeButtonRect()
{
    if (m_clsBtn->isVisible())
        return m_clsBtn->geometry();

    return QRect();
}

void QGoodTitleBar::updateWindow()
{
    int left_width = leftWidth();
    int right_width = rightWidth();

    int distance = right_width - left_width;
    int left_distance = 0;
    int right_distance = 0;

    if (distance > 0)
        left_distance = qAbs(distance);
    else
        right_distance = qAbs(distance);

    m_centerSpacerItemLeft->changeSize(left_distance, 0, QSizePolicy::Preferred, QSizePolicy::Expanding);
    m_centerSpacerItemRight->changeSize(right_distance, 0, QSizePolicy::Preferred, QSizePolicy::Expanding);
}

void QGoodTitleBar::captionButtonStateChanged(const QGoodWindow::CaptionButtonState &state)
{
    if (!m_captionButtons->isVisible())
        return;

    switch (state) {
        // Hover enter
    case QGoodWindow::CaptionButtonState::MinimizeHoverEnter: {
        m_minBtn->setState(QEvent::HoverEnter);
        break;
    }
    case QGoodWindow::CaptionButtonState::MaximizeHoverEnter: {
        if (!m_isMaximized)
            m_maxBtn->setState(QEvent::HoverEnter);
        else
            m_restoreBtn->setState(QEvent::HoverEnter);
        break;
    }
    case QGoodWindow::CaptionButtonState::CloseHoverEnter: {
        m_clsBtn->setState(QEvent::HoverEnter);
        break;
    }
        // Hover leave
    case QGoodWindow::CaptionButtonState::MinimizeHoverLeave: {
        m_minBtn->setState(QEvent::HoverLeave);
        break;
    }
    case QGoodWindow::CaptionButtonState::MaximizeHoverLeave: {
        if (!m_isMaximized)
            m_maxBtn->setState(QEvent::HoverLeave);
        else
            m_restoreBtn->setState(QEvent::HoverLeave);
        break;
    }
    case QGoodWindow::CaptionButtonState::CloseHoverLeave: {
        m_clsBtn->setState(QEvent::HoverLeave);
        break;
    }
    // Mouse button press
    case QGoodWindow::CaptionButtonState::MinimizePress: {
        m_minBtn->setState(QEvent::MouseButtonPress);
        break;
    }
    case QGoodWindow::CaptionButtonState::MaximizePress: {
        if (!m_isMaximized)
            m_maxBtn->setState(QEvent::MouseButtonPress);
        else
            m_restoreBtn->setState(QEvent::MouseButtonPress);
        break;
    }
    case QGoodWindow::CaptionButtonState::ClosePress: {
        m_clsBtn->setState(QEvent::MouseButtonPress);
        break;
    }
    // Mouse button release
    case QGoodWindow::CaptionButtonState::MinimizeRelease: {
        m_minBtn->setState(QEvent::MouseButtonRelease);
        break;
    }
    case QGoodWindow::CaptionButtonState::MaximizeRelease: {
        if (!m_isMaximized)
            m_maxBtn->setState(QEvent::MouseButtonRelease);
        else
            m_restoreBtn->setState(QEvent::MouseButtonRelease);
        break;
    }
    case QGoodWindow::CaptionButtonState::CloseRelease: {
        m_clsBtn->setState(QEvent::MouseButtonRelease);
        break;
    }
        // Mouse button clicked
    case QGoodWindow::CaptionButtonState::MinimizeClicked: {
        Q_EMIT m_minBtn->clicked();
        break;
    }
    case QGoodWindow::CaptionButtonState::MaximizeClicked: {
        if (!m_isMaximized)
            Q_EMIT m_maxBtn->clicked();
        else
            Q_EMIT m_restoreBtn->clicked();
        break;
    }
    case QGoodWindow::CaptionButtonState::CloseClicked: {
        Q_EMIT m_clsBtn->clicked();
        break;
    }
    default:
        break;
    }
}
