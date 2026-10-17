/**
 * Copyleft (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/

#include "dockstylesheet.h"

#include <QFile>
#include <QTextStream>
#include <QDebug>

QX_DOCK_BEGIN_NAMESPACE

QString dockStyleSheet(bool focusHighlighting)
{
    // The prefix belongs to res/dockstyle.qrc, which the application builds in; see
    // the header for why this function hands the text out instead of applying it.
    QString fileName = QStringLiteral(":/dockstyle/");
    fileName += focusHighlighting ? QStringLiteral("focus_highlighting") : QStringLiteral("default");
#if QX_DOCK_X11
    fileName += QStringLiteral("_linux");
#endif
    fileName += QStringLiteral(".css");

    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly)) {
        qWarning("qxdock: cannot open the dock style sheet %s", qPrintable(fileName));
        return QString();
    }
    const QString result = QTextStream(&file).readAll();
    file.close();
    return result;
}

QX_DOCK_END_NAMESPACE