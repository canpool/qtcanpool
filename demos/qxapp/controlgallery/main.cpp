/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#include "mainwindow.h"

#include "qxtheme/qxthememotion.h"

#include <QApplication>
#include <QCommandLineOption>
#include <QCommandLineParser>
#include <QCoreApplication>
#include <QElapsedTimer>
#include <QEventLoop>
#include <QGuiApplication>
#include <QPixmap>
#include <QStyleHints>
#include <QTimer>

#include <cstdio>

namespace
{
/*!
 * The size a capture is taken at.
 *
 * Fixed, because there is no window manager to maximise the window and the
 * golden would otherwise be whatever the offscreen screen happens to be. Tall,
 * because a gallery is a column of rows and a row that has to be scrolled to is
 * a row the visual regression test cannot crop to.
 */
constexpr int kCaptureWidth = 1280;
constexpr int kCaptureHeight = 1080;
}   // namespace

int main(int argc, char *argv[])
{
    // The command line is copied before QApplication exists, and that ordering
    // is the whole of the reason for it: "--style" is Qt's own switch as well -
    // it names the platform style - and Qt takes it out of the argument list
    // while it constructs itself, so a parser that asked afterwards would never
    // see it and this demo would silently ignore the switch it is built around.
    QStringList rawArguments;
    rawArguments.reserve(argc);
    for (int i = 0; i < argc; ++i) {
        rawArguments.append(QString::fromLocal8Bit(argv[i]));
    }

#if (QT_VERSION < QT_VERSION_CHECK(6, 0, 0))
    QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);
    QApplication::setHighDpiScaleFactorRoundingPolicy(Qt::HighDpiScaleFactorRoundingPolicy::PassThrough);
#endif

    QApplication a(argc, argv);

    QCommandLineParser parser;
    parser.setApplicationDescription(QStringLiteral("ControlGallery"));
    parser.addHelpOption();
    // "--screenshot <file>" turns the demo into a headless renderer: it is what
    // the visual regression test drives, and it is the quickest way for a human
    // to capture what the two control sets look like on this machine.
    QCommandLineOption screenshotOption(
        QStringLiteral("screenshot"), QStringLiteral("Save a screenshot to <file> and exit."), QStringLiteral("file"));
    QCommandLineOption delayOption(QStringLiteral("screenshot-delay"),
                                   QStringLiteral("Wait <ms> after showing the window before capturing."),
                                   QStringLiteral("ms"), QStringLiteral("2000"));
    QCommandLineOption styleOption(
        QStringLiteral("style"), QStringLiteral("Open on style <id>, one of fluent, material."), QStringLiteral("id"));
    QCommandLineOption themeOption(
        QStringLiteral("theme"),
        QStringLiteral("Start on theme <id>, one of the registered theme ids (soft-light, soft-dark, ...)."),
        QStringLiteral("id"));
    // A picture says "does this look right"; this says "where is this and how
    // big did it come out", which is the question the visual regression test
    // needs answered before it can crop anything.
    QCommandLineOption inspectOption(QStringLiteral("inspect"),
                                     QStringLiteral("Print every widget with its rectangle in window coordinates."));
    parser.addOption(screenshotOption);
    parser.addOption(delayOption);
    parser.addOption(styleOption);
    parser.addOption(themeOption);
    parser.addOption(inspectOption);

    // parse(), not process(): the demo is routinely started with Qt's own
    // switches (-platform, -style, ...), which the parser would reject as
    // unknown options and turn into a hard exit.
    parser.parse(rawArguments);
    if (parser.isSet(QStringLiteral("help"))) {
        parser.showHelp(0);
    }

    const QString screenshotFile = parser.value(screenshotOption);
    const bool inspect = parser.isSet(QStringLiteral("inspect"));
    // A comma-separated list switches through the styles in order, so a capture
    // can show what a control looks like after a switch and not only after a
    // start - the same shape the ribbon demo's --theme has, and what the style
    // switch test drives.
    const QStringList styleIds = parser.isSet(QStringLiteral("style"))
                                     ? parser.value(styleOption).split(QLatin1Char(','), Qt::SkipEmptyParts)
                                     : QStringList();
    const QStringList themeIds = parser.isSet(QStringLiteral("theme"))
                                     ? parser.value(themeOption).split(QLatin1Char(','), Qt::SkipEmptyParts)
                                     : QStringList();
    const QString styleId = !styleIds.isEmpty() ? styleIds.first().trimmed() : QStringLiteral("fluent");
    // An interactive run keeps whatever theme the engine restored from last
    // time, so the demo opens on the one the user left it on. A capture names
    // one: the golden has to be the same picture whatever the machine's last
    // run picked.
    const QString themeId = !themeIds.isEmpty()
                                ? themeIds.first().trimmed()
                                : ((screenshotFile.isEmpty() && !inspect) ? QString() : QStringLiteral("soft-light"));

    const bool headless = !screenshotFile.isEmpty() || inspect;
    if (!screenshotFile.isEmpty()) {
        // A focused control blinks its caret, and a capture that lands on the
        // blink boundary shows it while the next one does not - a handful of
        // pixels between two runs of the same code. The default flash time is
        // 1000 ms, so a delay that is a multiple of it is a coin flip; zero
        // stops the blinking and the caret stays where it is.
        QGuiApplication::styleHints()->setCursorFlashTime(0);
    }
    // The reduce-motion switch a capture needs is the window's business, not
    // main()'s: it has to be said after the theme engine has restored whatever
    // it stored and before the controls that animate are built, and only the
    // window knows when that is.

    QElapsedTimer cost;
    cost.start();

    MainWindow w(Q_NULLPTR, !screenshotFile.isEmpty(), styleId, themeId);

    if (themeIds.size() > 1 || styleIds.size() > 1) {
        // The window is built on the first of each, so the rest are asked for
        // behind the construction. A switch is a stack of palette, sheet and tab
        // changes; let each land before the next is asked for, or the capture is
        // of a window halfway between two states.
        QTimer::singleShot(0, &w, [&w, themeIds, styleIds]() {
            for (int i = 1; i < themeIds.size(); ++i) {
                w.applyTheme(themeIds.at(i).trimmed());
                QCoreApplication::processEvents(QEventLoop::AllEvents);
            }
            for (int i = 1; i < styleIds.size(); ++i) {
                w.applyStyle(styleIds.at(i).trimmed());
                QCoreApplication::processEvents(QEventLoop::AllEvents);
            }
        });
    }

    if (!headless) {
        w.show();
    } else {
        w.resize(kCaptureWidth, kCaptureHeight);
        w.show();
    }

    if (inspect) {
        // A tree describes the window it was read from, so it has to be the same
        // size the captures are taken at - otherwise a rectangle read here cannot
        // be found in a picture taken there. Drained first: a --style or --theme
        // sequence is applied on the next turn of the event loop, and a tree read
        // before that turn describes a window that has not switched yet.
        QCoreApplication::processEvents(QEventLoop::AllEvents);
        w.printTree();
        return 0;
    }

    fprintf(stdout, "window build cost: %lld ms\n", static_cast<long long>(cost.elapsed()));

    if (!screenshotFile.isEmpty()) {
        const int delay = parser.value(delayOption).toInt();
        QTimer::singleShot(delay, &w, [&w, &a, screenshotFile]() {
            const QPixmap shot = w.grab();
            const bool saved = shot.save(screenshotFile);
            fprintf(stdout, "screenshot: %s %dx%d %s\n", qPrintable(screenshotFile), shot.width(), shot.height(),
                    saved ? "ok" : "FAILED");
            a.exit(saved ? 0 : 1);
        });
    }

    return (a.exec());
}
