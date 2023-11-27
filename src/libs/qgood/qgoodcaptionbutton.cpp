/**
 * Copyright (C) 2023 maminjie <canpool@163.com> (https://github.com/canpool)
 * Copyright (C) 2018-2023 Antonio Dias (https://github.com/antonypro)
 * SPDX-License-Identifier: MIT
 **/

#include "qgoodcaptionbutton.h"

#include <QPainter>
#include <QIcon>
#include <QEvent>

QGoodCaptionButton::QGoodCaptionButton(IconType type, QWidget *parent) : QWidget(parent)
{
    setVisible(false);

    m_type = type;
    m_isActive = false;
    m_isUnderMouse = false;
    m_isPressed = false;
    m_iconDark = false;

    setColors();
    drawIcons();
}

QGoodCaptionButton::~QGoodCaptionButton()
{
}

QPixmap QGoodCaptionButton::loadSVG(const QString &svgPath, int w, int h)
{
    QPixmap pix = QIcon(svgPath).pixmap(w, h);
    return pix;
}

void QGoodCaptionButton::paintIcons(const QPixmap &pixIn, bool dark, QPixmap *pixActiveOut, QPixmap *pixInactiveOut)
{
    QImage img_active = pixIn.toImage();
    QImage img_inactive = img_active;

    const qreal grayed_reduction = qreal(0.40);

    for (int y = 0; y < img_inactive.height(); y++) {
        QRgb *pixel_ptr = reinterpret_cast<QRgb *>(img_inactive.scanLine(y));

        for (int x = 0; x < img_inactive.width(); x++) {
            QRgb pixel = pixel_ptr[x];

            QRgb rgba = qRgba(qRound(qRed(pixel) * grayed_reduction), qRound(qGreen(pixel) * grayed_reduction),
                              qRound(qBlue(pixel) * grayed_reduction), qAlpha(pixel));

            pixel_ptr[x] = rgba;
        }
    }

    if (dark) {
        img_active.invertPixels();
        img_inactive.invertPixels();
    }

    if (pixActiveOut)
        *pixActiveOut = QPixmap::fromImage(img_active);

    if (pixInactiveOut)
        *pixInactiveOut = QPixmap::fromImage(img_inactive);
}

void QGoodCaptionButton::drawIcons()
{
    const int size = 10;

    const int w = size;
    const int h = size;

    switch (m_type) {
    case IconType::Minimize: {
        QPixmap icon = loadSVG(":/res/minimize.svg", w, h);
        paintIcons(icon, m_iconDark, &m_activeIcon, &m_inactiveIcon);
        break;
    }
    case IconType::Restore: {
        QPixmap icon = loadSVG(":/res/restore.svg", w, h);
        paintIcons(icon, m_iconDark, &m_activeIcon, &m_inactiveIcon);
        break;
    }
    case IconType::Maximize: {
        QPixmap icon = loadSVG(":/res/maximize.svg", w, h);
        paintIcons(icon, m_iconDark, &m_activeIcon, &m_inactiveIcon);
        break;
    }
    case IconType::Close: {
        QPixmap icon = loadSVG(":/res/close.svg", w, h);
        paintIcons(icon, m_iconDark, &m_activeIcon, &m_inactiveIcon);
        if (m_iconDark)
            paintIcons(icon, false /*dark*/, &m_closeIconHover, nullptr);

        break;
    }
    }
}

void QGoodCaptionButton::setColors()
{
    if (m_iconDark) {
        if (m_type == IconType::Close) {
            m_normal = QColor("transparent");
            m_hover = QColor("#F00000");
            m_pressed = QColor("#F1707A");
        } else {
            m_normal = QColor("transparent");
            m_hover = QColor("#E5E5E5");
            m_pressed = QColor("#CACACB");
        }
    } else {
        if (m_type == IconType::Close) {
            m_normal = QColor("transparent");
            m_hover = QColor("#F00000");
            m_pressed = QColor("#F1707A");
        } else {
            m_normal = QColor("transparent");
            m_hover = QColor("#505050");
            m_pressed = QColor("#3F3F3F");
        }
    }

    update();
}

void QGoodCaptionButton::setIconMode(bool iconDark)
{
    m_iconDark = iconDark;

    drawIcons();
    setColors();

    update();
}

void QGoodCaptionButton::setActive(bool isActive)
{
    m_isActive = isActive;
    update();
}

void QGoodCaptionButton::setState(int state)
{
    switch (state) {
    case QEvent::HoverEnter: {
        m_isUnderMouse = true;
        update();
        break;
    }
    case QEvent::HoverLeave: {
        m_isUnderMouse = false;
        update();
        break;
    }
    case QEvent::MouseButtonPress: {
        m_isPressed = true;
        m_isUnderMouse = true;
        update();
        break;
    }
    case QEvent::MouseButtonRelease: {
        m_isPressed = false;
        m_isUnderMouse = false;
        update();
        break;
    }
    default:
        break;
    }
}

void QGoodCaptionButton::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event)

    QPixmap current_icon = m_activeIcon;
    QColor current_color = m_normal;

    // Change icon if needed
    if (m_isUnderMouse) {
        if (m_type == IconType::Close && m_iconDark)
            current_icon = m_closeIconHover;
    } else {
        if (!m_isActive)
            current_icon = m_inactiveIcon;
    }

    // Change background color if needed
    if (m_isPressed) {
        if (m_isUnderMouse)
            current_color = m_pressed;
    } else {
        if (m_isUnderMouse)
            current_color = m_hover;
    }

    QPainter painter(this);
    painter.setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);
    painter.setCompositionMode(QPainter::CompositionMode_SourceOver);

    painter.fillRect(rect(), current_color);

    QRect target_rect;
    target_rect = current_icon.rect();
    target_rect.moveCenter(rect().center());
    painter.drawPixmap(target_rect, current_icon);
}
