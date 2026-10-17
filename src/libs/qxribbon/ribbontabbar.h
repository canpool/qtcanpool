/**
 * Copyleft (C) 2023-2025 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MIT
 **/
#pragma once

#include "qxribbon_global.h"
#include <QMargins>
#include <QTabBar>

QX_RIBBON_BEGIN_NAMESPACE

class RibbonTabBar : public QTabBar
{
    Q_OBJECT
public:
    explicit RibbonTabBar(QWidget *parent = Q_NULLPTR);

    const QMargins &tabMargin() const;
    void setTabMargin(const QMargins &margin);

    int addTab(const QString &text);
    int insertTab(int index, const QString &text);

    int currentIndex() const;
    int count() const;

    /// The placeholder tab that carries "no page is selected"; it is not one of count()
    int nullTabIndex() const;

public Q_SLOTS:
    void setCurrentIndex(int index);
protected:
    virtual QSize tabSizeHint(int index) const override;
    virtual QSize minimumTabSizeHint(int index) const override;
private:
    QMargins m_tabMargin;
};

QX_RIBBON_END_NAMESPACE
