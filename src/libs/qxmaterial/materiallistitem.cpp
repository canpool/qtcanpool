/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#include "materiallistitem.h"

#include "materialtokens.h"

#include "qxcontrols/qxstatelayer.h"

#include <QtCore/QtMath>
#include <QtGui/QFontMetrics>
#include <QtGui/QKeyEvent>
#include <QtGui/QPainter>

QX_MATERIAL_BEGIN_NAMESPACE

namespace
{
// Material's list row measures: forty-eight tall saying one line,
// seventy-two saying two; sixteen on each side, an icon of twenty-four
// standing before the words with sixteen between them.
constexpr qreal kOneLineHeight = 48.0;
constexpr qreal kTwoLineHeight = 72.0;
constexpr qreal kPadding = 16.0;
constexpr qreal kIconSize = 24.0;
constexpr qreal kIconGap = 16.0;
constexpr qreal kSecondaryOpacity = 0.6;
constexpr qreal kSelectedWashOpacity = 0.08;
constexpr qreal kDisabledOpacity = 0.38;
}   // namespace

class MaterialListItemPrivate
{
public:
    QX_DECLARE_PUBLIC(MaterialListItem)

    QString text;
    QString secondaryText;
    QIcon icon;
    bool selected = false;
    QWidget *trailing = Q_NULLPTR;

    /*! The x where the words begin: past the icon when there is one. */
    qreal textLeft() const;
    /*! The x where the words end: before the trailing widget when there is
     *  one. */
    qreal textRight() const;
    /*! Puts the trailing widget where it stands: at the trailing edge, on
     *  the middle line. */
    void placeTrailing();
};

qreal MaterialListItemPrivate::textLeft() const
{
    return q_ptr->icon().isNull() ? kPadding : kPadding + kIconSize + kIconGap;
}

qreal MaterialListItemPrivate::textRight() const
{
    const qreal width = qreal(q_ptr->width());
    return trailing ? width - kPadding - qreal(trailing->width()) - kIconGap : width - kPadding;
}

void MaterialListItemPrivate::placeTrailing()
{
    if (!trailing) {
        return;
    }
    // The slot offers the widget its own size first; what it stands at is
    // what it is - a widget with a size of its own fixed keeps that.
    trailing->resize(trailing->sizeHint());
    const QSize size = trailing->size();
    trailing->move(int(qreal(q_ptr->width()) - kPadding - qreal(size.width())),
                   int((qreal(q_ptr->height()) - qreal(size.height())) / 2.0));
}

MaterialListItem::MaterialListItem(QWidget *parent)
    : QxControls::QxControl(parent)
    , d_ptr(new MaterialListItemPrivate)
{
    QX_SET_PRIVATE(d_ptr)
    setFocusPolicy(Qt::StrongFocus);
}

MaterialListItem::~MaterialListItem(){QX_FINI_PRIVATE()}

QString MaterialListItem::text() const
{
    return d_ptr->text;
}

QString MaterialListItem::secondaryText() const
{
    return d_ptr->secondaryText;
}

QIcon MaterialListItem::icon() const
{
    return d_ptr->icon;
}

bool MaterialListItem::isSelected() const
{
    return d_ptr->selected;
}

QWidget *MaterialListItem::trailingWidget() const
{
    return d_ptr->trailing;
}

void MaterialListItem::setTrailingWidget(QWidget *widget)
{
    if (d_ptr->trailing == widget) {
        return;
    }
    if (d_ptr->trailing) {
        d_ptr->trailing->hide();
        d_ptr->trailing->deleteLater();
    }
    d_ptr->trailing = widget;
    if (d_ptr->trailing) {
        d_ptr->trailing->setParent(this);
        d_ptr->trailing->show();
    }
    d_ptr->placeTrailing();
    updateGeometry();
    update();
}

void MaterialListItem::setText(const QString &text)
{
    if (d_ptr->text == text) {
        return;
    }
    d_ptr->text = text;
    Q_EMIT textChanged(text);
    updateGeometry();
    update();
}

void MaterialListItem::setSecondaryText(const QString &text)
{
    if (d_ptr->secondaryText == text) {
        return;
    }
    d_ptr->secondaryText = text;
    Q_EMIT secondaryTextChanged(text);
    updateGeometry();
    update();
}

void MaterialListItem::setIcon(const QIcon &icon)
{
    if (d_ptr->icon.cacheKey() == icon.cacheKey()) {
        return;
    }
    d_ptr->icon = icon;
    Q_EMIT iconChanged(icon);
    updateGeometry();
    update();
}

void MaterialListItem::setSelected(bool selected)
{
    if (d_ptr->selected == selected) {
        return;
    }
    d_ptr->selected = selected;
    Q_EMIT selectedChanged(selected);
    update();
}

QSize MaterialListItem::sizeHint() const
{
    const QFontMetrics body(MaterialType::font(MaterialType::Scale::Body));
    const QFontMetrics caption(MaterialType::font(MaterialType::Scale::Label));
    const qreal textLeft = d_ptr->textLeft();
    const qreal trailingWidth = d_ptr->trailing ? qreal(d_ptr->trailing->width()) + kIconGap : 0.0;
    qreal width = textLeft + trailingWidth + kPadding;
    if (!d_ptr->text.isEmpty()) {
        width += body.horizontalAdvance(d_ptr->text);
    }
    if (!d_ptr->secondaryText.isEmpty()) {
        width = qMax(width, textLeft + trailingWidth + kPadding + caption.horizontalAdvance(d_ptr->secondaryText));
    }
    const qreal height = d_ptr->secondaryText.isEmpty() ? kOneLineHeight : kTwoLineHeight;
    return QSize(qCeil(width), int(height));
}

QSize MaterialListItem::minimumSizeHint() const
{
    return sizeHint();
}

qreal MaterialListItem::cornerRadius() const
{
    // A row is all one thing and runs edge to edge of the list it stands in.
    return 0.0;
}

QColor MaterialListItem::washColor() const
{
    return QxControls::QxControl::washColor();
}

void MaterialListItem::paintSurface(QPainter *painter)
{
    // The chosen row keeps the accent under everything else - under the
    // wash of being pointed at, over the page.
    if (d_ptr->selected) {
        painter->fillRect(
            QRectF(0.0, 0.0, qreal(width()), qreal(height())),
            QxControls::QxStateLayer::layered(palette().color(QPalette::Window), accentColor(), kSelectedWashOpacity));
    }
}

void MaterialListItem::paintContent(QPainter *painter)
{
    const qreal height = qreal(this->height());
    const qreal textLeft = d_ptr->textLeft();
    const qreal textRight = d_ptr->textRight();
    const bool twoLines = !d_ptr->secondaryText.isEmpty();

    painter->save();
    painter->setOpacity(isEnabled() ? 1.0 : kDisabledOpacity);

    if (!d_ptr->icon.isNull()) {
        const QRectF iconRect(kPadding, (height - kIconSize) / 2.0, kIconSize, kIconSize);
        d_ptr->icon.paint(painter, iconRect.toRect(), Qt::AlignCenter, isEnabled() ? QIcon::Normal : QIcon::Disabled);
    }

    const QRectF room(textLeft, 0.0, qMax(0.0, textRight - textLeft), height);
    painter->setFont(MaterialType::font(MaterialType::Scale::Body));
    // The chosen row's words are the accent's, which is the pointing a
    // chosen row does.
    painter->setPen(d_ptr->selected ? accentColor() : textColor());
    if (twoLines) {
        painter->drawText(QRectF(room.left(), 0.0, room.width(), 40.0),
                          Qt::AlignBottom | Qt::AlignLeft | Qt::TextSingleLine, d_ptr->text);
        painter->setFont(MaterialType::font(MaterialType::Scale::Label));
        painter->setPen(
            QxControls::QxStateLayer::layered(palette().color(QPalette::Window), textColor(), kSecondaryOpacity));
        painter->drawText(QRectF(room.left(), 40.0, room.width(), height - 40.0),
                          Qt::AlignTop | Qt::AlignLeft | Qt::TextSingleLine, d_ptr->secondaryText);
    } else {
        painter->drawText(room, Qt::AlignCenter | Qt::TextSingleLine, d_ptr->text);
    }
    painter->restore();
}

void MaterialListItem::resizeEvent(QResizeEvent *event)
{
    QxControls::QxControl::resizeEvent(event);
    d_ptr->placeTrailing();
}

void MaterialListItem::keyPressEvent(QKeyEvent *event)
{
    const int key = event->key();
    if (key == Qt::Key_Enter || key == Qt::Key_Return) {
        Q_EMIT clicked();
        event->accept();
        return;
    }
    if (key == Qt::Key_Space) {
        event->accept();
        return;
    }
    QxControls::QxControl::keyPressEvent(event);
}

void MaterialListItem::keyReleaseEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Space && !event->isAutoRepeat()) {
        Q_EMIT clicked();
        event->accept();
        return;
    }
    QxControls::QxControl::keyReleaseEvent(event);
}

QAccessible::Role MaterialListItem::accessibleRole() const
{
    return QAccessible::ListItem;
}

void MaterialListItem::fillAccessibleState(QAccessible::State *state) const
{
    QxControls::QxControl::fillAccessibleState(state);
    state->selectable = true;
    state->selected = d_ptr->selected;
}

QX_MATERIAL_END_NAMESPACE
