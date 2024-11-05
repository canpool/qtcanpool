/**
 * Copyleft (C) 2024 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
**/
#pragma once

#include "qxdock_global.h"

#include <QStyle>
#include <QAbstractButton>

QX_DOCK_BEGIN_NAMESPACE

namespace internal {

template <class T>
void setFlag(T &flags, typename T::enum_type flag, bool on = true)
{
#if QT_VERSION >= 0x050700
    flags.setFlag(flag, on);
#else
    if (on) {
        flags |= flag;
    } else {
        flags &= ~flag;
    }
#endif
}

template <class QObjectPtr>
void setToolTip(QObjectPtr obj, const QString &tip)
{
#ifndef QT_NO_TOOLTIP
    obj->setToolTip(tip);
#else
    Q_UNUSED(obj);
    Q_UNUSED(tip);
#endif
}

QPixmap createTransparentPixmap(const QPixmap &source, qreal opacity);

void setButtonIcon(QAbstractButton *b, QStyle::StandardPixmap pixmap, Qx::DockIcon iconId);


class DockInsertParam : public QPair<Qt::Orientation, bool>
{
public:
    using QPair<Qt::Orientation, bool>::QPair;
    Qt::Orientation orientation() const {return this->first;}
    bool append() const {return this->second;}
    int insertOffset() const {return append() ? 1 : 0;}
};

DockInsertParam dockAreaInsertParameters(Qx::DockWidgetArea area);

} // internal

QX_DOCK_END_NAMESPACE
