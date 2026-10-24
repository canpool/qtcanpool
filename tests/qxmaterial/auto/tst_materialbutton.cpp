/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#include "tst_global.h"

#include "qxmaterial/materialbutton.h"

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

QColor pixel(QWidget *widget, int x, int y)
{
    return widget->grab().toImage().pixelColor(x, y);
}

QIcon inkIcon()
{
    QPixmap patch(20, 20);
    patch.fill(QColor(20, 20, 20));
    return QIcon(patch);
}

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
void sendEnter(QWidget *widget)
{
    QEnterEvent enter(QPointF(1, 1), QPointF(1, 1), QPointF(1, 1));
    QApplication::sendEvent(widget, &enter);
}
#else
void sendEnter(QWidget *widget)
{
    QEvent enter(QEvent::Enter);
    QApplication::sendEvent(widget, &enter);
}
#endif
}   // namespace

class tst_MaterialButton : public QObject
{
    Q_OBJECT
private slots:

    void theKeyboardLandsOnIt();
    void init();
    void cleanup();

    void theThreeVariantsWearThreeFills();
    void anIconAloneIsASquare();
    void aDisabledFaceAnswersNothing();
    void theRippleTravelsOnTheFacesThatCarryOne();
    void aClickReachesWhoeverListens();
private:
    bool m_hadOverride = false;
};

void tst_MaterialButton::init()
{
    m_hadOverride = QxTheme::QxThemeManager::instance()->hasAccentOverride();
    QxTheme::QxThemeManager::instance()->setAccentColor(kAccent);
}

void tst_MaterialButton::cleanup()
{
    if (!m_hadOverride) {
        QxTheme::QxThemeManager::instance()->setAccentColor(QColor());
    }
}

/*! Three surfaces a command can claim: the filled one is the accent's own
 *  colour, the outlined one is the page under an outline, and the text one
 *  is no surface at all - its words point instead. */
void tst_MaterialButton::theThreeVariantsWearThreeFills()
{
    MaterialButton filled;
    filled.resize(filled.sizeHint());
    QCOMPARE(pixel(&filled, filled.width() / 2, 18), kAccent);

    MaterialButton outlined;
    outlined.setVariant(MaterialButton::Variant::Outlined);
    outlined.resize(outlined.sizeHint());
    const QColor page = outlined.palette().color(QPalette::Window);
    QCOMPARE(pixel(&outlined, outlined.width() / 2, 18), page);
    // The outline is on the edge, a half pixel inside it.
    QVERIFY(pixel(&outlined, 0, 18) != page);

    MaterialButton text;
    text.setVariant(MaterialButton::Variant::Text);
    text.resize(text.sizeHint());
    QCOMPARE(pixel(&text, text.width() / 2, 18), page);
    QVERIFY(pixel(&text, 0, 18) == page);
}

/*! A command that says it with an icon alone keeps no padding either side:
 *  its face is the square the height asks for. */
void tst_MaterialButton::anIconAloneIsASquare()
{
    MaterialButton iconOnly;
    iconOnly.setIcon(inkIcon());
    const QSize hint = iconOnly.sizeHint();
    QCOMPARE(hint.width(), hint.height());

    MaterialButton wordy;
    wordy.setText(QStringLiteral("Open"));
    QVERIFY(wordy.sizeHint().width() > wordy.sizeHint().height());
}

/*! A disabled command does not answer the pointer: no wash and no ripple
 *  arrives where nothing can be pressed. */
void tst_MaterialButton::aDisabledFaceAnswersNothing()
{
    MaterialButton button;
    button.resize(button.sizeHint());
    button.setEnabled(false);

    const QImage rest = button.grab().toImage();
    sendEnter(&button);
    QTest::mousePress(&button, Qt::LeftButton, Qt::NoModifier, QPoint(button.width() / 2, button.height() / 2));
    QTest::qWait(60);
    QCOMPARE(button.grab().toImage(), rest);
}

/*! The ripple is the press's answer on the faces that carry one. The wash
 *  under a held press is flat and still, so what moves between the first
 *  frame and a later one is the ripple and only the ripple: the filled and
 *  the outlined faces carry it, the text face does not. */
void tst_MaterialButton::theRippleTravelsOnTheFacesThatCarryOne()
{
    const QPoint origin(12, 18);

    MaterialButton filled;
    filled.resize(filled.sizeHint());
    QTest::mousePress(&filled, Qt::LeftButton, Qt::NoModifier, origin);
    const QImage filledEarly = filled.grab().toImage();
    QTest::qWait(60);
    QVERIFY(filled.grab().toImage() != filledEarly);
    QTest::mouseRelease(&filled, Qt::LeftButton, Qt::NoModifier, origin);
    QTest::qWait(400);

    MaterialButton text;
    text.setVariant(MaterialButton::Variant::Text);
    text.resize(text.sizeHint());
    QTest::mousePress(&text, Qt::LeftButton, Qt::NoModifier, origin);
    const QImage textEarly = text.grab().toImage();
    QTest::qWait(60);
    QCOMPARE(text.grab().toImage(), textEarly);
    QTest::mouseRelease(&text, Qt::LeftButton, Qt::NoModifier, origin);
}

/*! The click is the base's promise: released on the face, and only then -
 *  and the keyboard answers with the same signal. */
void tst_MaterialButton::aClickReachesWhoeverListens()
{
    MaterialButton button;
    button.resize(button.sizeHint());
    QSignalSpy clicked(&button, &MaterialButton::clicked);

    QTest::mouseClick(&button, Qt::LeftButton, Qt::NoModifier, QPoint(button.width() / 2, button.height() / 2));
    QCOMPARE(clicked.count(), 1);

    // A press that leaves before it releases is not a click.
    QTest::mousePress(&button, Qt::LeftButton, Qt::NoModifier, QPoint(button.width() / 2, button.height() / 2));
    QTest::mouseRelease(&button, Qt::LeftButton, Qt::NoModifier, QPoint(button.width() / 2, button.height() + 20));
    QCOMPARE(clicked.count(), 1);

    QTest::keyClick(&button, Qt::Key_Space);
    QCOMPARE(clicked.count(), 2);
}

void tst_MaterialButton::theKeyboardLandsOnIt()
{
    MaterialButton widget;
    widget.show();
    QApplication::processEvents();

    widget.setFocus();
    QVERIFY(widget.hasFocus());
    QCOMPARE(QApplication::focusWidget(), &widget);
}

TEST_ADD(tst_MaterialButton)

#include "tst_materialbutton.moc"
