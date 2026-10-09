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
    void consoleOnly();
    void writeWithCategory();
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
