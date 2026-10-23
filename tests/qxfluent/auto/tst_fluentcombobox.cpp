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

TEST_ADD(tst_FluentComboBox)

#include "tst_fluentcombobox.moc"
