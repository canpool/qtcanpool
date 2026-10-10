#include "tst_global.h"

#include "qxapp/qxproperty.h"
#include "qxapp/qxpropertyeditor.h"

#include <QCheckBox>
#include <QComboBox>
#include <QDoubleSpinBox>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QSpinBox>
#include <QTextEdit>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QWidget>

QX_APP_USE_NAMESPACE

/*! Builds a property of one type with the fields that matter to it. */
static QxProperty makeProperty(const QString &key, QxProperty::Type type, const QVariant &value = QVariant())
{
    QxProperty property;
    property.key = key;
    property.label = key + QLatin1String("Label");
    property.type = type;
    property.value = value;
    return property;
}

/*!
 * The key/value kernel behind a settings dialog.
 *
 * The editor is a form, but everything asserted here is the model behind it:
 * what a value is after it went in, which control a type turned into, and when
 * the values count as modified. The controls are driven directly - setting a
 * spin box rather than clicking it - which is both deterministic and the same
 * path a user takes. No dialog is ever opened: the colour and the font buttons
 * only reach theirs from a click, and a modal dialog would block the suite.
 */
class tst_QxPropertyEditor : public QObject
{
    Q_OBJECT
private slots:
    void emptyEditor();
    void addingProperties();
    void typeMapping();
    void values();
    void coercion();
    void unknownKeys();
    void signalsOnChange();
    void modifiedTracking();
    void resetAndInitial();
    void controlsReachTheModel();
    void labelsTooltipsAndReadOnly();
    void groups();
};

void tst_QxPropertyEditor::emptyEditor()
{
    QxPropertyEditor editor;

    QCOMPARE(editor.count(), 0);
    QVERIFY(editor.keys().isEmpty());
    QCOMPARE(editor.contains(QStringLiteral("nope")), false);
    QCOMPARE(editor.value(QStringLiteral("nope")).isValid(), false);
    QVERIFY(editor.allValues().isEmpty());
    QCOMPARE(editor.isModified(), false);

    // Nothing to reset is not an error.
    editor.reset();
    editor.resetToInitial();
    QCOMPARE(editor.count(), 0);
}

void tst_QxPropertyEditor::addingProperties()
{
    QxPropertyEditor editor;
    editor.addProperty(makeProperty(QStringLiteral("a"), QxProperty::String));
    editor.addProperty(makeProperty(QStringLiteral("b"), QxProperty::Bool));

    // In the order they were added: a form reads in the order it was built.
    const QStringList expected{QStringLiteral("a"), QStringLiteral("b")};
    QCOMPARE(editor.count(), 2);
    QCOMPARE(editor.keys(), expected);
    QCOMPARE(editor.contains(QStringLiteral("a")), true);

    // Two controls writing the same key would fight over it, so a duplicate is
    // refused rather than added twice.
    editor.addProperty(makeProperty(QStringLiteral("a"), QxProperty::String));
    QCOMPARE(editor.count(), 2);

    // And so is a property with nothing to store itself under.
    editor.addProperty(makeProperty(QString(), QxProperty::String));
    QCOMPARE(editor.count(), 2);
}

void tst_QxPropertyEditor::typeMapping()
{
    const QFont font(QStringLiteral("Arial"), 11);

    // One type per editor, so "which control came out of it" is unambiguous.
    {
        QxPropertyEditor editor;
        editor.addProperty(makeProperty(QStringLiteral("k"), QxProperty::Bool));
        const QList<QCheckBox *> checks = editor.findChildren<QCheckBox *>();
        QCOMPARE(checks.count(), 1);
        // The check box carries the label itself instead of getting one beside it.
        QCOMPARE(checks.at(0)->text(), QStringLiteral("kLabel"));
        QCOMPARE(editor.findChildren<QLabel *>().count(), 0);
    }
    {
        QxPropertyEditor editor;
        editor.addProperty(makeProperty(QStringLiteral("k"), QxProperty::Int));
        QCOMPARE(editor.findChildren<QSpinBox *>().count(), 1);
    }
    {
        QxPropertyEditor editor;
        editor.addProperty(makeProperty(QStringLiteral("k"), QxProperty::Double));
        QCOMPARE(editor.findChildren<QDoubleSpinBox *>().count(), 1);
    }
    {
        QxPropertyEditor editor;
        editor.addProperty(makeProperty(QStringLiteral("k"), QxProperty::String));
        QCOMPARE(editor.findChildren<QLineEdit *>().count(), 1);
    }
    {
        QxPropertyEditor editor;
        editor.addProperty(makeProperty(QStringLiteral("k"), QxProperty::Text));
        QCOMPARE(editor.findChildren<QTextEdit *>().count(), 1);
    }
    {
        QxPropertyEditor editor;
        QxProperty choices = makeProperty(QStringLiteral("k"), QxProperty::Enum);
        choices.choices = QStringList{QStringLiteral("one"), QStringLiteral("two")};
        editor.addProperty(choices);
        const QList<QComboBox *> combos = editor.findChildren<QComboBox *>();
        QCOMPARE(combos.count(), 1);
        QCOMPARE(combos.at(0)->count(), 2);
        QCOMPARE(combos.at(0)->itemText(1), QStringLiteral("two"));
    }
    {
        QxPropertyEditor editor;
        editor.addProperty(
            makeProperty(QStringLiteral("k"), QxProperty::Color, QVariant::fromValue(QColor(255, 0, 0))));
        // The colour is shown by name and by a swatch; the button is what carries
        // the value back, so there is nothing else to look for.
        const QList<QPushButton *> buttons = editor.findChildren<QPushButton *>();
        QCOMPARE(buttons.count(), 1);
        QCOMPARE(buttons.at(0)->text(), QStringLiteral("#ff0000"));
        QVERIFY(!buttons.at(0)->icon().isNull());
    }
    {
        QxPropertyEditor editor;
        editor.addProperty(makeProperty(QStringLiteral("k"), QxProperty::Font, QVariant::fromValue(font)));
        const QList<QPushButton *> buttons = editor.findChildren<QPushButton *>();
        QCOMPARE(buttons.count(), 1);
        QCOMPARE(buttons.at(0)->text(), QStringLiteral("Arial, 11"));
    }
    {
        QxPropertyEditor editor;
        editor.addProperty(makeProperty(QStringLiteral("k"), QxProperty::Path));
        // A line edit to hold the value and a button to fill it in: the only type
        // that needs a container.
        QCOMPARE(editor.findChildren<QLineEdit *>().count(), 1);
        QCOMPARE(editor.findChildren<QPushButton *>().count(), 1);
    }
}

void tst_QxPropertyEditor::values()
{
    const QFont font(QStringLiteral("Arial"), 11);

    QxPropertyEditor editor;
    editor.addProperty(makeProperty(QStringLiteral("flag"), QxProperty::Bool, true));
    editor.addProperty(makeProperty(QStringLiteral("port"), QxProperty::Int, 8080));
    editor.addProperty(makeProperty(QStringLiteral("ratio"), QxProperty::Double, 1.5));
    editor.addProperty(makeProperty(QStringLiteral("name"), QxProperty::String, QStringLiteral("acme")));
    editor.addProperty(makeProperty(QStringLiteral("notes"), QxProperty::Text, QStringLiteral("two\nlines")));
    editor.addProperty(makeProperty(QStringLiteral("mode"), QxProperty::Enum, QStringLiteral("fast")));
    editor.addProperty(makeProperty(QStringLiteral("ink"), QxProperty::Color, QVariant::fromValue(QColor(0, 0, 255))));
    editor.addProperty(makeProperty(QStringLiteral("face"), QxProperty::Font, QVariant::fromValue(font)));
    editor.addProperty(makeProperty(QStringLiteral("home"), QxProperty::Path, QStringLiteral("/tmp/x")));

    QCOMPARE(editor.value(QStringLiteral("flag")).toBool(), true);
    QCOMPARE(editor.value(QStringLiteral("port")).toInt(), 8080);
    QCOMPARE(editor.value(QStringLiteral("ratio")).toDouble(), 1.5);
    QCOMPARE(editor.value(QStringLiteral("name")).toString(), QStringLiteral("acme"));
    QCOMPARE(editor.value(QStringLiteral("notes")).toString(), QStringLiteral("two\nlines"));
    QCOMPARE(editor.value(QStringLiteral("mode")).toString(), QStringLiteral("fast"));
    QCOMPARE(qvariant_cast<QColor>(editor.value(QStringLiteral("ink"))), QColor(0, 0, 255));
    QCOMPARE(qvariant_cast<QFont>(editor.value(QStringLiteral("face"))), font);
    QCOMPARE(editor.value(QStringLiteral("home")).toString(), QStringLiteral("/tmp/x"));

    // Values are stored in the type the property declares, not in whatever type
    // they arrived as - otherwise a value could never be compared with the one
    // the control holds. Comparing whole variants covers type and value at once.
    QCOMPARE(editor.value(QStringLiteral("flag")), QVariant(true));
    QCOMPARE(editor.value(QStringLiteral("port")), QVariant(8080));
    QCOMPARE(editor.value(QStringLiteral("ratio")), QVariant(1.5));

    QHash<QString, QVariant> all = editor.allValues();
    QCOMPARE(all.count(), 9);
    QCOMPARE(all.value(QStringLiteral("port")).toInt(), 8080);
}

void tst_QxPropertyEditor::coercion()
{
    QxPropertyEditor editor;
    editor.addProperty(makeProperty(QStringLiteral("port"), QxProperty::Int, 80));

    // A number that arrived as text - which is what a configuration file hands
    // over - is still a number afterwards.
    editor.setValue(QStringLiteral("port"), QStringLiteral("8080"));
    QCOMPARE(editor.value(QStringLiteral("port")), QVariant(8080));

    // And something that is not a number at all is refused, so a bad value does
    // not silently turn into a zero.
    editor.setValue(QStringLiteral("port"), QStringLiteral("not a number"));
    QCOMPARE(editor.value(QStringLiteral("port")).toInt(), 8080);

    // A colour may be given the way a configuration file writes one.
    QxPropertyEditor colours;
    colours.addProperty(makeProperty(QStringLiteral("ink"), QxProperty::Color));
    colours.setValue(QStringLiteral("ink"), QStringLiteral("#00ff00"));
    QCOMPARE(qvariant_cast<QColor>(colours.value(QStringLiteral("ink"))), QColor(0, 255, 0));
}

void tst_QxPropertyEditor::unknownKeys()
{
    QxPropertyEditor editor;
    editor.addProperty(makeProperty(QStringLiteral("known"), QxProperty::String, QStringLiteral("x")));

    QCOMPARE(editor.value(QStringLiteral("unknown")).isValid(), false);
    editor.setValue(QStringLiteral("unknown"), QStringLiteral("y"));
    QCOMPARE(editor.count(), 1);

    // A bulk load that names a key this editor does not have is reported and
    // skipped, not stored behind the form's back.
    QHash<QString, QVariant> values;
    values.insert(QStringLiteral("known"), QStringLiteral("changed"));
    values.insert(QStringLiteral("stale"), QStringLiteral("gone"));
    editor.setAllValues(values);
    QCOMPARE(editor.count(), 1);
    QCOMPARE(editor.value(QStringLiteral("known")).toString(), QStringLiteral("changed"));
    QCOMPARE(editor.value(QStringLiteral("stale")).isValid(), false);
}

void tst_QxPropertyEditor::signalsOnChange()
{
    QxPropertyEditor editor;
    editor.addProperty(makeProperty(QStringLiteral("name"), QxProperty::String, QStringLiteral("old")));

    QSignalSpy spy(&editor, &QxPropertyEditor::valueChanged);
    editor.setValue(QStringLiteral("name"), QStringLiteral("new"));
    QCOMPARE(spy.count(), 1);
    QCOMPARE(spy.at(0).at(0).toString(), QStringLiteral("name"));
    QCOMPARE(spy.at(0).at(1).toString(), QStringLiteral("new"));

    // Setting the value it already has is not a change.
    editor.setValue(QStringLiteral("name"), QStringLiteral("new"));
    QCOMPARE(spy.count(), 1);
}

void tst_QxPropertyEditor::modifiedTracking()
{
    QxProperty after = makeProperty(QStringLiteral("name"), QxProperty::String, QStringLiteral("old"));
    after.defaultValue = QStringLiteral("default");

    QxPropertyEditor editor;
    editor.addProperty(after);
    QCOMPARE(editor.isModified(), false);

    QSignalSpy spy(&editor, &QxPropertyEditor::modifiedChanged);

    // Loading is the baseline, not an edit.
    QHash<QString, QVariant> loaded;
    loaded.insert(QStringLiteral("name"), QStringLiteral("stored"));
    editor.setAllValues(loaded);
    QCOMPARE(editor.value(QStringLiteral("name")).toString(), QStringLiteral("stored"));
    QCOMPARE(editor.isModified(), false);
    QCOMPARE(spy.count(), 0);

    // Any departure from it is a modification...
    editor.setValue(QStringLiteral("name"), QStringLiteral("edited"));
    QCOMPARE(editor.isModified(), true);
    QCOMPARE(spy.count(), 1);
    QCOMPARE(spy.at(0).at(0).toBool(), true);

    // ... and so is going back by hand, which is why it is reported once.
    editor.setValue(QStringLiteral("name"), QStringLiteral("stored"));
    QCOMPARE(editor.isModified(), false);
    QCOMPARE(spy.count(), 2);
    QCOMPARE(spy.at(1).at(0).toBool(), false);
}

void tst_QxPropertyEditor::resetAndInitial()
{
    QxProperty one = makeProperty(QStringLiteral("one"), QxProperty::Int, 10);
    one.defaultValue = 1;
    QxProperty two = makeProperty(QStringLiteral("two"), QxProperty::String, QStringLiteral("start"));
    two.defaultValue = QStringLiteral("fresh");

    QxPropertyEditor editor;
    editor.addProperty(one);
    editor.addProperty(two);

    QHash<QString, QVariant> loaded;
    loaded.insert(QStringLiteral("one"), 42);
    loaded.insert(QStringLiteral("two"), QStringLiteral("stored"));
    editor.setAllValues(loaded);
    QCOMPARE(editor.value(QStringLiteral("one")).toInt(), 42);

    // Discarding edits goes back to what was loaded.
    editor.setValue(QStringLiteral("one"), 7);
    QCOMPARE(editor.isModified(), true);
    editor.resetToInitial();
    QCOMPARE(editor.value(QStringLiteral("one")).toInt(), 42);
    QCOMPARE(editor.value(QStringLiteral("two")).toString(), QStringLiteral("stored"));
    QCOMPARE(editor.isModified(), false);

    // Defaults are a different question: what a fresh installation would have.
    editor.reset();
    QCOMPARE(editor.value(QStringLiteral("one")).toInt(), 1);
    QCOMPARE(editor.value(QStringLiteral("two")).toString(), QStringLiteral("fresh"));
    // And restoring the defaults is itself a change worth applying.
    QCOMPARE(editor.isModified(), true);

    editor.resetToInitial();
    QCOMPARE(editor.isModified(), false);
}

void tst_QxPropertyEditor::controlsReachTheModel()
{
    QxProperty choice = makeProperty(QStringLiteral("mode"), QxProperty::Enum, QStringLiteral("slow"));
    choice.choices = QStringList{QStringLiteral("slow"), QStringLiteral("fast")};

    QxPropertyEditor editor;
    editor.addProperty(makeProperty(QStringLiteral("flag"), QxProperty::Bool, false));
    editor.addProperty(makeProperty(QStringLiteral("port"), QxProperty::Int, 80));
    editor.addProperty(makeProperty(QStringLiteral("ratio"), QxProperty::Double, 1.0));
    editor.addProperty(makeProperty(QStringLiteral("name"), QxProperty::String, QStringLiteral("old")));
    editor.addProperty(makeProperty(QStringLiteral("notes"), QxProperty::Text));
    editor.addProperty(choice);

    QSignalSpy spy(&editor, &QxPropertyEditor::valueChanged);

    // Driven the way a user drives them, which is the only way to know the
    // controls and the model are actually wired to each other.
    editor.findChild<QCheckBox *>()->setChecked(true);
    editor.findChild<QSpinBox *>()->setValue(8080);
    editor.findChild<QDoubleSpinBox *>()->setValue(2.5);
    editor.findChild<QLineEdit *>()->setText(QStringLiteral("new"));
    editor.findChild<QTextEdit *>()->setPlainText(QStringLiteral("typed"));
    editor.findChild<QComboBox *>()->setCurrentIndex(1);

    QCOMPARE(editor.value(QStringLiteral("flag")).toBool(), true);
    QCOMPARE(editor.value(QStringLiteral("port")).toInt(), 8080);
    QCOMPARE(editor.value(QStringLiteral("ratio")).toDouble(), 2.5);
    QCOMPARE(editor.value(QStringLiteral("name")).toString(), QStringLiteral("new"));
    QCOMPARE(editor.value(QStringLiteral("notes")).toString(), QStringLiteral("typed"));
    // The value of a choice is the choice itself, not its index - an index would
    // move every time the list did.
    QCOMPARE(editor.value(QStringLiteral("mode")).toString(), QStringLiteral("fast"));
    QCOMPARE(spy.count(), 6);
    QCOMPARE(editor.isModified(), true);

    // And the other way round: storing a value puts it on the control.
    editor.setValue(QStringLiteral("port"), 90);
    QCOMPARE(editor.findChild<QSpinBox *>()->value(), 90);
    editor.setValue(QStringLiteral("mode"), QStringLiteral("slow"));
    QCOMPARE(editor.findChild<QComboBox *>()->currentText(), QStringLiteral("slow"));
    editor.setValue(QStringLiteral("flag"), false);
    QCOMPARE(editor.findChild<QCheckBox *>()->isChecked(), false);
}

void tst_QxPropertyEditor::labelsTooltipsAndReadOnly()
{
    QxProperty plain = makeProperty(QStringLiteral("named"), QxProperty::String);
    QxProperty nameless = makeProperty(QStringLiteral("bare"), QxProperty::String);
    nameless.label = QString();
    QxProperty locked = makeProperty(QStringLiteral("locked"), QxProperty::String);
    locked.readOnly = true;
    locked.tooltip = QStringLiteral("why not");

    QxPropertyEditor editor;
    editor.addProperty(plain);
    editor.addProperty(nameless);
    editor.addProperty(locked);

    const QList<QLabel *> labels = editor.findChildren<QLabel *>();
    QCOMPARE(labels.count(), 3);
    // Without a label of its own a row is titled by its key, which is better
    // than an empty column.
    QCOMPARE(labels.at(1)->text(), QStringLiteral("bare"));

    const QList<QLineEdit *> lines = editor.findChildren<QLineEdit *>();
    QCOMPARE(lines.count(), 3);
    QVERIFY(lines.at(0)->isEnabled());
    QCOMPARE(lines.at(2)->isEnabled(), false);
    QCOMPARE(lines.at(2)->toolTip(), QStringLiteral("why not"));
}

void tst_QxPropertyEditor::groups()
{
    QxProperty inGroup = makeProperty(QStringLiteral("grouped"), QxProperty::String);
    QxProperty afterGroup = makeProperty(QStringLiteral("after"), QxProperty::String);

    QxPropertyEditor editor;
    editor.addGroup(QStringLiteral("First"), QList<QxProperty>{inGroup});
    // A property added after a group lands inside it: sections are opened by
    // addGroup() and stay open until the next one.
    editor.addProperty(afterGroup);
    editor.addGroup(QStringLiteral("Second"), QList<QxProperty>());

    const QList<QGroupBox *> groups = editor.findChildren<QGroupBox *>();
    QCOMPARE(groups.count(), 2);
    QCOMPARE(groups.at(0)->title(), QStringLiteral("First"));
    QCOMPARE(groups.at(1)->title(), QStringLiteral("Second"));

    QCOMPARE(editor.count(), 2);
    QCOMPARE(groups.at(0)->findChildren<QLineEdit *>().count(), 2);
    QCOMPARE(groups.at(1)->findChildren<QLineEdit *>().count(), 0);
}

TEST_ADD(tst_QxPropertyEditor)

#include "tst_qxpropertyeditor.moc"
