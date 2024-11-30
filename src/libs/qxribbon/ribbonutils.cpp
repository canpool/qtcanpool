/**
 * Copyleft (C) 2023 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MIT
 **/
#include "ribbonutils.h"
#include <QPainter>

namespace QxRibbon {

QIcon colorIcon(const QColor &color, const QSize &size)
{
    QPixmap res(size);
    res.fill(Qt::transparent);
    QPainter painter(&res);
    QRect pr(1, 1, res.height() - 2, res.width() - 2);
    if (color.isValid()) {
        painter.fillRect(pr, color);
    } else {
        QPen pen(Qt::black, 1, Qt::SolidLine, Qt::RoundCap);
        painter.setPen(pen);
        painter.drawRect(pr);
        pen.setColor(Qt::red);
        painter.setPen(pen);
        painter.setRenderHint(QPainter::SmoothPixmapTransform, true);
        painter.setRenderHint(QPainter::Antialiasing, true);
        painter.drawLine(QPoint(1, size.height()), QPoint(size.width() - 1, 1));
    }
    return QIcon(res);
}

} // namespace QxRibbon
