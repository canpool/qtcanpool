/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#include "tst_probe.h"

#include <QtCore/QCoreApplication>
#include <QtCore/QEventLoop>
#include <QtCore/QHash>
#include <QtCore/QList>
#include <QtCore/QObject>
#include <QtGui/QImage>
#include <QtGui/QPixmap>
#include <QtWidgets/QAbstractButton>
#include <QtWidgets/QLabel>
#include <QtWidgets/QSplitter>
#include <QtWidgets/QWidget>

namespace Probe
{

namespace
{
// The line a widget is written as. Everything that reads a tree reads this, so
// it is written in one place: a class, a name, a rectangle, and whether it is
// showing.
QString describe(QWidget *widget, int depth)
{
    const QString indent(depth * 2, QLatin1Char(' '));
    return QStringLiteral("%1%2 \"%3\" (%4,%5 %6x%7)%8")
        .arg(indent, QLatin1String(widget->metaObject()->className()), widget->objectName(),
             QString::number(widget->x()), QString::number(widget->y()), QString::number(widget->width()),
             QString::number(widget->height()), widget->isVisible() ? QString() : QStringLiteral(" hidden"));
}

QImage imageOf(QWidget *widget)
{
    return widget->grab().toImage().convertToFormat(QImage::Format_ARGB32);
}

QRgb commonest(const QHash<QRgb, int> &counts)
{
    QRgb top = 0;
    int best = 0;
    for (auto it = counts.constBegin(); it != counts.constEnd(); ++it) {
        if (it.value() > best) {
            best = it.value();
            top = it.key();
        }
    }
    return top;
}
}   // namespace

QString painted(QWidget *widget)
{
    if (!widget->isVisible() || widget->width() <= 0 || widget->height() <= 0) {
        return QStringLiteral("(not painted)");
    }
    const QImage image = imageOf(widget);
    int opaque = 0;
    QHash<QRgb, int> counts;
    for (int y = 0; y < image.height(); ++y) {
        for (int x = 0; x < image.width(); ++x) {
            const QRgb pixel = image.pixel(x, y);
            if (qAlpha(pixel) == 0) {
                continue;
            }
            ++opaque;
            ++counts[pixel];
        }
    }
    if (!opaque) {
        return QStringLiteral("transparent");
    }
    const int total = image.width() * image.height();
    const QRgb top = commonest(counts);
    return QStringLiteral("%1, %2% filled, %3% of it that colour")
        .arg(QColor(top).name(), QString::number(opaque * 100 / total),
             QString::number(counts.value(top) * 100 / opaque));
}

QString glyph(QWidget *widget)
{
    if (!widget->isVisible() || widget->width() <= 0 || widget->height() <= 0) {
        return QString();
    }
    // A label carries its glyph as a pixmap and a button as an icon; Qt 5 hands
    // the label's back as a pointer and Qt 6 as a value, so the two spellings
    // differ. Both are asked for the picture rather than the widget: the
    // widget's own background is what painted() reports.
    QPixmap pixmap;
    if (const QLabel *label = qobject_cast<const QLabel *>(widget)) {
#if (QT_VERSION < QT_VERSION_CHECK(6, 0, 0))
        if (const QPixmap *labelPixmap = label->pixmap()) {
            pixmap = *labelPixmap;
        }
#else
        pixmap = label->pixmap();
#endif
    } else if (const QAbstractButton *button = qobject_cast<const QAbstractButton *>(widget)) {
        if (!button->icon().isNull()) {
            pixmap = button->icon().pixmap(QSize(16, 16));
        }
    }
    if (pixmap.isNull()) {
        return QString();
    }
    const QImage image = pixmap.toImage().convertToFormat(QImage::Format_ARGB32);
    QHash<QRgb, int> counts;
    for (int y = 0; y < image.height(); ++y) {
        for (int x = 0; x < image.width(); ++x) {
            const QRgb pixel = image.pixel(x, y);
            if (qAlpha(pixel) > 200) {
                ++counts[pixel];
            }
        }
    }
    const QRgb top = commonest(counts);
    return counts.value(top) ? QColor(top).name() : QString();
}

QString line(QWidget *widget)
{
    QString text = describe(widget, 0) + QStringLiteral(" | ") + painted(widget);
    const QString ink = glyph(widget);
    if (!ink.isEmpty()) {
        text += QStringLiteral("\n   glyph: ") + ink;
    }
    return text;
}

QString tree(QWidget *root)
{
    QStringList lines;
    // Depth first, in the order the children were made: a tree that sorted
    // itself would hide the one thing a scene test is usually looking at, which
    // is that something moved.
    QList<QPair<QWidget *, int>> stack;
    stack.append(qMakePair(root, 0));
    while (!stack.isEmpty()) {
        const QPair<QWidget *, int> entry = stack.takeFirst();
        QWidget *widget = entry.first;
        const int depth = entry.second;
        lines.append(describe(widget, depth) + QStringLiteral(" | ") + painted(widget));
        const QString ink = glyph(widget);
        if (!ink.isEmpty()) {
            lines.append(QString(depth * 2, QLatin1Char(' ')) + QStringLiteral("   glyph: ") + ink);
        }
        const QList<QObject *> children = widget->children();
        for (int i = children.count() - 1; i >= 0; --i) {
            if (QWidget *child = qobject_cast<QWidget *>(children.at(i))) {
                stack.prepend(qMakePair(child, depth + 1));
            }
        }
    }
    return lines.join(QLatin1Char('\n'));
}

void settle(QWidget *window)
{
    Q_UNUSED(window)
    // A theme sequence, a layout and a polish are all things that happen on a
    // turn of the event loop, and a tree read before that turn describes a
    // window that has not had them.
    QCoreApplication::processEvents(QEventLoop::AllEvents);
    // And a widget thrown away with deleteLater() is still in that tree: it
    // goes when the deferred delete is delivered, not when it is posted. A
    // pinned pane that was asked to unpin is one of those - its side tab is
    // deleted by the container's destructor - so a settle that stopped here
    // would call it still pinned.
    QCoreApplication::sendPostedEvents(Q_NULLPTR, QEvent::DeferredDelete);
    QCoreApplication::processEvents(QEventLoop::AllEvents);
}

void revealCollapsed(QWidget *root)
{
    const QList<QObject *> children = root->children();
    for (QObject *child : children) {
        QWidget *widget = qobject_cast<QWidget *>(child);
        if (!widget) {
            continue;
        }
        if (QLatin1String(widget->metaObject()->className()).contains(QStringLiteral("AutoHideContainer"))) {
            widget->resize(220, widget->height() > 0 ? widget->height() : 400);
            widget->show();
        }
        revealCollapsed(widget);
    }
}

QStringList lines(const QString &tree, const QString &what)
{
    QStringList matches;
    for (const QString &line : tree.split(QLatin1Char('\n'))) {
        if (line.contains(what)) {
            matches.append(line);
        }
    }
    return matches;
}

int heightOf(const QString &tree, const QString &what)
{
    const QStringList matches = lines(tree, what);
    if (matches.isEmpty()) {
        return -1;
    }
    // "(x,y WxH)": the height is the number after the 'x' that follows the
    // comma-separated position, and it is the one number on the line that says
    // how tall the widget ended up.
    const int comma = matches.first().indexOf(QLatin1Char(','));
    const int space = matches.first().indexOf(QLatin1Char(' '), comma);
    const int cross = matches.first().indexOf(QLatin1Char('x'), space);
    if (comma < 0 || space < 0 || cross < 0) {
        return -1;
    }
    bool ok = false;
    const int close = matches.first().indexOf(QLatin1Char(')'), cross);
    const int height = matches.first().mid(cross + 1, close - cross - 1).toInt(&ok);
    return ok ? height : -1;
}

int lengthOf(QSplitter *splitter)
{
    return splitter->orientation() == Qt::Horizontal ? splitter->width() : splitter->height();
}

bool fillsItself(QSplitter *splitter)
{
    const QList<int> sizes = splitter->sizes();
    if (sizes.isEmpty()) {
        return false;
    }
    int total = 0;
    int showing = 0;
    for (int i = 0; i < sizes.count(); ++i) {
        // A part that is not showing is given no room at all - that is what a
        // splitter does with a hidden child, and a closed pane is exactly
        // that - so it is not one of the parts that have to fill the splitter,
        // and it does not carry a handle of its own either.
        QWidget *part = splitter->widget(i);
        if (part && part->isHidden()) {
            continue;
        }
        if (sizes.at(i) <= 0) {
            return false;
        }
        total += sizes.at(i);
        ++showing;
    }
    if (!showing) {
        return false;
    }
    const int handles = (showing - 1) * splitter->handleWidth();
    return qAbs(total + handles - lengthOf(splitter)) <= 1;
}

QString describe(QSplitter *splitter)
{
    QStringList parts;
    const QList<int> sizes = splitter->sizes();
    parts.reserve(sizes.count());
    for (int size : sizes) {
        parts.append(QString::number(size));
    }
    return QStringLiteral("%1 parts [%2] in %3 pixels")
        .arg(splitter->count())
        .arg(parts.join(QLatin1Char(',')))
        .arg(lengthOf(splitter));
}

}   // namespace Probe
