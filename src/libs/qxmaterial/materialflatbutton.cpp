/**
 * Copyleft (C) 2023 maminjie <canpool@163.com>
 * SPDX-License-Identifier: BSD-3-Clause
 **/
#include "materialflatbutton.h"
#include "materialflatbutton_p.h"
#include "materialripple.h"
#include "materialrippleoverlay.h"
#include "materialstyle.h"
#include "materialstatetransition.h"
#include <QBitmap>
#include <QIcon>
#include <QMouseEvent>
#include <QPainter>
#include <QPainterPath>
#include <QResizeEvent>
#include <QEventTransition>
#include <QFocusEvent>
#include <QPropertyAnimation>
#include <QSequentialAnimationGroup>

QX_MATERIAL_BEGIN_NAMESPACE

/*!
 *  \class MaterialFlatButtonStateMachine
 *  \internal
 */

MaterialFlatButtonStateMachine::MaterialFlatButtonStateMachine(MaterialFlatButton *parent)
    : QStateMachine(parent)
    , m_button(parent)
    , m_topLevelState(new QState(QState::ParallelStates))
    , m_configState(new QState(m_topLevelState))
    , m_checkableState(new QState(m_topLevelState))
    , m_checkedState(new QState(m_checkableState))
    , m_uncheckedState(new QState(m_checkableState))
    , m_neutralState(new QState(m_configState))
    , m_neutralFocusedState(new QState(m_configState))
    , m_hoveredState(new QState(m_configState))
    , m_hoveredFocusedState(new QState(m_configState))
    , m_pressedState(new QState(m_configState))
    , m_haloAnimation(new QSequentialAnimationGroup(this))
    , m_overlayOpacity(0)
    , m_checkedOverlayProgress(parent->isChecked() ? 1 : 0)
    , m_haloOpacity(0)
    , m_haloSize(0.8)
    , m_haloScaleFactor(1)
    , m_wasChecked(false)
{
    Q_ASSERT(parent);

    parent->installEventFilter(this);

    m_configState->setInitialState(m_neutralState);
    addState(m_topLevelState);
    setInitialState(m_topLevelState);

    m_checkableState->setInitialState(parent->isChecked() ? m_checkedState : m_uncheckedState);
    MaterialStateTransition *transition;
    QPropertyAnimation *animation;

    transition = new MaterialStateTransition(FlatButtonCheckedTransition);
    transition->setTargetState(m_checkedState);
    m_uncheckedState->addTransition(transition);

    animation = new QPropertyAnimation(this, "checkedOverlayProgress", this);
    animation->setDuration(200);
    transition->addAnimation(animation);

    transition = new MaterialStateTransition(FlatButtonUncheckedTransition);
    transition->setTargetState(m_uncheckedState);
    m_checkedState->addTransition(transition);

    animation = new QPropertyAnimation(this, "checkedOverlayProgress", this);
    animation->setDuration(200);
    transition->addAnimation(animation);

    addTransition(m_button, QEvent::FocusIn, m_neutralState, m_neutralFocusedState);
    addTransition(m_button, QEvent::FocusOut, m_neutralFocusedState, m_neutralState);
    addTransition(m_button, QEvent::Enter, m_neutralState, m_hoveredState);
    addTransition(m_button, QEvent::Leave, m_hoveredState, m_neutralState);
    addTransition(m_button, QEvent::Enter, m_neutralFocusedState, m_hoveredFocusedState);
    addTransition(m_button, QEvent::Leave, m_hoveredFocusedState, m_neutralFocusedState);
    addTransition(m_button, QEvent::FocusIn, m_hoveredState, m_hoveredFocusedState);
    addTransition(m_button, QEvent::FocusOut, m_hoveredFocusedState, m_hoveredState);

    transition = new MaterialStateTransition(FlatButtonPressedTransition);
    transition->setTargetState(m_pressedState);
    m_hoveredState->addTransition(transition);

    addTransition(m_button, QEvent::Leave, m_pressedState, m_neutralFocusedState);
    addTransition(m_button, QEvent::FocusOut, m_pressedState, m_hoveredState);

    m_neutralState->assignProperty(this, "haloSize", 0);
    m_neutralFocusedState->assignProperty(this, "haloSize", 0.7);
    m_hoveredState->assignProperty(this, "haloSize", 0);
    m_pressedState->assignProperty(this, "haloSize", 4);
    m_hoveredFocusedState->assignProperty(this, "haloSize", 0.7);

    QPropertyAnimation *grow = new QPropertyAnimation(this);
    QPropertyAnimation *shrink = new QPropertyAnimation(this);

    grow->setTargetObject(this);
    grow->setPropertyName("haloScaleFactor");
    grow->setStartValue(0.56);
    grow->setEndValue(0.63);
    grow->setEasingCurve(QEasingCurve::InOutSine);
    grow->setDuration(840);

    shrink->setTargetObject(this);
    shrink->setPropertyName("haloScaleFactor");
    shrink->setStartValue(0.63);
    shrink->setEndValue(0.56);
    shrink->setEasingCurve(QEasingCurve::InOutSine);
    shrink->setDuration(840);

    m_haloAnimation->addAnimation(grow);
    m_haloAnimation->addAnimation(shrink);
    m_haloAnimation->setLoopCount(-1);
}

MaterialFlatButtonStateMachine::~MaterialFlatButtonStateMachine()
{
}

void MaterialFlatButtonStateMachine::setOverlayOpacity(qreal opacity)
{
    m_overlayOpacity = opacity;
    m_button->update();
}

void MaterialFlatButtonStateMachine::setCheckedOverlayProgress(qreal progress)
{
    m_checkedOverlayProgress = progress;
    m_button->update();
}

void MaterialFlatButtonStateMachine::setHaloOpacity(qreal opacity)
{
    m_haloOpacity = opacity;
    m_button->update();
}

void MaterialFlatButtonStateMachine::setHaloSize(qreal size)
{
    m_haloSize = size;
    m_button->update();
}

void MaterialFlatButtonStateMachine::setHaloScaleFactor(qreal factor)
{
    m_haloScaleFactor = factor;
    m_button->update();
}

void MaterialFlatButtonStateMachine::startAnimations()
{
    m_haloAnimation->start();
    start();
}

void MaterialFlatButtonStateMachine::setupProperties()
{
    QColor overlayColor;

    if (Qt::TransparentMode == m_button->backgroundMode()) {
        overlayColor = m_button->backgroundColor();
    } else {
        overlayColor = m_button->foregroundColor();
    }

    const qreal baseOpacity = m_button->baseOpacity();

    m_neutralState->assignProperty(this, "overlayOpacity", 0);
    m_neutralState->assignProperty(this, "haloOpacity", 0);
    m_neutralFocusedState->assignProperty(this, "overlayOpacity", 0);
    m_neutralFocusedState->assignProperty(this, "haloOpacity", baseOpacity);
    m_hoveredState->assignProperty(this, "overlayOpacity", baseOpacity);
    m_hoveredState->assignProperty(this, "haloOpacity", 0);
    m_hoveredFocusedState->assignProperty(this, "overlayOpacity", baseOpacity);
    m_hoveredFocusedState->assignProperty(this, "haloOpacity", baseOpacity);
    m_pressedState->assignProperty(this, "overlayOpacity", baseOpacity);
    m_pressedState->assignProperty(this, "haloOpacity", 0);
    m_checkedState->assignProperty(this, "checkedOverlayProgress", 1);
    m_uncheckedState->assignProperty(this, "checkedOverlayProgress", 0);

    m_button->update();
}

void MaterialFlatButtonStateMachine::updateCheckedStatus()
{
    const bool checked = m_button->isChecked();
    if (m_wasChecked != checked) {
        m_wasChecked = checked;
        if (checked) {
            postEvent(new MaterialStateTransitionEvent(FlatButtonCheckedTransition));
        } else {
            postEvent(new MaterialStateTransitionEvent(FlatButtonUncheckedTransition));
        }
    }
}

bool MaterialFlatButtonStateMachine::eventFilter(QObject *watched, QEvent *event)
{
    if (QEvent::FocusIn == event->type()) {
        QFocusEvent *focusEvent = static_cast<QFocusEvent *>(event);
        if (focusEvent && Qt::MouseFocusReason == focusEvent->reason()) {
            postEvent(new MaterialStateTransitionEvent(FlatButtonPressedTransition));
            return true;
        }
    }
    return QStateMachine::eventFilter(watched, event);
}

void MaterialFlatButtonStateMachine::addTransition(QObject *object, QEvent::Type eventType, QState *fromState,
                                                   QState *toState)
{
    addTransition(new QEventTransition(object, eventType), fromState, toState);
}

void MaterialFlatButtonStateMachine::addTransition(QAbstractTransition *transition, QState *fromState, QState *toState)
{
    transition->setTargetState(toState);

    QPropertyAnimation *animation;

    animation = new QPropertyAnimation(this, "overlayOpacity", this);
    animation->setDuration(150);
    transition->addAnimation(animation);

    animation = new QPropertyAnimation(this, "haloOpacity", this);
    animation->setDuration(170);
    transition->addAnimation(animation);

    animation = new QPropertyAnimation(this, "haloSize", this);
    animation->setDuration(350);
    animation->setEasingCurve(QEasingCurve::OutCubic);
    transition->addAnimation(animation);

    fromState->addTransition(transition);
}

/*!
 *  \class MaterialFlatButtonPrivate
 *  \internal
 */

/*!
 *  \internal
 */
MaterialFlatButtonPrivate::MaterialFlatButtonPrivate(MaterialFlatButton *q)
    : q_ptr(q)
{
}

/*!
 *  \internal
 */
MaterialFlatButtonPrivate::~MaterialFlatButtonPrivate()
{
}

/*!
 *  \internal
 */
void MaterialFlatButtonPrivate::init()
{
    Q_Q(MaterialFlatButton);

    rippleOverlay = new MaterialRippleOverlay(q);
    stateMachine = new MaterialFlatButtonStateMachine(q);
    role = QxMaterial::Default;
    rippleStyle = QxMaterial::PositionedRipple;
    iconPlacement = QxMaterial::LeftIcon;
    overlayStyle = QxMaterial::GrayOverlay;
    bgMode = Qt::TransparentMode;
    textAlignment = Qt::AlignHCenter;
    fixedRippleRadius = 64;
    cornerRadius = 3;
    baseOpacity = 0.13;
    fontSize = 10;   // 10.5;
    useThemeColors = true;
    useFixedRippleRadius = false;
    haloVisible = true;

    q->setStyle(&MaterialStyle::instance());
    q->setAttribute(Qt::WA_Hover);
    q->setMouseTracking(true);

    QFont font("Roboto", fontSize, QFont::Medium);
    font.setCapitalization(QFont::AllUppercase);
    q->setFont(font);

    QPainterPath path;
    path.addRoundedRect(q->rect(), cornerRadius, cornerRadius);
    rippleOverlay->setClipPath(path);
    rippleOverlay->setClipping(true);

    stateMachine->setupProperties();
    stateMachine->startAnimations();
}

/*!
 *  \class MaterialFlatButton
 */

MaterialFlatButton::MaterialFlatButton(QWidget *parent, QxMaterial::ButtonPreset preset)
    : QPushButton(parent), d_ptr(new MaterialFlatButtonPrivate(this))
{
    d_func()->init();

    applyPreset(preset);
}

MaterialFlatButton::MaterialFlatButton(const QString &text, QWidget *parent, QxMaterial::ButtonPreset preset)
    : QPushButton(text, parent), d_ptr(new MaterialFlatButtonPrivate(this))
{
    d_func()->init();

    applyPreset(preset);
}

MaterialFlatButton::MaterialFlatButton(const QString &text, QxMaterial::Role role, QWidget *parent,
                                       QxMaterial::ButtonPreset preset)
    : QPushButton(text, parent), d_ptr(new MaterialFlatButtonPrivate(this))
{
    d_func()->init();

    applyPreset(preset);
    setRole(role);
}

MaterialFlatButton::~MaterialFlatButton()
{
}

void MaterialFlatButton::applyPreset(QxMaterial::ButtonPreset preset)
{
    switch (preset) {
    case QxMaterial::FlatPreset:
        setOverlayStyle(QxMaterial::NoOverlay);
        break;
    case QxMaterial::CheckablePreset:
        setOverlayStyle(QxMaterial::NoOverlay);
        setCheckable(true);
        setHaloVisible(false);
        break;
    default:
        break;
    }
}

void MaterialFlatButton::setUseThemeColors(bool value)
{
    Q_D(MaterialFlatButton);

    if (d->useThemeColors == value) {
        return;
    }

    d->useThemeColors = value;
    d->stateMachine->setupProperties();
}

bool MaterialFlatButton::useThemeColors() const
{
    Q_D(const MaterialFlatButton);

    return d->useThemeColors;
}

void MaterialFlatButton::setRole(QxMaterial::Role role)
{
    Q_D(MaterialFlatButton);

    d->role = role;
    d->stateMachine->setupProperties();
}

QxMaterial::Role MaterialFlatButton::role() const
{
    Q_D(const MaterialFlatButton);

    return d->role;
}

void MaterialFlatButton::setForegroundColor(const QColor &color)
{
    Q_D(MaterialFlatButton);

    d->foregroundColor = color;

    MATERIAL_DISABLE_THEME_COLORS
    update();
}

QColor MaterialFlatButton::foregroundColor() const
{
    Q_D(const MaterialFlatButton);

    if (d->useThemeColors || !d->foregroundColor.isValid()) {
        if (Qt::OpaqueMode == d->bgMode) {
            return MaterialStyle::instance().themeColor("canvas");
        }
        switch (d->role) {
        case QxMaterial::Primary:
            return MaterialStyle::instance().themeColor("primary1");
        case QxMaterial::Secondary:
            return MaterialStyle::instance().themeColor("accent1");
        case QxMaterial::Default:
        default:
            return MaterialStyle::instance().themeColor("text");
        }
    }
    return d->foregroundColor;
}

void MaterialFlatButton::setBackgroundColor(const QColor &color)
{
    Q_D(MaterialFlatButton);

    d->backgroundColor = color;

    MATERIAL_DISABLE_THEME_COLORS
    update();
}

QColor MaterialFlatButton::backgroundColor() const
{
    Q_D(const MaterialFlatButton);

    if (d->useThemeColors || !d->backgroundColor.isValid()) {
        switch (d->role) {
        case QxMaterial::Primary:
            return MaterialStyle::instance().themeColor("primary1");
        case QxMaterial::Secondary:
            return MaterialStyle::instance().themeColor("accent1");
        case QxMaterial::Default:
        default:
            return MaterialStyle::instance().themeColor("text");
        }
    }
    return d->backgroundColor;
}

void MaterialFlatButton::setOverlayColor(const QColor &color)
{
    Q_D(MaterialFlatButton);

    d->overlayColor = color;

    MATERIAL_DISABLE_THEME_COLORS

    setOverlayStyle(QxMaterial::TintedOverlay);
    update();
}

QColor MaterialFlatButton::overlayColor() const
{
    Q_D(const MaterialFlatButton);

    if (d->useThemeColors || !d->overlayColor.isValid()) {
        return foregroundColor();
    }
    return d->overlayColor;
}

void MaterialFlatButton::setDisabledForegroundColor(const QColor &color)
{
    Q_D(MaterialFlatButton);

    d->disabledColor = color;

    MATERIAL_DISABLE_THEME_COLORS
    update();
}

QColor MaterialFlatButton::disabledForegroundColor() const
{
    Q_D(const MaterialFlatButton);

    if (d->useThemeColors || !d->disabledColor.isValid()) {
        return MaterialStyle::instance().themeColor("disabled");
    } else {
        return d->disabledColor;
    }
}

void MaterialFlatButton::setDisabledBackgroundColor(const QColor &color)
{
    Q_D(MaterialFlatButton);

    d->disabledBackgroundColor = color;

    MATERIAL_DISABLE_THEME_COLORS
    update();
}

QColor MaterialFlatButton::disabledBackgroundColor() const
{
    Q_D(const MaterialFlatButton);

    if (d->useThemeColors || !d->disabledBackgroundColor.isValid()) {
        return MaterialStyle::instance().themeColor("disabled3");
    } else {
        return d->disabledBackgroundColor;
    }
}

void MaterialFlatButton::setFontSize(qreal size)
{
    Q_D(MaterialFlatButton);

    d->fontSize = size;

    QFont f(font());
    f.setPointSizeF(size);
    setFont(f);

    update();
}

qreal MaterialFlatButton::fontSize() const
{
    Q_D(const MaterialFlatButton);

    return d->fontSize;
}

void MaterialFlatButton::setHaloVisible(bool visible)
{
    Q_D(MaterialFlatButton);

    d->haloVisible = visible;
    update();
}

bool MaterialFlatButton::isHaloVisible() const
{
    Q_D(const MaterialFlatButton);

    return d->haloVisible;
}

void MaterialFlatButton::setOverlayStyle(QxMaterial::OverlayStyle style)
{
    Q_D(MaterialFlatButton);

    d->overlayStyle = style;
    update();
}

QxMaterial::OverlayStyle MaterialFlatButton::overlayStyle() const
{
    Q_D(const MaterialFlatButton);

    return d->overlayStyle;
}

void MaterialFlatButton::setRippleStyle(QxMaterial::RippleStyle style)
{
    Q_D(MaterialFlatButton);

    d->rippleStyle = style;
}

QxMaterial::RippleStyle MaterialFlatButton::rippleStyle() const
{
    Q_D(const MaterialFlatButton);

    return d->rippleStyle;
}

void MaterialFlatButton::setIconPlacement(QxMaterial::ButtonIconPlacement placement)
{
    Q_D(MaterialFlatButton);

    d->iconPlacement = placement;
    update();
}

QxMaterial::ButtonIconPlacement MaterialFlatButton::iconPlacement() const
{
    Q_D(const MaterialFlatButton);

    return d->iconPlacement;
}

void MaterialFlatButton::setCornerRadius(qreal radius)
{
    Q_D(MaterialFlatButton);

    d->cornerRadius = radius;
    updateClipPath();
    update();
}

qreal MaterialFlatButton::cornerRadius() const
{
    Q_D(const MaterialFlatButton);

    return d->cornerRadius;
}

void MaterialFlatButton::setBackgroundMode(Qt::BGMode mode)
{
    Q_D(MaterialFlatButton);

    d->bgMode = mode;
    d->stateMachine->setupProperties();
}

Qt::BGMode MaterialFlatButton::backgroundMode() const
{
    Q_D(const MaterialFlatButton);

    return d->bgMode;
}

void MaterialFlatButton::setBaseOpacity(qreal opacity)
{
    Q_D(MaterialFlatButton);

    d->baseOpacity = opacity;
    d->stateMachine->setupProperties();
}

qreal MaterialFlatButton::baseOpacity() const
{
    Q_D(const MaterialFlatButton);

    return d->baseOpacity;
}

void MaterialFlatButton::setCheckable(bool value)
{
    Q_D(MaterialFlatButton);

    d->stateMachine->updateCheckedStatus();

    QPushButton::setCheckable(value);
}

void MaterialFlatButton::setHasFixedRippleRadius(bool value)
{
    Q_D(MaterialFlatButton);

    d->useFixedRippleRadius = value;
}

bool MaterialFlatButton::hasFixedRippleRadius() const
{
    Q_D(const MaterialFlatButton);

    return d->useFixedRippleRadius;
}

void MaterialFlatButton::setFixedRippleRadius(qreal radius)
{
    Q_D(MaterialFlatButton);

    d->fixedRippleRadius = radius;
    setHasFixedRippleRadius(true);
}

void MaterialFlatButton::setTextAlignment(Qt::Alignment alignment)
{
    Q_D(MaterialFlatButton);

    d->textAlignment = alignment;
}

Qt::Alignment MaterialFlatButton::textAlignment() const
{
    Q_D(const MaterialFlatButton);

    return d->textAlignment;
}

/*!
 *  \reimp
 */
QSize MaterialFlatButton::sizeHint() const
{
    ensurePolished();

    QSize label(fontMetrics().size(Qt::TextSingleLine, text()));

    int w = 20 + label.width();
    int h = label.height();
    if (!icon().isNull()) {
        w += iconSize().width() + MaterialFlatButton::IconPadding;
        h = qMax(h, iconSize().height());
    }
    return QSize(w, 20 + h);
}

MaterialFlatButton::MaterialFlatButton(MaterialFlatButtonPrivate &d, QWidget *parent, QxMaterial::ButtonPreset preset)
    : QPushButton(parent), d_ptr(&d)
{
    d_func()->init();

    applyPreset(preset);
}

/*!
 *  \reimp
 */
void MaterialFlatButton::checkStateSet()
{
    Q_D(MaterialFlatButton);

    d->stateMachine->updateCheckedStatus();

    QPushButton::checkStateSet();
}

/*!
 *  \reimp
 */
void MaterialFlatButton::mousePressEvent(QMouseEvent *event)
{
    Q_D(MaterialFlatButton);

    if (QxMaterial::NoRipple != d->rippleStyle) {
        QPoint pos;
        qreal radiusEndValue;

        if (QxMaterial::CenteredRipple == d->rippleStyle) {
            pos = rect().center();
        } else {
            pos = event->pos();
        }

        if (d->useFixedRippleRadius) {
            radiusEndValue = d->fixedRippleRadius;
        } else {
            radiusEndValue = static_cast<qreal>(width()) / 2;
        }

        MaterialRipple *ripple = new MaterialRipple(pos);

        ripple->setRadiusEndValue(radiusEndValue);
        ripple->setOpacityStartValue(0.35);
        ripple->setColor(foregroundColor());
        ripple->radiusAnimation()->setDuration(600);
        ripple->opacityAnimation()->setDuration(1300);

        d->rippleOverlay->addRipple(ripple);
    }

    QPushButton::mousePressEvent(event);
}

/*!
 *  \reimp
 */
void MaterialFlatButton::mouseReleaseEvent(QMouseEvent *event)
{
    Q_D(MaterialFlatButton);

    QPushButton::mouseReleaseEvent(event);

    d->stateMachine->updateCheckedStatus();
}

void MaterialFlatButton::resizeEvent(QResizeEvent *event)
{
    QPushButton::resizeEvent(event);

    updateClipPath();
}

/*!
 *  \reimp
 */
void MaterialFlatButton::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event)

    Q_D(MaterialFlatButton);

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    const qreal cr = d->cornerRadius;

    if (cr > 0) {
        QPainterPath path;
        path.addRoundedRect(rect(), cr, cr);

        painter.setClipPath(path);
        painter.setClipping(true);
    }

    paintBackground(&painter);
    paintHalo(&painter);

    painter.setOpacity(1);
    painter.setClipping(false);

    paintForeground(&painter);
}

/*!
 *  \internal
 */
void MaterialFlatButton::paintBackground(QPainter *painter)
{
    Q_D(MaterialFlatButton);

    const qreal overlayOpacity = d->stateMachine->overlayOpacity();
    const qreal checkedProgress = d->stateMachine->checkedOverlayProgress();

    if (Qt::OpaqueMode == d->bgMode) {
        QBrush brush;
        brush.setStyle(Qt::SolidPattern);
        if (isEnabled()) {
            brush.setColor(backgroundColor());
        } else {
            brush.setColor(disabledBackgroundColor());
        }
        painter->setOpacity(1);
        painter->setBrush(brush);
        painter->setPen(Qt::NoPen);
        painter->drawRect(rect());
    }

    QBrush brush;
    brush.setStyle(Qt::SolidPattern);
    painter->setPen(Qt::NoPen);

    if (!isEnabled()) {
        return;
    }

    if ((QxMaterial::NoOverlay != d->overlayStyle) && (overlayOpacity > 0)) {
        if (QxMaterial::TintedOverlay == d->overlayStyle) {
            brush.setColor(overlayColor());
        } else {
            brush.setColor(Qt::gray);
        }
        painter->setOpacity(overlayOpacity);
        painter->setBrush(brush);
        painter->drawRect(rect());
    }

    if (isCheckable() && checkedProgress > 0) {
        const qreal q = Qt::TransparentMode == d->bgMode ? 0.45 : 0.7;
        brush.setColor(foregroundColor());
        painter->setOpacity(q * checkedProgress);
        painter->setBrush(brush);
        QRect r(rect());
        r.setHeight(static_cast<qreal>(r.height()) * checkedProgress);
        painter->drawRect(r);
    }
}

/*!
 *  \internal
 */
void MaterialFlatButton::paintHalo(QPainter *painter)
{
    Q_D(MaterialFlatButton);

    if (!d->haloVisible) {
        return;
    }

    const qreal opacity = d->stateMachine->haloOpacity();
    const qreal s = d->stateMachine->haloScaleFactor() * d->stateMachine->haloSize();
    const qreal radius = static_cast<qreal>(width()) * s;

    if (isEnabled() && opacity > 0) {
        QBrush brush;
        brush.setStyle(Qt::SolidPattern);
        brush.setColor(foregroundColor());
        painter->setOpacity(opacity);
        painter->setBrush(brush);
        painter->setPen(Qt::NoPen);
        const QPointF center = rect().center();
        painter->drawEllipse(center, radius, radius);
    }
}

#define COLOR_INTERPOLATE(CH) (1 - progress) * source.CH() + progress *dest.CH()

/*!
 *  \internal
 */
void MaterialFlatButton::paintForeground(QPainter *painter)
{
    Q_D(MaterialFlatButton);

    if (isEnabled()) {
        painter->setPen(foregroundColor());
        const qreal progress = d->stateMachine->checkedOverlayProgress();
        if (isCheckable() && progress > 0) {
            QColor source = foregroundColor();
            QColor dest = Qt::TransparentMode == d->bgMode ? Qt::white : backgroundColor();
            if (qFuzzyCompare(1, progress)) {
                painter->setPen(dest);
            } else {
                painter->setPen(QColor(COLOR_INTERPOLATE(red), COLOR_INTERPOLATE(green), COLOR_INTERPOLATE(blue),
                                       COLOR_INTERPOLATE(alpha)));
            }
        }
    } else {
        painter->setPen(disabledForegroundColor());
    }

    if (icon().isNull()) {
        if (Qt::AlignLeft == d->textAlignment) {
            painter->drawText(rect().adjusted(12, 0, 0, 0), Qt::AlignLeft | Qt::AlignVCenter, text());
        } else {
            painter->drawText(rect(), Qt::AlignCenter, text());
        }
        return;
    }

    QSize textSize(fontMetrics().size(Qt::TextSingleLine, text()));
    QSize base(size() - textSize);

    const int iw = iconSize().width() + IconPadding;
    QPoint pos(Qt::AlignLeft == d->textAlignment ? 12 : (base.width() - iw) / 2, 0);

    QRect textGeometry(pos + QPoint(0, base.height() / 2), textSize);
    QRect iconGeometry(pos + QPoint(0, (height() - iconSize().height()) / 2), iconSize());

    if (QxMaterial::LeftIcon == d->iconPlacement) {
        textGeometry.translate(iw, 0);
    } else {
        iconGeometry.translate(textSize.width() + IconPadding, 0);
    }

    painter->drawText(textGeometry, Qt::AlignCenter, text());

    QPixmap pixmap = icon().pixmap(iconSize());
    QPainter icon(&pixmap);
    icon.setCompositionMode(QPainter::CompositionMode_SourceIn);
    icon.fillRect(pixmap.rect(), painter->pen().color());
    painter->drawPixmap(iconGeometry, pixmap);
}

/*!
 *  \internal
 */
void MaterialFlatButton::updateClipPath()
{
    Q_D(MaterialFlatButton);

    const qreal radius = d->cornerRadius;

    QPainterPath path;
    path.addRoundedRect(rect(), radius, radius);
    d->rippleOverlay->setClipPath(path);
}

QX_MATERIAL_END_NAMESPACE
