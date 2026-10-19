/**
 * Copyleft (C) 2023 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MIT
 **/
#include "ribbongallerygroup.h"
#include "ribbongallerygroup_p.h"

#include <QAction>
#include <QActionGroup>
#include <QApplication>
#include <QEvent>
#include <QFontMetrics>
#include <QMouseEvent>
#include <QPainter>
#include <QResizeEvent>
#include <QWheelEvent>

QX_RIBBON_BEGIN_NAMESPACE

/* RibbonGalleryGroupPrivate */
class RibbonGalleryGroupPrivate
{
    QX_DECLARE_PUBLIC(RibbonGalleryGroup)
public:
    RibbonGalleryGroupPrivate();
    void init();
    int displayRowCount() const;
    int rowCount() const;
    int columnCount() const;
public:
    QString m_groupTitle;
    RibbonGalleryGroup::GalleryGroupStyle m_preStyle;
    RibbonGalleryGroup::DisplayRow m_displayRow;
    int m_gridMinimumWidth;   ///< minimum grid width
    int m_gridMaximumWidth;   ///< maximum grid width
    int m_spacing;
    QSize m_grid;
    QList<QAction *> m_actions;    ///< the items, in the order they are shown
    QActionGroup *m_actionGroup;   ///< all actions added to the RibbonGalleryGroup are managed by this action group
    bool m_blockRecalc;
    int m_selectedIndex;
    int m_hoverIndex;
    int m_pressedIndex;
    int m_viewportRows;   ///< 0: every row is shown and the group never scrolls
    int m_scrollRow;
};

RibbonGalleryGroupPrivate::RibbonGalleryGroupPrivate()
    : m_preStyle(RibbonGalleryGroup::IconWithText)
    , m_displayRow(RibbonGalleryGroup::DisplayOneRow)
    , m_gridMinimumWidth(0)
    , m_gridMaximumWidth(0)
    , m_spacing(1)
    , m_grid(64, 64)
    , m_actionGroup(Q_NULLPTR)
    , m_blockRecalc(false)
    , m_selectedIndex(-1)
    , m_hoverIndex(-1)
    , m_pressedIndex(-1)
    , m_viewportRows(0)
    , m_scrollRow(0)
{
}

void RibbonGalleryGroupPrivate::init()
{
    Q_Q(RibbonGalleryGroup);
    m_actionGroup = new QActionGroup(q);
    QObject::connect(m_actionGroup, &QActionGroup::triggered, q, &RibbonGalleryGroup::triggered);
    QObject::connect(m_actionGroup, &QActionGroup::hovered, q, &RibbonGalleryGroup::hovered);
}

int RibbonGalleryGroupPrivate::displayRowCount() const
{
    switch (m_displayRow) {
    case RibbonGalleryGroup::DisplayOneRow:
        return 1;
    case RibbonGalleryGroup::DisplayTwoRow:
        return 2;
    case RibbonGalleryGroup::DisplayThreeRow:
        return 3;
    }
    return 1;
}

int RibbonGalleryGroupPrivate::columnCount() const
{
    Q_Q(const RibbonGalleryGroup);
    const int step = m_grid.width() + m_spacing;
    const int inner = q->width() - 2 * m_spacing;
    if (step <= 0 || inner <= 0) {
        return 1;
    }
    return qMax(1, (inner + m_spacing) / step);
}

int RibbonGalleryGroupPrivate::rowCount() const
{
    const int columns = qMax(1, columnCount());
    return (m_actions.count() + columns - 1) / columns;
}

/* RibbonGalleryPaintSpec - geometry */

int galleryColumnCount(const QRect &area, const RibbonGalleryPaintSpec &spec)
{
    const int step = spec.grid.width() + spec.spacing;
    if (step <= 0) {
        return 1;
    }
    return qMax(1, (area.width() + spec.spacing) / step);
}

int galleryRowCount(const RibbonGalleryPaintSpec &spec)
{
    const int columns = qMax(1, spec.columns);
    return (spec.actions.count() + columns - 1) / columns;
}

QRect galleryItemRect(const QRect &area, const RibbonGalleryPaintSpec &spec, int index)
{
    const int columns = qMax(1, spec.columns);
    const int stepX = spec.grid.width() + spec.spacing;
    const int stepY = spec.grid.height() + spec.spacing;
    return QRect(area.left() + (index % columns) * stepX, area.top() + (index / columns - spec.firstRow) * stepY,
                 spec.grid.width(), spec.grid.height());
}

int galleryItemAt(const QRect &area, const RibbonGalleryPaintSpec &spec, const QPoint &pos)
{
    const int columns = qMax(1, spec.columns);
    const int stepX = spec.grid.width() + spec.spacing;
    const int stepY = spec.grid.height() + spec.spacing;
    if (stepX <= 0 || stepY <= 0) {
        return -1;
    }
    const int col = (pos.x() - area.left()) / stepX;
    const int relRow = (pos.y() - area.top()) / stepY;
    // The gaps between the items are not part of any item.
    if ((pos.x() - area.left()) % stepX >= spec.grid.width()) {
        return -1;
    }
    if ((pos.y() - area.top()) % stepY >= spec.grid.height()) {
        return -1;
    }
    if (col < 0 || col >= columns || relRow < 0) {
        return -1;
    }
    const int index = (relRow + spec.firstRow) * columns + col;
    if (index < 0 || index >= spec.actions.count()) {
        return -1;
    }
    if (spec.visibleRows > 0 && relRow >= spec.visibleRows) {
        return -1;
    }
    return index;
}

/* RibbonGalleryPaintSpec - painting */

/*
 * One item.
 *
 * The rectangle is the grid's and the grid does not know what state the item is
 * in: a wash and a border are drawn inside it, and the content is inset by the
 * same margin whatever is drawn. That is the whole reason pointing at an item
 * cannot shift it - there is nothing in here whose width depends on the state.
 */
static void paintGalleryItem(QPainter *painter, const QRect &box, const RibbonGalleryPaintSpec &spec, int index)
{
    QAction *act = spec.actions.value(index);
    if (Q_NULLPTR == act) {
        return;
    }
    const bool enabled = act->isEnabled();
    const bool selected = (index == spec.selectedIndex);
    const bool pressed = (index == spec.pressedIndex);
    const bool hovered = (index == spec.hoverIndex);

    const QPalette &pl = spec.palette;
    const QColor accent = pl.color(QPalette::Highlight);

    int washAlpha = 0;
    QColor border;
    if (pressed) {
        washAlpha = 70;
        border = accent;
    } else if (selected) {
        washAlpha = 46;
        border = accent;
    } else if (hovered) {
        washAlpha = 24;
        border = QColor(accent.red(), accent.green(), accent.blue(), 110);
    }
    if (washAlpha > 0) {
        // A fraction of the accent over whatever the item sits on, rather than a
        // colour of its own: the surface shows through and every row still reads
        // as part of the same strip.
        QColor wash = accent;
        wash.setAlpha(washAlpha);
        painter->fillRect(box, wash);
        painter->setPen(QPen(border, 1));
        painter->setBrush(Qt::NoBrush);
        painter->drawRect(box.adjusted(0, 0, -1, -1));
    }

    const int margin = 3;
    const QRect content = box.adjusted(margin, margin, -margin, -margin);
    if (!content.isValid()) {
        return;
    }

    QFontMetrics fm(spec.font);
    int textHeight = 0;
    switch (spec.style) {
    case RibbonGalleryGroup::IconWithText:
        textHeight = fm.lineSpacing();
        break;
    case RibbonGalleryGroup::IconWithWordWrapText:
        textHeight = fm.lineSpacing() * 2;
        break;
    case RibbonGalleryGroup::IconOnly:
    default:
        textHeight = 0;
        break;
    }

    QRect iconRect = content;
    if (textHeight > 0) {
        iconRect.setHeight(qMax(0, content.height() - textHeight - 2));
    }
    if (iconRect.isValid()) {
        // Square, and centred in whatever the style left for it.
        const int side = qMin(iconRect.width(), iconRect.height());
        const QRect glyph(iconRect.center().x() - side / 2, iconRect.center().y() - side / 2, side, side);
        act->icon().paint(painter, glyph, Qt::AlignCenter, enabled ? QIcon::Normal : QIcon::Disabled, QIcon::Off);
    }

    if (textHeight <= 0 || content.height() <= textHeight) {
        return;
    }
    const QRect textRect(content.left(), content.bottom() - textHeight + 1, content.width(), textHeight);
    painter->setFont(spec.font);
    // The label keeps the colour text has everywhere else. Reversing it out on a
    // wash this light is what made a selected item look like a hole in the strip.
    painter->setPen(pl.color(enabled ? QPalette::Normal : QPalette::Disabled, QPalette::Text));
    if (spec.style == RibbonGalleryGroup::IconWithWordWrapText) {
        painter->save();
        painter->setClipRect(textRect);
        painter->drawText(textRect, Qt::AlignHCenter | Qt::AlignVCenter | Qt::TextWordWrap, act->text());
        painter->restore();
    } else {
        const QString text = fm.elidedText(act->text(), Qt::ElideRight, textRect.width());
        painter->drawText(textRect, Qt::AlignHCenter | Qt::AlignVCenter, text);
    }
}

void galleryPaintGrid(QPainter *painter, const QRect &area, const RibbonGalleryPaintSpec &spec)
{
    const int rows = galleryRowCount(spec);
    const int lastRow = (spec.visibleRows > 0) ? qMin(rows, spec.firstRow + spec.visibleRows) : rows;
    const int first = spec.firstRow * qMax(1, spec.columns);
    for (int i = first; i < spec.actions.count(); ++i) {
        if (i / qMax(1, spec.columns) >= lastRow) {
            break;
        }
        paintGalleryItem(painter, galleryItemRect(area, spec, i), spec, i);
    }
}

/* RibbonGalleryGroup */

RibbonGalleryGroup::RibbonGalleryGroup(QWidget *w)
    : QWidget(w)
{
    QX_INIT_PRIVATE(RibbonGalleryGroup)
    Q_D(RibbonGalleryGroup);
    d->init();
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    // The items are painted, not widgets, so the group tracks the pointer itself.
    setMouseTracking(true);
    setFocusPolicy(Qt::NoFocus);
}

RibbonGalleryGroup::~RibbonGalleryGroup()
{
    QX_FINI_PRIVATE()
}

/**
 * @brief Whether recalculation is disabled
 * @param on
 */
void RibbonGalleryGroup::setRecalcGridSizeBlock(bool on)
{
    Q_D(RibbonGalleryGroup);
    d->m_blockRecalc = on;
}

bool RibbonGalleryGroup::isRecalcGridSizeBlock() const
{
    Q_D(const RibbonGalleryGroup);
    return d->m_blockRecalc;
}

/**
 * @brief Recalculates the size of the grid
 */
void RibbonGalleryGroup::recalcGridSize()
{
    recalcGridSize(height());
}

void RibbonGalleryGroup::recalcGridSize(int galleryHeight)
{
    if (isRecalcGridSizeBlock()) {
        return;
    }
    Q_D(RibbonGalleryGroup);
    const int rowCount = d->displayRowCount();
    // The rows have to fit between the spacings, not merely inside the height:
    // there is a gap above the first row, below the last one and between the rows.
    // A grid that is simply height/rows tall runs the last row past the bottom
    // edge, and an item whose bottom row is off the widget loses its bottom line
    // - which reads as the gallery cutting the item off.
    int h = (galleryHeight - (rowCount + 1) * d->m_spacing) / rowCount;
    if (h <= 1) {
        h = galleryHeight;
    }
    int w = h;
    if ((d->m_gridMinimumWidth > 0) && (w < d->m_gridMinimumWidth)) {
        w = d->m_gridMinimumWidth;
    }
    if ((d->m_gridMaximumWidth > 0) && (w > d->m_gridMaximumWidth)) {
        w = d->m_gridMaximumWidth;
    }
    if (d->m_grid == QSize(w, h)) {
        return;
    }
    d->m_grid = QSize(w, h);
    updateGeometry();
    update();
}

///
/// \brief Sets the default preset style
/// \param style
///
void RibbonGalleryGroup::setGalleryGroupStyle(RibbonGalleryGroup::GalleryGroupStyle style)
{
    Q_D(RibbonGalleryGroup);
    if (d->m_preStyle == style) {
        return;
    }
    d->m_preStyle = style;
    update();
}

RibbonGalleryGroup::GalleryGroupStyle RibbonGalleryGroup::galleryGroupStyle() const
{
    Q_D(const RibbonGalleryGroup);
    return d->m_preStyle;
}

void RibbonGalleryGroup::addItem(const QString &text, const QIcon &icon)
{
    addItem(new QAction(icon, text, this));
}

void RibbonGalleryGroup::addItem(QAction *act)
{
    if (Q_NULLPTR == act) {
        return;
    }
    Q_D(RibbonGalleryGroup);
    d->m_actions.append(act);
    d->m_actionGroup->addAction(act);
    updateGeometry();
    update();
}

void RibbonGalleryGroup::addItems(const QList<QAction *> &acts)
{
    for (QAction *act : acts) {
        addItem(act);
    }
}

int RibbonGalleryGroup::itemCount() const
{
    Q_D(const RibbonGalleryGroup);
    return d->m_actions.count();
}

QAction *RibbonGalleryGroup::itemAction(int index) const
{
    Q_D(const RibbonGalleryGroup);
    return d->m_actions.value(index, Q_NULLPTR);
}

void RibbonGalleryGroup::setGroupTitle(const QString &title)
{
    Q_D(RibbonGalleryGroup);
    if (d->m_groupTitle == title) {
        return;
    }
    d->m_groupTitle = title;
    Q_EMIT groupTitleChanged(title);
}

QString RibbonGalleryGroup::groupTitle() const
{
    Q_D(const RibbonGalleryGroup);
    return d->m_groupTitle;
}

void RibbonGalleryGroup::selectByIndex(int i)
{
    Q_D(RibbonGalleryGroup);
    int next = -1;
    if (i >= 0 && i < d->m_actions.count()) {
        next = i;
    }
    if (d->m_selectedIndex == next) {
        return;
    }
    d->m_selectedIndex = next;
    scrollToItem(next);
    update();
}

int RibbonGalleryGroup::selectedIndex() const
{
    Q_D(const RibbonGalleryGroup);
    return d->m_selectedIndex;
}

QAction *RibbonGalleryGroup::selectedAction() const
{
    Q_D(const RibbonGalleryGroup);
    return d->m_actions.value(d->m_selectedIndex, Q_NULLPTR);
}

void RibbonGalleryGroup::triggerCurrentItem()
{
    Q_D(RibbonGalleryGroup);
    QAction *act = d->m_actions.value(d->m_selectedIndex, Q_NULLPTR);
    if (act && act->isEnabled()) {
        act->trigger();
    }
}

void RibbonGalleryGroup::setDisplayRow(RibbonGalleryGroup::DisplayRow r)
{
    Q_D(RibbonGalleryGroup);
    if (d->m_displayRow == r) {
        return;
    }
    d->m_displayRow = r;
    updateGeometry();
    update();
}

RibbonGalleryGroup::DisplayRow RibbonGalleryGroup::displayRow() const
{
    Q_D(const RibbonGalleryGroup);
    return d->m_displayRow;
}

void RibbonGalleryGroup::setGridMinimumWidth(int w)
{
    Q_D(RibbonGalleryGroup);
    d->m_gridMinimumWidth = w;
}

int RibbonGalleryGroup::gridMinimumWidth() const
{
    Q_D(const RibbonGalleryGroup);
    return d->m_gridMinimumWidth;
}

void RibbonGalleryGroup::setGridMaximumWidth(int w)
{
    Q_D(RibbonGalleryGroup);
    d->m_gridMaximumWidth = w;
}

int RibbonGalleryGroup::gridMaximumWidth() const
{
    Q_D(const RibbonGalleryGroup);
    return d->m_gridMaximumWidth;
}

void RibbonGalleryGroup::setSpacing(int spacing)
{
    Q_D(RibbonGalleryGroup);
    if (d->m_spacing == spacing) {
        return;
    }
    d->m_spacing = qMax(0, spacing);
    updateGeometry();
    update();
}

int RibbonGalleryGroup::spacing() const
{
    Q_D(const RibbonGalleryGroup);
    return d->m_spacing;
}

QSize RibbonGalleryGroup::gridSize() const
{
    Q_D(const RibbonGalleryGroup);
    return d->m_grid;
}

int RibbonGalleryGroup::rowCount() const
{
    Q_D(const RibbonGalleryGroup);
    return d->rowCount();
}

int RibbonGalleryGroup::columnCount() const
{
    Q_D(const RibbonGalleryGroup);
    return d->columnCount();
}

QRect RibbonGalleryGroup::itemRect(int index) const
{
    Q_D(const RibbonGalleryGroup);
    RibbonGalleryPaintSpec spec;
    spec.actions = d->m_actions;
    spec.grid = d->m_grid;
    spec.spacing = d->m_spacing;
    spec.columns = d->columnCount();
    spec.firstRow = d->m_scrollRow;
    spec.visibleRows = d->m_viewportRows;
    return galleryItemRect(rect().adjusted(d->m_spacing, d->m_spacing, -d->m_spacing, -d->m_spacing), spec, index);
}

int RibbonGalleryGroup::itemAt(const QPoint &pos) const
{
    Q_D(const RibbonGalleryGroup);
    RibbonGalleryPaintSpec spec;
    spec.actions = d->m_actions;
    spec.grid = d->m_grid;
    spec.spacing = d->m_spacing;
    spec.columns = d->columnCount();
    spec.firstRow = d->m_scrollRow;
    spec.visibleRows = d->m_viewportRows;
    return galleryItemAt(rect().adjusted(d->m_spacing, d->m_spacing, -d->m_spacing, -d->m_spacing), spec, pos);
}

void RibbonGalleryGroup::setViewportRows(int rows)
{
    Q_D(RibbonGalleryGroup);
    const int next = qMax(0, rows);
    if (d->m_viewportRows == next) {
        return;
    }
    d->m_viewportRows = next;
    setScrollRow(d->m_scrollRow);
    updateGeometry();
    update();
}

int RibbonGalleryGroup::viewportRows() const
{
    Q_D(const RibbonGalleryGroup);
    return d->m_viewportRows;
}

int RibbonGalleryGroup::scrollRow() const
{
    Q_D(const RibbonGalleryGroup);
    return d->m_scrollRow;
}

void RibbonGalleryGroup::setScrollRow(int row)
{
    Q_D(RibbonGalleryGroup);
    const int next = qBound(0, row, maxScrollRow());
    if (d->m_scrollRow == next) {
        return;
    }
    d->m_scrollRow = next;
    update();
}

void RibbonGalleryGroup::scrollByRows(int rows)
{
    setScrollRow(scrollRow() + rows);
}

void RibbonGalleryGroup::scrollToItem(int index)
{
    Q_D(RibbonGalleryGroup);
    if (index < 0 || d->m_viewportRows <= 0) {
        return;
    }
    const int row = index / qMax(1, d->columnCount());
    if (row < d->m_scrollRow) {
        setScrollRow(row);
    } else if (row >= d->m_scrollRow + d->m_viewportRows) {
        setScrollRow(row - d->m_viewportRows + 1);
    }
}

int RibbonGalleryGroup::maxScrollRow() const
{
    Q_D(const RibbonGalleryGroup);
    if (d->m_viewportRows <= 0) {
        return 0;
    }
    return qMax(0, d->rowCount() - d->m_viewportRows);
}

QSize RibbonGalleryGroup::sizeHint() const
{
    Q_D(const RibbonGalleryGroup);
    const int columns = qMax(1, d->columnCount());
    const int rows = (d->m_viewportRows > 0) ? qMin(d->m_viewportRows, d->rowCount()) : d->rowCount();
    const int stepX = d->m_grid.width() + d->m_spacing;
    const int stepY = d->m_grid.height() + d->m_spacing;
    return QSize(columns * stepX - d->m_spacing + 2 * d->m_spacing,
                 qMax(1, rows) * stepY - d->m_spacing + 2 * d->m_spacing);
}

QActionGroup *RibbonGalleryGroup::actionGroup() const
{
    Q_D(const RibbonGalleryGroup);
    return d->m_actionGroup;
}

void RibbonGalleryGroup::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    Q_D(RibbonGalleryGroup);
    QPainter painter(this);
    RibbonGalleryPaintSpec spec;
    spec.actions = d->m_actions;
    spec.grid = d->m_grid;
    spec.spacing = d->m_spacing;
    spec.columns = d->columnCount();
    spec.style = d->m_preStyle;
    spec.palette = palette();
    spec.font = font();
    spec.hoverIndex = d->m_hoverIndex;
    spec.selectedIndex = d->m_selectedIndex;
    spec.pressedIndex = d->m_pressedIndex;
    spec.firstRow = d->m_scrollRow;
    spec.visibleRows = d->m_viewportRows;
    galleryPaintGrid(&painter, rect().adjusted(d->m_spacing, d->m_spacing, -d->m_spacing, -d->m_spacing), spec);
}

void RibbonGalleryGroup::mousePressEvent(QMouseEvent *event)
{
    Q_D(RibbonGalleryGroup);
    if (event->button() != Qt::LeftButton) {
        QWidget::mousePressEvent(event);
        return;
    }
    d->m_pressedIndex = itemAt(event->pos());
    update();
    event->accept();
}

void RibbonGalleryGroup::mouseReleaseEvent(QMouseEvent *event)
{
    Q_D(RibbonGalleryGroup);
    if (event->button() != Qt::LeftButton) {
        QWidget::mouseReleaseEvent(event);
        return;
    }
    const int index = itemAt(event->pos());
    const int pressed = d->m_pressedIndex;
    d->m_pressedIndex = -1;
    if (index >= 0 && index == pressed) {
        selectByIndex(index);
        QAction *act = d->m_actions.value(index, Q_NULLPTR);
        if (act && act->isEnabled()) {
            act->trigger();
        }
    }
    update();
    event->accept();
}

void RibbonGalleryGroup::mouseMoveEvent(QMouseEvent *event)
{
    Q_D(RibbonGalleryGroup);
    const int index = itemAt(event->pos());
    if (index == d->m_hoverIndex) {
        event->accept();
        return;
    }
    d->m_hoverIndex = index;
    update();
    // Hovering is an action event like any other, so the group's hovered() carries
    // the same action the rest of the ribbon would see.
    QAction *act = d->m_actions.value(index, Q_NULLPTR);
    if (act && act->isEnabled()) {
        act->activate(QAction::Hover);
    }
    event->accept();
}

void RibbonGalleryGroup::leaveEvent(QEvent *event)
{
    Q_D(RibbonGalleryGroup);
    if (d->m_hoverIndex != -1) {
        d->m_hoverIndex = -1;
        update();
    }
    QWidget::leaveEvent(event);
}

void RibbonGalleryGroup::wheelEvent(QWheelEvent *event)
{
    Q_D(RibbonGalleryGroup);
    if (d->m_viewportRows <= 0 || maxScrollRow() == 0) {
        QWidget::wheelEvent(event);
        return;
    }
    scrollByRows(event->angleDelta().y() < 0 ? 1 : -1);
    event->accept();
}

void RibbonGalleryGroup::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);
    recalcGridSize(event->size().height());
}

void RibbonGalleryGroup::changeEvent(QEvent *event)
{
    // Every colour the items use comes from the palette, so a new palette is a
    // full repaint and nothing else.
    if (event->type() == QEvent::PaletteChange) {
        update();
    }
    QWidget::changeEvent(event);
}

QX_RIBBON_END_NAMESPACE
