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

QImage loadAsArgb32(const QString &path)
{
    // Both sides are converted to the same format before they are compared:
    // nothing guarantees that the golden and a fresh capture come back in the
    // same one (Qt picks per file and per platform), and comparing across
    // formats would report every pixel as different.
    return QImage(path).convertToFormat(QImage::Format_ARGB32);
}

int countDifferingPixels(const QImage &lhs, const QImage &rhs)
{
    int differing = 0;

    for (int y = 0; y < lhs.height(); ++y) {
        const QRgb *a = reinterpret_cast<const QRgb *>(lhs.constScanLine(y));
        const QRgb *b = reinterpret_cast<const QRgb *>(rhs.constScanLine(y));

        for (int x = 0; x < lhs.width(); ++x) {
            if (qAbs(qRed(a[x]) - qRed(b[x])) > kChannelTolerance ||
                qAbs(qGreen(a[x]) - qGreen(b[x])) > kChannelTolerance ||
                qAbs(qBlue(a[x]) - qBlue(b[x])) > kChannelTolerance) {
                ++differing;
            }
        }
    }

    return differing;
}

/*!
 * Runs one demo to completion and leaves its capture at @p screenshotPath.
 * Returns false with a message in @p error that is meant to be shown as is.
 */
bool runDemo(const QString &executable, const QString &screenshotPath, const QString &logName, QString &error)
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

    QProcess process;
    process.setProcessEnvironment(environment);
    process.setProgram(demo.absoluteFilePath());
    process.setArguments(QStringList{QStringLiteral("--screenshot"), screenshotPath,
                                     QStringLiteral("--screenshot-delay"), QString::fromLatin1(kScreenshotDelay)});
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
    logFile.open(QIODevice::ReadOnly);
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
#ifdef DEMO_DOCK_EXECUTABLE
    void dockDemoRenders();
    void dockDemoMatchesGolden();
#endif
private:
    QImage m_ribbonScreenshot;
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
