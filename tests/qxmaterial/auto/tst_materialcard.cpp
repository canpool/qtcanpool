/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#include "tst_global.h"

#include "qxmaterial/materialcard.h"

#include "qxcontrols/qxcontrol.h"

#include <QAccessible>
#include <QApplication>
#include <QImage>
#include <QSignalSpy>
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
#include <QEnterEvent>
#endif

QX_MATERIAL_USE_NAMESPACE
QX_CONTROLS_USE_NAMESPACE

namespace
{
/*! How much darker the card made the page round its surface: the shadow it
 *  casts is the only thing that lands there, and a lift is more shadow rather
 *  than more of it - so what is measured is how dark the band got, not how far
 *  it reaches, which is the same band either way. */
int inkOutsideTheSurface(QWidget *card)
{
    const QImage image = card->grab().toImage().convertToFormat(QImage::Format_ARGB32);
    const QRect surface = card->contentsRect();
    const int pageGray = qGray(card->palette().color(QPalette::Window).rgb());
    int ink = 0;
    for (int y = 0; y < image.height(); ++y) {
        for (int x = 0; x < image.width(); ++x) {
            if (surface.contains(QPoint(x, y))) {
                continue;
            }
            ink += qMax(0, pageGray - qGray(image.pixel(x, y)));
        }
    }
    return ink;
}

void sendEnter(QWidget *widget)
{
    const QPoint pos = widget->rect().center();
    const QPoint global = widget->mapToGlobal(pos);
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    QEnterEvent event(QPointF{pos}, QPointF{pos}, QPointF{global});
#else
    QEvent event(QEvent::Enter);
    Q_UNUSED(global)
#endif
    QApplication::sendEvent(widget, &event);
}
}   // namespace

class tst_MaterialCard : public QObject
{
    Q_OBJECT
private slots:
    void init();
    void cleanup();

    void theRoomTheCardKeepsDependsOnTheVariant();
    void theElevatedCardCastsAShadowAndTheOthersDoNot();
    void theOutlinedCardHasALineRoundItAndTheFilledOneDoesNot();
    void theElevatedCardLiftsWhenThePointerIsOnIt();
    void aCardIsNotSomethingThePointerPresses();
    void whatAnAssistiveToolReads();
private:
    bool m_wasActive = false;
};

void tst_MaterialCard::init()
{
    m_wasActive = QAccessible::isActive();
}

void tst_MaterialCard::cleanup()
{
    QAccessible::setActive(m_wasActive);
}

/*! Only one of the three stands out by being above the page, and only that one
 *  has to keep room for a shadow. */
void tst_MaterialCard::theRoomTheCardKeepsDependsOnTheVariant()
{
    QVERIFY(MaterialCard::shadowMargin(MaterialCard::Variant::Elevated) > 0);
    QCOMPARE(MaterialCard::shadowMargin(MaterialCard::Variant::Filled), 0);
    QCOMPARE(MaterialCard::shadowMargin(MaterialCard::Variant::Outlined), 0);

    MaterialCard card;
    const int margin = MaterialCard::shadowMargin(MaterialCard::Variant::Elevated);
    QCOMPARE(card.contentsMargins().left(), margin);

    card.setVariant(MaterialCard::Variant::Filled);
    QCOMPARE(card.contentsMargins().left(), 0);
}

/*! What stands out by being above the page is the one that casts a shadow: the
 *  other two are told apart by their surface, not by a lift. */
void tst_MaterialCard::theElevatedCardCastsAShadowAndTheOthersDoNot()
{
    MaterialCard elevated(MaterialCard::Variant::Elevated);
    elevated.resize(160, 80);
    QVERIFY(inkOutsideTheSurface(&elevated) > 0);

    MaterialCard filled(MaterialCard::Variant::Filled);
    filled.resize(160, 80);
    QCOMPARE(inkOutsideTheSurface(&filled), 0);

    MaterialCard outlined(MaterialCard::Variant::Outlined);
    outlined.resize(160, 80);
    QCOMPARE(inkOutsideTheSurface(&outlined), 0);
}

/*! The outlined card's line is the whole of what makes it findable, so it is
 *  at the edge of its surface and nowhere else. */
void tst_MaterialCard::theOutlinedCardHasALineRoundItAndTheFilledOneDoesNot()
{
    MaterialCard outlined(MaterialCard::Variant::Outlined);
    outlined.resize(160, 80);
    const QImage outlinedImage = outlined.grab().toImage().convertToFormat(QImage::Format_ARGB32);
    const int middle = outlinedImage.height() / 2;
    QVERIFY(outlinedImage.pixelColor(0, middle) != outlinedImage.pixelColor(4, middle));

    MaterialCard filled(MaterialCard::Variant::Filled);
    filled.resize(160, 80);
    const QImage filledImage = filled.grab().toImage().convertToFormat(QImage::Format_ARGB32);
    QCOMPARE(filledImage.pixelColor(0, middle), filledImage.pixelColor(4, middle));
}

/*! The pointer does not tint a card - it lifts it one level, which is more
 *  shadow rather than more colour. */
void tst_MaterialCard::theElevatedCardLiftsWhenThePointerIsOnIt()
{
    MaterialCard card(MaterialCard::Variant::Elevated);
    card.resize(160, 80);
    QCOMPARE(card.state(), QxControl::State::Rest);
    const int resting = inkOutsideTheSurface(&card);

    sendEnter(&card);
    QCOMPARE(card.state(), QxControl::State::Hover);
    QVERIFY(inkOutsideTheSurface(&card) > resting);
}

/*! A card is a surface, not a button: what is inside it is what gets pressed,
 *  and a card that swallowed the press would keep its contents from answering
 *  it. */
void tst_MaterialCard::aCardIsNotSomethingThePointerPresses()
{
    MaterialCard card(MaterialCard::Variant::Elevated);
    card.resize(160, 80);

    QSignalSpy pressed(&card, &MaterialCard::clicked);
    QTest::mouseClick(&card, Qt::LeftButton);
    QCOMPARE(pressed.count(), 0);
}

/*! What an assistive tool is told: a grouping, so what a card holds is
 *  announced as a section rather than as loose widgets. */
void tst_MaterialCard::whatAnAssistiveToolReads()
{
    MaterialCard card;
    QAccessible::setActive(true);
    QAccessibleInterface *iface = QAccessible::queryAccessibleInterface(&card);
    QVERIFY(iface);
    QCOMPARE(iface->role(), QAccessible::Grouping);
    QAccessible::deleteAccessibleInterface(QAccessible::uniqueId(iface));
}

TEST_ADD(tst_MaterialCard)

#include "tst_materialcard.moc"
