/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#include "tst_global.h"

#include "qxfluent/fluentcombobox.h"

#include <QSignalSpy>
#include <QtGui/QGuiApplication>
#include <QtGui/QScreen>
#include <QtWidgets/QApplication>
#include <QtTest/QTest>

QX_FLUENT_USE_NAMESPACE

namespace
{
FluentComboBox *comboWith(QWidget *parent, int count, const QString &prefix)
{
    FluentComboBox *combo = new FluentComboBox(parent);
    for (int i = 0; i < count; ++i) {
        combo->addItem(QStringLiteral("%1 %2").arg(prefix).arg(i));
    }
    combo->setCurrentIndex(0);
    combo->resize(combo->sizeHint());
    return combo;
}
}   // namespace

class tst_FluentComboBox : public QObject
{
    Q_OBJECT
private slots:
    void theAnswerOpensBelowWhenThereIsRoom();
    void theListTurnsAboveWhenBelowHasNoRoom();
    void theListKeepsInsideTheScreenWhenNeitherHasRoom();
    void theKeyboardWalksTheListAndAnswers();
    void theSameChoiceSaidTwiceIsSilence();
    void aClickOnTheFaceLeavesTheListOpen();
    void theReleaseThatEndsTheOpeningClickIsNoAnswer();
    void aClickBesideTheRowsIsNoAnswer();
    void theRowUnderThePointerLights();
    void aClickOffTheListPutsItAway();
};

/*! The standing answer: the list opens under the face, one pixel of air
 *  between them. */
void tst_FluentComboBox::theAnswerOpensBelowWhenThereIsRoom()
{
    QWidget window;
    window.resize(400, 300);
    FluentComboBox *combo = comboWith(&window, 4, QStringLiteral("Item"));
    combo->move(50, 100);
    window.show();
    QTest::qWait(50);

    combo->showPopup();
    QVERIFY(combo->popup());
    QVERIFY(combo->popup()->isVisible());
    // Below: the list's top is under the face's bottom, on the same screen.
    QVERIFY(combo->popup()->y() >= combo->mapToGlobal(QPoint(0, combo->height())).y());
    QVERIFY(combo->popup()->x() >= combo->mapToGlobal(QPoint(0, 0)).x());

    // A click on a row chooses it, and the choice is in force.
    QTest::mouseClick(combo->popup(), Qt::LeftButton, Qt::NoModifier,
                      QPoint(combo->popup()->width() / 2, int(4 + 40 / 2.0)));
    QCOMPARE(combo->currentIndex(), 0);
    QVERIFY(!combo->popup()->isVisible());
    window.close();
}

/*! Below has no room for the list, so the list turns above the face. The
 *  face stands near the bottom of the screen, as far down as the room
 *  allows. */
void tst_FluentComboBox::theListTurnsAboveWhenBelowHasNoRoom()
{
    QWidget window;
    window.resize(400, 600);
    FluentComboBox *combo = comboWith(&window, 4, QStringLiteral("Item"));
    const QRect room = QGuiApplication::primaryScreen()->availableGeometry();
    combo->move(50, room.bottom() - combo->height() - 4);
    window.show();
    QTest::qWait(50);

    combo->showPopup();
    QVERIFY(combo->popup()->isVisible());
    // Above: the list's bottom is at the face's top, and it stayed inside
    // the screen.
    QVERIFY(combo->popup()->y() + combo->popup()->height() <= combo->mapToGlobal(QPoint(0, 0)).y());
    QVERIFY(combo->popup()->y() >= room.top());
    combo->hidePopup();
    window.close();
}

/*! Neither side has room: the list keeps inside the screen, even over the
 *  face it opened from - a list clipped by the screen is no list at all.
 *  The case needs a screen too short for a face to have the list's height
 *  on both sides of it, which a screen twice the list plus a face tall
 *  cannot offer. */
void tst_FluentComboBox::theListKeepsInsideTheScreenWhenNeitherHasRoom()
{
    QWidget window;
    window.resize(400, 600);
    // Twelve rows make the list 320 tall, taller than what either side of a
    // face standing in the middle of the screen can offer.
    FluentComboBox *combo = comboWith(&window, 12, QStringLiteral("Item"));
    const QRect room = combo->screen()->availableGeometry();
    const int listHeight = 328;
    // Standing in the middle of the room, the face has half of the room on
    // either side of it; the case exists where each half is less than the
    // list, which a screen twice the list plus a face tall cannot offer.
    if (room.height() - combo->height() >= 2 * listHeight) {
        QSKIP("the screen is too tall for either side to run out of room");
    }
    combo->move(50, (room.height() - combo->height()) / 2);
    window.show();
    QTest::qWait(50);

    combo->showPopup();
    QVERIFY(combo->popup()->isVisible());
    QVERIFY(combo->popup()->y() >= room.top());
    QVERIFY(combo->popup()->y() + combo->popup()->height() <= room.bottom() + 1);
    combo->hidePopup();
    window.close();
}

/*! The keyboard walks the list - down, home, end - and answers with Enter,
 *  and leaves it alone with Escape. */
void tst_FluentComboBox::theKeyboardWalksTheListAndAnswers()
{
    QWidget window;
    window.resize(400, 600);
    FluentComboBox *combo = comboWith(&window, 4, QStringLiteral("Item"));
    combo->move(50, 100);
    window.show();
    QTest::qWait(50);

    combo->showPopup();
    QWidget *list = combo->popup();
    QTest::keyClick(list, Qt::Key_Down);
    QTest::keyClick(list, Qt::Key_Home);
    QTest::keyClick(list, Qt::Key_End);
    QTest::keyClick(list, Qt::Key_Enter);
    QCOMPARE(combo->currentIndex(), 3);
    QVERIFY(!combo->popup()->isVisible());

    // Escape leaves the choice as it was.
    combo->showPopup();
    QTest::keyClick(combo->popup(), Qt::Key_Escape);
    QVERIFY(!combo->popup()->isVisible());
    QCOMPARE(combo->currentIndex(), 3);
    window.close();
}

/*! The same choice said twice is silence: the index signal speaks when the
 *  choice moved, and only then. */
void tst_FluentComboBox::theSameChoiceSaidTwiceIsSilence()
{
    FluentComboBox combo;
    combo.addItem(QStringLiteral("One"));
    combo.addItem(QStringLiteral("Two"));
    QSignalSpy changed(&combo, &FluentComboBox::currentIndexChanged);

    combo.setCurrentIndex(1);
    QCOMPARE(changed.count(), 1);
    combo.setCurrentIndex(1);
    QCOMPARE(changed.count(), 1);
    combo.setCurrentIndex(5);
    QCOMPARE(changed.count(), 1);
    QCOMPARE(combo.currentIndex(), 1);
}

/*! The click that opens the list opens it, and the rest of that same click
 *  leaves it alone.
 *
 *  The press lands on the face, the list opens under it, and the grab has
 *  already moved to the list by the time the button comes up - so the release
 *  arrives at the list, not at the face. A list that answers any release
 *  closes on the very click that opened it: it shows on the press and is gone
 *  by the release, which is too fast to read, let alone choose from.
 *
 *  The press is replayed to whatever has the grab when it is opened, and a
 *  replayed press lands where the pointer is - outside the surface - which is
 *  a click away as far as Qt is concerned. \c WA_NoMouseReplay is what Qt's own
 *  menus and combo boxes set to keep that from happening, and the assertion
 *  below is the one thing a test can hold of it: the mechanism is inside the
 *  platform, and a synthetic event never goes through it.
 */
void tst_FluentComboBox::aClickOnTheFaceLeavesTheListOpen()
{
    QWidget window;
    window.resize(400, 300);
    FluentComboBox *combo = comboWith(&window, 4, QStringLiteral("Item"));
    combo->move(50, 100);
    window.show();
    QTest::qWait(50);

    QTest::mousePress(combo, Qt::LeftButton);
    QVERIFY(combo->popup());
    QVERIFY(combo->popup()->isVisible());
    QTest::mouseRelease(combo, Qt::LeftButton);
    QTest::qWait(50);
    QVERIFY(combo->popup()->isVisible());
    QCOMPARE(combo->currentIndex(), 0);

    QVERIFY(combo->popup()->testAttribute(Qt::WA_NoMouseReplay));
    window.close();
}

/*! The list answers a click that started on the list.
 *
 *  A click that opens the list can end with the pointer over a row, because
 *  the list does not always stand clear of the field: in a window too small
 *  for it either side, or near the bottom of the screen, it is laid over the
 *  field itself and the pointer comes up inside it. Reading that release as an
 *  answer picks a choice nobody made, on a click that only meant to open the
 *  list - the list shows on the press and is gone by the release, too quick to
 *  read, let alone choose from.
 */
void tst_FluentComboBox::theReleaseThatEndsTheOpeningClickIsNoAnswer()
{
    QWidget window;
    window.resize(400, 300);
    FluentComboBox *combo = comboWith(&window, 4, QStringLiteral("Item"));
    combo->move(50, 100);
    window.show();
    QTest::qWait(50);
    combo->setCurrentIndex(2);

    combo->showPopup();
    QVERIFY(combo->popup()->isVisible());

    // The button comes up over the first row of a list nothing was pressed on.
    const int firstRow = int(combo->popup()->contentsRect().top() + 4 + 40 / 2.0);
    QTest::mouseRelease(combo->popup(), Qt::LeftButton, Qt::NoModifier, QPoint(combo->popup()->width() / 2, firstRow));

    QVERIFY(combo->popup()->isVisible());
    QCOMPARE(combo->currentIndex(), 2);
    window.close();
}

/*! A place beside the rows is not a row.
 *
 *  The surface is a window with a shadow's margin around the list, and while
 *  it is up the pointer belongs to it: everything the pointer does anywhere on
 *  the screen arrives here with the place it happened measured from this
 *  window's corner. A row read from the height alone answers clicks that
 *  landed beside the list - the margin, or anywhere to the left of the rows -
 *  with whichever answer stands at that height, so a click that meant "put
 *  this away" picks something instead.
 */
void tst_FluentComboBox::aClickBesideTheRowsIsNoAnswer()
{
    QWidget window;
    window.resize(400, 300);
    FluentComboBox *combo = comboWith(&window, 4, QStringLiteral("Item"));
    combo->move(50, 100);
    window.show();
    QTest::qWait(50);
    combo->setCurrentIndex(2);

    combo->showPopup();
    QVERIFY(combo->popup()->isVisible());

    const QRect margin = combo->popup()->contentsRect();
    // Inside the surface's window, level with the first row, and off the list.
    const QPoint beside(margin.left() / 2, int(margin.top() + 4 + 40 / 2.0));
    QTest::mouseClick(combo->popup(), Qt::LeftButton, Qt::NoModifier, beside);

    QVERIFY(combo->popup()->isVisible());
    QCOMPARE(combo->currentIndex(), 2);
    window.close();
}

/*! The list lights the row under the pointer, and to know that it has to hear
 *  the pointer: a widget is told where the mouse is while it moves only when
 *  it asked to be, and a list that never asked has no hover at all - the
 *  pointer crosses its rows and nothing answers. */
void tst_FluentComboBox::theRowUnderThePointerLights()
{
    QWidget window;
    window.resize(400, 300);
    FluentComboBox *combo = comboWith(&window, 4, QStringLiteral("Item"));
    combo->move(50, 100);
    window.show();
    QTest::qWait(50);

    combo->showPopup();
    QVERIFY(combo->popup()->isVisible());
    QVERIFY(combo->popup()->hasMouseTracking());
    window.close();
}

/*! A press that lands off the surface altogether is the pointer asking for it
 *  to go, and it goes without carrying an answer out with it. While the list
 *  is up it holds the pointer, so a click on the page behind arrives here as
 *  well - measured from this window's corner, and outside it. */
void tst_FluentComboBox::aClickOffTheListPutsItAway()
{
    QWidget window;
    window.resize(400, 300);
    FluentComboBox *combo = comboWith(&window, 4, QStringLiteral("Item"));
    combo->move(50, 100);
    window.show();
    QTest::qWait(50);
    combo->setCurrentIndex(2);

    combo->showPopup();
    QVERIFY(combo->popup()->isVisible());

    const QPoint off(combo->popup()->width() + 40, 20);
    QTest::mouseClick(combo->popup(), Qt::LeftButton, Qt::NoModifier, off);

    QVERIFY(!combo->popup()->isVisible());
    QCOMPARE(combo->currentIndex(), 2);
    window.close();
}

TEST_ADD(tst_FluentComboBox)

#include "tst_fluentcombobox.moc"
