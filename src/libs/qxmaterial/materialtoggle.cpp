/**
 * Copyleft (C) 2023 maminjie <canpool@163.com>
 * SPDX-License-Identifier: BSD-3-Clause
 **/
#include "materialtoggle.h"
#include "materialtoggle_p.h"
#include "materialstyle.h"
#include "materialripple.h"
#include <QPropertyAnimation>
#include <QSignalTransition>
#include <QStateMachine>
#include <QtWidgets/QApplication>
#include <QEvent>
#include <QPainter>
#include <QtWidgets/QGraphicsDropShadowEffect>

QX_MATERIAL_BEGIN_NAMESPACE

/*!
 *  \class MaterialToggleRippleOverlay
 *  \internal
 */

MaterialToggleRippleOverlay::MaterialToggleRippleOverlay(MaterialToggleThumb *thumb, MaterialToggleTrack *track,
                                                         MaterialToggle *parent)
    : MaterialRippleOverlay(parent->parentWidget())
    , m_toggle(parent)
    , m_thumb(thumb)
    , m_track(track)
{
    connect(parent, SIGNAL(toggled(bool)), this, SLOT(addToggleRipple()));

    thumb->installEventFilter(this);
}

MaterialToggleRippleOverlay::~MaterialToggleRippleOverlay()
{
}

void MaterialToggleRippleOverlay::addToggleRipple()
{
    if (!m_toggle->isEnabled()) {
        return;
    }

    int t, w;

    if (Qt::Horizontal == m_toggle->orientation()) {
        t = m_toggle->height() / 2;
        w = m_thumb->height() / 2 + 10;
    } else {
        t = m_toggle->width() / 2;
        w = m_thumb->width() / 2 + 10;
    }

    MaterialRipple *ripple = new MaterialRipple(QPoint(10 + t, 20 + t));
    ripple->setColor(m_track->trackColor());
    ripple->setRadiusEndValue(w);
    ripple->setOpacityStartValue(0.8);

    addRipple(ripple);
}

bool MaterialToggleRippleOverlay::eventFilter(QObject *obj, QEvent *event)
{
    if (QEvent::Paint == event->type()) {
        setGeometry(overlayGeometry());
        QList<MaterialRipple *>::const_iterator i;
        QList<MaterialRipple *> items = ripples();
        QColor color = m_track->trackColor();
        for (i = items.begin(); i != items.end(); ++i) {
            (*i)->setColor(color);
        }
    }
    return MaterialRippleOverlay::eventFilter(obj, event);
}

QRect MaterialToggleRippleOverlay::overlayGeometry() const
{
    const qreal offset = m_thumb->offset();
    if (Qt::Horizontal == m_toggle->orientation()) {
        return m_toggle->geometry().adjusted(-10 + offset, -20, 10 + offset, 20);
    } else {
        return m_toggle->geometry().adjusted(-10, -20 + offset, 10, 20 + offset);
    }
}

/*!
 *  \class MaterialToggleThumb
 *  \internal
 */

MaterialToggleThumb::MaterialToggleThumb(MaterialToggle *parent)
    : QWidget(parent), m_toggle(parent), m_shift(0), m_offset(0)
{
    Q_ASSERT(parent);

    QGraphicsDropShadowEffect *effect = new QGraphicsDropShadowEffect;
    effect->setBlurRadius(6);
    effect->setColor(QColor(0, 0, 0, 80));
    effect->setOffset(QPointF(0, 1));
    setGraphicsEffect(effect);

    parent->installEventFilter(this);
}

MaterialToggleThumb::~MaterialToggleThumb()
{
}

void MaterialToggleThumb::setShift(qreal shift)
{
    if (m_shift == shift) {
        return;
    }

    m_shift = shift;
    updateOffset();
}

bool MaterialToggleThumb::eventFilter(QObject *obj, QEvent *event)
{
    const QEvent::Type type = event->type();

    if (QEvent::Resize == type || QEvent::Move == type) {
        setGeometry(m_toggle->rect().adjusted(8, 8, -8, -8));
        updateOffset();
    }
    return QWidget::eventFilter(obj, event);
}

void MaterialToggleThumb::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event)

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    QBrush brush;
    brush.setStyle(Qt::SolidPattern);
    brush.setColor(m_toggle->isEnabled() ? m_thumbColor : Qt::white);

    painter.setBrush(brush);
    painter.setPen(Qt::NoPen);

    int s;
    QRectF r;

    if (Qt::Horizontal == m_toggle->orientation()) {
        s = height() - 10;
        r = QRectF(5 + m_offset, 5, s, s);
    } else {
        s = width() - 10;
        r = QRectF(5, 5 + m_offset, s, s);
    }

    painter.drawEllipse(r);

    if (!m_toggle->isEnabled()) {
        brush.setColor(m_toggle->disabledColor());
        painter.setBrush(brush);
        painter.drawEllipse(r);
    }
}

void MaterialToggleThumb::updateOffset()
{
    const QSize s(Qt::Horizontal == m_toggle->orientation() ? size() : size().transposed());
    m_offset = m_shift * static_cast<qreal>(s.width() - s.height());
    update();
}

/*!
 *  \class MaterialToggleTrack
 *  \internal
 */

MaterialToggleTrack::MaterialToggleTrack(MaterialToggle *parent) : QWidget(parent), m_toggle(parent)
{
    Q_ASSERT(parent);

    parent->installEventFilter(this);
}

MaterialToggleTrack::~MaterialToggleTrack()
{
}

void MaterialToggleTrack::setTrackColor(const QColor &color)
{
    m_trackColor = color;
    update();
}

bool MaterialToggleTrack::eventFilter(QObject *obj, QEvent *event)
{
    const QEvent::Type type = event->type();

    if (QEvent::Resize == type || QEvent::Move == type) {
        setGeometry(m_toggle->rect());
    }
    return QWidget::eventFilter(obj, event);
}

void MaterialToggleTrack::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event)

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    QBrush brush;
    if (m_toggle->isEnabled()) {
        brush.setColor(m_trackColor);
        painter.setOpacity(0.8);
    } else {
        brush.setColor(m_toggle->disabledColor());
        painter.setOpacity(0.6);
    }
    brush.setStyle(Qt::SolidPattern);
    painter.setBrush(brush);
    painter.setPen(Qt::NoPen);

    if (Qt::Horizontal == m_toggle->orientation()) {
        const int h = height() / 2;
        const QRect r(0, h / 2, width(), h);
        painter.drawRoundedRect(r.adjusted(14, 4, -14, -4), h / 2 - 4, h / 2 - 4);
    } else {
        const int w = width() / 2;
        const QRect r(w / 2, 0, w, height());
        painter.drawRoundedRect(r.adjusted(4, 14, -4, -14), w / 2 - 4, w / 2 - 4);
    }
}

/*!
 *  \class MaterialTogglePrivate
 *  \internal
 */

MaterialTogglePrivate::MaterialTogglePrivate(MaterialToggle *q)
    : q_ptr(q)
{
}

MaterialTogglePrivate::~MaterialTogglePrivate()
{
}

void MaterialTogglePrivate::init()
{
    Q_Q(MaterialToggle);

    track = new MaterialToggleTrack(q);
    thumb = new MaterialToggleThumb(q);
    rippleOverlay = new MaterialToggleRippleOverlay(thumb, track, q);
    stateMachine = new QStateMachine(q);
    offState = new QState;
    onState = new QState;
    orientation = Qt::Horizontal;
    useThemeColors = true;

    q->setCheckable(true);
    q->setChecked(false);
    q->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);

    stateMachine->addState(offState);
    stateMachine->addState(onState);
    stateMachine->setInitialState(offState);

    offState->assignProperty(thumb, "shift", 0);
    onState->assignProperty(thumb, "shift", 1);

    QSignalTransition *transition;
    QPropertyAnimation *animation;

    //

    transition = new QSignalTransition(q, SIGNAL(toggled(bool)));
    transition->setTargetState(onState);
    offState->addTransition(transition);

    animation = new QPropertyAnimation(q);
    animation->setPropertyName("shift");
    animation->setTargetObject(thumb);
    animation->setDuration(200);
    animation->setEasingCurve(QEasingCurve::OutQuad);
    transition->addAnimation(animation);

    animation = new QPropertyAnimation(q);
    animation->setPropertyName("trackColor");
    animation->setTargetObject(track);
    animation->setDuration(150);
    transition->addAnimation(animation);

    animation = new QPropertyAnimation(q);
    animation->setPropertyName("thumbColor");
    animation->setTargetObject(thumb);
    animation->setDuration(150);
    transition->addAnimation(animation);

    //

    transition = new QSignalTransition(q, SIGNAL(toggled(bool)));
    transition->setTargetState(offState);
    onState->addTransition(transition);

    animation = new QPropertyAnimation(q);
    animation->setPropertyName("shift");
    animation->setTargetObject(thumb);
    animation->setDuration(200);
    animation->setEasingCurve(QEasingCurve::OutQuad);
    transition->addAnimation(animation);

    animation = new QPropertyAnimation(q);
    animation->setPropertyName("trackColor");
    animation->setTargetObject(track);
    animation->setDuration(150);
    transition->addAnimation(animation);

    animation = new QPropertyAnimation(q);
    animation->setPropertyName("thumbColor");
    animation->setTargetObject(thumb);
    animation->setDuration(150);
    transition->addAnimation(animation);

    //

    setupProperties();

    stateMachine->start();
    QCoreApplication::processEvents();
}

void MaterialTogglePrivate::setupProperties()
{
    Q_Q(MaterialToggle);

    if (q->isEnabled()) {
        const qreal shift = thumb->shift();
        if (qFuzzyCompare(shift, 1)) {
            thumb->setThumbColor(q->activeColor());
            track->setTrackColor(q->activeColor().lighter(110));
        } else if (qFuzzyCompare(1 + shift, 1)) {
            thumb->setThumbColor(q->inactiveColor());
            track->setTrackColor(q->trackColor());
        }
    }

    offState->assignProperty(track, "trackColor", q->trackColor().lighter(110));
    onState->assignProperty(track, "trackColor", q->activeColor().lighter(110));

    offState->assignProperty(thumb, "thumbColor", q->inactiveColor());
    onState->assignProperty(thumb, "thumbColor", q->activeColor());

    q->update();
}

/*!
 *  \class MaterialToggle
 */

MaterialToggle::MaterialToggle(QWidget *parent) : QAbstractButton(parent), d_ptr(new MaterialTogglePrivate(this))
{
    d_func()->init();
}

MaterialToggle::~MaterialToggle()
{
}

void MaterialToggle::setUseThemeColors(bool value)
{
    Q_D(MaterialToggle);

    d->useThemeColors = value;
    d->setupProperties();
}

bool MaterialToggle::useThemeColors() const
{
    Q_D(const MaterialToggle);

    return d->useThemeColors;
}

void MaterialToggle::setDisabledColor(const QColor &color)
{
    Q_D(MaterialToggle);

    d->disabledColor = color;

    MATERIAL_DISABLE_THEME_COLORS
    d->setupProperties();
}

QColor MaterialToggle::disabledColor() const
{
    Q_D(const MaterialToggle);

    if (d->useThemeColors || !d->disabledColor.isValid()) {
        return MaterialStyle::instance().themeColor("disabled");
    } else {
        return d->disabledColor;
    }
}

void MaterialToggle::setActiveColor(const QColor &color)
{
    Q_D(MaterialToggle);

    d->activeColor = color;

    MATERIAL_DISABLE_THEME_COLORS
    d->setupProperties();
}

QColor MaterialToggle::activeColor() const
{
    Q_D(const MaterialToggle);

    if (d->useThemeColors || !d->activeColor.isValid()) {
        return MaterialStyle::instance().themeColor("primary1");
    } else {
        return d->activeColor;
    }
}

void MaterialToggle::setInactiveColor(const QColor &color)
{
    Q_D(MaterialToggle);

    d->inactiveColor = color;

    MATERIAL_DISABLE_THEME_COLORS
    d->setupProperties();
}

QColor MaterialToggle::inactiveColor() const
{
    Q_D(const MaterialToggle);

    if (d->useThemeColors || !d->inactiveColor.isValid()) {
        return MaterialStyle::instance().themeColor("canvas");
    } else {
        return d->inactiveColor;
    }
}

void MaterialToggle::setTrackColor(const QColor &color)
{
    Q_D(MaterialToggle);

    d->trackColor = color;

    MATERIAL_DISABLE_THEME_COLORS
    d->setupProperties();
}

QColor MaterialToggle::trackColor() const
{
    Q_D(const MaterialToggle);

    if (d->useThemeColors || !d->trackColor.isValid()) {
        return MaterialStyle::instance().themeColor("accent3");
    } else {
        return d->trackColor;
    }
}

void MaterialToggle::setOrientation(Qt::Orientation orientation)
{
    Q_D(MaterialToggle);

    if (d->orientation == orientation) {
        return;
    }

    d->orientation = orientation;
    updateGeometry();
}

Qt::Orientation MaterialToggle::orientation() const
{
    Q_D(const MaterialToggle);

    return d->orientation;
}

QSize MaterialToggle::sizeHint() const
{
    Q_D(const MaterialToggle);

    return Qt::Horizontal == d->orientation ? QSize(64, 48) : QSize(48, 64);
}

bool MaterialToggle::event(QEvent *event)
{
    Q_D(MaterialToggle);

    switch (event->type()) {
    case QEvent::ParentChange: {
        QWidget *widget;
        if ((widget = parentWidget())) {
            d->rippleOverlay->setParent(widget);
        }
        break;
    }
    default:
        break;
    }
    return QAbstractButton::event(event);
}

void MaterialToggle::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event)
}

QX_MATERIAL_END_NAMESPACE
