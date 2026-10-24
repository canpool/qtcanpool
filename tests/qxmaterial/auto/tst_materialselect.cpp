/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#include "tst_global.h"

#include "qxmaterial/materialselect.h"

#include "qxtheme/qxthememanager.h"

#include <QImage>
#include <QSignalSpy>
#include <QtWidgets/QApplication>
#include <QtTest/QTest>

QX_MATERIAL_USE_NAMESPACE
QX_THEME_USE_NAMESPACE

namespace
{
const QColor kAccent(0, 0, 255);

MaterialSelect *selectWith(QWidget *parent, int count)
{
    MaterialSelect *select = new MaterialSelect(parent);
    for (int i = 0; i < count; ++i) {
        select->addItem(QStringLiteral("Option %1").arg(i));
    }
    select->setCurrentIndex(0);
    select->resize(select->sizeHint());
    return select;
}
}   // namespace

class tst_MaterialSelect : public QObject
{
    Q_OBJECT
private slots:
    void init();
    void cleanup();

    void bothFacesOpenTheirList();
    void theKeyboardWalksTheListAndAnswers();
    void theChosenRowWearsTheMarkAtItsLeft();
    void aRowIsFortyEightTall();
    void theSameChoiceSaidTwiceIsSilence();
    void theReleaseThatEndsTheOpeningClickIsNoAnswer();
private:
    bool m_hadOverride = false;
};

void tst_MaterialSelect::init()
{
    m_hadOverride = QxTheme::QxThemeManager::instance()->hasAccentOverride();
    QxTheme::QxThemeManager::instance()->setAccentColor(kAccent);
}

void tst_MaterialSelect::cleanup()
{
    if (!m_hadOverride) {
        QxTheme::QxThemeManager::instance()->setAccentColor(QColor());
    }
}

/*! Either face of the field carries the list: a press on the filled one and
 *  a press on the outlined one both open it, and a row chosen in it is the
 *  answer in force. */
void tst_MaterialSelect::bothFacesOpenTheirList()
{
    QWidget window;
    window.resize(400, 400);
    MaterialSelect *filled = selectWith(&window, 4);
    filled->move(50, 100);
    filled->show();
    QTest::qWait(50);

    QTest::mouseClick(filled, Qt::LeftButton, Qt::NoModifier, QPoint(filled->width() / 2, filled->height() / 2));
    QVERIFY(filled->popup());
    QVERIFY(filled->popup()->isVisible());
    QTest::mouseClick(filled->popup(), Qt::LeftButton, Qt::NoModifier,
                      QPoint(filled->popup()->width() / 2, filled->popup()->contentsRect().top() + int(48 / 2.0)));
    QCOMPARE(filled->currentIndex(), 0);
    QVERIFY(!filled->popup()->isVisible());

    MaterialSelect *outlined = selectWith(&window, 4);
    outlined->setVariant(MaterialTextField::Variant::Outlined);
    outlined->move(50, 200);
    outlined->show();
    QTest::qWait(50);

    outlined->showPopup();
    QVERIFY(outlined->popup()->isVisible());
    outlined->hidePopup();
    QVERIFY(!outlined->popup()->isVisible());
    window.close();
}

/*! The keyboard walks the list and answers with Enter, and Escape leaves the
 *  choice as it was. */
void tst_MaterialSelect::theKeyboardWalksTheListAndAnswers()
{
    QWidget window;
    window.resize(400, 600);
    MaterialSelect *select = selectWith(&window, 4);
    select->move(50, 100);
    window.show();
    QTest::qWait(50);

    select->showPopup();
    QWidget *list = select->popup();
    QTest::keyClick(list, Qt::Key_Down);
    QTest::keyClick(list, Qt::Key_Home);
    QTest::keyClick(list, Qt::Key_End);
    QTest::keyClick(list, Qt::Key_Enter);
    QCOMPARE(select->currentIndex(), 3);
    QVERIFY(!select->popup()->isVisible());

    select->showPopup();
    QTest::keyClick(select->popup(), Qt::Key_Escape);
    QVERIFY(!select->popup()->isVisible());
    QCOMPARE(select->currentIndex(), 3);
    window.close();
}

/*! The row in force wears the mark at its left; an unchosen row keeps that
 *  zone empty. */
void tst_MaterialSelect::theChosenRowWearsTheMarkAtItsLeft()
{
    QWidget window;
    window.resize(400, 600);
    MaterialSelect *select = selectWith(&window, 4);
    select->setCurrentIndex(1);
    select->move(50, 100);
    window.show();
    QTest::qWait(50);

    select->showPopup();
    const QImage frame = select->popup()->grab().toImage();
    const QColor page = select->popup()->palette().color(QPalette::Window);
    // The chosen row (index 1) has the mark in its left zone; row 2 keeps
    // the same zone empty.
    const int markX = select->popup()->contentsRect().left() + 14;
    const int top = select->popup()->contentsRect().top();
    QVERIFY(frame.pixelColor(markX, top + 48 + 24) != page);
    QCOMPARE(frame.pixelColor(markX, top + 2 * 48 + 24), page);
    select->hidePopup();
    window.close();
}

/*! The list is a column of rows of forty-eight, and nothing else. */
void tst_MaterialSelect::aRowIsFortyEightTall()
{
    QWidget window;
    window.resize(400, 600);
    MaterialSelect *select = selectWith(&window, 4);
    select->move(50, 100);
    window.show();
    QTest::qWait(50);

    select->showPopup();
    QCOMPARE(select->popup()->contentsRect().height(), 4 * 48);
    select->hidePopup();
    window.close();
}

/*! The same choice said twice is silence: the index signal speaks when the
 *  choice moved, and only then. */
void tst_MaterialSelect::theSameChoiceSaidTwiceIsSilence()
{
    MaterialSelect select;
    select.addItem(QStringLiteral("One"));
    select.addItem(QStringLiteral("Two"));
    QSignalSpy changed(&select, &MaterialSelect::currentIndexChanged);

    select.setCurrentIndex(1);
    QCOMPARE(changed.count(), 1);
    select.setCurrentIndex(1);
    QCOMPARE(changed.count(), 1);
    select.setCurrentIndex(9);
    QCOMPARE(changed.count(), 1);
    QCOMPARE(select.currentIndex(), 1);
}

/*! The list answers a click that started on the list.
 *
 *  A click that opens the list can end with the pointer over a row, because
 *  the list does not always stand clear of the field: in a window too small
 *  for it either side, or near the bottom of the screen, it is laid over the
 *  field itself and the pointer comes up inside it. Reading that release as an
 *  answer picks a choice nobody made, on a click that only meant to open the
 *  list - the list shows on the press and is gone by the release.
 */
void tst_MaterialSelect::theReleaseThatEndsTheOpeningClickIsNoAnswer()
{
    QWidget window;
    window.resize(400, 400);
    MaterialSelect *select = selectWith(&window, 4);
    select->move(50, 100);
    window.show();
    QTest::qWait(50);
    select->setCurrentIndex(2);

    select->showPopup();
    QVERIFY(select->popup()->isVisible());

    const int firstRow = int(select->popup()->contentsRect().top() + 48 / 2.0);
    QTest::mouseRelease(select->popup(), Qt::LeftButton, Qt::NoModifier,
                        QPoint(select->popup()->width() / 2, firstRow));

    QVERIFY(select->popup()->isVisible());
    QCOMPARE(select->currentIndex(), 2);
    window.close();
}

TEST_ADD(tst_MaterialSelect)

#include "tst_materialselect.moc"
