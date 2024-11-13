/**
 * Copyleft (C) 2024 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
**/
#pragma once

#include "qxdock_global.h"
#include <QToolButton>

QX_DOCK_BEGIN_NAMESPACE

class TitleBarButton : public QToolButton
{
    Q_OBJECT
public:
    TitleBarButton(bool showInTitleBar, bool hideWhenDisabled, Qx::DockTitleBarButton id,
                   QWidget *parent = nullptr);

    void setShowInTitleBar(bool show);

public:
    Qx::DockTitleBarButton m_id;
    bool m_showInTitleBar = true;
    bool m_hideWhenDisabled = false;
};

/**
 * This spacer widget is here because of the following problem.
 * The dock area title bar handles mouse dragging and moving the floating widget.
 * The  problem is, that if the title bar becomes invisible, i.e. if the dock
 * area contains only one single dock widget and the dock area is moved
 * into a floating widget, then mouse events are not handled anymore and dragging
 * of the floating widget stops.
 */
class SpacerWidget : public QWidget
{
    Q_OBJECT
public:
    SpacerWidget(QWidget *parent = nullptr);
    virtual QSize sizeHint() const override {return QSize(0, 0);}
    virtual QSize minimumSizeHint() const override {return QSize(0, 0);}
};

QX_DOCK_END_NAMESPACE
