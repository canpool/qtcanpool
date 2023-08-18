/**
 * Copyleft (C) 2023 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
**/
#pragma once

#include "QxGlobal.h"
#include <QFrame>
#include <QMenu>
#include <QToolButton>

QX_BEGIN_NAMESPACE

class RibbonButtonGroupPrivate;

class QX_WIDGET_EXPORT RibbonButtonGroup : public QFrame
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

QX_END_NAMESPACE
