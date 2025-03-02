/**
 * Copyleft (C) 2024 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#pragma once

#include "qxdock_global.h"
#include <QFrame>

QX_DOCK_BEGIN_NAMESPACE

class DockResizeHandlePrivate;

/**
 * Resize handle for resizing its parent widget (such as DockAutoHideContainer)
 */
class QX_DOCK_EXPORT DockResizeHandle : public QFrame
{
    Q_OBJECT
    Q_PROPERTY(bool opaqueResize READ opaqueResize WRITE setOpaqueResize)
public:
    using Super = QFrame;
public:
    explicit DockResizeHandle(Qt::Edge handlePosition, QWidget *parent);
    virtual ~DockResizeHandle();

    Qt::Orientation orientation() const;

    QSize sizeHint() const override;

    bool isResizing() const;

    void setMinResizeSize(int minSize);
    void setMaxResizeSize(int maxSize);

    Qt::Edge handlePostion() const;
    void setHandlePosition(Qt::Edge handlePosition);

    bool opaqueResize() const;
    void setOpaqueResize(bool opaque = true);

protected:
    void mouseMoveEvent(QMouseEvent *e) override;
    void mousePressEvent(QMouseEvent *e) override;
    void mouseReleaseEvent(QMouseEvent *e) override;

private:
    QX_DECLARE_PRIVATE(DockResizeHandle)
};

QX_DOCK_END_NAMESPACE
