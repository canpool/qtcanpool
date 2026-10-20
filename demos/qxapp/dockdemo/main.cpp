/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#include "mainwindow.h"

#include <QApplication>
#include <QCommandLineOption>
#include <QCommandLineParser>
#include <QAbstractButton>
#include <QColor>
#include <QCoreApplication>
#include <QDebug>
#include <QElapsedTimer>
#include <QEventLoop>
#include <QGuiApplication>
#include <QHash>
#include <QImage>
#include <QLabel>
#include <QPixmap>
#include <QStyleHints>
#include <QTimer>
#include <QWidget>

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

namespace
{
/*
 * What a widget paints behind its own children, measured rather than read: a
 * style sheet keeps no public record of what it resolved, and a widget with no
 * rule of its own renders nothing at all - which is why the question "does this
 * one cover the surface it sits on" cannot be answered by looking at the sheet.
 * Painting the widget alone and counting the pixels it filled is the answer.
 */
QString painted(QWidget *widget)
{
    if (!widget->isVisible() || widget->width() <= 0 || widget->height() <= 0) {
        return QStringLiteral("(not painted)");
    }
    const QImage image = widget->grab().toImage().convertToFormat(QImage::Format_ARGB32);
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
    const int total = image.width() * image.height();
    if (!opaque) {
        return QStringLiteral("transparent");
    }
    QRgb top = 0;
    int best = 0;
    for (auto it = counts.constBegin(); it != counts.constEnd(); ++it) {
        if (it.value() > best) {
            best = it.value();
            top = it.key();
        }
    }
    return QStringLiteral("%1, %2% filled, %3% of it that colour")
        .arg(QColor(top).name(), QString::number(opaque * 100 / total), QString::number(best * 100 / opaque));
}

// The colour a label paints its glyph in. An icon that never got tinted keeps
// the colour it shipped with, and on an accent fill the shipped colour is the
// one thing the eye reports as "the icon is still black".
QString glyph(QWidget *widget)
{
    if (!widget->isVisible() || widget->width() <= 0 || widget->height() <= 0) {
        return QString();
    }
    // A label carries its glyph as a pixmap and a button as an icon; Qt 5 hands
    // the label's back as a pointer and Qt 6 as a value, so the two spellings
    // differ. Both are asked for the picture rather than the widget: the
    // widget's own background is what the line above this one reports.
    QPixmap pixmap;
    if (const QLabel *label = qobject_cast<const QLabel *>(widget)) {
#if (QT_VERSION < QT_VERSION_CHECK(6, 0, 0))
        if (const QPixmap *p = label->pixmap()) {
            pixmap = *p;
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
    QRgb top = 0;
    int best = 0;
    for (auto it = counts.constBegin(); it != counts.constEnd(); ++it) {
        if (it.value() > best) {
            best = it.value();
            top = it.key();
        }
    }
    return best ? QColor(top).name() : QString();
}

void dump(QWidget *widget, int depth)
{
    const QString indent(depth * 2, QLatin1Char(' '));
    QString line =
        QStringLiteral("%1%2 \"%3\" (%4,%5 %6x%7)%8")
            .arg(indent, QLatin1String(widget->metaObject()->className()), widget->objectName(),
                 QString::number(widget->x()), QString::number(widget->y()), QString::number(widget->width()),
                 QString::number(widget->height()), widget->isVisible() ? QString() : QStringLiteral(" hidden"));
    qDebug().noquote() << line << "|" << painted(widget);
    const QString glyphColor = glyph(widget);
    if (!glyphColor.isEmpty()) {
        qDebug().noquote() << indent << "   glyph:" << glyphColor;
    }
    const QList<QObject *> children = widget->children();
    for (QObject *child : children) {
        if (QWidget *childWidget = qobject_cast<QWidget *>(child)) {
            dump(childWidget, depth + 1);
        }
    }
}

// A collapsed auto hide container paints nothing, and off a real desktop it
// collapses again before a capture can be taken - so the view that is worth
// inspecting is the one that has to be asked for.
void revealAutoHideContainers(QWidget *root)
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
        revealAutoHideContainers(widget);
    }
}
}   // namespace

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
    // The auto hide panel is collapsed in the demo's own arrangement, so a
    // capture of it needs this switch: its title bar is filled with the
    // highlight and the tabs sit on that fill, which is a different surface
    // from a docked tab's.
    QCommandLineOption autoHideOption(QStringLiteral("autohide"),
                                      QStringLiteral("Open the auto hide panel before capturing."));
    // A style sheet keeps no public record of what it resolved, so "which widget
    // is painting over this surface" is a question the sheet cannot answer. This
    // prints the widget tree with what each widget actually paints, which is the
    // first thing to reach for when a dock surface looks wrong.
    QCommandLineOption inspectOption(QStringLiteral("inspect"),
                                     QStringLiteral("Print the widget tree with the colour each widget paints."));
    parser.addOption(screenshotOption);
    parser.addOption(delayOption);
    parser.addOption(themeOption);
    parser.addOption(autoHideOption);
    parser.addOption(inspectOption);

    // parse(), not process(): the demo is routinely started with Qt's own
    // switches (-platform, -style, ...), which the parser would reject as
    // unknown options and turn into a hard exit.
    parser.parse(QCoreApplication::arguments());
    if (parser.isSet(QStringLiteral("help"))) {
        parser.showHelp(0);
    }

    const QString screenshotFile = parser.value(screenshotOption);
    const bool inspect = parser.isSet(QStringLiteral("inspect"));
    // A comma-separated list switches through the themes in order, so a capture
    // can show what the window looks like after a switch and not only after a
    // start - the same shape the ribbon demo's --theme has, and what the theme
    // switch test drives.
    const QStringList themeIds = parser.isSet(QStringLiteral("theme"))
                                     ? parser.value(themeOption).split(QLatin1Char(','), Qt::SkipEmptyParts)
                                     : QStringList();
    // An interactive run keeps whatever theme the engine restored from last time,
    // so the demo opens on the one the user left it on. A capture names one: the
    // golden has to be the same picture whatever the machine's last run picked.
    const QString themeId = !themeIds.isEmpty()
                                ? themeIds.first().trimmed()
                                : ((screenshotFile.isEmpty() && !inspect) ? QString() : QStringLiteral("soft-light"));

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

    if (themeIds.size() > 1) {
        // The window is built on the first theme, so the rest are asked for
        // behind the construction. A switch is a stack of palette and sheet
        // changes, and the dock re-tints its glyphs from them; let each land
        // before the next one is asked for, or the capture is of a window
        // halfway between two themes.
        QTimer::singleShot(0, &w, [&w, themeIds]() {
            for (int i = 1; i < themeIds.size(); ++i) {
                w.applyTheme(themeIds.at(i).trimmed());
                QCoreApplication::processEvents(QEventLoop::AllEvents);
            }
        });
    }

    if (screenshotFile.isEmpty()) {
        w.show();
    } else {
        // A fixed size keeps the capture comparable between runs: there is no
        // window manager to maximise it, and the golden would otherwise
        // depend on whatever the offscreen screen happens to be.
        w.resize(1280, 800);
        w.show();
    }

    if (inspect) {
        // A tree describes the window it was read from, so it has to be the same
        // size the captures are taken at - otherwise a rectangle read here cannot
        // be found in a picture taken there.
        w.resize(1280, 800);
        // A theme sequence is applied on the first turn of the event loop, and this
        // branch answers before that turn runs: without draining the queue first,
        // --theme a,b --inspect describes a window still in a and says nothing about
        // what a capture of the same run shows.
        QCoreApplication::processEvents(QEventLoop::AllEvents);
        // The two roles a glyph is tinted from, printed first: a glyph that
        // comes out black is either a palette that never got the theme's accent
        // or a tint that was never applied, and this says which in one line.
        const QPalette palette = qApp->palette();
        qDebug().noquote() << "palette: Window" << palette.color(QPalette::Window).name() << "WindowText"
                           << palette.color(QPalette::WindowText).name() << "Highlight"
                           << palette.color(QPalette::Highlight).name() << "HighlightedText"
                           << palette.color(QPalette::HighlightedText).name();
        // Walked after the layout has run, because what a widget paints is only
        // answerable once it has a size; and with the collapsed auto hide
        // containers opened first, since they are the view a capture cannot
        // hold still long enough to look at.
        revealAutoHideContainers(&w);
        dump(&w, 0);
        return 0;
    }

    qDebug() << "window build cost:" << cost.elapsed() << " ms";

    if (!screenshotFile.isEmpty()) {
        const bool autoHide = parser.isSet(autoHideOption);
        const int delay = parser.value(delayOption).toInt();
        if (autoHide) {
            // Open early and let the slide animation settle before the grab.
            QTimer::singleShot(50, &w, [&w]() {
                w.expandAutoHidePanel();
            });
        }
        QTimer::singleShot(delay, &w, [&w, &a, screenshotFile]() {
            const QPixmap shot = w.grab();
            const bool saved = shot.save(screenshotFile);
            qInfo() << "screenshot:" << screenshotFile << shot.size() << (saved ? "ok" : "FAILED");
            a.exit(saved ? 0 : 1);
        });
    }

    return (a.exec());
}
