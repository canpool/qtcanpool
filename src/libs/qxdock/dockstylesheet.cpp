/**
 * Copyleft (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/

#include "dockstylesheet.h"

#include <QFile>
#include <QTextStream>
#include <QDebug>

QX_DOCK_BEGIN_NAMESPACE

namespace
{

/*
 * Reads one fragment out of res/dockstyle.qrc and returns its text, or an empty
 * string (with a word on stderr) when the resource is not there. Every caller
 * treats that as a broken build rather than as "no styling".
 */
QString readFragment(const QString &name)
{
    // The prefix belongs to res/dockstyle.qrc, which the application builds in.
    const QString fileName = QStringLiteral(":/dockstyle/") + name;
    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly)) {
        qWarning("qxdock: cannot open the dock style sheet %s", qPrintable(fileName));
        return QString();
    }
    const QString result = QTextStream(&file).readAll();
    file.close();
    return result;
}

}   // namespace

QString dockStyleSheet(bool focusHighlighting)
{
    // Three fragments in a fixed order: the look, then the two modifiers. The
    // order is not cosmetic - where a modifier restates a rule instead of adding
    // one, it has to come after the rule it restates.
    QString result = readFragment(QStringLiteral("base.css"));
    if (result.isEmpty()) {
        return QString();
    }

    if (focusHighlighting) {
        const QString focus = readFragment(QStringLiteral("focus.css"));
        if (focus.isEmpty()) {
            return QString();
        }
        result += focus;
    }

#if QX_DOCK_X11
    const QString x11 = readFragment(QStringLiteral("x11.css"));
    if (x11.isEmpty()) {
        return QString();
    }
    result += x11;
#endif

    return result;
}

QX_DOCK_END_NAMESPACE
