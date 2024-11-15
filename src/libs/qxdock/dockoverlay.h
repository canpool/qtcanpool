/**
 * Copyleft (C) 2024 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
**/
#pragma once

#include "qxdock_global.h"
#include <QFrame>

QX_DOCK_BEGIN_NAMESPACE

class DockOverlayPrivate;

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
        FrameColor,
        WindowBackgroundColor,
        OverlayColor,
        ArrowColor,
        ShadowColor
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
