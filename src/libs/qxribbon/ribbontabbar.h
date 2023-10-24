/**
 * Copyleft (C) 2023 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
**/
#pragma once

#include "qxglobal.h"
#include <QMargins>
#include <QTabBar>

QX_BEGIN_NAMESPACE

class QX_WIDGET_EXPORT RibbonTabBar : public QTabBar
{
    Q_OBJECT
public:
    explicit RibbonTabBar(QWidget *parent = Q_NULLPTR);

    const QMargins &tabMargin() const;
    void setTabMargin(const QMargins &margin);
private:
    QMargins m_tabMargin;
};

QX_END_NAMESPACE
