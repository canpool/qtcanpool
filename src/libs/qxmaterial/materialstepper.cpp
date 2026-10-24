/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#include "materialstepper.h"

#include "materialtokens.h"

#include "qxcontrols/qxstatelayer.h"

#include "qxtheme/qxthememanager.h"

#include <QtCore/QVector>
#include <QtGui/QKeyEvent>
#include <QtGui/QMouseEvent>
#include <QtGui/QPainter>
#include <QtGui/QPainterPath>

QX_MATERIAL_BEGIN_NAMESPACE

namespace
{
// The row's measures: a circle of twenty-four numbered inside, its name under
// it with a gap, its caption under that, and the sides of the row keeping the
// first and the last circle off the edge.
constexpr qreal kPadding = 16.0;
constexpr qreal kCircle = 24.0;
constexpr qreal kConnector = 2.0;
constexpr qreal kLabelGap = 8.0;
constexpr qreal kLabelHeight = 18.0;
constexpr qreal kCaptionHeight = 16.0;
// Room above the circles for the ring that says the row has the keyboard.
constexpr qreal kRingRoom = 3.0;
// One step's column never gets narrower than this: a name has to have room
// to be a name.
constexpr qreal kMinColumn = 96.0;

// The steps still ahead are an outline and a number in the quiet ink; the
// line joining them is quieter still.
constexpr qreal kQuietInkOpacity = 0.6;
constexpr qreal kOutlineOpacity = 0.12;
constexpr qreal kConnectorWalkedOpacity = 0.4;

QPointF eventPos(const QMouseEvent *event)
{
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    return event->position();
#else
    return event->localPos();
#endif
}

}   // namespace

class MaterialStepperPrivate
{
public:
    QX_DECLARE_PUBLIC(MaterialStepper)

    struct Step {
        QString label;
        QString caption;
    };

    QVector<Step> steps;
    int current = 0;

    int indexOf(const QPointF &pos, const QVector<QRectF> &columns) const
    {
        for (int i = 0; i < columns.count(); ++i) {
            if (columns.at(i).contains(pos)) {
                return i;
            }
        }
        return -1;
    }
    bool saysCaption() const
    {
        for (const Step &step : steps) {
            if (!step.caption.isEmpty()) {
                return true;
            }
        }
        return false;
    }
};

MaterialStepper::MaterialStepper(QWidget *parent)
    : QWidget(parent)
    , d_ptr(new MaterialStepperPrivate)
{
    QX_SET_PRIVATE(d_ptr)
    // A step that is behind can be taken again, so the row takes the keyboard
    // for the one thing it can do: walking back.
    setFocusPolicy(Qt::StrongFocus);
    setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
}

MaterialStepper::~MaterialStepper()
{
    QX_FINI_PRIVATE()
}

void MaterialStepper::addStep(const QString &label, const QString &caption)
{
    MaterialStepperPrivate::Step step;
    step.label = label;
    step.caption = caption;
    d_ptr->steps.append(step);
    updateGeometry();
    update();
}

int MaterialStepper::count() const
{
    return d_ptr->steps.count();
}

QString MaterialStepper::stepLabel(int index) const
{
    if (index < 0 || index >= count()) {
        return QString();
    }
    return d_ptr->steps.at(index).label;
}

QString MaterialStepper::stepCaption(int index) const
{
    if (index < 0 || index >= count()) {
        return QString();
    }
    return d_ptr->steps.at(index).caption;
}

int MaterialStepper::currentStep() const
{
    return d_ptr->current;
}

MaterialStepper::State MaterialStepper::stepState(int index) const
{
    if (index < d_ptr->current) {
        return State::Completed;
    }
    if (index == d_ptr->current) {
        return State::Current;
    }
    return State::Upcoming;
}

QSize MaterialStepper::sizeHint() const
{
    const qreal height = kRingRoom + kCircle + kLabelGap + kLabelHeight + (d_ptr->saysCaption() ? kCaptionHeight : 0.0);
    const qreal width = qMax(qreal(count()), 1.0) * kMinColumn + 2.0 * kPadding;
    return QSize(int(width), int(height));
}

QSize MaterialStepper::minimumSizeHint() const
{
    return sizeHint();
}

void MaterialStepper::setCurrentStep(int step)
{
    if (step < 0 || step >= count() || step == d_ptr->current) {
        return;
    }
    d_ptr->current = step;
    update();
    Q_EMIT currentChanged(step);
}

QRectF MaterialStepper::columnRect(int index) const
{
    const qreal columnWidth = (qreal(width()) - 2.0 * kPadding) / qMax(count(), 1);
    return QRectF(kPadding + qreal(index) * columnWidth, 0.0, columnWidth, qreal(height()));
}

QRectF MaterialStepper::circleRect(int index) const
{
    const QRectF column = columnRect(index);
    return QRectF(column.center().x() - kCircle / 2.0, kRingRoom, kCircle, kCircle);
}

int MaterialStepper::stepAt(const QPointF &pos) const
{
    QVector<QRectF> columns;
    columns.reserve(count());
    for (int i = 0; i < count(); ++i) {
        columns.append(columnRect(i));
    }
    return d_ptr->indexOf(pos, columns);
}

void MaterialStepper::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event)
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);

    const QColor accent = QxTheme::QxThemeManager::instance()->accentColor();
    const QColor page = palette().color(QPalette::Window);
    const QColor ink = palette().color(QPalette::WindowText);
    const QColor quiet = QxControls::QxStateLayer::layered(page, ink, kQuietInkOpacity);
    const QColor outline = QxControls::QxStateLayer::layered(page, ink, kOutlineOpacity);
    const QColor walkedLine = QxControls::QxStateLayer::layered(page, accent, kConnectorWalkedOpacity);

    // The line first: it goes from circle to circle, under everything else.
    for (int i = 0; i + 1 < count(); ++i) {
        const QRectF from = circleRect(i);
        const QRectF to = circleRect(i + 1);
        const QRectF line(from.right(), from.center().y() - kConnector / 2.0, to.left() - from.right(), kConnector);
        painter.fillRect(line, i < d_ptr->current ? walkedLine : outline);
    }

    for (int i = 0; i < count(); ++i) {
        const QRectF circle = circleRect(i);
        switch (stepState(i)) {
        case State::Completed: {
            painter.setPen(Qt::NoPen);
            painter.setBrush(accent);
            painter.drawEllipse(circle);
            // The tick: the same answer a chosen box gives.
            QPainterPath tick;
            tick.moveTo(circle.center().x() - 5.0, circle.center().y());
            tick.lineTo(circle.center().x() - 1.5, circle.center().y() + 4.0);
            tick.lineTo(circle.center().x() + 5.5, circle.center().y() - 4.5);
            painter.setBrush(Qt::NoBrush);
            painter.setPen(QPen(QColor(Qt::white), 2.0, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
            painter.drawPath(tick);
            break;
        }
        case State::Current: {
            painter.setBrush(Qt::NoBrush);
            painter.setPen(QPen(accent, 2.0));
            painter.drawEllipse(circle.adjusted(1.0, 1.0, -1.0, -1.0));
            painter.setPen(accent);
            painter.setFont(MaterialType::font(MaterialType::Scale::Label, true));
            painter.drawText(circle, Qt::AlignCenter, QString::number(i + 1));
            break;
        }
        case State::Upcoming: {
            painter.setBrush(Qt::NoBrush);
            painter.setPen(QPen(outline, 1.0));
            painter.drawEllipse(circle.adjusted(0.5, 0.5, -0.5, -0.5));
            painter.setPen(quiet);
            painter.setFont(MaterialType::font(MaterialType::Scale::Label, true));
            painter.drawText(circle, Qt::AlignCenter, QString::number(i + 1));
            break;
        }
        }

        // The name, and the caption under it when the step says one.
        const QRectF column = columnRect(i);
        const QRectF nameRect(column.left(), circle.bottom() + kLabelGap, column.width(), kLabelHeight);
        painter.setPen(stepState(i) == State::Current ? accent : ink);
        painter.setFont(MaterialType::font(MaterialType::Scale::Body, stepState(i) == State::Current));
        painter.drawText(nameRect, Qt::AlignHCenter | Qt::AlignTop | Qt::TextSingleLine, d_ptr->steps.at(i).label);
        if (!d_ptr->steps.at(i).caption.isEmpty()) {
            const QRectF captionRect(column.left(), nameRect.bottom(), column.width(), kCaptionHeight);
            painter.setPen(quiet);
            painter.setFont(MaterialType::font(MaterialType::Scale::Label));
            painter.drawText(captionRect, Qt::AlignHCenter | Qt::AlignTop | Qt::TextSingleLine,
                             d_ptr->steps.at(i).caption);
        }

        // The row has the keyboard: the step in force says so with a ring
        // just outside its own.
        if (hasFocus() && i == d_ptr->current) {
            painter.setBrush(Qt::NoBrush);
            painter.setPen(QPen(accent, 1.0));
            painter.drawEllipse(
                circle.adjusted(-kRingRoom, -kRingRoom, kRingRoom, kRingRoom).adjusted(1.0, 1.0, -1.0, -1.0));
        }
    }
}

void MaterialStepper::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        const int step = stepAt(eventPos(event));
        // Only a step that is behind can be taken again: the row does not
        // walk ahead of the work.
        if (step >= 0 && step < d_ptr->current) {
            setCurrentStep(step);
        }
    }
    QWidget::mouseReleaseEvent(event);
}

void MaterialStepper::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Left || event->key() == Qt::Key_Up) {
        if (d_ptr->current > 0) {
            setCurrentStep(d_ptr->current - 1);
        }
        event->accept();
        return;
    }
    QWidget::keyPressEvent(event);
}

QX_MATERIAL_END_NAMESPACE
