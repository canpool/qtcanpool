/**
 * Copyleft (C) 2024 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#include "dockresizehandle.h"

#include <QMouseEvent>
#include <QRubberBand>

QX_DOCK_BEGIN_NAMESPACE

class DockResizeHandlePrivate
{
public:
    QX_DECLARE_PUBLIC(DockResizeHandle)
public:
    DockResizeHandlePrivate();

    bool isHorizontal() const;
    int pick(const QPoint &pos) const;
    void setRubberBand(int Pos);
    void doResizing(QMouseEvent *e, bool forceResize = false);
public:
    QWidget *m_target = nullptr;
    QPointer<QRubberBand> m_rubberBand;
    Qt::Edge m_handlePosition = Qt::LeftEdge;
    int m_minSize = 0;
    int m_maxSize = 1;
    int m_mouseOffset = 0;
    int m_handleWidth = 4;
    bool m_opaqueResize = false;
    bool m_pressed = false;
};

DockResizeHandlePrivate::DockResizeHandlePrivate()
{
}

bool DockResizeHandlePrivate::isHorizontal() const
{
    Q_Q(const DockResizeHandle);
    return q->orientation() == Qt::Horizontal;
}

int DockResizeHandlePrivate::pick(const QPoint &pos) const
{
    Q_Q(const DockResizeHandle);
    return q->orientation() == Qt::Horizontal ? pos.x() : pos.y();
}

void DockResizeHandlePrivate::setRubberBand(int Pos)
{
    Q_Q(DockResizeHandle);
    if (!m_rubberBand) {
        m_rubberBand = new QRubberBand(QRubberBand::Line, m_target->parentWidget());
    }

    auto geometry = q->geometry();
    auto topLeft = m_target->mapTo(m_target->parentWidget(), geometry.topLeft());
    switch (m_handlePosition) {
    case Qt::LeftEdge:
    case Qt::RightEdge:
        topLeft.rx() += Pos;
        break;
    case Qt::TopEdge:
    case Qt::BottomEdge:
        topLeft.ry() += Pos;
        break;
    }

    geometry.moveTopLeft(topLeft);
    m_rubberBand->setGeometry(geometry);
    m_rubberBand->show();
}

void DockResizeHandlePrivate::doResizing(QMouseEvent *e, bool forceResize)
{
    Q_Q(DockResizeHandle);
    int pos = pick(e->pos()) - m_mouseOffset;
    auto oldGeometry = m_target->geometry();
    auto newGeometry = oldGeometry;
    switch (m_handlePosition) {
    case Qt::LeftEdge: {
        newGeometry.adjust(pos, 0, 0, 0);
        int Size = qBound(m_minSize, newGeometry.width(), m_maxSize);
        pos += (newGeometry.width() - Size);
        newGeometry.setWidth(Size);
        newGeometry.moveTopRight(oldGeometry.topRight());
    } break;

    case Qt::RightEdge: {
        newGeometry.adjust(0, 0, pos, 0);
        int Size = qBound(m_minSize, newGeometry.width(), m_maxSize);
        pos -= (newGeometry.width() - Size);
        newGeometry.setWidth(Size);
    } break;

    case Qt::TopEdge: {
        newGeometry.adjust(0, pos, 0, 0);
        int Size = qBound(m_minSize, newGeometry.height(), m_maxSize);
        pos += (newGeometry.height() - Size);
        newGeometry.setHeight(Size);
        newGeometry.moveBottomLeft(oldGeometry.bottomLeft());
    } break;

    case Qt::BottomEdge: {
        newGeometry.adjust(0, 0, 0, pos);
        int Size = qBound(m_minSize, newGeometry.height(), m_maxSize);
        pos -= (newGeometry.height() - Size);
        newGeometry.setHeight(Size);
    } break;
    }

    if (q->opaqueResize() || forceResize) {
        m_target->setGeometry(newGeometry);
    } else {
        setRubberBand(pos);
    }
}

DockResizeHandle::DockResizeHandle(Qt::Edge handlePosition, QWidget *parent)
    : QWidget(parent)
{
    QX_INIT_PRIVATE(DockResizeHandle);
    Q_D(DockResizeHandle);
    d->m_target = parent;
    setMinResizeSize(48);
    setHandlePosition(handlePosition);
    setAttribute(Qt::WA_StyledBackground, true);
}

DockResizeHandle::~DockResizeHandle()
{
    QX_FINI_PRIVATE();
}

Qt::Orientation DockResizeHandle::orientation() const
{
    Q_D(const DockResizeHandle);
    switch (d->m_handlePosition) {
    case Qt::LeftEdge:   // fall through
    case Qt::RightEdge:
        return Qt::Horizontal;

    case Qt::TopEdge:   // fall through
    case Qt::BottomEdge:
        return Qt::Vertical;
    }

    return Qt::Horizontal;
}

QSize DockResizeHandle::sizeHint() const
{
    Q_D(const DockResizeHandle);
    QSize result;
    switch (d->m_handlePosition) {
    case Qt::LeftEdge:   // fall through
    case Qt::RightEdge:
        result = QSize(d->m_handleWidth, d->m_target->height());
        break;

    case Qt::TopEdge:   // fall through
    case Qt::BottomEdge:
        result = QSize(d->m_target->width(), d->m_handleWidth);
        break;
    }

    return result;
}

bool DockResizeHandle::isResizing() const
{
    Q_D(const DockResizeHandle);
    return d->m_pressed;
}

void DockResizeHandle::setMinResizeSize(int minSize)
{
    Q_D(DockResizeHandle);
    d->m_minSize = minSize;
}

void DockResizeHandle::setMaxResizeSize(int maxSize)
{
    Q_D(DockResizeHandle);
    d->m_maxSize = maxSize;
}

Qt::Edge DockResizeHandle::handlePostion() const
{
    Q_D(const DockResizeHandle);
    return d->m_handlePosition;
}

void DockResizeHandle::setHandlePosition(Qt::Edge handlePosition)
{
    Q_D(DockResizeHandle);
    d->m_handlePosition = handlePosition;
    switch (d->m_handlePosition) {
    case Qt::LeftEdge:   // fall through
    case Qt::RightEdge:
        setCursor(Qt::SizeHorCursor);
        break;

    case Qt::TopEdge:   // fall through
    case Qt::BottomEdge:
        setCursor(Qt::SizeVerCursor);
        break;
    }

    setMaxResizeSize(d->isHorizontal() ? parentWidget()->height() : parentWidget()->width());
    if (!d->isHorizontal()) {
        setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    } else {
        setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
    }
}

bool DockResizeHandle::opaqueResize() const
{
    Q_D(const DockResizeHandle);
    return d->m_opaqueResize;
}

void DockResizeHandle::setOpaqueResize(bool opaque)
{
    Q_D(DockResizeHandle);
    d->m_opaqueResize = opaque;
}

void DockResizeHandle::mouseMoveEvent(QMouseEvent *e)
{
    Q_D(DockResizeHandle);
    if (!(e->buttons() & Qt::LeftButton)) {
        return;
    }

    d->doResizing(e);
}

void DockResizeHandle::mousePressEvent(QMouseEvent *e)
{
    Q_D(DockResizeHandle);
    if (e->button() == Qt::LeftButton) {
        d->m_mouseOffset = d->pick(e->pos());
        d->m_pressed = true;
        update();
    }
}

void DockResizeHandle::mouseReleaseEvent(QMouseEvent *e)
{
    Q_D(DockResizeHandle);
    if (!opaqueResize() && e->button() == Qt::LeftButton) {
        if (d->m_rubberBand) {
            d->m_rubberBand->deleteLater();
        }
        d->doResizing(e, true);
    }
    if (e->button() == Qt::LeftButton) {
        d->m_pressed = false;
        update();
    }
}

QX_DOCK_END_NAMESPACE
