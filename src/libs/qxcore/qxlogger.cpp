/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/

#include "qxlogger.h"

#include <QtCore/QDateTime>
#include <QtCore/QDir>
#include <QtCore/QFile>
#include <QtCore/QFileInfo>
#include <QtCore/QMutex>
#include <QtCore/QMutexLocker>
#include <QtCore/qlogging.h>

#include <cstdio>

QX_CORE_BEGIN_NAMESPACE

namespace
{

struct LoggerState {
    QxLogger::Options options;
    QFile file;
    QMutex mutex;
    bool enabled;
    QtMessageHandler previous;

    LoggerState()
        : enabled(false)
        , previous(nullptr)
    {
    }
};

/*!
 * Function-local static: the logger state is created on first use, which
 * avoids static-initialization-order problems, and stays private to this
 * translation unit.
 */
LoggerState &state()
{
    static LoggerState s;
    return s;
}

QxLogger::Level levelFromQt(QtMsgType type)
{
    switch (type) {
    case QtDebugMsg:
        return QxLogger::Debug;
    case QtInfoMsg:
        return QxLogger::Info;
    case QtWarningMsg:
        return QxLogger::Warning;
    case QtCriticalMsg:
        return QxLogger::Critical;
    case QtFatalMsg:
        break;
    }
    return QxLogger::Fatal;
}

char levelLetter(QxLogger::Level level)
{
    switch (level) {
    case QxLogger::Debug:
        return 'D';
    case QxLogger::Info:
        return 'I';
    case QxLogger::Warning:
        return 'W';
    case QxLogger::Critical:
        return 'C';
    case QxLogger::Fatal:
        break;
    }
    return 'F';
}

QString backupName(const QString &fileName, int index)
{
    return fileName + QLatin1Char('.') + QString::number(index);
}

/*!
 * Rotates <fileName> to <fileName>.1 and shifts the older backups up, dropping
 * the oldest one once maxBackups is reached. The caller must hold the lock and
 * have the file open.
 */
void rotate(LoggerState &s, qint64 pending)
{
    if (s.options.maxFileSize <= 0 || s.options.maxBackups <= 0) {
        return;
    }
    if (s.file.size() + pending <= s.options.maxFileSize) {
        return;
    }

    const QString base = s.options.fileName;
    s.file.close();

    for (int i = s.options.maxBackups; i > 1; --i) {
        const QString to = backupName(base, i);
        QFile::remove(to);
        QFile::rename(backupName(base, i - 1), to);
    }
    QFile::remove(backupName(base, 1));
    QFile::rename(base, backupName(base, 1));

    s.file.setFileName(base);
    s.file.open(QIODevice::WriteOnly | QIODevice::Append);
}

void messageHandler(QtMsgType type, const QMessageLogContext &context, const QString &message)
{
    QxLogger::write(levelFromQt(type), QString::fromLatin1(context.category ? context.category : "default"), message);
}

}   // namespace

bool QxLogger::init(const Options &options)
{
    LoggerState &s = state();
    QMutexLocker locker(&s.mutex);

    if (!s.enabled) {
        s.previous = qInstallMessageHandler(messageHandler);
        s.enabled = true;
    }
    s.options = options;

    if (s.file.isOpen()) {
        s.file.close();
    }
    if (options.fileName.isEmpty()) {
        s.file.setFileName(QString());
        return true;
    }

    QDir().mkpath(QFileInfo(options.fileName).absolutePath());
    s.file.setFileName(options.fileName);
    return s.file.open(QIODevice::WriteOnly | QIODevice::Append);
}

void QxLogger::shutdown()
{
    LoggerState &s = state();
    QMutexLocker locker(&s.mutex);

    if (!s.enabled) {
        return;
    }
    if (s.file.isOpen()) {
        s.file.flush();
        s.file.close();
    }
    qInstallMessageHandler(s.previous);
    s.previous = nullptr;
    s.enabled = false;
    s.options = Options();
}

bool QxLogger::isEnabled()
{
    LoggerState &s = state();
    QMutexLocker locker(&s.mutex);
    return s.enabled;
}

QString QxLogger::fileName()
{
    LoggerState &s = state();
    QMutexLocker locker(&s.mutex);
    return s.file.isOpen() ? s.file.fileName() : QString();
}

void QxLogger::flush()
{
    LoggerState &s = state();
    QMutexLocker locker(&s.mutex);
    if (s.file.isOpen()) {
        s.file.flush();
    }
}

void QxLogger::write(Level level, const QLoggingCategory &category, const QString &message)
{
    write(level, QString::fromLatin1(category.categoryName()), message);
}

void QxLogger::write(Level level, const QString &category, const QString &message)
{
    LoggerState &s = state();
    QMutexLocker locker(&s.mutex);

    if (!s.enabled || level < s.options.level) {
        return;
    }

    const QString line = QStringLiteral("%1 [%2] %3: %4\n")
                             .arg(QDateTime::currentDateTime().toString(QStringLiteral("yyyy-MM-dd HH:mm:ss.zzz")))
                             .arg(QLatin1Char(levelLetter(level)))
                             .arg(category)
                             .arg(message);

    if (s.options.toConsole) {
        const QByteArray utf8 = line.toUtf8();
        std::fwrite(utf8.constData(), 1, static_cast<size_t>(utf8.size()), stderr);
        std::fflush(stderr);
    }

    if (s.file.isOpen()) {
        const QByteArray utf8 = line.toUtf8();
        rotate(s, utf8.size());
        s.file.write(utf8);
        s.file.flush();
    }
}

QX_CORE_END_NAMESPACE
