/***************************************************************************
 **
 **  Copyright (C) 2018-2021 MaMinJie <canpool@163.com>
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
#ifndef FANCYBAR_H
#define FANCYBAR_H

#include <QWidget>
#include "qcanpool_global.h"

class QMenuBar;

QCANPOOL_BEGIN_NAMESPACE

class FancyTitleBar;
class FancyQuickAccessBar;
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

    explicit FancyBar(QWidget *parent);
    ~FancyBar();

    FancyTitleBar *titleBar() const;

    QMenuBar* menuBar() const;
    void showMenuBar(bool show = false);
    bool isMenuBarVisible() const;

    FancyQuickAccessBar* quickAccessBar() const;
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
    void updateWidgetFlags();

signals:
    void maximizationChanged(bool maximized);

private:
    FancyBarPrivate *d;
};

QCANPOOL_END_NAMESPACE

#endif // FANCYBAR_H
