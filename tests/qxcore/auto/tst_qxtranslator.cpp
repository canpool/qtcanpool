#include "tst_global.h"

#include "qxcore/qxtranslator.h"

#include <QCoreApplication>
#include <QDir>
#include <QFile>
#include <QTemporaryDir>

QX_CORE_USE_NAMESPACE

/*!
 * A file with a plausible name and unusable content. That is enough for the
 * name-driven enumeration and deliberately not enough for loading - QTranslator
 * parses what it is given - which is what makes the failure path testable.
 */
static bool writeStub(const QDir &dir, const QString &name)
{
    QFile file(dir.absoluteFilePath(name));
    if (!file.open(QIODevice::WriteOnly)) {
        return false;
    }

    file.write("stub, not a translation");
    return true;
}

class tst_QxTranslator : public QObject
{
    Q_OBJECT
private slots:
    void defaults();
    void availableLanguages();
    void malformedNames();
    void unavailableLanguageChangesNothing();
    void switchingAppliesAndReplaces();
    void destructionRemovesTheCatalogue();
};

void tst_QxTranslator::defaults()
{
    QxTranslator translator;
    QCOMPARE(translator.translationPath(), QString());
    QCOMPARE(translator.language(), QString());
    QCOMPARE(translator.availableLanguages(), QStringList());
    QCOMPARE(translator.loadedFiles(), QStringList());

    // An empty language is never a valid request, and it is not a language
    // change either.
    QSignalSpy spy(&translator, &QxTranslator::languageChanged);
    QCOMPARE(translator.setLanguage(QString()), false);
    QCOMPARE(spy.count(), 0);
}

void tst_QxTranslator::availableLanguages()
{
    QTemporaryDir dir;
    QVERIFY(dir.isValid());
    const QDir qdir(dir.path());

    // Two prefixes for the same language: it still counts once.
    QVERIFY(writeStub(qdir, QStringLiteral("app_de.qm")));
    QVERIFY(writeStub(qdir, QStringLiteral("qtbase_de.qm")));
    QVERIFY(writeStub(qdir, QStringLiteral("app_zh_CN.qm")));
    QVERIFY(writeStub(qdir, QStringLiteral("qtbase_pt_BR.qm")));

    QxTranslator translator;
    QCOMPARE(translator.availableLanguages(), QStringList());

    translator.setTranslationPath(dir.path());
    QCOMPARE(translator.translationPath(), dir.path());

    // The territory stays part of the language, and the result is sorted.
    QCOMPARE(translator.availableLanguages(),
             QStringList({QStringLiteral("de"), QStringLiteral("pt_BR"), QStringLiteral("zh_CN")}));

    // A path that holds nothing is not an error, it just has no languages.
    translator.setTranslationPath(dir.filePath(QStringLiteral("missing")));
    QCOMPARE(translator.availableLanguages(), QStringList());
}

void tst_QxTranslator::malformedNames()
{
    QTemporaryDir dir;
    QVERIFY(dir.isValid());
    const QDir qdir(dir.path());

    // None of these carries a language, so none of them shows up.
    QVERIFY(writeStub(qdir, QStringLiteral("appqm.qm")));    // no underscore at all
    QVERIFY(writeStub(qdir, QStringLiteral("app_.qm")));     // nothing after the underscore
    QVERIFY(writeStub(qdir, QStringLiteral("_de.qm")));      // nothing before it
    QVERIFY(writeStub(qdir, QStringLiteral("notes.txt")));   // not a catalogue
    QVERIFY(writeStub(qdir, QStringLiteral("app_en.ts")));   // the source, not the build

    // The one real name in the directory is the only language reported.
    QVERIFY(writeStub(qdir, QStringLiteral("app_ja.qm")));

    QxTranslator translator;
    translator.setTranslationPath(dir.path());
    QCOMPARE(translator.availableLanguages(), QStringList{QStringLiteral("ja")});
}

void tst_QxTranslator::unavailableLanguageChangesNothing()
{
    QTemporaryDir dir;
    QVERIFY(dir.isValid());
    QVERIFY(writeStub(QDir(dir.path()), QStringLiteral("app_xx.qm")));

    QxTranslator translator;
    translator.setTranslationPath(dir.path());
    // Enumerated, because the enumeration only reads names.
    QCOMPARE(translator.availableLanguages(), QStringList{QStringLiteral("xx")});

    QSignalSpy spy(&translator, &QxTranslator::languageChanged);

    // Loading it cannot work, and a language that cannot be installed must
    // leave the application in the one it was in.
    QCOMPARE(translator.setLanguage(QStringLiteral("xx")), false);
    QCOMPARE(translator.language(), QString());
    QCOMPARE(translator.loadedFiles(), QStringList());
    QCOMPARE(spy.count(), 0);

    // A language with no catalogue of its own fails even when the Qt
    // installation happens to ship one, because Qt's catalogues are only a
    // supplement: a language the application does not translate is not one it
    // can switch to.
    QCOMPARE(translator.setLanguage(QStringLiteral("de")), false);
    QCOMPARE(translator.language(), QString());
    QCOMPARE(translator.loadedFiles(), QStringList());
    QCOMPARE(spy.count(), 0);
}

void tst_QxTranslator::switchingAppliesAndReplaces()
{
#ifdef QXCORE_TEST_TRANSLATIONS
    // What the catalogues in data/ actually carry. The context is the one the
    // .ts files declare.
    QCOMPARE(QCoreApplication::translate("QxTranslatorTest", "Hello"), QStringLiteral("Hello"));

    QxTranslator translator;
    translator.setTranslationPath(QStringLiteral(QXCORE_TEST_TRANSLATIONS));

    QSignalSpy spy(&translator, &QxTranslator::languageChanged);
    QVERIFY(translator.setLanguage(QStringLiteral("fr")));
    QCOMPARE(translator.language(), QStringLiteral("fr"));
    QCOMPARE(spy.count(), 1);
    QCOMPARE(spy.first().at(0).toString(), QStringLiteral("fr"));

    // The point of the class: the text really changes.
    QCOMPARE(QCoreApplication::translate("QxTranslatorTest", "Hello"), QStringLiteral("Bonjour"));
    QCOMPARE(QCoreApplication::translate("QxTranslatorTest", "Goodbye"), QStringLiteral("Au revoir"));
    // The application's own catalogue is always in there and comes first; Qt's
    // may follow, depending on what the Qt installation carries, so the count is
    // not pinned (one application prefix, at most two Qt ones).
    const QStringList frFiles = translator.loadedFiles();
    QVERIFY(frFiles.count() >= 1);
    QVERIFY(frFiles.count() <= 3);
    QVERIFY(frFiles.first().endsWith(QStringLiteral("qxtranslator_fr.qm")));
    for (const QString &file : frFiles) {
        QVERIFY(file.endsWith(QStringLiteral("_fr.qm")));
    }

    // Switching again replaces the previous catalogue instead of stacking on
    // top of it: the text has to follow the new language, not stay behind.
    QVERIFY(translator.setLanguage(QStringLiteral("zh_CN")));
    QCOMPARE(translator.language(), QStringLiteral("zh_CN"));
    QCOMPARE(spy.count(), 2);
    QCOMPARE(QCoreApplication::translate("QxTranslatorTest", "Hello"), QStringLiteral("你好"));
    QCOMPARE(QCoreApplication::translate("QxTranslatorTest", "Goodbye"), QStringLiteral("再见"));
    QVERIFY(translator.loadedFiles().first().endsWith(QStringLiteral("qxtranslator_zh_CN.qm")));

    // The French catalogue is gone rather than merely outranked: nothing that
    // was loaded for it is still installed.
    for (const QString &file : translator.loadedFiles()) {
        QVERIFY(file.endsWith(QStringLiteral("_zh_CN.qm")));
    }
#else
    QSKIP("no lrelease was available, so there is no .qm file to prove a switch with");
#endif
}

void tst_QxTranslator::destructionRemovesTheCatalogue()
{
#ifdef QXCORE_TEST_TRANSLATIONS
    {
        QxTranslator translator;
        translator.setTranslationPath(QStringLiteral(QXCORE_TEST_TRANSLATIONS));
        QVERIFY(translator.setLanguage(QStringLiteral("fr")));
        QCOMPARE(QCoreApplication::translate("QxTranslatorTest", "Hello"), QStringLiteral("Bonjour"));
    }

    // The translator is not only switched, it is uninstalled, so an application
    // that goes out of scope does not leave its strings behind.
    QCOMPARE(QCoreApplication::translate("QxTranslatorTest", "Hello"), QStringLiteral("Hello"));
#else
    QSKIP("no lrelease was available, so there is no .qm file to install");
#endif
}

TEST_ADD(tst_QxTranslator)

#include "tst_qxtranslator.moc"
