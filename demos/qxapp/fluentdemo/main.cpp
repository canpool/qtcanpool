/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#include "mainwindow.h"

#include "qxcore/qxtranslator.h"

QX_CORE_USE_NAMESPACE

#include <QApplication>
#include <QCommandLineOption>
#include <QCommandLineParser>
#include <QCoreApplication>
#include <QDebug>
#include <QElapsedTimer>
#include <QEventLoop>
#include <QGuiApplication>
#include <QPixmap>
#include <QStyle>
#include <QStyleHints>
#include <QStringList>
#include <QTimer>

/**
 * @brief Sends qDebug output to stdout.
 */
void log_out_put(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    // The five kinds of message write the same line and differ only in the tag
    // in brackets, so the tag is chosen first and the line is written once.
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
    qDebug() << "Current style: " << QApplication::style()->objectName();

    QCommandLineParser parser;
    parser.setApplicationDescription(QStringLiteral("FluentDemo"));
    parser.addHelpOption();
    QCommandLineOption screenshotOption(
        QStringLiteral("screenshot"), QStringLiteral("Save a screenshot to <file> and exit."), QStringLiteral("file"));
    QCommandLineOption delayOption(QStringLiteral("screenshot-delay"),
                                   QStringLiteral("Wait <ms> after showing the window before capturing."),
                                   QStringLiteral("ms"), QStringLiteral("1500"));
    QCommandLineOption pageOption(QStringLiteral("page"),
                                  QStringLiteral("Open destination <index> (0-based), as clicking it does."),
                                  QStringLiteral("index"));
    QCommandLineOption searchOption(QStringLiteral("search"),
                                    QStringLiteral("Narrow the pane to what contains <text>, as typing it does."),
                                    QStringLiteral("text"));
    QCommandLineOption themeOption(QStringLiteral("theme"),
                                   QStringLiteral("Start with the theme <id>, for example accent-dark."),
                                   QStringLiteral("id"));
    QCommandLineOption langOption(QStringLiteral("lang"),
                                  QStringLiteral("Show the interface in <language>, for example zh_CN."),
                                  QStringLiteral("language"));
    QCommandLineOption inspectOption(QStringLiteral("inspect"),
                                     QStringLiteral("Print every widget with its rectangle and exit."));
    QCommandLineOption foldOption(QStringLiteral("fold"),
                                  QStringLiteral("Fold the pane to its icons, as the caption's command does."));
    QCommandLineOption widthOption(QStringLiteral("width"),
                                   QStringLiteral("Make the window <pixels> wide and show it, which is what the "
                                                  "responsive checks ask for."),
                                   QStringLiteral("pixels"));
    parser.addOption(screenshotOption);
    parser.addOption(delayOption);
    parser.addOption(pageOption);
    parser.addOption(searchOption);
    parser.addOption(themeOption);
    parser.addOption(langOption);
    parser.addOption(inspectOption);
    parser.addOption(foldOption);
    parser.addOption(widthOption);

    // parse(), not process(): the demo is routinely started with Qt's own
    // switches (-platform, -style, ...), which the parser would reject as
    // unknown options and turn into a hard exit.
    parser.parse(QCoreApplication::arguments());
    if (parser.isSet(QStringLiteral("help"))) {
        parser.showHelp(0);
    }

    const QString screenshotFile = parser.value(screenshotOption);

    // The libraries each ship a catalogue, and nothing loads one unless an
    // application says so. It happens before MainWindow exists on purpose,
    // because a catalogue reaches the strings a widget is constructed with and
    // not only the ones it rebuilds later.
    QxTranslator translator;
#ifdef QTCANPOOL_TRANSLATIONS_DIR
    translator.setTranslationPath(QStringLiteral(QTCANPOOL_TRANSLATIONS_DIR));
#endif
    if (parser.isSet(langOption)) {
        const QString language = parser.value(langOption);
        if (translator.setLanguage(language)) {
            qInfo() << "language" << language << "from" << translator.loadedFiles();
        } else {
            qWarning() << "no catalogue installed for language" << language;
        }
    }

    if (!screenshotFile.isEmpty()) {
        // A focused text edit blinks its cursor, and a capture that lands on the
        // blink boundary shows it while the next one does not. Zero stops the
        // blinking, so the caret stays where it is.
        QGuiApplication::styleHints()->setCursorFlashTime(0);
    }

    QElapsedTimer cost;
    cost.start();

    MainWindow w(nullptr, !screenshotFile.isEmpty());

    bool widthGiven = false;
    const int across = parser.value(widthOption).toInt(&widthGiven);
    widthGiven = widthGiven && across > 0;

    if (widthGiven) {
        // A window told how wide to be is not maximized: the question is what
        // the window does at that width, and a window manager that maximized it
        // would be answering a different one.
        w.resize(across, 800);
        w.show();
    } else if (screenshotFile.isEmpty()) {
        w.showMaximized();
    } else {
        // A fixed size keeps the capture comparable between runs: there is no
        // window manager to maximise it, and a golden image would otherwise
        // depend on whatever the offscreen screen happens to be.
        w.resize(1280, 800);
        w.show();
    }

    qDebug() << "window build cost:" << cost.elapsed() << " ms";

    if (parser.isSet(searchOption)) {
        // Asked for before the destination is, because narrowing the pane
        // rebuilds it and the index a destination has is a position in what
        // the pane lists now.
        const QString text = parser.value(searchOption);
        QTimer::singleShot(0, &w, [&w, text]() {
            w.setSearch(text);
        });
    }

    if (parser.isSet(pageOption)) {
        bool ok = false;
        const int index = parser.value(pageOption).toInt(&ok);
        if (ok) {
            QTimer::singleShot(0, &w, [&w, index]() {
                w.setDestination(index);
            });
        }
    }

    if (parser.isSet(foldOption)) {
        // The pane's other width, which is the shape the pane is most itself:
        // asked for after the destination, so the capture shows a folded pane
        // with the screen it opens still in front.
        QTimer::singleShot(0, &w, [&w]() {
            w.setPaneOpen(false);
        });
    }

    if (parser.isSet(themeOption)) {
        // A comma-separated list switches through the themes in order, so a
        // capture can show what the window looks like after a switch and not
        // only after a start. The window installs its default theme on the
        // first turn of the event loop, so these are asked for behind that
        // rather than racing it.
        const QStringList themeIds = parser.value(themeOption).split(QLatin1Char(','), Qt::SkipEmptyParts);
        QTimer::singleShot(0, &w, [&w, themeIds]() {
            for (const QString &themeId : themeIds) {
                w.applyTheme(themeId.trimmed());
                QCoreApplication::processEvents(QEventLoop::AllEvents);
            }
        });
    }

    if (parser.isSet(inspectOption)) {
        QTimer::singleShot(0, &w, [&w, &a]() {
            w.printTree();
            a.exit(0);
        });
        return a.exec();
    }

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
