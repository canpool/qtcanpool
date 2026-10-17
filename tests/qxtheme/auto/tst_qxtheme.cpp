#include "tst_global.h"

#include "qxtheme/qxtheme.h"
#include "qxtheme/qxthememanager.h"
#include "qxtheme/qxthemepalette.h"

#include <QApplication>
#include <QColor>
#include <QPalette>
#include <QSignalSpy>
#include <QTemporaryDir>

QX_THEME_USE_NAMESPACE
QX_CORE_USE_NAMESPACE

namespace
{

const char *const kLight = "test-light";
const char *const kDark = "test-dark";
const char *const kBare = "test-bare";

QxThemeInfo makeTheme(const QString &id, const QString &name, bool dark, const QString &css = QString())
{
    QxThemeInfo info(id, name, dark);
    info.styleSheet = css;
    return info;
}

}   // namespace

/*!
 * The engine owns no theme of its own: what it can apply is exactly what the
 * application registered, addressed by the id the application chose. Every case
 * therefore starts by registering the few themes it addresses, which is also
 * the order a real application uses - restore the stored selection first,
 * register second.
 *
 * The manager is a process-wide singleton, so every case starts by handing it a
 * brand new settings file and by taking its own registrations back out. That
 * keeps the cases independent of the order they run in without reaching into
 * the private state.
 */
class tst_QxTheme : public QObject
{
    Q_OBJECT
private slots:
    void initTestCase();
    void init();

    void registering();
    void unknownThemeIsLeftAlone();
    void palettes();
    void styleSheetOfARegisteredTheme();
    void applyTheme();
    void persistence();
    void accentColor();
    void followSystem();
    void systemThemePair();
    void applyReinstallsWithoutAnnouncing();
    void refreshSystemThemeIsANoOpWhileNotFollowing();
    void settingsIsTheObjectSetSettingsTook();
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

void tst_QxTheme::init()
{
    resetSettings();

    QxThemeManager *manager = QxThemeManager::instance();
    const QStringList stale = manager->themeIds();
    for (const QString &id : stale) {
        manager->unregisterTheme(id);
    }

    manager->registerTheme(makeTheme(QString::fromLatin1(kLight), QStringLiteral("Test light"), false,
                                     QStringLiteral("QWidget { color: #0f0f0f; }")));
    manager->registerTheme(makeTheme(QString::fromLatin1(kDark), QStringLiteral("Test dark"), true,
                                     QStringLiteral("QWidget { color: #f0f0f0; }")));
    // A theme with nothing to paint is a legitimate registration.
    manager->registerTheme(makeTheme(QString::fromLatin1(kBare), QStringLiteral("Test bare"), false));
}

void tst_QxTheme::resetSettings()
{
    QVERIFY(m_dir.isValid());
    m_settingsFile = m_dir.filePath(QStringLiteral("theme-%1.ini").arg(++m_sequence));
    QxThemeManager::instance()->setSettings(new QxSettings(m_settingsFile, QSettings::IniFormat));
}

void tst_QxTheme::registering()
{
    QxThemeManager *manager = QxThemeManager::instance();

    // The three themes of init(), in the order they were given - the order a
    // menu lists them in.
    QCOMPARE(manager->themeIds().size(), 3);
    QCOMPARE(manager->themeIds().first(), QString::fromLatin1(kLight));
    QVERIFY(manager->hasTheme(QString::fromLatin1(kDark)));
    QVERIFY(!manager->hasTheme(QStringLiteral("never-registered")));
    QVERIFY(!manager->themeInfo(QStringLiteral("never-registered")).isValid());
    QCOMPARE(manager->themeInfo(QString::fromLatin1(kDark)).name, QStringLiteral("Test dark"));
    QCOMPARE(manager->themeInfo(QString::fromLatin1(kDark)).dark, true);

    // Registering an id again replaces what was stored under it, and does not
    // add a second entry.
    manager->registerTheme(makeTheme(QString::fromLatin1(kDark), QStringLiteral("Renamed"), true));
    QCOMPARE(manager->themeIds().size(), 3);
    QCOMPARE(manager->themeInfo(QString::fromLatin1(kDark)).name, QStringLiteral("Renamed"));

    // An info without an id cannot be addressed, so there is nothing to store.
    manager->registerTheme(QxThemeInfo());
    QCOMPARE(manager->themeIds().size(), 3);

    // Unregistering takes it out of the lookup and of the order alike.
    manager->unregisterTheme(QString::fromLatin1(kDark));
    QVERIFY(!manager->hasTheme(QString::fromLatin1(kDark)));
    QCOMPARE(manager->themeIds().size(), 2);
}

/*!
 * An id that was never registered is the state a restored configuration lands
 * in before the application registers anything, and it is also what a stale
 * configuration looks like. Applying it has to leave the application alone
 * rather than clear a style sheet somebody else installed.
 */
void tst_QxTheme::unknownThemeIsLeftAlone()
{
    QxThemeManager *manager = QxThemeManager::instance();
    qApp->setStyleSheet(QStringLiteral("QWidget { color: magenta; }"));
    const QPalette before = qApp->palette();

    manager->setTheme(QStringLiteral("never-registered"));

    QCOMPARE(manager->theme(), QStringLiteral("never-registered"));
    QCOMPARE(manager->isDark(), false);
    QCOMPARE(qApp->styleSheet(), QStringLiteral("QWidget { color: magenta; }"));
    QCOMPARE(qApp->palette(), before);
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

    // Brightness alone names the palette.
    QCOMPARE(paletteFor(true).color(QPalette::Window), dark.color(QPalette::Window));
    QCOMPARE(paletteFor(false).color(QPalette::Window), light.color(QPalette::Window));

    // A registration says which of the two it is, and that is what reaches the
    // application - a theme never spells out a palette of its own.
    QxThemeManager *manager = QxThemeManager::instance();
    manager->setTheme(QString::fromLatin1(kDark));
    QCOMPARE(qApp->palette().color(QPalette::Window), dark.color(QPalette::Window));
    manager->setTheme(QString::fromLatin1(kLight));
    QCOMPARE(qApp->palette().color(QPalette::Window), light.color(QPalette::Window));
}

void tst_QxTheme::styleSheetOfARegisteredTheme()
{
    QxThemeManager *manager = QxThemeManager::instance();

    QCOMPARE(manager->styleSheet(QString::fromLatin1(kDark)), QStringLiteral("QWidget { color: #f0f0f0; }"));
    // Nothing to paint is a registration like any other, not a failure to
    // resolve.
    QCOMPARE(manager->styleSheet(QString::fromLatin1(kBare)), QString());
    QCOMPARE(manager->styleSheet(QStringLiteral("never-registered")), QString());

    // Replacing the sheet of a theme is how it is given its look after the
    // fact, and the theme on screen is repainted by it.
    manager->setTheme(QString::fromLatin1(kDark));
    manager->setStyleSheet(QString::fromLatin1(kDark), QStringLiteral("QWidget { color: red; }"));
    QCOMPARE(manager->styleSheet(QString::fromLatin1(kDark)), QStringLiteral("QWidget { color: red; }"));
    QCOMPARE(qApp->styleSheet(), QStringLiteral("QWidget { color: red; }"));

    // An id that was never registered is not something to override: there is
    // no built-in sheet behind it any more, so it would only be invented.
    manager->setStyleSheet(QStringLiteral("never-registered"), QStringLiteral("junk"));
    QVERIFY(!manager->hasTheme(QStringLiteral("never-registered")));
}

void tst_QxTheme::applyTheme()
{
    QxThemeManager *manager = QxThemeManager::instance();

    QSignalSpy spy(manager, &QxThemeManager::themeChanged);
    manager->setTheme(QString::fromLatin1(kDark));

    QCOMPARE(spy.count(), 1);
    QCOMPARE(spy.first().at(0).toString(), QString::fromLatin1(kDark));
    QCOMPARE(manager->theme(), QString::fromLatin1(kDark));
    QCOMPARE(manager->themeName(), QStringLiteral("Test dark"));
    QCOMPARE(manager->isDark(), true);

    // Both the palette and the style sheet have to reach the application.
    QCOMPARE(qApp->palette().color(QPalette::Window), darkPalette().color(QPalette::Window));
    QCOMPARE(qApp->styleSheet(), QStringLiteral("QWidget { color: #f0f0f0; }"));

    manager->setTheme(QString::fromLatin1(kLight));

    QCOMPARE(manager->theme(), QString::fromLatin1(kLight));
    QCOMPARE(manager->isDark(), false);
    QCOMPARE(qApp->palette().color(QPalette::Window), lightPalette().color(QPalette::Window));
    QCOMPARE(qApp->styleSheet(), QStringLiteral("QWidget { color: #0f0f0f; }"));
    QCOMPARE(spy.count(), 2);

    // A theme that paints nothing clears what the previous one installed,
    // which is the only way "no skin" can be a choice.
    manager->setTheme(QString::fromLatin1(kBare));
    QCOMPARE(qApp->styleSheet(), QString());
}

void tst_QxTheme::persistence()
{
    QxThemeManager *manager = QxThemeManager::instance();
    manager->setTheme(QString::fromLatin1(kDark));

    // The choice is written as the id, so a theme the application has not
    // registered yet still comes back on the next launch.
    QxSettings reader(m_settingsFile, QSettings::IniFormat);
    reader.beginGroup(QStringLiteral("ui"));
    QCOMPARE(reader.stringValue(QStringLiteral("theme")), QString::fromLatin1(kDark));
    reader.endGroup();

    // Handing the same file back restores the stored theme.
    manager->setSettings(new QxSettings(m_settingsFile, QSettings::IniFormat));
    QCOMPARE(manager->theme(), QString::fromLatin1(kDark));
}

/*!
 * E1: the accent colour the platform uses, and the override that replaces it.
 *
 * The platform half cannot be pinned - it depends on what the machine reports -
 * so what is held here is the contract around it: a usable colour with no
 * override, an override that reaches the application palette, and a key that is
 * removed rather than stored invalid when the override goes away.
 */
void tst_QxTheme::accentColor()
{
    QxThemeManager *manager = QxThemeManager::instance();
    manager->setTheme(QString::fromLatin1(kDark));

    // No override: the answer is the platform's colour, or the theme's own
    // highlight when the platform named none. Either way it is a colour.
    manager->setAccentColor(QColor());
    QVERIFY(!manager->hasAccentOverride());
    QVERIFY(manager->accentColor().isValid());

    QSignalSpy spy(manager, &QxThemeManager::accentColorChanged);

    // The override is what the engine reports and what the widgets paint with:
    // it reaches the application palette, text included.
    const QColor crimson(0xd1, 0x2b, 0x2b);
    manager->setAccentColor(crimson);
    QCOMPARE(spy.count(), 1);
    QCOMPARE(spy.first().at(0).value<QColor>(), crimson);
    QVERIFY(manager->hasAccentOverride());
    QCOMPARE(manager->accentColor(), crimson);
    QCOMPARE(QApplication::palette().color(QPalette::Highlight), crimson);
    QCOMPARE(QApplication::palette().color(QPalette::HighlightedText), QColor(Qt::white));

    // Saying the same thing again is not a change.
    manager->setAccentColor(crimson);
    QCOMPARE(spy.count(), 1);

    // A light accent takes dark text, which is the other half of the rule and
    // the one a single saturated colour cannot show.
    manager->setAccentColor(QColor(0xff, 0xf0, 0x9a));
    QCOMPARE(spy.count(), 2);
    QCOMPARE(QApplication::palette().color(QPalette::HighlightedText), QColor(Qt::black));

    // The alpha travels with it: name() would have dropped it.
    QxSettings reader(m_settingsFile, QSettings::IniFormat);
    reader.beginGroup(QStringLiteral("ui"));
    QCOMPARE(reader.stringValue(QStringLiteral("accentColor")), QStringLiteral("#fffff09a"));
    reader.endGroup();

    manager->setAccentColor(QColor());
    QCOMPARE(spy.count(), 3);
    QVERIFY(!manager->hasAccentOverride());

    // Clearing removes the key: an absent override and a stored invalid colour
    // would be two spellings of the same thing.
    QxSettings cleared(m_settingsFile, QSettings::IniFormat);
    cleared.beginGroup(QStringLiteral("ui"));
    QVERIFY(!cleared.contains(QStringLiteral("accentColor")));
    cleared.endGroup();
}

void tst_QxTheme::followSystem()
{
    QxThemeManager *manager = QxThemeManager::instance();

    // Nothing is preset: which two themes answer the system is the
    // application's to say, not the engine's to guess.
    QCOMPARE(manager->followSystemTheme(), false);
    QCOMPARE(manager->systemLightTheme(), QString());
    QCOMPARE(manager->systemDarkTheme(), QString());

    manager->setSystemThemePair(QString::fromLatin1(kLight), QString::fromLatin1(kDark));
    QSignalSpy spy(manager, &QxThemeManager::followSystemThemeChanged);
    manager->setFollowSystemTheme(true);

    QCOMPARE(spy.count(), 1);
    QCOMPARE(manager->followSystemTheme(), true);

    // Following selects one of the configured pair, never an unrelated theme.
    QVERIFY(manager->theme() == QString::fromLatin1(kLight) || manager->theme() == QString::fromLatin1(kDark));
    QCOMPARE(manager->isDark(), manager->theme() == QString::fromLatin1(kDark));

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
    QCOMPARE(after.stringValue(QStringLiteral("theme")), manager->theme());
    after.endGroup();
}

void tst_QxTheme::systemThemePair()
{
    QxThemeManager *manager = QxThemeManager::instance();

    manager->setFollowSystemTheme(true);
    // Start from a theme outside the pair, so that naming the pair is a change
    // rather than a confirmation of what is already on screen.
    manager->setTheme(QString::fromLatin1(kBare));
    QSignalSpy spy(manager, &QxThemeManager::themeChanged);

    manager->setSystemThemePair(QString::fromLatin1(kLight), QString::fromLatin1(kDark));

    QCOMPARE(manager->systemLightTheme(), QString::fromLatin1(kLight));
    QCOMPARE(manager->systemDarkTheme(), QString::fromLatin1(kDark));
    QVERIFY(manager->theme() == QString::fromLatin1(kLight) || manager->theme() == QString::fromLatin1(kDark));

    // The pair moved from nothing to a real pair, so the change is applied and
    // announced exactly once.
    QCOMPARE(spy.count(), 1);

    QxSettings reader(m_settingsFile, QSettings::IniFormat);
    reader.beginGroup(QStringLiteral("ui"));
    QCOMPARE(reader.stringValue(QStringLiteral("systemLightTheme")), QString::fromLatin1(kLight));
    QCOMPARE(reader.stringValue(QStringLiteral("systemDarkTheme")), QString::fromLatin1(kDark));
    reader.endGroup();

    manager->setFollowSystemTheme(false);
}

/*!
 * apply() is the public way to re-install what the current theme says, without
 * selecting anything: setSettings() calls it, and so may a caller that changed
 * the application state behind the manager's back. Unlike setTheme() it must not
 * announce a change - the theme did not change, it was re-applied.
 */
void tst_QxTheme::applyReinstallsWithoutAnnouncing()
{
    QxThemeManager *manager = QxThemeManager::instance();

    manager->setTheme(QString::fromLatin1(kDark));

    // Something else overwrites what the manager installed.
    qApp->setStyleSheet(QStringLiteral("QWidget { color: magenta; }"));
    qApp->setPalette(lightPalette());
    QVERIFY(qApp->styleSheet() != QStringLiteral("QWidget { color: #f0f0f0; }"));

    QSignalSpy spy(manager, &QxThemeManager::themeChanged);
    manager->apply();

    QCOMPARE(qApp->styleSheet(), QStringLiteral("QWidget { color: #f0f0f0; }"));
    QCOMPARE(qApp->palette().color(QPalette::Window), darkPalette().color(QPalette::Window));
    QCOMPARE(manager->theme(), QString::fromLatin1(kDark));
    QCOMPARE(spy.count(), 0);
}

/*!
 * refreshSystemTheme() is the manual hook for the platforms that do not report
 * a colour scheme change. While following is off it has to be a no-op: an
 * application that is not following the system must not be moved by it.
 */
void tst_QxTheme::refreshSystemThemeIsANoOpWhileNotFollowing()
{
    QxThemeManager *manager = QxThemeManager::instance();

    manager->setFollowSystemTheme(false);
    manager->setTheme(QString::fromLatin1(kLight));
    const QString before = manager->theme();

    QSignalSpy spy(manager, &QxThemeManager::themeChanged);
    manager->refreshSystemTheme();

    QCOMPARE(manager->theme(), before);
    QCOMPARE(spy.count(), 0);

    // With following on it is safe as well, and whatever it decides has to be
    // one of the configured pair - never a theme from outside it.
    manager->setSystemThemePair(QString::fromLatin1(kLight), QString::fromLatin1(kDark));
    manager->setFollowSystemTheme(true);
    manager->refreshSystemTheme();
    QVERIFY(manager->theme() == manager->systemLightTheme() || manager->theme() == manager->systemDarkTheme());
}

/*!
 * settings() is how a caller reaches the persisted state. It has to hand back
 * the very object setSettings() took rather than a fresh one, or a caller
 * reading through it would see none of what the manager wrote.
 */
void tst_QxTheme::settingsIsTheObjectSetSettingsTook()
{
    QxThemeManager *manager = QxThemeManager::instance();

    QxSettings *expected = manager->settings();
    QVERIFY(expected != Q_NULLPTR);
    QCOMPARE(manager->settings(), expected);

    // What the manager persists is visible through that object.
    manager->setTheme(QString::fromLatin1(kDark));
    QxSettings *reader = manager->settings();
    reader->beginGroup(QStringLiteral("ui"));
    QCOMPARE(reader->stringValue(QStringLiteral("theme")), QString::fromLatin1(kDark));
    reader->endGroup();
}

TEST_ADD(tst_QxTheme)

#include "tst_qxtheme.moc"
