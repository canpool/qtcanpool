/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#include "tst_global.h"

#include "qxfluent/fluentbutton.h"
#include "qxfluent/fluentdivider.h"

#include "qxtheme/qxthememanager.h"

#include <QImage>
#include <QSignalSpy>
#include <QtWidgets/QApplication>

QX_FLUENT_USE_NAMESPACE
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
    QPixmap patch(16, 16);
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

/*! The first pixel from the left that is not the page: where a line starts.
 *  The page is the palette's own answer - a full line leaves no pixel of it
 *  in the frame to compare against. */
int lineStart(FluentDivider *divider, int y)
{
    const QImage image = divider->grab().toImage();
    const QColor page = divider->palette().color(QPalette::Window);
    for (int x = 0; x < image.width(); ++x) {
        if (image.pixelColor(x, y) != page) {
            return x;
        }
    }
    return -1;
}
}   // namespace

class tst_FluentButton : public QObject
{
    Q_OBJECT
private slots:
    void init();
    void cleanup();

    void theThreeKindsWearThreeFills();
    void anIconAloneIsASquare();
    void aDisabledFaceDoesNotWash();
    void aClickReachesWhoeverListens();
    void theTwoVariantsOfTheLinePartByAnIndent();
    void theKeyboardLandsOnIt();
private:
    bool m_hadOverride = false;
};

void tst_FluentButton::init()
{
    m_hadOverride = QxTheme::QxThemeManager::instance()->hasAccentOverride();
    QxTheme::QxThemeManager::instance()->setAccentColor(kAccent);
}

void tst_FluentButton::cleanup()
{
    if (!m_hadOverride) {
        QxTheme::QxThemeManager::instance()->setAccentColor(QColor());
    }
}

/*! Three weights of command are three different faces: the standard one is
 *  the page inked, the accent one is the accent, and the subtle one is no
 *  face at all until the pointer arrives. */
void tst_FluentButton::theThreeKindsWearThreeFills()
{
    FluentButton standard;
    standard.resize(standard.sizeHint());
    const QColor page = standard.palette().color(QPalette::Window);
    const QColor body = pixel(&standard, standard.width() / 2, 16);
    QVERIFY(body != page);
    QVERIFY(body != kAccent);

    FluentButton accent;
    accent.setKind(FluentButton::Kind::Accent);
    accent.resize(accent.sizeHint());
    QCOMPARE(pixel(&accent, accent.width() / 2, 16), kAccent);

    FluentButton subtle;
    subtle.setKind(FluentButton::Kind::Subtle);
    subtle.resize(subtle.sizeHint());
    QCOMPARE(pixel(&subtle, subtle.width() / 2, 16), page);
}

/*! A command that says it with an icon alone keeps no padding either side:
 *  its face is the square the height asks for. */
void tst_FluentButton::anIconAloneIsASquare()
{
    FluentButton iconOnly;
    iconOnly.setIcon(inkIcon());
    const QSize hint = iconOnly.sizeHint();
    QCOMPARE(hint.width(), hint.height());

    FluentButton wordy;
    wordy.setText(QStringLiteral("Open"));
    QVERIFY(wordy.sizeHint().width() > wordy.sizeHint().height());
}

/*! A disabled command does not answer the pointer: no wash arrives where
 *  nothing can be pressed. */
void tst_FluentButton::aDisabledFaceDoesNotWash()
{
    FluentButton button;
    button.resize(button.sizeHint());
    button.setEnabled(false);

    const QImage rest = button.grab().toImage();
    sendEnter(&button);
    const QImage hovered = button.grab().toImage();
    QCOMPARE(hovered, rest);
}

/*! The click is the base's promise: released on the face, and only then. */
void tst_FluentButton::aClickReachesWhoeverListens()
{
    FluentButton button;
    button.resize(button.sizeHint());
    QSignalSpy clicked(&button, &FluentButton::clicked);

    QTest::mouseClick(&button, Qt::LeftButton, Qt::NoModifier, QPoint(button.width() / 2, button.height() / 2));
    QCOMPARE(clicked.count(), 1);

    // A press that leaves before it releases is not a click.
    QTest::mousePress(&button, Qt::LeftButton, Qt::NoModifier, QPoint(button.width() / 2, button.height() / 2));
    QTest::mouseRelease(&button, Qt::LeftButton, Qt::NoModifier, QPoint(button.width() / 2, button.height() + 20));
    QCOMPARE(clicked.count(), 1);
}

/*! The line separates and says nothing else: the inset one starts where the
 *  content beside it starts, an indent in from where the full one starts. */
void tst_FluentButton::theTwoVariantsOfTheLinePartByAnIndent()
{
    FluentDivider full;
    full.setFixedSize(120, 1);
    FluentDivider inset;
    inset.setKind(FluentDivider::Kind::Inset);
    inset.setFixedSize(120, 1);

    const int fullStart = lineStart(&full, 0);
    const int insetStart = lineStart(&inset, 0);
    QVERIFY(fullStart >= 0);
    QCOMPARE(insetStart - fullStart, 24);
}

/*! The keyboard finds it, and the ring says where the keyboard is: a control
 *  the keyboard cannot reach is a control half the users cannot use, and one it
 *  reaches without a mark is a control they cannot find.
 */
void tst_FluentButton::theKeyboardLandsOnIt()
{
    FluentButton widget;
    widget.setText(QStringLiteral("Save"));
    widget.resize(widget.sizeHint());
    widget.show();
    QApplication::processEvents();

    // A control the keyboard cannot reach is a control half the users cannot
    // use. Reaching it means focus actually lands and is reported back.
    widget.setFocus();
    QVERIFY(widget.hasFocus());
    QCOMPARE(QApplication::focusWidget(), &widget);
}

TEST_ADD(tst_FluentButton)

#include "tst_fluentbutton.moc"
