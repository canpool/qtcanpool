/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#include "tst_global.h"

#include "qxcupertino/cupertinotextfield.h"

#include <QtGui/QImage>
#include <QtWidgets/QAbstractButton>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QWidget>

QX_CUPERTINO_USE_NAMESPACE

namespace
{
/*! The ground a frame is rendered onto, so that "is there ink here" is a
 *  question about the field and not about the page behind it. */
const QColor kGround(255, 255, 255);

QImage frameOf(QWidget *widget)
{
    QImage image(widget->size(), QImage::Format_ARGB32_Premultiplied);
    image.fill(kGround);
    widget->render(&image);
    return image;
}

/*! The one line edit the field puts its text in. */
QLineEdit *lineEditOf(CupertinoTextField *field)
{
    auto *edit = field->findChild<QLineEdit *>();
    Q_ASSERT(edit);
    return edit;
}

/*! The one button the field owns - the clear button. */
QAbstractButton *clearButtonOf(CupertinoTextField *field)
{
    const QList<QAbstractButton *> buttons = field->findChildren<QAbstractButton *>();
    Q_ASSERT(buttons.size() == 1);
    return buttons.first();
}
}   // namespace

class tst_CupertinoTextField : public QObject
{
    Q_OBJECT
private slots:
    void theFieldAsksForOneLineOfBody();
    void theClearButtonIsOnlyThereWhenThereIsSomethingToClear();
    void aFieldBuiltOnAHiddenPageLaysItsTextOutTheSameWay();
    void theClearButtonEmptiesTheField();
    void escapeEmptiesTheFieldAndReturnDoesNot();
    void aSearchFieldDrawsItsMagnifierAtTheLeadingEdge();
};

/*! The field is one line of the family's body text plus the room a filled
 *  field keeps around it, and a layout cannot squash it below that. */
void tst_CupertinoTextField::theFieldAsksForOneLineOfBody()
{
    CupertinoTextField field;
    QCOMPARE(field.sizeHint().height(), 36);
    QCOMPARE(field.minimumSizeHint().height(), field.sizeHint().height());
    QVERIFY(field.sizeHint().width() > field.minimumSizeHint().width());
}

/*! A clear button that is there while there is nothing to clear is a mark that
 *  can only do nothing, so it is hidden until there is text. */
void tst_CupertinoTextField::theClearButtonIsOnlyThereWhenThereIsSomethingToClear()
{
    CupertinoTextField field;
    QVERIFY(field.isClearButtonEnabled());
    QAbstractButton *clear = clearButtonOf(&field);
    QVERIFY(clear->isHidden());

    field.setText(QStringLiteral("Cupertino"));
    QVERIFY(!clear->isHidden());

    field.clear();
    QVERIFY(clear->isHidden());

    // And an application that does not want one does not get one.
    field.setClearButtonEnabled(false);
    field.setText(QStringLiteral("Cupertino"));
    QVERIFY(clear->isHidden());
}

/*! The room the clear button takes is decided by whether the field wants it
 *  there, not by whether the page it sits on happens to be up yet: a field
 *  built on a page that is not current is built while everything in it reports
 *  itself invisible, and a layout that asked that question laid the text over
 *  the button's room and never took it back. */
void tst_CupertinoTextField::aFieldBuiltOnAHiddenPageLaysItsTextOutTheSameWay()
{
    CupertinoTextField shownFromTheStart;
    shownFromTheStart.resize(220, 36);
    shownFromTheStart.show();
    QCoreApplication::processEvents();
    shownFromTheStart.setText(QStringLiteral("Cupertino"));
    QCoreApplication::processEvents();

    QWidget page;
    auto *builtHidden = new CupertinoTextField(&page);
    builtHidden->resize(220, 36);
    builtHidden->setText(QStringLiteral("Cupertino"));
    page.show();
    QCoreApplication::processEvents();

    // The button is up in both - the field with the text is entitled to it, and
    // the question is only whether the text got out of its way.
    QVERIFY(!clearButtonOf(&shownFromTheStart)->isHidden());
    QVERIFY(!clearButtonOf(builtHidden)->isHidden());
    QCOMPARE(lineEditOf(builtHidden)->geometry(), lineEditOf(&shownFromTheStart)->geometry());
}

/*! Clearing empties the field and says so once: the text change and the clear
 *  are one event seen two ways, not two changes. */
void tst_CupertinoTextField::theClearButtonEmptiesTheField()
{
    CupertinoTextField field;
    field.resize(field.sizeHint());
    field.show();
    field.setText(QStringLiteral("Cupertino"));

    QSignalSpy changed(&field, &CupertinoTextField::textChanged);
    QSignalSpy cleared(&field, &CupertinoTextField::cleared);
    QTest::mouseClick(clearButtonOf(&field), Qt::LeftButton);

    QCOMPARE(field.text(), QString());
    QCOMPARE(changed.count(), 1);
    QCOMPARE(changed.first().first().toString(), QString());
    QCOMPARE(cleared.count(), 1);
}

/*! Escape is the keyboard's way of emptying the field and Return is the
 *  application's way of reading it: the first clears, the second does not. */
void tst_CupertinoTextField::escapeEmptiesTheFieldAndReturnDoesNot()
{
    CupertinoTextField field;
    field.resize(field.sizeHint());
    field.show();
    QCoreApplication::processEvents();
    QLineEdit *edit = lineEditOf(&field);
    edit->setFocus();
    QCoreApplication::processEvents();
    QVERIFY(edit->hasFocus());

    field.setText(QStringLiteral("Cupertino"));
    QSignalSpy returned(&field, &CupertinoTextField::returnPressed);
    QTest::keyClick(edit, Qt::Key_Return);
    QCOMPARE(returned.count(), 1);
    QCOMPARE(field.text(), QStringLiteral("Cupertino"));

    QTest::keyClick(edit, Qt::Key_Escape);
    QCOMPARE(field.text(), QString());
}

/*! A search field is a text field whose leading decoration is part of it: the
 *  magnifier sits where a plain field starts its text, and a plain field has
 *  nothing there but its own fill. */
void tst_CupertinoTextField::aSearchFieldDrawsItsMagnifierAtTheLeadingEdge()
{
    const QRect leading(8, 12, 3, 12);

    CupertinoTextField plain;
    plain.resize(plain.sizeHint());
    const QImage plainFrame = frameOf(&plain);
    // The strip is the fill and only the fill: one colour across it.
    const QColor fill = plainFrame.pixelColor(leading.left(), leading.top());
    for (int y = leading.top(); y <= leading.bottom(); ++y) {
        for (int x = leading.left(); x <= leading.right(); ++x) {
            QCOMPARE(plainFrame.pixelColor(x, y), fill);
        }
    }

    CupertinoTextField search;
    search.resize(search.sizeHint());
    search.setKind(CupertinoTextField::Kind::Search);
    const QImage searchFrame = frameOf(&search);

    // And with the kind set, the same strip carries the lens.
    int different = 0;
    for (int y = leading.top(); y <= leading.bottom(); ++y) {
        for (int x = leading.left(); x <= leading.right(); ++x) {
            if (plainFrame.pixelColor(x, y) != searchFrame.pixelColor(x, y)) {
                ++different;
            }
        }
    }
    QVERIFY2(different >= 12, qPrintable(QStringLiteral("the strip differs in %1 pixels").arg(different)));
}

TEST_ADD(tst_CupertinoTextField)

#include "tst_cupertinotextfield.moc"
