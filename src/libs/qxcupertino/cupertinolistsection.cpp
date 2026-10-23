/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#include "cupertinolistsection.h"

#include "cupertinotokens.h"

#include "qxcontrols/qxstatelayer.h"

#include <QtGui/QIcon>
#include <QtGui/QKeyEvent>
#include <QtGui/QPainter>
#include <QtGui/QPainterPath>
#include <QtWidgets/QLabel>
#include <QtWidgets/QVBoxLayout>

QX_CUPERTINO_BEGIN_NAMESPACE

namespace
{
/*! The room a row keeps: sixteen from the card's edges, and the icon square
 *  with a gap to the text after it. */
constexpr int kInset = 16;
constexpr int kIconSide = 24;
constexpr int kIconGap = 12;
/*! A row's two heights: with a second line under the title, and without. */
constexpr int kRowHeight = 44;
constexpr int kTwoLineRowHeight = 60;
/*! The card's corner. This is the family's `Large` - a group is a control with
 *  room, not a sheet. */
constexpr int kCardRadius = 10;
/*! The line between two rows, and how far the ink of it sits from the text. */
constexpr qreal kLineOpacity = 0.2;
/*! The mark at the end of a row is drawn in this box. */
constexpr qreal kMarkSide = 8.0;

bool onDarkFace(const QWidget *widget)
{
    return widget->palette().color(QPalette::Window).lightness() < 128;
}

/*! A rounded rectangle with the top corners, the bottom ones, both or none -
 *  the shape a row of a card turns. */
QPainterPath rowPath(const QRectF &rect, qreal radius, bool top, bool bottom)
{
    const qreal r = qMin(radius, rect.height() / 2.0);
    QPainterPath path;
    path.moveTo(rect.left() + (top ? r : 0), rect.top());
    path.lineTo(rect.right() - (top ? r : 0), rect.top());
    if (top) {
        path.quadTo(rect.right(), rect.top(), rect.right(), rect.top() + r);
    }
    path.lineTo(rect.right(), rect.bottom() - (bottom ? r : 0));
    if (bottom) {
        path.quadTo(rect.right(), rect.bottom(), rect.right() - r, rect.bottom());
    }
    path.lineTo(rect.left() + (bottom ? r : 0), rect.bottom());
    if (bottom) {
        path.quadTo(rect.left(), rect.bottom(), rect.left(), rect.bottom() - r);
    }
    path.lineTo(rect.left(), rect.top() + (top ? r : 0));
    if (top) {
        path.quadTo(rect.left(), rect.top(), rect.left() + r, rect.top());
    }
    path.closeSubpath();
    return path;
}
}   // namespace

class CupertinoListTilePrivate
{
public:
    QX_DECLARE_PUBLIC(CupertinoListTile)

    QString title;
    QString subtitle;
    QIcon icon;
    CupertinoListTile::Trailing trailing = CupertinoListTile::Trailing::None;
    CupertinoListTile::Position position = CupertinoListTile::Position::Only;
    QWidget *trailingWidget = Q_NULLPTR;

    bool topCorner() const
    {
        return position == CupertinoListTile::Position::Only || position == CupertinoListTile::Position::First;
    }
    bool bottomCorner() const
    {
        return position == CupertinoListTile::Position::Only || position == CupertinoListTile::Position::Last;
    }
    /*! Where the text starts: after the icon when there is one. */
    qreal textLeft() const
    {
        return icon.isNull() ? kInset : kInset + kIconSide + kIconGap;
    }
};

CupertinoListTile::CupertinoListTile(QWidget *parent)
    : QxControls::QxControl(parent)
    , d_ptr(new CupertinoListTilePrivate)
{
    QX_SET_PRIVATE(d_ptr)
    setFocusPolicy(Qt::StrongFocus);
    setCursor(Qt::PointingHandCursor);
    // The base already knows whether a press began and ended on the row; the
    // row reports that as its own click and leaves what it means to the
    // application.
    connect(this, &QxControls::QxControl::clicked, this, &CupertinoListTile::clicked);
}

CupertinoListTile::~CupertinoListTile(){QX_FINI_PRIVATE()}

QString CupertinoListTile::title() const
{
    return d_ptr->title;
}

QString CupertinoListTile::subtitle() const
{
    return d_ptr->subtitle;
}

QIcon CupertinoListTile::icon() const
{
    return d_ptr->icon;
}

CupertinoListTile::Trailing CupertinoListTile::trailing() const
{
    return d_ptr->trailing;
}

CupertinoListTile::Position CupertinoListTile::position() const
{
    return d_ptr->position;
}

QWidget *CupertinoListTile::trailingWidget() const
{
    return d_ptr->trailingWidget;
}

QSize CupertinoListTile::sizeHint() const
{
    return QSize(320, d_ptr->subtitle.isEmpty() ? kRowHeight : kTwoLineRowHeight);
}

QSize CupertinoListTile::minimumSizeHint() const
{
    return QSize(120, sizeHint().height());
}

void CupertinoListTile::setTitle(const QString &title)
{
    d_ptr->title = title;
    update();
}

void CupertinoListTile::setSubtitle(const QString &subtitle)
{
    const bool hadSubtitle = !d_ptr->subtitle.isEmpty();
    d_ptr->subtitle = subtitle;
    const bool hasSubtitle = !subtitle.isEmpty();
    if (hadSubtitle != hasSubtitle) {
        // The row grows or shrinks by a line, so the group above it has to
        // hear about it.
        updateGeometry();
        update();
        return;
    }
    update();
}

void CupertinoListTile::setIcon(const QIcon &icon)
{
    d_ptr->icon = icon;
    update();
}

void CupertinoListTile::setTrailing(Trailing trailing)
{
    d_ptr->trailing = trailing;
    update();
}

void CupertinoListTile::setTrailingWidget(QWidget *widget)
{
    if (d_ptr->trailingWidget == widget) {
        return;
    }
    if (d_ptr->trailingWidget) {
        d_ptr->trailingWidget->deleteLater();
    }
    d_ptr->trailingWidget = widget;
    if (widget) {
        widget->setParent(this);
        widget->show();
        layoutTrailing();
    }
    update();
}

void CupertinoListTile::setPosition(Position position)
{
    if (d_ptr->position == position) {
        return;
    }
    d_ptr->position = position;
    update();
}

QRectF CupertinoListTile::chromeRect() const
{
    // The row paints its own wash: the base's is one rectangle of one radius,
    // and a row of a card turns only the corners the card turns there.
    return QRectF();
}

void CupertinoListTile::paintSurface(QPainter *painter)
{
    const QRectF area = QRectF(rect());
    const QPainterPath card = rowPath(area, kCardRadius, d_ptr->topCorner(), d_ptr->bottomCorner());

    painter->save();
    painter->setRenderHint(QPainter::Antialiasing, true);
    painter->setPen(Qt::NoPen);
    painter->setBrush(palette().color(QPalette::Base));
    painter->drawPath(card);

    if (isEnabled() && state() != QxControls::QxControl::State::Rest) {
        // The wash is the shared state layer, clipped to the row's own shape:
        // a row of a card turns only the corners its place in the card turns,
        // and the layer draws rectangles. The opacities are the layer's own, so
        // a row reads the same under the pointer as every other control here.
        using QxControls::QxControl;
        namespace StateLayer = QxControls::QxStateLayer;
        const qreal opacity =
            state() == QxControl::State::Pressed ? StateLayer::pressedOpacity : StateLayer::hoverOpacity;
        painter->save();
        painter->setClipPath(card);
        StateLayer::paint(painter, area, washColor(), opacity);
        painter->restore();
    }

    // The line that separates this row from the one under it: it starts where
    // the text starts, because a line that ran to the card's edge would cut the
    // card rather than separate two rows on it.
    if (d_ptr->position != Position::Last && d_ptr->position != Position::Only) {
        QColor line = CupertinoColor::color(CupertinoColor::Role::Gray, onDarkFace(this));
        line.setAlphaF(kLineOpacity);
        painter->setPen(QPen(line, 1));
        painter->drawLine(QPointF(d_ptr->textLeft(), area.bottom() - 0.5),
                          QPointF(area.right() - kInset, area.bottom() - 0.5));
    }

    if (hasFocus()) {
        const QPainterPath outline =
            rowPath(area.adjusted(0.8, 0.8, -0.8, -0.8), kCardRadius, d_ptr->topCorner(), d_ptr->bottomCorner());
        painter->setBrush(Qt::NoBrush);
        painter->setPen(QPen(accentColor(), 1.6));
        painter->drawPath(outline);
    }

    painter->restore();
}

void CupertinoListTile::paintContent(QPainter *painter)
{
    const bool twoLines = !d_ptr->subtitle.isEmpty();
    const qreal ink = d_ptr->textLeft();

    painter->save();
    painter->setRenderHint(QPainter::Antialiasing, true);

    if (!d_ptr->icon.isNull()) {
        const QRect iconRect(kInset, int((height() - kIconSide) / 2.0), kIconSide, kIconSide);
        d_ptr->icon.paint(painter, iconRect, Qt::AlignCenter, isEnabled() ? QIcon::Normal : QIcon::Disabled);
    }

    QColor titleInk = textColor();
    if (!isEnabled()) {
        titleInk.setAlphaF(0.4);
    }
    qreal trailingRoom = kInset;
    if (d_ptr->trailing != Trailing::None) {
        trailingRoom += kMarkSide + kIconGap;
    } else if (d_ptr->trailingWidget) {
        trailingRoom += d_ptr->trailingWidget->sizeHint().width() + kIconGap;
    }

    if (twoLines) {
        // The title sits above the middle and the second line under it, so the
        // two read as one block rather than as two rows' worth of text.
        const QRectF titleRect(ink, height() / 2.0 - 20.0, width() - ink - trailingRoom, 22.0);
        const QRectF subtitleRect(ink, height() / 2.0 + 2.0, width() - ink - trailingRoom, 18.0);
        painter->setPen(titleInk);
        painter->setFont(CupertinoType::font(CupertinoType::Ramp::Body));
        painter->drawText(titleRect, Qt::AlignLeft | Qt::AlignVCenter, d_ptr->title);

        QColor subtitleInk = titleInk;
        subtitleInk.setAlphaF(0.55);
        painter->setPen(subtitleInk);
        painter->setFont(CupertinoType::font(CupertinoType::Ramp::Caption));
        painter->drawText(subtitleRect, Qt::AlignLeft | Qt::AlignVCenter, d_ptr->subtitle);
    } else {
        const QRectF titleRect(ink, 0, width() - ink - trailingRoom, height());
        painter->setPen(titleInk);
        painter->setFont(CupertinoType::font(CupertinoType::Ramp::Body));
        painter->drawText(titleRect, Qt::AlignLeft | Qt::AlignVCenter, d_ptr->title);
    }

    const QPointF markCentre(width() - kInset - kMarkSide / 2.0, height() / 2.0);
    if (d_ptr->trailing == Trailing::Chevron) {
        // A turn to the right, in the family's grey: it says the row goes
        // somewhere and says nothing about where.
        QColor chevron = CupertinoColor::color(CupertinoColor::Role::Gray, onDarkFace(this));
        chevron.setAlphaF(0.6);
        painter->setPen(QPen(chevron, 1.6, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
        painter->drawLine(QPointF(markCentre.x() - 3.0, markCentre.y() - 4.5),
                          QPointF(markCentre.x() + 1.0, markCentre.y()));
        painter->drawLine(QPointF(markCentre.x() + 1.0, markCentre.y()),
                          QPointF(markCentre.x() - 3.0, markCentre.y() + 4.5));
    } else if (d_ptr->trailing == Trailing::Checkmark) {
        painter->setPen(QPen(CupertinoColor::color(CupertinoColor::Role::Accent, onDarkFace(this)), 1.8, Qt::SolidLine,
                             Qt::RoundCap, Qt::RoundJoin));
        painter->drawLine(QPointF(markCentre.x() - 4.0, markCentre.y()),
                          QPointF(markCentre.x() - 1.0, markCentre.y() + 3.0));
        painter->drawLine(QPointF(markCentre.x() - 1.0, markCentre.y() + 3.0),
                          QPointF(markCentre.x() + 4.0, markCentre.y() - 3.5));
    }

    painter->restore();
}

void CupertinoListTile::resizeEvent(QResizeEvent *event)
{
    QxControls::QxControl::resizeEvent(event);
    layoutTrailing();
}

void CupertinoListTile::layoutTrailing()
{
    if (!d_ptr->trailingWidget) {
        return;
    }
    // Laid out here rather than in the paint pass: a widget that is moved while
    // the row is being drawn asks for another paint from inside this one.
    const QSize hint = d_ptr->trailingWidget->sizeHint();
    d_ptr->trailingWidget->setGeometry(width() - kInset - hint.width(), (height() - hint.height()) / 2, hint.width(),
                                       hint.height());
}

void CupertinoListTile::keyPressEvent(QKeyEvent *event)
{
    const int key = event->key();
    if (key == Qt::Key_Enter || key == Qt::Key_Return || key == Qt::Key_Space) {
        Q_EMIT clicked();
        event->accept();
        return;
    }
    QxControls::QxControl::keyPressEvent(event);
}

QAccessible::Role CupertinoListTile::accessibleRole() const
{
    return QAccessible::ListItem;
}

class CupertinoListSectionPrivate
{
public:
    QX_DECLARE_PUBLIC(CupertinoListSection)

    QString header;
    QLabel *headerLabel = Q_NULLPTR;
    QWidget *card = Q_NULLPTR;
    QVBoxLayout *cardLayout = Q_NULLPTR;
    QVBoxLayout *column = Q_NULLPTR;
};

CupertinoListSection::CupertinoListSection(QWidget *parent)
    : QWidget(parent)
    , d_ptr(new CupertinoListSectionPrivate)
{
    QX_SET_PRIVATE(d_ptr)

    // The card is inset from the page on both sides, which is what makes a
    // group read as a card rather than as a change of colour.
    d_ptr->column = new QVBoxLayout(this);
    d_ptr->column->setContentsMargins(kInset, 8, kInset, 8);
    d_ptr->column->setSpacing(0);

    d_ptr->headerLabel = new QLabel(this);
    // The caption starts where a row's title starts: the column already insets
    // the card by kInset, and a row insets its text by as much again.
    d_ptr->headerLabel->setContentsMargins(kInset, 0, 0, 6);
    d_ptr->headerLabel->hide();
    d_ptr->column->addWidget(d_ptr->headerLabel);

    d_ptr->card = new QWidget(this);
    d_ptr->cardLayout = new QVBoxLayout(d_ptr->card);
    d_ptr->cardLayout->setContentsMargins(0, 0, 0, 0);
    d_ptr->cardLayout->setSpacing(0);
    d_ptr->column->addWidget(d_ptr->card);
}

CupertinoListSection::~CupertinoListSection(){QX_FINI_PRIVATE()}

QString CupertinoListSection::header() const
{
    return d_ptr->header;
}

void CupertinoListSection::setHeader(const QString &header)
{
    d_ptr->header = header;
    // The caption above a group is the family's small print, in capitals and
    // in the ink that is not quite text.
    QFont font = CupertinoType::font(CupertinoType::Ramp::Caption, true);
    d_ptr->headerLabel->setFont(font);
    d_ptr->headerLabel->setText(header.toUpper());
    // The ink comes from the section's palette and not the label's, so that
    // asking twice for the same caption does not thin it twice.
    QPalette palette = d_ptr->headerLabel->palette();
    QColor ink = this->palette().color(QPalette::WindowText);
    ink.setAlphaF(0.55);
    palette.setColor(QPalette::WindowText, ink);
    d_ptr->headerLabel->setPalette(palette);
    d_ptr->headerLabel->setVisible(!header.isEmpty());
}

void CupertinoListSection::addTile(CupertinoListTile *tile)
{
    if (!tile) {
        return;
    }
    if (tile->parentWidget() != d_ptr->card) {
        tile->setParent(d_ptr->card);
    }
    d_ptr->cardLayout->addWidget(tile);
    assignPositions();
}

CupertinoListTile *CupertinoListSection::addTile(const QString &title, const QString &subtitle, const QIcon &icon)
{
    auto *tile = new CupertinoListTile(d_ptr->card);
    tile->setTitle(title);
    tile->setSubtitle(subtitle);
    tile->setIcon(icon);
    addTile(tile);
    return tile;
}

QSize CupertinoListSection::sizeHint() const
{
    return d_ptr->column->sizeHint();
}

void CupertinoListSection::assignPositions()
{
    const int count = d_ptr->cardLayout->count();
    for (int i = 0; i < count; ++i) {
        auto *tile = qobject_cast<CupertinoListTile *>(d_ptr->cardLayout->itemAt(i)->widget());
        if (!tile) {
            continue;
        }
        if (count == 1) {
            tile->setPosition(CupertinoListTile::Position::Only);
        } else if (i == 0) {
            tile->setPosition(CupertinoListTile::Position::First);
        } else if (i == count - 1) {
            tile->setPosition(CupertinoListTile::Position::Last);
        } else {
            tile->setPosition(CupertinoListTile::Position::Middle);
        }
    }
}

QX_CUPERTINO_END_NAMESPACE
