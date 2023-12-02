/**
 * Copyright (C) 2023 maminjie <canpool@163.com> (https://github.com/canpool)
 * Copyright (C) 2018-2023 Antonio Dias (https://github.com/antonypro)
 * SPDX-License-Identifier: MIT
 **/

#ifndef QGOODTITLEWIDGET_H
#define QGOODTITLEWIDGET_H

#include <QWidget>
#include <QPointer>

class QGoodTitleBar;

class QGoodTitleWidget : public QWidget
{
    Q_OBJECT
public:
    explicit QGoodTitleWidget(QGoodTitleBar *titleBar, QWidget *parent = nullptr);

public Q_SLOTS:
    void setText(const QString &text);
    void setActive(bool active);
    void setTitleAlignment(const Qt::Alignment &alignment);
    void setTitleColor(const QColor &activeColor, const QColor &inactiveColor);
    Qt::Alignment titleAlignment();
private:
    void paintEvent(QPaintEvent *event);
private:
    QPointer<QGoodTitleBar> m_titleBar;
    QString m_title;
    bool m_active;
    Qt::Alignment m_alignment;
    QColor m_activeColor;
    QColor m_inactiveColor;
};

#endif   // QGOODTITLEWIDGET_H
