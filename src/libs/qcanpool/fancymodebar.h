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
#ifndef FANCYMODEBAR_H
#define FANCYMODEBAR_H

#include <QWidget>
#include "qcanpool_global.h"

class QStackedWidget;
class FancyMode;
class QAction;
class FancyModeBarPrivate;

class QCANPOOL_SHARED_EXPORT FancyModeBar : public QWidget
{
    Q_OBJECT
    Q_ENUMS(Direction)
public:
    enum Direction { Horizontal, Vertical };
    enum ActionPosition { AP_Front, AP_Middle, AP_Back };
    enum ButtonType { Mode, Action };

    explicit FancyModeBar(QStackedWidget *modeStack, Direction direction = Vertical, QWidget *parent = nullptr);
    ~FancyModeBar();

    void addMode(FancyMode *mode);
    void selectMode(FancyMode *mode);
    void setEnabled(FancyMode *mode, bool enable);
    void setVisible(FancyMode *mode, bool visible);

    void setButtonStyle(ButtonType type, Qt::ToolButtonStyle style);
    void setButtonFont(ButtonType type, QFont &font);
    void setIconSize(QSize size);
    void setButtonSpace(int space);
    void setActionStyle(QAction* action, Qt::ToolButtonStyle style);

    void addAction(QAction *action, ActionPosition position = AP_Back);

    void setHoverColor(const QColor &color);
    QColor hoverColor() const;

    void setPressColor(const QColor &color);
    QColor pressColor() const;

    FancyMode* currentMode() const;
    FancyMode* mode(int);

    QWidget* spacer() const;
    QWidget* line() const;

    void setTextColor(const QColor &color);
    void setSelectedTextColor(const QColor &color);

signals:
    void currentChanged(int);

private:
    FancyModeBarPrivate *d;
    Direction m_direction;
};

#endif // FANCYMODEBAR_H
