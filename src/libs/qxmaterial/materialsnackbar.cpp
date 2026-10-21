/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#include "materialsnackbar.h"

#include "materialtokens.h"

#include <QtCore/QPropertyAnimation>
#include <QtCore/QTimer>
#include <QtGui/QFontMetrics>
#include <QtGui/QKeyEvent>
#include <QtGui/QMouseEvent>
#include <QtGui/QPainter>

QX_MATERIAL_BEGIN_NAMESPACE

namespace
{
// Material's snackbar is forty-eight tall, eight off the bottom of what it is
// on, and no wider than five hundred and sixty however wide that is.
constexpr int kHeight = 48;
constexpr int kMargin = 8;
constexpr int kMaxWidth = 560;
constexpr int kPadding = 16;
// The room between the sentence and the one thing to do about it.
constexpr int kActionGap = 8;

// How long it stays by default: long enough to read a sentence, not long
// enough to wait for.
constexpr int kDefaultDuration = 4000;

QPoint eventPos(const QMouseEvent *event)
{
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    return event->position().toPoint();
#else
    return event->pos();
#endif
}
}   // namespace

class MaterialSnackbarPrivate
{
public:
    QX_DECLARE_PUBLIC(MaterialSnackbar)

    QString message;
    QString actionText;
    int duration = kDefaultDuration;
    QTimer timer;
    QPropertyAnimation riser;

    QRect actionRect() const;
    void place(bool rise);
    void rise();
    void dismiss();
};

QRect MaterialSnackbarPrivate::actionRect() const
{
    if (actionText.isEmpty()) {
        return QRect();
    }
    const QFontMetrics metrics(MaterialType::font(MaterialType::Scale::Body));
    const int width = metrics.horizontalAdvance(actionText);
    // The one thing to do about it is at the end, reachable with the pointer
    // and not further away than the sentence is from the edge.
    return QRect(q_ptr->width() - kPadding - width - kActionGap, 0, width + kActionGap * 2, q_ptr->height());
}

void MaterialSnackbarPrivate::place(bool rise)
{
    QWidget *host = q_ptr->parentWidget();
    if (!host) {
        q_ptr->show();
        return;
    }
    const int width = qMax(qMin(kMaxWidth, host->width() - 2 * kMargin), kHeight);
    const int x = (host->width() - width) / 2;
    const int y = host->height() - kHeight - kMargin;
    q_ptr->resize(width, kHeight);

    const QxTheme::QxMotion motion =
        MaterialMotion::forDuration(MaterialMotion::Duration::Medium, MaterialMotion::Easing::Emphasized);
    riser.stop();
    if (!rise || motion.duration <= 0) {
        q_ptr->move(x, y);
        q_ptr->show();
        return;
    }
    // It comes up from under the bottom edge, which is the whole of the
    // entrance: a snackbar does not fade in, it arrives.
    riser.setDuration(motion.duration);
    riser.setEasingCurve(motion.curve);
    riser.setStartValue(QPoint(x, host->height()));
    riser.setEndValue(QPoint(x, y));
    q_ptr->move(x, host->height());
    q_ptr->show();
    riser.start();
}

void MaterialSnackbarPrivate::dismiss()
{
    timer.stop();
    q_ptr->hide();
}

MaterialSnackbar::MaterialSnackbar(QWidget *parent)
    : QxControls::QxControl(parent)
    , d_ptr(new MaterialSnackbarPrivate)
{
    QX_SET_PRIVATE(d_ptr)
    setFocusPolicy(Qt::StrongFocus);
    hide();
    d_ptr->timer.setSingleShot(true);
    d_ptr->riser.setTargetObject(this);
    d_ptr->riser.setPropertyName("pos");
    QObject::connect(&d_ptr->timer, &QTimer::timeout, this, [this]() {
        d_ptr->dismiss();
    });
    // A snackbar belongs to the bottom of what it is on, so it goes back there
    // when that changes size.
    if (parent) {
        parent->installEventFilter(this);
    }
}

MaterialSnackbar::~MaterialSnackbar(){QX_FINI_PRIVATE()}

QString MaterialSnackbar::message() const
{
    return d_ptr->message;
}

QString MaterialSnackbar::actionText() const
{
    return d_ptr->actionText;
}

int MaterialSnackbar::duration() const
{
    return d_ptr->duration;
}

QSize MaterialSnackbar::sizeHint() const
{
    QWidget *host = parentWidget();
    const int room = host ? host->width() - 2 * kMargin : kMaxWidth;
    return QSize(qMax(qMin(kMaxWidth, room), kHeight), kHeight);
}

void MaterialSnackbar::setMessage(const QString &message)
{
    if (d_ptr->message == message) {
        return;
    }
    d_ptr->message = message;
    // The sentence is the accessible name, and it is announced when it arrives.
    setAccessibleName(message);
    update();
}

void MaterialSnackbar::setActionText(const QString &actionText)
{
    if (d_ptr->actionText == actionText) {
        return;
    }
    d_ptr->actionText = actionText;
    update();
}

void MaterialSnackbar::setDuration(int duration)
{
    d_ptr->duration = qMax(0, duration);
}

void MaterialSnackbar::showMessage(const QString &message, int duration)
{
    setMessage(message);
    if (duration >= 0) {
        setDuration(duration);
    }
    if (isVisible()) {
        // A snackbar already up does not rise again: it says the new thing
        // where it is, and its time starts over.
        update();
    } else {
        d_ptr->place(true);
    }
    if (d_ptr->duration > 0) {
        d_ptr->timer.start(d_ptr->duration);
    }
}

qreal MaterialSnackbar::cornerRadius() const
{
    // The one surface that is from somewhere else is the one that barely turns:
    // four is Material's smallest step away from a square.
    return MaterialShape::radius(MaterialShape::Radius::Small);
}

QRectF MaterialSnackbar::chromeRect() const
{
    // The wash and the ring belong to the one thing to do about it: a snackbar
    // is not something that is pressed, and only that part of it is.
    return QRectF(d_ptr->actionRect());
}

void MaterialSnackbar::paintSurface(QPainter *painter)
{
    const QRectF area = QRectF(rect());
    const qreal radius = cornerRadius();

    painter->save();
    painter->setRenderHint(QPainter::Antialiasing, true);
    // The page's own colours turned over: the ink becomes the surface. It is
    // what makes a snackbar read as being from somewhere else.
    painter->setPen(Qt::NoPen);
    painter->setBrush(palette().color(QPalette::WindowText));
    painter->drawRoundedRect(area, radius, radius);
    painter->restore();
}

void MaterialSnackbar::paintContent(QPainter *painter)
{
    const QFontMetrics metrics(MaterialType::font(MaterialType::Scale::Body));
    const QColor ink = palette().color(QPalette::Window);
    const QRect action = d_ptr->actionRect();
    const int room = (action.isEmpty() ? width() - kPadding : action.left()) - kPadding;

    painter->save();
    painter->setFont(MaterialType::font(MaterialType::Scale::Body));
    painter->setPen(ink);
    // One line, and it is elided rather than wrapped: a snackbar that grew
    // would be a snackbar that moved what it is over.
    painter->drawText(QRect(kPadding, 0, room, height()), Qt::AlignVCenter | Qt::AlignLeft,
                      metrics.elidedText(d_ptr->message, Qt::ElideMiddle, room));

    if (!action.isEmpty()) {
        painter->setPen(accentColor());
        painter->drawText(action, Qt::AlignVCenter | Qt::AlignRight, d_ptr->actionText);
    }
    painter->restore();
}

void MaterialSnackbar::mousePressEvent(QMouseEvent *event)
{
    QxControls::QxControl::mousePressEvent(event);
    if (isEnabled() && event->button() == Qt::LeftButton && d_ptr->actionRect().contains(eventPos(event))) {
        event->accept();
    }
}

void MaterialSnackbar::mouseReleaseEvent(QMouseEvent *event)
{
    const bool onAction = d_ptr->actionRect().contains(eventPos(event));
    QxControls::QxControl::mouseReleaseEvent(event);
    if (event->button() != Qt::LeftButton || !onAction) {
        return;
    }
    // Doing the one thing is the end of the snackbar: it was there for that.
    Q_EMIT actionTriggered();
    d_ptr->dismiss();
    event->accept();
}

void MaterialSnackbar::keyPressEvent(QKeyEvent *event)
{
    const int key = event->key();
    if ((key == Qt::Key_Enter || key == Qt::Key_Return) && !d_ptr->actionText.isEmpty()) {
        Q_EMIT actionTriggered();
        d_ptr->dismiss();
        event->accept();
        return;
    }
    if (key == Qt::Key_Space || key == Qt::Key_Escape) {
        event->accept();
        return;
    }
    QxControls::QxControl::keyPressEvent(event);
}

void MaterialSnackbar::keyReleaseEvent(QKeyEvent *event)
{
    // Escape is the way out of anything that arrived without being asked for.
    if (event->key() == Qt::Key_Escape) {
        d_ptr->dismiss();
        event->accept();
        return;
    }
    if (event->key() == Qt::Key_Space && !event->isAutoRepeat() && !d_ptr->actionText.isEmpty()) {
        Q_EMIT actionTriggered();
        d_ptr->dismiss();
        event->accept();
        return;
    }
    QxControls::QxControl::keyReleaseEvent(event);
}

void MaterialSnackbar::hideEvent(QHideEvent *event)
{
    // Going away stops the clock, however it went away.
    d_ptr->timer.stop();
    QxControls::QxControl::hideEvent(event);
}

bool MaterialSnackbar::eventFilter(QObject *watched, QEvent *event)
{
    if (watched == parentWidget() && event->type() == QEvent::Resize && isVisible()) {
        // Already up: it goes where the bottom went, without arriving again.
        d_ptr->place(false);
    }
    return QxControls::QxControl::eventFilter(watched, event);
}

QAccessible::Role MaterialSnackbar::accessibleRole() const
{
    return QAccessible::AlertMessage;
}

QX_MATERIAL_END_NAMESPACE
