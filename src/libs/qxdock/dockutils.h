/**
 * Copyleft (C) 2024 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
**/
#pragma once

#include "qxdock_global.h"

#include <QStyle>
#include <QAbstractButton>
#include <QMouseEvent>

class QSplitter;

QX_DOCK_BEGIN_NAMESPACE

class DockSplitter;

namespace internal {
extern const int FloatingWidgetDragStartEvent;
extern const int DockedWidgetDragStartEvent;

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

template <class T>
T findParent(const QWidget *w)
{
    QWidget *pw = w->parentWidget();
    while (pw) {
        T parentImpl = qobject_cast<T>(pw);
        if (parentImpl) {
            return parentImpl;
        }
        pw = pw->parentWidget();
    }
    return nullptr;
}

inline QPoint globalPositionOf(QMouseEvent *e)
{
#if (QT_VERSION >= QT_VERSION_CHECK(6, 0, 0))
    return e->globalPosition().toPoint();
#else
    return e->globalPos();
#endif
}

QPixmap createTransparentPixmap(const QPixmap &source, qreal opacity);

void setButtonIcon(QAbstractButton *b, QStyle::StandardPixmap pixmap, Qx::DockIcon iconId);

void replaceSplitterWidget(QSplitter *splitter, QWidget *from, QWidget *to);

void hideEmptyParentSplitters(DockSplitter *firstParentSplitter);


class DockInsertParam : public QPair<Qt::Orientation, bool>
{
public:
    using QPair<Qt::Orientation, bool>::QPair;
    Qt::Orientation orientation() const {return this->first;}
    bool append() const {return this->second;}
    int insertOffset() const {return append() ? 1 : 0;}
};

DockInsertParam dockAreaInsertParameters(Qx::DockWidgetArea area);

bool isHorizontalSideBarArea(Qx::DockSideBarArea area);

enum RepolishChildOptions {
    RepolishIgnoreChildren,
    RepolishDirectChildren,
    RepolishChildrenRecursively
};

void repolishStyle(QWidget *w, RepolishChildOptions options = RepolishIgnoreChildren);

} // internal

QX_DOCK_END_NAMESPACE
