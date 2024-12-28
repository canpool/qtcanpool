/**
 * Copyleft (C) 2024 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
**/
#pragma once

#include "qxdock_global.h"
#include "dockfloatingwidget.h"
#include <QWidget>

QX_DOCK_BEGIN_NAMESPACE

class DockPanel;
class DockWidget;

class DockFloatingPreviewPrivate;

/**
 * A floating overlay is a temporary floating widget that is just used to
 * indicate the floating widget movement.
 * This widget is used as a placeholder for drag operations for non-opaque docking
 */
class QX_DOCK_EXPORT DockFloatingPreview : public QWidget
                                         , public DockFloatingWidget
{
    Q_OBJECT
public:
    explicit DockFloatingPreview(DockPanel *content);
    explicit DockFloatingPreview(DockWidget *content);
    virtual ~DockFloatingPreview();

public:
    virtual void startFloating(const QPoint &dragStartMousePos, const QSize &size,
        Qx::DockDragState dragState, QWidget *mouseEventHandler) override;
    virtual void finishDragging() override;
    void moveFloating() override;

    void cleanupAutoHideContainerWidget(Qx::DockWidgetArea containerDropArea);

Q_SIGNALS:
    void draggingCanceled();

private Q_SLOTS:
    void onApplicationStateChanged(Qt::ApplicationState state);

protected:
    DockFloatingPreview(QWidget *content, QWidget *parent);

    virtual bool eventFilter(QObject *watched, QEvent *event) override;
    virtual void paintEvent(QPaintEvent *e) override;

private:
    QX_DECLARE_PRIVATE(DockFloatingPreview)
};

QX_DOCK_END_NAMESPACE
