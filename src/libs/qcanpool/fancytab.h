/***************************************************************************
 **
 **  Copyright (C) 2018-2020 MaMinJie <canpool@163.com>
 **  Contact: https://github.com/canpool
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
#ifndef FANCYTAB_H
#define FANCYTAB_H

#include "fancybutton.h"
#include <QColor>
#include <QIcon>

QCANPOOL_BEGIN_NAMESPACE

class FancyTab : public FancyButton
{
    Q_OBJECT
public:
    enum TabType { Mode, Action };
    explicit FancyTab(QWidget *parent = nullptr);
    FancyTab(const QIcon &icon, QWidget *parent = nullptr);
    ~FancyTab();

    void select(bool selected);

    void setType(TabType type) { m_type = type; }
    TabType type() const { return m_type; }

    void setSelectedTextColor(const QColor &color);

    void setColor(const QColor &color);

private:
    void init(void);
    void painterInfo(const QColor &color);
    void painterArrow(const QColor &color);

protected:
    void enterEvent(QEvent *event);
    void leaveEvent(QEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);

    void paintEvent(QPaintEvent *event);

private:
    void update();

private:
    bool m_bMouseHover;
    bool m_bMousePress;
    QColor m_selectedTextColor;

    TabType m_type;
};

QCANPOOL_END_NAMESPACE

#endif // FANCYTAB_H
