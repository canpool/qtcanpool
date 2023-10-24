/**
 * Copyleft (C) 2023 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
**/
#pragma once

#include "qxribbon_global.h"
#include <QMargins>
#include <QTabBar>

QX_RIBBON_BEGIN_NAMESPACE

class QX_RIBBON_EXPORT RibbonTabBar : public QTabBar
{
    Q_OBJECT
public:
    explicit RibbonTabBar(QWidget *parent = Q_NULLPTR);

    const QMargins &tabMargin() const;
    void setTabMargin(const QMargins &margin);
private:
    QMargins m_tabMargin;
};

QX_RIBBON_END_NAMESPACE
