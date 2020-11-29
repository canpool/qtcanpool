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
#ifndef FANCYMODE_H
#define FANCYMODE_H

#include <QWidget>

#include <QIcon>
#include <QMenu>

#include "qcanpool_global.h"

QCANPOOL_BEGIN_NAMESPACE

class QCANPOOL_SHARED_EXPORT FancyMode : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool enabled READ isEnabled WRITE setEnabled)
public:
    explicit FancyMode(QObject *parent = nullptr);
    ~FancyMode();

    QString displayName() const { return m_displayName;}
    QIcon icon() const { return m_icon;}
    int priority() const { return m_priority; }
    bool isEnabled() const;
    QMenu *menu() const { return m_menu; }
    void setWidget(QWidget *widget) { m_widget = widget; }
    QWidget* widget();

    void setEnabled(bool enabled);
    void setDisplayName(const QString &displayName) { m_displayName = displayName;}
    void setIcon(const QIcon &icon) { m_icon = icon;}
    void setPriority(int priority) { m_priority = priority; }
    void setMenu(QMenu *menu) { m_menu = menu; }

signals:
    void enabledStateChanged(bool enabled);

private:
    QString m_displayName;
    QIcon m_icon;
    QMenu *m_menu = nullptr;
    QWidget *m_widget = nullptr;
    int m_priority = -1;
    bool m_isEnabled;
};

QCANPOOL_END_NAMESPACE

#endif // FANCYMODE_H
