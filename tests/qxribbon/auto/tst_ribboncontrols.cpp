#include "tst_global.h"

#include "qxribbon/ribboncontrols.h"

#include <QCheckBox>
#include <QComboBox>
#include <QLineEdit>
#include <QToolButton>
#include <QWidget>

QX_RIBBON_USE_NAMESPACE

/*
 * ribboncontrols.h holds five classes that add nothing to the Qt class they derive from:
 * a constructor that forwards to the base and nothing else. That is why none of them has
 * a test - there is no behaviour to pin. It is also why they are worth pinning anyway:
 * they are exported, so a caller can build against them today, and the only thing that
 * is currently guaranteed about them is the base class.
 *
 * These cases assert exactly that: which Qt class each one is, that the defaults a caller
 * sees are the base class defaults, and that the parent passed in is wired up the way Qt
 * wires it. They will fail the moment one of these stops being a plain subclass, which is
 * the point - the failure then says "write the real test here".
 */
class tst_RibbonControls : public QObject
{
    Q_OBJECT
private slots:
    void controlIsAPlainWidget();
    void checkBoxIsACheckBox();
    void comboBoxIsAComboBox();
    void controlButtonIsAToolButton();
    void lineEditIsALineEdit();
    void constructorWiresTheParent();
};

void tst_RibbonControls::controlIsAPlainWidget()
{
    RibbonControl control;

    // className() carries the QxRibbon namespace, and the build can switch that off
    // (QX_RIBBON_NAMESPACE_DISABLE), so compare the tail rather than the whole name.
    QVERIFY(QString(control.metaObject()->className()).endsWith(QStringLiteral("RibbonControl")));
    QVERIFY(control.inherits("QWidget"));
    // No size hint override, no painting, no children: everything about this class is
    // inherited. Asserting that is the whole contract until it grows one.
    QVERIFY(control.children().isEmpty());
    QVERIFY(!control.sizeHint().isValid());
}

void tst_RibbonControls::checkBoxIsACheckBox()
{
    RibbonCheckBox box;

    QVERIFY(QString(box.metaObject()->className()).endsWith(QStringLiteral("RibbonCheckBox")));
    QVERIFY(box.inherits("QCheckBox"));
    QCOMPARE(box.checkState(), Qt::Unchecked);
    QVERIFY(box.text().isEmpty());
    QVERIFY(!box.isTristate());
}

void tst_RibbonControls::comboBoxIsAComboBox()
{
    RibbonComboBox box;

    QVERIFY(QString(box.metaObject()->className()).endsWith(QStringLiteral("RibbonComboBox")));
    QVERIFY(box.inherits("QComboBox"));
    QCOMPARE(box.count(), 0);
    QVERIFY(!box.isEditable());
    QCOMPARE(box.currentIndex(), -1);
    QCOMPARE(box.insertPolicy(), QComboBox::InsertAtBottom);
}

void tst_RibbonControls::controlButtonIsAToolButton()
{
    RibbonControlButton button;

    QVERIFY(QString(button.metaObject()->className()).endsWith(QStringLiteral("RibbonControlButton")));
    QVERIFY(button.inherits("QToolButton"));
    // The base class default, and the one RibbonFactory overrides when it builds the
    // hide-group button - see tst_ribbonfactory.
    QCOMPARE(button.autoRaise(), false);
    QCOMPARE(button.toolButtonStyle(), Qt::ToolButtonIconOnly);
}

void tst_RibbonControls::lineEditIsALineEdit()
{
    RibbonLineEdit edit;

    QVERIFY(QString(edit.metaObject()->className()).endsWith(QStringLiteral("RibbonLineEdit")));
    QVERIFY(edit.inherits("QLineEdit"));
    QVERIFY(edit.text().isEmpty());
    QVERIFY(edit.isEnabled());
    QVERIFY(edit.echoMode() == QLineEdit::Normal);
}

void tst_RibbonControls::constructorWiresTheParent()
{
    QWidget parent;

    RibbonControl control(&parent);
    RibbonCheckBox checkBox(&parent);
    RibbonComboBox comboBox(&parent);
    RibbonControlButton button(&parent);
    RibbonLineEdit edit(&parent);

    QCOMPARE(control.parentWidget(), &parent);
    QCOMPARE(checkBox.parentWidget(), &parent);
    QCOMPARE(comboBox.parentWidget(), &parent);
    QCOMPARE(button.parentWidget(), &parent);
    QCOMPARE(edit.parentWidget(), &parent);
}

TEST_ADD(tst_RibbonControls)

#include "tst_ribboncontrols.moc"
