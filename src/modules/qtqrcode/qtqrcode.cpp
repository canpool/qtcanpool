/**
 * Copyright (C) 2018-2023 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#include "qtqrcode.h"
#include "qrencode.h"

QtQrcode::QtQrcode(QObject *parent) : QObject(parent) {}

QPixmap QtQrcode::createQRCode(const QString &text, int width, const QColor &foreground, const QColor &background)
{
    if (text.trimmed().isEmpty() || width <= 0) {
        return QPixmap();
    }
    QRcode *qrcode = QRcode_encodeString(text.toLocal8Bit(), 2, QR_ECLEVEL_L, QR_MODE_8, 0);
    if (qrcode == NULL) {
        return QPixmap();
    }
    const int margin = 1;
    int realwidth = qrcode->width;
    QImage image = QImage(realwidth + 2 * margin, realwidth + 2 * margin, QImage::Format_Indexed8);
    image.setColor(0, background.rgb());
    image.setColor(1, foreground.rgb());
    image.fill(0);

    for (int x = 0; x < realwidth; ++x) {
        for (int y = 0; y < realwidth; ++y) {
            unsigned char b = qrcode->data[x * realwidth + y];
            if (b & 0x01) {
                image.setPixel(y + margin, x + margin, 1);
            }
        }
    }
    return QPixmap::fromImage(image.scaledToWidth(width));
}
