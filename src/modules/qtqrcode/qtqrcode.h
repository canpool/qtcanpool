/**
 * Copyright (C) 2018-2023 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
**/
#ifndef QTQRCODE_H
#define QTQRCODE_H

#include <QObject>
#include <QPixmap>

class QtQrcode : public QObject
{
    Q_OBJECT
public:
    explicit QtQrcode(QObject *parent = 0);

    QPixmap createQRCode(const QString &text, int width = 48, const QColor &foreground = QColor(Qt::black),
                         const QColor &background = QColor(Qt::white));
};

#endif   // QTQRCODE_H
