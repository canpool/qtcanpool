/**
 * Copyleft (C) 2023 maminjie <canpool@163.com>
 * SPDX-License-Identifier: BSD-3-Clause
 **/
#include "materialbadge.h"
#include "materialbadge_p.h"
#include "materialstyle.h"
#include <QPainter>

QX_MATERIAL_BEGIN_NAMESPACE

/*!
 *  \class MaterialBadgePrivate
 *  \internal
 */

/*!
 *  \internal
 */
MaterialBadgePrivate::MaterialBadgePrivate(MaterialBadge *q)
    : q_ptr(q)
{
}

/*!
 *  \internal
 */
MaterialBadgePrivate::~MaterialBadgePrivate()
{
}

/*!
 *  \internal
 */
void MaterialBadgePrivate::init()
{
    Q_Q(MaterialBadge);

    x = 0;
    y = 0;
    padding = 10;
    useThemeColors = true;

    q->setAttribute(Qt::WA_TransparentForMouseEvents);

    QFont font(q->font());
    font.setPointSizeF(10);
    font.setStyleName("Bold");
    q->setFont(font);

    q->setText("+1");
}

/*!
 *  \class MaterialBadge
 */

MaterialBadge::MaterialBadge(QWidget *parent) : MaterialOverlayWidget(parent), d_ptr(new MaterialBadgePrivate(this))
{
    d_func()->init();
}

MaterialBadge::MaterialBadge(const QIcon &icon, QWidget *parent)
    : MaterialOverlayWidget(parent), d_ptr(new MaterialBadgePrivate(this))
{
    d_func()->init();

    setIcon(icon);
}

MaterialBadge::MaterialBadge(const QString &text, QWidget *parent)
    : MaterialOverlayWidget(parent), d_ptr(new MaterialBadgePrivate(this))
{
    d_func()->init();

    setText(text);
}

MaterialBadge::~MaterialBadge()
{
}

void MaterialBadge::setUseThemeColors(bool value)
{
    Q_D(MaterialBadge);

    if (d->useThemeColors == value) {
        return;
    }

    d->useThemeColors = value;
    update();
}

bool MaterialBadge::useThemeColors() const
{
    Q_D(const MaterialBadge);

    return d->useThemeColors;
}

void MaterialBadge::setTextColor(const QColor &color)
{
    Q_D(MaterialBadge);

    d->textColor = color;

    MATERIAL_DISABLE_THEME_COLORS
    update();
}

QColor MaterialBadge::textColor() const
{
    Q_D(const MaterialBadge);

    if (d->useThemeColors || !d->textColor.isValid()) {
        return MaterialStyle::instance().themeColor(MK_canvas);
    } else {
        return d->textColor;
    }
}

void MaterialBadge::setBackgroundColor(const QColor &color)
{
    Q_D(MaterialBadge);

    d->backgroundColor = color;

    MATERIAL_DISABLE_THEME_COLORS
    update();
}

QColor MaterialBadge::backgroundColor() const
{
    Q_D(const MaterialBadge);

    if (d->useThemeColors || !d->backgroundColor.isValid()) {
        return MaterialStyle::instance().themeColor(MK_accent1);
    } else {
        return d->backgroundColor;
    }
}

void MaterialBadge::setRelativePosition(const QPointF &pos)
{
    setRelativePosition(pos.x(), pos.y());
}

void MaterialBadge::setRelativePosition(qreal x, qreal y)
{
    Q_D(MaterialBadge);

    d->x = x;
    d->y = y;
    update();
}

QPointF MaterialBadge::relativePosition() const
{
    Q_D(const MaterialBadge);

    return QPointF(d->x, d->y);
}

void MaterialBadge::setRelativeXPosition(qreal x)
{
    Q_D(MaterialBadge);

    d->x = x;
    update();
}

qreal MaterialBadge::relativeXPosition() const
{
    Q_D(const MaterialBadge);

    return d->x;
}

void MaterialBadge::setRelativeYPosition(qreal y)
{
    Q_D(MaterialBadge);

    d->y = y;
    update();
}

qreal MaterialBadge::relativeYPosition() const
{
    Q_D(const MaterialBadge);

    return d->y;
}

/*!
 *  \reimp
 */
QSize MaterialBadge::sizeHint() const
{
    const int s = getDiameter();
    return QSize(s + 4, s + 4);
}

void MaterialBadge::setIcon(const QIcon &icon)
{
    Q_D(MaterialBadge);

    d->icon = icon;
    update();
}

QIcon MaterialBadge::icon() const
{
    Q_D(const MaterialBadge);

    return d->icon;
}

void MaterialBadge::setText(const QString &text)
{
    Q_D(MaterialBadge);

    d->text = text;

    if (!d->icon.isNull()) {
        d->icon = QIcon();
    }

    d->size = fontMetrics().size(Qt::TextShowMnemonic, text);

    update();
}

QString MaterialBadge::text() const
{
    Q_D(const MaterialBadge);

    return d->text;
}

/*!
 *  \reimp
 */
void MaterialBadge::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event)

    Q_D(MaterialBadge);

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    painter.translate(d->x, d->y);

    QBrush brush;
    brush.setStyle(Qt::SolidPattern);
    brush.setColor(isEnabled() ? backgroundColor() : MaterialStyle::instance().themeColor(MK_disabled1));
    painter.setBrush(brush);
    painter.setPen(Qt::NoPen);

    const int s = getDiameter();

    QRectF r(0, 0, s, s);
    r.translate(QPointF((width() - s), (height() - s)) / 2);

    if (d->icon.isNull()) {
        painter.drawEllipse(r);
        painter.setPen(textColor());
        painter.setBrush(Qt::NoBrush);
        painter.drawText(r.translated(0, -0.5), Qt::AlignCenter, d->text);
    } else {
        painter.drawEllipse(r);
        QRectF q(0, 0, 16, 16);
        q.moveCenter(r.center());
        QPixmap pixmap = icon().pixmap(16, 16);
        QPainter icon(&pixmap);
        icon.setCompositionMode(QPainter::CompositionMode_SourceIn);
        icon.fillRect(pixmap.rect(), textColor());
        painter.drawPixmap(q.toRect(), pixmap);
    }
}

int MaterialBadge::getDiameter() const
{
    Q_D(const MaterialBadge);

    if (d->icon.isNull()) {
        return qMax(d->size.width(), d->size.height()) + d->padding;
    } else {
        return 24;
    }
}

QX_MATERIAL_END_NAMESPACE
