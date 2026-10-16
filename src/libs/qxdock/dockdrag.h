/**
 * Copyleft (C) 2024 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#pragma once

#include "qxdock_global.h"
#include <QMimeData>
#include <QObject>
#include <QString>

class QWidget;

QX_DOCK_BEGIN_NAMESPACE

class DockWidget;

/**
 * The drag and drop payload that carries a dock widget.
 *
 * The docking system moves a dock widget with plain mouse events, which only
 * works inside one process: the object that is dragged is the same object that
 * is dropped, and no data ever leaves the application. A native drag replaces
 * that with a payload naming the dock widget, so that a window in another
 * process can decide what to do with it.
 *
 * A receiver never needs a DockWidget - or a DockWindow - to read the payload,
 * which is why the encoding lives here rather than in DockWindow. A payload is
 * refused unless it carries the format below, so the payload travels as UTF-8
 * text and can be looked at with a text editor:
 * \code
 * QxDock/1
 * <dock widget name>
 * <title>
 * \endcode
 * The first line is a signature the future formats can replace; the title is a
 * label only, so a receiver that has no such dock widget can still name the one
 * it builds.
 **/
class QX_DOCK_EXPORT DockDrag
{
public:
    /// The mime type a dock widget travels under. Only a drag carrying it is
    /// one of ours; anything else is left to the widget under the cursor.
    static QString mimeType();

    /// Encodes a dock widget identity. \a dockWidgetName is the identity that
    /// DockWindow::findDockWidget() takes, so a receiver in the same process can
    /// resolve it back to the widget.
    static QMimeData *createMimeData(const QString &dockWidgetName, const QString &title = QString());

    static bool canDecode(const QMimeData *data);
    /// The dock widget name of a payload, or an empty string if \a data is not
    /// one. Callers must check canDecode() first if they need to tell an empty
    /// name apart from a foreign payload.
    static QString dockWidgetName(const QMimeData *data);
    static QString dockWidgetTitle(const QMimeData *data);

    /// Starts a native drag of \a dockWidget. Blocks until the drag finished,
    /// like QDrag::exec(), and returns the action the drop was accepted with -
    /// Qt::IgnoreAction if nothing took it. Requires a running drag loop, so it
    /// belongs in a mouse move handler.
    static Qt::DropAction startDrag(QWidget *source, DockWidget *dockWidget,
                                    Qt::DropActions supportedActions = Qt::MoveAction);
};

QX_DOCK_END_NAMESPACE
