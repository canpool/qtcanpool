/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#include "mainwindow.h"
#include "qxribbon/ribbonpage.h"

#include <QApplication>
#include <QCommandLineOption>
#include <QCommandLineParser>
#include <QCoreApplication>
#include <QDebug>
#include <QElapsedTimer>
#include <QEventLoop>
#include <QGuiApplication>
#include <QPixmap>
#include <QStringList>
#include <QStyle>
#include <QStyleFactory>
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
    // type outside the enum prints like debug, which is what the default arm of
    // the switch this replaced did, and abort() stays where it was: after the
    // line, and only for a fatal.
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

#if defined(Q_OS_WINDOWS) && (QT_VERSION >= QT_VERSION_CHECK(6, 7, 0)) && (QT_VERSION < QT_VERSION_CHECK(6, 8, 0))
    // Qt 6.7 made windows11 the default style on Windows 11, and that style paints from
    // its own light and dark table chosen by the *system* color scheme: it never consults
    // the palette. On a dark desktop this demo, which starts on a light theme, came out
    // with dark input fields, dark scroll bars and a dark title bar. From 6.8 the scheme
    // can be told which way the theme paints (QxTheme::applyColorScheme, called when a
    // theme is installed), so the native style can stay; 6.7 has no such lever, and
    // windowsvista is the only style there that paints what the palette says.
    QApplication::setStyle(QStringLiteral("windowsvista"));
#endif

    QApplication a(argc, argv);
    qInstallMessageHandler(log_out_put);
    qDebug() << "Current style: " << QApplication::style()->objectName();
    qDebug() << "Supported styles: " << QStyleFactory::keys();
    QFont f = a.font();

    f.setFamily("Microsoft YaHei");
    a.setFont(f);

    // "--screenshot <file>" turns the demo into a headless renderer. It is what
    // the visual regression test drives, and it is also the quickest way for a
    // human to capture what the ribbon looks like on this machine.
    QCommandLineParser parser;
    parser.setApplicationDescription(QStringLiteral("RibbonDemo"));
    parser.addHelpOption();
    QCommandLineOption screenshotOption(
        QStringLiteral("screenshot"), QStringLiteral("Save a screenshot to <file> and exit."), QStringLiteral("file"));
    QCommandLineOption delayOption(QStringLiteral("screenshot-delay"),
                                   QStringLiteral("Wait <ms> after showing the window before "
                                                  "capturing."),
                                   QStringLiteral("ms"), QStringLiteral("1500"));
    QCommandLineOption themeOption(QStringLiteral("theme"),
                                   QStringLiteral("Start with the theme <id>, for example accent-dark."),
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

    // The construction log carries elapsed milliseconds, so a screenshot asks
    // the window to keep it out of the editor: the numbers still go to the log
    // file, but the capture renders the same pixels on every run.
    MainWindow w(nullptr, !screenshotFile.isEmpty());

    if (screenshotFile.isEmpty()) {
        w.showMaximized();
    } else {
        // A fixed size keeps the capture comparable between runs: there is no
        // window manager to maximise it, and the golden image would otherwise
        // depend on whatever the offscreen screen happens to be.
        w.resize(1280, 800);
        w.show();
    }

    qDebug() << "window build cost:" << cost.elapsed() << " ms";

    if (parser.isSet(themeOption)) {
        // A comma-separated list switches through the themes in order, so a capture can
        // show what the window looks like after a switch and not only after a start.
        const QStringList themeIds = parser.value(themeOption).split(QLatin1Char(','), Qt::SkipEmptyParts);
        // The window installs its default theme on the first turn of the event loop, so
        // ask for these behind that rather than racing it.
        QTimer::singleShot(0, &w, [&w, themeIds]() {
            for (const QString &themeId : themeIds) {
                w.applyTheme(themeId.trimmed());
                // A switch is a stack of palette and style sheet changes; let them land
                // before the next one, and before the capture.
                QCoreApplication::processEvents(QEventLoop::AllEvents);
            }
        });
    }

    if (!screenshotFile.isEmpty()) {
        const int delay = parser.value(delayOption).toInt();
        QTimer::singleShot(delay, &w, [&w, &a, screenshotFile]() {
            // A page with no name renders as an empty tab, and no pixel check can
            // tell that apart from a narrow one. Rebuilding a page is where the
            // name gets lost, so the screenshot mode - the test's way in - refuses
            // to write a capture of a ribbon that has unlabelled tabs.
            QStringList unnamed;
            const QList<RibbonPage *> pages = w.findChildren<RibbonPage *>();
            for (RibbonPage *page : pages) {
                if (page->pageName().isEmpty()) {
                    unnamed << page->objectName();
                }
            }
            if (!unnamed.isEmpty()) {
                qCritical() << "ribbon pages without a name:" << unnamed;
                a.exit(1);
                return;
            }

            const QPixmap shot = w.grab();
            const bool saved = shot.save(screenshotFile);
            qInfo() << "screenshot:" << screenshotFile << shot.size() << (saved ? "ok" : "FAILED");
            a.exit(saved ? 0 : 1);
        });
    }

    return (a.exec());
}
