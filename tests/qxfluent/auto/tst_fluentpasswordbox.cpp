/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#include "tst_global.h"

#include "qxfluent/fluentpasswordbox.h"
#include "qxfluent/fluenttextbox.h"

#include <QLineEdit>

QX_FLUENT_USE_NAMESPACE

namespace
{
QImage grabOf(QWidget *widget)
{
    return widget->grab().toImage();
}

QPoint revealAt(QWidget *box)
{
    return QPoint(box->width() - 20, box->height() / 2);
}

/*! The room where the words stand, left of the room where the box's own
 *  answer does: the trailing glyphs differ by design, the words do not. */
QImage wordsOf(QWidget *box)
{
    return grabOf(box).copy(0, 0, box->width() - 45, box->height());
}
}   // namespace

class tst_FluentPasswordBox : public QObject
{
    Q_OBJECT
private slots:
    void theKeyboardLandsOnIt();
    void init();

    void aSecretIsDrawnAsTheEchoUntilItIsAskedFor();
    void lettingGoHidesItAgain();
    void thePromptGoesWhenTheSecretComes();
    void aDisabledBoxDoesNotReveal();
};

/*! Held, the reveal shows the characters as themselves: the drawing is the
 *  one a plain box with the same words gives. Before that, the words are the
 *  echo, and not the words. */
void tst_FluentPasswordBox::init()
{
    qtcGiveFonts();
}

void tst_FluentPasswordBox::aSecretIsDrawnAsTheEchoUntilItIsAskedFor()
{
    FluentPasswordBox box;
    box.resize(160, 32);
    box.setText(QStringLiteral("secret"));
    box.show();
    box.lineEdit()->clearFocus();

    FluentTextBox plain;
    plain.resize(160, 32);
    plain.setText(QStringLiteral("secret"));
    plain.show();
    plain.lineEdit()->clearFocus();

    // Not the words: the echo is a different drawing.
    grabOf(&box).save("pw_echo.png");
    grabOf(&plain).save("pw_plain.png");
    QVERIFY(wordsOf(&box) != wordsOf(&plain));

    QTest::mousePress(&box, Qt::LeftButton, Qt::NoModifier, revealAt(&box));
    QVERIFY(box.isRevealed());
    QCOMPARE(wordsOf(&box), wordsOf(&plain));

    QTest::mouseRelease(&box, Qt::LeftButton, Qt::NoModifier, revealAt(&box));
    QVERIFY(!box.isRevealed());
}

/*! A secret returns to hiding on its own: letting the reveal go is enough. */
void tst_FluentPasswordBox::lettingGoHidesItAgain()
{
    FluentPasswordBox box;
    box.resize(160, 32);
    box.setText(QStringLiteral("secret"));
    box.show();
    box.lineEdit()->clearFocus();

    const QImage echo = grabOf(&box);
    QTest::mousePress(&box, Qt::LeftButton, Qt::NoModifier, revealAt(&box));
    QVERIFY(grabOf(&box) != echo);
    QTest::mouseRelease(&box, Qt::LeftButton, Qt::NoModifier, QPoint(4, 4));
    QCOMPARE(grabOf(&box), echo);
    QCOMPARE(box.lineEdit()->echoMode(), QLineEdit::Password);
}

/*! The prompt stands where the secret will while there is none, and goes the
 *  moment there is one - echoed or not. */
void tst_FluentPasswordBox::thePromptGoesWhenTheSecretComes()
{
    FluentPasswordBox box;
    box.resize(160, 32);
    box.setPlaceholderText(QStringLiteral("Password"));
    box.show();
    const QImage prompted = grabOf(&box);

    FluentPasswordBox bare;
    bare.resize(160, 32);
    bare.show();
    bare.lineEdit()->clearFocus();
    box.lineEdit()->clearFocus();
    QVERIFY(prompted != grabOf(&bare));

    box.setText(QStringLiteral("secret"));
    bare.setText(QStringLiteral("secret"));
    QCOMPARE(grabOf(&box), grabOf(&bare));
    QVERIFY(grabOf(&box) != prompted);
}

/*! A box that cannot be worked keeps its secret: the eye does not answer. */
void tst_FluentPasswordBox::aDisabledBoxDoesNotReveal()
{
    FluentPasswordBox box;
    box.resize(160, 32);
    box.setText(QStringLiteral("secret"));
    box.show();
    box.setEnabled(false);

    const QImage echo = grabOf(&box);
    QTest::mousePress(&box, Qt::LeftButton, Qt::NoModifier, revealAt(&box));
    QVERIFY(!box.isRevealed());
    QCOMPARE(grabOf(&box), echo);
    QCOMPARE(box.lineEdit()->echoMode(), QLineEdit::Password);
}

void tst_FluentPasswordBox::theKeyboardLandsOnIt()
{
    FluentPasswordBox widget;
    widget.show();
    QApplication::processEvents();

    widget.setFocus();
    QApplication::processEvents();
    QWidget *focus = QApplication::focusWidget();
    QVERIFY(focus == &widget || widget.isAncestorOf(focus));
}

TEST_ADD(tst_FluentPasswordBox)

#include "tst_fluentpasswordbox.moc"
