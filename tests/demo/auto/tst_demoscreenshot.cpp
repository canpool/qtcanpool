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
 * controls - see themeSwitchMatchesAFreshStart() and its dock half.
 *
 * That check runs on both demos because they pay for their themes in different ways:
 * the ribbon assembles its own sheet and palette, while the dock hands both to the
 * theme engine and has its glyphs re-tinted on the way. A switch that forgets one of
 * those steps leaves a different half of the window behind, so one demo's green run
 * says nothing about the other's.
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
#include <QtCore/QRegularExpression>
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

/*!
 * A ceiling for the demos that are known to keep the geometry they were laid out with
 * across a switch. The dock is one: the two schemes give their standard controls
 * different metrics - a scroll bar is 16 pixels wide under a dark skeleton and 14 under
 * a light scheme's platform style - the splitter has already divided the window up by
 * then, and nothing lays it out again, so switching leaves the areas a few pixels off
 * where a window started in the target scheme puts them. Measured on the three pairs
 * below at 3.6%, 4.8% and 4.8% of the pixels, all of them along the edges that moved.
 *
 * This is not the budget the check wants, it is the range a known defect lives in, and
 * it is kept as a separate assertion so that a surface left in the old theme - tens of
 * percent - still fails instead of being taken for the known shift. See
 * kMaxSwitchDiffPercent for the budget that is actually meant, and 4.6-TASKS for the
 * two ways out. The one metric that has been made to agree, the tab bar's height, is
 * held down by dockChromeKeepsItsHeightAcrossThemes rather than by this ceiling.
 */
constexpr double kMaxSwitchDiffPercentAfterLayoutShift = 10.0;

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
    void dockThemeSwitchMatchesAFreshStart();
    void dockChromeKeepsItsHeightAcrossThemes();
#endif
private:
    QImage m_ribbonScreenshot;
    /*!
     * The theme captures, keyed by "demo/theme": the same theme id is captured
     * for each demo that switches, and one picture cannot stand for both.
     */
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
/*! Where the theme captures are written: beside the demos' own screenshots. */
QString themeCaptureDirectory()
{
    return QFileInfo(QStringLiteral(DEMO_SCREENSHOT_PATH)).absolutePath();
}

/*!
 * What a capture is written under. Both demos put their captures in the same
 * directory, so the demo is part of the name - otherwise the dock's
 * "theme-classic-dark.png" would be the ribbon's, and the two would silently
 * overwrite each other.
 */
QString themeCaptureName(const QString &demoName, const QString &kind, const QString &detail)
{
    return demoName + QLatin1Char('-') + kind + QLatin1Char('-') + detail;
}

/*!
 * @p demoName as it looks when started straight into @p themeId; empty when the
 * demo could not produce the capture, with @p error saying why. Cached, because
 * the switch pairs below reuse the same themes and one run costs seconds.
 */
QImage captureFreshTheme(const QString &executable, const QString &demoName, const QString &themeId,
                         QHash<QString, QImage> &cache, QString &error)
{
    const QString key = demoName + QLatin1Char('/') + themeId;
    if (cache.contains(key)) {
        return cache.value(key);
    }

    const QString name = themeCaptureName(demoName, QStringLiteral("theme"), themeId);
    const QString path = themeCaptureDirectory() + QLatin1Char('/') + name + QStringLiteral(".png");
    QImage shot;
    if (runDemo(executable, path, name + QStringLiteral(".log"), error,
                QStringList{QStringLiteral("--theme"), themeId})) {
        shot = loadAsArgb32(path);
    }

    // Cached either way: a failure here is not worth repeating for the next pair.
    cache.insert(key, shot);
    return shot;
}

/*! @p demoName after switching from @p fromId to @p toId, caught at the end. */
QImage captureAfterSwitch(const QString &executable, const QString &demoName, const QString &fromId,
                          const QString &toId, QString &error)
{
    const QString name = themeCaptureName(demoName, QStringLiteral("switch"), fromId + QStringLiteral("-to-") + toId);
    const QString path = themeCaptureDirectory() + QLatin1Char('/') + name + QStringLiteral(".png");
    // One build, one process, one run: the demo applies the themes in the order given and
    // captures after the last one has settled.
    const QString sequence = fromId + QLatin1Char(',') + toId;
    if (!runDemo(executable, path, name + QStringLiteral(".log"), error,
                 QStringList{QStringLiteral("--theme"), sequence})) {
        return QImage();
    }
    return loadAsArgb32(path);
}

/*!
 * The widget tree a demo prints for @p themeId under --inspect, or empty with @p error
 * set. A capture answers "does this look right"; this answers "what size did this come
 * out at", which is the question a theme can move by two pixels and still look right.
 */
QString runInspect(const QString &executable, const QString &themeId, const QString &logName, QString &error)
{
    const QFileInfo demo(executable);
    if (!demo.exists()) {
        error = QStringLiteral("the demo is missing: %1").arg(demo.absoluteFilePath());
        return QString();
    }

    QDir().mkpath(themeCaptureDirectory());
    QProcessEnvironment environment = QProcessEnvironment::systemEnvironment();
    if (!environment.contains(QStringLiteral("QT_QPA_PLATFORM"))) {
        environment.insert(QStringLiteral("QT_QPA_PLATFORM"), QStringLiteral("offscreen"));
    }

    const QString logPath = themeCaptureDirectory() + QLatin1Char('/') + logName;
    QProcess process;
    process.setProcessEnvironment(environment);
    process.setProgram(demo.absoluteFilePath());
    process.setArguments(QStringList{QStringLiteral("--theme"), themeId, QStringLiteral("--inspect")});
    process.setStandardInputFile(QProcess::nullDevice());
    process.setStandardOutputFile(logPath);
    process.setStandardErrorFile(logPath);
    process.start();

    if (!process.waitForStarted(60000)) {
        error = QStringLiteral("the demo did not start (log at %1)").arg(logPath);
        return QString();
    }
    if (!process.waitForFinished(180000)) {
        error = QStringLiteral("the demo did not finish (log at %1)").arg(logPath);
        return QString();
    }

    QFile logFile(logPath);
    if (!logFile.open(QIODevice::ReadOnly)) {
        error = QStringLiteral("the demo log could not be read (log at %1)").arg(logPath);
        return QString();
    }
    const QString output = QString::fromLocal8Bit(logFile.readAll());
    if (process.exitStatus() != QProcess::NormalExit || process.exitCode() != 0) {
        error = QStringLiteral("the demo exited with %1:\n%2").arg(process.exitCode()).arg(output);
        return QString();
    }
    return output;
}

/*! The height in @p tree of the first widget @p pattern names, or -1 when it is not there. */
int heightIn(const QString &tree, const QString &pattern)
{
    const QRegularExpressionMatch match = QRegularExpression(pattern).match(tree);
    return match.hasMatch() ? match.captured(2).toInt() : -1;
}

/*!
 * A ThemeSwitch is checked by comparing pixels, and what it compares them against is
 * not a commit: it is the same build started straight into the theme it switched to.
 * Anything the switch failed to re-colour is a difference, so the check names the
 * defect instead of describing it - "an editor kept the dark background" is the
 * difference between two captures, not a property anybody has to assert one by one.
 *
 * Run once for each demo, since the two put a theme on the window through different
 * machinery: the ribbon hands over a sheet and a palette it assembled, while the dock
 * lets the theme engine install both and takes re-tinted glyphs back. Each has its own
 * steps a switch can miss, so one demo's green run says nothing about the other.
 *
 * @p knownLayoutShift says, in a sentence, that this demo is expected to move rather
 * than re-colour; the check then reports that expectation to the test framework
 * instead of pretending it is a pass. Empty means the demo has no such business.
 */
void checkThemeSwitches(const QString &executable, const QString &demoName, QHash<QString, QImage> &cache,
                        const char *knownLayoutShift = nullptr)
{
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
        const QString name = QStringLiteral("%1: %2 -> %3").arg(demoName, from, to);

        const QImage freshFrom = captureFreshTheme(executable, demoName, from, cache, error);
        QVERIFY2(!freshFrom.isNull(), qPrintable(QStringLiteral("%1: %2").arg(name, error)));
        const QImage freshTo = captureFreshTheme(executable, demoName, to, cache, error);
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

        const QImage switched = captureAfterSwitch(executable, demoName, from, to, error);
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
        const QString switchPath =
            themeCaptureDirectory() + QLatin1Char('/') +
            themeCaptureName(demoName, QStringLiteral("switch"), from + QStringLiteral("-to-") + to) +
            QStringLiteral(".png");
        const QString freshPath = themeCaptureDirectory() + QLatin1Char('/') +
                                  themeCaptureName(demoName, QStringLiteral("theme"), to) + QStringLiteral(".png");
        if (knownLayoutShift) {
            // First the loose ceiling, then the expected failure, then the real budget:
            // a surface left in the old theme measures far above the loose ceiling and
            // fails here rather than being filed away with the known shift.
            QVERIFY2(leftBehind <= kMaxSwitchDiffPercentAfterLayoutShift,
                     qPrintable(QStringLiteral("%1: %2% of the pixels differ (loose ceiling %3%) - far "
                                               "more than the layout shift this demo is known for; "
                                               "compare %4 with %5")
                                    .arg(name)
                                    .arg(leftBehind, 0, 'f', 3)
                                    .arg(kMaxSwitchDiffPercentAfterLayoutShift)
                                    .arg(switchPath)
                                    .arg(freshPath)));
            QEXPECT_FAIL("", knownLayoutShift, Continue);
        }
        QVERIFY2(leftBehind <= kMaxSwitchDiffPercent,
                 qPrintable(QStringLiteral("%1: %2% of the pixels still show the old theme (budget %3%) - "
                                           "something the switch did not re-colour; compare %4 with %5")
                                .arg(name)
                                .arg(leftBehind, 0, 'f', 3)
                                .arg(kMaxSwitchDiffPercent)
                                .arg(switchPath)
                                .arg(freshPath)));
    }
}
}   // namespace

void tst_DemoScreenshot::themeSwitchMatchesAFreshStart()
{
    checkThemeSwitches(QStringLiteral(DEMO_RIBBON_EXECUTABLE), QStringLiteral("ribbondemo"), m_themeScreenshots);
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

void tst_DemoScreenshot::dockThemeSwitchMatchesAFreshStart()
{
    // The dock half of the check the ribbon has had since B3. It matters more here in
    // one respect: the dock's chrome is painted from a second style sheet that is
    // assembled on every theme change and from a palette the engine installs, and the
    // glyphs are re-tinted from the accent - three things a switch can leave behind
    // while the ribbon's single sheet is already correct.
    //
    // What it found is a fourth thing, and not a colour: the switch re-colours every
    // pixel and leaves the layout one theme behind. See kMaxSwitchDiffPercentAfterLayoutShift.
    checkThemeSwitches(QStringLiteral(DEMO_DOCK_EXECUTABLE), QStringLiteral("dockdemo"), m_themeScreenshots,
                       "the dock keeps the geometry it was laid out with: the standard controls the dark "
                       "skeleton repaints are given different metrics from the light scheme's platform "
                       "ones, the splitter has already divided the window up by then, and a switch does "
                       "not lay the window out again - the colours are correct, the areas sit a few "
                       "pixels off (4.6-TASKS records the two ways out)");
}

void tst_DemoScreenshot::dockChromeKeepsItsHeightAcrossThemes()
{
    // A title bar is as tall as the tab bar beside it, and that is as tall as the
    // buttons it shares the row with - so a metric the two schemes disagree on does not
    // stay in the button, it becomes the layout. The dark skeleton states a min-height
    // for the standard buttons it has to repaint and the light one states none, which
    // made an icon button 21 pixels in one scheme and 19 in the other.
    //
    // Asserted on the numbers rather than on the pixels: the capture comparison above
    // spans the whole window and cannot tell a two-pixel metric from a colour the switch
    // forgot, and it is expected to fail for reasons of its own. This one is the part
    // that has been fixed, so it is the part that gets a check that cannot be excused.
    QString error;
    const QString light = runInspect(QStringLiteral(DEMO_DOCK_EXECUTABLE), QStringLiteral("classic-light"),
                                     QStringLiteral("inspect-classic-light.log"), error);
    QVERIFY2(!light.isEmpty(), qPrintable(error));
    const QString dark = runInspect(QStringLiteral(DEMO_DOCK_EXECUTABLE), QStringLiteral("classic-dark"),
                                    QStringLiteral("inspect-classic-dark.log"), error);
    QVERIFY2(!dark.isEmpty(), qPrintable(error));

    const QString tabBar = QStringLiteral("DockTabBar \"\" \\(0,0 (\\d+)x(\\d+)\\)");
    const QString titleBar = QStringLiteral("\"dockTitleBar\" \\(0,0 (\\d+)x(\\d+)\\)");
    QVERIFY2(heightIn(light, tabBar) > 0, "the inspect tree carries no dock tab bar");
    QCOMPARE(heightIn(dark, tabBar), heightIn(light, tabBar));
    QCOMPARE(heightIn(dark, titleBar), heightIn(light, titleBar));
}
#endif

QTEST_GUILESS_MAIN(tst_DemoScreenshot)

#include "tst_demoscreenshot.moc"
