/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#include "qxtoast.h"
#include "qxtoast_p.h"

#include <QEasingCurve>
#include <QEvent>
#include <QGraphicsOpacityEffect>
#include <QLabel>
#include <QMouseEvent>
#include <QPaintEvent>
#include <QPainter>
#include <QPainterPath>
#include <QPropertyAnimation>
#include <QResizeEvent>
#include <QStyle>
#include <QTimer>

QX_APP_BEGIN_NAMESPACE

namespace
{

/*! Distance between the plate and its contents. */
const int kPadding = 10;
/*! Edge of the level icon and distance between it and the text. */
const int kIconSize = 16;
const int kIconSpacing = 8;
/*! Height of a one line plate, so that a very short message still reads as a bar. */
const int kMinPlateHeight = 34;
/*! Corner radius of the plate and of the accent strip. */
const int kCornerRadius = 4;
/*! Width of the accent strip along the left edge of the plate. */
const int kAccentWidth = 3;
/*! Opacity of the plate, so that what is behind it stays faintly visible. */
const int kPlateAlpha = 236;
/*! Number of rings the drop shadow is drawn with, and the alpha of the outermost. */
const int kShadowRings = 3;
const int kShadowAlpha = 30;
/*! Hue used when the theme has none to offer, i.e. an achromatic highlight. */
const int kAchromaticHue = 203;
/*! Bounds that keep an accent legible on a plate that may be near white or near black. */
const int kMinAccentSaturation = 140;
const int kMinAccentLightness = 100;
const int kMaxAccentLightness = 170;

/*!
 * The hue a level is drawn in. Information keeps the hue of the theme's own
 * accent; the other three move to the corner of the wheel they are read in.
 */
int levelHue(QxToast::Level level, int fallback)
{
    switch (level) {
    case QxToast::Success:
        return 120;
    case QxToast::Warning:
        return 45;
    case QxToast::Error:
        return 0;
    case QxToast::Information:
        break;
    }
    return fallback;
}

QColor levelAccent(QxToast::Level level, const QColor &highlight)
{
    int hue = levelHue(level, highlight.hslHue());
    if (hue < 0) {
        hue = kAchromaticHue;
    }

    // The theme decides how loud the accent is; these two clamps only keep it
    // readable on whatever surface the theme paints the plate with.
    const int saturation = qMax(highlight.hslSaturation(), kMinAccentSaturation);
    const int lightness = qBound(kMinAccentLightness, highlight.lightness(), kMaxAccentLightness);
    return QColor::fromHsl(hue, saturation, lightness);
}

}   // namespace

QxToastPrivate::QxToastPrivate() = default;

void QxToastPrivate::init()
{
    Q_Q(QxToast);

    // A toast never takes the focus and never activates anything: a notification
    // may not interrupt what the user is typing. Setting the attribute on a
    // child widget costs nothing and states the contract.
    q->setFocusPolicy(Qt::NoFocus);
    q->setAttribute(Qt::WA_ShowWithoutActivating, true);
    q->setCursor(Qt::PointingHandCursor);

    m_icon = new QLabel(q);
    m_icon->setFixedSize(kIconSize, kIconSize);
    m_icon->setAttribute(Qt::WA_TransparentForMouseEvents, true);

    m_label = new QLabel(m_text, q);
    m_label->setWordWrap(true);
    m_label->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    // Clicks belong to the toast, not to the text inside it.
    m_label->setAttribute(Qt::WA_TransparentForMouseEvents, true);

    m_timer = new QTimer(q);
    m_timer->setSingleShot(true);
    QObject::connect(m_timer, &QTimer::timeout, q, &QxToast::dismiss);

    m_effect = new QGraphicsOpacityEffect(q);
    m_effect->setOpacity(0.0);
    q->setGraphicsEffect(m_effect);

    m_fade = new QPropertyAnimation(m_effect, "opacity", q);
    m_fade->setEasingCurve(QEasingCurve::InOutQuad);
    QObject::connect(m_fade, &QPropertyAnimation::finished, q, [this]() {
        if (m_dismissWhenFaded) {
            finishDismiss();
        }
    });

    applyLevel();
    q->resize(q->sizeHint());
}

void QxToastPrivate::applyLevel()
{
    Q_Q(QxToast);

    // The icon comes from the style rather than from a resource of our own: it
    // is the artwork the platform already draws for exactly these four
    // meanings, and it keeps its meaning when the palette changes.
    QStyle::StandardPixmap standard = QStyle::SP_MessageBoxInformation;
    switch (m_level) {
    case QxToast::Success:
        standard = QStyle::SP_DialogApplyButton;
        break;
    case QxToast::Warning:
        standard = QStyle::SP_MessageBoxWarning;
        break;
    case QxToast::Error:
        standard = QStyle::SP_MessageBoxCritical;
        break;
    case QxToast::Information:
        break;
    }
    m_icon->setPixmap(q->style()->standardIcon(standard).pixmap(kIconSize, kIconSize));
    m_label->setText(m_text);

    // The plate is a tooltip shaped surface, so the text is drawn with the role
    // a tooltip would use - which is the role the theme keeps in step with the
    // plate.
    QPalette labelPalette = m_label->palette();
    labelPalette.setColor(QPalette::WindowText, q->palette().color(QPalette::ToolTipText));
    m_label->setPalette(labelPalette);
}

int QxToastPrivate::plateWidth(int outerWidth) const
{
    const int bare = qMax(1, outerWidth - 2 * QxToast::ShadowMargin);
    // Narrower than the minimum is allowed - the host may simply be that small -
    // but wider than the maximum is not.
    return qBound(qMin(QxToast::MinPlateWidth, bare), bare, QxToast::MaxPlateWidth);
}

int QxToastPrivate::textWidthFor(int plate)
{
    return qMax(1, plate - 2 * kPadding - kIconSize - kIconSpacing);
}

int QxToastPrivate::outerHeightFor(int outerWidth) const
{
    const int textHeight = m_label ? m_label->heightForWidth(textWidthFor(plateWidth(outerWidth))) : 0;
    return qMax(kMinPlateHeight, 2 * kPadding + textHeight) + 2 * QxToast::ShadowMargin;
}

void QxToastPrivate::placeChildren()
{
    Q_Q(QxToast);

    const int plate = plateWidth(q->width());
    const int plateLeft = (q->width() - plate) / 2;
    const int plateTop = QxToast::ShadowMargin;
    const int plateHeight = qMax(0, q->height() - 2 * QxToast::ShadowMargin);

    m_icon->move(plateLeft + kPadding, plateTop + qMax(0, (plateHeight - kIconSize) / 2));
    m_label->setGeometry(plateLeft + kPadding + kIconSize + kIconSpacing, plateTop + kPadding, textWidthFor(plate),
                         qMax(0, plateHeight - 2 * kPadding));
}

void QxToastPrivate::startFade(qreal to, bool dismissWhenFaded)
{
    Q_Q(QxToast);

    m_dismissWhenFaded = dismissWhenFaded;
    m_fade->stop();

    if (m_fadeDuration <= 0) {
        // No animation: the state is taken straight away, which is what lets a
        // test walk the whole life cycle without waiting for a clock.
        m_effect->setOpacity(to);
        if (dismissWhenFaded) {
            finishDismiss();
        }
        return;
    }

    m_fade->setDuration(m_fadeDuration);
    m_fade->setStartValue(m_effect->opacity());
    m_fade->setEndValue(to);
    m_fade->start();
}

void QxToastPrivate::finishDismiss()
{
    Q_Q(QxToast);

    q->hide();
    Q_EMIT q->dismissed();
}

QxToast::QxToast(const QString &text, Level level, QWidget *parent)
    : QWidget(parent)
    , d_ptr(new QxToastPrivate())
{
    Q_D(QxToast);
    d->setPublic(this);
    d->m_text = text;
    d->m_level = level;
    d->init();
}

QxToast::~QxToast()
{
    QX_FINI_PRIVATE();
}

QString QxToast::text() const
{
    Q_D(const QxToast);
    return d->m_text;
}

void QxToast::setText(const QString &text)
{
    Q_D(QxToast);
    if (d->m_text == text) {
        return;
    }

    const QSize before = sizeHint();
    d->m_text = text;
    d->applyLevel();
    if (sizeHint() != before) {
        Q_EMIT sizeHintChanged();
    }
}

QxToast::Level QxToast::level() const
{
    Q_D(const QxToast);
    return d->m_level;
}

void QxToast::setLevel(Level level)
{
    Q_D(QxToast);
    if (d->m_level == level) {
        return;
    }
    d->m_level = level;
    d->applyLevel();
    update();
}

int QxToast::timeout() const
{
    Q_D(const QxToast);
    return d->m_timeout;
}

void QxToast::setTimeout(int timeoutMs)
{
    Q_D(QxToast);
    d->m_timeout = timeoutMs;
}

int QxToast::fadeDuration() const
{
    Q_D(const QxToast);
    return d->m_fadeDuration;
}

void QxToast::setFadeDuration(int ms)
{
    Q_D(QxToast);
    d->m_fadeDuration = qMax(0, ms);
}

bool QxToast::isPaused() const
{
    Q_D(const QxToast);
    return d->m_paused;
}

bool QxToast::isDismissing() const
{
    Q_D(const QxToast);
    return d->m_dismissing;
}

QColor QxToast::accentColor() const
{
    Q_D(const QxToast);
    return levelAccent(d->m_level, palette().color(QPalette::Highlight));
}

QSize QxToast::sizeHint() const
{
    const int width = MaxPlateWidth + 2 * ShadowMargin;
    return QSize(width, heightForWidth(width));
}

int QxToast::heightForWidth(int width) const
{
    Q_D(const QxToast);
    return d->outerHeightFor(width);
}

void QxToast::start()
{
    Q_D(QxToast);
    if (d->m_dismissing) {
        return;
    }

    d->m_paused = false;
    show();
    // Siblings such as the ribbon and the docks are already in the host; the
    // toast has to go above them rather than behind them.
    raise();
    d->placeChildren();
    d->startFade(1.0, false);

    if (d->m_timeout > 0) {
        d->m_timer->start(d->m_timeout);
    }
}

void QxToast::stop()
{
    Q_D(QxToast);
    d->m_paused = false;
    d->m_timer->stop();
}

void QxToast::dismiss()
{
    Q_D(QxToast);
    if (d->m_dismissing) {
        return;
    }

    d->m_dismissing = true;
    d->m_paused = false;
    d->m_timer->stop();

    // The manager listens to this and takes the toast out of its stack before
    // the fade even starts, so the state it reports is the state on screen.
    Q_EMIT dismissRequested();
    d->startFade(0.0, true);
}

void QxToast::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);

    const QRectF plate(ShadowMargin, ShadowMargin, qMax(0, width() - 2 * ShadowMargin),
                       qMax(0, height() - 2 * ShadowMargin));

    // A real drop shadow would need a translucent top level window. A few
    // fading rings cost less, need no window manager and are enough for an
    // overlay that always sits on a background we know.
    const QColor shadow = palette().color(QPalette::Shadow);
    painter.setPen(Qt::NoPen);
    for (int ring = kShadowRings; ring >= 1; --ring) {
        QColor layer = shadow;
        layer.setAlpha(kShadowAlpha / ring);
        painter.setBrush(layer);
        painter.drawRoundedRect(plate.adjusted(-ring, -ring + 1, ring, ring + 1), kCornerRadius + ring,
                                kCornerRadius + ring);
    }

    QColor background = palette().color(QPalette::ToolTipBase);
    background.setAlpha(kPlateAlpha);
    QColor border = palette().color(QPalette::Mid);
    border.setAlpha(kPlateAlpha);

    QPainterPath platePath;
    platePath.addRoundedRect(plate.adjusted(0.5, 0.5, -0.5, -0.5), kCornerRadius, kCornerRadius);
    painter.setBrush(background);
    painter.setPen(QPen(border, 1));
    painter.drawPath(platePath);

    // The accent runs down the left edge; clipping it to the plate keeps the
    // rounded corners intact.
    painter.save();
    painter.setClipPath(platePath);
    painter.setPen(Qt::NoPen);
    painter.setBrush(accentColor());
    painter.drawRect(QRectF(plate.left(), plate.top(), kAccentWidth, plate.height()));
    painter.restore();
}

void QxToast::resizeEvent(QResizeEvent *event)
{
    Q_D(QxToast);
    QWidget::resizeEvent(event);
    d->placeChildren();
}

bool QxToast::event(QEvent *event)
{
    Q_D(QxToast);

    switch (event->type()) {
    case QEvent::Enter:
        // Hovering holds the countdown back instead of dismissing: a message
        // that is being read must not vanish under the pointer.
        if (!d->m_timer->isActive()) {
            break;
        }
        d->m_remaining = d->m_timer->remainingTime();
        d->m_timer->stop();
        d->m_paused = true;
        break;
    case QEvent::Leave:
        if (!d->m_paused) {
            break;
        }
        d->m_paused = false;
        if (d->m_remaining > 0) {
            d->m_timer->start(d->m_remaining);
        }
        break;
    default:
        break;
    }

    return QWidget::event(event);
}

void QxToast::mouseReleaseEvent(QMouseEvent *event)
{
    QWidget::mouseReleaseEvent(event);
    if (event->button() != Qt::LeftButton) {
        return;
    }

    Q_EMIT clicked();
    dismiss();
}

QX_APP_END_NAMESPACE
