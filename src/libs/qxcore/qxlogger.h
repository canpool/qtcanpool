/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#ifndef QXLOGGER_H
#define QXLOGGER_H

#include "qxcore_global.h"

#include <QtCore/QLoggingCategory>
#include <QtCore/QString>

QX_CORE_BEGIN_NAMESPACE

/*!
 * Application logging built on top of Qt's message handler: the usual
 * qDebug()/qInfo()/qWarning()/qCritical() calls are captured, formatted with a
 * timestamp, level and category, and written to the console and/or a rotating
 * log file.
 *
 * All members are static: the logger owns process-wide state, and init() must
 * be called once before the first message is logged. shutdown() restores the
 * previous message handler, which keeps the class usable in tests.
 */
class QX_CORE_EXPORT QxLogger
{
public:
    /*! Level letters used in the output, in ascending severity. */
    enum Level {
        Debug = 0,
        Info,
        Warning,
        Critical,
        Fatal,
    };

    struct Options {
        Options()
            : level(Debug)
            , maxFileSize(4 * 1024 * 1024)
            , maxBackups(3)
            , toConsole(true)
        {
        }

        /*! Log file path; empty disables file output. */
        QString fileName;
        /*! Messages below this level are dropped. */
        Level level;
        /*! Rotate once the file grows past this many bytes; <= 0 disables rotation. */
        qint64 maxFileSize;
        /*! Number of rotated files kept, named after the log file with a numeric suffix. */
        int maxBackups;
        /*! Also write to stderr, which is what development builds want. */
        bool toConsole;
    };

    /*!
     * Installs the message handler. Repeated calls replace the previous
     * configuration; returns false when the log file cannot be opened, in
     * which case console output is still active.
     */
    static bool init(const Options &options = Options());
    /*! Flushes and uninstalls the message handler. */
    static void shutdown();
    static bool isEnabled();
    /*! Path of the active log file, empty when file output is disabled. */
    static QString fileName();
    /*! Flushes buffered output to disk. */
    static void flush();

    /*! Formats and writes one message, bypassing the installed handler. */
    static void write(Level level, const QString &category, const QString &message);
    /*! Convenience overload that reads the name from \a category. */
    static void write(Level level, const QLoggingCategory &category, const QString &message);
private:
    Q_DISABLE_COPY(QxLogger)
    QxLogger() = delete;
};

QX_CORE_END_NAMESPACE

#endif   // QXLOGGER_H
