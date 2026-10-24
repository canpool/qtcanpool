/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#include "tst_global.h"

#include "qxmaterial/materialfab.h"

#include "qxtheme/qxthememanager.h"

#include <QAccessible>
#include <QApplication>
#include <QImage>
#include <QSignalSpy>
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
#include <QEnterEvent>
#endif

QX_MATERIAL_USE_NAMESPACE
QX_THEME_USE_NAMESPACE

namespace
{
const QColor kAccent(0, 0, 255);
const QColor kIconInk(255, 0, 0);

int pixelsOf(const QColor &color, QWidget *widget)
{
    const QImage image = widget->grab().toImage();
    int count = 0;
    for (int y = 0; y < image.height(); ++y) {
        for (int x = 0; x < image.width(); ++x) {
            if (image.pixelColor(x, y) == color) {
                ++count;
            }
        }
    }
    return count;
}

/*! How much darker the button made the page round its surface: the shadow it
 *  casts is the only thing that lands there. */
int inkOutsideTheSurface(QWidget *widget)
{
    const QImage image = widget->grab().toImage().convertToFormat(QImage::Format_ARGB32);
    const QRect surface = widget->contentsRect();
    const int pageGray = qGray(widget->palette().color(QPalette::Window).rgb());
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

QIcon solidIcon(const QColor &color)
{
    QPixmap pixmap(24, 24);
    pixmap.fill(color);
    return QIcon(pixmap);
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

class tst_MaterialFAB : public QObject
{
    Q_OBJECT
private slots:

    void theKeyboardLandsOnIt();
    void init();
    void cleanup();

    void itIsRoundAndKeepsRoomForItsShadow();
    void itWearsTheAccent();
    void theIconIsDrawnAsItWasGiven();
    void thePointerLiftsIt();
    void itIsPressedByThePointerAndTheKeyboard();
    void whatItSaysIsItsName();
    void aButtonNobodyCanPressIsDrawnFainter();
private:
    bool m_hadOverride = false;
    bool m_wasActive = false;
};

void tst_MaterialFAB::init()
{
    m_hadOverride = QxThemeManager::instance()->hasAccentOverride();
    QxThemeManager::instance()->setAccentColor(kAccent);
    m_wasActive = QAccessible::isActive();
}

void tst_MaterialFAB::cleanup()
{
    if (!m_hadOverride) {
        QxThemeManager::instance()->setAccentColor(QColor());
    }
    QAccessible::setActive(m_wasActive);
}

/*! A FAB is the one thing a screen is for, and it says so by being round and
 *  clear of what it floats over - so it keeps room for the shadow it casts
 *  rather than growing to fill the widget it is given. */
void tst_MaterialFAB::itIsRoundAndKeepsRoomForItsShadow()
{
    MaterialFAB fab;
    QCOMPARE(fab.sizeHint().width(), fab.sizeHint().height());

    fab.resize(fab.sizeHint());
    const QRect surface = fab.contentsRect();
    QCOMPARE(surface.width(), surface.height());
    QCOMPARE(surface.width(), 56);
    QVERIFY(inkOutsideTheSurface(&fab) > 0);
}

/*! It is the accent's own colour, because what it stands for is what the
 *  screen is for. */
void tst_MaterialFAB::itWearsTheAccent()
{
    MaterialFAB fab;
    fab.resize(fab.sizeHint());
    QVERIFY(pixelsOf(kAccent, &fab) > 0);
}

/*! What a FAB carries is an icon, and the icon is the application's - drawn as
 *  it was given rather than recoloured here. */
void tst_MaterialFAB::theIconIsDrawnAsItWasGiven()
{
    MaterialFAB fab;
    fab.resize(fab.sizeHint());
    QCOMPARE(pixelsOf(kIconInk, &fab), 0);

    fab.setIcon(solidIcon(kIconInk));
    QVERIFY(pixelsOf(kIconInk, &fab) > 0);
}

/*! The pointer does not tint it - it lifts it, which is more shadow rather
 *  than more colour. */
void tst_MaterialFAB::thePointerLiftsIt()
{
    MaterialFAB fab;
    fab.resize(fab.sizeHint());
    const int resting = inkOutsideTheSurface(&fab);

    sendEnter(&fab);
    QVERIFY(inkOutsideTheSurface(&fab) > resting);
}

/*! It is a button, so the pointer presses it and so does the keyboard - Enter
 *  on the press and Space on the release. */
void tst_MaterialFAB::itIsPressedByThePointerAndTheKeyboard()
{
    MaterialFAB fab;
    fab.resize(fab.sizeHint());

    QSignalSpy pressed(&fab, &MaterialFAB::clicked);
    QTest::mouseClick(&fab, Qt::LeftButton);
    QCOMPARE(pressed.count(), 1);
    QTest::keyClick(&fab, Qt::Key_Enter);
    QCOMPARE(pressed.count(), 2);
    QTest::keyClick(&fab, Qt::Key_Space);
    QCOMPARE(pressed.count(), 3);
}

/*! What it says is not drawn, so it is what an assistive tool is told - and it
 *  is a button, in a FAB's shape. */
void tst_MaterialFAB::whatItSaysIsItsName()
{
    MaterialFAB fab;
    fab.setText(QStringLiteral("Compose"));
    QCOMPARE(fab.accessibleName(), QStringLiteral("Compose"));

    QAccessible::setActive(true);
    QAccessibleInterface *iface = QAccessible::queryAccessibleInterface(&fab);
    QVERIFY(iface);
    QCOMPARE(iface->role(), QAccessible::PushButton);
    QCOMPARE(iface->text(QAccessible::Name), QStringLiteral("Compose"));
    QAccessible::deleteAccessibleInterface(QAccessible::uniqueId(iface));
}

/*! A floating action nobody can press is written a step down - its disc, its
 *  glyph and its word together. Pinned first, so the frame that differs is the
 *  button's own.
 */
void tst_MaterialFAB::aButtonNobodyCanPressIsDrawnFainter()
{
    MaterialFAB fab;
    fab.setText(QStringLiteral("Compose"));
    fab.resize(fab.sizeHint());
    fab.show();
    qtcPinDisabledColours(&fab);

    const QImage live = fab.grab().toImage();
    fab.setEnabled(false);
    const QImage faded = fab.grab().toImage();
    QVERIFY(faded != live);
}

void tst_MaterialFAB::theKeyboardLandsOnIt()
{
    MaterialFAB widget;
    widget.show();
    QApplication::processEvents();

    widget.setFocus();
    QVERIFY(widget.hasFocus());
    QCOMPARE(QApplication::focusWidget(), &widget);
}

TEST_ADD(tst_MaterialFAB)

#include "tst_materialfab.moc"
