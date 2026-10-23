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
 * The control gallery adds a switch of its own, and one check that only it can make.
 * Its --style switch replaces the whole page with the other control set's, and the
 * equivalence is the same shape as the theme one: a window switched into a style has
 * to look exactly like a window started in it. What the gallery adds is the crop: a
 * whole-window comparison across two dozen controls is diluted - one control that
 * kept the other set's ink is a fraction of a percent of the frame - and a pixel
 * count cannot tell "not re-coloured" from "moved" anyway. So the gallery also
 * prints where every control is (--inspect), and the comparison is made once per
 * control, inside that control's own rectangle, each side cut at its own offset.
 * A control that moved passes; a control that kept the wrong ink does not.
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
 * The gallery's ceiling for the same proxy, and not the shared one: a gallery is
 * a form of small controls on a large page, so its background is most of the
 * frame by construction - 94% of it on the capture this was measured on, where
 * the dock's empty panels and the ribbon's empty page area measure 64% and 80%.
 * The distinct-colour floor is the half of the guard that still bites here.
 */
constexpr int kMaxGalleryDominantPercent = 97;

/*!
 * The two application demos' ceiling for the same proxy, and not the shared one:
 * each is a column of rows on a surface that wraps them, and the surface is the
 * window's own colour to the edges. Measured: 95.7% and 94.5% on Qt 6's offscreen
 * plugin, 97.7% and 97.4% on Qt 5's, whose glyphs are boxes rather than text and
 * whose boxes leave less ink behind. The sharp half of the guard is still the
 * distinct-colour floor - a blank window is one or two colours and both
 * applications measure in the hundreds - and "the theme never arrived" is the
 * theme switch's question, not this one's.
 */
constexpr int kMaxApplicationDemoDominantPercent = 98;

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
 * across a switch.
 *
 * The dock used to be one, and it no longer is: its comparison is made inside the
 * dock's own rectangle (see checkDockSwitchWithinItsOwnRect), which is where the
 * question belongs. What sits above the dock - a menu bar, the demo's own tool bar -
 * is answered by the platform style, and a platform answers a light scheme and a dark
 * one with different metrics; measuring those and calling the result a dock defect is
 * how this ceiling came to exist. Nothing passes it any more, and it stays documented
 * because the next demo that spans a metric it does not own will need the same split.
 */
constexpr double kMaxSwitchDiffPercentAfterLayoutShift = 10.0;

/*!
 * The range the dock's known shift lives in, measured inside the dock's own rectangle.
 *
 * A switch re-colours the dock and leaves the sizes the splitter was given under the
 * old theme in place: the chrome the dock paints is 17 pixels shorter in one scheme, the
 * panes that were divided up with the taller one keep their sizes, and the horizontal
 * split ends up 17 pixels off where a window started in the target scheme puts it -
 * 5.1% of the dock's pixels, all of them on that one boundary. Re-distributing the panes
 * after a switch would match a fresh start exactly and would also throw away whatever
 * arrangement the user had dragged, which is not a trade this library makes on its own;
 * see 4.6-TASKS for the record. This ceiling is the defect's range, not the budget: a
 * surface left in the old theme measures tens of percent and fails below rather than
 * being filed away with it.
 */
constexpr double kMaxDockSwitchDiffPercentKnownShift = 8.0;

/*!
 * A per-control crop is small, and its whole point is that anything in it is a
 * finding - so the budget is loose only where a stray pixel can reach: half a
 * percent of the control's own area, at full channel tolerance. A control that
 * kept the wrong ink moves tens of percent of its own pixels.
 */
constexpr double kMaxControlDiffPercent = 0.5;

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
void checkRenders(const QImage &screenshot, const QString &name, int maxDominantPercent = kMaxDominantPercent)
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

    QVERIFY2(100 * dominant < maxDominantPercent * sampled,
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
#ifdef DEMO_GALLERY_EXECUTABLE
    void galleryRenders();
    void galleryMatchesGolden();
    void gallerySwitchLeavesNoControlBehind();
#endif
#ifdef DEMO_FLUENT_EXECUTABLE
    void fluentDemoRenders();
    void fluentDemoMatchesGolden();
    void fluentThemeSwitchMatchesAFreshStart();
#endif
#ifdef DEMO_MATERIAL_EXECUTABLE
    void materialDemoRenders();
    void materialDemoMatchesGolden();
    void materialThemeSwitchMatchesAFreshStart();
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
#ifdef DEMO_GALLERY_EXECUTABLE
    QImage m_galleryScreenshot;
#endif
#ifdef DEMO_FLUENT_EXECUTABLE
    QImage m_fluentScreenshot;
#endif
#ifdef DEMO_MATERIAL_EXECUTABLE
    QImage m_materialScreenshot;
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

#ifdef DEMO_GALLERY_EXECUTABLE
    QVERIFY2(runDemo(QStringLiteral(DEMO_GALLERY_EXECUTABLE), QStringLiteral(DEMO_GALLERY_SCREENSHOT_PATH),
                     QStringLiteral("controlgallery.log"), error),
             qPrintable(error));
    m_galleryScreenshot = loadAsArgb32(QStringLiteral(DEMO_GALLERY_SCREENSHOT_PATH));
    QVERIFY2(
        !m_galleryScreenshot.isNull(),
        qPrintable(
            QStringLiteral("the screenshot could not be read: %1").arg(QStringLiteral(DEMO_GALLERY_SCREENSHOT_PATH))));
#endif

#ifdef DEMO_FLUENT_EXECUTABLE
    QVERIFY2(runDemo(QStringLiteral(DEMO_FLUENT_EXECUTABLE), QStringLiteral(DEMO_FLUENT_SCREENSHOT_PATH),
                     QStringLiteral("fluentdemo.log"), error),
             qPrintable(error));
    m_fluentScreenshot = loadAsArgb32(QStringLiteral(DEMO_FLUENT_SCREENSHOT_PATH));
    QVERIFY2(
        !m_fluentScreenshot.isNull(),
        qPrintable(
            QStringLiteral("the screenshot could not be read: %1").arg(QStringLiteral(DEMO_FLUENT_SCREENSHOT_PATH))));
#endif

#ifdef DEMO_MATERIAL_EXECUTABLE
    QVERIFY2(runDemo(QStringLiteral(DEMO_MATERIAL_EXECUTABLE), QStringLiteral(DEMO_MATERIAL_SCREENSHOT_PATH),
                     QStringLiteral("materialdemo.log"), error),
             qPrintable(error));
    m_materialScreenshot = loadAsArgb32(QStringLiteral(DEMO_MATERIAL_SCREENSHOT_PATH));
    QVERIFY2(
        !m_materialScreenshot.isNull(),
        qPrintable(
            QStringLiteral("the screenshot could not be read: %1").arg(QStringLiteral(DEMO_MATERIAL_SCREENSHOT_PATH))));
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
 * A theme switch is named by the two ids it goes between.
 */
struct ThemeSwitch {
    const char *from;
    const char *to;
};

/*!
 * The three pairs the demos that predate this test are asked: the reported case
 * (dark to light), its reverse, and a pair that crosses theme families, where a
 * sheet and a palette disagreeing shows up first.
 */
QList<ThemeSwitch> standardThemeSwitches()
{
    return {{"classic-dark", "classic-light"}, {"classic-light", "classic-dark"}, {"accent-dark", "soft-light"}};
}

/*!
 * What the two application demos are asked: one pair, light to dark, and its
 * reverse. Every check here is a demo run, and the two demos are already in
 * initTestCase twice and in a golden check once each; the question the pairs
 * answer - did the switch forget a control - is asked as well, but the answer
 * to "does a family crossing leave something behind" is what the theme engine
 * already answers for the two older demos, since both applications hand the
 * same engine their sheet and palette and paint from the same palette after.
 */
QList<ThemeSwitch> applicationDemoSwitches()
{
    return {{"soft-light", "soft-dark"}, {"soft-dark", "soft-light"}};
}

/*!
 * The widget tree a demo prints for @p themeId under --inspect, or empty with @p error
 * set. A capture answers "does this look right"; this answers "what size did this come
 * out at", which is the question a theme can move by two pixels and still look right.
 */
QString runInspectWith(const QString &executable, const QStringList &extraArguments, const QString &logName,
                       QString &error)
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
    QStringList arguments = extraArguments;
    arguments.append(QStringLiteral("--inspect"));
    process.setArguments(arguments);
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

/*!
 * The tree a demo prints for @p themeId, asked for through the theme switch the
 * two older demos carry.
 */
QString runInspect(const QString &executable, const QString &themeId, const QString &logName, QString &error)
{
    return runInspectWith(executable, QStringList{QStringLiteral("--theme"), themeId}, logName, error);
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
 * A demo whose own chrome does not hold still between schemes is not compared this way:
 * the dock's half of the check is checkDockSwitchWithinItsOwnRect, which asks the same
 * question about the one rectangle the library is answerable for.
 */
void checkThemeSwitches(const QString &executable, const QString &demoName, QHash<QString, QImage> &cache,
                        const QList<ThemeSwitch> &switches)
{
    QString error;
    for (const ThemeSwitch &pair : switches) {
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

/*!
 * The rectangle the dock window occupies in @p tree, or a null rectangle when the tree
 * has no dock window in it. The line looks like
 * \code
 * QxDock::DockWindow "dockWindow" (0,41 800x559) | ...
 * \endcode
 */
QRect dockWindowRect(const QString &tree)
{
    const QRegularExpression pattern(QStringLiteral(R"(QxDock::DockWindow "[^"]*" \((-?\d+),(-?\d+) (\d+)x(\d+)\))"));
    const QRegularExpressionMatch match = pattern.match(tree);
    if (!match.hasMatch()) {
        return QRect();
    }
    return QRect(match.captured(1).toInt(), match.captured(2).toInt(), match.captured(3).toInt(),
                 match.captured(4).toInt());
}

/*!
 * The dock half of the switch check, compared inside the dock's own rectangle.
 *
 * The question "did the switch leave anything behind" is the same one
 * checkThemeSwitches asks; what changes is what counts as an answer. Above the dock
 * is the demo's own chrome, and what a menu bar or a scroll bar in it measures is
 * the platform style's to say - 16 pixels wide under one and 12, 14 or 17 under
 * another. Comparing whole windows reports that on every edge, though the dock
 * answers for none of it.
 *
 * So the two captures are cut down to the dock window first, each at its own offset,
 * and only their common height is compared: the dock's top edge is where the demo's
 * chrome ends, and above it is not this library's rectangle. Nothing is excused inside
 * the cut - a surface left in the old theme, a glyph not re-tinted, a tab bar a pixel
 * off are all still differences.
 */
void checkDockSwitchWithinItsOwnRect(const QString &executable, QHash<QString, QImage> &cache)
{
    struct ThemeSwitch {
        const char *from;
        const char *to;
    };
    const ThemeSwitch pairs[] = {
        {"classic-dark", "classic-light"},
        {"classic-light", "classic-dark"},
        {"accent-dark", "soft-light"},
    };

    const QString demoName = QStringLiteral("dockdemo");
    QString error;
    for (const ThemeSwitch &pair : pairs) {
        const QString from = QString::fromLatin1(pair.from);
        const QString to = QString::fromLatin1(pair.to);
        const QString name = QStringLiteral("%1: %2 -> %3").arg(demoName, from, to);

        const QImage freshFrom = captureFreshTheme(executable, demoName, from, cache, error);
        QVERIFY2(!freshFrom.isNull(), qPrintable(QStringLiteral("%1: %2").arg(name, error)));
        const QImage freshTo = captureFreshTheme(executable, demoName, to, cache, error);
        QVERIFY2(!freshTo.isNull(), qPrintable(QStringLiteral("%1: %2").arg(name, error)));

        // The comparison below only means something when the two themes look different:
        // a demo that ignored --theme would produce the same picture twice and pass.
        const double pixels = freshTo.width() * freshTo.height();
        if (pixels <= 0) {
            QFAIL(qPrintable(QStringLiteral("%1: the capture is empty").arg(name)));
        }
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

        // Where the dock sits in each capture, asked of the demo rather than guessed: a
        // tree is the only record of it, and it is read from the same build in the same
        // two states the pictures were taken in.
        const QRect freshRect = dockWindowRect(runInspect(
            executable, to, themeCaptureName(demoName, QStringLiteral("inspect"), to) + QStringLiteral(".log"), error));
        QVERIFY2(!freshRect.isNull(), qPrintable(QStringLiteral("%1: the tree of a window started in %2 has "
                                                                "no dock window in it: %3")
                                                     .arg(name, to, error)));
        const QRect switchedRect = dockWindowRect(
            runInspect(executable, from + QLatin1Char(',') + to,
                       themeCaptureName(demoName, QStringLiteral("switchinspect"), from + QStringLiteral("-to-") + to) +
                           QStringLiteral(".log"),
                       error));
        QVERIFY2(
            !switchedRect.isNull(),
            qPrintable(QStringLiteral("%1: the tree after the switch has no dock window in it: %2").arg(name, error)));

        const int commonHeight = qMin(freshRect.height(), switchedRect.height());
        QVERIFY2(commonHeight > 0, qPrintable(QStringLiteral("%1: the dock window is %2 tall after the switch "
                                                             "and %3 when started in that theme")
                                                  .arg(name)
                                                  .arg(switchedRect.height())
                                                  .arg(freshRect.height())));
        const QRect freshArea(freshRect.left(), freshRect.top(), freshRect.width(), commonHeight);
        const QRect switchedArea(switchedRect.left(), switchedRect.top(), switchedRect.width(), commonHeight);
        // A cut that leaves a picture is only worth comparing against one of the same
        // size: two different sizes would be a finding of their own, and it is not the
        // one this check reports.
        QVERIFY2(freshArea.size() == switchedArea.size(),
                 qPrintable(QStringLiteral("%1: the dock is %2 wide after the switch but %3 when started in "
                                           "that theme")
                                .arg(name)
                                .arg(switchedArea.width())
                                .arg(freshArea.width())));

        const QImage switchedDock = switched.copy(switchedArea);
        const QImage freshDock = freshTo.copy(freshArea);
        const double dockPixels = freshDock.width() * freshDock.height();
        const double leftBehind = 100.0 * countDifferingPixels(switchedDock, freshDock, 0) / dockPixels;
        const QString switchPath =
            themeCaptureDirectory() + QLatin1Char('/') +
            themeCaptureName(demoName, QStringLiteral("switch"), from + QStringLiteral("-to-") + to) +
            QStringLiteral(".png");
        const QString freshPath = themeCaptureDirectory() + QLatin1Char('/') +
                                  themeCaptureName(demoName, QStringLiteral("theme"), to) + QStringLiteral(".png");
        QVERIFY2(leftBehind <= kMaxDockSwitchDiffPercentKnownShift,
                 qPrintable(QStringLiteral("%1: %2% of the dock's pixels differ - far more than the pane "
                                           "sizes this demo is known to keep across a switch; compare %3 "
                                           "with %4")
                                .arg(name)
                                .arg(leftBehind, 0, 'f', 3)
                                .arg(switchPath)
                                .arg(freshPath)));
        QEXPECT_FAIL("",
                     "the dock keeps the row it was divided with: the split is measured from the theme "
                     "in force while the window was being built, and re-dividing the row gives an even "
                     "row - further from a fresh start than leaving it, besides discarding the user's "
                     "own arrangement (tst_DockTheme pins it, and 4.7-TASKS carries the measurements)",
                     Continue);
        QVERIFY2(leftBehind <= kMaxSwitchDiffPercent,
                 qPrintable(QStringLiteral("%1: %2% of the dock's pixels still show the old theme (budget "
                                           "%3%) - something the switch did not re-colour; compare %4 with %5")
                                .arg(name)
                                .arg(leftBehind, 0, 'f', 3)
                                .arg(kMaxSwitchDiffPercent)
                                .arg(switchPath)
                                .arg(freshPath)));
    }
}
#ifdef DEMO_GALLERY_EXECUTABLE
/*!
 * A control the gallery names in its --inspect tree, and the rectangle it was
 * read at.
 */
struct GalleryControl {
    QString name;
    QRect rect;
};

/*!
 * The four id prefixes the gallery's pages name their rows with: one per family,
 * spelled the family's way. A row whose id does not start with one of these is
 * not a control - it is the page, a section heading or a scroll area.
 */
bool isGalleryControlName(const QString &name)
{
    static const QStringList prefixes{QStringLiteral("fluent"), QStringLiteral("material"), QStringLiteral("cupertino"),
                                      QStringLiteral("harmony")};
    for (const QString &prefix : prefixes) {
        if (name.startsWith(prefix)) {
            return true;
        }
    }
    return false;
}

/*!
 * Every control of the control sets the gallery's tree carries, on the page
 * that was up when the tree was printed.
 *
 * The page that is not current is in the tree too - laid out squeezed and marked
 * hidden - and a rectangle read from it would cut into a picture of the other
 * page, so the hidden ones are dropped here rather than trusted to the caller.
 */
QList<GalleryControl> galleryControls(const QString &tree)
{
    static const QRegularExpression pattern(
        QStringLiteral("^\\s*[\\w:]+ \"(\\w+)\" \\((-?\\d+),(-?\\d+) (\\d+)x(\\d+)\\)( hidden)?$"),
        QRegularExpression::MultilineOption);

    // The demo's log arrives with CRLF line endings (the log is written in text
    // mode on Windows), and a pattern anchored at $ never matches there: $ is
    // before the newline, and the carriage return sits between it and the
    // rectangle. The dock's own tree patterns get away without an anchor; this
    // one needs the end of the line, so the returns are dropped instead.
    QString lines = tree;
    lines.remove(QLatin1Char('\r'));

    QList<GalleryControl> controls;
    QRegularExpressionMatchIterator it = pattern.globalMatch(lines);
    while (it.hasNext()) {
        const QRegularExpressionMatch match = it.next();
        const QString name = match.captured(1);
        if (!isGalleryControlName(name)) {
            continue;
        }
        // The group carries the space it matched, so it is trimmed before it is
        // compared - an exact compare against "hidden" would be false for every
        // hidden widget and the filter would keep exactly the ones it exists to
        // drop.
        if (match.captured(6).trimmed() == QLatin1String("hidden")) {
            continue;
        }
        controls.append({name, QRect(match.captured(2).toInt(), match.captured(3).toInt(), match.captured(4).toInt(),
                                     match.captured(5).toInt())});
    }
    return controls;
}

/*!
 * The gallery's own half of "did a switch leave anything behind", asked once per
 * control instead of once per window.
 *
 * A whole-window comparison across two dozen controls is diluted: one control
 * that kept the wrong ink is a fraction of a percent of the frame, and a pixel
 * count cannot tell "not re-coloured" from "moved" anyway (the pitfall the task
 * records). The tree is what makes the per-control question answerable - it says
 * where every control is, in window coordinates, so the two captures can be cut
 * to the same control at each side's own offset. A control that moved between the
 * two runs passes; a control that kept the wrong ink does not.
 *
 * Two switches are asked. The style switch is the one only this demo can make -
 * the set that comes up is a different page painted by a different library, and
 * the one thing it must not bring with it is the other set's answers. Every
 * ordered pair of the four sets is walked, not one pair of them: a page built on
 * arrival and a page woken up on arrival are two different code paths, and the
 * pair a previous version of this demo happened to make is not the pair the next
 * one does. The theme switch is the one every demo asks, and the first time the
 * control sets have ever been asked it: every set reads the palette the engine
 * installs, so a control that kept the light ink on a dark sheet shows up here
 * and nowhere else.
 */
void checkGallerySwitches(const QString &executable)
{
    struct GallerySwitch {
        const char *option;
        const char *from;
        const char *to;
    };
    // Every ordered pair of the four families: twelve edges, because a switch
    // that leaves something behind is a one-way thing - a page refreshed on the
    // way in and one refreshed on the way out are two different code paths, and
    // the pair the other family happens to make is not the pair this one makes.
    QList<GallerySwitch> pairs;
    static const char *const styles[] = {"fluent", "material", "cupertino", "harmony"};
    for (const char *from : styles) {
        for (const char *to : styles) {
            if (from != to) {
                pairs.append({"--style", from, to});
            }
        }
    }
    pairs.append({"--theme", "soft-light", "soft-dark"});

    QString error;
    for (const GallerySwitch &pair : pairs) {
        const QString option = QString::fromLatin1(pair.option);
        const QString from = QString::fromLatin1(pair.from);
        const QString to = QString::fromLatin1(pair.to);
        const QString name = QStringLiteral("controlgallery %1 %2 -> %3").arg(option.mid(2), from, to);
        const QString stem = QStringLiteral("gallery-%1-%2-to-%3").arg(option.mid(2), from, to);

        const QStringList freshArguments{option, to};
        const QStringList switchedArguments{option, from + QLatin1Char(',') + to};

        const QString freshPath = themeCaptureDirectory() + QLatin1Char('/') + stem + QStringLiteral("-fresh.png");
        QVERIFY2(runDemo(executable, freshPath, stem + QStringLiteral("-fresh.log"), error, freshArguments),
                 qPrintable(error));
        const QImage fresh = loadAsArgb32(freshPath);
        QVERIFY2(!fresh.isNull(), qPrintable(QStringLiteral("%1: the capture could not be read").arg(name)));

        const QString switchedPath =
            themeCaptureDirectory() + QLatin1Char('/') + stem + QStringLiteral("-switched.png");
        QVERIFY2(runDemo(executable, switchedPath, stem + QStringLiteral("-switched.log"), error, switchedArguments),
                 qPrintable(error));
        const QImage switched = loadAsArgb32(switchedPath);
        QVERIFY2(switched.size() == fresh.size(),
                 qPrintable(QStringLiteral("%1: the window is %2x%3 after the switch but %4x%5 when started there")
                                .arg(name)
                                .arg(switched.width())
                                .arg(switched.height())
                                .arg(fresh.width())
                                .arg(fresh.height())));

        const QString freshTree =
            runInspectWith(executable, freshArguments, stem + QStringLiteral("-fresh-inspect.log"), error);
        QVERIFY2(!freshTree.isEmpty(), qPrintable(QStringLiteral("%1: %2").arg(name, error)));
        const QString switchedTree =
            runInspectWith(executable, switchedArguments, stem + QStringLiteral("-switched-inspect.log"), error);
        QVERIFY2(!switchedTree.isEmpty(), qPrintable(QStringLiteral("%1: %2").arg(name, error)));

        const QList<GalleryControl> controls = galleryControls(freshTree);
        // Four rows is the shortest page there is: a page that named fewer did
        // not come up, or the tree stopped naming its rows.
        QVERIFY2(controls.count() >= 4,
                 qPrintable(QStringLiteral("%1: the gallery named only %2 controls - the page did not come up, "
                                           "or the tree stopped naming them")
                                .arg(name)
                                .arg(controls.count())));

        QHash<QString, QRect> after;
        const QList<GalleryControl> switchedControls = galleryControls(switchedTree);
        for (const GalleryControl &control : switchedControls) {
            after.insert(control.name, control.rect);
        }

        for (const GalleryControl &control : controls) {
            const QString where = QStringLiteral("%1: %2").arg(name, control.name);
            QVERIFY2(control.rect.width() > 0 && control.rect.height() > 0,
                     qPrintable(QStringLiteral("%1: came out %2x%3 - it rendered nothing")
                                    .arg(where)
                                    .arg(control.rect.width())
                                    .arg(control.rect.height())));
            QVERIFY2(fresh.rect().contains(control.rect),
                     qPrintable(QStringLiteral("%1: sits at (%2,%3) %4x%5, outside the %6x%7 window - the page "
                                               "has to be shown whole at the capture size")
                                    .arg(where)
                                    .arg(control.rect.x())
                                    .arg(control.rect.y())
                                    .arg(control.rect.width())
                                    .arg(control.rect.height())
                                    .arg(fresh.width())
                                    .arg(fresh.height())));
            QVERIFY2(after.contains(control.name),
                     qPrintable(QStringLiteral("%1: gone from the tree after the switch").arg(where)));

            const QRect afterRect = after.value(control.name);
            const QImage freshCrop = fresh.copy(control.rect);
            const QImage switchedCrop = switched.copy(afterRect);
            QVERIFY2(freshCrop.size() == switchedCrop.size(),
                     qPrintable(QStringLiteral("%1: %2x%3 when started there, %4x%5 after the switch")
                                    .arg(where)
                                    .arg(freshCrop.width())
                                    .arg(freshCrop.height())
                                    .arg(switchedCrop.width())
                                    .arg(switchedCrop.height())));

            const int differing = countDifferingPixels(freshCrop, switchedCrop, 0);
            const double percent = 100.0 * differing / (freshCrop.width() * freshCrop.height());
            QVERIFY2(percent <= kMaxControlDiffPercent,
                     qPrintable(QStringLiteral("%1: %2% of its own pixels differ after the switch (budget %3%) - "
                                               "something the switch did not re-colour, or the other set's ink; "
                                               "compare %4 with %5")
                                    .arg(where)
                                    .arg(percent, 0, 'f', 2)
                                    .arg(kMaxControlDiffPercent)
                                    .arg(switchedPath)
                                    .arg(freshPath)));
        }
    }
}
#endif   // DEMO_GALLERY_EXECUTABLE

}   // namespace

#ifdef DEMO_GALLERY_EXECUTABLE
void tst_DemoScreenshot::galleryRenders()
{
    // The gallery's own ceiling, and not the shared one; see the constant.
    checkRenders(m_galleryScreenshot, QStringLiteral("controlgallery"), kMaxGalleryDominantPercent);
}

void tst_DemoScreenshot::galleryMatchesGolden()
{
    checkGolden(m_galleryScreenshot, QStringLiteral(DEMO_GALLERY_GOLDEN_PATH),
                QStringLiteral(DEMO_GALLERY_SCREENSHOT_PATH), QStringLiteral("controlgallery"));
}

void tst_DemoScreenshot::gallerySwitchLeavesNoControlBehind()
{
    // Both switches at once: the style switch is the gallery's own question and
    // the theme switch is the one every demo asks, but only here is the answer
    // cut up per control - see checkGallerySwitches for why the crop is the part
    // that cannot be done on a whole window.
    checkGallerySwitches(QStringLiteral(DEMO_GALLERY_EXECUTABLE));
}
#endif

#ifdef DEMO_FLUENT_EXECUTABLE
void tst_DemoScreenshot::fluentDemoRenders()
{
    // The application demos' own ceiling; see the constant.
    checkRenders(m_fluentScreenshot, QStringLiteral("fluentdemo"), kMaxApplicationDemoDominantPercent);
}

void tst_DemoScreenshot::fluentDemoMatchesGolden()
{
    checkGolden(m_fluentScreenshot, QStringLiteral(DEMO_FLUENT_GOLDEN_PATH),
                QStringLiteral(DEMO_FLUENT_SCREENSHOT_PATH), QStringLiteral("fluentdemo"));
}

void tst_DemoScreenshot::fluentThemeSwitchMatchesAFreshStart()
{
    checkThemeSwitches(QStringLiteral(DEMO_FLUENT_EXECUTABLE), QStringLiteral("fluentdemo"), m_themeScreenshots,
                       applicationDemoSwitches());
}
#endif

#ifdef DEMO_MATERIAL_EXECUTABLE
void tst_DemoScreenshot::materialDemoRenders()
{
    // The material application's own ceiling; see the constant.
    checkRenders(m_materialScreenshot, QStringLiteral("materialdemo"), kMaxApplicationDemoDominantPercent);
}

void tst_DemoScreenshot::materialDemoMatchesGolden()
{
    checkGolden(m_materialScreenshot, QStringLiteral(DEMO_MATERIAL_GOLDEN_PATH),
                QStringLiteral(DEMO_MATERIAL_SCREENSHOT_PATH), QStringLiteral("materialdemo"));
}

void tst_DemoScreenshot::materialThemeSwitchMatchesAFreshStart()
{
    checkThemeSwitches(QStringLiteral(DEMO_MATERIAL_EXECUTABLE), QStringLiteral("materialdemo"), m_themeScreenshots,
                       applicationDemoSwitches());
}
#endif

void tst_DemoScreenshot::themeSwitchMatchesAFreshStart()
{
    checkThemeSwitches(QStringLiteral(DEMO_RIBBON_EXECUTABLE), QStringLiteral("ribbondemo"), m_themeScreenshots,
                       standardThemeSwitches());
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
    // pixel and leaves the layout one theme behind. So the comparison is made inside
    // the dock's own rectangle - that is the part of the window the library answers for,
    // and the metrics that move it belong to the platform style. See
    // checkDockSwitchWithinItsOwnRect.
    checkDockSwitchWithinItsOwnRect(QStringLiteral(DEMO_DOCK_EXECUTABLE), m_themeScreenshots);
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
