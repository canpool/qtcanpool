/**
 * Copyright (C) 2023 maminjie <canpool@163.com> (https://github.com/canpool)
 * Copyright (C) 2022-2023 Antonio Dias (https://github.com/antonypro)
 * Copyright (C) 2018, Juergen Skrotzky (https://github.com/Jorgen-VikingGod, JorgenVikingGod@gmail.com)
 * SPDX-License-Identifier: MIT
 **/

#ifndef QGOODSTYLE_H
#define QGOODSTYLE_H

#include <QProxyStyle>

class QGoodStyle : public QProxyStyle
{
    Q_OBJECT
public:
    enum StyleType {
        LightStyle,
        DarkStyle,
    };
public:
    explicit QGoodStyle(StyleType type = LightStyle);
    explicit QGoodStyle(QStyle *style, StyleType type = LightStyle);
    ~QGoodStyle();
public:
    QStyle *styleBase() const;

    QIcon standardIcon(StandardPixmap standardPixmap, const QStyleOption *option, const QWidget *widget) const;
    void polish(QPalette &palette);
    void polish(QApplication *app);
    void unpolish(QApplication *app);

    void setStyleType(StyleType type);
    StyleType styleType() const;
private:
    QStyle *baseStyle() const;
private:
#ifdef Q_OS_WIN
    QHash<StandardPixmap, QPixmap> m_hashPixmapCache;
#endif
    StyleType m_type;
};

#endif   // QGOODSTYLE_H
