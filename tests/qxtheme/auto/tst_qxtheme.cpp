/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#include "tst_global.h"

#include "qxtheme/qxtheme.h"
#include "qxtheme/qxthemeelevation.h"
#include "qxtheme/qxthemeicon.h"
#include "qxtheme/qxthememotion.h"
#include "qxtheme/qxthememanager.h"
#include "qxtheme/qxthemepalette.h"

#include <QAction>
#include <QApplication>
#include <QColor>
#include <QCoreApplication>
#include <QEvent>
#include <QIcon>
#include <QImage>
#include <QPalette>
#include <QPixmap>
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

/*!
 * A glyph to repaint: one solid square, which is all a colour assertion needs -
 * what is being tested is the colour that comes out, not the drawing. PNG
 * rather than SVG on purpose, so the case says something about the tinting and
 * not about whether the SVG plugin happens to be installed.
 */
QString writeGlyph(const QTemporaryDir &dir, const QString &name)
{
    const QString path = dir.filePath(name);
    QImage image(24, 24, QImage::Format_ARGB32);
    image.fill(Qt::black);
    if (!image.save(path)) {
        return QString();
    }
    return path;
}

/*! The colour an action's icon is actually painted in, taken from its middle. */
QColor iconColor(const QAction *action, const QSize &size = QSize(24, 24))
{
    const QPixmap pixmap = action->icon().pixmap(size);
    if (pixmap.isNull()) {
        return QColor();
    }
    return pixmap.toImage().pixelColor(size.width() / 2, size.height() / 2);
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
    void colorSchemeHintLeavesThePaletteAlone();
    void styleSheetOfARegisteredTheme();
    void applyTheme();
    void persistence();
    void restoredThemeWaitsForItsRegistration();
    void accentColor();
    void followSystem();
    void systemThemePair();
    void applyReinstallsWithoutAnnouncing();
    void refreshSystemThemeIsANoOpWhileNotFollowing();
    void settingsIsTheObjectSetSettingsTook();
    void shadowTiers();
    void motionTiersAndTheReduceSwitch();
    void reduceMotionIsRemembered();

    void tintedPixmapRepaintsTheGlyph();
    void tintedIconIsEmptyForAFileThatIsNotThere();
    void themedPixmapTakesTheColourFromThePalette();
    void themedIconFollowsThePalette();
    void iconsFollowThePalette();
    void iconsCanBeRefreshedOnRequest();
    void iconsForgetAnActionThatIsGone();
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

/*!
 * The color scheme is a different question from the palette: it tells the platform
 * which way the application paints so the parts the platform draws itself - the
 * window frame, the windows11 style's own table - can follow. Asking for one must
 * never turn into a second opinion about the application's colours.
 *
 * The platform behind these cases ignores the request, which is why the call has to
 * be harmless rather than load-bearing: a theme's palette has to survive it either
 * way, and on the platforms that do honour it, the entries a theme set explicitly are
 * the ones Qt is documented not to overwrite.
 */
void tst_QxTheme::colorSchemeHintLeavesThePaletteAlone()
{
    QxThemeManager *manager = QxThemeManager::instance();
    manager->setTheme(QString::fromLatin1(kDark));
    const QPalette installed = qApp->palette();

    const QPalette::ColorRole roles[] = {QPalette::Window, QPalette::Base, QPalette::Text, QPalette::ButtonText};
    const auto paletteIsStillTheThemes = [&installed, &roles]() {
        for (QPalette::ColorRole role : roles) {
            if (qApp->palette().color(role) != installed.color(role)) {
                return false;
            }
        }
        return true;
    };

    applyColorScheme(false);
    QVERIFY(paletteIsStillTheThemes());

    applyColorScheme(true);
    QVERIFY(paletteIsStillTheThemes());

    followSystemColorScheme();
    QVERIFY(paletteIsStillTheThemes());
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
 * A selection restored from the configuration always arrives before the
 * registration that supplies it: instance() is what reads the stored id, and
 * registering can only happen once instance() has returned. So the id is
 * remembered rather than applied - looking it up in an empty table would be a
 * warning about a theme that is on its way - and the registration is what
 * installs it.
 */
void tst_QxTheme::restoredThemeWaitsForItsRegistration()
{
    QxThemeManager *manager = QxThemeManager::instance();
    // Nothing painted, so anything that lands afterwards can be seen.
    manager->setTheme(QString::fromLatin1(kBare));
    QCOMPARE(qApp->styleSheet(), QString());

    // What a second launch looks like: the file names a theme this build has
    // not registered yet.
    const QString id = QStringLiteral("late-light");
    QxSettings writer(m_settingsFile, QSettings::IniFormat);
    writer.beginGroup(QStringLiteral("ui"));
    writer.setValue(QStringLiteral("theme"), id);
    writer.endGroup();

    manager->setSettings(new QxSettings(m_settingsFile, QSettings::IniFormat));
    QCOMPARE(manager->theme(), id);
    QVERIFY(!manager->hasTheme(id));
    QCOMPARE(qApp->styleSheet(), QString());

    // The registration that supplies it is what installs it - and without
    // announcing, because the selection itself did not change.
    QSignalSpy spy(manager, &QxThemeManager::themeChanged);
    manager->registerTheme(
        makeTheme(id, QStringLiteral("Late light"), false, QStringLiteral("QWidget { color: #123456; }")));
    QCOMPARE(qApp->styleSheet(), QStringLiteral("QWidget { color: #123456; }"));
    QCOMPARE(spy.count(), 0);
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

/*!
 * Repainting works on the glyph's alpha, so the colour the file was drawn in is
 * replaced outright and the coverage - the antialiased edges included - is kept.
 * Asking for an invalid colour has to hand the raster back rather than nothing:
 * without an application there is no palette to read, and an unthemed glyph is
 * still a glyph.
 */
void tst_QxTheme::tintedPixmapRepaintsTheGlyph()
{
    const QString path = writeGlyph(m_dir, QStringLiteral("glyph-tinted.png"));
    QVERIFY(!path.isEmpty());

    const QPixmap glyph = tintedPixmap(path, Qt::red, QSize(24, 24));
    const QPixmap untouched = tintedPixmap(path, QColor(), QSize(24, 24));

    QVERIFY(!glyph.isNull());
    QCOMPARE(glyph.size(), QSize(24, 24));
    QCOMPARE(glyph.toImage().pixelColor(12, 12), QColor(Qt::red));
    QCOMPARE(untouched.toImage().pixelColor(12, 12), QColor(Qt::black));
}

/*! A file that cannot be loaded is reported as such instead of an empty box. */
void tst_QxTheme::tintedIconIsEmptyForAFileThatIsNotThere()
{
    const QIcon icon = tintedIcon(m_dir.filePath(QStringLiteral("no-such-glyph.png")), Qt::red, QSize(16, 16));
    QVERIFY(icon.isNull());
}

/*!
 * The colour comes from the palette in force, by role: the same call has to
 * answer differently on a light theme and a dark one, which is the whole reason
 * an application hands its icons through here instead of naming a colour.
 */
void tst_QxTheme::themedPixmapTakesTheColourFromThePalette()
{
    const QString path = writeGlyph(m_dir, QStringLiteral("glyph-themed.png"));
    QVERIFY(!path.isEmpty());

    const QColor role(0x11, 0x22, 0x33);
    const QPalette saved = QApplication::palette();
    QPalette palette = saved;
    palette.setColor(QPalette::WindowText, role);
    QApplication::setPalette(palette);
    const QPixmap glyph = themedPixmap(path, QPalette::WindowText, QSize(24, 24));
    const QPixmap other = themedPixmap(path, QPalette::Highlight, QSize(24, 24));
    QApplication::setPalette(saved);

    QCOMPARE(glyph.toImage().pixelColor(12, 12), role);
    QCOMPARE(other.toImage().pixelColor(12, 12), QApplication::palette().color(QPalette::Highlight));
}

/*!
 * Painting an icon once fixes its colour, and a fixed colour is exactly what a
 * theme switch breaks. QxThemeIcons is the piece that answers again: it has to
 * repaint on the palette change an application announces by replacing its
 * palette, without the application naming the icons a second time.
 */
void tst_QxTheme::iconsFollowThePalette()
{
    const QString path = writeGlyph(m_dir, QStringLiteral("glyph-bound.png"));
    QVERIFY(!path.isEmpty());

    const QColor before(0x11, 0x22, 0x33);
    const QColor after(0xaa, 0xbb, 0xcc);
    const QPalette saved = QApplication::palette();

    QPalette palette = saved;
    palette.setColor(QPalette::ButtonText, before);
    QApplication::setPalette(palette);

    QxThemeIcons icons;
    QAction action(nullptr);
    icons.add(&action, path, QPalette::ButtonText, QSize(24, 24));
    const QColor painted = iconColor(&action);

    palette.setColor(QPalette::ButtonText, after);
    QApplication::setPalette(palette);
    // What QApplication::setPalette() announces. Sent here explicitly so the
    // case says something about the filter and not about which widget tree
    // happens to be on screen.
    QEvent change(QEvent::ApplicationPaletteChange);
    QCoreApplication::sendEvent(QCoreApplication::instance(), &change);
    const QColor repainted = iconColor(&action);
    QApplication::setPalette(saved);

    QCOMPARE(painted, before);
    QCOMPARE(repainted, after);
}

/*! refresh() is the way out when the palette was replaced without an announcement. */
void tst_QxTheme::iconsCanBeRefreshedOnRequest()
{
    const QString path = writeGlyph(m_dir, QStringLiteral("glyph-refresh.png"));
    QVERIFY(!path.isEmpty());

    const QColor after(0x0a, 0x0b, 0x0c);
    const QPalette saved = QApplication::palette();
    QPalette palette = saved;
    palette.setColor(QPalette::ButtonText, after);

    QxThemeIcons icons;
    QAction first(nullptr);
    QAction second(nullptr);
    icons.add(&first, path, QPalette::ButtonText, QSize(24, 24));
    icons.add(&second, path, QPalette::ButtonText, QSize(24, 24));
    QCOMPARE(icons.count(), 2);

    QApplication::setPalette(palette);
    icons.refresh();
    const QColor firstColor = iconColor(&first);
    const QColor secondColor = iconColor(&second);
    QApplication::setPalette(saved);

    QCOMPARE(firstColor, after);
    QCOMPARE(secondColor, after);
}

/*!
 * A binding is not a reference the application has to manage: an action deleted
 * while its icon is being kept up to date must not be painted again, and must
 * not be counted.
 */
void tst_QxTheme::iconsForgetAnActionThatIsGone()
{
    const QString path = writeGlyph(m_dir, QStringLiteral("glyph-dropped.png"));
    QVERIFY(!path.isEmpty());

    QxThemeIcons icons;
    auto *action = new QAction(nullptr);
    icons.add(action, path, QPalette::WindowText, QSize(24, 24));
    QCOMPARE(icons.count(), 1);

    delete action;
    icons.refresh();
    QCOMPARE(icons.count(), 0);

    // Forgetting on purpose leaves the icon alone but stops keeping it current.
    QAction other(nullptr);
    icons.add(&other, path, QPalette::WindowText, QSize(24, 24));
    QVERIFY(icons.remove(&other));
    QCOMPARE(icons.count(), 0);
    QVERIFY(!icons.remove(&other));
}

/*! The icon is the same answer at several sizes, and follows the role as well. */
void tst_QxTheme::themedIconFollowsThePalette()
{
    const QString path = writeGlyph(m_dir, QStringLiteral("glyph-themed-icon.png"));
    QVERIFY(!path.isEmpty());

    const QColor role(0x40, 0x80, 0xc0);
    const QPalette saved = QApplication::palette();
    QPalette palette = saved;
    palette.setColor(QPalette::WindowText, role);
    QApplication::setPalette(palette);
    const QIcon icon = themedIcon(path, QPalette::WindowText, QSize(16, 16));
    QApplication::setPalette(saved);

    QVERIFY(!icon.isNull());
    const QPixmap pixmap = icon.pixmap(QSize(16, 16));
    QCOMPARE(pixmap.toImage().pixelColor(pixmap.width() / 2, pixmap.height() / 2), role);
}

/*! The tiers read as a table: one call per tier, saturating instead of
 *  asserting, and a shadow colour that survives a theme that names none. */
void tst_QxTheme::shadowTiers()
{
    QCOMPARE(shadowFor(-1).blurRadius, shadowNone.blurRadius);
    QCOMPARE(shadowFor(0).blurRadius, shadowNone.blurRadius);
    QCOMPARE(shadowFor(1).blurRadius, shadowSm.blurRadius);
    QCOMPARE(shadowFor(2).blurRadius, shadowMd.blurRadius);
    QCOMPARE(shadowFor(3).blurRadius, shadowLg.blurRadius);
    QCOMPARE(shadowFor(4).blurRadius, shadowXl.blurRadius);
    // A tier past the last one saturates there: a caller that walks a level
    // scale does not have to clamp it first.
    QCOMPARE(shadowFor(9).blurRadius, shadowXl.blurRadius);
    QCOMPARE(shadowFor(9).alpha, shadowXl.alpha);

    // A shadow asks for black unless a theme hands a colour over, and the
    // alpha it does hand over lands on the colour it handed.
    QCOMPARE(shadowColor(QColor(), shadowSm.alpha), QColor(0, 0, 0, shadowSm.alpha));
    const QColor base(0x20, 0x40, 0x80);
    QCOMPARE(shadowColor(base, shadowMd.alpha), QColor(0x20, 0x40, 0x80, shadowMd.alpha));
}

/*! The tiers read as a table, and the reduce switch is the answer every tier
 *  gives - an animator that reads its motion here needs no check of its own. */
void tst_QxTheme::motionTiersAndTheReduceSwitch()
{
    QCOMPARE(motionFor(-1).duration, motionFast.duration);
    QCOMPARE(motionFor(0).duration, motionFast.duration);
    QCOMPARE(motionFor(1).duration, motionNormal.duration);
    QCOMPARE(motionFor(2).duration, motionEmphasized.duration);
    // Past the last tier saturates there, like the shadows do.
    QCOMPARE(motionFor(9).duration, motionEmphasized.duration);

    // Reduced, every tier is a jump to the end state: zero time, no shape.
    setMotionReduced(true);
    const QxMotion reduced = motionFor(1);
    QCOMPARE(reduced.duration, 0);
    QCOMPARE(reduced.curve, QEasingCurve::Linear);
    setMotionReduced(false);
    QCOMPARE(motionFor(1).duration, motionNormal.duration);
}

/*! The ask for less motion is the application's, and it outlives the run:
 *  written as a value a fresh settings object reads back. */
void tst_QxTheme::reduceMotionIsRemembered()
{
    QxThemeManager *manager = QxThemeManager::instance();
    setMotionReduced(false);

    QSignalSpy spy(manager, &QxThemeManager::reduceMotionChanged);
    manager->setReduceMotion(true);
    QCOMPARE(manager->reduceMotion(), true);
    QCOMPARE(spy.count(), 1);
    // The switch is process-wide: the free function is the same answer.
    QCOMPARE(isMotionReduced(), true);

    QxSettings reader(m_settingsFile, QSettings::IniFormat);
    reader.beginGroup(QStringLiteral("ui"));
    QCOMPARE(reader.boolValue(QStringLiteral("reduceMotion")), true);
    reader.endGroup();

    // Handing the same file back starts reduced again.
    manager->setSettings(new QxSettings(m_settingsFile, QSettings::IniFormat));
    QCOMPARE(manager->reduceMotion(), true);

    // Leave the process as it was found: motion is global.
    manager->setReduceMotion(false);
    QCOMPARE(isMotionReduced(), false);
}

TEST_ADD(tst_QxTheme)

#include "tst_qxtheme.moc"
