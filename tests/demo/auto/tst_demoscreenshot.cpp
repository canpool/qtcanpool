/*!
 * Visual regression for the demos (roadmap task B4).
 *
 * The demo renders itself - it knows how, through its own --screenshot switch,
 * so the test does not have to drive the window - and the result is checked in
 * two steps. The sanity check (window size, "something was drawn") runs
 * everywhere and is the actual regression guard. The golden comparison is
 * opt-in (QTCANPOOL_VISUAL_BASELINE=1): a screenshot is only comparable
 * between the machine that produced the golden and the same machine later, so
 * CI - with its own fonts and theme - must never judge one, and this test must
 * not gate.
 *
 * The golden is per platform (`tests/demo/golden/<CMAKE_SYSTEM_NAME>/`). To
 * (re)create one, look at the screenshot first and then run with
 * QTCANPOOL_UPDATE_GOLDEN=1. The demo runs offscreen by default on every
 * platform: on Linux/macOS that is the only headless option, and on Windows
 * the offscreen plugin has no font database (every glyph is a box) but the
 * render is otherwise complete and perfectly deterministic - which is exactly
 * what a golden wants. Set QT_QPA_PLATFORM to compare against a real desktop
 * session instead.
 *
 * Paths arrive as compile definitions from tests/CMakeLists.txt; there is no
 * reasonable way to guess them at runtime (the demo and the test do not share an
 * output directory on every platform).
 */

#include <QtCore/QDir>
#include <QtCore/QFile>
#include <QtCore/QFileInfo>
#include <QtCore/QProcess>
#include <QtCore/QProcessEnvironment>
#include <QtCore/QSet>
#include <QtGui/QImage>
#include <QtTest/QtTest>

namespace
{

/*!
 * Font rasterisation moves a pixel here and there between two machines, so a
 * handful of differing pixels must not fail the run. A real regression - a
 * stylesheet that stopped applying, a layout that collapsed - moves orders of
 * magnitude more.
 */
constexpr int kChannelTolerance = 12;
constexpr double kMaxDiffPercent = 0.5;

/*!
 * How long the demo is given after show() before it captures itself. The ribbon
 * needs its layout, theme and stylesheet to settle.
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

}   // namespace

class tst_DemoScreenshot : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase();

    void ribbonDemoRenders();
    void ribbonDemoMatchesGolden();
private:
    QString m_screenshotPath;
    QImage m_screenshot;
};

/*!
 * Runs the demo once; both test functions work on the same capture.
 */
void tst_DemoScreenshot::initTestCase()
{
    const QFileInfo demo(QStringLiteral(DEMO_RIBBON_EXECUTABLE));
    QVERIFY2(demo.exists(), qPrintable(QStringLiteral("the demo is missing: %1").arg(demo.absoluteFilePath())));

    m_screenshotPath = QStringLiteral(DEMO_SCREENSHOT_PATH);
    QVERIFY(QDir().mkpath(QFileInfo(m_screenshotPath).absolutePath()));
    QFile::remove(m_screenshotPath);

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
    process.setArguments(QStringList{QStringLiteral("--screenshot"), m_screenshotPath,
                                     QStringLiteral("--screenshot-delay"), QString::fromLatin1(kScreenshotDelay)});
    // The demo is chatty (hundreds of style warnings per run), so its output
    // goes to files instead of pipes: nothing can fill up and block the child,
    // and the log survives for the failure messages below. This also keeps the
    // run off QProcess's Windows named-pipe path, which has been observed to
    // fail with ERROR_PIPE_BUSY on some machines.
    const QString outputDir = QFileInfo(m_screenshotPath).absolutePath();
    const QString logPath = outputDir + QStringLiteral("/ribbondemo.log");
    process.setStandardInputFile(QProcess::nullDevice());
    process.setStandardOutputFile(logPath);
    process.setStandardErrorFile(logPath);
    process.start();

    QVERIFY2(process.waitForStarted(60000), "the demo did not start");
    QVERIFY2(process.waitForFinished(180000), "the demo did not finish");

    QFile logFile(logPath);
    logFile.open(QIODevice::ReadOnly);
    const QString output = QString::fromLocal8Bit(logFile.readAll());

    QVERIFY2(process.exitStatus() == QProcess::NormalExit,
             qPrintable(QStringLiteral("the demo crashed:\n%1").arg(output)));
    QVERIFY2(process.exitCode() == 0,
             qPrintable(QStringLiteral("the demo exited with %1:\n%2").arg(process.exitCode()).arg(output)));
    QVERIFY2(QFileInfo::exists(m_screenshotPath),
             qPrintable(QStringLiteral("no screenshot was written:\n%1").arg(output)));

    m_screenshot = loadAsArgb32(m_screenshotPath);
    QVERIFY2(!m_screenshot.isNull(),
             qPrintable(QStringLiteral("the screenshot could not be read: %1").arg(m_screenshotPath)));
}

/*!
 * Catches the failures that are worth catching on every run: a demo that starts
 * but renders nothing, or a window that lost its geometry. Whether the pixels
 * are the *right* pixels is the golden's business.
 */
void tst_DemoScreenshot::ribbonDemoRenders()
{
    QVERIFY2(m_screenshot.width() >= 800 && m_screenshot.height() >= 600,
             qPrintable(
                 QStringLiteral("implausible window size %1x%2").arg(m_screenshot.width()).arg(m_screenshot.height())));

    // "Not a flat colour" is a cheap proxy for "something was drawn": a blank
    // window is one or two colours, while the real thing measures in the
    // hundreds even at its worst (Windows' offscreen plugin has no fonts, and
    // the box-glyph capture still measures ~300). Sampled every other pixel;
    // the count only has to be a lower bound.
    QSet<QRgb> colours;

    for (int y = 0; y < m_screenshot.height() && colours.size() < 100; y += 2) {
        const QRgb *line = reinterpret_cast<const QRgb *>(m_screenshot.constScanLine(y));

        for (int x = 0; x < m_screenshot.width() && colours.size() < 100; x += 2) {
            colours.insert(line[x]);
        }
    }

    QVERIFY2(
        colours.size() >= 100,
        qPrintable(QStringLiteral("only %1 distinct colours - the demo rendered a blank window").arg(colours.size())));
}

void tst_DemoScreenshot::ribbonDemoMatchesGolden()
{
    const QString goldenPath = QStringLiteral(DEMO_GOLDEN_PATH);

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
        QVERIFY2(QFile::copy(m_screenshotPath, goldenPath),
                 qPrintable(QStringLiteral("the golden could not be written: %1").arg(goldenPath)));
        QSKIP("golden updated - review the screenshot before committing it");
    }

    if (!QFileInfo::exists(goldenPath)) {
        QSKIP(qPrintable(QStringLiteral("no golden for this platform: %1 (the capture is at %2; "
                                        "QTCANPOOL_UPDATE_GOLDEN=1 creates one)")
                             .arg(goldenPath)
                             .arg(m_screenshotPath)));
    }

    const QImage golden = loadAsArgb32(goldenPath);
    QVERIFY2(!golden.isNull(), qPrintable(QStringLiteral("the golden could not be read: %1").arg(goldenPath)));

    QVERIFY2(golden.size() == m_screenshot.size(),
             qPrintable(QStringLiteral("the window size changed: golden %1x%2, now %3x%4")
                            .arg(golden.width())
                            .arg(golden.height())
                            .arg(m_screenshot.width())
                            .arg(m_screenshot.height())));

    const int differing = countDifferingPixels(golden, m_screenshot);
    const double percent = 100.0 * differing / (golden.width() * golden.height());

    QVERIFY2(percent <= kMaxDiffPercent, qPrintable(QStringLiteral("%1% of the pixels differ from %2 (budget %3%, "
                                                                   "channel tolerance %4); the capture is at %5")
                                                        .arg(percent, 0, 'f', 3)
                                                        .arg(goldenPath)
                                                        .arg(kMaxDiffPercent)
                                                        .arg(kChannelTolerance)
                                                        .arg(m_screenshotPath)));
}

QTEST_GUILESS_MAIN(tst_DemoScreenshot)

#include "tst_demoscreenshot.moc"
