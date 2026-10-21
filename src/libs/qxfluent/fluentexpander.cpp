/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#include "fluentexpander.h"

#include "fluenttokens.h"

#include <QtCore/QVariantAnimation>
#include <QtGui/QPainter>
#include <QtGui/QPainterPath>
#include <QtGui/QResizeEvent>

QX_FLUENT_BEGIN_NAMESPACE

namespace
{
// Fluent's expander header is forty tall, and its chevron sits twelve in.
constexpr qreal kHeaderHeight = 40.0;
constexpr qreal kChevronInset = 12.0;
constexpr qreal kChevronSize = 8.0;
constexpr qreal kTitleGap = 16.0;
}   // namespace

class FluentExpanderPrivate
{
public:
    QX_DECLARE_PUBLIC(FluentExpander)

    bool expanded = false;
    QString title;
    QWidget *content = Q_NULLPTR;
    /*! How far the body is out: 0 closed, 1 open. */
    qreal fraction = 0.0;
    QVariantAnimation animation;

    QRectF headerRect(const QWidget *widget) const
    {
        return QRectF(0.0, 0.0, widget->width(), kHeaderHeight);
    }

    int bodyHeight() const
    {
        return content ? content->sizeHint().height() : 0;
    }

    void place(qreal at)
    {
        if (!content) {
            return;
        }
        // The body is clipped to what it has been given, so a half-open
        // expander shows the top of it rather than a squashed whole.
        content->setGeometry(0, int(kHeaderHeight), content->width(), int(at * bodyHeight()));
        content->setVisible(at > 0.0);
    }
};

FluentExpander::FluentExpander(QWidget *parent)
    : QxControls::QxControl(parent)
    , d_ptr(new FluentExpanderPrivate)
{
    QX_SET_PRIVATE(d_ptr)
    connect(&d_ptr->animation, &QVariantAnimation::valueChanged, this, [this](const QVariant &value) {
        d_ptr->fraction = value.toReal();
        d_ptr->place(d_ptr->fraction);
        updateGeometry();
        update();
    });
    connect(this, &QxControls::QxControl::clicked, this, &FluentExpander::toggle);
}

FluentExpander::~FluentExpander()
{
    QX_FINI_PRIVATE()
}

bool FluentExpander::isExpanded() const
{
    return d_ptr->expanded;
}

QString FluentExpander::title() const
{
    return d_ptr->title;
}

QWidget *FluentExpander::content() const
{
    return d_ptr->content;
}

void FluentExpander::setTitle(const QString &title)
{
    if (d_ptr->title == title) {
        return;
    }
    d_ptr->title = title;
    updateGeometry();
    update();
}

void FluentExpander::setContent(QWidget *content)
{
    if (d_ptr->content == content) {
        return;
    }
    d_ptr->content = content;
    if (content) {
        content->setParent(this);
        content->setVisible(d_ptr->expanded);
    }
    updateGeometry();
    update();
}

void FluentExpander::setExpanded(bool expanded)
{
    if (d_ptr->expanded == expanded) {
        return;
    }
    d_ptr->expanded = expanded;

    // Opening is a movement with distance to cover, so it takes the tier that
    // covers distance; and a tier is what the reduce switch answers, so an
    // expander opened under reduce-motion is simply open.
    const QxTheme::QxMotion motion = FluentMotion::forDuration(FluentMotion::Duration::Normal);
    const qreal target = expanded ? 1.0 : 0.0;
    d_ptr->animation.stop();
    if (motion.duration <= 0) {
        d_ptr->fraction = target;
        d_ptr->place(target);
        updateGeometry();
    } else {
        d_ptr->animation.setDuration(motion.duration);
        d_ptr->animation.setEasingCurve(motion.curve);
        d_ptr->animation.setStartValue(d_ptr->fraction);
        d_ptr->animation.setEndValue(target);
        d_ptr->animation.start();
    }
    update();
    Q_EMIT expandedChanged(expanded);
}

void FluentExpander::toggle()
{
    setExpanded(!d_ptr->expanded);
}

QSize FluentExpander::sizeHint() const
{
    const QFontMetrics metrics(FluentType::font(FluentType::Ramp::Body));
    const int title = metrics.horizontalAdvance(d_ptr->title);
    const int width = qMax(int(kChevronInset + kChevronSize + kTitleGap + title + kChevronInset),
                           d_ptr->content ? d_ptr->content->sizeHint().width() : 0);
    const int height = int(kHeaderHeight) + int(d_ptr->fraction * d_ptr->bodyHeight());
    return QSize(width, height);
}

QRectF FluentExpander::chromeRect() const
{
    // The wash and the ring belong to the header: that is the part the pointer
    // can reach.
    return d_ptr->headerRect(this);
}

bool FluentExpander::hitTest(const QPointF &pos) const
{
    return d_ptr->headerRect(this).contains(pos);
}

void FluentExpander::paintContent(QPainter *painter)
{
    painter->save();
    painter->setRenderHint(QPainter::Antialiasing, true);

    // The chevron turns as the body comes out: a triangle that is half way
    // between pointing at the body and pointing into it is what tells the eye
    // the movement is still happening.
    const QPointF tip(kChevronInset + kChevronSize / 2.0, kHeaderHeight / 2.0);
    painter->translate(tip);
    painter->rotate(d_ptr->fraction * 90.0);
    painter->translate(-tip);
    QPainterPath chevron;
    chevron.moveTo(kChevronInset, kHeaderHeight / 2.0 - kChevronSize / 2.0);
    chevron.lineTo(kChevronInset + kChevronSize, kHeaderHeight / 2.0);
    chevron.lineTo(kChevronInset, kHeaderHeight / 2.0 + kChevronSize / 2.0);
    painter->setPen(QPen(textColor(), 1.5, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
    painter->drawPath(chevron);
    painter->restore();

    painter->save();
    painter->setFont(FluentType::font(FluentType::Ramp::Body));
    painter->setPen(textColor());
    const QRectF title(kChevronInset + kChevronSize + kTitleGap, 0.0,
                       width() - kChevronInset - kChevronSize - kTitleGap, kHeaderHeight);
    painter->drawText(title, Qt::AlignLeft | Qt::AlignVCenter, d_ptr->title);
    painter->restore();
}

void FluentExpander::resizeEvent(QResizeEvent *event)
{
    QxControls::QxControl::resizeEvent(event);
    if (d_ptr->content) {
        d_ptr->content->setGeometry(0, int(kHeaderHeight), width(), int(d_ptr->fraction * d_ptr->bodyHeight()));
    }
}

QAccessible::Role FluentExpander::accessibleRole() const
{
    // What it is to an assistive tool is a button that opens something: the
    // role is the control's, and the state says whether what it keeps is out.
    return QAccessible::Button;
}

void FluentExpander::fillAccessibleState(QAccessible::State *state) const
{
    state->expandable = true;
    state->expanded = d_ptr->expanded;
}

QX_FLUENT_END_NAMESPACE
