/**
 * Copyleft (C) 2023 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MIT
 **/
#include "ribbongrouplayout.h"
#include "ribbongroup.h"
#include "ribbongroup_p.h"
#include "ribbonmanager.h"

#include <QQueue>
#include <QWidgetAction>

QX_RIBBON_BEGIN_NAMESPACE

#define RibbonGroupLayout_DEBUG_PRINT 0
#define HELP_DRAW_RECT(p, rect)                                                                                        \
    do {                                                                                                               \
        p.save();                                                                                                      \
        QPen _pen(Qt::DashLine);                                                                                       \
        _pen.setColor(Qt::blue);                                                                                       \
        p.setPen(_pen);                                                                                                \
        p.setBrush(QBrush());                                                                                          \
        p.drawRect(rect);                                                                                              \
        p.restore();                                                                                                   \
    } while (0)

RibbonGroupItem::RibbonGroupItem(QWidget *widget)
    : QWidgetItem(widget)
    , action(Q_NULLPTR)
    , customWidget(false)
{
}

bool RibbonGroupItem::isEmpty() const
{
    return (action == Q_NULLPTR || !action->isVisible());
}

class RibbonGroupLayoutPrivate
{
    QX_DECLARE_PUBLIC(RibbonGroupLayout)
public:
    RibbonGroupLayoutPrivate();

    // Compute the window width and maximum width based on the column count
    void columnWidthInfo(int colindex, int &width, int &maximum) const;

    // Recompute expand sizes; must be called after updateGeomArray
    void recalcExpandGeomArray(const QRect &setrect);

    RibbonGroupItem *createItem(QAction *action, RibbonGroup::RowProportion rp = RibbonGroup::Auto);
    void updateGeomArray(const QRect &setrect);
    void layoutActions();
public:
    QList<RibbonGroupItem *> m_items;
    QSize m_sizeHint;   ///< Size returned by sizeHint
    static QMargins s_contentsMargins;
    int m_columnCount;   ///< Number of columns
    int m_largeHeight;   ///< Height of the large icon
    bool m_expandFlag;   ///< Whether the layout can expand
    bool m_dirty;        ///< Marks whether elements need refreshing; see QToolBarLayout source
    bool m_reduced;      ///< B11: place buttons that would own a column as small ones instead
};

QMargins RibbonGroupLayoutPrivate::s_contentsMargins = QMargins(1, 1, 1, 1);

RibbonGroupLayoutPrivate::RibbonGroupLayoutPrivate()
    : m_columnCount(0)
    , m_expandFlag(false)
    , m_dirty(true)
    , m_reduced(false)
{
}

/**
 * @brief Compute the window width and maximum width based on the column count
 * @param colIndex
 * @param width Returns -1 if the given column does not exist
 * @param maximum Returns -1 if the given column does not exist
 */
void RibbonGroupLayoutPrivate::columnWidthInfo(int colIndex, int &width, int &maximum) const
{
    width = -1;
    maximum = -1;
    for (RibbonGroupItem *item : std::as_const(m_items)) {
        if (!item->isEmpty() && (item->columnIndex == colIndex)) {
            width = qMax(width, item->willGeometry.width());
            maximum = qMax(maximum, item->widget()->maximumWidth());
        }
    }
}

void RibbonGroupLayoutPrivate::recalcExpandGeomArray(const QRect &setrect)
{
    // Compute the expandable size
    int expandwidth = setrect.width() - m_sizeHint.width();

    if (expandwidth <= 0) {
        return;
    }
    // Column expansion info
    struct _columnExpandInfo {
        int oldColumnWidth = 0;        ///< Original column width
        int columnMaximumWidth = -1;   ///< Maximum column width
        int columnExpandedWidth = 0;   ///< Column width after expansion
        QList<RibbonGroupItem *> expandItems;
    };
    // Records the columns and widgets that can expand horizontally; after layout, remaining
    // space is used to expand the horizontally expandable widgets
    QMap<int, _columnExpandInfo> columnExpandInfo;

    for (RibbonGroupItem *item : std::as_const(m_items)) {
        if ((!item->isEmpty()) && item->expandingDirections() & Qt::Horizontal) {
            // Only take the visible ones
            QMap<int, _columnExpandInfo>::iterator i = columnExpandInfo.find(item->columnIndex);
            if (i == columnExpandInfo.end()) {
                i = columnExpandInfo.insert(item->columnIndex, _columnExpandInfo());
            }
            i.value().expandItems.append(item);
        }
    }
    if (columnExpandInfo.size() <= 0) {
        // Nothing to expand, so exit
        return;
    }
    // After collecting the expandable columns and widgets, compute the size of each column
    // Compute the expandable size
    int oneColCanExpandWidth = expandwidth / columnExpandInfo.size();

    for (QMap<int, _columnExpandInfo>::iterator i = columnExpandInfo.begin(); i != columnExpandInfo.end();) {
        int &oldColumnWidth = i.value().oldColumnWidth;
        int &columnMaximumWidth = i.value().columnMaximumWidth;
        columnWidthInfo(i.key(), oldColumnWidth, columnMaximumWidth);
        if ((oldColumnWidth <= 0) || (oldColumnWidth > columnMaximumWidth)) {
            // A value <= 0 means the column does not exist, which is abnormal; erase and continue
            // oldColumnWidth > columnMaximumWidth is also abnormal
            i = columnExpandInfo.erase(i);
            continue;
        }
        // Start adjusting
        int colWidth = oneColCanExpandWidth + oldColumnWidth;   // Expand first
        if (colWidth >= columnMaximumWidth) {
            i.value().columnExpandedWidth = columnMaximumWidth;
        } else {
            i.value().columnExpandedWidth = colWidth;
        }
        ++i;
    }
    // Readjust the sizes
    // Because other columns are affected, all of them must be traversed
    for (auto i = columnExpandInfo.begin(); i != columnExpandInfo.end(); ++i) {
        int moveXLen = i.value().columnExpandedWidth - i.value().oldColumnWidth;
        for (RibbonGroupItem *item : this->m_items) {
            if (item->isEmpty() || (item->columnIndex < i.key())) {
                // Earlier columns can be ignored
                continue;
            }
            if (item->columnIndex == i.key()) {
                // Expansion of this column
                if (i.value().expandItems.contains(item)) {
                    // Only items that need to expand in this column get resized
                    item->willGeometry.setWidth(i.value().columnExpandedWidth);
                } else {
                    // Non-expanding widgets in this column keep their original size
                    continue;
                }
            } else {
                // Move the following ones
                item->willGeometry.moveLeft(item->willGeometry.x() + moveXLen);
            }
        }
    }
}

/**
 * @brief Convert an action into an item
 *
 * This function is based on QToolBarItem *QToolBarLayout::createItem(QAction *action)
 *
 * For a normal QAction, this function creates a RibbonButton; the button type is based on
 * RibbonGroupItem::RowProportion
 * @param action
 * @param rp Row height proportion
 * @return The converted RibbonGroupItem
 * @note Every RibbonGroupItem ultimately carries a widget; if a QWidgetAction is passed in, its
 * widget is used directly, otherwise a RibbonButton is created internally
 */
RibbonGroupItem *RibbonGroupLayoutPrivate::createItem(QAction *action, RibbonGroup::RowProportion rp)
{
    Q_Q(RibbonGroupLayout);
    bool customWidget = false;
    QWidget *widget = Q_NULLPTR;
    RibbonGroup *group = qobject_cast<RibbonGroup *>(q->parentWidget());
    if (!group) {
        return Q_NULLPTR;
    }
    if (QWidgetAction *widgetAction = qobject_cast<QWidgetAction *>(action)) {
        widget = widgetAction->requestWidget(group);
        if (widget != Q_NULLPTR) {
            widget->setAttribute(Qt::WA_LayoutUsesWidgetRect);
            // Mark as true; on removal this widget is not deleted, whereas false (default)
            // causes deletion, e.g. RibbonSeparator and RibbonButton
            customWidget = true;
        }
    } else if (action->isSeparator()) {
        RibbonSeparator *sep = new RibbonSeparator(group);
        widget = sep;
    }
    // Not a widget, so create a RibbonButton automatically
    if (!widget) {
        RibbonButton::RibbonButtonType buttonType =
            ((rp == RibbonGroup::Large) ? RibbonButton::LargeButton : RibbonButton::SmallButton);

        RibbonButton *button = new RibbonButton(group);
        button->setAutoRaise(true);
        button->setFocusPolicy(Qt::NoFocus);
        button->setButtonType(buttonType);
        if (RibbonButton::LargeButton == buttonType) {
            // Set the button style according to the group mode
            button->setLargeButtonType(group->isTwoRow() ? RibbonButton::Lite : RibbonButton::Normal);
        }
        button->setDefaultAction(action);

        QObject::connect(button, &RibbonButton::triggered, group, &RibbonGroup::actionTriggered);
        widget = button;
    }
    // At this point there is always a widget
    widget->hide();
    RibbonGroupItem *result = new RibbonGroupItem(widget);

    result->rowProportion = rp;
    result->customWidget = customWidget;
    result->action = action;
    return result;
}

/**
 * @brief Update the sizes
 */
void RibbonGroupLayoutPrivate::updateGeomArray(const QRect &setrect)
{
    Q_Q(RibbonGroupLayout);
    RibbonGroup *group = qobject_cast<RibbonGroup *>(q->parentWidget());
    if (!group) {
        return;
    }
#if RibbonGroupLayout_DEBUG_PRINT
    qDebug() << "RibbonGroupLayoutPrivate::updateGeomArray(" << setrect << ")";
#endif
    int height = setrect.height();
    const QMargins &margin = q->groupContentsMargins();
    const int spacing = q->spacing();
    int x = margin.left();

    // Get the group layout mode: 3 rows or 2 rows
    const short rowCount = (group->groupLayoutMode() == RibbonGroup::ThreeRowMode) ? 3 : 2;
    // largeHeight is the height for the large proportion; group->titleHeight() returns 0 in two-row mode
    const int largeHeight = q->calcLargeHeight(setrect, group);
    m_largeHeight = largeHeight;
    // Compute the smallHeight
    const int smallHeight = (largeHeight - (rowCount - 1) * spacing) / rowCount;

    // Y position of the Medium row
    // Three-row mode: assuming the gap above/middle/below two Mediums is mediumSpacing and each
    // Medium has height smallHeight, then largeHeight = mediumSpacing + smallHeight + mediumSpacing
    // + smallHeight + mediumSpacing = 3 * mediumSpacing + 2 * smallHeight,
    // i.e. mediumSpacing = (largeHeight - 2 * smallHeight) / 3
    const int mediumSpacing = (largeHeight - 2 * smallHeight) / 3;
    const int yMediumRow0 = (2 == rowCount) ? margin.top() : (margin.top() + mediumSpacing);
    const int yMediumRow1 =
        (2 == rowCount) ? (yMediumRow0 + smallHeight + spacing) : (yMediumRow0 + smallHeight + mediumSpacing);
    // Y position of the Small row
    const int ySmallRow0 = margin.top();
    const int ySmallRow1 = ySmallRow0 + smallHeight + spacing;
    const int ySmallRow2 = ySmallRow1 + smallHeight + spacing;

    // Two-row mode:
    // yMediumRow0 == ySmallRow0
    // yMediumRow1 == ySmallRow1

    // row records which row the next item should belong to; item->rowIndex records the current row
    // item->rowIndex is mainly used for RibbonGroupItem::Medium
    short row = 0;
    int column = 0;
    // Records the maximum width of each column
    int columnMaxWidth = 0;
    // Records the total width
    int totalWidth = 0;

    int itemCount = m_items.count();

#if RibbonGroupLayout_DEBUG_PRINT
    qDebug() << "\r\n\r\n============================================="
             << "\r\RibbonGroupLayoutPrivate::updateGeomArray()"
             << " setrect:" << setrect << "\r\ngroup name:" << group->windowTitle()
             << "\r\n largeHeight:" << largeHeight << "\r\n smallHeight:" << smallHeight
             << "\r\n rowCount:" << rowCount;
#endif

    // Proportion of the first/second row of this column
    RibbonGroup::RowProportion thisColumnRP0 = RibbonGroup::Auto;
    RibbonGroupItem *lastGeomItem = Q_NULLPTR;   // Records the last item whose position was set

    // Next column: x advances to the next position, the column count increases, the row count
    // resets to zero, and the max column width resets to zero
    auto startNextColumn = [&]() {
        x += (columnMaxWidth + spacing);
        ++column;
        row = 0;
        columnMaxWidth = 0;
    };

    for (int i = 0; i < itemCount; ++i) {
        RibbonGroupItem *item = m_items.at(i);
        if (item->isEmpty()) {
            // Skip directly if hidden
#if RibbonGroupLayout_DEBUG_PRINT
            qDebug() << item->widget()->metaObject()->className() << "is hide"
                     << " row:" << row << " col:" << column;
#endif
            item->rowIndex = -1;
            item->columnIndex = -1;
            continue;
        }

        QSize hint = item->sizeHint();
        Qt::Orientations exp = item->expandingDirections();
        if (item->widget()) {
            // A widget that expands horizontally marks this as expanding
            if ((item->widget()->sizePolicy().horizontalPolicy() & QSizePolicy::ExpandFlag)) {
                m_expandFlag = true;
            }
        }
        RibbonGroup::RowProportion rp = item->rowProportion;
        if (RibbonGroup::Auto == rp) {
            // Determine the row proportion: if it expands vertically, use Large, otherwise Small
            if (exp & Qt::Vertical) {
                rp = RibbonGroup::Large;
            } else {
                rp = RibbonGroup::Small;
            }
        }
        // B11: a reduced group is narrower than its natural width, because a button
        // that would otherwise own a whole column is packed as a small one instead.
        // Items that are not buttons - a gallery, a widget added through addWidget()
        // - keep the proportion they were added with: their height cannot be
        // squeezed, so demoting them would only clip them.
        if (m_reduced && (RibbonGroup::Small != rp) && qobject_cast<RibbonButton *>(item->widget())) {
            rp = RibbonGroup::Small;
        }
        // Start the layout according to the proportion and the layout mode
        switch (rp) {
        case RibbonGroup::Large: {
            // !! In Large, if not on the first row of a new column, a column break is needed
            // Always place large at the start of the next column
            if (row != 0) {
                startNextColumn();
            }
            item->rowIndex = 0;
            item->columnIndex = column;
            item->willGeometry = QRect(x, margin.top(), hint.width(), largeHeight);
            columnMaxWidth = hint.width();
            startNextColumn();
        } break;

        case RibbonGroup::Medium: {
            // In two-row mode Medium never reaches row==2 (it starts a new column after occupying
            // the second row), so the three-row handling below already covers two-row mode; the
            // two previously separate paths are equivalent.
            if (0 == row) {
                item->rowIndex = 0;
                item->columnIndex = column;
                item->willGeometry = QRect(x, yMediumRow0, hint.width(), smallHeight);
                thisColumnRP0 = RibbonGroup::Medium;
                columnMaxWidth = hint.width();
                // Next row is 1, x unchanged
                row = 1;
            } else if (1 == row) {
                item->rowIndex = 1;
                item->columnIndex = column;
                item->willGeometry = QRect(x, yMediumRow1, hint.width(), smallHeight);
                // Compare with the previous one to get the longest width
                columnMaxWidth = qMax(columnMaxWidth, hint.width());
                startNextColumn();
            } else {
                // The first two rows of the current column are Small and now another Medium arrives:
                // start a new column first, then it is equivalent to 0 == row
                startNextColumn();
                item->rowIndex = 0;
                item->columnIndex = column;
                item->willGeometry = QRect(x, yMediumRow0, hint.width(), smallHeight);
                thisColumnRP0 = RibbonGroup::Medium;
                columnMaxWidth = hint.width();
                row = 1;
            }
        } break;

        case RibbonGroup::Small: {
            // Three-row mode with the first row of the current column being Medium: this Small is
            // treated as a second Medium (occupies yMediumRow1 and starts a new column)
            const bool pairedWithMedium = (3 == rowCount) && (RibbonGroup::Medium == thisColumnRP0);
            if (0 == row) {
                // First row
                item->rowIndex = 0;
                item->columnIndex = column;
                item->willGeometry = QRect(x, ySmallRow0, hint.width(), smallHeight);
                thisColumnRP0 = RibbonGroup::Small;
                columnMaxWidth = hint.width();
                // Next row is 1, x unchanged
                row = 1;
            } else if (1 == row) {
                // Second row
                item->rowIndex = 1;
                item->columnIndex = column;
                // Two-row mode: ySmallRow1 == yMediumRow1, so it does not matter whether the first
                // row is Medium or Small
                item->willGeometry = QRect(x, ySmallRow1, hint.width(), smallHeight);
                if (pairedWithMedium) {
                    // Three-row mode and the first row is Medium
                    item->willGeometry = QRect(x, yMediumRow1, hint.width(), smallHeight);
                }
                // Compare with the previous one to get the longest width
                columnMaxWidth = qMax(columnMaxWidth, hint.width());
                // Two-row mode starts a new column here; three-row mode keeps increasing the row
                // count (or starts a new column when the first row is Medium)
                if ((2 == rowCount) || pairedWithMedium) {
                    startNextColumn();
                } else {
                    row = 2;
                }
            } else {
                // Third row
                item->rowIndex = 2;
                item->columnIndex = column;
                item->willGeometry = QRect(x, ySmallRow2, hint.width(), smallHeight);
                columnMaxWidth = qMax(columnMaxWidth, hint.width());
                startNextColumn();
            }
        } break;

        default:
            // Should never happen
            break;
        }
        lastGeomItem = item;
#if RibbonGroupLayout_DEBUG_PRINT
        qDebug() << item->widget()->metaObject()->className() << " rp:" << rp << " row:" << item->rowIndex
                 << " col:" << item->columnIndex << " new row:" << row << " new column:" << column
                 << " willGeometry:" << item->willGeometry << " sizeHint:" << hint << " x:" << x;
#endif
    }
    // The last element updates the column count
    //  2022-06-20 This used to be inside the loop; if the last element is hidden, this check would
    //  never be reached, causing an anomaly
    if (lastGeomItem) {   // The last element updates totalWidth
        if (lastGeomItem->columnIndex != column) {
            // The last element is at the final position and triggered a column break, so the real
            // column count must be decremented by 1, i.e. equal to the column index
            m_columnCount = column;
            // Since the last element triggered a column break, x is the position of the new column
            // and can be used directly as totalWidth
            totalWidth = x + margin.right();
        } else {
            // The last element is not at the final position and did not trigger the next column
            // break, so the real column count equals the column index + 1
            m_columnCount = column + 1;
            // Since the last element did not trigger a column break, totalWidth must be computed
            totalWidth = x + columnMaxWidth + spacing + margin.right();
        }
    }
    // When there is an optionButton and the title is not visible, totalWidth must be adjusted
    if (group->hasOptionAction()) {
        if (!group->titleVisible()) {
            totalWidth += group->optionActionButtonSize().width();
        }
    }
    // After all widgets are placed, set the widgets with the expanding property
    if (totalWidth < setrect.width()) {
        // The widgets with the expanding property can be set now
        recalcExpandGeomArray(setrect);
    }
    m_sizeHint = QSize(totalWidth, height);
}

/**
 * @brief Lay out all actions
 */
void RibbonGroupLayoutPrivate::layoutActions()
{
    Q_Q(RibbonGroupLayout);
    if (m_dirty) {
        updateGeomArray(q->geometry());
    }
    QList<QWidget *> showWidgets, hideWidgets;

#if RibbonGroupLayout_DEBUG_PRINT
    qDebug() << "\r\n\r\n =============================================="
                "\r\n RibbonGroupLayoutPrivate::layoutActions"
             << " \r\n name:" << q->parentWidget()->windowTitle() << " sizehint:" << q->sizeHint();
#endif
    for (RibbonGroupItem *item : std::as_const(m_items)) {
        if (item->isEmpty()) {
            hideWidgets << item->widget();
        } else {
            item->setGeometry(item->willGeometry);
            // item->widget()->setFixedSize(item->willGeometry.size());
            // item->widget()->move(item->willGeometry.topLeft());
            showWidgets << item->widget();
#if RibbonGroupLayout_DEBUG_PRINT
            qDebug() << "[" << item->rowIndex << "," << item->columnIndex << "]"
                     << " -> " << item->willGeometry << ":" << item->widget()->metaObject()->className();
#endif
        }
    }

    // Show and hide are not done above because that would trigger a repaint of
    // RibbonGroupLayout, causing recursive painting and hurting efficiency
    for (QWidget *w : std::as_const(showWidgets)) {
        w->show();
    }
    for (QWidget *w : std::as_const(hideWidgets)) {
        w->hide();
    }
}

RibbonGroupLayout::RibbonGroupLayout(QWidget *p)
    : QLayout(p)
{
    QX_INIT_PRIVATE(RibbonGroupLayout)
    setSpacing(1);
}

RibbonGroupLayout::~RibbonGroupLayout()
{
    Q_D(RibbonGroupLayout);
    while (!d->m_items.isEmpty()) {
        RibbonGroupItem *item = d->m_items.takeFirst();
        if (QWidgetAction *widgetAction = qobject_cast<QWidgetAction *>(item->action)) {
            if (item->customWidget) {
                widgetAction->releaseWidget(item->widget());
            }
        }
        delete item;
    }
    QX_FINI_PRIVATE()
}

/**
 * @brief Find the index by action, used by actionEvent when adding an action
 * @param action
 * @return Returns -1 if not found
 */
int RibbonGroupLayout::indexOf(QAction *action) const
{
    Q_D(const RibbonGroupLayout);
    for (int i = 0; i < d->m_items.count(); ++i) {
        if (d->m_items.at(i)->action == action) {
            return i;
        }
    }
    return -1;
}

void RibbonGroupLayout::addItem(QLayoutItem *item)
{
    Q_UNUSED(item);
    qWarning("RibbonGroupLayout::addItem(): please use insertAction() instead");
    return;
}

/**
 * @brief RibbonGroup mainly uses this function to add actions
 * @param index Insert position
 * @param act
 * @param rp Layout strategy
 */
void RibbonGroupLayout::insertAction(int index, QAction *act, RibbonGroup::RowProportion rp)
{
    Q_D(RibbonGroupLayout);
    if (index < 0 || index > d->m_items.count()) {
        index = d->m_items.count();
    }
    RibbonGroupItem *item = d->createItem(act, rp);

    if (item) {
        d->m_items.insert(index, item);
        // Mark that the sizes need to be recomputed
        invalidate();
    }
}

QLayoutItem *RibbonGroupLayout::itemAt(int index) const
{
    Q_D(const RibbonGroupLayout);
    if ((index < 0) || (index >= d->m_items.count())) {
        return Q_NULLPTR;
    }
    return d->m_items.at(index);
}

QLayoutItem *RibbonGroupLayout::takeAt(int index)
{
    Q_D(RibbonGroupLayout);
    if ((index < 0) || (index >= d->m_items.count())) {
        return Q_NULLPTR;
    }
    RibbonGroupItem *item = d->m_items.takeAt(index);

    QWidgetAction *widgetAction = qobject_cast<QWidgetAction *>(item->action);

    if ((widgetAction != Q_NULLPTR) && item->customWidget) {
        widgetAction->releaseWidget(item->widget());
    } else {
        // destroy the RibbonButton/RibbonSeparator
        item->widget()->hide();
        item->widget()->deleteLater();
    }

    invalidate();
    return item;
}

int RibbonGroupLayout::count() const
{
    Q_D(const RibbonGroupLayout);
    return d->m_items.count();
}

bool RibbonGroupLayout::isEmpty() const
{
    Q_D(const RibbonGroupLayout);
    return d->m_items.isEmpty();
}

void RibbonGroupLayout::invalidate()
{
    Q_D(RibbonGroupLayout);
    d->m_dirty = true;
    QLayout::invalidate();
}

Qt::Orientations RibbonGroupLayout::expandingDirections() const
{
    return Qt::Horizontal;
}

QSize RibbonGroupLayout::minimumSize() const
{
    Q_D(const RibbonGroupLayout);
    return d->m_sizeHint;
}

QSize RibbonGroupLayout::sizeHint() const
{
    Q_D(const RibbonGroupLayout);
    return d->m_sizeHint;
}

RibbonGroupItem *RibbonGroupLayout::groupItem(QAction *action) const
{
    Q_D(const RibbonGroupLayout);
    int index = indexOf(action);

    if (index >= 0) {
        return d->m_items[index];
    }
    return Q_NULLPTR;
}

RibbonGroupItem *RibbonGroupLayout::lastItem() const
{
    Q_D(const RibbonGroupLayout);
    if (d->m_items.isEmpty()) {
        return Q_NULLPTR;
    }
    return d->m_items.last();
}

/**
 * @brief Get the last created widget
 * @return Returns Q_NULLPTR if there is no widget or the item is empty
 */
QWidget *RibbonGroupLayout::lastWidget() const
{
    RibbonGroupItem *item = lastItem();

    if (item) {
        return item->widget();
    }
    return Q_NULLPTR;
}

/**
 * @brief Move two items
 * @param from
 * @param to
 * @note After moving, everything is invalidated and a relayout is required
 */
void RibbonGroupLayout::move(int from, int to)
{
    Q_D(RibbonGroupLayout);
    int c = count();
    if (c <= 0) {
        return;
    }
    if (from == to) {
        return;
    }
    if (from < 0 || from >= c || to < 0 || to >= c) {
        return;
    }
    d->m_items.move(from, to);
    invalidate();
}

/**
 * @brief Determine whether a relayout is needed
 * @return
 */
bool RibbonGroupLayout::isDirty() const
{
    Q_D(const RibbonGroupLayout);
    return d->m_dirty;
}

/**
 * @brief Compute the height of the large icon
 * @param setrect
 * @param group
 * @return
 */
int RibbonGroupLayout::calcLargeHeight(const QRect &setrect, const RibbonGroup *group)
{
    const QMargins &m = groupContentsMargins();
    return setrect.height() - m.top() - m.bottom() - group->titleHeight();
}

/**
 * @brief The global contentsMargins
 *
 * The ribbonbar height estimation needs to know the group contentsMargins in advance, so it is
 * made global; making it global also unifies the style
 *
 * @return
 */
const QMargins &RibbonGroupLayout::groupContentsMargins()
{
    return RibbonGroupLayoutPrivate::s_contentsMargins;
}

/**
 * @brief The global contentsMargins
 * @note RibbonStyleOption uses the groupContentsMargins function; after calling the setter the
 * RibbonStyleOption contents must be recomputed manually, @sa
 * RibbonStyleOption::recalc
 * @sa RibbonStyleOption
 * @param m
 */
void RibbonGroupLayout::setGroupContentsMargins(const QMargins &m)
{
    if (RibbonGroupLayoutPrivate::s_contentsMargins != m) {
        RibbonGroupLayoutPrivate::s_contentsMargins = m;
        RibbonElementStyleOpt.recalc();
    }
}

const QList<RibbonGroupItem *> &RibbonGroupLayout::ribbonGroupItems() const
{
    Q_D(const RibbonGroupLayout);
    return d->m_items;
}

void RibbonGroupLayout::setGeometry(const QRect &rect)
{
    Q_D(RibbonGroupLayout);
    d->m_dirty = false;
    d->updateGeomArray(rect);
    QLayout::setGeometry(rect);
    d->layoutActions();
}

/**
 * @brief B11: choose between the natural and the reduced placement
 *
 * A reduced layout packs a button that would own a whole column as a small one
 * instead, which costs rows but saves a column; that is what makes a group narrower
 * than its sizeHint. RibbonGroup drives this together with the button presentation,
 * because the placement and the way each button is drawn have to agree.
 */
void RibbonGroupLayout::setReduced(bool reduced)
{
    Q_D(RibbonGroupLayout);
    if (d->m_reduced == reduced) {
        return;
    }
    d->m_reduced = reduced;
    invalidate();
}

/**
 * @brief B11: refresh the cached size hint from the current geometry
 *
 * setGeometry() does the same and then lays the items out. While the two
 * presentations of a group are being measured the items are laid out by the pass
 * that follows anyway, so only the sizes are recomputed here.
 */
void RibbonGroupLayout::recalcSizeHint()
{
    Q_D(RibbonGroupLayout);
    QWidget *w = parentWidget();

    if (w) {
        d->updateGeomArray(w->rect());
    }
}

QX_RIBBON_END_NAMESPACE
