/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#include "tst_global.h"

#include "qxharmony/harmonybutton.h"
#include "qxharmony/harmonytokens.h"

#include <QAccessible>
#include <QApplication>
#include <QFontMetrics>
#include <QImage>
#include <QSignalSpy>

QX_HARMONY_USE_NAMESPACE

namespace
{
QColor pixel(QWidget *widget, int x, int y)
{
    return widget->grab().toImage().pixelColor(x, y);
}

/*! The page the button lies on. */
QColor pageOf(QWidget *widget)
{
    return widget->palette().color(QPalette::Window);
}

/*! Which cut of the semantic colours this palette asks for, read the way the
 *  family reads it. */
bool onDarkFace(QWidget *widget)
{
    return widget->palette().color(QPalette::Window).lightness() < 128;
}

bool closeTo(const QColor &a, const QColor &b, int tolerance = 8)
{
    return qAbs(a.red() - b.red()) <= tolerance && qAbs(a.green() - b.green()) <= tolerance &&
           qAbs(a.blue() - b.blue()) <= tolerance;
}

/*! \a colour laid over \a page at \a opacity: what a thinned surface is. */
QColor over(const QColor &colour, const QColor &page, qreal opacity)
{
    return QColor(qRound(colour.red() * opacity + page.red() * (1.0 - opacity)),
                  qRound(colour.green() * opacity + page.green() * (1.0 - opacity)),
                  qRound(colour.blue() * opacity + page.blue() * (1.0 - opacity)));
}
}   // namespace

class tst_HarmonyButton : public QObject
{
    Q_OBJECT
private slots:
    void cleanup();

    void theLevelsStandOnDifferentSurfaces();
    void theCapsuleIsRoundAndOpenAtItsCorners();
    void sizeHintFollowsTheWord();
    void aClickIsAClickWhicheverWayItComes();
    void theWordIsItsAccessibleName();
    void aButtonThatIsOffDoesNotAnswer();
};

void tst_HarmonyButton::cleanup()
{
    QAccessible::setActive(false);
}

/*! A button's level is how much surface its word stands on - and here the second
 *  level is the family's grey, not a thinned blue: one action in a row of two is
 *  the point of having levels at all. */
void tst_HarmonyButton::theLevelsStandOnDifferentSurfaces()
{
    HarmonyButton primary(QStringLiteral("Continue"));
    primary.resize(primary.sizeHint());
    HarmonyButton secondary(QStringLiteral("Continue"));
    secondary.setKind(HarmonyButton::Kind::Secondary);
    secondary.resize(secondary.sizeHint());
    HarmonyButton text(QStringLiteral("Continue"));
    text.setKind(HarmonyButton::Kind::Text);
    text.resize(text.sizeHint());

    // Read between the round end and the word, where the surface is: the word
    // starts one padding in, so the first ten points are the capsule whatever
    // the font turns out to be.
    const int x = 10;
    const int y = primary.height() / 2;
    const QColor page = pageOf(&primary);
    const QColor accent = HarmonyColor::color(HarmonyColor::Role::Accent, onDarkFace(&primary));
    const QColor grey = HarmonyColor::color(HarmonyColor::Role::Gray, onDarkFace(&primary));

    // The one action on the surface is the brand's blue itself.
    QVERIFY(closeTo(pixel(&primary, x, y), accent));
    // The ones beside it stand on the family's grey thinned into the page.
    const QColor neutral = pixel(&secondary, x, y);
    QVERIFY(closeTo(neutral, over(grey, page, 0.12), 10));
    QVERIFY(neutral != page);
    // An action that is only a word stands on nothing at all.
    QCOMPARE(pixel(&text, x, y), page);
}

/*! The capsule is fully round: the corner of the widget is page, the side at
 *  half the height is surface, and the middle of the top edge is surface. The
 *  last two together are what makes it a capsule rather than an ellipse - an
 *  ellipse would start its top edge only at the middle. */
void tst_HarmonyButton::theCapsuleIsRoundAndOpenAtItsCorners()
{
    HarmonyButton button(QStringLiteral("Continue"));
    button.resize(button.sizeHint());

    QCOMPARE(pixel(&button, 0, 0), pageOf(&button));
    QVERIFY(pixel(&button, 2, button.height() / 2) != pageOf(&button));
    QVERIFY(pixel(&button, button.width() / 2, 1) != pageOf(&button));

    // A semicircular end of radius half the height reaches the top one point
    // short of the arc's centre; a half-ellipse stretched over the whole width
    // is still far below the top there.
    QVERIFY(pixel(&button, button.height() / 2 - 1, 1) != pageOf(&button));
}

/*! How wide a button is, is how wide its word is - with the family's own room
 *  either side of it, which is more than the other family's. */
void tst_HarmonyButton::sizeHintFollowsTheWord()
{
    HarmonyButton shortWord(QStringLiteral("Go"));
    HarmonyButton longWord(QStringLiteral("Continue where you left off"));

    const QFontMetrics metrics(HarmonyType::font(HarmonyType::Ramp::Body));
    QCOMPARE(shortWord.sizeHint().width(), qRound(2.0 * 20.0 + metrics.horizontalAdvance(QStringLiteral("Go"))));
    QVERIFY(longWord.sizeHint().width() > shortWord.sizeHint().width());
    QCOMPARE(shortWord.sizeHint().height(), int(40));
    QCOMPARE(shortWord.minimumSizeHint(), QSize(int(40), int(40)));
}

/*! A button answers to the pointer, to Enter and to Space - and Space answers on
 *  the release, so holding the key down is one press and not a stream. */
void tst_HarmonyButton::aClickIsAClickWhicheverWayItComes()
{
    HarmonyButton button(QStringLiteral("Continue"));
    button.resize(button.sizeHint());
    QSignalSpy clicked(&button, &HarmonyButton::clicked);

    QTest::mouseClick(&button, Qt::LeftButton, Qt::NoModifier, QPoint(button.width() / 2, button.height() / 2));
    QCOMPARE(clicked.count(), 1);

    QTest::keyClick(&button, Qt::Key_Return);
    QCOMPARE(clicked.count(), 2);

    QTest::keyClick(&button, Qt::Key_Space);
    QCOMPARE(clicked.count(), 3);

    // An auto-repeated Space is still held down, and a held button is one press.
    QKeyEvent held(QEvent::KeyRelease, Qt::Key_Space, Qt::NoModifier, QString(), true);
    QApplication::sendEvent(&button, &held);
    QCOMPARE(clicked.count(), 3);
}

/*! What a button says is what it is called, for as long as the application has
 *  not named it something else - and a name of the application's survives the
 *  word being replaced. */
void tst_HarmonyButton::theWordIsItsAccessibleName()
{
    QAccessible::setActive(true);
    HarmonyButton button(QStringLiteral("Continue"));
    QCOMPARE(button.accessibleName(), QStringLiteral("Continue"));

    button.setText(QStringLiteral("Go on"));
    QCOMPARE(button.accessibleName(), QStringLiteral("Go on"));

    button.setAccessibleName(QStringLiteral("Resume the download"));
    button.setText(QStringLiteral("Resume"));
    QCOMPARE(button.accessibleName(), QStringLiteral("Resume the download"));

    QAccessibleInterface *iface = QAccessible::queryAccessibleInterface(&button);
    QVERIFY(iface);
    QCOMPARE(iface->role(), QAccessible::PushButton);
    QAccessible::deleteAccessibleInterface(QAccessible::uniqueId(iface));
}

/*! A disabled button is drawn and does not answer: neither the pointer nor the
 *  keyboard gets a click out of it. */
void tst_HarmonyButton::aButtonThatIsOffDoesNotAnswer()
{
    HarmonyButton button(QStringLiteral("Continue"));
    button.resize(button.sizeHint());
    button.setEnabled(false);
    QSignalSpy clicked(&button, &HarmonyButton::clicked);

    QTest::mouseClick(&button, Qt::LeftButton, Qt::NoModifier, QPoint(button.width() / 2, button.height() / 2));
    QTest::keyClick(&button, Qt::Key_Return);
    QTest::keyClick(&button, Qt::Key_Space);
    QCOMPARE(clicked.count(), 0);

    // Still drawn, though: a button that vanished when it was not available
    // would leave a hole where an action is going to be.
    QVERIFY(pixel(&button, 10, button.height() / 2) != pageOf(&button));
}

TEST_ADD(tst_HarmonyButton)

#include "tst_harmonybutton.moc"
