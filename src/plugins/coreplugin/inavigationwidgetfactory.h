/****************************************************************************
**
** Copyright (C) 2016 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of Qt Creator.
**
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3 as published by the Free Software
** Foundation with exceptions as appearing in the file LICENSE.GPL3-EXCEPT
** included in the packaging of this file. Please review the following
** information to ensure the GNU General Public License requirements will
** be met: https://www.gnu.org/licenses/gpl-3.0.html.
**
****************************************************************************/

#pragma once

#include "core_global.h"

#include <utils/id.h>

#include <QObject>
#include <QList>
#include <QKeySequence>

QT_BEGIN_NAMESPACE
class QSettings;
class QToolButton;
class QWidget;
QT_END_NAMESPACE

namespace Utils {
class QtcSettings;
}

namespace Core {

struct NavigationView
{
    NavigationView(QWidget *w = nullptr) : widget(w) {}

    QWidget *widget;
    QList<QToolButton *> dockToolBarWidgets;
};

class CORE_EXPORT INavigationWidgetFactory : public QObject
{
    Q_OBJECT

public:
    INavigationWidgetFactory();
    ~INavigationWidgetFactory() override;

    static const QList<INavigationWidgetFactory *> allNavigationFactories();

    void setDisplayName(const QString &displayName);
    void setPriority(int priority);
    void setId(Utils::Id id);
    void setActivationSequence(const QKeySequence &keys);

    QString displayName() const { return m_displayName ; }
    int priority() const { return m_priority; }
    Utils::Id id() const { return m_id; }
    QKeySequence activationSequence() const;

    // This design is not optimal, think about it again once we need to extend it
    // It could be implemented as returning an object which has both the widget
    // and the docktoolbar widgets
    // Similar to how IView
    virtual NavigationView createWidget() = 0;

    virtual void saveSettings(Utils::QtcSettings *settings, int position, QWidget *widget);
    virtual void restoreSettings(QSettings *settings, int position, QWidget *widget);

private:
    QString m_displayName;
    int m_priority = 0;
    Utils::Id m_id;
    QKeySequence m_activationSequence;
};

} // namespace Core
