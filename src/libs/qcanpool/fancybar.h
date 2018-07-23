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
#ifndef FANCYBAR_H
#define FANCYBAR_H

#include <QWidget>
#include "qcanpool_global.h"

class QMenuBar;
class QuickAccessBar;
class FancyBarPrivate;

class QCANPOOL_SHARED_EXPORT FancyBar : public QWidget
{
    Q_OBJECT
public:
    enum AdditionalControlPosition {
        TitlePosition, MenuPosition
    };

    enum FancyStyle{
      WindowStyle, ClassicStyle, MergedStyle, DialogStyle
    };

    explicit FancyBar(QWidget *parent = 0, Qt::WindowFlags f = 0);
    ~FancyBar();

    QMenuBar* menuBar() const;
    void showMenuBar(bool show = false);
    bool isMenuBarVisible() const;

    QuickAccessBar* quickAccessBar() const;
    void showQuickAccess(bool show = true);
    bool isQuickAccessVisible() const;

    void setHoverColor(const QColor &color);
    void setPressColor(const QColor &color);
    void setTextColor(const QColor &color);
    void setBackgroundColor(const QColor &color);

    void addAdditionalControl(QAction *action, AdditionalControlPosition position);
    void addAdditionalControl(QWidget *widget, AdditionalControlPosition position);
    void setApplicationWidget(const QString &label, QWidget *widget);
    void setApplicationButtonBkColor(const QColor &color);

    void setWidgetResizable(bool resizable);
    void setTitleBarHeight(int height);

    void setFancyStyle(FancyStyle style);

signals:
    void maximizationChanged(bool maximized);

protected:
    virtual bool eventFilter(QObject* object, QEvent* event);

private:
    FancyBarPrivate *d;
};

#endif // FANCYBAR_H
