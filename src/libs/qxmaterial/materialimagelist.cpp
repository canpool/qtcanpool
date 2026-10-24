/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#include "materialimagelist.h"

#include "materialtokens.h"

#include "qxcontrols/qxcontrol.h"
#include "qxcontrols/qxstatelayer.h"

#include <QtCore/QEvent>
#include <QtCore/QVector>
#include <QtGui/QBrush>
#include <QtGui/QFontMetrics>
#include <QtGui/QKeyEvent>
#include <QtGui/QPainter>
#include <QtGui/QPainterPath>
#include <QtGui/QResizeEvent>
#include <QtWidgets/QSizePolicy>

QX_MATERIAL_BEGIN_NAMESPACE

namespace
{
// The grid's own defaults, as the site states them: two columns, a row of a
// hundred and eighty, four between two tiles.
constexpr int kDefaultColumns = 2;
constexpr int kDefaultRowHeight = 180;
constexpr int kDefaultGap = 4;

// A tile narrower than this is a stamp rather than a picture.
constexpr int kLeastTileWidth = 96;

// The bar: two heights, because a name with a line under it is not a name.
constexpr qreal kBarHeight = 48.0;
constexpr qreal kBarHeightWithSubtitle = 68.0;
constexpr qreal kBarPadding = 16.0;
constexpr qreal kTitleHeight = 22.0;
constexpr qreal kSubtitleHeight = 16.0;
constexpr qreal kTrailingGap = 8.0;

// The scrim is the one colour in this family that does not come from the
// palette: what is behind the words is a photograph, and the theme has never
// seen it. It runs from the bar's own edge inward and fades out, so the words
// stand on the darkest part of it.
constexpr qreal kScrimAtEdge = 0.65;
constexpr qreal kScrimAtMiddle = 0.25;
constexpr qreal kSubtitleOpacity = 0.7;
constexpr qreal kDisabledOpacity = 0.38;

// What a tile shows until its picture arrives: a surface one step from the
// page, which is how this family draws a place where something will be.
constexpr qreal kPlaceholderOpacity = 0.08;
}   // namespace

class Tile;

class MaterialImageListPrivate
{
public:
    QX_DECLARE_PUBLIC(MaterialImageList)

    QList<Tile *> tiles;
    /*! Which cell each tile's top left corner stands on, in reading order. */
    QVector<QPoint> places;
    int columns = kDefaultColumns;
    int rowHeight = kDefaultRowHeight;
    int gap = kDefaultGap;
    /*! How many rows the packing came to. */
    int rows = 0;

    void pack();
    void arrange();
    QRect rectOf(int index) const;
    /*! The picture standing on cell (\a column, \a row), or -1. */
    int itemAt(int column, int row) const;
    /*! Put the keyboard on the picture a step \a key away from \a from. */
    void step(int from, int key);
};

/*!
 * One picture: the photograph, and a bar across one edge of it carrying the
 * names. The photograph is the tile's surface, so the state wash the base draws
 * lands on the picture rather than under it - a wash under an opaque photograph
 * is a wash nobody sees.
 */
class Tile : public QxControls::QxControl
{
public:
    Tile(MaterialImageListPrivate *owner, int index)
        : QxControls::QxControl(owner->q_ptr)
        , m_owner(owner)
        , m_index(index)
    {
    }

    void setIndex(int index)
    {
        m_index = index;
    }
    int index() const
    {
        return m_index;
    }

    void setPixmap(const QPixmap &pixmap)
    {
        m_pixmap = pixmap;
        m_scaled = QPixmap();
        update();
    }
    QPixmap pixmap() const
    {
        return m_pixmap;
    }

    void setTitle(const QString &title)
    {
        const QString was = m_title;
        m_title = title;
        // The name an assistive tool hears follows the title until the
        // application names the tile itself.
        if (accessibleName().isEmpty() || accessibleName() == was) {
            setAccessibleName(title);
        }
        placeTrailing();
        update();
    }
    QString title() const
    {
        return m_title;
    }
    void setSubtitle(const QString &subtitle)
    {
        m_subtitle = subtitle;
        update();
    }
    QString subtitle() const
    {
        return m_subtitle;
    }
    void setBarPosition(MaterialImageList::BarPosition position)
    {
        m_barPosition = position;
        placeTrailing();
        update();
    }
    MaterialImageList::BarPosition barPosition() const
    {
        return m_barPosition;
    }

    void setColumnSpan(int span)
    {
        m_columnSpan = span;
    }
    int columnSpan() const
    {
        return m_columnSpan;
    }
    void setRowSpan(int span)
    {
        m_rowSpan = span;
    }
    int rowSpan() const
    {
        return m_rowSpan;
    }

    /*! Stand \a widget at the bar's far end; the widget becomes this tile's
     *  child, and is put back to its own parent when it is replaced. */
    void setTrailing(QWidget *widget)
    {
        if (m_trailing == widget) {
            return;
        }
        if (m_trailing) {
            m_trailing->hide();
            m_trailing->setParent(Q_NULLPTR);
        }
        m_trailing = widget;
        if (m_trailing) {
            m_trailing->setParent(this);
            m_trailing->show();
        }
        placeTrailing();
        update();
    }
    QWidget *trailing() const
    {
        return m_trailing;
    }

    /*! True when the tile has anything to put in a bar. */
    bool hasBar() const
    {
        return !m_title.isEmpty() || !m_subtitle.isEmpty() || m_trailing != Q_NULLPTR;
    }
    /*! The bar's own rectangle, in this tile's coordinates. */
    QRectF barRect() const
    {
        const qreal tall = barHeight();
        if (tall <= 0.0) {
            return QRectF();
        }
        return m_barPosition == MaterialImageList::BarPosition::Top
                   ? QRectF(0.0, 0.0, qreal(width()), tall)
                   : QRectF(0.0, qreal(height()) - tall, qreal(width()), tall);
    }
    qreal barHeight() const
    {
        if (!hasBar()) {
            return 0.0;
        }
        return m_subtitle.isEmpty() ? kBarHeight : kBarHeightWithSubtitle;
    }

    qreal cornerRadius() const override
    {
        // A tile is small: the card's sixteen would eat the photograph's corners
        // and the grid's own gaps already say where one picture ends.
        return MaterialShape::radius(MaterialShape::Radius::Small);
    }
    QRectF chromeRect() const override
    {
        return QRectF(rect());
    }

    /*! The photograph, filling the tile: it is scaled until it covers the tile
     *  and the middle of it is what is shown, the way a picture is put in a
     *  frame - the whole of it, letterboxed, would be a picture nobody chose. */
    void paintSurface(QPainter *painter) override
    {
        painter->save();
        if (!isEnabled()) {
            painter->setOpacity(kDisabledOpacity);
        }
        QPainterPath clip;
        const qreal radius = cornerRadius();
        clip.addRoundedRect(QRectF(rect()), radius, radius);
        painter->setClipPath(clip);

        if (m_pixmap.isNull()) {
            const QColor placeholder =
                QxControls::QxStateLayer::layered(palette().color(QPalette::Window), textColor(), kPlaceholderOpacity);
            painter->fillRect(rect(), placeholder);
        } else {
            if (m_scaled.size() != size()) {
                m_scaled = m_pixmap.scaled(size(), Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation);
            }
            const QRect from((m_scaled.width() - width()) / 2, (m_scaled.height() - height()) / 2, width(), height());
            painter->drawPixmap(rect(), m_scaled, from);
        }
        painter->restore();
    }

    /*! The bar: the scrim over the photograph, then the names on it, then the
     *  application's widget at its far end - which the tile places but does not
     *  draw, since it is a widget and answers for itself. */
    void paintContent(QPainter *painter) override
    {
        if (!hasBar()) {
            return;
        }
        painter->save();
        if (!isEnabled()) {
            painter->setOpacity(kDisabledOpacity);
        }
        QPainterPath clip;
        const qreal radius = cornerRadius();
        clip.addRoundedRect(QRectF(rect()), radius, radius);
        painter->setClipPath(clip);
        paintScrim(painter);

        const QRectF bar = barRect();
        const qreal inner = bar.right() - kBarPadding;
        qreal textRight = inner;
        if (m_trailing) {
            textRight = qreal(m_trailing->x()) - kTrailingGap;
        }
        const QRectF titleRoom(bar.left() + kBarPadding, bar.top(), qMax(0.0, textRight - bar.left() - kBarPadding),
                               bar.height());
        painter->setPen(white());
        if (m_subtitle.isEmpty()) {
            painter->setFont(MaterialType::font(MaterialType::Scale::Body));
            painter->drawText(titleRoom, Qt::AlignLeft | Qt::AlignVCenter, elided(painter, m_title, titleRoom.width()));
        } else {
            const qreal block = kTitleHeight + kSubtitleHeight;
            const qreal top = bar.top() + (bar.height() - block) / 2.0;
            painter->setFont(MaterialType::font(MaterialType::Scale::Body));
            painter->drawText(QRectF(titleRoom.left(), top, titleRoom.width(), kTitleHeight),
                              Qt::AlignLeft | Qt::AlignVCenter, elided(painter, m_title, titleRoom.width()));
            QColor second = white();
            second.setAlphaF(kSubtitleOpacity);
            painter->setPen(second);
            painter->setFont(MaterialType::font(MaterialType::Scale::Label));
            painter->drawText(QRectF(titleRoom.left(), top + kTitleHeight, titleRoom.width(), kSubtitleHeight),
                              Qt::AlignLeft | Qt::AlignVCenter, elided(painter, m_subtitle, titleRoom.width()));
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
        if (event->key() == Qt::Key_Left || event->key() == Qt::Key_Right || event->key() == Qt::Key_Up ||
            event->key() == Qt::Key_Down) {
            m_owner->step(m_index, event->key());
            event->accept();
            return;
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

    void resizeEvent(QResizeEvent *event) override
    {
        QxControls::QxControl::resizeEvent(event);
        placeTrailing();
    }

    QAccessible::Role accessibleRole() const override
    {
        return QAccessible::Button;
    }
private:
    /*! White, always: the words stand on a photograph and on the scrim the tile
     *  puts under them, not on a surface the palette knows. */
    static QColor white()
    {
        return QColor(Qt::white);
    }

    static QString elided(QPainter *painter, const QString &text, qreal room)
    {
        return QFontMetrics(painter->font()).elidedText(text, Qt::ElideRight, int(qMax(0.0, room)));
    }

    void paintScrim(QPainter *painter) const
    {
        const QRectF bar = barRect();
        const bool atTop = m_barPosition == MaterialImageList::BarPosition::Top;
        // The scrim is black whatever the theme is: it is not drawing a
        // surface, it is taking light away from a photograph, and the words on
        // it are white for the same reason. It runs from the bar's own edge
        // inward and fades out, so the words stand on the darkest part of it.
        QColor dark = QColor(Qt::black);
        dark.setAlphaF(kScrimAtEdge);
        QColor faded = QColor(Qt::black);
        faded.setAlphaF(kScrimAtMiddle);

        QLinearGradient gradient(bar.topLeft(), bar.bottomLeft());
        gradient.setColorAt(0.0, atTop ? dark : faded);
        gradient.setColorAt(1.0, atTop ? faded : dark);
        painter->setPen(Qt::NoPen);
        painter->setBrush(gradient);
        painter->drawRect(bar);
    }

    void placeTrailing()
    {
        if (!m_trailing) {
            return;
        }
        const QRectF bar = barRect();
        if (bar.isEmpty()) {
            m_trailing->hide();
            return;
        }
        QSize hint = m_trailing->sizeHint();
        if (!hint.isValid()) {
            // A widget that does not say how big it wants to be takes the size
            // it has - which is what a fixed size is, and what a widget made of
            // one thing usually is.
            hint = m_trailing->size();
        }
        const int width = qBound(0, hint.width(), qMax(0, int(bar.width() - 2 * kBarPadding)));
        const int height = qBound(0, hint.height(), int(bar.height()));
        // A rectangle's right edge is where it ends, not the last pixel in it,
        // so the far end of the bar is bar.right() and the widget stands one
        // padding in from it.
        m_trailing->setGeometry(int(bar.right() - kBarPadding) - width, int(bar.center().y() - height / 2.0), width,
                                height);
        m_trailing->show();
    }

    MaterialImageListPrivate *m_owner;
    int m_index;
    QPixmap m_pixmap;
    QPixmap m_scaled;
    QString m_title;
    QString m_subtitle;
    MaterialImageList::BarPosition m_barPosition = MaterialImageList::BarPosition::Bottom;
    QWidget *m_trailing = Q_NULLPTR;
    int m_columnSpan = 1;
    int m_rowSpan = 1;
};

void MaterialImageListPrivate::pack()
{
    places.clear();
    rows = 0;

    // A cell is taken or it is not: the packing walks the cells in reading
    // order and puts each picture in the first place its span fits, which is
    // what keeps a two-row picture from leaving a hole beside it.
    const auto fits = [this](const QVector<QVector<bool>> &used, int column, int row, int across, int down) {
        if (column + across > columns) {
            return false;
        }
        for (int r = row; r < row + down; ++r) {
            if (r >= used.count()) {
                continue;
            }
            for (int c = column; c < column + across; ++c) {
                if (used.at(r).at(c)) {
                    return false;
                }
            }
        }
        return true;
    };
    const auto take = [this](QVector<QVector<bool>> &used, int column, int row, int across, int down) {
        while (used.count() < row + down) {
            used.append(QVector<bool>(columns, false));
        }
        for (int r = row; r < row + down; ++r) {
            for (int c = column; c < column + across; ++c) {
                used[r][c] = true;
            }
        }
    };

    QVector<QVector<bool>> used;
    for (int i = 0; i < tiles.count(); ++i) {
        const int across = qBound(1, tiles.at(i)->columnSpan(), columns);
        const int down = qMax(1, tiles.at(i)->rowSpan());
        for (int row = 0;; ++row) {
            bool landed = false;
            for (int column = 0; column + across <= columns; ++column) {
                if (fits(used, column, row, across, down)) {
                    take(used, column, row, across, down);
                    places.append(QPoint(column, row));
                    rows = qMax(rows, row + down);
                    landed = true;
                    break;
                }
            }
            if (landed) {
                break;
            }
        }
    }
}

QRect MaterialImageListPrivate::rectOf(int index) const
{
    if (index < 0 || index >= places.count() || columns < 1) {
        return QRect();
    }
    const int room = q_ptr->width();
    const int unit = (room - (columns - 1) * gap) / columns;
    if (unit <= 0) {
        return QRect();
    }
    const Tile *tile = tiles.at(index);
    const QPoint cell = places.at(index);
    const int across = qBound(1, tile->columnSpan(), columns);
    const int down = qMax(1, tile->rowSpan());

    const int left = cell.x() * (unit + gap);
    // Whatever the division left over belongs to the column at the edge, so the
    // grid ends where the widget does instead of a few pixels short of it.
    const int right = (cell.x() + across == columns) ? room - 1 : left + across * unit + (across - 1) * gap - 1;
    const int top = cell.y() * (rowHeight + gap);
    const int height = down * rowHeight + (down - 1) * gap;
    return QRect(left, top, right - left + 1, height);
}

int MaterialImageListPrivate::itemAt(int column, int row) const
{
    for (int i = 0; i < places.count(); ++i) {
        const Tile *tile = tiles.at(i);
        const QPoint cell = places.at(i);
        const int across = qBound(1, tile->columnSpan(), columns);
        const int down = qMax(1, tile->rowSpan());
        if (row >= cell.y() && row < cell.y() + down && column >= cell.x() && column < cell.x() + across) {
            return i;
        }
    }
    return -1;
}

void MaterialImageListPrivate::step(int from, int key)
{
    if (from < 0 || from >= places.count()) {
        return;
    }
    int to = -1;
    if (key == Qt::Key_Left) {
        to = from - 1;
    } else if (key == Qt::Key_Right) {
        to = from + 1;
    } else {
        // Up and down walk the cells rather than the order: the picture above
        // is the one standing on the cell above, whatever its number is.
        const QPoint cell = places.at(from);
        const int across = qBound(1, tiles.at(from)->columnSpan(), columns);
        const int below = qMax(1, tiles.at(from)->rowSpan());
        const int row = (key == Qt::Key_Down) ? cell.y() + below : cell.y() - 1;
        if (row < 0 || row >= rows) {
            return;
        }
        for (int column = cell.x(); column < cell.x() + across && to < 0; ++column) {
            to = itemAt(column, row);
        }
    }
    if (to >= 0 && to < tiles.count() && to != from) {
        tiles.at(to)->setFocus();
    }
}

void MaterialImageListPrivate::arrange()
{
    for (int i = 0; i < tiles.count(); ++i) {
        tiles.at(i)->setGeometry(rectOf(i));
    }
}

MaterialImageList::MaterialImageList(QWidget *parent)
    : QWidget(parent)
    , d_ptr(new MaterialImageListPrivate)
{
    QX_SET_PRIVATE(d_ptr);
    // The height is the rows the spans came to, which no amount of width
    // changes, so the grid takes the width it is given and keeps its own height.
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
}

MaterialImageList::~MaterialImageList()
{
    QX_FINI_PRIVATE();
}

int MaterialImageList::count() const
{
    return d_ptr->tiles.count();
}

QPixmap MaterialImageList::itemPixmap(int index) const
{
    const Tile *tile = d_ptr->tiles.value(index, Q_NULLPTR);
    return tile ? tile->pixmap() : QPixmap();
}

QString MaterialImageList::itemTitle(int index) const
{
    const Tile *tile = d_ptr->tiles.value(index, Q_NULLPTR);
    return tile ? tile->title() : QString();
}

QString MaterialImageList::itemSubtitle(int index) const
{
    const Tile *tile = d_ptr->tiles.value(index, Q_NULLPTR);
    return tile ? tile->subtitle() : QString();
}

int MaterialImageList::itemColumnSpan(int index) const
{
    const Tile *tile = d_ptr->tiles.value(index, Q_NULLPTR);
    return tile ? tile->columnSpan() : 0;
}

int MaterialImageList::itemRowSpan(int index) const
{
    const Tile *tile = d_ptr->tiles.value(index, Q_NULLPTR);
    return tile ? tile->rowSpan() : 0;
}

MaterialImageList::BarPosition MaterialImageList::itemBarPosition(int index) const
{
    const Tile *tile = d_ptr->tiles.value(index, Q_NULLPTR);
    return tile ? tile->barPosition() : BarPosition::Bottom;
}

QWidget *MaterialImageList::itemTrailingWidget(int index) const
{
    const Tile *tile = d_ptr->tiles.value(index, Q_NULLPTR);
    return tile ? tile->trailing() : Q_NULLPTR;
}

QRect MaterialImageList::itemRect(int index) const
{
    return d_ptr->rectOf(index);
}

int MaterialImageList::columns() const
{
    return d_ptr->columns;
}

int MaterialImageList::rowHeight() const
{
    return d_ptr->rowHeight;
}

int MaterialImageList::gap() const
{
    return d_ptr->gap;
}

QSize MaterialImageList::sizeHint() const
{
    // Square tiles at the columns asked for: the hint says what the grid is,
    // and the width it is given decides how wide a cell really is.
    const int height = d_ptr->rows > 0 ? d_ptr->rows * d_ptr->rowHeight + (d_ptr->rows - 1) * d_ptr->gap : 0;
    return QSize(d_ptr->columns * d_ptr->rowHeight, height);
}

QSize MaterialImageList::minimumSizeHint() const
{
    const int height = d_ptr->rows > 0 ? d_ptr->rows * d_ptr->rowHeight + (d_ptr->rows - 1) * d_ptr->gap : 0;
    return QSize(d_ptr->columns * kLeastTileWidth, height);
}

int MaterialImageList::addItem(const QPixmap &pixmap, const QString &title, const QString &subtitle)
{
    auto *tile = new Tile(d_ptr, d_ptr->tiles.count());
    tile->setPixmap(pixmap);
    tile->setTitle(title);
    tile->setSubtitle(subtitle);
    QObject::connect(tile, &QxControls::QxControl::clicked, this, [this, tile]() {
        Q_EMIT itemClicked(tile->index());
    });
    d_ptr->tiles.append(tile);
    tile->show();
    d_ptr->pack();
    d_ptr->arrange();
    updateGeometry();
    return tile->index();
}

void MaterialImageList::setItemPixmap(int index, const QPixmap &pixmap)
{
    if (Tile *tile = d_ptr->tiles.value(index, Q_NULLPTR)) {
        tile->setPixmap(pixmap);
    }
}

void MaterialImageList::setItemTitle(int index, const QString &title)
{
    if (Tile *tile = d_ptr->tiles.value(index, Q_NULLPTR)) {
        tile->setTitle(title);
    }
}

void MaterialImageList::setItemSubtitle(int index, const QString &subtitle)
{
    if (Tile *tile = d_ptr->tiles.value(index, Q_NULLPTR)) {
        tile->setSubtitle(subtitle);
    }
}

void MaterialImageList::setItemSpan(int index, int columns, int rows)
{
    Tile *tile = d_ptr->tiles.value(index, Q_NULLPTR);
    if (!tile) {
        return;
    }
    // A picture that takes no cell is not a tile, so the least it can take is
    // one. What is kept is what was asked for: a span wider than the grid is
    // read as the whole grid while the grid is that narrow, and widening the
    // grid again gives the picture its cells back.
    const int across = qMax(1, columns);
    const int down = qMax(1, rows);
    if (tile->columnSpan() == across && tile->rowSpan() == down) {
        return;
    }
    tile->setColumnSpan(across);
    tile->setRowSpan(down);
    d_ptr->pack();
    d_ptr->arrange();
    updateGeometry();
}

void MaterialImageList::setItemBarPosition(int index, BarPosition position)
{
    if (Tile *tile = d_ptr->tiles.value(index, Q_NULLPTR)) {
        tile->setBarPosition(position);
    }
}

void MaterialImageList::setItemTrailingWidget(int index, QWidget *widget)
{
    if (Tile *tile = d_ptr->tiles.value(index, Q_NULLPTR)) {
        tile->setTrailing(widget);
    }
}

void MaterialImageList::removeItem(int index)
{
    if (index < 0 || index >= d_ptr->tiles.count()) {
        return;
    }
    Tile *tile = d_ptr->tiles.takeAt(index);
    tile->hide();
    tile->deleteLater();
    for (int i = 0; i < d_ptr->tiles.count(); ++i) {
        d_ptr->tiles.at(i)->setIndex(i);
    }
    d_ptr->pack();
    d_ptr->arrange();
    updateGeometry();
}

void MaterialImageList::clear()
{
    const QList<Tile *> gone = d_ptr->tiles;
    d_ptr->tiles.clear();
    for (Tile *tile : gone) {
        tile->hide();
        tile->deleteLater();
    }
    d_ptr->pack();
    updateGeometry();
}

void MaterialImageList::setColumns(int columns)
{
    const int wanted = qMax(1, columns);
    if (d_ptr->columns == wanted) {
        return;
    }
    d_ptr->columns = wanted;
    d_ptr->pack();
    d_ptr->arrange();
    updateGeometry();
}

void MaterialImageList::setRowHeight(int rowHeight)
{
    const int wanted = qMax(1, rowHeight);
    if (d_ptr->rowHeight == wanted) {
        return;
    }
    d_ptr->rowHeight = wanted;
    d_ptr->arrange();
    updateGeometry();
}

void MaterialImageList::setGap(int gap)
{
    const int wanted = qMax(0, gap);
    if (d_ptr->gap == wanted) {
        return;
    }
    d_ptr->gap = wanted;
    d_ptr->arrange();
    updateGeometry();
}

void MaterialImageList::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);
    d_ptr->arrange();
}

void MaterialImageList::changeEvent(QEvent *event)
{
    QWidget::changeEvent(event);
    if (event->type() == QEvent::FontChange || event->type() == QEvent::PaletteChange) {
        update();
    }
}

QX_MATERIAL_END_NAMESPACE
