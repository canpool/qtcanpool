/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#include "tst_global.h"

#include "qxfluent/fluentdropdownbutton.h"

#include "qxtheme/qxthememanager.h"

#include <QAccessible>
#include <QApplication>
#include <QImage>
#include <QMenu>
#include <QSignalSpy>

QX_FLUENT_USE_NAMESPACE
QX_THEME_USE_NAMESPACE

namespace
{
// A button given a size of its own: the tests click places on it rather than
// ask a layout where those places are.
constexpr int kWidth = 160;
constexpr int kHeight = 32;

void giveSize(FluentDropDownButton *button)
{
    button->resize(kWidth, kHeight);
}

QColor pixel(QWidget *widget, int x, int y)
{
    const QImage image = widget->grab().toImage();
    const qreal dpr = image.width() / qreal(widget->width());
    return image.pixelColor(qRound(x * dpr), qRound(y * dpr));
}

bool isAccent(const QColor &color)
{
    return color.red() < 80 && color.blue() > 200;
}
}   // namespace

class tst_FluentDropDownButton : public QObject
{
    Q_OBJECT
private slots:
    void cleanup();

    void theWholeFaceOpensTheMenu();
    void theKeyboardOpensItToo();
    void aMenuWithNothingInItIsNotOpened();
    void theMenuIsTheButtonsOwn();
    void theTextAndTheIconWidenTheHint();
    void theTextIsTheAccessibleName();
    void whatItIs();
    void aChangeOfTextIsAnnounced();
    void theRingFollowsTheCorners();
    void aButtonNobodyCanUseOffersNothing();
private:
    bool m_wasActive = false;
};

void tst_FluentDropDownButton::cleanup()
{
    // The accessible registry is global.
    QAccessible::setActive(m_wasActive);
    m_wasActive = false;
}

/*! The face is one thing: there is no half of it that means something else,
 *  so a press anywhere on it is the menu - including over the chevron, which
 *  is where a reader of the control looks. */
void tst_FluentDropDownButton::theWholeFaceOpensTheMenu()
{
    FluentDropDownButton button;
    giveSize(&button);
    button.setText(QStringLiteral("Export"));
    button.menu()->addAction(QStringLiteral("As PDF"));

    QSignalSpy about(&button, &FluentDropDownButton::aboutToShowMenu);

    // Well clear of the chevron, which is the last twenty pixels.
    QTest::mouseClick(&button, Qt::LeftButton, Qt::NoModifier, QPoint(12, kHeight / 2));
    QCOMPARE(about.count(), 1);
    QVERIFY(button.menu()->isVisible());
    button.menu()->hide();

    QTest::mouseClick(&button, Qt::LeftButton, Qt::NoModifier, QPoint(kWidth - 8, kHeight / 2));
    QCOMPARE(about.count(), 2);
    QVERIFY(button.menu()->isVisible());
    button.menu()->hide();
}

/*! Every key that opens a menu on the platforms this runs on. */
void tst_FluentDropDownButton::theKeyboardOpensItToo()
{
    FluentDropDownButton button;
    giveSize(&button);
    button.setText(QStringLiteral("Export"));
    button.menu()->addAction(QStringLiteral("As PDF"));

    QSignalSpy about(&button, &FluentDropDownButton::aboutToShowMenu);

    const int keys[] = {Qt::Key_Space, Qt::Key_Return, Qt::Key_Enter, Qt::Key_Down, Qt::Key_F4};
    int expected = 0;
    for (int key : keys) {
        QTest::keyClick(&button, Qt::Key(key));
        ++expected;
        QCOMPARE(about.count(), expected);
        button.menu()->hide();
    }
}

/*! A menu with nothing in it is not a menu: the caller is given its chance to
 *  fill it, and if it is still empty the button keeps the promise unopened
 *  rather than opening an empty box. */
void tst_FluentDropDownButton::aMenuWithNothingInItIsNotOpened()
{
    FluentDropDownButton button;
    giveSize(&button);

    QSignalSpy about(&button, &FluentDropDownButton::aboutToShowMenu);
    QVERIFY(button.menu()->isEmpty());

    button.showMenu();
    QCOMPARE(about.count(), 1);
    QVERIFY(!button.menu()->isVisible());
}

/*! The menu is the button's child: an application fills it and does not own
 *  it, which is the one ownership question this class answers. */
void tst_FluentDropDownButton::theMenuIsTheButtonsOwn()
{
    FluentDropDownButton button;
    QVERIFY(button.menu());
    QCOMPARE(button.menu()->parent(), static_cast<QObject *>(&button));

    // The same menu every time: a second call is not a second menu.
    QCOMPARE(button.menu(), button.menu());
}

/*! The button grows with what it has to say, and its floor does not: a command
 *  can lose its text to an elision, but not its chevron. */
void tst_FluentDropDownButton::theTextAndTheIconWidenTheHint()
{
    FluentDropDownButton button;
    const QSize bare = button.sizeHint();
    QCOMPARE(bare.height(), kHeight);

    button.setText(QStringLiteral("Export"));
    const QSize labelled = button.sizeHint();
    QVERIFY(labelled.width() > bare.width());
    QCOMPARE(button.minimumSizeHint().width(), bare.width());

    QPixmap pixmap(16, 16);
    pixmap.fill(Qt::black);
    button.setIcon(QIcon(pixmap));
    QVERIFY(button.sizeHint().width() > labelled.width());
    QVERIFY(button.minimumSizeHint().width() > bare.width());
}

/*! The text is the accessible name for as long as the application has not
 *  given a better one. */
void tst_FluentDropDownButton::theTextIsTheAccessibleName()
{
    FluentDropDownButton button;
    button.setText(QStringLiteral("Export"));
    QCOMPARE(button.accessibleName(), QStringLiteral("Export"));

    button.setAccessibleName(QStringLiteral("Export the report"));
    button.setText(QStringLiteral("Export report"));
    QCOMPARE(button.accessibleName(), QStringLiteral("Export the report"));
}

/*! A button, and it says so rather than being a Client with a rectangle. */
void tst_FluentDropDownButton::whatItIs()
{
    m_wasActive = QAccessible::isActive();
    QAccessible::setActive(true);

    FluentDropDownButton button;
    QAccessibleInterface *iface = QAccessible::queryAccessibleInterface(&button);
    QVERIFY(iface);
    QCOMPARE(iface->role(), QAccessible::Button);
    QAccessible::deleteAccessibleInterface(QAccessible::uniqueId(iface));
}

void tst_FluentDropDownButton::aChangeOfTextIsAnnounced()
{
    FluentDropDownButton button;
    QSignalSpy changed(&button, &FluentDropDownButton::textChanged);

    button.setText(QStringLiteral("Export"));
    QCOMPARE(changed.count(), 1);
    QCOMPARE(button.text(), QStringLiteral("Export"));

    // The same text twice is one change.
    button.setText(QStringLiteral("Export"));
    QCOMPARE(changed.count(), 1);
}

/*! The ring and the wash follow the button's corners rather than its
 *  rectangle: a Fluent command is a rounded thing, and a square ring at a round
 *  corner is a ring that misses the control. */
void tst_FluentDropDownButton::theRingFollowsTheCorners()
{
    QxTheme::QxThemeManager::instance()->setAccentColor(QColor(0, 0, 255));

    FluentDropDownButton button;
    giveSize(&button);
    button.setText(QStringLiteral("Export"));
    button.show();
    QVERIFY(QTest::qWaitForWindowExposed(&button));
    button.setFocus();
    QVERIFY(button.hasFocus());

    // The middle of an edge is the ring; the corner is the page, because the
    // ring turned the corner instead of crossing it.
    QVERIFY(isAccent(pixel(&button, kWidth / 2, 0)));
    QVERIFY(!isAccent(pixel(&button, 0, 0)));

    QxTheme::QxThemeManager::instance()->setAccentColor(QColor());
}

/*! A button nobody can use offers nothing: the face is the question the menu
 *  answers, and a control that cannot take the answer stays shut. */
void tst_FluentDropDownButton::aButtonNobodyCanUseOffersNothing()
{
    FluentDropDownButton button;
    giveSize(&button);
    button.setText(QStringLiteral("Sort"));
    button.menu()->addAction(QStringLiteral("By name"));
    const QImage live = button.grab().toImage();

    QSignalSpy showing(&button, &FluentDropDownButton::aboutToShowMenu);
    // Qt fades a disabled widget through its palette; pinned, the only thing
    // left that can change is what the control itself paints.
    qtcPinDisabledColours(&button);

    button.setEnabled(false);
    QTest::mouseClick(&button, Qt::LeftButton, Qt::NoModifier, QPoint(12, kHeight / 2));

    QCOMPARE(showing.count(), 0);
    QVERIFY(!button.menu()->isVisible());
    QVERIFY(button.grab().toImage() != live);
}

TEST_ADD(tst_FluentDropDownButton)

#include "tst_fluentdropdownbutton.moc"
