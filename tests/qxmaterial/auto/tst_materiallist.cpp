/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#include "tst_global.h"

#include "qxmaterial/materiallist.h"
#include "qxmaterial/materiallistitem.h"

#include "qxcontrols/qxcontrol.h"
#include "qxtheme/qxthememanager.h"

#include <QAccessible>
#include <QPushButton>
#include <QSignalSpy>

QX_MATERIAL_USE_NAMESPACE

namespace
{
/*! The pointer arriving and leaving, said the way the widget reads it - the
 *  platform's answer differs by version, the arriving does not. */
void pointAt(QWidget *widget)
{
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    QEnterEvent enter(QPointF(24.0, 24.0), QPointF(24.0, 24.0), QPointF(24.0, 24.0));
    QApplication::sendEvent(widget, &enter);
#else
    QEvent enter(QEvent::Enter);
    QApplication::sendEvent(widget, &enter);
#endif
}

void pointAway(QWidget *widget)
{
    QEvent leave(QEvent::Leave);
    QApplication::sendEvent(widget, &leave);
}
}   // namespace

class tst_MaterialList : public QObject
{
    Q_OBJECT
private slots:
    void init();

    void aRowSaysOneLineOrTwo();
    void theWashFollowsThePointer();
    void theChosenRowWearsTheAccent();
    void aClickChoosesAndSaysSo();
    void choosingFromOutsideAnswersTheSame();
    void aRowTakenAwayGivesTheChoiceUp();
    void theTrailingWidgetStandsAfterTheWords();
    void theKeyboardAnswersForAPress();
    void whatAnAssistiveToolReads();
private:
    MaterialList *m_list = nullptr;
};

void tst_MaterialList::init()
{
    qRegisterMetaType<MaterialListItem *>("MaterialListItem*");
    qtcGiveFonts();
    m_list = new MaterialList;
    m_list->resize(240, 160);
    m_list->show();
}

/*! A row is forty-eight tall saying one line and seventy-two saying two;
 *  the list keeps them in the order given. */
void tst_MaterialList::aRowSaysOneLineOrTwo()
{
    MaterialListItem *oneLine = m_list->addItem(QStringLiteral("Inbox"));
    QCOMPARE(oneLine->sizeHint().height(), 48);

    MaterialListItem *twoLine = m_list->addItem(QStringLiteral("Inbox"), QStringLiteral("3 unread"));
    QCOMPARE(twoLine->sizeHint().height(), 72);

    // Saying two lines and saying one are two different faces.
    QCOMPARE(oneLine->sizeHint().height() != twoLine->sizeHint().height(), true);
    const QImage one = oneLine->grab().toImage();
    const QImage two = twoLine->grab().toImage();
    QVERIFY(one != two);

    // And the rows stand in the order they were given.
    QCOMPARE(m_list->count(), 2);
    QCOMPARE(m_list->itemAt(0), oneLine);
    QCOMPARE(m_list->itemAt(1), twoLine);
    QCOMPARE(m_list->itemAt(2), static_cast<MaterialListItem *>(nullptr));
}

/*! The row the pointer is on is washed; the rows it is not on are not. */
void tst_MaterialList::theWashFollowsThePointer()
{
    MaterialListItem *row = m_list->addItem(QStringLiteral("Inbox"));
    row->resize(240, 48);

    const QImage atRest = row->grab().toImage();
    pointAt(row);
    QCOMPARE(row->state(), QxControls::QxControl::State::Hover);
    const QImage onHover = row->grab().toImage();
    QVERIFY(onHover != atRest);

    pointAway(row);
    QCOMPARE(row->state(), QxControls::QxControl::State::Rest);
    QCOMPARE(row->grab().toImage(), atRest);
}

/*! The chosen row keeps the accent under everything else, and its words are
 *  the accent's; giving the choice up gives both back. */
void tst_MaterialList::theChosenRowWearsTheAccent()
{
    MaterialListItem *row = m_list->addItem(QStringLiteral("Inbox"));
    row->resize(240, 48);

    const QImage atRest = row->grab().toImage();
    QSignalSpy changedSpy(row, &MaterialListItem::selectedChanged);
    row->setSelected(true);
    QCOMPARE(changedSpy.count(), 1);
    const QImage chosen = row->grab().toImage();
    QVERIFY(chosen != atRest);

    // The accent is on the chosen row and nowhere else: a probe inside the
    // row reads the accent, the row at rest does not.
    const QColor accent = QxTheme::QxThemeManager::instance()->accentColor();
    const auto wearsAccent = [&accent](const QImage &image) {
        for (int y = 4; y < image.height() - 4; ++y) {
            for (int x = 4; x < image.width() - 4; ++x) {
                const QColor color = image.pixelColor(x, y);
                if (qAbs(color.red() - accent.red()) <= 8 && qAbs(color.green() - accent.green()) <= 8 &&
                    qAbs(color.blue() - accent.blue()) <= 8) {
                    return true;
                }
            }
        }
        return false;
    };
    QVERIFY(wearsAccent(chosen));
    QVERIFY(!wearsAccent(atRest));

    row->setSelected(false);
    QCOMPARE(row->grab().toImage(), atRest);
}

/*! A click on a row chooses it, gives the choice up elsewhere, and says so
 *  twice - once with the click, once with the choosing. */
void tst_MaterialList::aClickChoosesAndSaysSo()
{
    MaterialListItem *first = m_list->addItem(QStringLiteral("Inbox"));
    MaterialListItem *second = m_list->addItem(QStringLiteral("Starred"));

    QSignalSpy clickedSpy(m_list, &MaterialList::itemClicked);
    QSignalSpy currentSpy(m_list, &MaterialList::currentItemChanged);

    QTest::mouseClick(first, Qt::LeftButton, Qt::NoModifier, QPoint(24, 24));
    QCOMPARE(clickedSpy.count(), 1);
    QCOMPARE(currentSpy.count(), 1);
    QCOMPARE(m_list->currentItem(), first);
    QVERIFY(first->isSelected());

    QTest::mouseClick(second, Qt::LeftButton, Qt::NoModifier, QPoint(24, 24));
    QCOMPARE(clickedSpy.count(), 2);
    QCOMPARE(currentSpy.count(), 2);
    QCOMPARE(m_list->currentItem(), second);
    QVERIFY(second->isSelected());
    QVERIFY(!first->isSelected());

    // The choice said twice is said once: clicking the chosen row again
    // clicks, but does not choose again.
    QTest::mouseClick(second, Qt::LeftButton, Qt::NoModifier, QPoint(24, 24));
    QCOMPARE(clickedSpy.count(), 3);
    QCOMPARE(currentSpy.count(), 2);
}

/*! A row chosen from outside a click answers the same way, and letting the
 *  choice go says so with nobody chosen. */
void tst_MaterialList::choosingFromOutsideAnswersTheSame()
{
    MaterialListItem *first = m_list->addItem(QStringLiteral("Inbox"));
    MaterialListItem *second = m_list->addItem(QStringLiteral("Starred"));

    QSignalSpy currentSpy(m_list, &MaterialList::currentItemChanged);
    m_list->setCurrentItem(second);
    QCOMPARE(currentSpy.count(), 1);
    QVERIFY(second->isSelected());
    QVERIFY(!first->isSelected());

    // A row that is not of this list is no answer at all.
    MaterialListItem stranger;
    m_list->setCurrentItem(&stranger);
    QCOMPARE(currentSpy.count(), 1);
    QCOMPARE(m_list->currentItem(), second);

    m_list->clearSelection();
    QCOMPARE(currentSpy.count(), 2);
    QCOMPARE(m_list->currentItem(), static_cast<MaterialListItem *>(nullptr));
    QVERIFY(!second->isSelected());
    QCOMPARE(currentSpy.at(1).at(0).value<MaterialListItem *>(), static_cast<MaterialListItem *>(nullptr));

    // Nothing chosen, nothing to let go of: the letting go is not said
    // twice.
    m_list->clearSelection();
    QCOMPARE(currentSpy.count(), 2);
}

/*! A row taken away is no longer the chosen one, and the list says so rather
 *  than keeping a name without a row behind it. */
void tst_MaterialList::aRowTakenAwayGivesTheChoiceUp()
{
    MaterialListItem *row = m_list->addItem(QStringLiteral("Inbox"));
    m_list->setCurrentItem(row);
    QVERIFY(row->isSelected());

    QSignalSpy currentSpy(m_list, &MaterialList::currentItemChanged);
    delete row;
    QCOMPARE(currentSpy.count(), 1);
    QCOMPARE(m_list->currentItem(), static_cast<MaterialListItem *>(nullptr));
    QCOMPARE(m_list->count(), 0);
}

/*! The application's trailing widget stands after the words, at the row's
 *  trailing edge and on the middle line. */
void tst_MaterialList::theTrailingWidgetStandsAfterTheWords()
{
    MaterialListItem *row = m_list->addItem(QStringLiteral("Inbox"));
    row->resize(240, 48);
    QPushButton *action = new QPushButton(QStringLiteral("Send"));
    action->setFixedSize(60, 24);
    row->setTrailingWidget(action);

    QCOMPARE(row->trailingWidget(), action);
    QCOMPARE(action->parentWidget(), row);
    QCOMPARE(action->geometry().right(), 240 - 16 - 1);
    // The middle line: the row of forty-eight keeps the widget of
    // twenty-four twelve from its top.
    QCOMPARE(action->geometry().top(), 12);
    QVERIFY(action->isVisible());
}

/*! The keyboard answers for a press the way the pointer does. */
void tst_MaterialList::theKeyboardAnswersForAPress()
{
    MaterialListItem *row = m_list->addItem(QStringLiteral("Inbox"));

    QSignalSpy clickedSpy(row, &MaterialListItem::clicked);
    QSignalSpy clickedListSpy(m_list, &MaterialList::itemClicked);

    row->setFocus();
    QTest::keyClick(row, Qt::Key_Space);
    QCOMPARE(clickedSpy.count(), 1);
    QCOMPARE(clickedListSpy.count(), 1);
    QVERIFY(row->isSelected());

    QTest::keyClick(row, Qt::Key_Return);
    QCOMPARE(clickedSpy.count(), 2);
    QCOMPARE(clickedListSpy.count(), 2);
}

/*! What an assistive tool reads: a row is a list item, selectable, and says
 *  whether it is the chosen one. */
void tst_MaterialList::whatAnAssistiveToolReads()
{
    MaterialListItem *row = m_list->addItem(QStringLiteral("Inbox"));
    QAccessibleInterface *iface = QAccessible::queryAccessibleInterface(row);
    QVERIFY(iface);
    QCOMPARE(iface->role(), QAccessible::ListItem);
    QAccessible::State state = iface->state();
    QCOMPARE(state.selectable, true);
    QCOMPARE(state.selected, false);
    row->setSelected(true);
    state = iface->state();
    QCOMPARE(state.selected, true);
    QAccessible::deleteAccessibleInterface(QAccessible::uniqueId(iface));
}

TEST_ADD(tst_MaterialList)

#include "tst_materiallist.moc"
