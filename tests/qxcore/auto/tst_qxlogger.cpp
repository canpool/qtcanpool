#include "tst_global.h"

#include "qxcore/qxlogger.h"

#include <QDebug>
#include <QFile>
#include <QFileInfo>
#include <QTemporaryDir>

QX_CORE_USE_NAMESPACE

namespace
{

QString readAll(const QString &fileName)
{
    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly)) {
        return QString();
    }
    return QString::fromUtf8(file.readAll());
}

int countLines(const QString &fileName)
{
    const QString text = readAll(fileName);
    return text.isEmpty() ? 0 : text.count(QLatin1Char('\n'));
}

}   // namespace

class tst_QxLogger : public QObject
{
    Q_OBJECT
private slots:
    void init();
    void cleanup();

    void fileOutput();
    void levelFilter();
    void rotation();
    void rotationIsOffAtZeroSize();
    void consoleOnly();
    void unopenableFileKeepsConsoleOutput();
    void writeWithCategory();
    void writeTakesACategoryName();
    void shutdownRestoresHandler();
};

/*!
 * Each test starts from a clean logger. init() is empty on purpose: the logger
 * is process-wide state, so a leaked configuration would be a test bug, not a
 * fixture issue.
 */
void tst_QxLogger::init()
{
}

void tst_QxLogger::cleanup()
{
    QxLogger::shutdown();
}

void tst_QxLogger::fileOutput()
{
    QTemporaryDir dir;
    QVERIFY(dir.isValid());

    const QString logFile = dir.filePath("app.log");
    QxLogger::Options options;
    options.fileName = logFile;
    options.toConsole = false;
    QVERIFY(QxLogger::init(options));
    QVERIFY(QxLogger::isEnabled());
    QCOMPARE(QxLogger::fileName(), logFile);

    qDebug() << "debug message";
    qWarning() << "warning message";
    QxLogger::flush();

    const QString text = readAll(logFile);
    QVERIFY(text.contains("[D]"));
    QVERIFY(text.contains("[W]"));
    QVERIFY(text.contains("debug message"));
    QVERIFY(text.contains("warning message"));
    // The default category Qt reports for qDebug()/qWarning().
    QVERIFY(text.contains("default:"));
    QCOMPARE(countLines(logFile), 2);
}

void tst_QxLogger::levelFilter()
{
    QTemporaryDir dir;
    QVERIFY(dir.isValid());

    const QString logFile = dir.filePath("filter.log");
    QxLogger::Options options;
    options.fileName = logFile;
    options.level = QxLogger::Warning;
    options.toConsole = false;
    QVERIFY(QxLogger::init(options));

    qDebug() << "dropped";
    qInfo() << "dropped too";
    qWarning() << "kept";
    QxLogger::flush();

    const QString text = readAll(logFile);
    QVERIFY(!text.contains("dropped"));
    QVERIFY(text.contains("kept"));
    QCOMPARE(countLines(logFile), 1);
}

void tst_QxLogger::rotation()
{
    QTemporaryDir dir;
    QVERIFY(dir.isValid());

    const QString logFile = dir.filePath("rotate.log");
    QxLogger::Options options;
    options.fileName = logFile;
    options.maxFileSize = 256;
    options.maxBackups = 2;
    options.toConsole = false;
    QVERIFY(QxLogger::init(options));

    for (int i = 0; i < 40; ++i) {
        qWarning() << "rotation line" << i << QString(60, QLatin1Char('x'));
    }
    QxLogger::flush();

    // The active file was rotated at least once and stays within the limit.
    QVERIFY(QFile::exists(logFile + ".1"));
    QVERIFY(QFileInfo(logFile).size() <= options.maxFileSize);

    // Older backups are dropped, so at most maxBackups files are kept besides
    // the active one.
    QVERIFY(QFile::exists(logFile + ".2"));
    QVERIFY(!QFile::exists(logFile + ".3"));
}

/*!
 * Options::maxFileSize says a value of zero or less disables rotation. That is
 * a promise a caller can rely on to keep one growing file instead of a set of
 * numbered ones, so it is pinned: nothing is rotated away and nothing is lost.
 */
void tst_QxLogger::rotationIsOffAtZeroSize()
{
    QTemporaryDir dir;
    QVERIFY(dir.isValid());

    const QString logFile = dir.filePath("grow.log");
    QxLogger::Options options;
    options.fileName = logFile;
    options.maxFileSize = 0;
    options.toConsole = false;
    QVERIFY(QxLogger::init(options));

    for (int i = 0; i < 40; ++i) {
        qWarning() << "growing line" << i << QString(60, QLatin1Char('x'));
    }
    QxLogger::flush();

    QVERIFY(!QFile::exists(logFile + ".1"));
    QCOMPARE(countLines(logFile), 40);
    QVERIFY(QFileInfo(logFile).size() > 256);
}

void tst_QxLogger::consoleOnly()
{
    QxLogger::Options options;
    options.toConsole = false;   // keep the test output clean
    QVERIFY(QxLogger::init(options));

    QVERIFY(QxLogger::isEnabled());
    QCOMPARE(QxLogger::fileName(), QString());

    qWarning() << "no file was configured";
    QxLogger::flush();   // must not crash without a file
}

/*!
 * A log file that cannot be opened is not a failure of the logger. The message
 * handler was installed before the file was touched, so messages are still
 * captured - which is what the header means by "console output is still
 * active" - and only fileName() reports that there is no file behind it.
 */
void tst_QxLogger::unopenableFileKeepsConsoleOutput()
{
    QTemporaryDir dir;
    QVERIFY(dir.isValid());

    QxLogger::Options options;
    options.fileName = dir.path();   // a directory: the open has to fail
    options.toConsole = false;

    QVERIFY(!QxLogger::init(options));
    QVERIFY(QxLogger::isEnabled());
    QCOMPARE(QxLogger::fileName(), QString());

    // The handler is installed and has no file to write to: this must be a
    // no-op rather than a crash.
    qWarning() << "nowhere to go";
    QxLogger::flush();
}

void tst_QxLogger::writeWithCategory()
{
    QTemporaryDir dir;
    QVERIFY(dir.isValid());

    const QString logFile = dir.filePath("category.log");
    QxLogger::Options options;
    options.fileName = logFile;
    options.toConsole = false;
    QVERIFY(QxLogger::init(options));

    QLoggingCategory category("qxcore.test");
    QxLogger::write(QxLogger::Critical, category, QStringLiteral("categorized"));
    QxLogger::flush();

    const QString text = readAll(logFile);
    QVERIFY(text.contains("[C]"));
    QVERIFY(text.contains("qxcore.test:"));
    QVERIFY(text.contains("categorized"));
}

/*!
 * The other write() overload takes the category by name. It is what the
 * installed handler calls, so it is reachable without a QLoggingCategory in
 * hand - and the level filter has to apply to it exactly as it does to a
 * message that went through qDebug() and friends.
 */
void tst_QxLogger::writeTakesACategoryName()
{
    QTemporaryDir dir;
    QVERIFY(dir.isValid());

    const QString logFile = dir.filePath("by-name.log");
    QxLogger::Options options;
    options.fileName = logFile;
    options.level = QxLogger::Info;
    options.toConsole = false;
    QVERIFY(QxLogger::init(options));

    QxLogger::write(QxLogger::Info, QStringLiteral("qxcore.by-name"), QStringLiteral("named"));
    QxLogger::write(QxLogger::Debug, QStringLiteral("qxcore.by-name"), QStringLiteral("filtered out"));
    QxLogger::flush();

    const QString text = readAll(logFile);
    QVERIFY(text.contains("[I]"));
    QVERIFY(text.contains("qxcore.by-name:"));
    QVERIFY(text.contains("named"));
    QVERIFY(!text.contains("filtered out"));
    QCOMPARE(countLines(logFile), 1);
}

void tst_QxLogger::shutdownRestoresHandler()
{
    QTemporaryDir dir;
    QVERIFY(dir.isValid());

    QxLogger::Options options;
    options.fileName = dir.filePath("shutdown.log");
    options.toConsole = false;
    QVERIFY(QxLogger::init(options));
    QVERIFY(QxLogger::isEnabled());

    QxLogger::shutdown();
    QVERIFY(!QxLogger::isEnabled());
    QCOMPARE(QxLogger::fileName(), QString());

    // Logging after shutdown goes back to Qt's own handler and must not crash.
    qWarning() << "after shutdown";

    // A second shutdown is a no-op.
    QxLogger::shutdown();
    QVERIFY(!QxLogger::isEnabled());
}

TEST_ADD(tst_QxLogger)

#include "tst_qxlogger.moc"
