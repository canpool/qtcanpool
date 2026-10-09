#include "tst_global.h"

#include "qxtheme/qxtheme.h"
#include "qxtheme/qxthememanager.h"
#include "qxtheme/qxthemepalette.h"

#include <QColor>
#include <QFile>
#include <QPalette>
#include <QSignalSpy>
#include <QTemporaryDir>

QX_THEME_USE_NAMESPACE
QX_CORE_USE_NAMESPACE

/*!
 * The manager is a process-wide singleton, so every case starts by handing it a
 * brand new settings file. That keeps the cases independent from the order they
 * run in without reaching into the private state.
 */
class tst_QxTheme : public QObject
{
    Q_OBJECT
private slots:
    void initTestCase();

    void themeIds();
    void themeLookup();
    void palettes();
    void styleSheetSources();
    void applyTheme();
    void persistence();
    void followSystem();
    void systemThemePair();
private:
    void resetSettings();

    QTemporaryDir m_dir;
    QString m_settingsFile;
    int m_sequence = 0;
};

void tst_QxTheme::initTestCase()
{
    QVERIFY(m_dir.isValid());
}

void tst_QxTheme::resetSettings()
{
    QVERIFY(m_dir.isValid());
    m_settingsFile = m_dir.filePath(QStringLiteral("theme-%1.ini").arg(++m_sequence));
    QxThemeManager::instance()->setSettings(new QxSettings(m_settingsFile, QSettings::IniFormat));
}

void tst_QxTheme::themeIds()
{
    // Ids end up in the configuration file, so they are part of the contract.
    QCOMPARE(themeId(LightYellow), QStringLiteral("light-yellow"));
    QCOMPARE(themeId(LightOffice2013), QStringLiteral("light-office2013"));
    QCOMPARE(themeId(LightClassic), QStringLiteral("light-classic"));
    QCOMPARE(themeId(LightFancy), QStringLiteral("light-fancy"));
    QCOMPARE(themeId(DarkWps), QStringLiteral("dark-wps"));
    QCOMPARE(themeId(DarkOfficePlus), QStringLiteral("dark-officeplus"));
    QCOMPARE(themeId(Custom), QStringLiteral("custom"));

    // Display names are for a settings dialog, so only their presence is fixed.
    const Theme themes[] = {LightYellow, LightOffice2013, LightClassic, LightFancy, DarkWps, DarkOfficePlus, Custom};
    for (Theme theme : themes) {
        QVERIFY(!themeName(theme).isEmpty());
    }

    // Custom ships no resource of its own.
    QCOMPARE(themeStyleSheetPath(Custom), QString());
    QVERIFY(themeStyleSheetPath(LightYellow).startsWith(QStringLiteral(":/")));
    QVERIFY(themeStyleSheetPath(DarkWps).endsWith(QStringLiteral("/dark_wps.css")));
    QVERIFY(themeStyleSheetPath(DarkOfficePlus).endsWith(QStringLiteral("/dark_officeplus.css")));

    // The built-in sheets live in the qxribbon resource bundle. This test only
    // links qxtheme, so either the resource is absent or it is real CSS; what
    // must not happen is a garbage string.
    const QString builtIn = themeStyleSheet(DarkWps);
    QVERIFY(builtIn.isEmpty() || builtIn.contains(QStringLiteral("QWidget")));
    QCOMPARE(themeStyleSheet(Custom), QString());
}

void tst_QxTheme::themeLookup()
{
    const Theme themes[] = {LightYellow, LightOffice2013, LightClassic, LightFancy, DarkWps, DarkOfficePlus, Custom};
    for (Theme theme : themes) {
        QCOMPARE(themeFromId(themeId(theme)), theme);
    }

    QCOMPARE(themeFromId(QStringLiteral("does-not-exist")), LightYellow);
    QCOMPARE(themeFromId(QStringLiteral("does-not-exist"), DarkWps), DarkWps);
    QCOMPARE(themeFromId(QString()), LightYellow);

    QCOMPARE(isDarkTheme(DarkWps), true);
    QCOMPARE(isDarkTheme(DarkOfficePlus), true);
    QCOMPARE(isDarkTheme(LightClassic), false);
    QCOMPARE(isDarkTheme(LightFancy), false);
    QCOMPARE(isDarkTheme(Custom), false);
}

void tst_QxTheme::palettes()
{
    const QPalette light = lightPalette();
    const QPalette dark = darkPalette();

    // A dark theme has to be dark for the surfaces and light for the text.
    QVERIFY(dark.color(QPalette::Window).lightness() < 128);
    QVERIFY(dark.color(QPalette::Text).lightness() > 128);
    QVERIFY(light.color(QPalette::Window).lightness() > dark.color(QPalette::Window).lightness());

    // Disabled text is muted, never identical to the enabled one.
    QCOMPARE(light.color(QPalette::Disabled, QPalette::Text), QColor(0xa0, 0xa0, 0xa0));
    QVERIFY(light.color(QPalette::Disabled, QPalette::Text) != light.color(QPalette::Text));
    QVERIFY(dark.color(QPalette::Disabled, QPalette::Text) != dark.color(QPalette::Text));

    // A theme routes to the palette that matches its brightness.
    QCOMPARE(palette(DarkWps).color(QPalette::Window), dark.color(QPalette::Window));
    QCOMPARE(palette(DarkOfficePlus).color(QPalette::Window), dark.color(QPalette::Window));
    QCOMPARE(palette(LightFancy).color(QPalette::Window), light.color(QPalette::Window));
    QCOMPARE(palette(Custom).color(QPalette::Window), light.color(QPalette::Window));
}

void tst_QxTheme::styleSheetSources()
{
    resetSettings();
    QxThemeManager *manager = QxThemeManager::instance();

    // Inline content has the highest priority.
    manager->setStyleSheet(DarkWps, QStringLiteral("QWidget { color: red; }"));
    QCOMPARE(manager->styleSheet(DarkWps), QStringLiteral("QWidget { color: red; }"));

    // Without inline content the registered file is read. Written in binary
    // mode so the read-back is byte-identical on every platform.
    const QString fileName = m_dir.filePath(QStringLiteral("custom.css"));
    QFile file(fileName);
    QVERIFY(file.open(QIODevice::WriteOnly));
    file.write("QWidget { color: blue; }\n");
    file.close();

    manager->setStyleSheetFile(LightClassic, fileName);
    QCOMPARE(manager->styleSheet(LightClassic), QStringLiteral("QWidget { color: blue; }\n"));

    // Inline content replaces the file registration.
    manager->setStyleSheet(LightClassic, QStringLiteral("inline"));
    QCOMPARE(manager->styleSheet(LightClassic), QStringLiteral("inline"));

    // Clearing it falls back to the built-in resource, which this test does not
    // link, so the override has to be gone either way.
    manager->setStyleSheet(LightClassic, QString());
    QVERIFY(manager->styleSheet(LightClassic) != QStringLiteral("inline"));

    // A file that does not exist is reported as "no style sheet", not as a crash.
    manager->setStyleSheetFile(LightFancy, m_dir.filePath(QStringLiteral("absent.css")));
    QCOMPARE(manager->styleSheet(LightFancy), QString());
}

void tst_QxTheme::applyTheme()
{
    resetSettings();
    QxThemeManager *manager = QxThemeManager::instance();

    // Keep the applied sheet recognisable regardless of the built-in resources.
    manager->setStyleSheet(DarkWps, QStringLiteral("QWidget { color: #f0f0f0; }"));
    manager->setStyleSheet(LightClassic, QStringLiteral("QWidget { color: #0f0f0f; }"));

    QSignalSpy spy(manager, &QxThemeManager::themeChanged);
    manager->setTheme(DarkWps);

    QCOMPARE(spy.count(), 1);
    QCOMPARE(manager->theme(), DarkWps);
    QCOMPARE(manager->isDark(), true);

    // Both the palette and the style sheet have to reach the application.
    QCOMPARE(qApp->palette().color(QPalette::Window), darkPalette().color(QPalette::Window));
    QCOMPARE(qApp->styleSheet(), QStringLiteral("QWidget { color: #f0f0f0; }"));

    manager->setTheme(LightClassic);

    QCOMPARE(manager->theme(), LightClassic);
    QCOMPARE(manager->isDark(), false);
    QCOMPARE(qApp->palette().color(QPalette::Window), lightPalette().color(QPalette::Window));
    QCOMPARE(qApp->styleSheet(), QStringLiteral("QWidget { color: #0f0f0f; }"));
    QCOMPARE(spy.count(), 2);
}

void tst_QxTheme::persistence()
{
    resetSettings();
    QxThemeManager *manager = QxThemeManager::instance();
    manager->setTheme(DarkOfficePlus);

    // The choice is written under a stable key, not as a raw enum value.
    QxSettings reader(m_settingsFile, QSettings::IniFormat);
    reader.beginGroup(QStringLiteral("ui"));
    QCOMPARE(reader.stringValue(QStringLiteral("theme")), QStringLiteral("dark-officeplus"));
    reader.endGroup();

    // Handing the same file back restores the stored theme.
    manager->setSettings(new QxSettings(m_settingsFile, QSettings::IniFormat));
    QCOMPARE(manager->theme(), DarkOfficePlus);
}

void tst_QxTheme::followSystem()
{
    resetSettings();
    QxThemeManager *manager = QxThemeManager::instance();

    QCOMPARE(manager->followSystemTheme(), false);
    QCOMPARE(manager->systemLightTheme(), LightOffice2013);
    QCOMPARE(manager->systemDarkTheme(), DarkOfficePlus);

    QSignalSpy spy(manager, &QxThemeManager::followSystemThemeChanged);
    manager->setFollowSystemTheme(true);

    QCOMPARE(spy.count(), 1);
    QCOMPARE(manager->followSystemTheme(), true);

    // Following selects one of the configured pair, never an unrelated theme.
    QVERIFY(manager->theme() == manager->systemLightTheme() || manager->theme() == manager->systemDarkTheme());
    QCOMPARE(manager->isDark(), manager->theme() == manager->systemDarkTheme());

    // The flag survives a restart.
    QxSettings reader(m_settingsFile, QSettings::IniFormat);
    reader.beginGroup(QStringLiteral("ui"));
    QCOMPARE(reader.boolValue(QStringLiteral("followSystemTheme")), true);
    reader.endGroup();

    // Turning it off keeps the theme that was on screen and makes it explicit.
    manager->setFollowSystemTheme(false);
    QCOMPARE(manager->followSystemTheme(), false);
    QCOMPARE(spy.count(), 2);

    QxSettings after(m_settingsFile, QSettings::IniFormat);
    after.beginGroup(QStringLiteral("ui"));
    QCOMPARE(after.stringValue(QStringLiteral("theme")), themeId(manager->theme()));
    after.endGroup();
}

void tst_QxTheme::systemThemePair()
{
    resetSettings();
    QxThemeManager *manager = QxThemeManager::instance();

    manager->setFollowSystemTheme(true);
    QSignalSpy spy(manager, &QxThemeManager::themeChanged);

    manager->setSystemThemePair(LightClassic, DarkWps);

    QCOMPARE(manager->systemLightTheme(), LightClassic);
    QCOMPARE(manager->systemDarkTheme(), DarkWps);
    QVERIFY(manager->theme() == LightClassic || manager->theme() == DarkWps);

    // The pair moved to themes that were not on screen before, so the change is
    // applied and announced exactly once.
    QCOMPARE(spy.count(), 1);

    QxSettings reader(m_settingsFile, QSettings::IniFormat);
    reader.beginGroup(QStringLiteral("ui"));
    QCOMPARE(reader.stringValue(QStringLiteral("systemLightTheme")), QStringLiteral("light-classic"));
    QCOMPARE(reader.stringValue(QStringLiteral("systemDarkTheme")), QStringLiteral("dark-wps"));
    reader.endGroup();

    manager->setFollowSystemTheme(false);
}

TEST_ADD(tst_QxTheme)

#include "tst_qxtheme.moc"
