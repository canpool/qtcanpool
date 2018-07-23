/***************************************************************************
 **
 **  Copyright (C) 2018 MaMinJie <canpool@163.com>
 **  Contact: https://github.com/canpool
 **
 **  This program is free software: you can redistribute it and/or modify
 **  it under the terms of the GNU General Public License as published by
 **  the Free Software Foundation, either version 3 of the License, or
 **  (at your option) any later version.
 **
 **  This program is distributed in the hope that it will be useful,
 **  but WITHOUT ANY WARRANTY; without even the implied warranty of
 **  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 **  GNU General Public License for more details.
 **
 **  You should have received a copy of the GNU General Public License
 **  along with this program.  If not, see http://www.gnu.org/licenses/.
 **
****************************************************************************/
#ifndef TINYTABWIDGET_H
#define TINYTABWIDGET_H

#include <QWidget>
#include "qcanpool_global.h"

class TinyTabWidgetPrivate;
class QCANPOOL_SHARED_EXPORT TinyTabWidget : public QWidget
{
    Q_OBJECT
public:
    explicit TinyTabWidget(QWidget *parent = 0);
    ~TinyTabWidget();

    int addTab(QWidget *page, const QString &label);
    int addTab(QWidget *page, const QIcon &icon, const QString &label);

    enum TabPosition { North, South, West, East };
    Q_ENUM(TabPosition)
    TabPosition tabPosition() const;
    void setTabPosition(TabPosition pos);

    void setHoverColor(const QColor &color);
    QColor hoverColor() const;

    void setPressColor(const QColor &color);
    QColor pressColor() const;

    void setTextColor(const QColor &color);
    void setSelectedTextColor(const QColor &color);

    void setBackgroundColor(const QColor &color);
    void setNormalColor(const QColor &color);
    void setSlideColor(const QColor &color);

    void setTabSpace(int space);

    int currentIndex() const;
    QWidget* currentWidget() const;
    QWidget* widget(int index);

signals:
    void currentChanged(int index);

public slots:
    void setCurrentIndex(int index);
    void setCurrentWidget(QWidget *widget);

private:
    TinyTabWidgetPrivate *d;
};

#endif // TINYTABWIDGET_H
