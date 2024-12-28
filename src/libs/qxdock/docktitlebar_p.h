/**
 * Copyleft (C) 2024 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
**/
#pragma once

#include "qxdock_global.h"
#include <QToolButton>

QX_DOCK_BEGIN_NAMESPACE

class DockTitleBar;

/**
 * Title bar button of a dock area that customizes QToolButton appearance/behaviour
 * according to various config flags such as:
 * DockManager::DockAreaHas_xxx_Button - if set to 'false' keeps the button always invisible
 * DockManager::DockAreaHideDisabledButtons - if set to 'true' hides button when it is disabled
 */
class TitleBarButton : public QToolButton
{
    Q_OBJECT
public:
    using Super = QToolButton;
public:
    TitleBarButton(bool showInTitleBar, bool hideWhenDisabled, Qx::DockTitleBarButton id,
                   QWidget *parent = nullptr);

    void setShowInTitleBar(bool show);

    DockTitleBar *titleBar() const;
    bool isInAutoHideArea() const;

    virtual void setVisible(bool visible) override;

public:
    Qx::DockTitleBarButton m_id;
    bool m_showInTitleBar = true;
    bool m_hideWhenDisabled = false;

protected:
    bool event(QEvent *e) override;
};

/**
 * This spacer widget is here because of the following problem.
 * The dock title bar handles mouse dragging and moving the floating widget.
 * The problem is, that if the title bar becomes invisible, i.e. if the dock
 * panel contains only one single dock widget and the dock panel is moved
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
