/**
 * Copyleft (C) 2023 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MIT
**/
#pragma once

#include "qxribbon_global.h"
#include <QFrame>
#include <QMenu>
#include <QToolButton>

QX_RIBBON_BEGIN_NAMESPACE

class RibbonButtonGroupPrivate;

class QX_RIBBON_EXPORT RibbonButtonGroup : public QFrame
{
    Q_OBJECT
public:
    explicit RibbonButtonGroup(QWidget *parent = Q_NULLPTR);
    virtual ~RibbonButtonGroup();

    QAction *addAction(QAction *a);
    QAction *addAction(const QString &text, const QIcon &icon,
                       QToolButton::ToolButtonPopupMode popMode = QToolButton::InstantPopup);
    QAction *addMenu(QMenu *menu, QToolButton::ToolButtonPopupMode popMode = QToolButton::InstantPopup);
    QAction *addSeparator();
    QAction *addWidget(QWidget *w);

    QSize sizeHint() const Q_DECL_OVERRIDE;
    QSize minimumSizeHint() const Q_DECL_OVERRIDE;
signals:
    void actionTriggered(QAction *action);
protected:
    virtual void actionEvent(QActionEvent *e) Q_DECL_OVERRIDE;
private:
    QX_DECLARE_PRIVATE(RibbonButtonGroup)
};

QX_RIBBON_END_NAMESPACE
