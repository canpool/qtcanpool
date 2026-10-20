/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/

/*!
 * \file
 *
 * DockOverlay and DockOverlayCross are the two halves of the drop indicator: the
 * overlay owns the allowed areas, the cross is the widget that paints them and is
 * reset whenever those areas change.
 *
 * The overlay half is a flags round trip, which is worth having only because
 * setAllowedArea() goes through QFlags::setFlag() and not through a plain
 * assignment - turning one area off has to leave the others alone.
 *
 * The cross half pins how the qproperty iconColors string is parsed: five
 * "Name=#AARRGGBB" entries separated by spaces, and anything it does not
 * recognise is skipped rather than treated as an error. Styling comes from a
 * stylesheet, so a name that is not in the table is a typo the user should see
 * ignored, not an exception.
 */
#include "tst_global.h"

#include "qxdock/dockoverlay.h"

#include <QGuiApplication>
#include <QLabel>
#include <QImage>
#include <QPalette>
#include <QTest>
#include <QWidget>

QX_DOCK_USE_NAMESPACE

namespace
{
/*!
 * The pixmap one of the cross's drop indicators is showing.
 *
 * Read from the label rather than captured from the cross: the cross is a
 * translucent tool window, and a capture of it on the offscreen platform comes
 * back empty - measured, 113 bytes of nothing for either scheme, which would
 * make any comparison of two captures pass without measuring anything.
 */
QImage indicatorImage(DockOverlayCross *cross, Qx::DockWidgetArea area)
{
    const QList<QLabel *> labels = cross->findChildren<QLabel *>(QStringLiteral("DockWidgetAreaLabel"));
    for (QLabel *label : labels) {
        if (label->property("dockWidgetArea").toInt() != int(area)) {
            continue;
        }
        // A label hands its pixmap out by value from Qt 6 on, by pointer before.
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
        const QPixmap pixmap = label->pixmap();
        if (!pixmap.isNull()) {
            return pixmap.toImage();
        }
#else
        const QPixmap *pixmap = label->pixmap();
        if (pixmap && !pixmap->isNull()) {
            return pixmap->toImage();
        }
#endif
    }
    return QImage();
}
}   // namespace

class tst_DockOverlay : public QObject
{
    Q_OBJECT

private slots:
    void allowedAreasRoundTrip();
    void turningOneAreaOffLeavesTheOthers();
};

class tst_DockOverlayCross : public QObject
{
    Q_OBJECT

private slots:
    void colorsAreIndependent();
    void colorStringIsParsedByName();
    void followsTheApplicationPalette();
};

void tst_DockOverlay::allowedAreasRoundTrip()
{
    QWidget target;
    DockOverlay overlay(&target);

    QCOMPARE(overlay.allowedAreas(), Qx::DockWidgetAreas(Qx::InvalidDockWidgetArea));

    overlay.setAllowedAreas(Qx::LeftDockWidgetArea | Qx::RightDockWidgetArea);
    QCOMPARE(overlay.allowedAreas(), Qx::DockWidgetAreas(Qx::LeftDockWidgetArea | Qx::RightDockWidgetArea));
}

void tst_DockOverlay::turningOneAreaOffLeavesTheOthers()
{
    QWidget target;
    DockOverlay overlay(&target);

    overlay.setAllowedAreas(Qx::LeftDockWidgetArea | Qx::RightDockWidgetArea | Qx::TopDockWidgetArea);
    overlay.setAllowedArea(Qx::LeftDockWidgetArea, false);
    QCOMPARE(overlay.allowedAreas(), Qx::DockWidgetAreas(Qx::RightDockWidgetArea | Qx::TopDockWidgetArea));

    overlay.setAllowedArea(Qx::BottomDockWidgetArea, true);
    QCOMPARE(overlay.allowedAreas(),
             Qx::DockWidgetAreas(Qx::RightDockWidgetArea | Qx::TopDockWidgetArea | Qx::BottomDockWidgetArea));

    // Setting what is already set must not toggle it off.
    overlay.setAllowedArea(Qx::TopDockWidgetArea, true);
    QVERIFY(overlay.allowedAreas().testFlag(Qx::TopDockWidgetArea));
}

void tst_DockOverlayCross::colorsAreIndependent()
{
    QWidget target;
    DockOverlay overlay(&target);
    DockOverlayCross cross(&overlay);

    cross.setIconColor(DockOverlayCross::FrameColor, QColor(QStringLiteral("#ff0000")));
    cross.setIconColor(DockOverlayCross::ArrowColor, QColor(QStringLiteral("#00ff00")));

    QCOMPARE(cross.iconColor(DockOverlayCross::FrameColor), QColor(QStringLiteral("#ff0000")));
    QCOMPARE(cross.iconColor(DockOverlayCross::ArrowColor), QColor(QStringLiteral("#00ff00")));
}

void tst_DockOverlayCross::colorStringIsParsedByName()
{
    QWidget target;
    DockOverlay overlay(&target);
    DockOverlayCross cross(&overlay);

    cross.setIconColors(QStringLiteral("Frame=#ff3d3d3d Background=#ff929292 Overlay=#1f3d3d3d "
                                       "Arrow=#ffb4b4b4 Shadow=#40474747"));

    QCOMPARE(cross.iconColor(DockOverlayCross::FrameColor), QColor(QStringLiteral("#ff3d3d3d")));
    QCOMPARE(cross.iconColor(DockOverlayCross::WindowBackgroundColor), QColor(QStringLiteral("#ff929292")));
    QCOMPARE(cross.iconColor(DockOverlayCross::OverlayColor), QColor(QStringLiteral("#1f3d3d3d")));
    QCOMPARE(cross.iconColor(DockOverlayCross::ArrowColor), QColor(QStringLiteral("#ffb4b4b4")));
    QCOMPARE(cross.iconColor(DockOverlayCross::ShadowColor), QColor(QStringLiteral("#40474747")));

    // A name the table does not know is skipped, and the rest still lands.
    cross.setIconColors(QStringLiteral("Nope=#ffffffff Arrow=#0000ff"));
    QCOMPARE(cross.iconColor(DockOverlayCross::ArrowColor), QColor(QStringLiteral("#0000ff")));
    QCOMPARE(cross.iconColor(DockOverlayCross::FrameColor), QColor(QStringLiteral("#ff3d3d3d")));
}

/*!
 * The cross is painted from the application's palette, so a theme change has to
 * reach it - both the colors it asks for and the pixmaps it has already painted
 * with them.
 *
 * Both halves are measured because they fail independently. The colors were
 * memoized on first use, so the cross kept the scheme it was first shown in;
 * and the drop indicators are pixmaps, painted once when the cross is set up, so
 * even a color asked for again would not be painted again.
 */
void tst_DockOverlayCross::followsTheApplicationPalette()
{
    const QPalette savedPalette = QGuiApplication::palette();

    QWidget host;
    DockOverlay overlay(&host);
    DockOverlayCross *cross = host.findChild<DockOverlayCross *>();
    QVERIFY2(cross != nullptr, "the overlay has no cross to measure");

    cross->setupOverlayCross(DockOverlay::PanelOverlayMode);
    cross->resize(240, 240);
    cross->show();
    QVERIFY(QTest::qWaitForWindowExposed(cross));

    QPalette light;
    light.setColor(QPalette::Highlight, QColor(0, 120, 212));
    light.setColor(QPalette::Base, QColor(255, 255, 255));
    qApp->setPalette(light);
    QTest::qWait(50);
    const QColor frameInLight = cross->iconColor(DockOverlayCross::FrameColor);

    QPalette dark;
    dark.setColor(QPalette::Highlight, QColor(76, 194, 255));
    dark.setColor(QPalette::Base, QColor(32, 32, 32));
    qApp->setPalette(dark);
    QTest::qWait(50);
    const QColor frameInDark = cross->iconColor(DockOverlayCross::FrameColor);

    // Between two drags, which is when a theme is switched: the cross is hidden
    // while nothing is being dragged, and showing it again is the next drag.
    cross->hide();
    cross->show();
    QVERIFY(QTest::qWaitForWindowExposed(cross));
    QTest::qWait(50);
    const QImage paintedInDark = indicatorImage(cross, Qx::TopDockWidgetArea);

    // And the first scheme's, painted by asking for it again: the pixmaps are
    // what a drag shows, so they are what has to follow.
    qApp->setPalette(light);
    cross->hide();
    cross->show();
    QVERIFY(QTest::qWaitForWindowExposed(cross));
    QTest::qWait(50);
    const QImage paintedInLight = indicatorImage(cross, Qx::TopDockWidgetArea);

    qApp->setPalette(savedPalette);

    QCOMPARE(frameInLight, QColor(0, 120, 212));
    QCOMPARE(frameInDark, QColor(76, 194, 255));

    QVERIFY2(!paintedInLight.isNull() && !paintedInDark.isNull(), "a drop indicator has no pixmap to compare");
    QVERIFY2(paintedInLight.size() == paintedInDark.size(), "the cross changed size with the palette");
    QVERIFY2(paintedInLight != paintedInDark,
             "the cross is painted with the palette it was first shown with: switching the theme left it as it "
             "was, and the drop indicators a drag shows are the one thing on screen at that moment");
}

TEST_ADD(tst_DockOverlay)
TEST_ADD(tst_DockOverlayCross)

#include "tst_dockoverlay.moc"
