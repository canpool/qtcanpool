/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#include "fluentpasswordbox.h"

#include <QtGui/QMouseEvent>
#include <QtWidgets/QLineEdit>

QX_FLUENT_BEGIN_NAMESPACE

namespace
{
QPointF eventPos(const QMouseEvent *event)
{
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    return event->position();
#else
    return event->localPos();
#endif
}
}   // namespace

class FluentPasswordBoxPrivate
{
public:
    QX_DECLARE_PUBLIC(FluentPasswordBox)

    bool revealed = false;
};

FluentPasswordBox::FluentPasswordBox(QWidget *parent)
    : FluentTextBox(parent)
    , d_ptr(new FluentPasswordBoxPrivate)
{
    QX_SET_PRIVATE(d_ptr)
    lineEdit()->setEchoMode(QLineEdit::Password);
}

FluentPasswordBox::~FluentPasswordBox()
{
    QX_FINI_PRIVATE()
}

bool FluentPasswordBox::isRevealed() const
{
    return d_ptr->revealed;
}

bool FluentPasswordBox::clearWanted() const
{
    // A secret is not emptied by a click; the room answers the reveal.
    return false;
}

void FluentPasswordBox::paintContent(QPainter *painter)
{
    FluentTextBox::paintContent(painter);

    // The eye in the trailing room: hold it, and the characters stand as
    // themselves. Held, it draws in the accent - the answer is in use.
    const QRectF room = clearRect();
    painter->save();
    painter->setRenderHint(QPainter::Antialiasing, true);
    painter->setOpacity(isEnabled() ? 1.0 : 0.4);
    painter->setBrush(Qt::NoBrush);
    painter->setPen(QPen(palette().color(QPalette::WindowText), 1.2));
    painter->drawEllipse(room.center().x() - 8.0, room.center().y() - 4.5, 16.0, 9.0);
    painter->setPen(Qt::NoPen);
    painter->setBrush(d_ptr->revealed ? accentColor() : palette().color(QPalette::WindowText));
    painter->drawEllipse(room.center(), 2.0, 2.0);
    painter->restore();
}

void FluentPasswordBox::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton && isEnabled() && clearRect().contains(eventPos(event))) {
        d_ptr->revealed = true;
        lineEdit()->setEchoMode(QLineEdit::Normal);
        update();
        return;
    }
    FluentTextBox::mousePressEvent(event);
}

void FluentPasswordBox::mouseReleaseEvent(QMouseEvent *event)
{
    if (d_ptr->revealed) {
        d_ptr->revealed = false;
        lineEdit()->setEchoMode(QLineEdit::Password);
        update();
        return;
    }
    FluentTextBox::mouseReleaseEvent(event);
}

QX_FLUENT_END_NAMESPACE
