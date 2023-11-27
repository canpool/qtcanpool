/**
 * Copyright (C) 2023 maminjie <canpool@163.com> (https://github.com/canpool)
 * Copyright (C) 2018-2023 Antonio Dias (https://github.com/antonypro)
 * SPDX-License-Identifier: MIT
 **/

#include "qgoodiconwidget.h"

#include <QPainter>

#define ICONWIDTH  16
#define ICONHEIGHT 16

QGoodIconWidget::QGoodIconWidget(QWidget *parent) : QWidget(parent)
{
    m_active = true;
}

void QGoodIconWidget::setPixmap(const QPixmap &pixmap)
{
    m_pixmap = pixmap;

    QImage tmp = m_pixmap.toImage();

    for (int y = 0; y < tmp.height(); y++) {
        QRgb *pixel_ptr = reinterpret_cast<QRgb *>(tmp.scanLine(y));

        for (int x = 0; x < tmp.width(); x++) {
            QRgb pixel = pixel_ptr[x];

            int gray = qGray(pixel);

            int alpha = qAlpha(pixel);

            QRgb rgba = qRgba(gray, gray, gray, alpha);

            pixel_ptr[x] = rgba;
        }
    }

    m_grayed_pixmap = QPixmap::fromImage(tmp);

    update();
}

void QGoodIconWidget::setActive(bool active)
{
    m_active = active;
    update();
}

void QGoodIconWidget::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event)

    QPainter painter(this);
    painter.setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);

    painter.drawPixmap((width() - ICONWIDTH) / 2, (height() - ICONHEIGHT) / 2, ICONWIDTH, ICONHEIGHT,
                       (m_active ? m_pixmap : m_grayed_pixmap));
}
