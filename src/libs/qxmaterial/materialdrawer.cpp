/**
 * Copyleft (C) 2023 maminjie <canpool@163.com>
 * SPDX-License-Identifier: BSD-3-Clause
 **/
#include "materialdrawer.h"
#include "materialdrawer_p.h"
#include <QDebug>
#include <QEvent>
#include <QLinearGradient>
#include <QMouseEvent>
#include <QPainter>
#include <QtWidgets/QApplication>
#include <QtWidgets/QLayout>
#include <QtWidgets/QVBoxLayout>
#include <QPropertyAnimation>
#include <QSignalTransition>
#include <QState>

QX_MATERIAL_BEGIN_NAMESPACE

/*!
 *  \class MaterialDrawerStateMachine
 *  \internal
 */

MaterialDrawerStateMachine::MaterialDrawerStateMachine(MaterialDrawerWidget *drawer, MaterialDrawer *parent)
    : QStateMachine(parent)
    , m_drawer(drawer)
    , m_main(parent)
    , m_openingState(new QState)
    , m_openedState(new QState)
    , m_closingState(new QState)
    , m_closedState(new QState)
    , m_opacity(0)
{
    addState(m_openingState);
    addState(m_openedState);
    addState(m_closingState);
    addState(m_closedState);

    setInitialState(m_closedState);

    QSignalTransition *transition;
    QPropertyAnimation *animation;

    transition = new QSignalTransition(this, SIGNAL(signalOpen()));
    transition->setTargetState(m_openingState);
    m_closedState->addTransition(transition);

    animation = new QPropertyAnimation(drawer, "offset", this);
    animation->setDuration(220);
    animation->setEasingCurve(QEasingCurve::OutCirc);
    transition->addAnimation(animation);

    animation = new QPropertyAnimation(this, "opacity", this);
    animation->setDuration(220);
    transition->addAnimation(animation);

    transition = new QSignalTransition(animation, SIGNAL(finished()));
    transition->setTargetState(m_openedState);
    m_openingState->addTransition(transition);

    transition = new QSignalTransition(this, SIGNAL(signalClose()));
    transition->setTargetState(m_closingState);
    m_openingState->addTransition(transition);

    animation = new QPropertyAnimation(this, "opacity", this);
    animation->setDuration(220);
    transition->addAnimation(animation);

    animation = new QPropertyAnimation(drawer, "offset", this);
    animation->setDuration(220);
    animation->setEasingCurve(QEasingCurve::InCirc);
    transition->addAnimation(animation);

    transition = new QSignalTransition(animation, SIGNAL(finished()));
    transition->setTargetState(m_closedState);
    m_closingState->addTransition(transition);

    transition = new QSignalTransition(this, SIGNAL(signalClose()));
    transition->setTargetState(m_closingState);
    m_openedState->addTransition(transition);

    animation = new QPropertyAnimation(drawer, "offset", this);
    animation->setDuration(220);
    animation->setEasingCurve(QEasingCurve::InCirc);
    transition->addAnimation(animation);

    animation = new QPropertyAnimation(this, "opacity", this);
    animation->setDuration(220);
    transition->addAnimation(animation);

    transition = new QSignalTransition(animation, SIGNAL(finished()));
    transition->setTargetState(m_closedState);
    m_closingState->addTransition(transition);

    updatePropertyAssignments();
}

MaterialDrawerStateMachine::~MaterialDrawerStateMachine()
{
}

void MaterialDrawerStateMachine::setOpacity(qreal opacity)
{
    m_opacity = opacity;
    m_main->update();
}

bool MaterialDrawerStateMachine::isInClosedState() const
{
    return m_closedState->active();
}

void MaterialDrawerStateMachine::updatePropertyAssignments()
{
    const qreal closedOffset = -(m_drawer->width() + 32);

    m_closingState->assignProperty(m_drawer, "offset", closedOffset);
    m_closedState->assignProperty(m_drawer, "offset", closedOffset);

    m_closingState->assignProperty(this, "opacity", 0);
    m_closedState->assignProperty(this, "opacity", 0);

    m_openingState->assignProperty(m_drawer, "offset", 0);
    m_openingState->assignProperty(this, "opacity", 0.4);
}

/*!
 *  \class MaterialDrawerWidget
 *  \internal
 */

MaterialDrawerWidget::MaterialDrawerWidget(QWidget *parent) : MaterialOverlayWidget(parent), m_offset(0)
{
}

MaterialDrawerWidget::~MaterialDrawerWidget()
{
}

void MaterialDrawerWidget::setOffset(int offset)
{
    m_offset = offset;

    QWidget *widget = parentWidget();
    if (widget) {
        setGeometry(widget->rect().translated(offset, 0));
    }
    update();
}

void MaterialDrawerWidget::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event)

    QPainter painter(this);

    QBrush brush;
    brush.setStyle(Qt::SolidPattern);
    brush.setColor(Qt::white);
    painter.setBrush(brush);
    painter.setPen(Qt::NoPen);

    painter.drawRect(rect().adjusted(0, 0, -16, 0));

    QLinearGradient gradient(QPointF(width() - 16, 0), QPointF(width(), 0));
    gradient.setColorAt(0, QColor(0, 0, 0, 80));
    gradient.setColorAt(0.5, QColor(0, 0, 0, 20));
    gradient.setColorAt(1, QColor(0, 0, 0, 0));
    painter.setBrush(QBrush(gradient));

    painter.drawRect(width() - 16, 0, 16, height());
}

QRect MaterialDrawerWidget::overlayGeometry() const
{
    return MaterialOverlayWidget::overlayGeometry().translated(m_offset, 0);
}

/*!
 *  \class MaterialDrawerPrivate
 *  \internal
 */

/*!
 *  \internal
 */
MaterialDrawerPrivate::MaterialDrawerPrivate(MaterialDrawer *q)
    : q_ptr(q)
{
}

/*!
 *  \internal
 */
MaterialDrawerPrivate::~MaterialDrawerPrivate()
{
}

/*!
 *  \internal
 */
void MaterialDrawerPrivate::init()
{
    Q_Q(MaterialDrawer);

    widget = new MaterialDrawerWidget;
    stateMachine = new MaterialDrawerStateMachine(widget, q);
    window = new QWidget;
    width = 250;
    clickToClose = false;
    autoRaise = true;
    closed = true;
    overlay = false;

    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(window);

    widget->setLayout(layout);
    widget->setFixedWidth(width + 16);

    widget->setParent(q);

    stateMachine->start();
    QCoreApplication::processEvents();
}

/*!
 *  \class MaterialDrawer
 */

MaterialDrawer::MaterialDrawer(QWidget *parent) : MaterialOverlayWidget(parent), d_ptr(new MaterialDrawerPrivate(this))
{
    d_func()->init();
}

MaterialDrawer::~MaterialDrawer()
{
}

void MaterialDrawer::setDrawerWidth(int width)
{
    Q_D(MaterialDrawer);

    d->width = width;
    d->stateMachine->updatePropertyAssignments();
    d->widget->setFixedWidth(width + 16);
}

int MaterialDrawer::drawerWidth() const
{
    Q_D(const MaterialDrawer);

    return d->width;
}

void MaterialDrawer::setDrawerLayout(QLayout *layout)
{
    Q_D(MaterialDrawer);

    d->window->setLayout(layout);
}

QLayout *MaterialDrawer::drawerLayout() const
{
    Q_D(const MaterialDrawer);

    return d->window->layout();
}

void MaterialDrawer::setClickOutsideToClose(bool state)
{
    Q_D(MaterialDrawer);

    d->clickToClose = state;
}

bool MaterialDrawer::clickOutsideToClose() const
{
    Q_D(const MaterialDrawer);

    return d->clickToClose;
}

void MaterialDrawer::setAutoRaise(bool state)
{
    Q_D(MaterialDrawer);

    d->autoRaise = state;
}

bool MaterialDrawer::autoRaise() const
{
    Q_D(const MaterialDrawer);

    return d->autoRaise;
}

void MaterialDrawer::setOverlayMode(bool value)
{
    Q_D(MaterialDrawer);

    d->overlay = value;
    update();
}

bool MaterialDrawer::overlayMode() const
{
    Q_D(const MaterialDrawer);

    return d->overlay;
}

void MaterialDrawer::openDrawer()
{
    Q_D(MaterialDrawer);

    emit d->stateMachine->signalOpen();

    if (d->autoRaise) {
        raise();
    }
    setAttribute(Qt::WA_TransparentForMouseEvents, false);
    setAttribute(Qt::WA_NoSystemBackground, false);
}

void MaterialDrawer::closeDrawer()
{
    Q_D(MaterialDrawer);

    emit d->stateMachine->signalClose();

    if (d->overlay) {
        setAttribute(Qt::WA_TransparentForMouseEvents);
        setAttribute(Qt::WA_NoSystemBackground);
    }
}

bool MaterialDrawer::event(QEvent *event)
{
    Q_D(MaterialDrawer);

    switch (event->type()) {
    case QEvent::Move:
    case QEvent::Resize:
        if (!d->overlay) {
            setMask(QRegion(d->widget->rect()));
        }
        break;
    default:
        break;
    }
    return MaterialOverlayWidget::event(event);
}

bool MaterialDrawer::eventFilter(QObject *obj, QEvent *event)
{
    Q_D(MaterialDrawer);

    switch (event->type()) {
    case QEvent::MouseButtonPress: {
        QMouseEvent *mouseEvent;
        if ((mouseEvent = static_cast<QMouseEvent *>(event))) {
            const bool canClose = d->clickToClose || d->overlay;
            if (!d->widget->geometry().contains(mouseEvent->pos()) && canClose) {
                closeDrawer();
            }
        }
        break;
    }
    case QEvent::Move:
    case QEvent::Resize: {
        QLayout *lw = d->widget->layout();
        if (lw && 16 != lw->contentsMargins().right()) {
            lw->setContentsMargins(0, 0, 16, 0);
        }
        break;
    }
    default:
        break;
    }
    return MaterialOverlayWidget::eventFilter(obj, event);
}

void MaterialDrawer::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event)

    Q_D(MaterialDrawer);

    if (!d->overlay || d->stateMachine->isInClosedState()) {
        return;
    }
    QPainter painter(this);
    painter.setOpacity(d->stateMachine->opacity());
    painter.fillRect(rect(), Qt::SolidPattern);
}

QX_MATERIAL_END_NAMESPACE
