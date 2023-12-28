/**
 * Copyleft (C) 2023 maminjie <canpool@163.com>
 * SPDX-License-Identifier: BSD-3-Clause
 **/
#include "materialslider.h"
#include "materialslider_p.h"
#include "materialstatetransitionevent.h"
#include "materialstatetransition.h"
#include "materialstyle.h"
#include <QMouseEvent>
#include <QtWidgets/QApplication>
#include <QAbstractTransition>
#include <QEventTransition>
#include <QPainter>
#include <QPropertyAnimation>
#include <QSignalTransition>
#include <QState>

QX_MATERIAL_BEGIN_NAMESPACE

/*!
 *  \class MaterialSliderStateMachine
 *  \internal
 */

MaterialSliderStateMachine::MaterialSliderStateMachine(MaterialSlider *slider, MaterialSliderThumb *thumb,
                                                       MaterialSliderTrack *track)
    : QStateMachine(slider)
    , m_slider(slider)
    , m_thumb(thumb)
    , m_track(track)
    , m_topState(new QState(QState::ParallelStates))
    , m_fstState(new QState(m_topState))
    , m_sndState(new QState(m_topState))
    , m_inactiveState(new QState(m_fstState))
    , m_focusState(new QState(m_fstState))
    , m_slidingState(new QState(m_fstState))
    , m_pulseOutState(new QState(m_focusState))
    , m_pulseInState(new QState(m_focusState))
    , m_minState(new QState(m_sndState))
    , m_normalState(new QState(m_sndState))
{
    addState(m_topState);
    setInitialState(m_topState);

    m_fstState->setInitialState(m_inactiveState);
    m_focusState->setInitialState(m_pulseOutState);

    m_inactiveState->assignProperty(thumb, "haloSize", 0);
    m_slidingState->assignProperty(thumb, "haloSize", 0);

    m_pulseOutState->assignProperty(thumb, "haloSize", 35);
    m_pulseInState->assignProperty(thumb, "haloSize", 28);

    m_inactiveState->assignProperty(thumb, "diameter", 11);
    m_focusState->assignProperty(thumb, "diameter", 11);
    m_slidingState->assignProperty(thumb, "diameter", 17);

    QAbstractTransition *transition;
    MaterialStateTransition *customTransition;
    QPropertyAnimation *animation;

    // Show halo on mouse enter

    customTransition = new MaterialStateTransition(SliderNoFocusMouseEnter);
    customTransition->setTargetState(m_focusState);

    animation = new QPropertyAnimation(thumb, "haloSize", this);
    animation->setEasingCurve(QEasingCurve::InOutSine);
    customTransition->addAnimation(animation);
    customTransition->addAnimation(new QPropertyAnimation(track, "fillColor", this));
    m_inactiveState->addTransition(customTransition);

    // Show halo on focus in

    transition = new QEventTransition(slider, QEvent::FocusIn);
    transition->setTargetState(m_focusState);

    animation = new QPropertyAnimation(thumb, "haloSize", this);
    animation->setEasingCurve(QEasingCurve::InOutSine);
    transition->addAnimation(animation);
    transition->addAnimation(new QPropertyAnimation(track, "fillColor", this));
    m_inactiveState->addTransition(transition);

    // Hide halo on focus out

    transition = new QEventTransition(slider, QEvent::FocusOut);
    transition->setTargetState(m_inactiveState);

    animation = new QPropertyAnimation(thumb, "haloSize", this);
    animation->setEasingCurve(QEasingCurve::InOutSine);
    transition->addAnimation(animation);
    transition->addAnimation(new QPropertyAnimation(track, "fillColor", this));
    m_focusState->addTransition(transition);

    // Hide halo on mouse leave, except if widget has focus

    customTransition = new MaterialStateTransition(SliderNoFocusMouseLeave);
    customTransition->setTargetState(m_inactiveState);

    animation = new QPropertyAnimation(thumb, "haloSize", this);
    animation->setEasingCurve(QEasingCurve::InOutSine);
    customTransition->addAnimation(animation);
    customTransition->addAnimation(new QPropertyAnimation(track, "fillColor", this));
    m_focusState->addTransition(customTransition);

    // Pulse in

    transition = new QSignalTransition(m_pulseOutState, SIGNAL(propertiesAssigned()));
    transition->setTargetState(m_pulseInState);

    animation = new QPropertyAnimation(thumb, "haloSize", this);
    animation->setEasingCurve(QEasingCurve::InOutSine);
    animation->setDuration(1000);
    transition->addAnimation(animation);
    m_pulseOutState->addTransition(transition);

    // Pulse out

    transition = new QSignalTransition(m_pulseInState, SIGNAL(propertiesAssigned()));
    transition->setTargetState(m_pulseOutState);

    animation = new QPropertyAnimation(thumb, "haloSize", this);
    animation->setEasingCurve(QEasingCurve::InOutSine);
    animation->setDuration(1000);
    transition->addAnimation(animation);
    m_pulseInState->addTransition(transition);

    // Slider pressed

    transition = new QSignalTransition(slider, SIGNAL(sliderPressed()));
    transition->setTargetState(m_slidingState);
    animation = new QPropertyAnimation(thumb, "diameter", this);
    animation->setDuration(70);
    transition->addAnimation(animation);

    animation = new QPropertyAnimation(thumb, "haloSize", this);
    animation->setEasingCurve(QEasingCurve::InOutSine);
    transition->addAnimation(animation);
    m_focusState->addTransition(transition);

    // Slider released

    transition = new QSignalTransition(slider, SIGNAL(sliderReleased()));
    transition->setTargetState(m_focusState);
    animation = new QPropertyAnimation(thumb, "diameter", this);
    animation->setDuration(70);
    transition->addAnimation(animation);

    animation = new QPropertyAnimation(thumb, "haloSize", this);
    animation->setEasingCurve(QEasingCurve::InOutSine);
    transition->addAnimation(animation);
    m_slidingState->addTransition(transition);

    // Min. value transitions

    m_minState->assignProperty(thumb, "borderWidth", 2);
    m_normalState->assignProperty(thumb, "borderWidth", 0);

    m_sndState->setInitialState(m_minState);

    customTransition = new MaterialStateTransition(SliderChangedFromMinimum);
    customTransition->setTargetState(m_normalState);

    animation = new QPropertyAnimation(thumb, "fillColor", this);
    animation->setDuration(200);
    customTransition->addAnimation(animation);

    animation = new QPropertyAnimation(thumb, "haloColor", this);
    animation->setDuration(300);
    customTransition->addAnimation(animation);

    animation = new QPropertyAnimation(thumb, "borderColor", this);
    animation->setDuration(200);
    customTransition->addAnimation(animation);

    animation = new QPropertyAnimation(thumb, "borderWidth", this);
    animation->setDuration(200);
    customTransition->addAnimation(animation);

    m_minState->addTransition(customTransition);

    customTransition = new MaterialStateTransition(SliderChangedToMinimum);
    customTransition->setTargetState(m_minState);

    animation = new QPropertyAnimation(thumb, "fillColor", this);
    animation->setDuration(200);
    customTransition->addAnimation(animation);

    animation = new QPropertyAnimation(thumb, "haloColor", this);
    animation->setDuration(300);
    customTransition->addAnimation(animation);

    animation = new QPropertyAnimation(thumb, "borderColor", this);
    animation->setDuration(200);
    customTransition->addAnimation(animation);

    animation = new QPropertyAnimation(thumb, "borderWidth", this);
    animation->setDuration(200);
    customTransition->addAnimation(animation);

    m_normalState->addTransition(customTransition);

    setupProperties();
}

MaterialSliderStateMachine::~MaterialSliderStateMachine()
{
}

void MaterialSliderStateMachine::setupProperties()
{
    QColor trackColor = m_slider->trackColor();
    QColor thumbColor = m_slider->thumbColor();

    m_inactiveState->assignProperty(m_track, "fillColor", trackColor.lighter(130));
    m_slidingState->assignProperty(m_track, "fillColor", trackColor);
    m_focusState->assignProperty(m_track, "fillColor", trackColor);

    QColor holeColor = m_slider->palette().color(QPalette::Base);

    if (m_slider->parentWidget()) {
        holeColor = m_slider->parentWidget()->palette().color(m_slider->backgroundRole());
    }

    m_minState->assignProperty(m_thumb, "fillColor", holeColor);

    m_minState->assignProperty(m_thumb, "haloColor", trackColor);
    m_minState->assignProperty(m_thumb, "borderColor", trackColor);

    m_normalState->assignProperty(m_thumb, "fillColor", thumbColor);
    m_normalState->assignProperty(m_thumb, "haloColor", thumbColor);
    m_normalState->assignProperty(m_thumb, "borderColor", thumbColor);

    m_slider->update();
}

/*!
 *  \class MaterialSliderThumb
 *  \internal
 */

MaterialSliderThumb::MaterialSliderThumb(MaterialSlider *slider)
    : MaterialOverlayWidget(slider->parentWidget()), m_slider(slider), m_diameter(11), m_borderWidth(2), m_haloSize(0),
      m_offset(0)
{
    slider->installEventFilter(this);

    setAttribute(Qt::WA_TransparentForMouseEvents, true);
}

MaterialSliderThumb::~MaterialSliderThumb()
{
}

bool MaterialSliderThumb::eventFilter(QObject *obj, QEvent *event)
{
    if (QEvent::ParentChange == event->type()) {
        setParent(m_slider->parentWidget());
    }

    return MaterialOverlayWidget::eventFilter(obj, event);
}

void MaterialSliderThumb::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event)

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    // Halo

    QBrush brush;
    brush.setStyle(Qt::SolidPattern);
    brush.setColor(m_haloColor);
    painter.setBrush(brush);
    painter.setPen(Qt::NoPen);

    QPointF disp = Qt::Horizontal == m_slider->orientation()
                       ? QPointF(QT_MATERIAL_SLIDER_MARGIN + m_offset, m_slider->height() / 2)
                       : QPointF(m_slider->width() / 2, QT_MATERIAL_SLIDER_MARGIN + m_offset);

    QRectF halo((m_slider->pos() - QPointF(m_haloSize, m_haloSize) / 2) + disp, QSizeF(m_haloSize, m_haloSize));

    painter.setOpacity(0.15);
    painter.drawEllipse(halo);

    // Knob

    const bool isMin = m_slider->value() == m_slider->minimum();

    brush.setColor(m_slider->isEnabled() ? m_fillColor : m_slider->disabledColor());
    painter.setBrush(!m_slider->isEnabled() && isMin ? Qt::NoBrush : brush);

    if (m_slider->isEnabled() || isMin) {
        QPen pen;
        pen.setColor(m_borderColor);
        pen.setWidthF((isMin && !m_slider->isEnabled()) ? 1.7 : m_borderWidth);
        painter.setPen(pen);
    } else {
        painter.setPen(Qt::NoPen);
    }

    QRectF geometry = Qt::Horizontal == m_slider->orientation()
                          ? QRectF(m_offset, m_slider->height() / 2 - QT_MATERIAL_SLIDER_MARGIN,
                                   QT_MATERIAL_SLIDER_MARGIN * 2, QT_MATERIAL_SLIDER_MARGIN * 2)
                                .translated(m_slider->pos())
                          : QRectF(m_slider->width() / 2 - QT_MATERIAL_SLIDER_MARGIN, m_offset,
                                   QT_MATERIAL_SLIDER_MARGIN * 2, QT_MATERIAL_SLIDER_MARGIN * 2)
                                .translated(m_slider->pos());

    qreal s = m_slider->isEnabled() ? m_diameter : 7;

    QRectF thumb(0, 0, s, s);

    thumb.moveCenter(geometry.center());

    painter.setOpacity(1);
    painter.drawEllipse(thumb);
}

/*!
 *  \class MaterialSliderTrack
 *  \internal
 */

MaterialSliderTrack::MaterialSliderTrack(MaterialSliderThumb *thumb, MaterialSlider *slider)
    : MaterialOverlayWidget(slider->parentWidget()), m_slider(slider), m_thumb(thumb), m_trackWidth(2)
{
    slider->installEventFilter(this);

    setAttribute(Qt::WA_TransparentForMouseEvents, true);

    connect(slider, SIGNAL(sliderMoved(int)), this, SLOT(update()));
}

MaterialSliderTrack::~MaterialSliderTrack()
{
}

bool MaterialSliderTrack::eventFilter(QObject *obj, QEvent *event)
{
    if (QEvent::ParentChange == event->type()) {
        setParent(m_slider->parentWidget());
    }

    return MaterialOverlayWidget::eventFilter(obj, event);
}

void MaterialSliderTrack::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event)

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    QBrush fg;
    fg.setStyle(Qt::SolidPattern);
    fg.setColor(m_slider->isEnabled() ? m_slider->thumbColor() : m_slider->disabledColor());
    QBrush bg;
    bg.setStyle(Qt::SolidPattern);
    bg.setColor(m_slider->isEnabled() ? m_fillColor : m_slider->disabledColor());

    qreal offset = m_thumb->offset();

    if (Qt::Horizontal == m_slider->orientation()) {
        painter.translate(m_slider->x() + QT_MATERIAL_SLIDER_MARGIN,
                          m_slider->y() + m_slider->height() / 2 - static_cast<qreal>(m_trackWidth) / 2);
    } else {
        painter.translate(m_slider->x() + m_slider->width() / 2 - static_cast<qreal>(m_trackWidth) / 2,
                          m_slider->y() + QT_MATERIAL_SLIDER_MARGIN);
    }

    QRectF geometry = Qt::Horizontal == m_slider->orientation()
                          ? QRectF(0, 0, m_slider->width() - QT_MATERIAL_SLIDER_MARGIN * 2, m_trackWidth)
                          : QRectF(0, 0, m_trackWidth, m_slider->height() - QT_MATERIAL_SLIDER_MARGIN * 2);

    QRectF bgRect;
    QRectF fgRect;

    if (Qt::Horizontal == m_slider->orientation()) {
        fgRect = QRectF(0, 0, offset, m_trackWidth);
        bgRect = QRectF(offset, 0, m_slider->width(), m_trackWidth).intersected(geometry);
    } else {
        fgRect = QRectF(0, 0, m_trackWidth, offset);
        bgRect = QRectF(0, offset, m_trackWidth, m_slider->height()).intersected(geometry);
    }

    if (!m_slider->isEnabled()) {
        fgRect = fgRect.width() < 9 ? QRectF() : fgRect.adjusted(0, 0, -6, 0);
        bgRect = bgRect.width() < 9 ? QRectF() : bgRect.adjusted(6, 0, 0, 0);
    }

    if (m_slider->invertedAppearance()) {
        qSwap(bgRect, fgRect);
    }

    painter.fillRect(bgRect, bg);
    painter.fillRect(fgRect, fg);
}

/*!
 *  \class MaterialSliderPrivate
 *  \internal
 */

MaterialSliderPrivate::MaterialSliderPrivate(MaterialSlider *q)
    : q_ptr(q)
{
}

MaterialSliderPrivate::~MaterialSliderPrivate()
{
}

void MaterialSliderPrivate::init()
{
    Q_Q(MaterialSlider);

    thumb = new MaterialSliderThumb(q);
    track = new MaterialSliderTrack(thumb, q);
    stateMachine = new MaterialSliderStateMachine(q, thumb, track);
    stepTo = 0;
    oldValue = q->value();
    trackWidth = 2;
    hoverTrack = false;
    hoverThumb = false;
    hover = false;
    step = false;
    pageStepMode = true;
    useThemeColors = true;

    q->setMouseTracking(true);
    q->setFocusPolicy(Qt::StrongFocus);
    q->setPageStep(1);

    QSizePolicy sp(QSizePolicy::Expanding, QSizePolicy::Fixed);

    if (q->orientation() == Qt::Vertical) {
        sp.transpose();
    }

    q->setSizePolicy(sp);
    q->setAttribute(Qt::WA_WState_OwnSizePolicy, false);

    stateMachine->start();
    QCoreApplication::processEvents();
}

QRectF MaterialSliderPrivate::trackBoundingRect() const
{
    Q_Q(const MaterialSlider);

    qreal hw = static_cast<qreal>(trackWidth) / 2;

    return Qt::Horizontal == q->orientation() ? QRectF(QT_MATERIAL_SLIDER_MARGIN, q->height() / 2 - hw,
                                                       q->width() - QT_MATERIAL_SLIDER_MARGIN * 2, hw * 2)
                                              : QRectF(q->width() / 2 - hw, QT_MATERIAL_SLIDER_MARGIN, hw * 2,
                                                       q->height() - QT_MATERIAL_SLIDER_MARGIN * 2);
}

QRectF MaterialSliderPrivate::thumbBoundingRect() const
{
    Q_Q(const MaterialSlider);

    return Qt::Horizontal == q->orientation() ? QRectF(thumb->offset(), q->height() / 2 - QT_MATERIAL_SLIDER_MARGIN,
                                                       QT_MATERIAL_SLIDER_MARGIN * 2, QT_MATERIAL_SLIDER_MARGIN * 2)
                                              : QRectF(q->width() / 2 - QT_MATERIAL_SLIDER_MARGIN, thumb->offset(),
                                                       QT_MATERIAL_SLIDER_MARGIN * 2, QT_MATERIAL_SLIDER_MARGIN * 2);
}

int MaterialSliderPrivate::valueFromPosition(const QPoint &pos) const
{
    Q_Q(const MaterialSlider);

    const int position = Qt::Horizontal == q->orientation() ? pos.x() : pos.y();

    const int span = Qt::Horizontal == q->orientation() ? q->width() - QT_MATERIAL_SLIDER_MARGIN * 2
                                                        : q->height() - QT_MATERIAL_SLIDER_MARGIN * 2;

    return MaterialStyle::sliderValueFromPosition(q->minimum(), q->maximum(), position - QT_MATERIAL_SLIDER_MARGIN,
                                                  span, q->invertedAppearance());
}

void MaterialSliderPrivate::setHovered(bool status)
{
    Q_Q(MaterialSlider);

    if (hover == status) {
        return;
    }

    hover = status;

    if (!q->hasFocus()) {
        if (status) {
            stateMachine->postEvent(new MaterialStateTransitionEvent(SliderNoFocusMouseEnter));
        } else {
            stateMachine->postEvent(new MaterialStateTransitionEvent(SliderNoFocusMouseLeave));
        }
    }

    q->update();
}

/*!
 *  \class MaterialSlider
 */

MaterialSlider::MaterialSlider(QWidget *parent) : QAbstractSlider(parent), d_ptr(new MaterialSliderPrivate(this))
{
    d_func()->init();
}

MaterialSlider::~MaterialSlider()
{
}

void MaterialSlider::setUseThemeColors(bool value)
{
    Q_D(MaterialSlider);

    if (d->useThemeColors == value) {
        return;
    }

    d->useThemeColors = value;
    d->stateMachine->setupProperties();
}

bool MaterialSlider::useThemeColors() const
{
    Q_D(const MaterialSlider);

    return d->useThemeColors;
}

void MaterialSlider::setThumbColor(const QColor &color)
{
    Q_D(MaterialSlider);

    d->thumbColor = color;

    MATERIAL_DISABLE_THEME_COLORS
    d->stateMachine->setupProperties();
    update();
}

QColor MaterialSlider::thumbColor() const
{
    Q_D(const MaterialSlider);

    if (d->useThemeColors || !d->thumbColor.isValid()) {
        return MaterialStyle::instance().themeColor(MK_primary1);
    } else {
        return d->thumbColor;
    }
}

void MaterialSlider::setTrackColor(const QColor &color)
{
    Q_D(MaterialSlider);

    d->trackColor = color;

    MATERIAL_DISABLE_THEME_COLORS
    d->stateMachine->setupProperties();
    update();
}

QColor MaterialSlider::trackColor() const
{
    Q_D(const MaterialSlider);

    if (d->useThemeColors || !d->trackColor.isValid()) {
        return MaterialStyle::instance().themeColor(MK_accent3);
    } else {
        return d->trackColor;
    }
}

void MaterialSlider::setDisabledColor(const QColor &color)
{
    Q_D(MaterialSlider);

    d->disabledColor = color;

    MATERIAL_DISABLE_THEME_COLORS
    d->stateMachine->setupProperties();
    update();
}

QColor MaterialSlider::disabledColor() const
{
    Q_D(const MaterialSlider);

    if (d->useThemeColors || !d->disabledColor.isValid()) {
        return MaterialStyle::instance().themeColor(MK_disabled1);
    } else {
        return d->disabledColor;
    }
}

void MaterialSlider::setPageStepMode(bool pageStep)
{
    Q_D(MaterialSlider);

    d->pageStepMode = pageStep;
}

bool MaterialSlider::pageStepMode() const
{
    Q_D(const MaterialSlider);

    return d->pageStepMode;
}

/*!
 *  \remip
 */
QSize MaterialSlider::minimumSizeHint() const
{
    return Qt::Horizontal == orientation() ? QSize(130, 34) : QSize(34, 130);
}

void MaterialSlider::setInvertedAppearance(bool value)
{
    QAbstractSlider::setInvertedAppearance(value);

    updateThumbOffset();
}

/*!
 *  \remip
 */
void MaterialSlider::sliderChange(SliderChange change)
{
    Q_D(MaterialSlider);

    if (SliderOrientationChange == change) {
        QSizePolicy sp(QSizePolicy::Expanding, QSizePolicy::Fixed);
        if (orientation() == Qt::Vertical) {
            sp.transpose();
        }
        setSizePolicy(sp);
    } else if (SliderValueChange == change) {
        if (minimum() == value()) {
            triggerAction(SliderToMinimum);
            d->stateMachine->postEvent(new MaterialStateTransitionEvent(SliderChangedToMinimum));
        } else if (maximum() == value()) {
            triggerAction(SliderToMaximum);
        }
        if (minimum() == d->oldValue) {
            d->stateMachine->postEvent(new MaterialStateTransitionEvent(SliderChangedFromMinimum));
        }
        d->oldValue = value();
    }

    updateThumbOffset();

    QAbstractSlider::sliderChange(change);
}

/*!
 *  \remip
 */
void MaterialSlider::mouseMoveEvent(QMouseEvent *event)
{
    Q_D(MaterialSlider);

    if (isSliderDown()) {
        setSliderPosition(d->valueFromPosition(event->pos()));
    } else {
        QRectF track(d->trackBoundingRect().adjusted(-2, -2, 2, 2));

        if (track.contains(event->pos()) != d->hoverTrack) {
            d->hoverTrack = !d->hoverTrack;
            update();
        }

        QRectF thumb(0, 0, 16, 16);
        thumb.moveCenter(d->thumbBoundingRect().center());

        if (thumb.contains(event->pos()) != d->hoverThumb) {
            d->hoverThumb = !d->hoverThumb;
            update();
        }

        d->setHovered(d->hoverTrack || d->hoverThumb);
    }

    QAbstractSlider::mouseMoveEvent(event);
}

/*!
 *  \remip
 */
void MaterialSlider::mousePressEvent(QMouseEvent *event)
{
    Q_D(MaterialSlider);

    const QPoint pos = event->pos();

    QRectF thumb(0, 0, 16, 16);
    thumb.moveCenter(d->thumbBoundingRect().center());

    if (thumb.contains(pos)) {
        setSliderDown(true);
        return;
    }

    if (!d->pageStepMode) {
        setSliderPosition(d->valueFromPosition(event->pos()));
        d->thumb->setHaloSize(0);
        setSliderDown(true);
        return;
    }

    d->step = true;
    d->stepTo = d->valueFromPosition(pos);

    SliderAction action = d->stepTo > sliderPosition() ? SliderPageStepAdd : SliderPageStepSub;

    triggerAction(action);
    setRepeatAction(action, 400, 8);
}

/*!
 *  \remip
 */
void MaterialSlider::mouseReleaseEvent(QMouseEvent *event)
{
    Q_D(MaterialSlider);

    if (isSliderDown()) {
        setSliderDown(false);
    } else if (d->step) {
        d->step = false;
        setRepeatAction(SliderNoAction, 0);
    }

    QAbstractSlider::mouseReleaseEvent(event);
}

/*!
 *  \remip
 */
void MaterialSlider::leaveEvent(QEvent *event)
{
    Q_D(MaterialSlider);

    if (d->hoverTrack) {
        d->hoverTrack = false;
        update();
    }
    if (d->hoverThumb) {
        d->hoverThumb = false;
        update();
    }

    d->setHovered(false);

    QAbstractSlider::leaveEvent(event);
}

void MaterialSlider::updateThumbOffset()
{
    Q_D(MaterialSlider);

    const int offset = MaterialStyle::sliderPositionFromValue(minimum(), maximum(), sliderPosition(),
                                                              Qt::Horizontal == orientation()
                                                                  ? width() - QT_MATERIAL_SLIDER_MARGIN * 2
                                                                  : height() - QT_MATERIAL_SLIDER_MARGIN * 2,
                                                              invertedAppearance());

    d->thumb->setOffset(offset);
}

QX_MATERIAL_END_NAMESPACE
