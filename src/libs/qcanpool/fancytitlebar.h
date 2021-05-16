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
/****************************************************************************
 * NOTES:
 *  1) the mainWidget is host-widget, FancyTitleBar is used as its event filter
 *  2) the titleWidget is the title bar area of mainWidget, used to guide the
 *     mouse event operation of FancyTitleBar
 *
 *  ------------------------------------------------,
 *  |               titleWidget                    | \
 *  |-----------------------------------------------  \
 *  |                                              |   \
 *  |                                              |    \
 *  |                                              |     \
 *  |                                              |       mainWindget
 *  |                                              |     /
 *  |                                              |    /
 *  |                                              |   /
 *  |                                              |  /
 *  |                                              | /
 *  ------------------------------------------------'
 *
 *****************************************************************************
 */
#ifndef FANCYTITLEBAR_H
#define FANCYTITLEBAR_H

#include <QObject>
#include "qcanpool_global.h"

class QMouseEvent;
class QLabel;
class QToolButton;
class QToolBar;
class QAction;

QCANPOOL_BEGIN_NAMESPACE

class FancyTitleBarPrivate;
class QCANPOOL_SHARED_EXPORT FancyTitleBar : public QObject
{
    Q_OBJECT
public:
    explicit FancyTitleBar(QWidget *mainWidget);
    ~FancyTitleBar();

    void setTitleWidget(QWidget *widget);

    QWidget *mainWidget() const;
    QLabel *titleLabel() const;
    QToolButton *logoButton() const;
    QWidget *systemGroup() const;

    // set nondark icon
    void setCloseIcon(QIcon &icon);
    void setMaximizeIcon(QIcon &icon);
    void setMinimizeIcon(QIcon &icon);
    void setNormalIcon(QIcon &icon);
    void setIconDark(bool dark);

    void setWidgetResizable(bool resizable);
    void setWidgetMovable(bool movable);
    void setWidgetMaximizable(bool maximizable);

    void updateWidgetFlags();

signals:
    void windowResizable(bool resizable);
    void windowMovable(bool movable);
    void windowIconChanged(QIcon &icon);

protected:
    virtual bool eventFilter(QObject* object, QEvent* event);

private:
    FancyTitleBarPrivate *d;
};

QCANPOOL_END_NAMESPACE

#endif // FANCYTITLEBAR_H
