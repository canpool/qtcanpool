/**
 * Copyleft (C) 2023 maminjie <canpool@163.com>
 * SPDX-License-Identifier: BSD-3-Clause
 **/
#include "materialtextfield.h"
#include "materialtextfield_p.h"
#include "materialstyle.h"
#include <QDebug>
#include <QPainter>
#include <QtWidgets/QApplication>
#include <QEventTransition>
#include <QPropertyAnimation>

QX_MATERIAL_BEGIN_NAMESPACE

/*!
 *  \class MaterialTextFieldStateMachine
 *  \internal
 */

MaterialTextFieldStateMachine::MaterialTextFieldStateMachine(MaterialTextField *parent)
    : QStateMachine(parent)
    , m_textField(parent)
    , m_normalState(new QState)
    , m_focusedState(new QState)
    , m_label(0)
    , m_offsetAnimation(0)
    , m_colorAnimation(0)
    , m_progress(0.0)
{
    Q_ASSERT(parent);

    addState(m_normalState);
    addState(m_focusedState);

    setInitialState(m_normalState);

    QEventTransition *transition;
    QPropertyAnimation *animation;

    transition = new QEventTransition(parent, QEvent::FocusIn);
    transition->setTargetState(m_focusedState);
    m_normalState->addTransition(transition);

    animation = new QPropertyAnimation(this, "progress", this);
    animation->setEasingCurve(QEasingCurve::InCubic);
    animation->setDuration(310);
    transition->addAnimation(animation);

    transition = new QEventTransition(parent, QEvent::FocusOut);
    transition->setTargetState(m_normalState);
    m_focusedState->addTransition(transition);

    animation = new QPropertyAnimation(this, "progress", this);
    animation->setEasingCurve(QEasingCurve::OutCubic);
    animation->setDuration(310);
    transition->addAnimation(animation);

    m_normalState->assignProperty(this, "progress", 0);
    m_focusedState->assignProperty(this, "progress", 1);

    setupProperties();

    connect(m_textField, SIGNAL(textChanged(QString)), this, SLOT(setupProperties()));
}

MaterialTextFieldStateMachine::~MaterialTextFieldStateMachine()
{
}

void MaterialTextFieldStateMachine::setLabel(MaterialTextFieldLabel *label)
{
    if (m_label) {
        delete m_label;
    }

    if (m_offsetAnimation) {
        removeDefaultAnimation(m_offsetAnimation);
        delete m_offsetAnimation;
    }

    if (m_colorAnimation) {
        removeDefaultAnimation(m_colorAnimation);
        delete m_colorAnimation;
    }

    m_label = label;

    if (m_label) {
        m_offsetAnimation = new QPropertyAnimation(m_label, "offset", this);
        m_offsetAnimation->setDuration(210);
        m_offsetAnimation->setEasingCurve(QEasingCurve::OutCubic);
        addDefaultAnimation(m_offsetAnimation);

        m_colorAnimation = new QPropertyAnimation(m_label, "color", this);
        m_colorAnimation->setDuration(210);
        addDefaultAnimation(m_colorAnimation);
    }

    setupProperties();
}

void MaterialTextFieldStateMachine::setupProperties()
{
    if (m_label) {
        const int m = m_textField->textMargins().top();

        if (m_textField->text().isEmpty()) {
            m_normalState->assignProperty(m_label, "offset", QPointF(0, 26));
        } else {
            m_normalState->assignProperty(m_label, "offset", QPointF(0, 0 - m));
        }

        m_focusedState->assignProperty(m_label, "offset", QPointF(0, 0 - m));
        m_focusedState->assignProperty(m_label, "color", m_textField->inkColor());
        m_normalState->assignProperty(m_label, "color", m_textField->labelColor());

        if (0 != m_label->offset().y() && !m_textField->text().isEmpty()) {
            m_label->setOffset(QPointF(0, 0 - m));
        } else if (!m_textField->hasFocus() && m_label->offset().y() <= 0 && m_textField->text().isEmpty()) {
            m_label->setOffset(QPointF(0, 26));
        }
    }

    m_textField->update();
}

/*!
 *  \class MaterialTextFieldLabel
 *  \internal
 */

MaterialTextFieldLabel::MaterialTextFieldLabel(MaterialTextField *parent)
    : QWidget(parent), m_textField(parent), m_scale(1), m_posX(0), m_posY(26), m_color(parent->labelColor())
{
    Q_ASSERT(parent);

    QFont font("Roboto", parent->labelFontSize(), QFont::Medium);
    font.setLetterSpacing(QFont::PercentageSpacing, 102);
    setFont(font);
}

MaterialTextFieldLabel::~MaterialTextFieldLabel()
{
}

/*!
 *  \reimp
 */
void MaterialTextFieldLabel::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event)

    if (!m_textField->hasLabel()) {
        return;
    }

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.scale(m_scale, m_scale);
    painter.setPen(m_color);
    painter.setOpacity(1);

    QPointF pos(2 + m_posX, height() - 36 + m_posY);
    painter.drawText(pos.x(), pos.y(), m_textField->label());
}

/*!
 *  \class MaterialTextFieldPrivate
 *  \internal
 */

MaterialTextFieldPrivate::MaterialTextFieldPrivate(MaterialTextField *q)
    : q_ptr(q)
{
}

MaterialTextFieldPrivate::~MaterialTextFieldPrivate()
{
}

void MaterialTextFieldPrivate::init()
{
    Q_Q(MaterialTextField);

    stateMachine = new MaterialTextFieldStateMachine(q);
    label = 0;
    labelFontSize = 9.5;
    showLabel = false;
    showInputLine = true;
    useThemeColors = true;

    q->setFrame(false);
    q->setStyle(&MaterialStyle::instance());
    q->setAttribute(Qt::WA_Hover);
    q->setMouseTracking(true);
    q->setTextMargins(0, 2, 0, 4);

    q->setFont(QFont("Roboto", 11, QFont::Normal));

    stateMachine->start();
    QCoreApplication::processEvents();
}

/*!
 *  \class MaterialTextField
 */

MaterialTextField::MaterialTextField(QWidget *parent) : QLineEdit(parent), d_ptr(new MaterialTextFieldPrivate(this))
{
    d_func()->init();
}

MaterialTextField::~MaterialTextField()
{
}

void MaterialTextField::setUseThemeColors(bool value)
{
    Q_D(MaterialTextField);

    if (d->useThemeColors == value) {
        return;
    }

    d->useThemeColors = value;
    d->stateMachine->setupProperties();
}

bool MaterialTextField::useThemeColors() const
{
    Q_D(const MaterialTextField);

    return d->useThemeColors;
}

void MaterialTextField::setShowLabel(bool value)
{
    Q_D(MaterialTextField);

    if (d->showLabel == value) {
        return;
    }

    d->showLabel = value;

    if (!d->label && value) {
        d->label = new MaterialTextFieldLabel(this);
        d->stateMachine->setLabel(d->label);
    }

    if (value) {
        setContentsMargins(0, 23, 0, 0);
    } else {
        setContentsMargins(0, 0, 0, 0);
    }
}

bool MaterialTextField::hasLabel() const
{
    Q_D(const MaterialTextField);

    return d->showLabel;
}

void MaterialTextField::setLabelFontSize(qreal size)
{
    Q_D(MaterialTextField);

    d->labelFontSize = size;

    if (d->label) {
        QFont font(d->label->font());
        font.setPointSizeF(size);
        d->label->setFont(font);
        d->label->update();
    }
}

qreal MaterialTextField::labelFontSize() const
{
    Q_D(const MaterialTextField);

    return d->labelFontSize;
}

void MaterialTextField::setLabel(const QString &label)
{
    Q_D(MaterialTextField);

    d->labelString = label;
    setShowLabel(true);
    d->label->update();
}

QString MaterialTextField::label() const
{
    Q_D(const MaterialTextField);

    return d->labelString;
}

void MaterialTextField::setTextColor(const QColor &color)
{
    Q_D(MaterialTextField);

    d->textColor = color;
    setStyleSheet(QString("QLineEdit { color: %1; }").arg(color.name()));

    MATERIAL_DISABLE_THEME_COLORS
    d->stateMachine->setupProperties();
}

QColor MaterialTextField::textColor() const
{
    Q_D(const MaterialTextField);

    if (d->useThemeColors || !d->textColor.isValid()) {
        return MaterialStyle::instance().themeColor(MK_text);
    } else {
        return d->textColor;
    }
}

void MaterialTextField::setLabelColor(const QColor &color)
{
    Q_D(MaterialTextField);

    d->labelColor = color;

    MATERIAL_DISABLE_THEME_COLORS
    d->stateMachine->setupProperties();
}

QColor MaterialTextField::labelColor() const
{
    Q_D(const MaterialTextField);

    if (d->useThemeColors || !d->labelColor.isValid()) {
        return MaterialStyle::instance().themeColor(MK_accent3);
    } else {
        return d->labelColor;
    }
}

void MaterialTextField::setInkColor(const QColor &color)
{
    Q_D(MaterialTextField);

    d->inkColor = color;

    MATERIAL_DISABLE_THEME_COLORS
    d->stateMachine->setupProperties();
}

QColor MaterialTextField::inkColor() const
{
    Q_D(const MaterialTextField);

    if (d->useThemeColors || !d->inkColor.isValid()) {
        return MaterialStyle::instance().themeColor(MK_primary1);
    } else {
        return d->inkColor;
    }
}

void MaterialTextField::setInputLineColor(const QColor &color)
{
    Q_D(MaterialTextField);

    d->inputLineColor = color;

    MATERIAL_DISABLE_THEME_COLORS
    d->stateMachine->setupProperties();
}

QColor MaterialTextField::inputLineColor() const
{
    Q_D(const MaterialTextField);

    if (d->useThemeColors || !d->inputLineColor.isValid()) {
        return MaterialStyle::instance().themeColor(MK_border);
    } else {
        return d->inputLineColor;
    }
}

void MaterialTextField::setShowInputLine(bool value)
{
    Q_D(MaterialTextField);

    if (d->showInputLine == value) {
        return;
    }

    d->showInputLine = value;
    update();
}

bool MaterialTextField::hasInputLine() const
{
    Q_D(const MaterialTextField);

    return d->showInputLine;
}

MaterialTextField::MaterialTextField(MaterialTextFieldPrivate &d, QWidget *parent) : QLineEdit(parent), d_ptr(&d)
{
    d_func()->init();
}

/*!
 *  \reimp
 */
bool MaterialTextField::event(QEvent *event)
{
    Q_D(MaterialTextField);

    switch (event->type()) {
    case QEvent::Resize:
    case QEvent::Move: {
        if (d->label) {
            d->label->setGeometry(rect());
        }
    }
    default:
        break;
    }
    return QLineEdit::event(event);
}

/*!
 *  \reimp
 */
void MaterialTextField::paintEvent(QPaintEvent *event)
{
    Q_D(MaterialTextField);

    QLineEdit::paintEvent(event);

    QPainter painter(this);

    const qreal progress = d->stateMachine->progress();

    if (text().isEmpty() && progress < 1) {
        painter.setOpacity(1 - progress);
        painter.fillRect(rect(), parentWidget()->palette().color(backgroundRole()));
    }

    const int y = height() - 1;
    const int wd = width() - 5;

    if (d->showInputLine) {
        QPen pen;
        pen.setWidth(1);
        pen.setColor(inputLineColor());

        if (!isEnabled())
            pen.setStyle(Qt::DashLine);

        painter.setPen(pen);
        painter.setOpacity(1);
        painter.drawLine(QLineF(2.5, y, wd, y));

        QBrush brush;
        brush.setStyle(Qt::SolidPattern);
        brush.setColor(inkColor());

        if (progress > 0) {
            painter.setPen(Qt::NoPen);
            painter.setBrush(brush);
            const int w = (1 - progress) * static_cast<qreal>(wd / 2);
            painter.drawRect(w + 2.5, height() - 2, wd - w * 2, 2);
        }
    }
}

QX_MATERIAL_END_NAMESPACE
