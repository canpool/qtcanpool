/**
 * Copyright (C) 2023 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#include "minitabbar.h"

#include <QStyle>
#include <QStylePainter>
#include <QStyleOptionTab>

QX_WIDGET_BEGIN_NAMESPACE

inline static bool verticalTabs(QTabBar::Shape shape)
{
    return shape == QTabBar::RoundedWest
           || shape == QTabBar::RoundedEast
           || shape == QTabBar::TriangularWest
           || shape == QTabBar::TriangularEast;
}

inline static bool leftVerticalTabs(QTabBar::Shape shape)
{
    return shape == QTabBar::RoundedWest
           || shape == QTabBar::TriangularWest;
}

/* MiniTabBar */
MiniTabBar::MiniTabBar(QWidget *parent)
    : QTabBar{parent}
{

}

MiniTabBar::~MiniTabBar()
{

}

QSize MiniTabBar::tabSizeHint(int index) const
{
    QSize s = QTabBar::tabSizeHint(index);
    if (verticalTabs(shape())) {
        s.transpose();
    }
    return s;
}

void MiniTabBar::paintEvent(QPaintEvent *event)
{
    if (verticalTabs(shape())) {
        QStylePainter painter(this);
        QStyleOptionTab opt;

        for (int i = 0; i < count(); i++) {
#if QT_VERSION >= QT_VERSION_CHECK(5, 15, 0)
            if (!isTabVisible(i)) {
                continue;
            }
#endif
            initStyleOption(&opt, i);
            painter.drawControl(QStyle::CE_TabBarTabShape, opt);

            painter.save();

            QSize s = opt.rect.size();
            s.transpose();
            QRect r(QPoint(), s);
            r.moveCenter(opt.rect.center());
            opt.rect = r;

            QPoint c = tabRect(i).center();
            painter.translate(c);
            if (leftVerticalTabs(shape())) {
                painter.rotate(90);
            } else {
                painter.rotate(-90);
            }
            painter.translate(-c);
            painter.drawControl(QStyle::CE_TabBarTabLabel, opt);

            painter.restore();
        }
    } else {
        QTabBar::paintEvent(event);
    }
}

QX_WIDGET_END_NAMESPACE
