/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#include "qxcontrol.h"

#include "qxstatelayer.h"
#include "qxringpainter.h"

#include "qxtheme/qxthememanager.h"

#include <QtGui/QFocusEvent>
#include <QtGui/QMouseEvent>
#include <QtGui/QPaintEvent>
#include <QtGui/QPainter>
#include <QtGui/QPainterPath>
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
#include <QtGui/QEnterEvent>
#endif

QX_CONTROLS_BEGIN_NAMESPACE

namespace
{
// Where a mouse event happened, in widget coordinates. Qt 6 renamed the
// accessor; the answer is the same one.
QPointF eventPos(const QMouseEvent *event)
{
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    return event->position();
#else
    return event->localPos();
#endif
}
}

class QxControlPrivate
{
public:
    QX_DECLARE_PUBLIC(QxControl)
    QxControl::State m_state = QxControl::State::Rest;
    /*! True between a press that started on the control and its release. */
    bool m_pressed = false;
};

QxControl::QxControl(QWidget *parent)
    : QWidget(parent)
    , d_ptr(new QxControlPrivate)
{
    QX_SET_PRIVATE(d_ptr)
    // A control of this kind is focusable and paints everything itself; the
    // pointer tracking is wanted whether or not a button is under it, because
    // hover is a state and not a click target.
    setAttribute(Qt::WA_Hover, true);
    setFocusPolicy(Qt::StrongFocus);
}

QxControl::~QxControl(){QX_FINI_PRIVATE()}

QxControl::State QxControl::state() const
{
    return d_ptr->m_state;
}

bool QxControl::isActive() const
{
    return d_ptr->m_state != State::Rest;
}

QxTheme::Density QxControl::density() const
{
    QxTheme::QxThemeManager *manager = QxTheme::QxThemeManager::instance();
    const QxTheme::QxThemeInfo info = manager->themeInfo(manager->theme());
    return info.isValid() ? info.density : QxTheme::Density::Comfortable;
}

QColor QxControl::textColor() const
{
    return palette().color(QPalette::WindowText);
}

QColor QxControl::accentColor() const
{
    return QxTheme::QxThemeManager::instance()->accentColor();
}

qreal QxControl::cornerRadius() const
{
    return 0.0;
}

bool QxControl::hitTest(const QPointF &pos) const
{
    return rect().contains(pos.toPoint());
}

QColor QxControl::washColor() const
{
    return textColor();
}

void QxControl::paintSurface(QPainter *painter)
{
    Q_UNUSED(painter)
}

void QxControl::paintContent(QPainter *painter)
{
    Q_UNUSED(painter)
}

void QxControl::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event)
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);

    paintSurface(&painter);

    // The wash goes between the surface and the content: over the fill the
    // control painted, under the glyphs it is telling the user about.
    if (isActive() && isEnabled()) {
        const qreal opacity =
            d_ptr->m_state == State::Pressed ? QxStateLayer::pressedOpacity : QxStateLayer::hoverOpacity;
        // The wash follows the control's corners, so a rounded control does
        // not get a square tint at its own round edge.
        QPainterPath clip;
        const qreal radius = cornerRadius();
        clip.addRoundedRect(QRectF(rect()), radius, radius);
        painter.save();
        painter.setClipPath(clip);
        QxStateLayer::paint(&painter, QRectF(rect()), washColor(), opacity);
        painter.restore();
    }

    paintContent(&painter);

    if (hasFocus() && isEnabled()) {
        // One band in the accents the theme is running with: the ring's width
        // is a mechanism constant, where the family hangs it is the family's.
        QxRing ring;
        ring.color = accentColor();
        ring.width = 2.0;
        ring.inset = 0.0;
        QxRingPainter::paint(&painter, QRectF(rect()), cornerRadius(), ring);
    }
}

void QxControl::setState(State state)
{
    if (d_ptr->m_state == state) {
        return;
    }
    d_ptr->m_state = state;
    update();
    Q_EMIT stateChanged(state);
}

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
void QxControl::enterEvent(QEnterEvent *event)
#else
void QxControl::enterEvent(QEvent *event)
#endif
{
    QWidget::enterEvent(event);
    if (!d_ptr->m_pressed) {
        setState(State::Hover);
    }
}

void QxControl::leaveEvent(QEvent *event)
{
    QWidget::leaveEvent(event);
    // A press keeps the control pressed while the pointer wanders off it; the
    // release is what decides whether a click happened.
    if (!d_ptr->m_pressed) {
        setState(State::Rest);
    }
}

void QxControl::mousePressEvent(QMouseEvent *event)
{
    if (!isEnabled() || event->button() != Qt::LeftButton || !hitTest(eventPos(event))) {
        QWidget::mousePressEvent(event);
        return;
    }
    d_ptr->m_pressed = true;
    setState(State::Pressed);
}

void QxControl::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() != Qt::LeftButton || !d_ptr->m_pressed) {
        QWidget::mouseReleaseEvent(event);
        return;
    }
    d_ptr->m_pressed = false;
    const bool inside = hitTest(eventPos(event));
    // The pointer may be elsewhere by now: the release decides both what the
    // click was and what state the control is left in.
    setState(inside ? State::Hover : State::Rest);
    if (inside) {
        Q_EMIT clicked();
    }
}

void QxControl::mouseMoveEvent(QMouseEvent *event)
{
    if (d_ptr->m_pressed) {
        setState(hitTest(eventPos(event)) ? State::Pressed : State::Hover);
    }
    QWidget::mouseMoveEvent(event);
}

void QxControl::focusInEvent(QFocusEvent *event)
{
    QWidget::focusInEvent(event);
    update();
}

void QxControl::focusOutEvent(QFocusEvent *event)
{
    QWidget::focusOutEvent(event);
    update();
}

void QxControl::changeEvent(QEvent *event)
{
    QWidget::changeEvent(event);
    const QEvent::Type type = event->type();
    // Enabled and the palette both change what every hook draws.
    if (type == QEvent::EnabledChange || type == QEvent::PaletteChange || type == QEvent::ApplicationPaletteChange ||
        type == QEvent::StyleChange) {
        update();
    }
}

QX_CONTROLS_END_NAMESPACE
