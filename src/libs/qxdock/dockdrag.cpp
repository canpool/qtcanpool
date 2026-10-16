/**
 * Copyleft (C) 2024 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/

#include "dockdrag.h"
#include "dockwidget.h"

#include <QDrag>
#include <QPixmap>

QX_DOCK_BEGIN_NAMESPACE

namespace
{

const char *kDockDragMimeType = "application/x-qxdock-dockwidget";
const char *kDockDragSignature = "QxDock/1";
const QChar kDockDragSeparator = QLatin1Char('\n');

QByteArray encode(const QString &dockWidgetName, const QString &title)
{
    const QString text =
        QString::fromLatin1(kDockDragSignature) + kDockDragSeparator + dockWidgetName + kDockDragSeparator + title;
    return text.toUtf8();
}

// The payload lines, or an empty list when the data is not a dock widget
// payload at all. A payload without a name is refused here rather than by every
// caller, because the name is the only thing a receiver can act on.
QStringList decode(const QMimeData *data)
{
    if (!data) {
        return QStringList();
    }

    const QString text = QString::fromUtf8(data->data(DockDrag::mimeType()));
    const QStringList lines = text.split(kDockDragSeparator);
    if (lines.count() < 2 || lines.at(0) != QString::fromLatin1(kDockDragSignature) || lines.at(1).isEmpty()) {
        return QStringList();
    }
    return lines;
}

}   // namespace

QString DockDrag::mimeType()
{
    return QString::fromLatin1(kDockDragMimeType);
}

QMimeData *DockDrag::createMimeData(const QString &dockWidgetName, const QString &title)
{
    auto *data = new QMimeData;
    data->setData(mimeType(), encode(dockWidgetName, title));
    return data;
}

bool DockDrag::canDecode(const QMimeData *data)
{
    return !decode(data).isEmpty();
}

QString DockDrag::dockWidgetName(const QMimeData *data)
{
    const QStringList lines = decode(data);
    return lines.isEmpty() ? QString() : lines.at(1);
}

QString DockDrag::dockWidgetTitle(const QMimeData *data)
{
    const QStringList lines = decode(data);
    return lines.count() > 2 ? lines.at(2) : QString();
}

Qt::DropAction DockDrag::startDrag(QWidget *source, DockWidget *dockWidget, Qt::DropActions supportedActions)
{
    if (!source || !dockWidget) {
        return Qt::IgnoreAction;
    }

    // A dock widget knows its identity and its title, and both are what a
    // receiver needs: the identity to find the widget, the title to build one
    // when the drop comes from another process.
    auto *drag = new QDrag(source);
    drag->setMimeData(createMimeData(dockWidget->objectName(), dockWidget->windowTitle()));

    // The pixmap is a courtesy, not part of the payload: a widget that is not
    // shown produces none and the drag still carries the dock widget.
    const QPixmap pixmap = source->grab();
    if (!pixmap.isNull()) {
        drag->setPixmap(pixmap);
        drag->setHotSpot(QPoint(pixmap.width() / 2, pixmap.height() / 2));
    }

    return drag->exec(supportedActions, Qt::MoveAction);
}

QX_DOCK_END_NAMESPACE
