/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#include "tst_global.h"

#include "qxfluent/fluentmenuflyout.h"

#include <QKeyEvent>
#include <QSignalSpy>
#include <QWidget>

QX_FLUENT_USE_NAMESPACE

namespace
{
/*! A popup surface paints over the page it stands on; the row a command
 *  occupies is found from the surface's own room. */
QRectF contentOf(FluentMenuFlyout *flyout)
{
    return flyout->contentRect();
}

int inkRows(const QImage &image, const QColor &page)
{
    int rows = 0;
    for (int y = 0; y < image.height(); ++y) {
        for (int x = 0; x < image.width(); ++x) {
            if (image.pixelColor(x, y) != page) {
                ++rows;
                break;
            }
        }
    }
    return rows;
}
}   // namespace

class tst_FluentMenuFlyout : public QObject
{
    Q_OBJECT
private slots:
    void theKeyboardLandsOnIt();
    void init();
    void cleanup();

    void openIsDrawnOverThePage();
    void hoverWashesItsRow();
    void aCommandInForceWearsTheCheck();
    void theKeyboardWalksTheCommands();
    void escapePutsItAway();
private:
    QWidget *m_anchor = nullptr;
};

void tst_FluentMenuFlyout::init()
{
    m_anchor = new QWidget;
    m_anchor->resize(120, 32);
    m_anchor->show();
}

void tst_FluentMenuFlyout::cleanup()
{
    delete m_anchor;
    m_anchor = nullptr;
}

/*! Opened, the flyout stands over the page: the surface is drawn and its
 *  commands are written on it. */
void tst_FluentMenuFlyout::openIsDrawnOverThePage()
{
    FluentMenuFlyout flyout(m_anchor);
    flyout.addAction(QStringLiteral("Open"));
    flyout.addAction(QStringLiteral("Save a copy"));
    flyout.open();

    QVERIFY(flyout.isVisible());
    // The flyout stands below its anchor, in the room of its surface.
    QVERIFY(flyout.geometry().top() >= m_anchor->geometry().bottom());
    // And the surface carries ink the page did not have.
    QVERIFY(inkRows(flyout.grab().toImage(), flyout.palette().color(QPalette::Window)) > 1);
}

/*! The row the pointer is on is washed; the rows it is not on are not. */
void tst_FluentMenuFlyout::hoverWashesItsRow()
{
    FluentMenuFlyout flyout(m_anchor);
    QAction *first = flyout.addAction(QStringLiteral("Open"));
    QAction *second = flyout.addAction(QStringLiteral("Save a copy"));
    flyout.open();

    flyout.setCurrentAction(first);
    const QImage onFirst = flyout.grab().toImage();

    flyout.setCurrentAction(second);
    const QImage onSecond = flyout.grab().toImage();

    QVERIFY(onFirst != onSecond);
    // And a hover that lands on the hairline between them is nobody's.
    flyout.setCurrentAction(nullptr);
    QCOMPARE(flyout.currentAction(), static_cast<QAction *>(nullptr));
    const QImage onNone = flyout.grab().toImage();
    QVERIFY(onNone != onFirst);
    QVERIFY(onNone != onSecond);
}

/*! A command that keeps a state wears the check when it is in force, and
 *  nothing when it is not. */
void tst_FluentMenuFlyout::aCommandInForceWearsTheCheck()
{
    FluentMenuFlyout flyout(m_anchor);
    QAction *plain = flyout.addAction(QStringLiteral("Plain"));
    QAction *checked = flyout.addCheckable(QStringLiteral("Word wrap"), true);
    flyout.addCheckable(QStringLiteral("Line numbers"), false);
    flyout.open();

    const int glyphLeft = int(contentOf(&flyout).left()) + 12;
    const int zoneWidth = 24;
    const int checkedRow = flyout.actions().indexOf(checked);
    const int rowY = int(contentOf(&flyout).top()) + 4 + checkedRow * 32 + 16;

    const QImage image = flyout.grab().toImage();
    int checkInk = 0;
    for (int x = glyphLeft; x < glyphLeft + zoneWidth; ++x) {
        for (int y = rowY - 8; y <= rowY + 8; ++y) {
            if (image.pixelColor(x, y).alpha() > 0 &&
                image.pixelColor(x, y) != flyout.palette().color(QPalette::Window) &&
                image.pixelColor(x, y).value() < 200) {
                ++checkInk;
            }
        }
    }
    QVERIFY(checkInk > 0);

    // The command beside it, without a state, wears nothing there.
    const int plainRowY = rowY - 32;
    int plainInk = 0;
    for (int x = glyphLeft; x < glyphLeft + zoneWidth; ++x) {
        for (int y = plainRowY - 8; y <= plainRowY + 8; ++y) {
            const QColor color = image.pixelColor(x, y);
            if (color.alpha() > 0 && color != flyout.palette().color(QPalette::Window) && color.value() < 200) {
                ++plainInk;
            }
        }
    }
    QCOMPARE(plainInk, 0);
    QVERIFY(plain->isEnabled());
}

/*! The keyboard walks the commands that can be worked, Enter works the one
 *  in hand - and the flyout goes away with it. */
void tst_FluentMenuFlyout::theKeyboardWalksTheCommands()
{
    FluentMenuFlyout flyout(m_anchor);
    QAction *first = flyout.addAction(QStringLiteral("Open"));
    flyout.addSeparator();
    QAction *second = flyout.addAction(QStringLiteral("Save a copy"));
    flyout.open();
    QCOMPARE(flyout.currentAction(), first);

    QSignalSpy triggeredSpy(&flyout, &FluentMenuFlyout::triggered);
    QSignalSpy actionSpy(second, &QAction::triggered);

    QTest::keyClick(&flyout, Qt::Key_Down);
    QCOMPARE(flyout.currentAction(), second);
    QTest::keyClick(&flyout, Qt::Key_Up);
    QCOMPARE(flyout.currentAction(), first);
    QTest::keyClick(&flyout, Qt::Key_Down);
    QCOMPARE(flyout.currentAction(), second);

    QTest::keyClick(&flyout, Qt::Key_Return);
    QCOMPARE(triggeredSpy.count(), 1);
    QCOMPARE(actionSpy.count(), 1);
    QVERIFY(!flyout.isVisible());
}

/*! Escape is the hand that puts it away without working anything. */
void tst_FluentMenuFlyout::escapePutsItAway()
{
    FluentMenuFlyout flyout(m_anchor);
    QAction *first = flyout.addAction(QStringLiteral("Open"));
    flyout.open();
    QVERIFY(flyout.isVisible());

    QSignalSpy dismissed(&flyout, &QxControls::QxPopupSurface::dismissed);
    QSignalSpy triggeredSpy(&flyout, &FluentMenuFlyout::triggered);

    QTest::keyClick(&flyout, Qt::Key_Escape);
    QVERIFY(!flyout.isVisible());
    QCOMPARE(dismissed.count(), 1);
    QCOMPARE(triggeredSpy.count(), 0);
    QCOMPARE(flyout.currentAction(), first);
}

void tst_FluentMenuFlyout::theKeyboardLandsOnIt()
{
    FluentMenuFlyout widget(m_anchor);
    widget.show();
    widget.open();
    QApplication::processEvents();

    widget.setFocus();
    QApplication::processEvents();
    QWidget *focus = QApplication::focusWidget();
    QVERIFY(focus == &widget || widget.isAncestorOf(focus));
}

TEST_ADD(tst_FluentMenuFlyout)

#include "tst_fluentmenuflyout.moc"
