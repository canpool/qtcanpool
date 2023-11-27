/**
 * Copyright (C) 2023 maminjie <canpool@163.com> (https://github.com/canpool)
 * Copyright (C) 2018-2023 Antonio Dias (https://github.com/antonypro)
 * SPDX-License-Identifier: MIT
 **/

#ifndef QGOODCAPTIONBUTTON_H
#define QGOODCAPTIONBUTTON_H

#include <QWidget>

class QGoodCaptionButton : public QWidget
{
    Q_OBJECT
public:
    enum class IconType {
        Minimize,
        Restore,
        Maximize,
        Close
    };

    explicit QGoodCaptionButton(IconType type, QWidget *parent = nullptr);
    ~QGoodCaptionButton();

Q_SIGNALS:
    void clicked();

public Q_SLOTS:
    void setIconMode(bool iconDark);
    void setActive(bool isActive);
    void setState(int state);
private:
    QPixmap loadSVG(const QString &svgPath, int w, int h);
    void paintIcons(const QPixmap &pixIn, bool dark, QPixmap *pixActiveOut, QPixmap *pixInactiveOut);
    void drawIcons();
    void setColors();
    void paintEvent(QPaintEvent *event);
private:
    QPixmap m_inactiveIcon;
    QPixmap m_activeIcon;
    QPixmap m_closeIconHover;

    QColor m_normal;
    QColor m_hover;
    QColor m_pressed;

    IconType m_type;
    bool m_isActive;
    bool m_isUnderMouse;
    bool m_isPressed;
    bool m_iconDark;
};

#endif   // QGOODCAPTIONBUTTON_H
