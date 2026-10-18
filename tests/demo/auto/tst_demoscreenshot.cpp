/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/

/*!
 * Visual regression for the demos (roadmap task B4).
 *
 * The demos render themselves - each knows how, through its own --screenshot
 * switch, so the test does not have to drive the windows - and each result is
 * checked in two steps. The sanity check (window size, "something was drawn")
 * runs everywhere and is the actual regression guard. The golden comparison is
 * opt-in (QTCANPOOL_VISUAL_BASELINE=1): a screenshot is only comparable
 * between the machine that produced the golden and the same machine later, so
 * CI - with its own fonts and theme - must never judge one, and this test must
 * not gate.
 *
 * The golden is per platform and per demo
 * (`tests/demo/golden/<CMAKE_SYSTEM_NAME>/`). To (re)create one, look at the
 * screenshot first and then run with QTCANPOOL_UPDATE_GOLDEN=1. The demos run
 * offscreen by default on every platform: on Linux/macOS that is the only
 * headless option, and on Windows the offscreen plugin has no font database
 * (every glyph is a box) but the render is otherwise complete and perfectly
 * deterministic - which is exactly what a golden wants. Set QT_QPA_PLATFORM to
 * compare against a real desktop session instead.
 *
 * The third check does not use a golden at all. A window that switched from one theme
 * to another has to look exactly like a window started in that theme, so the two
 * captures are compared against each other: same build, same machine, seconds apart.
 * Whatever the switch failed to re-colour shows up as a difference, which is how a
 * control left behind in the previous theme gets caught without anybody listing the
 * controls - see themeSwitchMatchesAFreshStart().
 *
 * Paths arrive as compile definitions from tests/CMakeLists.txt; there is no
 * reasonable way to guess them at runtime (the demos and the test do not share
 * an output directory on every platform). A demo whose definition is missing
 * is simply not covered - the demos are registered one by one, so a build with
 * only some of them compiles and the rest of the coverage still runs.
 */

#include <QtCore/QDir>
#include <QtCore/QFile>
#include <QtCore/QFileInfo>
#include <QtCore/QHash>
#include <QtCore/QProcess>
#include <QtCore/QProcessEnvironment>
#include <QtCore/QStringList>
#include <QtGui/QImage>
#include <QtTest/QtTest>

namespace
{

/*!
 * "Not a flat colour" is a cheap proxy for "something was drawn". A blank
 * window is one or two colours, while the real thing measures in the dozens
 * even at its worst: the dock demo measures 62 distinct colours and the ribbon
 * demo 485 on Windows' offscreen plugin, where every glyph is a box and the
 * captures are otherwise plain. Sampled every other pixel; the count only has
 * to be a lower bound, so the floor sits an order of magnitude above blank.
 */
constexpr int kMinDistinctColours = 20;

/*!
 * The other half of the same proxy: a blank window is one colour over (almost)
 * the whole frame. The two demos measure 64% and 80% for their most common
 * colour - the empty panels of the dock demo, the empty page area of the
 * ribbon - so the ceiling is set well above both while still catching a window
 * that painted nothing at all.
 */
constexpr int kMaxDominantPercent = 90;

/*!
 * Font rasterisation moves a pixel here and there between two machines, so a
 * handful of differing pixels must not fail the run. A real regression - a
 * stylesheet that stopped applying, a layout that collapsed - moves orders of
 * magnitude more.
 */
constexpr int kChannelTolerance = 12;
constexpr double kMaxDiffPercent = 0.5;

/*!
 * How long a demo is given after show() before it captures itself. The layouts
 * need their theme and stylesheet to settle.
 */
const char *const kScreenshotDelay = "2000";

/*!
 * A theme switch is judged by comparing two captures of the same build on the same
 * machine, seconds apart - no committed golden, no second machine's fonts - so there is
 * no reason for a pixel to move. The budget is therefore almost nothing: enough for a
 * stray pixel, while a control that kept the previous theme moves thousands.
 */
constexpr double kMaxSwitchDiffPercent = 0.05;

/*!
 * The two themes a switch is made between have to differ enough for the comparison to
 * mean anything: if the demo ignored the theme switch entirely, both captures would be
 * the same picture and "the switch left nothing behind" would pass for the wrong reason.
 */
constexpr double kMinThemeDiffPercent = 1.0;

QImage loadAsArgb32(const QString &path)
{
    // Both sides are converted to the same format before they are compared:
    // nothing guarantees that the golden and a fresh capture come back in the
    // same one (Qt picks per file and per platform), and comparing across
    // formats would report every pixel as different.
    return QImage(path).convertToFormat(QImage::Format_ARGB32);
}

int countDifferingPixels(const QImage &lhs, const QImage &rhs, int tolerance = kChannelTolerance)
{
    int differing = 0;

    for (int y = 0; y < lhs.height(); ++y) {
        const QRgb *a = reinterpret_cast<const QRgb *>(lhs.constScanLine(y));
        const QRgb *b = reinterpret_cast<const QRgb *>(rhs.constScanLine(y));

        for (int x = 0; x < lhs.width(); ++x) {
            if (qAbs(qRed(a[x]) - qRed(b[x])) > tolerance || qAbs(qGreen(a[x]) - qGreen(b[x])) > tolerance ||
                qAbs(qBlue(a[x]) - qBlue(b[x])) > tolerance) {
                ++differing;
            }
        }
    }

    return differing;
}

/*!
 * Runs one demo to completion and leaves its capture at @p screenshotPath.
 * Returns false with a message in @p error that is meant to be shown as is.
 *
 * @p extraArguments are appended after the screenshot switches, which is how the
 * theme checks below ask for a theme or a sequence of them.
 */
bool runDemo(const QString &executable, const QString &screenshotPath, const QString &logName, QString &error,
             const QStringList &extraArguments = QStringList())
{
    const QFileInfo demo(executable);
    if (!demo.exists()) {
        error = QStringLiteral("the demo is missing: %1").arg(demo.absoluteFilePath());
        return false;
    }

    QDir().mkpath(QFileInfo(screenshotPath).absolutePath());
    QFile::remove(screenshotPath);

    QProcessEnvironment environment = QProcessEnvironment::systemEnvironment();
    if (!environment.contains(QStringLiteral("QT_QPA_PLATFORM"))) {
        // Headless by default: what is being checked is the pixels, not the
        // window. Windows' offscreen plugin has no fonts (every glyph is a
        // box), but the render is otherwise complete and deterministic; set
        // QT_QPA_PLATFORM to render through the real plugin instead.
        environment.insert(QStringLiteral("QT_QPA_PLATFORM"), QStringLiteral("offscreen"));
    }

    QStringList arguments{QStringLiteral("--screenshot"), screenshotPath, QStringLiteral("--screenshot-delay"),
                          QString::fromLatin1(kScreenshotDelay)};
    arguments.append(extraArguments);

    QProcess process;
    process.setProcessEnvironment(environment);
    process.setProgram(demo.absoluteFilePath());
    process.setArguments(arguments);
    // The demos are chatty (hundreds of style warnings per run), so their output
    // goes to files instead of pipes: nothing can fill up and block the child,
    // and the log survives for the failure messages below. This also keeps the
    // run off QProcess's Windows named-pipe path, which has been observed to
    // fail with ERROR_PIPE_BUSY on some machines.
    const QString outputDir = QFileInfo(screenshotPath).absolutePath();
    const QString logPath = outputDir + QStringLiteral("/") + logName;
    process.setStandardInputFile(QProcess::nullDevice());
    process.setStandardOutputFile(logPath);
    process.setStandardErrorFile(logPath);
    process.start();

    if (!process.waitForStarted(60000)) {
        error = QStringLiteral("the demo did not start (log at %1)").arg(logPath);
        return false;
    }
    if (!process.waitForFinished(180000)) {
        error = QStringLiteral("the demo did not finish (log at %1)").arg(logPath);
        return false;
    }

    QFile logFile(logPath);
    if (!logFile.open(QIODevice::ReadOnly)) {
        error = QStringLiteral("the demo log could not be read (log at %1)").arg(logPath);
        return false;
    }
    const QString output = QString::fromLocal8Bit(logFile.readAll());

    if (process.exitStatus() != QProcess::NormalExit) {
        error = QStringLiteral("the demo crashed:\n%1").arg(output);
        return false;
    }
    if (process.exitCode() != 0) {
        error = QStringLiteral("the demo exited with %1:\n%2").arg(process.exitCode()).arg(output);
        return false;
    }
    if (!QFileInfo::exists(screenshotPath)) {
        error = QStringLiteral("no screenshot was written:\n%1").arg(output);
        return false;
    }

    return true;
}

/*!
 * Catches the failures that are worth catching on every run: a demo that starts
 * but renders nothing, or a window that lost its geometry. Whether the pixels
 * are the *right* pixels is the golden's business.
 */
void checkRenders(const QImage &screenshot, const QString &name)
{
    QVERIFY2(screenshot.width() >= 800 && screenshot.height() >= 600,
             qPrintable(QStringLiteral("%1: implausible window size %2x%3")
                            .arg(name)
                            .arg(screenshot.width())
                            .arg(screenshot.height())));

    // The histogram is over every second pixel: it feeds both the distinct
    // colour count and the dominant colour fraction, and neither needs more
    // resolution than that.
    QHash<QRgb, int> counts;
    int sampled = 0;

    for (int y = 0; y < screenshot.height(); y += 2) {
        const QRgb *line = reinterpret_cast<const QRgb *>(screenshot.constScanLine(y));

        for (int x = 0; x < screenshot.width(); x += 2) {
            ++counts[line[x]];
            ++sampled;
        }
    }

    int dominant = 0;
    for (auto it = counts.constBegin(); it != counts.constEnd(); ++it) {
        dominant = qMax(dominant, it.value());
    }

    QVERIFY2(counts.size() >= kMinDistinctColours,
             qPrintable(QStringLiteral("%1: only %2 distinct colours - the demo rendered a blank window")
                            .arg(name)
                            .arg(counts.size())));

    QVERIFY2(100 * dominant < kMaxDominantPercent * sampled,
             qPrintable(QStringLiteral("%1: %2% of the pixels are one colour - the demo rendered a blank window")
                            .arg(name)
                            .arg(100.0 * dominant / sampled, 0, 'f', 1)));
}

void checkGolden(const QImage &screenshot, const QString &goldenPath, const QString &screenshotPath,
                 const QString &name)
{
    // Deliberately opt-in, even when a golden exists: a screenshot comparison
    // is only meaningful between the machine that produced the golden and the
    // same machine later. CI's fonts and theme differ from the author's, so a
    // committed golden must never be judged there - this test must not gate.
    if (!qEnvironmentVariableIsSet("QTCANPOOL_VISUAL_BASELINE") &&
        !qEnvironmentVariableIsSet("QTCANPOOL_UPDATE_GOLDEN")) {
        QSKIP(qPrintable(QStringLiteral("golden comparison is opt-in: "
                                        "QTCANPOOL_VISUAL_BASELINE=1 compares, "
                                        "QTCANPOOL_UPDATE_GOLDEN=1 recreates (%1)")
                             .arg(goldenPath)));
    }

    if (qEnvironmentVariableIsSet("QTCANPOOL_UPDATE_GOLDEN")) {
        QVERIFY(QDir().mkpath(QFileInfo(goldenPath).absolutePath()));
        // QFile::copy() does not overwrite: it fails when the destination
        // exists, which is the common case here - refreshing a golden that is
        // already committed rather than creating the first one.
        QFile::remove(goldenPath);
        QVERIFY2(QFile::copy(screenshotPath, goldenPath),
                 qPrintable(QStringLiteral("the golden could not be written: %1").arg(goldenPath)));
        QSKIP("golden updated - review the screenshot before committing it");
    }

    if (!QFileInfo::exists(goldenPath)) {
        QSKIP(qPrintable(QStringLiteral("no golden for this platform: %1 (the capture is at %2; "
                                        "QTCANPOOL_UPDATE_GOLDEN=1 creates one)")
                             .arg(goldenPath)
                             .arg(screenshotPath)));
    }

    const QImage golden = loadAsArgb32(goldenPath);
    QVERIFY2(!golden.isNull(), qPrintable(QStringLiteral("the golden could not be read: %1").arg(goldenPath)));

    QVERIFY2(golden.size() == screenshot.size(),
             qPrintable(QStringLiteral("%1: the window size changed: golden %2x%3, now %4x%5")
                            .arg(name)
                            .arg(golden.width())
                            .arg(golden.height())
                            .arg(screenshot.width())
                            .arg(screenshot.height())));

    const int differing = countDifferingPixels(golden, screenshot);
    const double percent = 100.0 * differing / (golden.width() * golden.height());

    QVERIFY2(percent <= kMaxDiffPercent, qPrintable(QStringLiteral("%1: %2% of the pixels differ from %3 (budget %4%, "
                                                                   "channel tolerance %5); the capture is at %6")
                                                        .arg(name)
                                                        .arg(percent, 0, 'f', 3)
                                                        .arg(goldenPath)
                                                        .arg(kMaxDiffPercent)
                                                        .arg(kChannelTolerance)
                                                        .arg(screenshotPath)));
}

}   // namespace

class tst_DemoScreenshot : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase();

    void ribbonDemoRenders();
    void ribbonDemoMatchesGolden();
    void themeSwitchMatchesAFreshStart();
#ifdef DEMO_DOCK_EXECUTABLE
    void dockDemoRenders();
    void dockDemoMatchesGolden();
#endif
private:
    /*!
     * The ribbon as it looks when started straight into @p themeId; empty when the demo
     * could not produce it, with @p error saying why. Cached, because the switch pairs
     * below reuse the same themes.
     */
    QImage captureFreshTheme(const QString &themeId, QString &error);
    /*! The ribbon after switching from @p fromId to @p toId, caught at the end. */
    QImage captureAfterSwitch(const QString &fromId, const QString &toId, QString &error);

    QImage m_ribbonScreenshot;
    QHash<QString, QImage> m_themeScreenshots;
#ifdef DEMO_DOCK_EXECUTABLE
    QImage m_dockScreenshot;
#endif
};

/*!
 * Runs every registered demo once; the test functions work on the captures.
 */
void tst_DemoScreenshot::initTestCase()
{
    QString error;

    QVERIFY2(runDemo(QStringLiteral(DEMO_RIBBON_EXECUTABLE), QStringLiteral(DEMO_SCREENSHOT_PATH),
                     QStringLiteral("ribbondemo.log"), error),
             qPrintable(error));
    m_ribbonScreenshot = loadAsArgb32(QStringLiteral(DEMO_SCREENSHOT_PATH));
    QVERIFY2(
        !m_ribbonScreenshot.isNull(),
        qPrintable(QStringLiteral("the screenshot could not be read: %1").arg(QStringLiteral(DEMO_SCREENSHOT_PATH))));

#ifdef DEMO_DOCK_EXECUTABLE
    QVERIFY2(runDemo(QStringLiteral(DEMO_DOCK_EXECUTABLE), QStringLiteral(DEMO_DOCK_SCREENSHOT_PATH),
                     QStringLiteral("dockdemo.log"), error),
             qPrintable(error));
    m_dockScreenshot = loadAsArgb32(QStringLiteral(DEMO_DOCK_SCREENSHOT_PATH));
    QVERIFY2(
        !m_dockScreenshot.isNull(),
        qPrintable(
            QStringLiteral("the screenshot could not be read: %1").arg(QStringLiteral(DEMO_DOCK_SCREENSHOT_PATH))));
#endif
}

void tst_DemoScreenshot::ribbonDemoRenders()
{
    checkRenders(m_ribbonScreenshot, QStringLiteral("ribbondemo"));
}

void tst_DemoScreenshot::ribbonDemoMatchesGolden()
{
    checkGolden(m_ribbonScreenshot, QStringLiteral(DEMO_GOLDEN_PATH), QStringLiteral(DEMO_SCREENSHOT_PATH),
                QStringLiteral("ribbondemo"));
}

namespace
{
/*! Where the theme captures are written: beside the demo's own screenshot. */
QString themeCaptureDirectory()
{
    return QFileInfo(QStringLiteral(DEMO_SCREENSHOT_PATH)).absolutePath();
}
}   // namespace

QImage tst_DemoScreenshot::captureFreshTheme(const QString &themeId, QString &error)
{
    if (m_themeScreenshots.contains(themeId)) {
        return m_themeScreenshots.value(themeId);
    }

    const QString name = QStringLiteral("theme-%1").arg(themeId);
    const QString path = themeCaptureDirectory() + QLatin1Char('/') + name + QStringLiteral(".png");
    QImage shot;
    if (runDemo(QStringLiteral(DEMO_RIBBON_EXECUTABLE), path, name + QStringLiteral(".log"), error,
                QStringList{QStringLiteral("--theme"), themeId})) {
        shot = loadAsArgb32(path);
    }

    // Cached either way: a failure here is not worth repeating for the next pair.
    m_themeScreenshots.insert(themeId, shot);
    return shot;
}

QImage tst_DemoScreenshot::captureAfterSwitch(const QString &fromId, const QString &toId, QString &error)
{
    const QString name = QStringLiteral("switch-%1-to-%2").arg(fromId, toId);
    const QString path = themeCaptureDirectory() + QLatin1Char('/') + name + QStringLiteral(".png");
    // One build, one process, one run: the demo applies the themes in the order given and
    // captures after the last one has settled.
    const QString sequence = fromId + QLatin1Char(',') + toId;
    if (!runDemo(QStringLiteral(DEMO_RIBBON_EXECUTABLE), path, name + QStringLiteral(".log"), error,
                 QStringList{QStringLiteral("--theme"), sequence})) {
        return QImage();
    }
    return loadAsArgb32(path);
}

void tst_DemoScreenshot::themeSwitchMatchesAFreshStart()
{
    // A ThemeSwitch is checked by comparing pixels, and what it compares them against is
    // not a commit: it is the same build started straight into the theme it switched to.
    // Anything the switch failed to re-colour is a difference, so the check names the
    // defect instead of describing it - "the editor kept the dark background" is the
    // difference between two captures, not a property anybody has to assert one by one.
    struct ThemeSwitch {
        const char *from;
        const char *to;
    };
    // The reported case was dark to light; the reverse runs the same code path and has
    // the same mistake available to it. The third pair crosses theme families, where a
    // sheet and a palette disagreeing shows up first.
    const ThemeSwitch pairs[] = {
        {"classic-dark", "classic-light"},
        {"classic-light", "classic-dark"},
        {"accent-dark", "soft-light"},
    };

    QString error;
    for (const ThemeSwitch &pair : pairs) {
        const QString from = QString::fromLatin1(pair.from);
        const QString to = QString::fromLatin1(pair.to);
        const QString name = QStringLiteral("%1 -> %2").arg(from, to);

        const QImage freshFrom = captureFreshTheme(from, error);
        QVERIFY2(!freshFrom.isNull(), qPrintable(QStringLiteral("%1: %2").arg(name, error)));
        const QImage freshTo = captureFreshTheme(to, error);
        QVERIFY2(!freshTo.isNull(), qPrintable(QStringLiteral("%1: %2").arg(name, error)));

        const double pixels = freshTo.width() * freshTo.height();
        if (pixels <= 0) {
            QFAIL(qPrintable(QStringLiteral("%1: the capture is empty").arg(name)));
        }

        // The comparison below only means something when the two themes look different:
        // a demo that ignored --theme would produce the same picture twice and pass.
        const double themesDiffer = 100.0 * countDifferingPixels(freshFrom, freshTo, 0) / pixels;
        QVERIFY2(themesDiffer >= kMinThemeDiffPercent,
                 qPrintable(QStringLiteral("%1: the two themes differ in only %2% of the pixels - "
                                           "the captures are not telling the themes apart")
                                .arg(name)
                                .arg(themesDiffer, 0, 'f', 2)));

        const QImage switched = captureAfterSwitch(from, to, error);
        QVERIFY2(!switched.isNull(), qPrintable(QStringLiteral("%1: %2").arg(name, error)));
        QVERIFY2(switched.size() == freshTo.size(),
                 qPrintable(QStringLiteral("%1: the window is %2x%3 after the switch but %4x%5 when "
                                           "started in that theme")
                                .arg(name)
                                .arg(switched.width())
                                .arg(switched.height())
                                .arg(freshTo.width())
                                .arg(freshTo.height())));

        const double leftBehind = 100.0 * countDifferingPixels(switched, freshTo, 0) / pixels;
        QVERIFY2(
            leftBehind <= kMaxSwitchDiffPercent,
            qPrintable(QStringLiteral("%1: %2% of the pixels still show the old theme (budget %3%) - "
                                      "something the switch did not re-colour; compare %4 with %5")
                           .arg(name)
                           .arg(leftBehind, 0, 'f', 3)
                           .arg(kMaxSwitchDiffPercent)
                           .arg(themeCaptureDirectory() + QStringLiteral("/switch-") + from + QStringLiteral("-to-") +
                                to + QStringLiteral(".png"))
                           .arg(themeCaptureDirectory() + QStringLiteral("/theme-") + to + QStringLiteral(".png"))));
    }
}

#ifdef DEMO_DOCK_EXECUTABLE
void tst_DemoScreenshot::dockDemoRenders()
{
    checkRenders(m_dockScreenshot, QStringLiteral("dockdemo"));
}

void tst_DemoScreenshot::dockDemoMatchesGolden()
{
    checkGolden(m_dockScreenshot, QStringLiteral(DEMO_DOCK_GOLDEN_PATH), QStringLiteral(DEMO_DOCK_SCREENSHOT_PATH),
                QStringLiteral("dockdemo"));
}
#endif

QTEST_GUILESS_MAIN(tst_DemoScreenshot)

#include "tst_demoscreenshot.moc"
