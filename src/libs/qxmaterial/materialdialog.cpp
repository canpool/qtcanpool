/**
 * Copyleft (C) 2023 maminjie <canpool@163.com>
 * SPDX-License-Identifier: BSD-3-Clause
 **/
#include "materialdialog.h"
#include "materialdialog_p.h"
#include "materialstatetransition.h"
#include <QPainter>
#include <QPropertyAnimation>
#include <QState>
#include <QStateMachine>
#include <QtWidgets/QApplication>
#include <QtWidgets/QGraphicsDropShadowEffect>
#include <QtWidgets/QStackedLayout>
#include <QEvent>
#include <QPixmap>

QX_MATERIAL_BEGIN_NAMESPACE

/*!
 *  \class MaterialDialogProxy
 *  \internal
 */

MaterialDialogProxy::MaterialDialogProxy(MaterialDialogWindow *source, QStackedLayout *layout, MaterialDialog *dialog,
                                         QWidget *parent)
    : QWidget(parent)
    , m_source(source)
    , m_layout(layout)
    , m_dialog(dialog)
    , m_opacity(0)
    , m_mode(Transparent)
{
}

MaterialDialogProxy::~MaterialDialogProxy()
{
}

void MaterialDialogProxy::setOpacity(qreal opacity)
{
    m_opacity = opacity;
    m_mode = SemiTransparent;
    update();
    m_dialog->update();
}

void MaterialDialogProxy::makeOpaque()
{
    m_dialog->setAttribute(Qt::WA_TransparentForMouseEvents, false);
    m_layout->setCurrentIndex(0);
    m_opacity = 1.0;
    m_mode = Opaque;
    update();
}

void MaterialDialogProxy::makeTransparent()
{
    m_opacity = 0.0;
    m_mode = Transparent;
    update();
}

QSize MaterialDialogProxy::sizeHint() const
{
    return m_source->sizeHint();
}

bool MaterialDialogProxy::event(QEvent *event)
{
    const QEvent::Type type = event->type();

    if (QEvent::Move == type || QEvent::Resize == type) {
        m_source->setGeometry(geometry());
    }
    return QWidget::event(event);
}

void MaterialDialogProxy::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event)

    QPainter painter(this);

    if (Transparent == m_mode) {
        return;
    } else if (Opaque != m_mode) {
        painter.setOpacity(m_opacity);
    }
    QPixmap pm = m_source->grab(m_source->rect());
    painter.drawPixmap(0, 0, pm);
}

/*!
 *  \class MaterialDialogWindow
 *  \internal
 */

MaterialDialogWindow::MaterialDialogWindow(MaterialDialog *dialog, QWidget *parent) : QWidget(parent), m_dialog(dialog)
{
}

MaterialDialogWindow::~MaterialDialogWindow()
{
}

void MaterialDialogWindow::setOffset(int offset)
{
    QMargins margins = m_dialog->layout()->contentsMargins();
    margins.setBottom(offset);
    m_dialog->layout()->setContentsMargins(margins);
}

int MaterialDialogWindow::offset() const
{
    return m_dialog->layout()->contentsMargins().bottom();
}

void MaterialDialogWindow::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event)

    QPainter painter(this);

    QBrush brush;
    brush.setStyle(Qt::SolidPattern);
    brush.setColor(Qt::white);
    painter.setPen(Qt::NoPen);
    painter.setBrush(brush);
    painter.drawRect(rect());
}

/*!
 *  \class MaterialDialogPrivate
 *  \internal
 */

MaterialDialogPrivate::MaterialDialogPrivate(MaterialDialog *q)
    : q_ptr(q)
{
}

MaterialDialogPrivate::~MaterialDialogPrivate()
{
}

void MaterialDialogPrivate::init()
{
    Q_Q(MaterialDialog);

    dialogWindow = new MaterialDialogWindow(q);
    proxyStack = new QStackedLayout;
    stateMachine = new QStateMachine(q);
    proxy = new MaterialDialogProxy(dialogWindow, proxyStack, q);

    QVBoxLayout *layout = new QVBoxLayout;
    q->setLayout(layout);

    QWidget *widget = new QWidget;
    widget->setLayout(proxyStack);
    widget->setMinimumWidth(400);

    QGraphicsDropShadowEffect *effect = new QGraphicsDropShadowEffect;
    effect->setColor(QColor(0, 0, 0, 200));
    effect->setBlurRadius(64);
    effect->setOffset(0, 13);
    widget->setGraphicsEffect(effect);

    layout->addWidget(widget);
    layout->setAlignment(widget, Qt::AlignCenter);

    proxyStack->addWidget(dialogWindow);
    proxyStack->addWidget(proxy);
    proxyStack->setCurrentIndex(1);

    q->setAttribute(Qt::WA_TransparentForMouseEvents);

    QState *hiddenState = new QState;
    QState *visibleState = new QState;

    stateMachine->addState(hiddenState);
    stateMachine->addState(visibleState);
    stateMachine->setInitialState(hiddenState);

    MaterialStateTransition *transition;

    transition = new MaterialStateTransition(DialogShowTransition);
    transition->setTargetState(visibleState);
    hiddenState->addTransition(transition);

    transition = new MaterialStateTransition(DialogHideTransition);
    transition->setTargetState(hiddenState);
    visibleState->addTransition(transition);

    visibleState->assignProperty(proxy, "opacity", 1);
    visibleState->assignProperty(effect, "color", QColor(0, 0, 0, 200));
    visibleState->assignProperty(dialogWindow, "offset", 0);
    hiddenState->assignProperty(proxy, "opacity", 0);
    hiddenState->assignProperty(effect, "color", QColor(0, 0, 0, 0));
    hiddenState->assignProperty(dialogWindow, "offset", 200);

    QPropertyAnimation *animation;

    animation = new QPropertyAnimation(proxy, "opacity", q);
    animation->setDuration(280);
    stateMachine->addDefaultAnimation(animation);

    animation = new QPropertyAnimation(effect, "color", q);
    animation->setDuration(280);
    stateMachine->addDefaultAnimation(animation);

    animation = new QPropertyAnimation(dialogWindow, "offset", q);
    animation->setDuration(280);
    animation->setEasingCurve(QEasingCurve::OutCirc);
    stateMachine->addDefaultAnimation(animation);

    QObject::connect(visibleState, SIGNAL(propertiesAssigned()), proxy, SLOT(makeOpaque()));
    QObject::connect(hiddenState, SIGNAL(propertiesAssigned()), proxy, SLOT(makeTransparent()));

    stateMachine->start();
    QCoreApplication::processEvents();
}

/*!
 *  \class MaterialDialog
 */

MaterialDialog::MaterialDialog(QWidget *parent) : MaterialOverlayWidget(parent), d_ptr(new MaterialDialogPrivate(this))
{
    d_func()->init();
}

MaterialDialog::~MaterialDialog()
{
}

QLayout *MaterialDialog::windowLayout() const
{
    Q_D(const MaterialDialog);

    return d->dialogWindow->layout();
}

void MaterialDialog::setWindowLayout(QLayout *layout)
{
    Q_D(MaterialDialog);

    d->dialogWindow->setLayout(layout);
}

void MaterialDialog::showDialog()
{
    Q_D(MaterialDialog);

    d->stateMachine->postEvent(new MaterialStateTransitionEvent(DialogShowTransition));
    raise();
}

void MaterialDialog::hideDialog()
{
    Q_D(MaterialDialog);

    d->stateMachine->postEvent(new MaterialStateTransitionEvent(DialogHideTransition));
    setAttribute(Qt::WA_TransparentForMouseEvents);
    d->proxyStack->setCurrentIndex(1);
}

void MaterialDialog::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event)

    Q_D(MaterialDialog);

    QPainter painter(this);

    QBrush brush;
    brush.setStyle(Qt::SolidPattern);
    brush.setColor(Qt::black);
    painter.setBrush(brush);
    painter.setPen(Qt::NoPen);
    painter.setOpacity(d->proxy->opacity() / 2.4);
    painter.drawRect(rect());
}

QX_MATERIAL_END_NAMESPACE
