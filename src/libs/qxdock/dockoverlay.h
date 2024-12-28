/**
 * Copyleft (C) 2024 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
**/
#pragma once

#include "qxdock_global.h"
#include <QFrame>

QX_DOCK_BEGIN_NAMESPACE

class DockOverlayPrivate;

/*!
 * DockOverlay paints a translucent rectangle over another widget. The geometry
 * of the rectangle is based on the mouse location.
 */
class QX_DOCK_EXPORT DockOverlay : public QFrame
{
    Q_OBJECT
public:
    using Super = QFrame;

    enum OverlayMode {
        PanelOverlayMode,
        ContainerOverlayMode
    };
public:
    explicit DockOverlay(QWidget *parent, OverlayMode mode = PanelOverlayMode);
    virtual ~DockOverlay();

    void setAllowedAreas(Qx::DockWidgetAreas areas);
    void setAllowedArea(Qx::DockWidgetArea area, bool enable);
    Qx::DockWidgetAreas allowedAreas() const;
    Qx::DockWidgetArea dropAreaUnderCursor();
    int tabIndexUnderCursor() const;

    Qx::DockWidgetArea visibleDropAreaUnderCursor();

    Qx::DockWidgetArea showOverlay(QWidget *target);
    void hideOverlay();

    void enableDropPreview(bool enable);

    QRect dropOverlayRect() const;

protected:
    virtual bool event(QEvent *e) override;
    virtual void paintEvent(QPaintEvent *e) override;
    virtual void showEvent(QShowEvent* e) override;
    virtual void hideEvent(QHideEvent* e) override;

private:
    QX_DECLARE_PRIVATE(DockOverlay)
};


class DockOverlayCrossPrivate;
/*!
 * DockOverlayCross shows a cross with 5 different drop area possibilities.
 * I could have handled everything inside DockOverlay, but because of some
 * styling issues it's better to have a separate class for the cross.
 * You can style the cross icon using the property system.
 * \code
 * QxDock--DockOverlayCross
  {
        qproperty-iconFrameColor: palette(highlight);
        qproperty-iconBackgroundColor: palette(base);
        qproperty-iconOverlayColor: palette(highlight);
        qproperty-iconArrowColor: rgb(227, 227, 227);
        qproperty-iconShadowColor: rgb(0, 0, 0);
    }
 * \endcode
 * Or you can use the iconColors property to pass in AARRGGBB values as
 * hex string like shown in the example below.
 * \code
 * QxDock--DockOverlayCross
 * {
 *     qproperty-iconColors: "Frame=#ff3d3d3d Background=#ff929292 Overlay=#1f3d3d3d Arrow=#ffb4b4b4 Shadow=#40474747";
 * }
 * \endcode
 */
class QX_DOCK_EXPORT DockOverlayCross : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(QString iconColors READ iconColors WRITE setIconColors)
    Q_PROPERTY(QColor iconFrameColor READ iconColor WRITE setIconFrameColor)
    Q_PROPERTY(QColor iconBackgroundColor READ iconColor WRITE setIconBackgroundColor)
    Q_PROPERTY(QColor iconOverlayColor READ iconColor WRITE setIconOverlayColor)
    Q_PROPERTY(QColor iconArrowColor READ iconColor WRITE setIconArrowColor)
    Q_PROPERTY(QColor iconShadowColor READ iconColor WRITE setIconShadowColor)
public:
    enum IconColor {
        FrameColor,             ///< the color of the frame of the small window icon
        WindowBackgroundColor,  ///< the background color of the small window in the icon
        OverlayColor,           ///< the color that shows the overlay (the dock side) in the icon
        ArrowColor,             ///< the arrow that points into the direction
        ShadowColor             ///< the color of the shadow rectangle that is painted below the icons
    };
    enum IconColorSizes {
        NIconColors = 5
    };
public:
    DockOverlayCross(DockOverlay *overlay);
    virtual ~DockOverlayCross();

    QColor iconColor(IconColor colorIndex) const;
    void setIconColor(IconColor colorIndex, const QColor &color);

    Qx::DockWidgetArea cursorLocation() const;

    void setupOverlayCross(DockOverlay::OverlayMode mode);

    void updateOverlayIcons();

    void reset();

    void updatePosition();

    void setIconColors(const QString &colors);

protected:
    QString iconColors() const;
    QColor iconColor() const {return QColor();}
    void setIconFrameColor(const QColor &color) {setIconColor(FrameColor, color);}
    void setIconBackgroundColor(const QColor &color) {setIconColor(WindowBackgroundColor, color);}
    void setIconOverlayColor(const QColor &color) {setIconColor(OverlayColor, color);}
    void setIconArrowColor(const QColor &color) {setIconColor(ArrowColor, color);}
    void setIconShadowColor(const QColor &color) {setIconColor(ShadowColor, color);}

    void setAreaWidgets(const QHash<Qx::DockWidgetArea, QWidget *> &widgets);

protected:
    virtual void showEvent(QShowEvent* e) override;

private:
    QX_DECLARE_PRIVATE(DockOverlayCross)
};

QX_DOCK_END_NAMESPACE
