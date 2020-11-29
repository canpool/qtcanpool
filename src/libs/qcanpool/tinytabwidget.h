/***************************************************************************
 **
 **  Copyright (C) 2018-2020 MaMinJie <canpool@163.com>
 **  Contact: https://github.com/canpool
 **           https://gitee.com/icanpool
 **
 **  GNU Lesser General Public License Usage
 **  Alternatively, this file may be used under the terms of the GNU Lesser
 **  General Public License version 3 as published by the Free Software
 **  Foundation and appearing in the file LICENSE.LGPL3 included in the
 **  packaging of this file. Please review the following information to
 **  ensure the GNU Lesser General Public License version 3 requirements
 **  will be met: https://www.gnu.org/licenses/lgpl-3.0.html.
 **
 **  GNU General Public License Usage
 **  Alternatively, this file may be used under the terms of the GNU
 **  General Public License version 2.0 or (at your option) the GNU General
 **  Public license version 3 or any later version approved by the KDE Free
 **  Qt Foundation. The licenses are as published by the Free Software
 **  Foundation and appearing in the file LICENSE.GPL2 and LICENSE.GPL3
 **  included in the packaging of this file. Please review the following
 **  information to ensure the GNU General Public License requirements will
 **  be met: https://www.gnu.org/licenses/gpl-2.0.html and
 **  https://www.gnu.org/licenses/gpl-3.0.html.
 **
****************************************************************************/
#ifndef TINYTABWIDGET_H
#define TINYTABWIDGET_H

#include <QWidget>
#include "qcanpool_global.h"

QCANPOOL_BEGIN_NAMESPACE

class TinyTabWidgetPrivate;
class QCANPOOL_SHARED_EXPORT TinyTabWidget : public QWidget
{
    Q_OBJECT
public:
    explicit TinyTabWidget(QWidget *parent = nullptr);
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

QCANPOOL_END_NAMESPACE

#endif // TINYTABWIDGET_H
