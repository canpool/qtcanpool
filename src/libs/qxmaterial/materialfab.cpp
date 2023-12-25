/**
 * Copyleft (C) 2023 maminjie <canpool@163.com>
 * SPDX-License-Identifier: BSD-3-Clause
 **/
#include "materialfab.h"
#include "materialfab_p.h"
#include "materialrippleoverlay.h"
#include <QEvent>
#include <QPainter>
#include <QState>
#include <QtWidgets/QGraphicsDropShadowEffect>

QX_MATERIAL_BEGIN_NAMESPACE

/*!
 *  \class MaterialFloatingActionButtonPrivate
 *  \internal
 */

/*!
 *  \internal
 */
MaterialFloatingActionButtonPrivate::MaterialFloatingActionButtonPrivate(MaterialFloatingActionButton *q)
    : MaterialRaisedButtonPrivate(q)
{
}

/*!
 *  \internal
 */
MaterialFloatingActionButtonPrivate::~MaterialFloatingActionButtonPrivate()
{
}

/*!
 *  \internal
 */
void MaterialFloatingActionButtonPrivate::init()
{
    Q_Q(MaterialFloatingActionButton);

    corner = Qt::BottomRightCorner;
    mini = false;
    offsX = 34;
    offsY = 36;

    q->setRole(QxMaterial::Primary);
    q->setFixedSize(DefaultDiameter, DefaultDiameter);
    q->setGeometry(fabGeometry());

    setupProperties();

    if (q->parentWidget()) {
        q->parentWidget()->installEventFilter(q);
    }

    q->setFixedRippleRadius(50);
}

/*!
 *  \internal
 */
QRect MaterialFloatingActionButtonPrivate::fabGeometry() const
{
    Q_Q(const MaterialFloatingActionButton);

    QWidget *parent = q->parentWidget();
    if (!parent) {
        return QRect();
    }

    const int s = mini ? MiniDiameter : DefaultDiameter;

    switch (corner) {
    case Qt::TopLeftCorner:
        return QRect(offsX, offsY, s, s);
    case Qt::TopRightCorner:
        return QRect(parent->width() - (offsX + s), offsY, s, s);
    case Qt::BottomLeftCorner:
        return QRect(offsX, parent->height() - (offsY + s), s, s);
    case Qt::BottomRightCorner:
    default:
        break;
    }
    return QRect(parent->width() - (offsX + s), parent->height() - (offsY + s), s, s);
}

/*!
 *  \internal
 */
void MaterialFloatingActionButtonPrivate::setupProperties()
{
    if (mini) {
        effect->setColor(QColor(0, 0, 0, 80));
        normalState->assignProperty(effect, "offset", QPointF(0, 3));
        normalState->assignProperty(effect, "blurRadius", 13);
        pressedState->assignProperty(effect, "offset", QPointF(0, 7));
        pressedState->assignProperty(effect, "blurRadius", 20);
    } else {
        effect->setColor(QColor(0, 0, 0, 105));
        normalState->assignProperty(effect, "offset", QPointF(0, 6));
        normalState->assignProperty(effect, "blurRadius", 16);
        pressedState->assignProperty(effect, "offset", QPointF(0, 11));
        pressedState->assignProperty(effect, "blurRadius", 28);
    }
}

/*!
 *  \class MaterialFloatingActionButton
 */

MaterialFloatingActionButton::MaterialFloatingActionButton(const QIcon &icon, QWidget *parent)
    : MaterialRaisedButton(*new MaterialFloatingActionButtonPrivate(this), parent)
{
    d_func()->init();

    setIcon(icon);
}

MaterialFloatingActionButton::~MaterialFloatingActionButton()
{
}

/*!
 *  \reimp
 */
QSize MaterialFloatingActionButton::sizeHint() const
{
    Q_D(const MaterialFloatingActionButton);

    if (d->mini) {
        return QSize(MaterialFloatingActionButtonPrivate::MiniDiameter,
                     MaterialFloatingActionButtonPrivate::MiniDiameter);
    } else {
        return QSize(MaterialFloatingActionButtonPrivate::DefaultDiameter,
                     MaterialFloatingActionButtonPrivate::DefaultDiameter);
    }
}

void MaterialFloatingActionButton::setMini(bool state)
{
    Q_D(MaterialFloatingActionButton);

    if (d->mini == state) {
        return;
    }

    d->mini = state;

    setFixedSize(d->diameter(), d->diameter());
    setFixedRippleRadius(state ? 30 : 50);

    d->setupProperties();
    updateClipPath();
    setGeometry(d->fabGeometry());
    update();
}

bool MaterialFloatingActionButton::isMini() const
{
    Q_D(const MaterialFloatingActionButton);

    return d->mini;
}

void MaterialFloatingActionButton::setCorner(Qt::Corner corner)
{
    Q_D(MaterialFloatingActionButton);

    if (d->corner == corner) {
        return;
    }

    d->corner = corner;
    setGeometry(d->fabGeometry());
    update();
}

Qt::Corner MaterialFloatingActionButton::corner() const
{
    Q_D(const MaterialFloatingActionButton);

    return d->corner;
}

void MaterialFloatingActionButton::setOffset(int x, int y)
{
    Q_D(MaterialFloatingActionButton);

    d->offsX = x;
    d->offsY = y;
    setGeometry(d->fabGeometry());
    update();
}

QSize MaterialFloatingActionButton::offset() const
{
    Q_D(const MaterialFloatingActionButton);

    return QSize(d->offsX, d->offsY);
}

void MaterialFloatingActionButton::setXOffset(int x)
{
    Q_D(MaterialFloatingActionButton);

    d->offsX = x;
    setGeometry(d->fabGeometry());
    update();
}

int MaterialFloatingActionButton::xOffset() const
{
    Q_D(const MaterialFloatingActionButton);

    return d->offsX;
}

void MaterialFloatingActionButton::setYOffset(int y)
{
    Q_D(MaterialFloatingActionButton);

    d->offsY = y;
    setGeometry(d->fabGeometry());
    update();
}

int MaterialFloatingActionButton::yOffset() const
{
    Q_D(const MaterialFloatingActionButton);

    return d->offsY;
}

/*!
 *  \reimp
 */
bool MaterialFloatingActionButton::event(QEvent *event)
{
    Q_D(MaterialFloatingActionButton);

    if (!parent()) {
        return MaterialRaisedButton::event(event);
    }
    switch (event->type()) {
    case QEvent::ParentChange: {
        parent()->installEventFilter(this);
        setGeometry(d->fabGeometry());
        break;
    }
    case QEvent::ParentAboutToChange: {
        parent()->removeEventFilter(this);
        break;
    }
    default:
        break;
    }
    return MaterialRaisedButton::event(event);
}

/*!
 *  \reimp
 */
bool MaterialFloatingActionButton::eventFilter(QObject *obj, QEvent *event)
{
    const QEvent::Type type = event->type();

    if (QEvent::Move == type || QEvent::Resize == type) {
        Q_D(MaterialFloatingActionButton);
        setGeometry(d->fabGeometry());
    }

    return MaterialRaisedButton::eventFilter(obj, event);
}

/*!
 *  \reimp
 */
void MaterialFloatingActionButton::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event)

    Q_D(MaterialFloatingActionButton);

    QRect square = QRect(0, 0, d->diameter(), d->diameter());
    square.moveCenter(rect().center());

    QPainter painter(this);
    painter.setRenderHints(QPainter::Antialiasing);

    QBrush brush;
    brush.setStyle(Qt::SolidPattern);

    if (isEnabled()) {
        brush.setColor(backgroundColor());
    } else {
        brush.setColor(disabledBackgroundColor());
    }

    painter.setBrush(brush);
    painter.setPen(Qt::NoPen);
    painter.drawEllipse(square);

    QRect iconGeometry(0, 0, d->iconSize(), d->iconSize());
    iconGeometry.moveCenter(square.center());

    QPixmap pixmap = icon().pixmap(QSize(d->iconSize(), d->iconSize()));
    QPainter icon(&pixmap);
    icon.setCompositionMode(QPainter::CompositionMode_SourceIn);
    icon.fillRect(pixmap.rect(), isEnabled() ? foregroundColor() : disabledForegroundColor());
    painter.drawPixmap(iconGeometry, pixmap);
}

void MaterialFloatingActionButton::updateClipPath()
{
    Q_D(MaterialFloatingActionButton);

    QPainterPath path;
    path.addEllipse(0, 0, d->diameter(), d->diameter());
    d->rippleOverlay->setClipPath(path);
}

QX_MATERIAL_END_NAMESPACE
