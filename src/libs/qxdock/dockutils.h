/**
 * Copyleft (C) 2024 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
**/
#pragma once

#include "qxdock_global.h"

#include <QStyle>
#include <QAbstractButton>
#include <QMouseEvent>

#if defined(Q_OS_UNIX) && !defined(Q_OS_MACOS)
#include <xcb/xcb.h>
#endif

// Define QXDOCK_DEBUG_PRINT to enable a lot of debug output
#ifdef QXDOCK_DEBUG_PRINT
#define QXDOCK_PRINT(s) qDebug() << s
#else
#define QXDOCK_PRINT(s)
#endif

class QSplitter;

QX_DOCK_BEGIN_NAMESPACE

class DockSplitter;

namespace internal {
extern const int FloatingWidgetDragStartEvent;
extern const int DockedWidgetDragStartEvent;
static const char *const LocationProperty = "Location";
static const char *const DirtyProperty = "dirty";
static const char *const ClosedProperty = "close";
static const bool RestoreTesting = true;
static const bool Restore = false;

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

Qx::DockSideBarArea toSideBarArea(Qx::DockWidgetArea area);
bool isHorizontalSideBarArea(Qx::DockSideBarArea area);
bool isSideBarArea(Qx::DockWidgetArea area);

enum RepolishChildOptions {
    RepolishIgnoreChildren,
    RepolishDirectChildren,
    RepolishChildrenRecursively
};

void repolishStyle(QWidget *w, RepolishChildOptions options = RepolishIgnoreChildren);

#if defined(Q_OS_UNIX) && !defined(Q_OS_MACOS)
// Utils to directly communicate with the X server
/**
 * Get atom from cache or request it from the XServer.
 */
xcb_atom_t xcb_get_atom(const char *name);

/**
 * Add a property to a window. Only works on "hidden" windows.
 */
void xcb_add_prop(bool state, WId window, const char *type, const char *prop);
/**
 * Updates up to two window properties. Can be set on a visible window.
 */
void xcb_update_prop(bool set, WId window, const char *type, const char *prop, const char *prop2 = nullptr);
/**
 * Only for debugging purposes.
 */
bool xcb_dump_props(WId window, const char *type);
/**
 * Gets the active window manager from the X11 Server.
 * Requires a EWMH conform window manager (Almost all common used ones are).
 * Returns "UNKNOWN" otherwise.
 */
QString windowManager();
#endif

} // internal

QX_DOCK_END_NAMESPACE
