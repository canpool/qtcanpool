/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#include "tst_global.h"

#include "qxcupertino/cupertinobutton.h"
#include "qxcupertino/cupertinotokens.h"

#include <QAccessible>
#include <QApplication>
#include <QFontMetrics>
#include <QImage>
#include <QSignalSpy>

QX_CUPERTINO_USE_NAMESPACE

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

/*! \a colour laid over \a page at \a opacity: what a tinted surface is. */
QColor over(const QColor &colour, const QColor &page, qreal opacity)
{
    return QColor(qRound(colour.red() * opacity + page.red() * (1.0 - opacity)),
                  qRound(colour.green() * opacity + page.green() * (1.0 - opacity)),
                  qRound(colour.blue() * opacity + page.blue() * (1.0 - opacity)));
}
}   // namespace

class tst_CupertinoButton : public QObject
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

void tst_CupertinoButton::cleanup()
{
    QAccessible::setActive(false);
}

/*! A button's level is the whole of what tells one from another: the same word
 *  on three different amounts of surface. */
void tst_CupertinoButton::theLevelsStandOnDifferentSurfaces()
{
    CupertinoButton filled(QStringLiteral("Continue"));
    filled.resize(filled.sizeHint());
    CupertinoButton tinted(QStringLiteral("Continue"));
    tinted.setRole(CupertinoButton::Role::Tinted);
    tinted.resize(tinted.sizeHint());
    CupertinoButton plain(QStringLiteral("Continue"));
    plain.setRole(CupertinoButton::Role::Plain);
    plain.resize(plain.sizeHint());

    // Read between the round end and the word, where the surface is.
    const int x = 6;
    const int y = filled.height() / 2;
    const QColor page = pageOf(&filled);
    const QColor accent = CupertinoColor::color(CupertinoColor::Role::Accent, onDarkFace(&filled));

    // The one action on the surface is the family's blue itself.
    QVERIFY(closeTo(pixel(&filled, x, y), accent));
    // The ones beside it are the same blue thinned into the page: neither the
    // blue nor the page, and nearer the page than the blue.
    const QColor washed = pixel(&tinted, x, y);
    QVERIFY(closeTo(washed, over(accent, page, 0.15), 10));
    QVERIFY(washed != accent);
    QVERIFY(washed != page);
    // An action that is only a word stands on nothing at all.
    QCOMPARE(pixel(&plain, x, y), page);
}

/*! The capsule is the roundest shape the family has: the corner of the widget
 *  is page, the side at half the height is surface, and so is the middle of the
 *  top edge - which together is a capsule and not an ellipse. */
void tst_CupertinoButton::theCapsuleIsRoundAndOpenAtItsCorners()
{
    CupertinoButton button(QStringLiteral("Continue"));
    button.resize(button.sizeHint());

    QCOMPARE(pixel(&button, 0, 0), pageOf(&button));
    QVERIFY(pixel(&button, 2, button.height() / 2) != pageOf(&button));
    // The mid of the top edge is surface as well, so the shape is a capsule and
    // not a diamond.
    QVERIFY(pixel(&button, button.width() / 2, 1) != pageOf(&button));

    // A semicircular end of radius half the height reaches the top one point
    // short of the arc's centre; a half-ellipse stretched over the whole width
    // is still far below the top there.
    QVERIFY(pixel(&button, button.height() / 2 - 1, 1) != pageOf(&button));
}

/*! How wide a button is, is how wide its word is: no fixed width, and the room
 *  either side is the family's, not the word's. */
void tst_CupertinoButton::sizeHintFollowsTheWord()
{
    CupertinoButton shortWord(QStringLiteral("Go"));
    CupertinoButton longWord(QStringLiteral("Continue where you left off"));

    const QFontMetrics metrics(CupertinoType::font(CupertinoType::Ramp::Body));
    QCOMPARE(shortWord.sizeHint().width(), qRound(2.0 * 16.0 + metrics.horizontalAdvance(QStringLiteral("Go"))));
    QVERIFY(longWord.sizeHint().width() > shortWord.sizeHint().width());
    QCOMPARE(shortWord.sizeHint().height(), int(32));
    // Narrower than a word can be is not narrower than the capsule can be.
    QCOMPARE(shortWord.minimumSizeHint(), QSize(int(32), int(32)));
}

/*! A button answers to the pointer, to Enter and to Space - and Space answers
 *  on the release, so holding the key down is one press and not a stream. */
void tst_CupertinoButton::aClickIsAClickWhicheverWayItComes()
{
    CupertinoButton button(QStringLiteral("Continue"));
    button.resize(button.sizeHint());
    QSignalSpy clicked(&button, &CupertinoButton::clicked);

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
void tst_CupertinoButton::theWordIsItsAccessibleName()
{
    QAccessible::setActive(true);
    CupertinoButton button(QStringLiteral("Continue"));
    QCOMPARE(button.accessibleName(), QStringLiteral("Continue"));
    QCOMPARE(button.text(), QStringLiteral("Continue"));

    button.setText(QStringLiteral("Go on"));
    QCOMPARE(button.accessibleName(), QStringLiteral("Go on"));

    button.setAccessibleName(QStringLiteral("Resume the download"));
    button.setText(QStringLiteral("Resume"));
    QCOMPARE(button.accessibleName(), QStringLiteral("Resume the download"));

    // And an assistive tool is told it is a button, not a panel.
    QAccessibleInterface *iface = QAccessible::queryAccessibleInterface(&button);
    QVERIFY(iface);
    QCOMPARE(iface->role(), QAccessible::PushButton);
    QAccessible::deleteAccessibleInterface(QAccessible::uniqueId(iface));
}

/*! A disabled button is drawn and does not answer: neither the pointer nor the
 *  keyboard gets a click out of it. */
void tst_CupertinoButton::aButtonThatIsOffDoesNotAnswer()
{
    CupertinoButton button(QStringLiteral("Continue"));
    button.resize(button.sizeHint());
    button.setEnabled(false);
    QSignalSpy clicked(&button, &CupertinoButton::clicked);

    QTest::mouseClick(&button, Qt::LeftButton, Qt::NoModifier, QPoint(button.width() / 2, button.height() / 2));
    QTest::keyClick(&button, Qt::Key_Return);
    QTest::keyClick(&button, Qt::Key_Space);
    QCOMPARE(clicked.count(), 0);

    // Still drawn, though: a button that vanished when it was not available
    // would leave a hole where an action is going to be.
    QVERIFY(pixel(&button, 6, button.height() / 2) != pageOf(&button));
}

TEST_ADD(tst_CupertinoButton)

#include "tst_cupertinobutton.moc"
