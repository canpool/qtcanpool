/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#include "qxtoastmanager.h"
#include "qxtoastmanager_p.h"

#include <QEvent>

QX_APP_BEGIN_NAMESPACE

namespace
{

/*!
 * The outer geometry a toast needs for a plate \a plate pixels wide. The widget
 * keeps ShadowMargin on every side for its shadow, so the plate is narrower
 * than the widget and the placement below has to know by how much.
 */
QSize outerSize(const QxToast *toast, int plate)
{
    const int width = plate + 2 * QxToast::ShadowMargin;
    return QSize(width, toast->heightForWidth(width));
}

}   // namespace

QxToastManagerPrivate::QxToastManagerPrivate() = default;

void QxToastManagerPrivate::adopt(QxToast *toast)
{
    Q_Q(QxToastManager);

    // Both connections carry the toast, so they go away with it. dismissRequested
    // is what keeps the stack honest when the toast decides on its own - the
    // clock, a click, or a caller holding the pointer; dismissed is what frees
    // the widget once it is off screen.
    QObject::connect(toast, &QxToast::dismissRequested, q, [this, toast]() {
        remove(toast);
    });
    QObject::connect(toast, &QxToast::dismissed, toast, &QObject::deleteLater);
    QObject::connect(toast, &QxToast::sizeHintChanged, q, [this]() {
        relayout();
    });

    m_toasts.append(toast);
}

void QxToastManagerPrivate::remove(QxToast *toast)
{
    Q_Q(QxToastManager);

    const int index = m_toasts.indexOf(toast);
    if (index < 0) {
        // Already gone. A dismissal that started here comes back through
        // dismissRequested, and it has to find this harmless.
        return;
    }

    m_toasts.removeAt(index);
    relayout();
    Q_EMIT q->toastDismissed(toast);

    // Whatever took the toast out of the stack, the widget still has to leave
    // the screen. When the dismissal started in dismiss() this is a no-op.
    toast->dismiss();
}

void QxToastManagerPrivate::relayout()
{
    if (!m_host) {
        return;
    }

    const int shadow = QxToast::ShadowMargin;
    // margin() and spacing() describe the plates. The widget of a toast is
    // ShadowMargin larger on every side - that is where its shadow goes - so
    // that room comes out of the spacing before anything is placed.
    const int gap = qMax(0, m_spacing - 2 * shadow);
    const QRect area = m_host->rect();
    const bool atTop = m_position == QxToastManager::TopLeft || m_position == QxToastManager::TopRight;
    const bool atLeft = m_position == QxToastManager::TopLeft || m_position == QxToastManager::BottomLeft;

    const int plate = plateWidth();

    // A column anchored to the bottom has to know how tall it is before it can
    // be placed, so it is measured in plate coordinates first.
    int stackHeight = 0;
    for (int i = 0; i < m_toasts.count(); ++i) {
        stackHeight += outerSize(m_toasts.at(i), plate).height() - 2 * shadow;
    }
    stackHeight += qMax(0, m_toasts.count() - 1) * m_spacing;

    int top = atTop ? area.top() + m_margin : area.bottom() - m_margin - stackHeight + 1;

    for (int i = 0; i < m_toasts.count(); ++i) {
        QxToast *toast = m_toasts.at(i);
        const QSize outer = outerSize(toast, plate);
        const int left = atLeft ? area.left() + m_margin : area.right() - m_margin - plate + 1;
        toast->setGeometry(left - shadow, top - shadow, outer.width(), outer.height());
        top += outer.height() + gap;
    }
}

int QxToastManagerPrivate::plateWidth() const
{
    if (!m_host) {
        return m_maxWidth;
    }
    const int available = qMax(1, m_host->width() - 2 * m_margin);
    return qMin(m_maxWidth, available);
}

QxToastManager::QxToastManager(QWidget *host, QObject *parent)
    : QObject(parent)
    , d_ptr(new QxToastManagerPrivate())
{
    Q_D(QxToastManager);
    d->setPublic(this);
    d->m_host = host;
    if (host) {
        // The stack is anchored to the host's edges, so a resize moves it.
        host->installEventFilter(this);
    }
}

QxToastManager::~QxToastManager()
{
    QX_FINI_PRIVATE();
}

QWidget *QxToastManager::host() const
{
    Q_D(const QxToastManager);
    return d->m_host;
}

int QxToastManager::maxVisible() const
{
    Q_D(const QxToastManager);
    return d->m_maxVisible;
}

void QxToastManager::setMaxVisible(int count)
{
    Q_D(QxToastManager);
    d->m_maxVisible = qMax(1, count);

    while (d->m_toasts.count() > d->m_maxVisible) {
        d->remove(d->m_toasts.first());
    }
    d->relayout();
}

QxToastManager::Position QxToastManager::position() const
{
    Q_D(const QxToastManager);
    return d->m_position;
}

void QxToastManager::setPosition(Position position)
{
    Q_D(QxToastManager);
    if (d->m_position == position) {
        return;
    }
    d->m_position = position;
    d->relayout();
}

int QxToastManager::margin() const
{
    Q_D(const QxToastManager);
    return d->m_margin;
}

void QxToastManager::setMargin(int pixels)
{
    Q_D(QxToastManager);
    d->m_margin = qMax(0, pixels);
    d->relayout();
}

int QxToastManager::spacing() const
{
    Q_D(const QxToastManager);
    return d->m_spacing;
}

void QxToastManager::setSpacing(int pixels)
{
    Q_D(QxToastManager);
    d->m_spacing = qMax(0, pixels);
    d->relayout();
}

int QxToastManager::maxWidth() const
{
    Q_D(const QxToastManager);
    return d->m_maxWidth;
}

void QxToastManager::setMaxWidth(int pixels)
{
    Q_D(QxToastManager);
    // Clamped to the range a plate can actually take, so that the width the
    // manager places at and the width the toast draws are never two different
    // numbers.
    d->m_maxWidth = qBound(QxToast::MinPlateWidth, pixels, QxToast::MaxPlateWidth);
    d->relayout();
}

int QxToastManager::fadeDuration() const
{
    Q_D(const QxToastManager);
    return d->m_fadeDuration;
}

void QxToastManager::setFadeDuration(int ms)
{
    Q_D(QxToastManager);
    d->m_fadeDuration = qMax(0, ms);
    for (QxToast *toast : d->m_toasts) {
        toast->setFadeDuration(d->m_fadeDuration);
    }
}

int QxToastManager::count() const
{
    Q_D(const QxToastManager);
    return d->m_toasts.count();
}

QList<QxToast *> QxToastManager::toasts() const
{
    Q_D(const QxToastManager);
    return d->m_toasts;
}

QxToast *QxToastManager::toastAt(int index) const
{
    Q_D(const QxToastManager);
    if (index < 0 || index >= d->m_toasts.count()) {
        return Q_NULLPTR;
    }
    return d->m_toasts.at(index);
}

bool QxToastManager::contains(QxToast *toast) const
{
    Q_D(const QxToastManager);
    return d->m_toasts.contains(toast);
}

QxToast *QxToastManager::show(const QString &text, QxToast::Level level, int timeoutMs)
{
    Q_D(QxToastManager);
    if (!d->m_host) {
        qWarning("QxToastManager: there is no host window to show a toast in");
        return Q_NULLPTR;
    }

    // Making room before the new toast joins keeps the stack at its height: the
    // column is never taller than maxVisible(), not even for the instant it
    // takes to evict and add.
    while (!d->m_toasts.isEmpty() && d->m_toasts.count() >= d->m_maxVisible) {
        d->remove(d->m_toasts.first());
    }

    QxToast *toast = new QxToast(text, level, d->m_host);
    toast->setTimeout(timeoutMs);
    toast->setFadeDuration(d->m_fadeDuration);
    d->adopt(toast);

    d->relayout();
    toast->start();
    Q_EMIT toastShown(toast);
    return toast;
}

void QxToastManager::dismissAll()
{
    Q_D(QxToastManager);
    // remove() rewrites the stack, so it is walked over a copy of it.
    const QList<QxToast *> toasts = d->m_toasts;
    for (QxToast *toast : toasts) {
        d->remove(toast);
    }
}

bool QxToastManager::eventFilter(QObject *watched, QEvent *event)
{
    Q_D(QxToastManager);
    if (watched == d->m_host && event->type() == QEvent::Resize) {
        d->relayout();
    }
    return QObject::eventFilter(watched, event);
}

QX_APP_END_NAMESPACE
