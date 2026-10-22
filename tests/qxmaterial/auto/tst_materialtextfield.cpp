/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#include "tst_global.h"

#include "qxmaterial/materialtextfield.h"

#include "qxtheme/qxthememanager.h"
#include "qxtheme/qxthememotion.h"

#include <QAccessible>
#include <QApplication>
#include <QImage>
#include <QLineEdit>
#include <QPainter>
#include <QSignalSpy>

QX_MATERIAL_USE_NAMESPACE
QX_THEME_USE_NAMESPACE

namespace
{
// A field sized to its hint. The outlined stroke stands at y 8, the editor
// begins at y 30, and the two bands below are the two places the label can be
// - both of them clear of the stroke's own rows and of the editor's.
constexpr int kFieldWidth = 240;
constexpr int kFieldHeight = 56;
constexpr int kLabelLeft = 16;
constexpr int kLabelRight = 224;
constexpr int kFloatedBandTop = 2;
constexpr int kFloatedBandBottom = 7;
constexpr int kRestBandTop = 24;
constexpr int kRestBandBottom = 29;
constexpr int kStrokeTop = 8;
constexpr int kStrokeBottom = 54;
constexpr int kIndicatorY = 55;

/*! The accent, pinned: a pixel probe that depends on the platform's accent
 *  depends on the machine it runs on. */
const QColor kAccent(0, 0, 255);

/*! Whether this platform draws text at all. Qt ships no fonts of its own, so a
 *  headless platform can be one where a string is drawn as nothing - and a
 *  probe that looks for the label's ink then measures the platform rather than
 *  the field. Asked of a canvas of its own, not of a field, so that a field
 *  which stops drawing its label is a failure and not a reason to stand down. */
bool drawsText()
{
    QImage image(48, 24, QImage::Format_ARGB32_Premultiplied);
    image.fill(Qt::white);
    {
        QPainter painter(&image);
        painter.setPen(Qt::black);
        painter.drawText(image.rect(), Qt::AlignCenter, QStringLiteral("Ag"));
    }
    const QColor page(Qt::white);
    for (int y = 0; y < image.height(); ++y) {
        for (int x = 0; x < image.width(); ++x) {
            if (image.pixelColor(x, y) != page) {
                return true;
            }
        }
    }
    return false;
}

QColor pageOf(QWidget *widget)
{
    return widget->palette().color(QPalette::Window);
}

/*! The field as it paints itself, in the coordinates this file thinks in.
 *  QWidget::grab() answers in device pixels - a 240 wide field on a 125%
 *  screen comes back 300 wide - while every number here is a layout number, so
 *  the widget is rendered into an image that stands at one to one. */
QImage capture(QWidget *widget)
{
    QImage image(widget->size(), QImage::Format_ARGB32_Premultiplied);
    image.setDevicePixelRatio(1.0);
    image.fill(pageOf(widget));
    widget->render(&image);
    return image;
}

/*! Rows inside \a y0..y1 that carry something other than the page, looked at
 *  between the side insets so the stroke's own verticals do not count. */
int labelRows(QWidget *widget, int y0, int y1)
{
    const QImage image = capture(widget);
    const QColor page = pageOf(widget);
    int rows = 0;
    for (int y = y0; y < qMin(y1, image.height()); ++y) {
        for (int x = kLabelLeft; x < qMin(kLabelRight, image.width()); ++x) {
            if (image.pixelColor(x, y) != page) {
                ++rows;
                break;
            }
        }
    }
    return rows;
}

/*! The first row that carries ink - a stroke's or a label's - looked at clear
 *  of the side strokes, which are ink on every row they run through. */
int topRow(QWidget *widget)
{
    const QImage image = capture(widget);
    const QColor page = pageOf(widget);
    for (int y = 0; y < image.height(); ++y) {
        for (int x = 20; x < image.width() - 20; ++x) {
            if (image.pixelColor(x, y) != page) {
                return y;
            }
        }
    }
    return -1;
}

/*! True when the band holds a pixel of the accent: the one thing the field
 *  says with colour rather than with a hairline. */
bool hasAccent(const QImage &image, int x0, int y0, int x1, int y1)
{
    for (int y = y0; y < qMin(y1, image.height()); ++y) {
        for (int x = x0; x < qMin(x1, image.width()); ++x) {
            const QColor c = image.pixelColor(x, y);
            if (c.red() < 80 && c.blue() > 200) {
                return true;
            }
        }
    }
    return false;
}

/*! True when the stroke is drawn between \a x0 and \a x1 on the stroke's own
 *  row: the accent counts, a hairline counts, the page does not. */
bool strokePresent(const QImage &image, const QColor &page, int x0, int x1)
{
    for (int x = x0; x < qMin(x1, image.width()); ++x) {
        for (int y = kStrokeTop - 2; y <= kStrokeTop + 2; ++y) {
            if (image.pixelColor(x, y) != page) {
                return true;
            }
        }
    }
    return false;
}

QLineEdit *editorOf(MaterialTextField *field)
{
    return field->findChild<QLineEdit *>();
}
}   // namespace

class tst_MaterialTextField : public QObject
{
    Q_OBJECT
private slots:
    void init();
    void cleanup();

    void theLabelReadsAsThePromptUntilThereIsSomethingToSay();
    void typingKeepsTheLabelOutOfTheWay();
    void theLabelFloatsOntoTheStroke();
    void theGapInTheStrokeIsTheLabelSquaring();
    void theFieldTakesTheAccentWhenItHasTheKeyboard();
    void theTwoVariantsAreTwoAnswersToWhereTheFieldEnds();
    void theTextIsTheEditorsText();
    void enterIsReported();
    void theFieldIsEditableText();
private:
    bool m_wasReduced = false;
    bool m_wasActive = false;
};

void tst_MaterialTextField::init()
{
    // The label travels on a Material duration; a test that has to wait for it
    // to arrive is a test that measures the clock. The token zeroes the
    // duration when motion is reduced, which is the same path a user who asked
    // for less motion gets.
    m_wasReduced = QxTheme::isMotionReduced();
    QxTheme::setMotionReduced(true);
    m_wasActive = QAccessible::isActive();
    QxTheme::QxThemeManager::instance()->setAccentColor(kAccent);
}

void tst_MaterialTextField::cleanup()
{
    QxTheme::setMotionReduced(m_wasReduced);
    QAccessible::setActive(m_wasActive);
    QxTheme::QxThemeManager::instance()->setAccentColor(QColor());
}

/*! This is the whole of what a Material text field is: the label stands where
 *  the text will be typed until there is something to say - a keyboard on the
 *  field, or text in it. A field that has not been shown has neither, which is
 *  what makes it the place to read the resting label from. */
void tst_MaterialTextField::theLabelReadsAsThePromptUntilThereIsSomethingToSay()
{
    if (!drawsText()) {
        QSKIP("this platform draws no text: there are no glyphs to measure");
    }

    MaterialTextField field;
    field.setVariant(MaterialTextField::Variant::Outlined);
    field.setLabel(QStringLiteral("Email"));
    field.resize(kFieldWidth, kFieldHeight);

    QVERIFY(labelRows(&field, kRestBandTop, kRestBandBottom) > 0);
    QCOMPARE(labelRows(&field, kFloatedBandTop, kFloatedBandBottom), 0);

    field.show();
    QVERIFY(QTest::qWaitForWindowExposed(&field));
    field.setFocus();
    QVERIFY(field.hasFocus());

    QVERIFY(labelRows(&field, kFloatedBandTop, kFloatedBandBottom) > 0);
    QCOMPARE(labelRows(&field, kRestBandTop, kRestBandBottom), 0);
}

/*! And it stays out of the way once there is text, even when the keyboard has
 *  gone somewhere else: the label names the field, it does not speak for the
 *  emptiness of it. */
void tst_MaterialTextField::typingKeepsTheLabelOutOfTheWay()
{
    if (!drawsText()) {
        QSKIP("this platform draws no text: there are no glyphs to measure");
    }

    MaterialTextField field;
    field.setVariant(MaterialTextField::Variant::Outlined);
    field.setLabel(QStringLiteral("Email"));
    field.resize(kFieldWidth, kFieldHeight);

    field.setText(QStringLiteral("someone@example.com"));
    QVERIFY(labelRows(&field, kFloatedBandTop, kFloatedBandBottom) > 0);
    QCOMPARE(labelRows(&field, kRestBandTop, kRestBandBottom), 0);

    // Clearing it puts the label back, because there is nothing to say again.
    field.clear();
    QVERIFY(labelRows(&field, kRestBandTop, kRestBandBottom) > 0);
    QCOMPARE(labelRows(&field, kFloatedBandTop, kFloatedBandBottom), 0);
}

/*! An outlined field's label floats onto the stroke rather than into the
 *  field: that is where the room for it was made, so it ends up standing
 *  higher than the highest the stroke itself reaches. */
void tst_MaterialTextField::theLabelFloatsOntoTheStroke()
{
    if (!drawsText()) {
        QSKIP("this platform draws no text: there are no glyphs to measure");
    }

    MaterialTextField field;
    field.setVariant(MaterialTextField::Variant::Outlined);
    field.setLabel(QStringLiteral("Email"));
    field.resize(kFieldWidth, kFieldHeight);

    QCOMPARE(topRow(&field), kStrokeTop);

    field.show();
    QVERIFY(QTest::qWaitForWindowExposed(&field));
    field.setFocus();
    QVERIFY(field.hasFocus());

    QVERIFY(topRow(&field) < kStrokeTop);
}

/*! A stroke through a twelve point label reads as struck through, so the label
 *  opens a window in the stroke where it lands. This is the one place the
 *  label's position can be read off the field without reading its glyphs, and
 *  it is watched just left of where the text starts, where the stroke is the
 *  only thing that can be. */
void tst_MaterialTextField::theGapInTheStrokeIsTheLabelSquaring()
{
    MaterialTextField field;
    field.setVariant(MaterialTextField::Variant::Outlined);
    field.setLabel(QStringLiteral("Email"));
    field.resize(kFieldWidth, kFieldHeight);

    // At rest the label is inside the field, so the stroke runs unbroken over
    // the place it will stand.
    QVERIFY(strokePresent(capture(&field), pageOf(&field), kLabelLeft - 3, kLabelLeft));

    field.show();
    QVERIFY(QTest::qWaitForWindowExposed(&field));
    field.setFocus();
    QVERIFY(field.hasFocus());

    const QImage floated = capture(&field);
    QVERIFY(!strokePresent(floated, pageOf(&field), kLabelLeft - 3, kLabelLeft));
    // And the rest of the stroke is still there: the window is the label's
    // width, not the field's.
    QVERIFY(strokePresent(floated, pageOf(&field), 120, 160));
}

/*! The accent is what a Material field says when it is the one with the
 *  keyboard - the indicator under a filled field, the stroke on an outlined
 *  one. */
void tst_MaterialTextField::theFieldTakesTheAccentWhenItHasTheKeyboard()
{
    MaterialTextField filled;
    filled.setVariant(MaterialTextField::Variant::Filled);
    filled.setLabel(QStringLiteral("Email"));
    filled.resize(kFieldWidth, kFieldHeight);

    QVERIFY(!hasAccent(capture(&filled), 20, kIndicatorY - 3, kFieldWidth - 20, kFieldHeight));

    filled.show();
    QVERIFY(QTest::qWaitForWindowExposed(&filled));
    filled.setFocus();
    QVERIFY(filled.hasFocus());
    QVERIFY(hasAccent(capture(&filled), 20, kIndicatorY - 3, kFieldWidth - 20, kFieldHeight));

    MaterialTextField outlined;
    outlined.setVariant(MaterialTextField::Variant::Outlined);
    outlined.setLabel(QStringLiteral("Email"));
    outlined.resize(kFieldWidth, kFieldHeight);

    QVERIFY(!hasAccent(capture(&outlined), 120, kStrokeBottom - 2, 180, kStrokeBottom + 2));

    outlined.show();
    QVERIFY(QTest::qWaitForWindowExposed(&outlined));
    outlined.setFocus();
    QVERIFY(outlined.hasFocus());
    QVERIFY(hasAccent(capture(&outlined), 120, kStrokeBottom - 2, 180, kStrokeBottom + 2));
}

/*! Filled is an opening with a floor - a container tinted with the page's own
 *  ink - and outlined is a stroke with nothing behind it. */
void tst_MaterialTextField::theTwoVariantsAreTwoAnswersToWhereTheFieldEnds()
{
    MaterialTextField filled;
    filled.setVariant(MaterialTextField::Variant::Filled);
    filled.resize(kFieldWidth, kFieldHeight);
    QVERIFY(capture(&filled).pixelColor(120, 40) != pageOf(&filled));

    MaterialTextField outlined;
    outlined.setVariant(MaterialTextField::Variant::Outlined);
    outlined.resize(kFieldWidth, kFieldHeight);
    QCOMPARE(capture(&outlined).pixelColor(120, 40), pageOf(&outlined));
}

/*! The text is the editor's, and the field reports it the way a line edit
 *  would - including when it is set rather than typed. */
void tst_MaterialTextField::theTextIsTheEditorsText()
{
    MaterialTextField field;
    field.resize(kFieldWidth, kFieldHeight);
    QSignalSpy changed(&field, &MaterialTextField::textChanged);

    QLineEdit *editor = editorOf(&field);
    QVERIFY(editor);

    field.setText(QStringLiteral("someone@example.com"));
    QCOMPARE(field.text(), QStringLiteral("someone@example.com"));
    QCOMPARE(editor->text(), QStringLiteral("someone@example.com"));
    QCOMPARE(changed.count(), 1);

    // What the user types is the field's text too, not only what the
    // application set.
    QTest::keyClick(editor, Qt::Key_Exclam);
    QCOMPARE(field.text(), editor->text());
    QCOMPARE(changed.count(), 2);

    field.clear();
    QCOMPARE(field.text(), QString());
    QCOMPARE(editor->text(), QString());
}

/*! A single-line field has one key that means "done", and it is reported. */
void tst_MaterialTextField::enterIsReported()
{
    MaterialTextField field;
    field.resize(kFieldWidth, kFieldHeight);
    QSignalSpy pressed(&field, &MaterialTextField::returnPressed);
    QSignalSpy finished(&field, &MaterialTextField::editingFinished);

    QTest::keyClick(editorOf(&field), Qt::Key_Return);
    QCOMPARE(pressed.count(), 1);
    QCOMPARE(finished.count(), 1);
}

/*! A text field is text an assistive tool can read and write, and it says so
 *  rather than being a Client with a rectangle. */
void tst_MaterialTextField::theFieldIsEditableText()
{
    QAccessible::setActive(true);
    MaterialTextField field;

    QAccessibleInterface *iface = QAccessible::queryAccessibleInterface(&field);
    QVERIFY(iface);
    QCOMPARE(iface->role(), QAccessible::EditableText);

    QAccessible::deleteAccessibleInterface(QAccessible::uniqueId(iface));
}

TEST_ADD(tst_MaterialTextField)

#include "tst_materialtextfield.moc"
