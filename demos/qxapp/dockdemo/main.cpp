/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#include "mainwindow.h"

#include <QApplication>
#include <QCommandLineOption>
#include <QCommandLineParser>
#include <QDebug>
#include <QElapsedTimer>
#include <QGuiApplication>
#include <QPixmap>
#include <QStyleHints>
#include <QTimer>

/**
 * @brief Sends qDebug output to stdout.
 * @param type
 * @param context
 * @param msg
 */
void log_out_put(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    // The five kinds of message write the same line and differ only in the tag
    // in brackets, so the tag is chosen first and the line is written once. A
    // type outside the enum prints like debug, and abort() stays where it was:
    // after the line, and only for a fatal.
    const char *tag = "Debug";
    switch (type) {
    case QtInfoMsg:
        tag = "Info";
        break;
    case QtWarningMsg:
        tag = "Warning";
        break;
    case QtCriticalMsg:
        tag = "Critical";
        break;
    case QtFatalMsg:
        tag = "Fatal";
        break;
    case QtDebugMsg:
    default:
        break;
    }

    const QByteArray localMsg = msg.toLocal8Bit();
    fprintf(stdout, "[%s] %s (%s:%u, %s)\n", tag, localMsg.constData(), context.file, context.line, context.function);

    if (type == QtFatalMsg) {
        abort();
    }

#ifndef QT_NO_DEBUG_OUTPUT
    fflush(stdout);
#endif
}

int main(int argc, char *argv[])
{
#if (QT_VERSION < QT_VERSION_CHECK(6, 0, 0))
    QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);
    QApplication::setHighDpiScaleFactorRoundingPolicy(Qt::HighDpiScaleFactorRoundingPolicy::PassThrough);
#endif

    QApplication a(argc, argv);
    qInstallMessageHandler(log_out_put);

    // "--screenshot <file>" turns the demo into a headless renderer. It is what
    // the visual regression test drives, and it is also the quickest way for a
    // human to capture what the docking looks like on this machine.
    QCommandLineParser parser;
    parser.setApplicationDescription(QStringLiteral("DockDemo"));
    parser.addHelpOption();
    QCommandLineOption screenshotOption(
        QStringLiteral("screenshot"), QStringLiteral("Save a screenshot to <file> and exit."), QStringLiteral("file"));
    QCommandLineOption delayOption(QStringLiteral("screenshot-delay"),
                                   QStringLiteral("Wait <ms> after showing the window before "
                                                  "capturing."),
                                   QStringLiteral("ms"), QStringLiteral("2000"));
    QCommandLineOption themeOption(
        QStringLiteral("theme"),
        QStringLiteral("Start on theme <id>, one of the registered theme ids (soft-light, soft-dark, ...)."),
        QStringLiteral("id"));
    parser.addOption(screenshotOption);
    parser.addOption(delayOption);
    parser.addOption(themeOption);

    // parse(), not process(): the demo is routinely started with Qt's own
    // switches (-platform, -style, ...), which the parser would reject as
    // unknown options and turn into a hard exit.
    parser.parse(QCoreApplication::arguments());
    if (parser.isSet(QStringLiteral("help"))) {
        parser.showHelp(0);
    }

    const QString screenshotFile = parser.value(screenshotOption);
    // An interactive run keeps whatever theme the engine restored from last time,
    // so the demo opens on the one the user left it on. A capture names one: the
    // golden has to be the same picture whatever the machine's last run picked.
    const QString themeId = parser.isSet(QStringLiteral("theme"))
                                ? parser.value(themeOption)
                                : (screenshotFile.isEmpty() ? QString() : QStringLiteral("soft-light"));

    if (!screenshotFile.isEmpty()) {
        // A focused text edit blinks its cursor, and a capture that lands on the
        // blink boundary shows it while the next one does not - 28 pixels of
        // difference between two runs of the same code. The default flash time
        // is 1000 ms, so a delay that is a multiple of it is a coin flip; zero
        // stops the blinking and the caret stays where it is. Qt 5 had this on
        // QGuiApplication itself; both versions reach it through the style
        // hints.
        QGuiApplication::styleHints()->setCursorFlashTime(0);
    }

    QElapsedTimer cost;

    cost.start();

    // The settings file is what makes the demo remember its arrangement between
    // runs; a screenshot must neither read one nor write one, so that the
    // capture is the demo's own layout on every machine.
    MainWindow w(nullptr, !screenshotFile.isEmpty(), themeId);

    if (screenshotFile.isEmpty()) {
        w.show();
    } else {
        // A fixed size keeps the capture comparable between runs: there is no
        // window manager to maximise it, and the golden would otherwise
        // depend on whatever the offscreen screen happens to be.
        w.resize(1280, 800);
        w.show();
    }

    qDebug() << "window build cost:" << cost.elapsed() << " ms";

    if (!screenshotFile.isEmpty()) {
        const int delay = parser.value(delayOption).toInt();
        QTimer::singleShot(delay, &w, [&w, &a, screenshotFile]() {
            const QPixmap shot = w.grab();
            const bool saved = shot.save(screenshotFile);
            qInfo() << "screenshot:" << screenshotFile << shot.size() << (saved ? "ok" : "FAILED");
            a.exit(saved ? 0 : 1);
        });
    }

    return (a.exec());
}
