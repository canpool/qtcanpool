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

#include <QWidget>

QX_DOCK_USE_NAMESPACE

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

TEST_ADD(tst_DockOverlay)
TEST_ADD(tst_DockOverlayCross)

#include "tst_dockoverlay.moc"
