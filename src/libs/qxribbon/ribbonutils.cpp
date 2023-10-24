/**
 * Copyleft (C) 2023 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#include "ribbonutils.h"
#include <QPainter>

namespace QxRibbon {

QIcon colorIcon(const QColor &color, const QSize &size)
{
    QPixmap res(size);
    res.fill(Qt::transparent);
    QPainter painter(&res);
    if (color.isValid()) {
        painter.fillRect(QRect(1, 1, res.height() - 2, res.width() - 2), color);
    } else {
        QPen pen(Qt::black, 1, Qt::SolidLine, Qt::RoundCap);
        painter.setPen(pen);
        painter.drawRect(QRect(1, 1, res.height() - 2, res.width() - 2));
        pen.setColor(Qt::red);
        painter.setPen(pen);
        painter.setRenderHint(QPainter::SmoothPixmapTransform, true);
        painter.setRenderHint(QPainter::Antialiasing, true);
        painter.drawLine(QPoint(1, size.height()), QPoint(size.width() - 1, 1));
    }
    return QIcon(res);
}

} // namespace QxRibbon
