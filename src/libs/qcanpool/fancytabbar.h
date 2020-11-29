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
#ifndef FANCYTABBAR_H
#define FANCYTABBAR_H

#include <QWidget>
#include "qcanpool_global.h"

QCANPOOL_BEGIN_NAMESPACE

class FancyTabBarPrivate;
class QCANPOOL_SHARED_EXPORT FancyTabBar : public QWidget
{
    Q_OBJECT
public:
    enum Direction {Horizontal, Vertical};
    enum ActionPosition {Front, Middle, Back };
    enum TabType { Mode, Action };
    enum TabStyle {IconOnly, TextOnly, TextBesideIcon, TextUnderIcon};

    explicit FancyTabBar(QWidget *parent = nullptr);
    virtual ~FancyTabBar();

    void setDirection(Direction direction);

    /* tab */
    void setTabEnabled(int index, bool enable);
    bool isTabEnabled(int index) const;

    void setTabVisible(int index, bool visible);
    bool isTabVisible(int index) const;

    int addTab(const QString &label, bool hasMenu = false);
    int addTab(const QIcon &icon, const QString &label, bool hasMenu = false);

    int insertTab(int index, const QString &label, bool hasMenu = false);
    int insertTab(int index, const QIcon &icon, const QString &label, bool hasMenu = false);
    void removeTab(int index);

    QWidget *tabWidget(int index) const;

    void setCurrentIndex(int index);
    int currentIndex() const;

    void unselectCurrent();

    void setTabToolTip(int index, QString toolTip);
    QString tabToolTip(int index) const;

    void setTabShortcut(int index, const QString &shortcut);

    void setTabStyle(TabType type, TabStyle style);
    void setTabFont(TabType type, QFont &font);
    void setTabSpace(TabType type, int space);
    void setTabIconSize(QSize size);

    /* action */
    int addAction(QAction *action, ActionPosition position = Back);
    int insertAction(int index, QAction *action);
    int insertAction(QAction *before, QAction *action);
    void removeAction(QAction *action);
    void setActionStyle(QAction *action, TabStyle style);

    QWidget *widgetForAction(QAction *action) const;

    /* color */
    void setHoverColor(const QColor &color);
    QColor hoverColor() const;

    void setPressColor(const QColor &color);
    QColor pressColor() const;

    void setTextColor(const QColor &color);
    void setSelectedTextColor(const QColor &color);

    void setBackgroundColor(const QColor &color);

    void setHeadSpace(int space);

signals:
    void currentChanged(int index);
    void menuTriggered(int index, QPoint pos);

public slots:
    void hideMenu(int index);

private:
    FancyTabBarPrivate *d;
};

QCANPOOL_END_NAMESPACE

#endif // FANCYTABBAR_H
