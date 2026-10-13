#include "tst_global.h"

#include "qxcore/qxsettings.h"

#include <QTemporaryDir>

QX_CORE_USE_NAMESPACE

class tst_QxSettings : public QObject
{
    Q_OBJECT
private slots:
    void defaults();
    void typedAccessors();
    void groups();
    void removeAndClear();
    void persistence();
    void migration();
    void versionKeyIsConfigurable();
    void underlyingSettingsIsReachable();
};

void tst_QxSettings::defaults()
{
    QTemporaryDir dir;
    QVERIFY(dir.isValid());

    QxSettings settings(dir.filePath("defaults.ini"), QSettings::IniFormat);

    // Reading a missing key hands back the caller's default.
    QCOMPARE(settings.contains("missing"), false);
    QCOMPARE(settings.value("missing").isValid(), false);
    QCOMPARE(settings.stringValue("missing", QStringLiteral("fallback")), QStringLiteral("fallback"));
    QCOMPARE(settings.intValue("missing", 42), 42);
    QCOMPARE(settings.boolValue("missing", true), true);
    QCOMPARE(settings.stringListValue("missing", QStringList{QStringLiteral("a")}), QStringList{QStringLiteral("a")});
    QCOMPARE(settings.allKeys(), QStringList());
    QCOMPARE(settings.group(), QString());
}

void tst_QxSettings::typedAccessors()
{
    QTemporaryDir dir;
    QVERIFY(dir.isValid());

    QxSettings settings(dir.filePath("typed.ini"), QSettings::IniFormat);

    settings.setValue("text", QStringLiteral("value"));
    settings.setValue("number", 7);
    settings.setValue("flag", true);
    settings.setValue("list", QStringList{QStringLiteral("x"), QStringLiteral("y")});

    QCOMPARE(settings.stringValue("text"), QStringLiteral("value"));
    QCOMPARE(settings.intValue("number"), 7);
    QCOMPARE(settings.boolValue("flag"), true);
    QCOMPARE(settings.stringListValue("list"), QStringList({QStringLiteral("x"), QStringLiteral("y")}));
    QCOMPARE(settings.contains("number"), true);

    // Typed accessors convert, they do not validate the stored type.
    QCOMPARE(settings.stringValue("number"), QStringLiteral("7"));

    QSignalSpy spy(&settings, &QxSettings::valueChanged);
    settings.setValue("number", 8);
    QCOMPARE(spy.count(), 1);
    QCOMPARE(spy.first().at(0).toString(), QStringLiteral("number"));
    QCOMPARE(spy.first().at(1).toInt(), 8);
}

void tst_QxSettings::groups()
{
    QTemporaryDir dir;
    QVERIFY(dir.isValid());

    QxSettings settings(dir.filePath("groups.ini"), QSettings::IniFormat);
    QSignalSpy spy(&settings, &QxSettings::valueChanged);

    settings.beginGroup("window");
    QCOMPARE(settings.group(), QStringLiteral("window"));
    settings.setValue("width", 800);
    settings.endGroup();

    settings.beginGroup("main");
    settings.beginGroup("window");
    settings.setValue("width", 640);
    settings.endGroup();
    settings.endGroup();

    QCOMPARE(settings.intValue("window/width"), 800);
    QCOMPARE(settings.intValue("main/window/width"), 640);
    QCOMPARE(settings.allKeys(), QStringList({QStringLiteral("main/window/width"), QStringLiteral("window/width")}));

    // The signal reports the group-qualified key.
    QCOMPARE(spy.count(), 2);
    QCOMPARE(spy.at(0).at(0).toString(), QStringLiteral("window/width"));
    QCOMPARE(spy.at(1).at(0).toString(), QStringLiteral("main/window/width"));

    settings.beginGroup("window");
    settings.remove("width");
    settings.endGroup();
    QCOMPARE(settings.contains("window/width"), false);
    QCOMPARE(spy.count(), 3);
    QCOMPARE(spy.at(2).at(0).toString(), QStringLiteral("window/width"));
}

void tst_QxSettings::removeAndClear()
{
    QTemporaryDir dir;
    QVERIFY(dir.isValid());

    QxSettings settings(dir.filePath("clear.ini"), QSettings::IniFormat);

    settings.setValue("a", 1);
    settings.setValue("b", 2);
    QCOMPARE(settings.allKeys().count(), 2);

    settings.remove("a");
    QCOMPARE(settings.contains("a"), false);
    QCOMPARE(settings.contains("b"), true);

    settings.clear();
    QCOMPARE(settings.allKeys(), QStringList());
}

void tst_QxSettings::persistence()
{
    QTemporaryDir dir;
    QVERIFY(dir.isValid());

    const QString fileName = dir.filePath("persist.ini");
    {
        QxSettings settings(fileName, QSettings::IniFormat);
        QCOMPARE(settings.fileName(), fileName);
        settings.setValue("app/theme", QStringLiteral("dark"));
        settings.sync();
    }

    // A second instance reads what the first one wrote.
    QxSettings settings(fileName, QSettings::IniFormat);
    QCOMPARE(settings.stringValue("app/theme"), QStringLiteral("dark"));
    // The stored string does not convert to int, so toInt() yields 0 instead of
    // falling back to the caller's default.
    QCOMPARE(settings.intValue("app/theme", -1), 0);
    QCOMPARE(settings.version(), 0);
}

void tst_QxSettings::migration()
{
    QTemporaryDir dir;
    QVERIFY(dir.isValid());

    QxSettings settings(dir.filePath("migrate.ini"), QSettings::IniFormat);
    QCOMPARE(settings.versionKey(), QStringLiteral("version"));
    QCOMPARE(settings.version(), 0);

    QStringList order;
    settings.addMigration(1, [&order](QxSettings &s) {
        order << QStringLiteral("v1");
        s.setValue("renamed", s.value("old").toInt() * 2);
        s.remove("old");
    });
    settings.addMigration(2, [&order](QxSettings &s) {
        order << QStringLiteral("v2");
        s.setValue("added", true);
    });
    settings.addMigration(3, [&order](QxSettings &) {
        order << QStringLiteral("v3");
    });

    settings.setValue("old", 21);

    // Migrating to 2 applies the steps for 1 and 2, in ascending order.
    QCOMPARE(settings.migrate(2), 2);
    QCOMPARE(order, QStringList({QStringLiteral("v1"), QStringLiteral("v2")}));
    QCOMPARE(settings.version(), 2);
    QCOMPARE(settings.intValue("renamed"), 42);
    QCOMPARE(settings.contains("old"), false);
    QCOMPARE(settings.boolValue("added"), true);

    // The stored version is up to date, so nothing runs again.
    QCOMPARE(settings.migrate(2), 0);
    QCOMPARE(order.count(), 2);

    // A later target applies only the remaining step.
    QCOMPARE(settings.migrate(3), 1);
    QCOMPARE(order, QStringList({QStringLiteral("v1"), QStringLiteral("v2"), QStringLiteral("v3")}));
    QCOMPARE(settings.version(), 3);

    // A target that is not ahead of the stored version is a no-op, including
    // a "downgrade": the stored version stays where it is.
    QCOMPARE(settings.migrate(1), 0);
    QCOMPARE(settings.version(), 3);
}

/*!
 * "version" is a name an application may already be using for something else,
 * which is why setVersionKey() exists. Both version() and migrate() have to
 * follow the configured key rather than the default one.
 */
void tst_QxSettings::versionKeyIsConfigurable()
{
    QTemporaryDir dir;
    QVERIFY(dir.isValid());

    const QString fileName = dir.filePath("custom-key.ini");
    QxSettings settings(fileName, QSettings::IniFormat);
    QCOMPARE(settings.versionKey(), QStringLiteral("version"));

    settings.setVersionKey(QStringLiteral("layoutVersion"));
    QCOMPARE(settings.versionKey(), QStringLiteral("layoutVersion"));
    settings.setVersion(4);
    QCOMPARE(settings.version(), 4);
    settings.sync();

    // Stored under the configured key, and the default one stays untouched -
    // which is the whole point of letting the key be chosen.
    QxSettings reader(fileName, QSettings::IniFormat);
    QCOMPARE(reader.contains(QStringLiteral("layoutVersion")), true);
    QCOMPARE(reader.contains(QStringLiteral("version")), false);

    // migrate() reads and writes that same configured key.
    QxSettings migrating(fileName, QSettings::IniFormat);
    migrating.setVersionKey(QStringLiteral("layoutVersion"));
    QCOMPARE(migrating.version(), 4);
    int ran = 0;
    migrating.addMigration(5, [&ran](QxSettings &) {
        ++ran;
    });
    QCOMPARE(migrating.migrate(5), 1);
    QCOMPARE(ran, 1);
    QCOMPARE(migrating.version(), 5);
}

/*!
 * settings() hands back the QSettings the object owns. It is there so a caller
 * that needs something QxSettings does not wrap can still reach it, and the two
 * views have to agree about what is stored in either direction.
 */
void tst_QxSettings::underlyingSettingsIsReachable()
{
    QTemporaryDir dir;
    QVERIFY(dir.isValid());

    QxSettings settings(dir.filePath("underlying.ini"), QSettings::IniFormat);
    QVERIFY(settings.settings() != Q_NULLPTR);
    QCOMPARE(settings.fileName(), settings.settings()->fileName());

    settings.setValue(QStringLiteral("k"), 3);
    QCOMPARE(settings.settings()->value(QStringLiteral("k")).toInt(), 3);

    settings.settings()->setValue(QStringLiteral("k"), 4);
    QCOMPARE(settings.intValue(QStringLiteral("k")), 4);
}

TEST_ADD(tst_QxSettings)

#include "tst_qxsettings.moc"
