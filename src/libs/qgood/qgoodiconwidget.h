/**
 * Copyright (C) 2023 maminjie <canpool@163.com> (https://github.com/canpool)
 * Copyright (C) 2018-2023 Antonio Dias (https://github.com/antonypro)
 * SPDX-License-Identifier: MIT
 **/

#ifndef QGOODICONWIDGET_H
#define QGOODICONWIDGET_H

#include <QWidget>

class QGoodIconWidget : public QWidget
{
    Q_OBJECT
public:
    explicit QGoodIconWidget(QWidget *parent = nullptr);

public Q_SLOTS:
    void setPixmap(const QPixmap &pixmap);
    void setActive(bool active);
private:
    void paintEvent(QPaintEvent *event);
private:
    QPixmap m_pixmap;
    QPixmap m_grayed_pixmap;
    bool m_active;
};

#endif   // QGOODICONWIDGET_H
