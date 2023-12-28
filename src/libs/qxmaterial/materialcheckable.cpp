/**
 * Copyleft (C) 2023 maminjie <canpool@163.com>
 * SPDX-License-Identifier: BSD-3-Clause
 **/
#include "materialcheckable.h"
#include "materialcheckable_p.h"
#include "materialripple.h"
#include "materialrippleoverlay.h"
#include "materialstyle.h"
#include <QColor>
#include <QCoreApplication>
#include <QEvent>
#include <QEventTransition>
#include <QPainter>
#include <QSignalTransition>
#include <QStateMachine>
#include <QIcon>
#include <QTransform>

QX_MATERIAL_BEGIN_NAMESPACE

/*!
 *  \class MaterialCheckableIcon
 *  \internal
 */

MaterialCheckableIcon::MaterialCheckableIcon(const QIcon &icon, MaterialCheckable *parent)
    : QWidget(parent)
    , m_checkable(parent)
    , m_color(Qt::black)
    , m_icon(icon)
    , m_iconSize(24)
    , m_opacity(1.0)
{
    Q_ASSERT(parent);

    setAttribute(Qt::WA_TransparentForMouseEvents);
}

MaterialCheckableIcon::~MaterialCheckableIcon()
{
}

QSize MaterialCheckableIcon::sizeHint() const
{
    return QSize(m_iconSize, m_iconSize);
}

void MaterialCheckableIcon::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event)

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setOpacity(m_opacity);

    QPixmap pixmap = icon().pixmap(24, 24);

    if (!pixmap.isNull()) {
        const qreal p = static_cast<qreal>((height()) - m_iconSize) / 2;
        const qreal z = m_iconSize / 24;

        QTransform t;
        if (MaterialCheckable::LabelPositionLeft == m_checkable->labelPosition()) {
            t.translate(p + width() - 42, p);
        } else {
            t.translate(p, p);
        }
        t.scale(z, z);
        painter.setTransform(t);

        QPainter icon(&pixmap);
        icon.setCompositionMode(QPainter::CompositionMode_SourceIn);
        icon.fillRect(pixmap.rect(), color());
        painter.drawPixmap(0, 0, pixmap);
    }
}

/*!
 *  \class MaterialCheckablePrivate
 *  \internal
 */

MaterialCheckablePrivate::MaterialCheckablePrivate(MaterialCheckable *q)
    : q_ptr(q)
{
}

MaterialCheckablePrivate::~MaterialCheckablePrivate()
{
}

void MaterialCheckablePrivate::init()
{
    Q_Q(MaterialCheckable);

    rippleOverlay = new MaterialRippleOverlay;
    checkedIcon = new MaterialCheckableIcon(QIcon(":/res/icons/toggle/svg/production/ic_check_box_24px.svg"), q);
    uncheckedIcon =
        new MaterialCheckableIcon(QIcon(":/res/icons/toggle/svg/production/ic_check_box_outline_blank_24px.svg"), q);
    stateMachine = new QStateMachine(q);
    uncheckedState = new QState;
    checkedState = new QState;
    disabledUncheckedState = new QState;
    disabledCheckedState = new QState;
    uncheckedTransition = new QSignalTransition(q, SIGNAL(toggled(bool)));
    checkedTransition = new QSignalTransition(q, SIGNAL(toggled(bool)));
    labelPosition = MaterialCheckable::LabelPositionRight;
    useThemeColors = true;

    rippleOverlay->setParent(q->parentWidget());
    rippleOverlay->installEventFilter(q);

    q->setCheckable(true);
    q->setStyle(&MaterialStyle::instance());
    q->setFont(QFont("Roboto", 11, QFont::Normal));

    stateMachine->addState(uncheckedState);
    stateMachine->addState(checkedState);
    stateMachine->addState(disabledUncheckedState);
    stateMachine->addState(disabledCheckedState);
    stateMachine->setInitialState(uncheckedState);

    // Transition to checked

    uncheckedTransition->setTargetState(checkedState);
    uncheckedState->addTransition(uncheckedTransition);

    // Transition to unchecked

    checkedTransition->setTargetState(uncheckedState);
    checkedState->addTransition(checkedTransition);

    QAbstractTransition *transition;

    // Transitions enabled <==> disabled

    transition = new QEventTransition(q, QEvent::EnabledChange);
    transition->setTargetState(disabledUncheckedState);
    uncheckedState->addTransition(transition);

    transition = new QEventTransition(q, QEvent::EnabledChange);
    transition->setTargetState(uncheckedState);
    disabledUncheckedState->addTransition(transition);

    transition = new QEventTransition(q, QEvent::EnabledChange);
    transition->setTargetState(disabledCheckedState);
    checkedState->addTransition(transition);

    transition = new QEventTransition(q, QEvent::EnabledChange);
    transition->setTargetState(checkedState);
    disabledCheckedState->addTransition(transition);

    transition = new QSignalTransition(q, SIGNAL(toggled(bool)));
    transition->setTargetState(disabledCheckedState);
    disabledUncheckedState->addTransition(transition);

    transition = new QSignalTransition(q, SIGNAL(toggled(bool)));
    transition->setTargetState(disabledUncheckedState);
    disabledCheckedState->addTransition(transition);

    //

    checkedState->assignProperty(checkedIcon, "opacity", 1);
    checkedState->assignProperty(uncheckedIcon, "opacity", 0);

    uncheckedState->assignProperty(checkedIcon, "opacity", 0);
    uncheckedState->assignProperty(uncheckedIcon, "opacity", 1);

    disabledCheckedState->assignProperty(checkedIcon, "opacity", 1);
    disabledCheckedState->assignProperty(uncheckedIcon, "opacity", 0);

    disabledUncheckedState->assignProperty(checkedIcon, "opacity", 0);
    disabledUncheckedState->assignProperty(uncheckedIcon, "opacity", 1);

    checkedState->assignProperty(checkedIcon, "color", q->checkedColor());
    checkedState->assignProperty(uncheckedIcon, "color", q->checkedColor());

    uncheckedState->assignProperty(uncheckedIcon, "color", q->uncheckedColor());
    uncheckedState->assignProperty(uncheckedIcon, "color", q->uncheckedColor());

    disabledUncheckedState->assignProperty(uncheckedIcon, "color", q->disabledColor());
    disabledCheckedState->assignProperty(checkedIcon, "color", q->disabledColor());

    stateMachine->start();
    QCoreApplication::processEvents();
}

/*!
 *  \class MaterialCheckable
 */

MaterialCheckable::MaterialCheckable(QWidget *parent)
    : QAbstractButton(parent), d_ptr(new MaterialCheckablePrivate(this))
{
    d_func()->init();
}

MaterialCheckable::~MaterialCheckable()
{
}

void MaterialCheckable::setLabelPosition(LabelPosition placement)
{
    Q_D(MaterialCheckable);

    d->labelPosition = placement;
    update();
}

MaterialCheckable::LabelPosition MaterialCheckable::labelPosition() const
{
    Q_D(const MaterialCheckable);

    return d->labelPosition;
}

void MaterialCheckable::setUseThemeColors(bool value)
{
    Q_D(MaterialCheckable);

    if (d->useThemeColors == value) {
        return;
    }

    d->useThemeColors = value;
    setupProperties();
}

bool MaterialCheckable::useThemeColors() const
{
    Q_D(const MaterialCheckable);

    return d->useThemeColors;
}

void MaterialCheckable::setCheckedColor(const QColor &color)
{
    Q_D(MaterialCheckable);

    d->checkedColor = color;

    MATERIAL_DISABLE_THEME_COLORS
    setupProperties();
}

QColor MaterialCheckable::checkedColor() const
{
    Q_D(const MaterialCheckable);

    if (d->useThemeColors || !d->checkedColor.isValid()) {
        return MaterialStyle::instance().themeColor(MK_primary1);
    } else {
        return d->checkedColor;
    }
}

void MaterialCheckable::setUncheckedColor(const QColor &color)
{
    Q_D(MaterialCheckable);

    d->uncheckedColor = color;

    MATERIAL_DISABLE_THEME_COLORS
    setupProperties();
}

QColor MaterialCheckable::uncheckedColor() const
{
    Q_D(const MaterialCheckable);

    if (d->useThemeColors || !d->uncheckedColor.isValid()) {
        return MaterialStyle::instance().themeColor(MK_text);
    } else {
        return d->uncheckedColor;
    }
}

void MaterialCheckable::setTextColor(const QColor &color)
{
    Q_D(MaterialCheckable);

    d->textColor = color;

    MATERIAL_DISABLE_THEME_COLORS
    setupProperties();
}

QColor MaterialCheckable::textColor() const
{
    Q_D(const MaterialCheckable);

    if (d->useThemeColors || !d->textColor.isValid()) {
        return MaterialStyle::instance().themeColor(MK_text);
    } else {
        return d->textColor;
    }
}

void MaterialCheckable::setDisabledColor(const QColor &color)
{
    Q_D(MaterialCheckable);

    d->disabledColor = color;

    MATERIAL_DISABLE_THEME_COLORS
    setupProperties();
}

QColor MaterialCheckable::disabledColor() const
{
    Q_D(const MaterialCheckable);

    if (d->useThemeColors || !d->disabledColor.isValid()) {
        return MaterialStyle::instance().themeColor(MK_accent3);
    } else {
        return d->disabledColor;
    }
}

void MaterialCheckable::setCheckedIcon(const QIcon &icon)
{
    Q_D(MaterialCheckable);

    d->checkedIcon->setIcon(icon);
    update();
}

QIcon MaterialCheckable::checkedIcon() const
{
    Q_D(const MaterialCheckable);

    return d->checkedIcon->icon();
}

void MaterialCheckable::setUncheckedIcon(const QIcon &icon)
{
    Q_D(MaterialCheckable);

    d->uncheckedIcon->setIcon(icon);
    update();
}

QIcon MaterialCheckable::uncheckedIcon() const
{
    Q_D(const MaterialCheckable);

    return d->uncheckedIcon->icon();
}

/*!
 *  \reimp
 */
QSize MaterialCheckable::sizeHint() const
{
    if (text().isEmpty()) {
        return QSize(40, 40);
    }
    return QSize(fontMetrics().size(Qt::TextShowMnemonic, text()).width() + 52, 40);
}

MaterialCheckable::MaterialCheckable(MaterialCheckablePrivate &d, QWidget *parent) : QAbstractButton(parent), d_ptr(&d)
{
    d_func()->init();
}

/*!
 *  \reimp
 */
bool MaterialCheckable::event(QEvent *event)
{
    Q_D(MaterialCheckable);

    switch (event->type()) {
    case QEvent::Resize:
    case QEvent::Move:
        d->checkedIcon->setGeometry(rect());
        d->uncheckedIcon->setGeometry(rect());
        d->rippleOverlay->setGeometry(geometry().adjusted(-8, -8, 8, 8));
        break;
    case QEvent::ParentChange:
        QWidget *widget;
        if ((widget = parentWidget())) {
            d->rippleOverlay->setParent(widget);
        }
        break;
    default:
        break;
    }
    return QAbstractButton::event(event);
}

/*!
 *  \reimp
 */
bool MaterialCheckable::eventFilter(QObject *obj, QEvent *event)
{
    if (QEvent::Resize == event->type()) {
        Q_D(MaterialCheckable);

        d->rippleOverlay->setGeometry(geometry().adjusted(-8, -8, 8, 8));
    }
    return QAbstractButton::eventFilter(obj, event);
}

/*!
 *  \reimp
 */
void MaterialCheckable::mousePressEvent(QMouseEvent *event)
{
    Q_UNUSED(event)

    Q_D(MaterialCheckable);

    if (!isEnabled()) {
        return;
    }

    MaterialRipple *ripple;
    if (MaterialCheckable::LabelPositionLeft == d->labelPosition) {
        ripple = new MaterialRipple(QPoint(width() - 14, 28));
    } else {
        ripple = new MaterialRipple(QPoint(28, 28));
    }
    ripple->setRadiusEndValue(22);
    ripple->setColor(isChecked() ? checkedColor() : uncheckedColor());
    if (isChecked()) {
        ripple->setOpacityStartValue(1);
    }
    d->rippleOverlay->addRipple(ripple);

    setChecked(!isChecked());
}

/*!
 *  \reimp
 */
void MaterialCheckable::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event)

    Q_D(MaterialCheckable);

    QPainter painter(this);

    QPen pen;
    pen.setColor(isEnabled() ? textColor() : disabledColor());
    painter.setPen(pen);

    if (MaterialCheckable::LabelPositionLeft == d->labelPosition) {
        painter.drawText(4, 25, text());
    } else {
        painter.drawText(48, 25, text());
    }
}

void MaterialCheckable::setupProperties()
{
    Q_D(MaterialCheckable);

    d->checkedState->assignProperty(d->checkedIcon, "color", checkedColor());
    d->checkedState->assignProperty(d->uncheckedIcon, "color", checkedColor());
    d->uncheckedState->assignProperty(d->uncheckedIcon, "color", uncheckedColor());
    d->disabledUncheckedState->assignProperty(d->uncheckedIcon, "color", disabledColor());
    d->disabledCheckedState->assignProperty(d->checkedIcon, "color", disabledColor());

    if (isEnabled()) {
        if (isChecked()) {
            d->checkedIcon->setColor(checkedColor());
        } else {
            d->uncheckedIcon->setColor(uncheckedColor());
        }
    } else {
        d->checkedIcon->setColor(disabledColor());
        d->uncheckedIcon->setColor(disabledColor());
    }

    update();
}

QX_MATERIAL_END_NAMESPACE
