/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#include "qxpopupsurface.h"

#include "qxshadowpainter.h"
#include "qxtheme/qxthemeelevation.h"

#include <QtCore/QPointer>
#include <QtGui/QGuiApplication>
#include <QtGui/QKeyEvent>
#include <QtGui/QPainter>
#include <QtGui/QScreen>
#include <QtMath>

QX_CONTROLS_BEGIN_NAMESPACE

namespace
{
// The surface lifts one level above the page - the tier a menu stands at -
// and keeps the room its shadow needs so that lifting does not clip it.
constexpr QxTheme::QxShadow kSurfaceShadow = QxTheme::shadowMd;
int shadowMargin()
{
    return int(qCeil(kSurfaceShadow.blurRadius)) + 1 + int(qCeil(kSurfaceShadow.yOffset));
}
}   // namespace

class QxPopupSurfacePrivate
{
public:
    QX_DECLARE_PUBLIC(QxPopupSurface)

    QPointer<QWidget> anchor;
    qreal radius = 0.0;
};

QxPopupSurface::QxPopupSurface(QWidget *anchor)
    : QWidget(nullptr, Qt::Popup | Qt::FramelessWindowHint)
    , d_ptr(new QxPopupSurfacePrivate)
{
    QX_SET_PRIVATE(d_ptr)
    d_ptr->anchor = anchor;
    if (anchor) {
        // A surface has nothing to stand beside once its anchor is gone, and
        // everything it draws is read from the anchor - so it goes with the
        // anchor rather than outliving it with a pointer that has stopped
        // pointing at anything.
        connect(anchor, &QObject::destroyed, this, [this]() {
            // The anchor's own hands are still connected to this surface while
            // it is being torn down, and a going announced now would reach them
            // half gone - so the last going is a quiet one.
            blockSignals(true);
            hide();
            deleteLater();
        });
    }
    setFocusPolicy(Qt::StrongFocus);
    // A surface has rows that light under the pointer, and a widget is told
    // where the pointer is only while a button is down unless it asked to be
    // told always - so a list that never asked has no hover at all.
    setMouseTracking(true);
    // A surface is opened by the press that lands on its anchor, and the grab
    // moves to it while that button is still down - so Qt replays the press to
    // the surface it has just opened. The replay arrives at the pointer's
    // place, which is on the anchor and therefore outside the surface, and Qt
    // reads a press outside a popup as a click away and closes it: the list
    // shows on the press and is gone by the release, too fast to answer. This
    // is the attribute Qt's own menus and combo boxes set for exactly that
    // reason, and one surface owes the same answer as another.
    setAttribute(Qt::WA_NoMouseReplay, true);
    const int margin = shadowMargin();
    setContentsMargins(margin, margin, margin, margin);
}

QxPopupSurface::~QxPopupSurface(){QX_FINI_PRIVATE()}

QWidget *QxPopupSurface::anchor() const
{
    return d_ptr->anchor;
}

qreal QxPopupSurface::surfaceRadius() const
{
    return d_ptr->radius;
}

void QxPopupSurface::setSurfaceRadius(qreal radius)
{
    d_ptr->radius = radius;
    update();
}

QRectF QxPopupSurface::contentRect() const
{
    return QRectF(contentsRect());
}

void QxPopupSurface::open()
{
    const QSize content = contentSizeHint();
    const int margin = shadowMargin();
    resize(content.width() + 2 * margin, content.height() + 2 * margin);
    place();
    show();
    setFocus();
}

void QxPopupSurface::place()
{
    if (!d_ptr->anchor) {
        return;
    }
    QScreen *screen = d_ptr->anchor->screen();
    if (!screen) {
        screen = QGuiApplication::primaryScreen();
    }
    const QRect room = screen ? screen->availableGeometry() : QRect();
    if (room.isEmpty()) {
        return;
    }
    const QPoint below = d_ptr->anchor->mapToGlobal(QPoint(0, d_ptr->anchor->height()));
    const QPoint above = d_ptr->anchor->mapToGlobal(QPoint(0, 0));
    const int height = this->height();
    const int width = this->width();

    // Below first; above when below has no room for it; and when neither
    // has, the surface keeps inside the screen, over the anchor if it must.
    int y = below.y() + 1;
    if (y + height > room.bottom() + 1) {
        if (above.y() - 1 - height >= room.top()) {
            y = above.y() - 1 - height;
        } else {
            y = qBound(room.top(), below.y() + 1, room.bottom() + 1 - height);
        }
    }
    int x = d_ptr->anchor->mapToGlobal(QPoint(0, 0)).x();
    x = qBound(room.left(), x, room.right() + 1 - width);
    move(x, y);
}

void QxPopupSurface::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event)
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);

    const QRectF surface = QRectF(contentsRect());
    const qreal radius = d_ptr->radius;
    const QxTheme::QxShadow shadow = kSurfaceShadow;
    if (shadow.alpha > 0 && shadow.blurRadius > 0.0) {
        QxControls::QxShadowPainter::paint(&painter, surface, radius, palette().color(QPalette::WindowText), shadow);
    }
    painter.setPen(Qt::NoPen);
    painter.setBrush(palette().color(QPalette::Window));
    painter.drawRoundedRect(surface, radius, radius);
    paintContent(&painter, surface);
}

void QxPopupSurface::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Escape) {
        // The going-away is announced by the hide, which is where every
        // other hand that puts the surface away announces it too: one
        // answer, said once.
        close();
        event->accept();
        return;
    }
    QWidget::keyPressEvent(event);
}

void QxPopupSurface::mousePressEvent(QMouseEvent *event)
{
    // While a surface is up it holds the pointer, so a press on the page behind
    // arrives here too - measured from this window's corner, and outside it.
    // That press asks for the surface to go, and it goes before it can answer
    // anything with the place the press landed.
    if (!rect().contains(event->pos())) {
        close();
        event->accept();
        return;
    }
    QWidget::mousePressEvent(event);
}

void QxPopupSurface::hideEvent(QHideEvent *event)
{
    Q_UNUSED(event)
    // Away by whatever hand: Escape, a click outside, the anchor moving on.
    Q_EMIT dismissed();
}

QX_CONTROLS_END_NAMESPACE
