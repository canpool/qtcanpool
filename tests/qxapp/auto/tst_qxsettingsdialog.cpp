#include "tst_global.h"

#include "qxapp/qxproperty.h"
#include "qxapp/qxpropertyeditor.h"
#include "qxapp/qxsettingsdialog.h"

#include "qxcore/qxsettings.h"

#include <QAbstractButton>
#include <QtCore/QTemporaryDir>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QPushButton>

QX_APP_USE_NAMESPACE
QX_CORE_USE_NAMESPACE

/*! A descriptor the way an application writes one: a key, a type and a default. */
static QxProperty descriptor(const QString &key, QxProperty::Type type, const QVariant &defaultValue = QVariant())
{
    QxProperty property;
    property.key = key;
    property.label = key;
    property.type = type;
    property.defaultValue = defaultValue;
    return property;
}

/*!
 * The half of a settings dialog that knows about storage.
 *
 * The editor keeps the values; this class is what reads them out of a
 * QxSettings object and writes them back. Every case therefore works against a
 * real file in a temporary directory - the one place where a stand-in would
 * hide the whole point, since "did it land on disk" is the question being
 * asked. Nothing here opens the dialog: exec() is modal and would block the
 * suite, and apply() / accept() / reject() are plain calls.
 */
class tst_QxSettingsDialog : public QObject
{
    Q_OBJECT
private slots:
    void initTestCase();

    void emptyDialog();
    void pages();
    void groups();
    void readsStoredValues();
    void applyWritesThrough();
    void rejectDiscards();
    void resetRestoresDefaults();
    void applyResetsTheBaseline();
    void acceptApplies();
    void reloadRereads();
    void showingRereads();
    void withoutSettings();
private:
    /*! A settings file of its own per case, so no case depends on another. */
    QString newSettingsFile();

    QTemporaryDir m_dir;
    int m_sequence = 0;
};

void tst_QxSettingsDialog::initTestCase()
{
    QVERIFY(m_dir.isValid());
}

QString tst_QxSettingsDialog::newSettingsFile()
{
    return m_dir.filePath(QStringLiteral("dialog-%1.ini").arg(++m_sequence));
}

void tst_QxSettingsDialog::emptyDialog()
{
    const QString file = newSettingsFile();
    QxSettings settings(file, QSettings::IniFormat);
    QxSettingsDialog dialog(&settings);

    QCOMPARE(dialog.settings(), &settings);
    QVERIFY(dialog.pageIds().isEmpty());
    QCOMPARE(dialog.currentPageId(), QString());
    QVERIFY(dialog.editor(QStringLiteral("nope")) == Q_NULLPTR);
    QCOMPARE(dialog.isModified(), false);

    // Nothing to apply yet, so the button says so.
    QDialogButtonBox *buttons = dialog.findChild<QDialogButtonBox *>();
    QVERIFY(buttons != Q_NULLPTR);
    QCOMPARE(buttons->button(QDialogButtonBox::Apply)->isEnabled(), false);

    dialog.apply();
    QCOMPARE(dialog.isModified(), false);
}

void tst_QxSettingsDialog::pages()
{
    const QString file = newSettingsFile();
    QxSettings settings(file, QSettings::IniFormat);
    QxSettingsDialog dialog(&settings);

    dialog.addPage(QStringLiteral("general"), QIcon(), QStringLiteral("General"),
                   QList<QxProperty>{descriptor(QStringLiteral("general/name"), QxProperty::String)});
    dialog.addPage(QStringLiteral("editor"), QIcon(), QStringLiteral("Editor"), QList<QxProperty>());

    const QStringList expected{QStringLiteral("general"), QStringLiteral("editor")};
    QCOMPARE(dialog.pageIds(), expected);
    QVERIFY(dialog.editor(QStringLiteral("general")) != Q_NULLPTR);
    QVERIFY(dialog.editor(QStringLiteral("editor")) != Q_NULLPTR);
    QVERIFY(dialog.editor(QStringLiteral("general")) != dialog.editor(QStringLiteral("editor")));

    // The first page of a dialog has nothing to compete with.
    QCOMPARE(dialog.currentPageId(), QStringLiteral("general"));

    QSignalSpy spy(&dialog, &QxSettingsDialog::currentPageChanged);
    dialog.setCurrentPage(QStringLiteral("editor"));
    QCOMPARE(dialog.currentPageId(), QStringLiteral("editor"));
    QCOMPARE(spy.count(), 1);
    QCOMPARE(spy.at(0).at(0).toString(), QStringLiteral("editor"));

    // Unknown ids, duplicate ids and empty ids are refused, so the list on the
    // left and the stack on the right never disagree.
    dialog.setCurrentPage(QStringLiteral("nope"));
    QCOMPARE(spy.count(), 1);
    dialog.addPage(QStringLiteral("general"), QIcon(), QStringLiteral("Again"), QList<QxProperty>());
    dialog.addPage(QString(), QIcon(), QStringLiteral("Nameless"), QList<QxProperty>());
    QCOMPARE(dialog.pageIds(), expected);
}

void tst_QxSettingsDialog::groups()
{
    const QString file = newSettingsFile();
    QxSettings settings(file, QSettings::IniFormat);
    QxSettingsDialog dialog(&settings);
    dialog.addPage(QStringLiteral("general"), QIcon(), QStringLiteral("General"), QList<QxProperty>());

    // A group is a way to structure a page, not an alternative to a page.
    dialog.addGroup(QStringLiteral("general"), QStringLiteral("Indentation"),
                    QList<QxProperty>{descriptor(QStringLiteral("general/tabWidth"), QxProperty::Int, 4)});
    QCOMPARE(dialog.editor(QStringLiteral("general"))->findChildren<QGroupBox *>().count(), 1);
    QCOMPARE(dialog.editor(QStringLiteral("general"))->count(), 1);

    // A key brought in by a group is read like any other.
    QCOMPARE(dialog.editor(QStringLiteral("general"))->value(QStringLiteral("general/tabWidth")).toInt(), 4);

    dialog.addGroup(QStringLiteral("nope"), QStringLiteral("Nowhere"), QList<QxProperty>());
    QCOMPARE(dialog.editor(QStringLiteral("general"))->findChildren<QGroupBox *>().count(), 1);
}

void tst_QxSettingsDialog::readsStoredValues()
{
    const QString file = newSettingsFile();
    {
        QxSettings seed(file, QSettings::IniFormat);
        seed.setValue(QStringLiteral("general/name"), QStringLiteral("acme"));
        seed.sync();
    }

    QxSettings settings(file, QSettings::IniFormat);
    QxSettingsDialog dialog(&settings);
    dialog.addPage(
        QStringLiteral("general"), QIcon(), QStringLiteral("General"),
        QList<QxProperty>{descriptor(QStringLiteral("general/name"), QxProperty::String, QStringLiteral("nobody")),
                          descriptor(QStringLiteral("general/port"), QxProperty::Int, 80)});

    QxPropertyEditor *editor = dialog.editor(QStringLiteral("general"));
    QCOMPARE(editor->value(QStringLiteral("general/name")).toString(), QStringLiteral("acme"));
    // A key that was never written reads as its default, which is what makes a
    // fresh installation look like a configured one.
    QCOMPARE(editor->value(QStringLiteral("general/port")).toInt(), 80);
    QCOMPARE(dialog.isModified(), false);
}

void tst_QxSettingsDialog::applyWritesThrough()
{
    const QString file = newSettingsFile();
    QxSettings settings(file, QSettings::IniFormat);
    QxSettingsDialog dialog(&settings);
    dialog.addPage(
        QStringLiteral("general"), QIcon(), QStringLiteral("General"),
        QList<QxProperty>{descriptor(QStringLiteral("general/name"), QxProperty::String, QStringLiteral("nobody")),
                          descriptor(QStringLiteral("general/port"), QxProperty::Int, 80)});

    QSignalSpy spy(&dialog, &QxSettingsDialog::applied);
    dialog.editor(QStringLiteral("general"))->setValue(QStringLiteral("general/name"), QStringLiteral("changed"));
    QCOMPARE(dialog.isModified(), true);

    dialog.apply();
    QCOMPARE(spy.count(), 1);

    // A settings object of its own, so this really reads the file and not the
    // copy the dialog is holding.
    QxSettings reader(file, QSettings::IniFormat);
    QCOMPARE(reader.stringValue(QStringLiteral("general/name")), QStringLiteral("changed"));
    QCOMPARE(reader.intValue(QStringLiteral("general/port"), -1), 80);
}

void tst_QxSettingsDialog::rejectDiscards()
{
    const QString file = newSettingsFile();
    {
        QxSettings seed(file, QSettings::IniFormat);
        seed.setValue(QStringLiteral("general/name"), QStringLiteral("acme"));
        seed.sync();
    }

    QxSettings settings(file, QSettings::IniFormat);
    QxSettingsDialog dialog(&settings);
    dialog.addPage(
        QStringLiteral("general"), QIcon(), QStringLiteral("General"),
        QList<QxProperty>{descriptor(QStringLiteral("general/name"), QxProperty::String, QStringLiteral("nobody"))});

    dialog.editor(QStringLiteral("general"))->setValue(QStringLiteral("general/name"), QStringLiteral("changed"));
    dialog.reject();

    // What the file had is what it still has: nothing is written unless it is
    // applied, and the edit never left the form.
    QxSettings reader(file, QSettings::IniFormat);
    QCOMPARE(reader.stringValue(QStringLiteral("general/name"), QStringLiteral("(gone)")), QStringLiteral("acme"));
    QCOMPARE(dialog.editor(QStringLiteral("general"))->value(QStringLiteral("general/name")).toString(),
             QStringLiteral("acme"));
}

void tst_QxSettingsDialog::resetRestoresDefaults()
{
    const QString file = newSettingsFile();
    {
        QxSettings seed(file, QSettings::IniFormat);
        seed.setValue(QStringLiteral("general/name"), QStringLiteral("acme"));
        seed.sync();
    }

    QxSettings settings(file, QSettings::IniFormat);
    QxSettingsDialog dialog(&settings);
    dialog.addPage(
        QStringLiteral("general"), QIcon(), QStringLiteral("General"),
        QList<QxProperty>{descriptor(QStringLiteral("general/name"), QxProperty::String, QStringLiteral("nobody"))});

    QxPropertyEditor *editor = dialog.editor(QStringLiteral("general"));
    QCOMPARE(editor->value(QStringLiteral("general/name")).toString(), QStringLiteral("acme"));

    dialog.reset();
    QCOMPARE(editor->value(QStringLiteral("general/name")).toString(), QStringLiteral("nobody"));
    // Restoring the defaults is a change of its own, and one worth applying.
    QCOMPARE(dialog.isModified(), true);
}

void tst_QxSettingsDialog::applyResetsTheBaseline()
{
    const QString file = newSettingsFile();
    QxSettings settings(file, QSettings::IniFormat);
    QxSettingsDialog dialog(&settings);
    dialog.addPage(
        QStringLiteral("general"), QIcon(), QStringLiteral("General"),
        QList<QxProperty>{descriptor(QStringLiteral("general/name"), QxProperty::String, QStringLiteral("nobody"))});

    QDialogButtonBox *buttons = dialog.findChild<QDialogButtonBox *>();
    dialog.editor(QStringLiteral("general"))->setValue(QStringLiteral("general/name"), QStringLiteral("changed"));
    QCOMPARE(dialog.isModified(), true);
    QCOMPARE(buttons->button(QDialogButtonBox::Apply)->isEnabled(), true);

    dialog.apply();
    // What is on screen is stored now, so there is nothing left to apply.
    QCOMPARE(dialog.isModified(), false);
    QCOMPARE(buttons->button(QDialogButtonBox::Apply)->isEnabled(), false);

    // And a second edit turns it back on.
    dialog.editor(QStringLiteral("general"))->setValue(QStringLiteral("general/name"), QStringLiteral("again"));
    QCOMPARE(buttons->button(QDialogButtonBox::Apply)->isEnabled(), true);
}

void tst_QxSettingsDialog::acceptApplies()
{
    const QString file = newSettingsFile();
    QxSettings settings(file, QSettings::IniFormat);
    QxSettingsDialog dialog(&settings);
    dialog.addPage(
        QStringLiteral("general"), QIcon(), QStringLiteral("General"),
        QList<QxProperty>{descriptor(QStringLiteral("general/name"), QxProperty::String, QStringLiteral("nobody"))});

    QSignalSpy spy(&dialog, &QxSettingsDialog::applied);
    dialog.editor(QStringLiteral("general"))->setValue(QStringLiteral("general/name"), QStringLiteral("accepted"));

    // Calling accept() rather than exec(): a modal loop would block the suite.
    dialog.accept();
    QCOMPARE(spy.count(), 1);

    QxSettings reader(file, QSettings::IniFormat);
    QCOMPARE(reader.stringValue(QStringLiteral("general/name")), QStringLiteral("accepted"));
}

void tst_QxSettingsDialog::reloadRereads()
{
    const QString file = newSettingsFile();
    {
        QxSettings seed(file, QSettings::IniFormat);
        seed.setValue(QStringLiteral("general/name"), QStringLiteral("acme"));
        seed.sync();
    }

    QxSettings settings(file, QSettings::IniFormat);
    QxSettingsDialog dialog(&settings);
    dialog.addPage(
        QStringLiteral("general"), QIcon(), QStringLiteral("General"),
        QList<QxProperty>{descriptor(QStringLiteral("general/name"), QxProperty::String, QStringLiteral("nobody"))});

    QxPropertyEditor *editor = dialog.editor(QStringLiteral("general"));
    editor->setValue(QStringLiteral("general/name"), QStringLiteral("edited"));
    QCOMPARE(dialog.isModified(), true);

    // Reading again throws the edit away and puts what is stored back.
    dialog.reload();
    QCOMPARE(editor->value(QStringLiteral("general/name")).toString(), QStringLiteral("acme"));
    QCOMPARE(dialog.isModified(), false);
}

void tst_QxSettingsDialog::showingRereads()
{
    const QString file = newSettingsFile();
    {
        QxSettings seed(file, QSettings::IniFormat);
        seed.setValue(QStringLiteral("general/name"), QStringLiteral("acme"));
        seed.sync();
    }

    QxSettings settings(file, QSettings::IniFormat);
    QxSettingsDialog dialog(&settings);
    dialog.addPage(
        QStringLiteral("general"), QIcon(), QStringLiteral("General"),
        QList<QxProperty>{descriptor(QStringLiteral("general/name"), QxProperty::String, QStringLiteral("nobody"))});

    QxPropertyEditor *editor = dialog.editor(QStringLiteral("general"));
    editor->setValue(QStringLiteral("general/name"), QStringLiteral("edited"));

    // Shown, not constructed: a dialog that is opened twice has to show what is
    // stored rather than what was edited and discarded last time.
    dialog.show();
    QCOMPARE(editor->value(QStringLiteral("general/name")).toString(), QStringLiteral("acme"));
    QCOMPARE(dialog.isModified(), false);
}

void tst_QxSettingsDialog::withoutSettings()
{
    // A dialog with nothing to write to still works as a form: the descriptors
    // carry the defaults, so there is something on screen either way.
    QxSettingsDialog dialog(Q_NULLPTR);
    dialog.addPage(
        QStringLiteral("general"), QIcon(), QStringLiteral("General"),
        QList<QxProperty>{descriptor(QStringLiteral("general/name"), QxProperty::String, QStringLiteral("nobody"))});

    QCOMPARE(dialog.settings(), static_cast<QxSettings *>(Q_NULLPTR));
    QCOMPARE(dialog.editor(QStringLiteral("general"))->value(QStringLiteral("general/name")).toString(),
             QStringLiteral("nobody"));

    // Applying reports rather than crashing.
    dialog.apply();
    QCOMPARE(dialog.editor(QStringLiteral("general"))->value(QStringLiteral("general/name")).toString(),
             QStringLiteral("nobody"));
}

TEST_ADD(tst_QxSettingsDialog)

#include "tst_qxsettingsdialog.moc"
