/**
 * Copyleft (C) 2023 maminjie <canpool@163.com>
 * SPDX-License-Identifier: BSD-3-Clause
 **/
#include "materialcircularprogress.h"
#include "materialcircularprogress_p.h"
#include "materialstyle.h"
#include <QPainter>
#include <QPainterPath>
#include <QParallelAnimationGroup>
#include <QPropertyAnimation>

QX_MATERIAL_BEGIN_NAMESPACE

/*!
 *  \class MaterialCircularProgressDelegate
 *  \internal
 */

MaterialCircularProgressDelegate::MaterialCircularProgressDelegate(MaterialCircularProgress *parent)
    : QObject(parent)
    , m_progress(parent)
    , m_dashOffset(0)
    , m_dashLength(89)
    , m_angle(0)
{
    Q_ASSERT(parent);
}

MaterialCircularProgressDelegate::~MaterialCircularProgressDelegate()
{
}

/*!
 *  \class MaterialCircularProgressPrivate
 *  \internal
 */

MaterialCircularProgressPrivate::MaterialCircularProgressPrivate(MaterialCircularProgress *q)
    : q_ptr(q)
{
}

MaterialCircularProgressPrivate::~MaterialCircularProgressPrivate()
{
}

void MaterialCircularProgressPrivate::init()
{
    Q_Q(MaterialCircularProgress);

    delegate = new MaterialCircularProgressDelegate(q);
    progressType = QxMaterial::IndeterminateProgress;
    penWidth = 6.25;
    size = 64;
    useThemeColors = true;

    q->setSizePolicy(QSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding));

    QParallelAnimationGroup *group = new QParallelAnimationGroup(q);
    group->setLoopCount(-1);

    QPropertyAnimation *animation;

    animation = new QPropertyAnimation(q);
    animation->setPropertyName("dashLength");
    animation->setTargetObject(delegate);
    animation->setEasingCurve(QEasingCurve::InOutQuad);
    animation->setStartValue(0.1);
    animation->setKeyValueAt(0.15, 0.2);
    animation->setKeyValueAt(0.6, 20);
    animation->setKeyValueAt(0.7, 20);
    animation->setEndValue(20);
    animation->setDuration(2050);

    group->addAnimation(animation);

    animation = new QPropertyAnimation(q);
    animation->setPropertyName("dashOffset");
    animation->setTargetObject(delegate);
    animation->setEasingCurve(QEasingCurve::InOutSine);
    animation->setStartValue(0);
    animation->setKeyValueAt(0.15, 0);
    animation->setKeyValueAt(0.6, -7);
    animation->setKeyValueAt(0.7, -7);
    animation->setEndValue(-25);
    animation->setDuration(2050);

    group->addAnimation(animation);

    animation = new QPropertyAnimation(q);
    animation->setPropertyName("angle");
    animation->setTargetObject(delegate);
    animation->setStartValue(0);
    animation->setEndValue(719);
    animation->setDuration(2050);

    group->addAnimation(animation);

    group->start();
}

/*!
 *  \class MaterialCircularProgress
 */

MaterialCircularProgress::MaterialCircularProgress(QWidget *parent)
    : QProgressBar(parent), d_ptr(new MaterialCircularProgressPrivate(this))
{
    d_func()->init();
}

MaterialCircularProgress::~MaterialCircularProgress()
{
}

void MaterialCircularProgress::setProgressType(QxMaterial::ProgressType type)
{
    Q_D(MaterialCircularProgress);

    d->progressType = type;
    update();
}

QxMaterial::ProgressType MaterialCircularProgress::progressType() const
{
    Q_D(const MaterialCircularProgress);

    return d->progressType;
}

void MaterialCircularProgress::setUseThemeColors(bool value)
{
    Q_D(MaterialCircularProgress);

    if (d->useThemeColors == value) {
        return;
    }

    d->useThemeColors = value;
    update();
}

bool MaterialCircularProgress::useThemeColors() const
{
    Q_D(const MaterialCircularProgress);

    return d->useThemeColors;
}

void MaterialCircularProgress::setLineWidth(qreal width)
{
    Q_D(MaterialCircularProgress);

    d->penWidth = width;
    update();
    updateGeometry();
}

qreal MaterialCircularProgress::lineWidth() const
{
    Q_D(const MaterialCircularProgress);

    return d->penWidth;
}

void MaterialCircularProgress::setSize(int size)
{
    Q_D(MaterialCircularProgress);

    d->size = size;
    update();
    updateGeometry();
}

int MaterialCircularProgress::size() const
{
    Q_D(const MaterialCircularProgress);

    return d->size;
}

void MaterialCircularProgress::setColor(const QColor &color)
{
    Q_D(MaterialCircularProgress);

    d->color = color;

    MATERIAL_DISABLE_THEME_COLORS
    update();
}

QColor MaterialCircularProgress::color() const
{
    Q_D(const MaterialCircularProgress);

    if (d->useThemeColors || !d->color.isValid()) {
        return MaterialStyle::instance().themeColor(MK_primary1);
    } else {
        return d->color;
    }
}

/*!
 *  \reimp
 */
QSize MaterialCircularProgress::sizeHint() const
{
    Q_D(const MaterialCircularProgress);

    const qreal s = d->size + d->penWidth + 8;
    return QSize(s, s);
}

/*!
 *  \reimp
 */
void MaterialCircularProgress::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event)

    Q_D(MaterialCircularProgress);

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    if (!isEnabled()) {
        QPen pen;
        pen.setCapStyle(Qt::RoundCap);
        pen.setWidthF(d->penWidth);
        pen.setColor(MaterialStyle::instance().themeColor(MK_border));
        painter.setPen(pen);
        painter.drawLine(rect().center() - QPointF(20, 20), rect().center() + QPointF(20, 20));
        painter.drawLine(rect().center() + QPointF(20, -20), rect().center() - QPointF(20, -20));
        return;
    }

    if (QxMaterial::IndeterminateProgress == d->progressType) {
        painter.translate(width() / 2, height() / 2);
        painter.rotate(d->delegate->angle());
    }

    QPen pen;
    pen.setCapStyle(Qt::RoundCap);
    pen.setWidthF(d->penWidth);
    pen.setColor(color());

    if (QxMaterial::IndeterminateProgress == d->progressType) {
        QVector<qreal> pattern;
        pattern << d->delegate->dashLength() * d->size / 50 << 30 * d->size / 50;

        pen.setDashOffset(d->delegate->dashOffset() * d->size / 50);
        pen.setDashPattern(pattern);

        painter.setPen(pen);

        painter.drawEllipse(QPoint(0, 0), d->size / 2, d->size / 2);
    } else {
        painter.setPen(pen);

        const qreal x = (width() - d->size) / 2;
        const qreal y = (height() - d->size) / 2;

        const qreal a = 360 * (value() - minimum()) / (maximum() - minimum());

        QPainterPath path;
        path.arcMoveTo(x, y, d->size, d->size, 0);
        path.arcTo(x, y, d->size, d->size, 0, a);

        painter.drawPath(path);
    }
}

QX_MATERIAL_END_NAMESPACE
