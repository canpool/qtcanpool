/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#include "tst_global.h"

#include "qxmaterial/materialmenu.h"

#include "qxtheme/qxthememanager.h"

#include <QSignalSpy>
#include <QWidget>

QX_MATERIAL_USE_NAMESPACE
QX_THEME_USE_NAMESPACE

namespace
{
const QColor kAccent(0, 0, 255);
}   // namespace

class tst_MaterialMenu : public QObject
{
    Q_OBJECT
private slots:
    void theKeyboardLandsOnIt();
    void init();
    void cleanup();

    void openIsAPaperBelowItsAnchor();
    void aRowIsFortyEightTall();
    void hoverWashesItsRow();
    void aCommandInForceWearsThePrimaryCheck();
    void theKeyboardWalksTheCommands();
    void aCommandNobodyCanPickIsWrittenFainter();
private:
    QWidget *m_anchor = nullptr;
};

void tst_MaterialMenu::init()
{
    QxTheme::QxThemeManager::instance()->setAccentColor(kAccent);
    m_anchor = new QWidget;
    m_anchor->resize(120, 48);
    m_anchor->show();
}

void tst_MaterialMenu::cleanup()
{
    QxTheme::QxThemeManager::instance()->setAccentColor(QColor());
    delete m_anchor;
    m_anchor = nullptr;
}

/*! Opened, the menu is a paper standing below its anchor, carrying ink the
 *  page did not have. */
void tst_MaterialMenu::openIsAPaperBelowItsAnchor()
{
    MaterialMenu menu(m_anchor);
    menu.addAction(QStringLiteral("Share"));
    menu.addAction(QStringLiteral("Get link"));
    menu.open();

    QVERIFY(menu.isVisible());
    QVERIFY(menu.geometry().top() >= m_anchor->geometry().bottom());
    const QImage image = menu.grab().toImage();
    int inkPixels = 0;
    const QColor page = menu.palette().color(QPalette::Window);
    for (int y = 0; y < image.height(); ++y) {
        for (int x = 0; x < image.width(); ++x) {
            if (image.pixelColor(x, y) != page) {
                ++inkPixels;
            }
        }
    }
    QVERIFY(inkPixels > 0);
}

/*! The rows of a menu are forty-eight tall, whatever they say; a separator
 *  is a hairline's worth. */
void tst_MaterialMenu::aRowIsFortyEightTall()
{
    MaterialMenu menu(m_anchor);
    menu.addAction(QStringLiteral("Share"));
    menu.addSeparator();
    menu.addAction(QStringLiteral("Get link"));
    menu.open();

    // Two rows of forty-eight, a hairline of nine, and the paper's breathing
    // room of eight top and bottom.
    QCOMPARE(menu.contentRect().height(), 48 + 9 + 48 + 16);
}

/*! The row the pointer is on is washed; the rows it is not on are not. */
void tst_MaterialMenu::hoverWashesItsRow()
{
    MaterialMenu menu(m_anchor);
    QAction *first = menu.addAction(QStringLiteral("Share"));
    QAction *second = menu.addAction(QStringLiteral("Get link"));
    menu.open();

    menu.setCurrentAction(first);
    const QImage onFirst = menu.grab().toImage();
    menu.setCurrentAction(second);
    const QImage onSecond = menu.grab().toImage();

    QVERIFY(onFirst != onSecond);
    menu.setCurrentAction(nullptr);
    const QImage onNone = menu.grab().toImage();
    QVERIFY(onNone != onFirst);
    QVERIFY(onNone != onSecond);
}

/*! A command that keeps a state wears the check in the primary when it is
 *  in force, and nothing when it is not. */
void tst_MaterialMenu::aCommandInForceWearsThePrimaryCheck()
{
    MaterialMenu menu(m_anchor);
    menu.addAction(QStringLiteral("Plain"));
    menu.addCheckable(QStringLiteral("Autosave"), true);
    menu.addCheckable(QStringLiteral("Spellcheck"), false);
    menu.open();

    const QImage image = menu.grab().toImage();
    // The leading zone of forty is where the check lives: dark pixels there
    // are the primary's strokes. The row in force has them; the row not in
    // force does not.
    const int zoneLeft = int(menu.contentRect().left()) + 16;
    auto checkInk = [&](int rowIndex) {
        const int rowTop = int(menu.contentRect().top()) + 8 + rowIndex * 48;
        int ink = 0;
        for (int x = zoneLeft; x < zoneLeft + 40; ++x) {
            for (int y = rowTop; y < rowTop + 48; ++y) {
                const QColor color = image.pixelColor(x, y);
                if (color.red() < 80 && color.blue() > 200) {
                    ++ink;
                }
            }
        }
        return ink;
    };
    QVERIFY(checkInk(1) > 0);
    QCOMPARE(checkInk(2), 0);
}

/*! The keyboard walks the commands that can be worked - over the separator,
 *  which is nobody's - and Enter works the one in hand. */
void tst_MaterialMenu::theKeyboardWalksTheCommands()
{
    MaterialMenu menu(m_anchor);
    QAction *first = menu.addAction(QStringLiteral("Share"));
    menu.addSeparator();
    QAction *second = menu.addAction(QStringLiteral("Get link"));
    menu.open();
    QCOMPARE(menu.currentAction(), first);

    QSignalSpy triggeredSpy(&menu, &MaterialMenu::triggered);
    QSignalSpy actionSpy(second, &QAction::triggered);

    QTest::keyClick(&menu, Qt::Key_Down);
    QCOMPARE(menu.currentAction(), second);
    QTest::keyClick(&menu, Qt::Key_Up);
    QCOMPARE(menu.currentAction(), first);
    QTest::keyClick(&menu, Qt::Key_Down);
    QCOMPARE(menu.currentAction(), second);

    QTest::keyClick(&menu, Qt::Key_Return);
    QCOMPARE(triggeredSpy.count(), 1);
    QCOMPARE(actionSpy.count(), 1);
    QVERIFY(!menu.isVisible());
}

/*! A command nobody can pick is written a step down while its brothers keep
 *  their voice - the row is still there to be read, which is the point of a
 *  menu that says what is out of reach.
 */
void tst_MaterialMenu::aCommandNobodyCanPickIsWrittenFainter()
{
    MaterialMenu menu(m_anchor);
    menu.addAction(QStringLiteral("Share"));
    menu.addAction(QStringLiteral("Get link"));
    menu.open();
    qtcPinDisabledColours(&menu);

    const QImage live = menu.grab().toImage();
    menu.actions().at(0)->setEnabled(false);
    const QImage faded = menu.grab().toImage();
    QVERIFY(faded != live);
}

void tst_MaterialMenu::theKeyboardLandsOnIt()
{
    MaterialMenu widget(m_anchor);
    widget.open();
    QApplication::processEvents();

    widget.setFocus();
    QVERIFY(widget.hasFocus());
    QCOMPARE(QApplication::focusWidget(), &widget);
}

TEST_ADD(tst_MaterialMenu)

#include "tst_materialmenu.moc"
