/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#include "materialspeeddial.h"

#include "materialtokens.h"

#include "qxcontrols/qxcontrol.h"

#include <QtCore/QVariantAnimation>
#include <QtGui/QFontMetrics>
#include <QtGui/QKeyEvent>
#include <QtGui/QMoveEvent>
#include <QtGui/QPainter>
#include <QtWidgets/QSizePolicy>

QX_MATERIAL_BEGIN_NAMESPACE

namespace
{
// The button is a floating action button, so it is the size one is; an action
// that comes out of it is smaller, because it is not the one thing the screen
// is for - it is one of the others.
constexpr qreal kButtonSize = 56.0;
constexpr qreal kActionSize = 40.0;
constexpr qreal kIconSize = 24.0;
constexpr qreal kGap = 8.0;
constexpr qreal kLabelGap = 12.0;
/*! How far apart the resting places of two actions are. */
constexpr qreal kStep = kActionSize + kGap;

// The action's own circle: the page's colour with the hairline the family keeps
// for a surface that is not a button's, so that the accent still says which
// circle is the one the screen is for.
constexpr qreal kOutlineOpacity = 0.12;
constexpr qreal kLabelOpacity = 0.87;
constexpr qreal kDisabledOpacity = 0.38;

// The cross the button wears while the dial is open: two strokes of the same
// weight the family draws its glyphs with.
constexpr qreal kCrossArm = 7.0;
constexpr qreal kCrossWidth = 2.0;

/*! \a area's place \a fraction of the way from \a from to \a to. */
QRect between(const QRect &from, const QRect &to, qreal fraction)
{
    const auto mix = [fraction](int a, int b) {
        return a + qRound(fraction * qreal(b - a));
    };
    return QRect(mix(from.left(), to.left()), mix(from.top(), to.top()), mix(from.width(), to.width()),
                 mix(from.height(), to.height()));
}
}   // namespace

class SpeedAction;
class SpeedFab;

class MaterialSpeedDialPrivate
{
public:
    QX_DECLARE_PUBLIC(MaterialSpeedDial)

    QList<SpeedAction *> actions;
    SpeedFab *button = Q_NULLPTR;
    /*! Where the actions stand: in the widget the dial was put in, because they
     *  fly outside the dial's own square. */
    QWidget *host = Q_NULLPTR;
    MaterialSpeedDial::Direction direction = MaterialSpeedDial::Direction::Up;
    bool open = false;
    /*! How far the actions are out: 0 on the button, 1 in their places. */
    qreal fraction = 0.0;
    QVariantAnimation animation;

    int boxWidth(int index) const;
    /*! Where action \a index sits once it is all the way out. */
    QRect place(int index) const;
    /*! Where it sits on the button, which is where it comes from. */
    QRect resting() const;
    /*! Where it sits \a fraction of the way out, in the dial's coordinates. */
    QRect rectOf(int index, qreal fraction) const;
    void arrange();
    void activate(int index);
};

/*!
 * One action: a small circle with its name beside it. The name is drawn rather
 * than hidden in a tooltip, and the circle is the page's colour rather than the
 * accent's, so that a screen with five of them still has one button that is the
 * screen's.
 */
class SpeedAction : public QxControls::QxControl
{
public:
    SpeedAction(MaterialSpeedDialPrivate *owner, int index)
        : QxControls::QxControl(owner->host)
        , m_owner(owner)
        , m_index(index)
    {
        // The dial hands the keyboard to the button, and the actions are
        // reached by opening it, not by tabbing through them.
        setFocusPolicy(Qt::NoFocus);
        // Nothing is out until the dial is opened.
        hide();
    }

    void setIndex(int index)
    {
        m_index = index;
    }
    int index() const
    {
        return m_index;
    }

    void setName(const QString &name)
    {
        if (accessibleName().isEmpty() || accessibleName() == m_name) {
            setAccessibleName(name);
        }
        m_name = name;
        update();
    }
    QString name() const
    {
        return m_name;
    }

    void setIcon(const QIcon &icon)
    {
        m_icon = icon;
        update();
    }
    QIcon icon() const
    {
        return m_icon;
    }

    /*! How wide the action wants to be: its name, the gap, and the circle. */
    int askedWidth() const
    {
        const int text = m_name.isEmpty()
                             ? 0
                             : QFontMetrics(MaterialType::font(MaterialType::Scale::Body)).horizontalAdvance(m_name);
        return text + (text > 0 ? int(kLabelGap) : 0) + int(kActionSize);
    }

    /*! The circle's own rectangle, which is all of the action that is a control. */
    QRectF circle() const
    {
        return QRectF(qreal(width()) - kActionSize, 0.0, kActionSize, kActionSize);
    }

    qreal cornerRadius() const override
    {
        return MaterialShape::radius(MaterialShape::Radius::Circular);
    }
    QRectF chromeRect() const override
    {
        return circle();
    }

    void paintSurface(QPainter *painter) override
    {
        painter->save();
        if (!isEnabled()) {
            painter->setOpacity(kDisabledOpacity);
        }
        painter->setPen(Qt::NoPen);
        painter->setBrush(palette().color(QPalette::Window));
        const qreal radius = MaterialShape::radius(MaterialShape::Radius::Circular);
        painter->drawRoundedRect(circle(), radius, radius);
        painter->restore();
    }

    void paintContent(QPainter *painter) override
    {
        painter->save();
        if (!isEnabled()) {
            painter->setOpacity(kDisabledOpacity);
        }
        const QRectF face = circle();

        // The hairline round it: what says the circle is a surface and not a
        // hole in the page.
        QColor edge = textColor();
        edge.setAlphaF(kOutlineOpacity);
        painter->setPen(QPen(edge, 1.0));
        painter->setBrush(Qt::NoBrush);
        const qreal radius = MaterialShape::radius(MaterialShape::Radius::Circular);
        painter->drawRoundedRect(face.adjusted(0.5, 0.5, -0.5, -0.5), radius, radius);

        if (!m_icon.isNull()) {
            const QRectF box(face.center().x() - kIconSize / 2.0, face.center().y() - kIconSize / 2.0, kIconSize,
                             kIconSize);
            m_icon.paint(painter, box.toRect(), Qt::AlignCenter, isEnabled() ? QIcon::Normal : QIcon::Disabled);
        }

        if (!m_name.isEmpty()) {
            QColor ink = textColor();
            ink.setAlphaF(kLabelOpacity);
            painter->setPen(ink);
            painter->setFont(MaterialType::font(MaterialType::Scale::Body));
            const QRectF room(0.0, 0.0, face.left() - kLabelGap, qreal(height()));
            const QString shown =
                QFontMetrics(painter->font()).elidedText(m_name, Qt::ElideRight, int(qMax(0.0, room.width())));
            painter->drawText(room, Qt::AlignRight | Qt::AlignVCenter, shown);
        }
        painter->restore();
    }

    void keyPressEvent(QKeyEvent *event) override
    {
        switch (event->key()) {
        case Qt::Key_Space:
        case Qt::Key_Return:
        case Qt::Key_Enter:
            event->accept();
            return;
        default:
            break;
        }
        QxControls::QxControl::keyPressEvent(event);
    }
    void keyReleaseEvent(QKeyEvent *event) override
    {
        switch (event->key()) {
        case Qt::Key_Space:
        case Qt::Key_Return:
        case Qt::Key_Enter:
            // Auto-repeat is one press held down, not a press every few
            // milliseconds.
            if (!event->isAutoRepeat()) {
                Q_EMIT clicked();
            }
            event->accept();
            return;
        default:
            break;
        }
        QxControls::QxControl::keyReleaseEvent(event);
    }

    QAccessible::Role accessibleRole() const override
    {
        return QAccessible::Button;
    }
private:
    MaterialSpeedDialPrivate *m_owner;
    int m_index;
    QString m_name;
    QIcon m_icon;
};

/*!
 * The button itself: the accent's own circle, with the application's icon on it
 * while the dial is closed - and a cross while it is open, because what the
 * button does then is close what it opened.
 */
class SpeedFab : public QxControls::QxControl
{
public:
    SpeedFab(MaterialSpeedDialPrivate *owner)
        : QxControls::QxControl(owner->q_ptr)
        , m_owner(owner)
    {
        setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
        setFixedSize(int(kButtonSize), int(kButtonSize));
        show();
    }

    void setIcon(const QIcon &icon)
    {
        m_icon = icon;
        update();
    }
    QIcon icon() const
    {
        return m_icon;
    }
    void setText(const QString &text)
    {
        setAccessibleName(text);
        m_text = text;
    }
    QString text() const
    {
        return m_text;
    }

    /*! True while the dial is out, which is what the cross says. */
    void setOpened(bool opened)
    {
        m_opened = opened;
        update();
    }

    qreal cornerRadius() const override
    {
        return MaterialShape::radius(MaterialShape::Radius::Circular);
    }

    void paintSurface(QPainter *painter) override
    {
        painter->save();
        if (!isEnabled()) {
            painter->setOpacity(kDisabledOpacity);
        }
        painter->setPen(Qt::NoPen);
        painter->setBrush(accentColor());
        const qreal radius = MaterialShape::radius(MaterialShape::Radius::Circular);
        painter->drawRoundedRect(QRectF(rect()), radius, radius);
        painter->restore();
    }

    void paintContent(QPainter *painter) override
    {
        painter->save();
        if (!isEnabled()) {
            painter->setOpacity(kDisabledOpacity);
        }
        if (m_opened) {
            // A cross, not the icon turned: the icon is the application's
            // picture of what the screen is for, and turning it is editing
            // someone else's drawing.
            const QPointF middle = QRectF(rect()).center();
            QPen pen(palette().color(QPalette::HighlightedText), kCrossWidth);
            pen.setCapStyle(Qt::RoundCap);
            painter->setPen(pen);
            painter->setBrush(Qt::NoBrush);
            painter->drawLine(middle + QPointF(-kCrossArm, -kCrossArm), middle + QPointF(kCrossArm, kCrossArm));
            painter->drawLine(middle + QPointF(kCrossArm, -kCrossArm), middle + QPointF(-kCrossArm, kCrossArm));
        } else if (!m_icon.isNull()) {
            const QRectF box((qreal(width()) - kIconSize) / 2.0, (qreal(height()) - kIconSize) / 2.0, kIconSize,
                             kIconSize);
            m_icon.paint(painter, box.toRect(), Qt::AlignCenter, isEnabled() ? QIcon::Normal : QIcon::Disabled);
        }
        painter->restore();
    }

    void keyPressEvent(QKeyEvent *event) override
    {
        switch (event->key()) {
        case Qt::Key_Space:
        case Qt::Key_Return:
        case Qt::Key_Enter:
            event->accept();
            return;
        case Qt::Key_Escape:
            // Escape puts a dial away, the way it puts away everything else
            // that came out of a button.
            if (m_owner->open) {
                m_owner->q_ptr->close();
                event->accept();
                return;
            }
            break;
        default:
            break;
        }
        QxControls::QxControl::keyPressEvent(event);
    }
    void keyReleaseEvent(QKeyEvent *event) override
    {
        switch (event->key()) {
        case Qt::Key_Space:
        case Qt::Key_Return:
        case Qt::Key_Enter:
            if (!event->isAutoRepeat()) {
                Q_EMIT clicked();
            }
            event->accept();
            return;
        default:
            break;
        }
        QxControls::QxControl::keyReleaseEvent(event);
    }

    QAccessible::Role accessibleRole() const override
    {
        return QAccessible::Button;
    }
private:
    MaterialSpeedDialPrivate *m_owner;
    QIcon m_icon;
    QString m_text;
    bool m_opened = false;
};

int MaterialSpeedDialPrivate::boxWidth(int index) const
{
    const SpeedAction *action = actions.value(index, Q_NULLPTR);
    return action ? qMax(int(kActionSize), action->askedWidth()) : int(kActionSize);
}

QRect MaterialSpeedDialPrivate::resting() const
{
    // On the button, low down: the actions come out from under the one that let
    // them out.
    return QRect(int(kButtonSize - kActionSize), int(kButtonSize - kActionSize), int(kActionSize), int(kActionSize));
}

QRect MaterialSpeedDialPrivate::place(int index) const
{
    const int width = boxWidth(index);
    const int right = int(kButtonSize) - 1;
    const int down = direction == MaterialSpeedDial::Direction::Down;
    const int top = down ? int(kButtonSize + kGap) + index * int(kStep) : -int(kGap + kActionSize) - index * int(kStep);
    return QRect(right - width + 1, top, width, int(kActionSize));
}

QRect MaterialSpeedDialPrivate::rectOf(int index, qreal fraction) const
{
    const QRect out = place(index);
    if (fraction >= 1.0) {
        return out;
    }
    return between(resting(), out, qBound(0.0, fraction, 1.0));
}

void MaterialSpeedDialPrivate::arrange()
{
    if (!host) {
        return;
    }
    const QPoint origin = q_ptr->mapTo(host, QPoint(0, 0));
    for (int i = 0; i < actions.count(); ++i) {
        SpeedAction *action = actions.at(i);
        const QRect room = rectOf(i, fraction);
        action->setGeometry(QRect(origin + room.topLeft(), room.size()));
    }
}

void MaterialSpeedDialPrivate::activate(int index)
{
    // An action taken is an answer: the question closes behind it.
    q_ptr->close();
    Q_EMIT q_ptr->actionTriggered(index);
}

MaterialSpeedDial::MaterialSpeedDial(QWidget *parent)
    : QWidget(parent)
    , d_ptr(new MaterialSpeedDialPrivate)
{
    QX_SET_PRIVATE(d_ptr);
    setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    resize(int(kButtonSize), int(kButtonSize));
    // The button is the only thing here that takes the keyboard; the dial is
    // the square it stands on.
    setFocusPolicy(Qt::NoFocus);

    QObject::connect(&d_ptr->animation, &QVariantAnimation::valueChanged, this, [this](const QVariant &value) {
        d_ptr->fraction = value.toReal();
        d_ptr->arrange();
    });
    QObject::connect(&d_ptr->animation, &QVariantAnimation::finished, this, [this]() {
        if (!d_ptr->open) {
            for (SpeedAction *action : d_ptr->actions) {
                action->hide();
            }
        }
    });

    d_ptr->host = parentWidget() ? parentWidget() : this;
    d_ptr->button = new SpeedFab(d_ptr);
    QObject::connect(d_ptr->button, &QxControls::QxControl::clicked, this, [this]() {
        toggle();
    });
    if (d_ptr->host != this) {
        d_ptr->host->installEventFilter(this);
    }
}

MaterialSpeedDial::~MaterialSpeedDial()
{
    QX_FINI_PRIVATE();
}

int MaterialSpeedDial::addAction(const QString &name, const QIcon &icon)
{
    SpeedAction *action = new SpeedAction(d_ptr, d_ptr->actions.count());
    action->setName(name);
    action->setIcon(icon);
    QObject::connect(action, &QxControls::QxControl::clicked, this, [this, action]() {
        d_ptr->activate(action->index());
    });
    d_ptr->actions.append(action);
    if (d_ptr->open) {
        action->show();
        raise();
    }
    updateGeometry();
    d_ptr->arrange();
    return action->index();
}

void MaterialSpeedDial::removeAction(int index)
{
    SpeedAction *action = d_ptr->actions.value(index, Q_NULLPTR);
    if (!action) {
        return;
    }
    d_ptr->actions.removeAt(index);
    action->hide();
    action->deleteLater();
    for (int i = 0; i < d_ptr->actions.count(); ++i) {
        d_ptr->actions.at(i)->setIndex(i);
    }
    updateGeometry();
    d_ptr->arrange();
}

void MaterialSpeedDial::clear()
{
    const QList<SpeedAction *> gone = d_ptr->actions;
    d_ptr->actions.clear();
    for (SpeedAction *action : gone) {
        action->hide();
        action->deleteLater();
    }
    updateGeometry();
}

int MaterialSpeedDial::actionCount() const
{
    return d_ptr->actions.count();
}

QString MaterialSpeedDial::actionName(int index) const
{
    const SpeedAction *action = d_ptr->actions.value(index, Q_NULLPTR);
    return action ? action->name() : QString();
}

QIcon MaterialSpeedDial::actionIcon(int index) const
{
    const SpeedAction *action = d_ptr->actions.value(index, Q_NULLPTR);
    return action ? action->icon() : QIcon();
}

QIcon MaterialSpeedDial::icon() const
{
    return d_ptr->button->icon();
}

QString MaterialSpeedDial::text() const
{
    return d_ptr->button->text();
}

MaterialSpeedDial::Direction MaterialSpeedDial::direction() const
{
    return d_ptr->direction;
}

bool MaterialSpeedDial::isOpen() const
{
    return d_ptr->open;
}

QRect MaterialSpeedDial::actionRect(int index) const
{
    if (index < 0 || index >= d_ptr->actions.count()) {
        return QRect();
    }
    return d_ptr->rectOf(index, d_ptr->fraction);
}

QSize MaterialSpeedDial::sizeHint() const
{
    return QSize(int(kButtonSize), int(kButtonSize));
}

QSize MaterialSpeedDial::minimumSizeHint() const
{
    return sizeHint();
}

void MaterialSpeedDial::setIcon(const QIcon &icon)
{
    d_ptr->button->setIcon(icon);
}

void MaterialSpeedDial::setText(const QString &text)
{
    d_ptr->button->setText(text);
}

void MaterialSpeedDial::setActionName(int index, const QString &name)
{
    SpeedAction *action = d_ptr->actions.value(index, Q_NULLPTR);
    if (!action || action->name() == name) {
        return;
    }
    action->setName(name);
    d_ptr->arrange();
}

void MaterialSpeedDial::setActionIcon(int index, const QIcon &icon)
{
    SpeedAction *action = d_ptr->actions.value(index, Q_NULLPTR);
    if (action) {
        action->setIcon(icon);
    }
}

void MaterialSpeedDial::setDirection(Direction direction)
{
    if (d_ptr->direction == direction) {
        return;
    }
    d_ptr->direction = direction;
    d_ptr->arrange();
    Q_EMIT directionChanged(direction);
}

void MaterialSpeedDial::setOpen(bool open)
{
    if (d_ptr->open == open) {
        return;
    }
    d_ptr->open = open;
    d_ptr->button->setOpened(open);

    if (open) {
        // Out comes first, over whatever the page has there; the button stays
        // above them, because they come out from under it.
        for (SpeedAction *action : d_ptr->actions) {
            action->show();
        }
        raise();
    }

    const qreal target = open ? 1.0 : 0.0;
    // The family's own answer to how long a movement takes: zero when the
    // application asked for less motion, and then the actions are simply out.
    const QxTheme::QxMotion motion = MaterialMotion::forDuration(MaterialMotion::Duration::Medium);
    d_ptr->animation.stop();
    if (motion.duration <= 0 || d_ptr->actions.isEmpty()) {
        d_ptr->fraction = target;
        d_ptr->arrange();
        if (!open) {
            for (SpeedAction *action : d_ptr->actions) {
                action->hide();
            }
        }
    } else {
        d_ptr->animation.setDuration(motion.duration);
        d_ptr->animation.setEasingCurve(motion.curve);
        d_ptr->animation.setStartValue(d_ptr->fraction);
        d_ptr->animation.setEndValue(target);
        d_ptr->animation.start();
    }
    Q_EMIT openChanged(open);
}

void MaterialSpeedDial::open()
{
    setOpen(true);
}

void MaterialSpeedDial::close()
{
    setOpen(false);
}

void MaterialSpeedDial::toggle()
{
    setOpen(!d_ptr->open);
}

void MaterialSpeedDial::moveEvent(QMoveEvent *event)
{
    QWidget::moveEvent(event);
    d_ptr->arrange();
}

bool MaterialSpeedDial::eventFilter(QObject *watched, QEvent *event)
{
    // The actions are the host's children, so where they stand is a fact about
    // the host as much as about the dial: when it moves or resizes, they follow.
    if (watched == d_ptr->host && (event->type() == QEvent::Resize || event->type() == QEvent::Move)) {
        d_ptr->arrange();
    }
    return QWidget::eventFilter(watched, event);
}

void MaterialSpeedDial::changeEvent(QEvent *event)
{
    QWidget::changeEvent(event);
    if (event->type() == QEvent::FontChange || event->type() == QEvent::PaletteChange) {
        d_ptr->arrange();
        update();
    }
}

QX_MATERIAL_END_NAMESPACE
