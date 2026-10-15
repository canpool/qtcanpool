#include "tst_global.h"

#include "qxribbon/ribbontheme.h"

#include <QApplication>
#include <QFile>
#include <QTemporaryFile>
#include <QWidget>

QX_RIBBON_USE_NAMESPACE

/*
 * RibbonTheme is two static functions over a set of stylesheets compiled into the
 * library, and the whole of its behaviour is about *where* the sheet lands: on the widget
 * passed in, or on QApplication when there is none. The application-wide variant is the
 * one that can leak into other tests, so every case that touches qApp saves the sheet and
 * puts it back.
 *
 * setTheme() is also the only place in the library where an out-of-range enum value can
 * be handed in - it takes a plain int, because RibbonMainWindow exposes the theme as a
 * Q_PROPERTY of type int. The value has to fall through without doing anything.
 */
class tst_RibbonTheme : public QObject
{
    Q_OBJECT
private slots:
    void loadThemeAppliesToTheWidget();
    void loadThemeWithoutAWidgetAppliesToTheApplication();
    void loadThemeOfAnUnreadableFileChangesNothing();
    void setThemeAppliesTheBundledSheet();
    void setThemeAppliesTheFluentSheets();
    void setThemeIgnoresAnUnknownStyle();
    void customThemeClearsTheWidgetSheet();
    void customThemeWithoutAWidgetClearsTheApplicationSheet();
private:
    static QString writeCss();
};

static QString readResource(const QString &path)
{
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return QString();
    }
    return QString::fromUtf8(file.readAll());
}

QString tst_RibbonTheme::writeCss()
{
    QTemporaryFile file(QStringLiteral("qxribbon-theme-XXXXXX.css"));
    file.setAutoRemove(false);
    if (!file.open()) {
        return QString();
    }
    file.write("QWidget { background: #010203; }");
    file.close();
    return file.fileName();
}

void tst_RibbonTheme::loadThemeAppliesToTheWidget()
{
    const QString path = writeCss();
    QVERIFY(!path.isEmpty());
    QWidget widget;

    RibbonTheme::loadTheme(path, &widget);

    QCOMPARE(widget.styleSheet(), QStringLiteral("QWidget { background: #010203; }"));
    QFile::remove(path);
}

void tst_RibbonTheme::loadThemeWithoutAWidgetAppliesToTheApplication()
{
    const QString path = writeCss();
    QVERIFY(!path.isEmpty());
    const QString saved = qApp->styleSheet();

    RibbonTheme::loadTheme(path);

    QCOMPARE(qApp->styleSheet(), QStringLiteral("QWidget { background: #010203; }"));
    qApp->setStyleSheet(saved);
    QFile::remove(path);
}

void tst_RibbonTheme::loadThemeOfAnUnreadableFileChangesNothing()
{
    QWidget widget;
    widget.setStyleSheet(QStringLiteral("QWidget { color: red; }"));

    // A file that does not exist: QFile::open fails and loadTheme returns before
    // touching either the widget or the application.
    RibbonTheme::loadTheme(QStringLiteral(":/qxribbon/res/stylesheets/does-not-exist.css"), &widget);

    QCOMPARE(widget.styleSheet(), QStringLiteral("QWidget { color: red; }"));
}

void tst_RibbonTheme::setThemeAppliesTheBundledSheet()
{
    const QString css = readResource(QStringLiteral(":/qxribbon/res/stylesheets/light_office2013.css"));
    QVERIFY(!css.isEmpty());
    QWidget widget;

    RibbonTheme::setTheme(RibbonTheme::LightOffice2013Theme, &widget);

    QCOMPARE(widget.styleSheet(), css);
}

void tst_RibbonTheme::setThemeAppliesTheFluentSheets()
{
    const QString office2013 = readResource(QStringLiteral(":/qxribbon/res/stylesheets/light_office2013.css"));
    const QString fluent2016 = readResource(QStringLiteral(":/qxribbon/res/stylesheets/light_fluent2016.css"));
    const QString fluent2019 = readResource(QStringLiteral(":/qxribbon/res/stylesheets/light_fluent2019.css"));
    QVERIFY(!fluent2016.isEmpty());
    QVERIFY(!fluent2019.isEmpty());
    // Both Fluent sheets share the selectors of the base theme but are genuine
    // recolourings: each differs from it and from the other.
    QVERIFY(fluent2016 != office2013);
    QVERIFY(fluent2019 != office2013);
    QVERIFY(fluent2016 != fluent2019);

    QWidget widget;
    RibbonTheme::setTheme(RibbonTheme::LightFluent2016Theme, &widget);
    QCOMPARE(widget.styleSheet(), fluent2016);
    RibbonTheme::setTheme(RibbonTheme::LightFluent2019Theme, &widget);
    QCOMPARE(widget.styleSheet(), fluent2019);
}

void tst_RibbonTheme::setThemeIgnoresAnUnknownStyle()
{
    QWidget widget;

    // Not a ThemeStyle value at all. The switch has a default branch that does nothing,
    // so the widget keeps whatever it had.
    widget.setStyleSheet(QStringLiteral("QWidget { color: red; }"));
    RibbonTheme::setTheme(999, &widget);
    QCOMPARE(widget.styleSheet(), QStringLiteral("QWidget { color: red; }"));

    QWidget empty;
    RibbonTheme::setTheme(999, &empty);
    QVERIFY(empty.styleSheet().isEmpty());
}

void tst_RibbonTheme::customThemeClearsTheWidgetSheet()
{
    QWidget widget;
    widget.setStyleSheet(QStringLiteral("QWidget { color: red; }"));

    RibbonTheme::setTheme(RibbonTheme::CustomTheme, &widget);

    QVERIFY(widget.styleSheet().isEmpty());
}

void tst_RibbonTheme::customThemeWithoutAWidgetClearsTheApplicationSheet()
{
    const QString saved = qApp->styleSheet();
    qApp->setStyleSheet(QStringLiteral("QWidget { color: red; }"));

    // The widget argument defaults to nullptr, and loadTheme() reads that as "apply it to
    // QApplication". CustomTheme used to dereference the pointer instead.
    RibbonTheme::setTheme(RibbonTheme::CustomTheme);

    QVERIFY(qApp->styleSheet().isEmpty());
    qApp->setStyleSheet(saved);
}

TEST_ADD(tst_RibbonTheme)

#include "tst_ribbontheme.moc"
