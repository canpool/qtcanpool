/*!
 * The IDE-style sample application (roadmap acceptance #15, plan task D1).
 *
 * The demo is run, never linked. That is the whole point of it: an application
 * assembled out of modules it does not know, so linking it into a test would
 * compile the claim away. IdeShellDemo's own --check switch loads the plugins the
 * way the window does, prints what they composed and leaves without a screen, and
 * what is asserted here is that the composition is real - modules found by
 * metadata alone, a declared dependency ordering the run, the pages and docks
 * coming from them, and the host's connection between two plugins that the
 * application never linked.
 *
 * The executable path arrives as a compile definition from tests/CMakeLists.txt;
 * the demo and the test do not share an output directory on every platform, so
 * there is no reasonable way to guess it at run time.
 */

#include <QtCore/QDir>
#include <QtCore/QFile>
#include <QtCore/QFileInfo>
#include <QtCore/QProcess>
#include <QtCore/QProcessEnvironment>
#include <QtCore/QStringList>
#include <QtTest/QtTest>

namespace
{
QString readAll(const QString &path)
{
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly)) {
        return QString();
    }
    return QString::fromLocal8Bit(file.readAll());
}

/*! The value printed after "<key>: " on the line that starts with \a key. */
QString valueOf(const QString &composition, const QString &key)
{
    for (const QString &line : composition.split(QLatin1Char('\n'))) {
        if (line.startsWith(key + QStringLiteral(": "))) {
            return line.mid(key.size() + 2);
        }
    }
    return QString();
}

/*!
 * The line describing the plugin \a id, or an empty string when the composition
 * does not mention it. Fields are space separated (the id and the state are
 * padded), so the first field is the whole of what the lookup needs.
 */
QString lineOf(const QString &composition, const QString &id)
{
    for (const QString &line : composition.split(QLatin1Char('\n'))) {
        const QStringList fields = line.split(QLatin1Char(' '), Qt::SkipEmptyParts);
        if (fields.value(0) == id) {
            return line;
        }
    }
    return QString();
}
}   // namespace

class tst_IdeShellDemo : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase();

    void startsAndFindsTheModules();
    void dependencyOrderedTheRun();
    void modulesFilledTheShell();
    void hostWiredTwoPluginsItNeverLinked();
private:
    QString m_composition;
};

void tst_IdeShellDemo::initTestCase()
{
    const QFileInfo demo(QStringLiteral(DEMO_IDESHELL_EXECUTABLE));
    QVERIFY2(demo.exists(), qPrintable(QStringLiteral("the demo is missing: %1").arg(demo.absoluteFilePath())));

    QProcessEnvironment environment = QProcessEnvironment::systemEnvironment();
    // The demo builds a window even in --check, so it needs a platform plugin;
    // offscreen is the one every platform here has without a display.
    environment.insert(QStringLiteral("QT_QPA_PLATFORM"), QStringLiteral("offscreen"));

    // The composition and the noise go to separate files. The demo is chatty on
    // stderr (style warnings), and keeping that off the stream being parsed - and
    // off a pipe that could fill up and block the child - is what makes the
    // parsing below about the demo's report rather than about its chatter.
    const QString outputPath = QDir(QDir::tempPath()).absoluteFilePath(QStringLiteral("ideshelldemo.out"));
    const QString errorPath = QDir(QDir::tempPath()).absoluteFilePath(QStringLiteral("ideshelldemo.err"));

    QProcess process;
    process.setProcessEnvironment(environment);
    process.setProgram(demo.absoluteFilePath());
    process.setArguments(QStringList{QStringLiteral("--check")});
    process.setStandardInputFile(QProcess::nullDevice());
    process.setStandardOutputFile(outputPath);
    process.setStandardErrorFile(errorPath);
    process.start();

    QVERIFY2(process.waitForStarted(60000), "the demo did not start");
    QVERIFY2(process.waitForFinished(180000), "the demo did not finish");

    m_composition = readAll(outputPath);
    const QString chatter = readAll(errorPath);
    const QString both = m_composition + QStringLiteral("\n--- stderr ---\n") + chatter;

    QVERIFY2(process.exitStatus() == QProcess::NormalExit,
             qPrintable(QStringLiteral("the demo crashed:\n%1").arg(both)));
    // --check fails when it found nothing at all or when a plugin failed, which
    // is exactly the state this suite is here to catch.
    QVERIFY2(process.exitCode() == 0,
             qPrintable(QStringLiteral("the demo exited with %1:\n%2").arg(process.exitCode()).arg(both)));
}

/*! Metadata alone was enough: nothing in the application names a module. */
void tst_IdeShellDemo::startsAndFindsTheModules()
{
    for (const QString &id : {QStringLiteral("output"), QStringLiteral("notebook"), QStringLiteral("filetree")}) {
        const QString line = lineOf(m_composition, id);
        QVERIFY2(!line.isEmpty(), qPrintable(QStringLiteral("'%1' is missing from:\n%2").arg(id).arg(m_composition)));
        QVERIFY2(line.contains(QStringLiteral("initialized")), qPrintable(line));
    }
}

/*! filetree requires output, and the manager had to honour that. */
void tst_IdeShellDemo::dependencyOrderedTheRun()
{
    const QString order = valueOf(m_composition, QStringLiteral("load order"));
    const QStringList steps = order.split(QStringLiteral(" -> "), Qt::SkipEmptyParts);

    const int outputAt = steps.indexOf(QStringLiteral("output"));
    const int fileTreeAt = steps.indexOf(QStringLiteral("filetree"));
    QVERIFY2(outputAt >= 0 && fileTreeAt >= 0, qPrintable(QStringLiteral("load order was: %1").arg(order)));
    QVERIFY2(outputAt < fileTreeAt, qPrintable(QStringLiteral("load order was: %1").arg(order)));
}

/*! The names on screen are the modules' own: a page and two docks. */
void tst_IdeShellDemo::modulesFilledTheShell()
{
    const QString pages = valueOf(m_composition, QStringLiteral("pages"));
    QVERIFY2(pages.contains(QStringLiteral("notebook")), qPrintable(QStringLiteral("pages were: %1").arg(pages)));

    const QString docks = valueOf(m_composition, QStringLiteral("docks"));
    QVERIFY2(docks.contains(QStringLiteral("output")), qPrintable(QStringLiteral("docks were: %1").arg(docks)));
    QVERIFY2(docks.contains(QStringLiteral("filetree")), qPrintable(QStringLiteral("docks were: %1").arg(docks)));
}

/*!
 * The host-side rule - filetree reports what it opened, output takes a line -
 * was applied, by member name, to two plugins the application never linked.
 */
void tst_IdeShellDemo::hostWiredTwoPluginsItNeverLinked()
{
    QCOMPARE(valueOf(m_composition, QStringLiteral("wiring")), QStringLiteral("ok"));
}

QTEST_GUILESS_MAIN(tst_IdeShellDemo)

#include "tst_ideshelldemo.moc"
