/**
 * Copyleft (C) 2023 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MIT
**/
#include "ribbongallerygroup.h"
#include "ribbongallerygroup_p.h"

#include <QActionGroup>
#include <QDebug>
#include <QItemSelectionModel>
#include <QPainter>

QX_RIBBON_BEGIN_NAMESPACE

/* RibbonGalleryItemPrivate */
class RibbonGalleryItemPrivate
{
public:
    RibbonGalleryItemPrivate();
public:
    QMap<int, QVariant> m_datas;
    Qt::ItemFlags m_flags;
    QAction *m_action;
};

RibbonGalleryItemPrivate::RibbonGalleryItemPrivate()
    : m_flags(Qt::ItemIsEnabled | Qt::ItemIsSelectable)
    , m_action(Q_NULLPTR)
{

}

/* RibbonGalleryItem */
RibbonGalleryItem::RibbonGalleryItem()
    : d(new RibbonGalleryItemPrivate)
{
    setTextAlignment(Qt::AlignTop | Qt::AlignHCenter);
}

RibbonGalleryItem::RibbonGalleryItem(const QString &text, const QIcon &icon)
    : RibbonGalleryItem()
{
    setText(text);
    setIcon(icon);
}

RibbonGalleryItem::RibbonGalleryItem(QAction *act)
    : RibbonGalleryItem()
{
    setAction(act);
}

RibbonGalleryItem::~RibbonGalleryItem()
{
    delete d;
}

void RibbonGalleryItem::setData(int role, const QVariant &data)
{
    d->m_datas[role] = data;
}

QVariant RibbonGalleryItem::data(int role) const
{
    if (d->m_action) {
        switch (role) {
        case Qt::DisplayRole:
            return d->m_action->text();

        case Qt::ToolTipRole:
            return d->m_action->toolTip();

        case Qt::DecorationRole:
            return d->m_action->icon();
        default:
            break;
        }
    }
    return d->m_datas.value(role);
}

void RibbonGalleryItem::setText(const QString &text)
{
    setData(Qt::DisplayRole, text);
}

QString RibbonGalleryItem::text() const
{
    return data(Qt::DisplayRole).toString();
}

void RibbonGalleryItem::setToolTip(const QString &text)
{
    setData(Qt::ToolTipRole, text);
}

QString RibbonGalleryItem::toolTip() const
{
    return data(Qt::ToolTipRole).toString();
}

void RibbonGalleryItem::setIcon(const QIcon &ico)
{
    setData(Qt::DecorationRole, ico);
}

QIcon RibbonGalleryItem::icon() const
{
    return qvariant_cast<QIcon>(data(Qt::DecorationRole));
}

bool RibbonGalleryItem::isSelectable() const
{
    return (d->m_flags & Qt::ItemIsSelectable);
}

void RibbonGalleryItem::setSelectable(bool isSelectable)
{
    if (isSelectable) {
        d->m_flags |= Qt::ItemIsSelectable;
    } else {
        d->m_flags &= ~Qt::ItemIsSelectable;
    }
}

bool RibbonGalleryItem::isEnable() const
{
    return (d->m_flags & Qt::ItemIsEnabled);
}

void RibbonGalleryItem::setEnable(bool isEnable)
{
    if (d->m_action) {
        d->m_action->setEnabled(isEnable);
    }

    if (isEnable) {
        d->m_flags |= Qt::ItemIsEnabled;
    } else {
        d->m_flags &= ~Qt::ItemIsEnabled;
    }
}

void RibbonGalleryItem::setFlags(Qt::ItemFlags flag)
{
    d->m_flags = flag;
    if (d->m_action) {
        d->m_action->setEnabled(flag & Qt::ItemIsEnabled);
    }
}

Qt::ItemFlags RibbonGalleryItem::flags() const
{
    return d->m_flags;
}

void RibbonGalleryItem::setAction(QAction *act)
{
    d->m_action = act;
    if (Q_NULLPTR == act) {
        return;
    }
    if (act->isEnabled()) {
        d->m_flags |= Qt::ItemIsEnabled;
    } else {
        d->m_flags &= ~Qt::ItemIsEnabled;
    }
}

QAction *RibbonGalleryItem::action()
{
    return d->m_action;
}

void RibbonGalleryItem::setTextAlignment(Qt::Alignment a)
{
    setData(Qt::TextAlignmentRole, (int)a);
}

Qt::Alignment RibbonGalleryItem::textAlignment() const
{
    return qvariant_cast<Qt::Alignment>(data(Qt::TextAlignmentRole));
}

/* RibbonGalleryGroupItemDelegate */
RibbonGalleryGroupItemDelegate::RibbonGalleryGroupItemDelegate(RibbonGalleryGroup *group, QObject *parent)
    : QStyledItemDelegate(parent), m_group(group)
{
}

void RibbonGalleryGroupItemDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option,
                                            const QModelIndex &index) const
{
    if (Q_NULLPTR == m_group) {
        return;
    }
    switch (m_group->galleryGroupStyle()) {
    case RibbonGalleryGroup::IconWithText:
        paintIconWithText(painter, option, index);
        break;
    case RibbonGalleryGroup::IconWithWordWrapText:
        paintIconWithTextWordWrap(painter, option, index);
        break;
    case RibbonGalleryGroup::IconOnly:
        paintIconOnly(painter, option, index);
        break;
    default:
        paintIconWithText(painter, option, index);
        break;
    }
}

void RibbonGalleryGroupItemDelegate::paintIconOnly(QPainter *painter, const QStyleOptionViewItem &option,
                                                    const QModelIndex &index) const
{
    QStyle *style = m_group->style();
    int sp = m_group->spacing();
    QRect rect = option.rect;
    rect.adjust(sp, sp, -sp, -sp);
    painter->save();
    painter->setClipRect(rect);
    style->drawPrimitive(QStyle::PE_PanelItemViewItem, &option, painter, m_group);
    // draw the icon
    QRect iconRect = option.rect;
    sp += 2;
    iconRect.adjust(sp, sp, -sp, -sp);
    QIcon ico = qvariant_cast<QIcon>(index.data(Qt::DecorationRole));
    ico.paint(painter, iconRect, Qt::AlignCenter, QIcon::Normal, QIcon::On);
    painter->restore();
}

void RibbonGalleryGroupItemDelegate::paintIconWithText(QPainter *painter, const QStyleOptionViewItem &option,
                                                        const QModelIndex &index) const
{
    QStyledItemDelegate::paint(painter, option, index);
}

void RibbonGalleryGroupItemDelegate::paintIconWithTextWordWrap(QPainter *painter, const QStyleOptionViewItem &option,
                                                                const QModelIndex &index) const
{
    QStyledItemDelegate::paint(painter, option, index);
}

QSize RibbonGalleryGroupItemDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    Q_UNUSED(index);
    Q_UNUSED(option);
    return m_group->gridSize();
}

/* RibbonGalleryGroupModel */
RibbonGalleryGroupModel::RibbonGalleryGroupModel(QObject *parent)
    : QAbstractListModel(parent)
{
}

RibbonGalleryGroupModel::~RibbonGalleryGroupModel()
{
    clear();
}

int RibbonGalleryGroupModel::rowCount(const QModelIndex &parent) const
{
    return (parent.isValid() ? 0 : m_items.count());
}

Qt::ItemFlags RibbonGalleryGroupModel::flags(const QModelIndex &index) const
{
    if (!index.isValid() || (index.row() >= m_items.count())) {
        return Qt::NoItemFlags;
    }
    return m_items.at(index.row())->flags();
}

QVariant RibbonGalleryGroupModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || (index.row() >= m_items.count())) {
        return QVariant();
    }
    return m_items.at(index.row())->data(role);
}

QModelIndex RibbonGalleryGroupModel::index(int row, int column, const QModelIndex &parent) const
{
    if (hasIndex(row, column, parent)) {
        return createIndex(row, column, m_items.at(row));
    }
    return QModelIndex();
}

bool RibbonGalleryGroupModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (!index.isValid() || (index.row() >= m_items.count())) {
        return false;
    }

    m_items.at(index.row())->setData(role, value);
    return true;
}

void RibbonGalleryGroupModel::clear()
{
    beginResetModel();
    qDeleteAll(m_items);
    m_items.clear();
    endResetModel();
}

RibbonGalleryItem *RibbonGalleryGroupModel::at(int row) const
{
    return m_items.value(row);
}

void RibbonGalleryGroupModel::insert(int row, RibbonGalleryItem *item)
{
    beginInsertRows(QModelIndex(), row, row);
    m_items.insert(row, item);
    endInsertRows();
}

RibbonGalleryItem *RibbonGalleryGroupModel::take(int row)
{
    if ((row < 0) || (row >= m_items.count())) {
        return Q_NULLPTR;
    }

    beginRemoveRows(QModelIndex(), row, row);
    RibbonGalleryItem *item = m_items.takeAt(row);
    endRemoveRows();
    return item;
}

void RibbonGalleryGroupModel::append(RibbonGalleryItem *item)
{
    beginInsertRows(QModelIndex(), m_items.count(), m_items.count() + 1);
    m_items.append(item);
    endInsertRows();
}

/* RibbonGalleryGroupPrivate */
class RibbonGalleryGroupPrivate
{
    QX_DECLARE_PUBLIC(RibbonGalleryGroup)
public:
    RibbonGalleryGroupPrivate();
    void init();
    RibbonGalleryGroupModel *groupModel();
    void itemActivate(const QModelIndex &index, QAction::ActionEvent event);
    int displayRowCount() const;
public:
    QString m_groupTitle;
    RibbonGalleryGroup::GalleryGroupStyle m_preStyle;
    RibbonGalleryGroup::DisplayRow m_displayRow;
    int m_gridMinimumWidth;        ///< grid最小宽度
    int m_gridMaximumWidth;        ///< grid最大宽度
    QActionGroup *m_actionGroup;   ///< 所有GalleryGroup管理的actions都由这个actiongroup管理
    bool m_blockRecalc;
};

RibbonGalleryGroupPrivate::RibbonGalleryGroupPrivate()
    : m_preStyle(RibbonGalleryGroup::IconWithText)
    , m_displayRow(RibbonGalleryGroup::DisplayOneRow)
    , m_gridMinimumWidth(0)
    , m_gridMaximumWidth(0)
    , m_blockRecalc(false)
{
}

void RibbonGalleryGroupPrivate::init()
{
    Q_Q(RibbonGalleryGroup);
    m_actionGroup = new QActionGroup(q);
    QObject::connect(m_actionGroup, &QActionGroup::triggered, q, &RibbonGalleryGroup::triggered);
    QObject::connect(m_actionGroup, &QActionGroup::hovered, q, &RibbonGalleryGroup::hovered);
}

RibbonGalleryGroupModel *RibbonGalleryGroupPrivate::groupModel()
{
    Q_Q(const RibbonGalleryGroup);
    return qobject_cast<RibbonGalleryGroupModel *>(q->model());
}

void RibbonGalleryGroupPrivate::itemActivate(const QModelIndex &index, QAction::ActionEvent event)
{
    if (index.isValid()) {
        RibbonGalleryItem *item = static_cast<RibbonGalleryItem *>(index.internalPointer());
        if (item) {
            QAction *act = item->action();
            if (act) {
                act->activate(event);
            }
        }
    }
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

/* RibbonGalleryGroup */
RibbonGalleryGroup::RibbonGalleryGroup(QWidget *w)
    : QListView(w)
{
    QX_INIT_PRIVATE(RibbonGalleryGroup)
    Q_D(RibbonGalleryGroup);
    d->init();
    setViewMode(QListView::IconMode);
    setResizeMode(QListView::Adjust);
    setSelectionRectVisible(true);
    setUniformItemSizes(true);
    setSpacing(1);
    setItemDelegate(new RibbonGalleryGroupItemDelegate(this, this));
    connect(this, &QAbstractItemView::clicked, this, &RibbonGalleryGroup::onItemClicked);
    connect(this, &QAbstractItemView::entered, this, &RibbonGalleryGroup::onItemEntered);
    RibbonGalleryGroupModel *m = new RibbonGalleryGroupModel(this);
    setModel(m);
}

RibbonGalleryGroup::~RibbonGalleryGroup()
{
    QX_FINI_PRIVATE()
}

/**
 * @brief 是否禁止计算
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
 * @brief 重新计算grid和icon的尺寸
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
    // 首先通过DisplayRow计算GridSize
    Q_D(RibbonGalleryGroup);
    int dr = d->displayRowCount();
    int h = galleryHeight / dr;
    if (h <= 1) {
        h = galleryHeight;
    }
    int w = h;
    int mw = gridMinimumWidth();
    if (mw > 0 && w < mw) {
        w = mw;
    }
    mw = gridMaximumWidth();
    if (mw > 0 && w > mw) {
        w = mw;
    }
    setGridSize(QSize(w, h));
    // 再通过GalleryGroupStyle确定icon的尺寸
    // 这个是移动像素，qt在鼠标移动到图标上时会移动一下，给用户明确的动态，导致如果布局很满会超出显示范围，因此要在此基础上缩放一点
    const int shiftpix = 4;
    switch (galleryGroupStyle()) {
    case IconWithText: {
        int textHeight = fontMetrics().lineSpacing();
        int iconHeight = h - textHeight - 2 * spacing() - shiftpix;
        if (iconHeight > 0) {
            setIconSize(QSize(w - 2 * spacing() - shiftpix, iconHeight));
        } else {
            setIconSize(QSize(w - 2 * spacing() - shiftpix, h - 2 * spacing() - shiftpix));
        }
        break;
    }
    case IconWithWordWrapText: {
        int textHeight = fontMetrics().lineSpacing() * 2;
        int iconHeight = h - textHeight;
        if (iconHeight > 0) {
            setIconSize(QSize(w - 2 * spacing() - shiftpix, iconHeight - 2 * spacing() - shiftpix));
        } else {
            setIconSize(QSize(w - 2 * spacing() - shiftpix, h - 2 * spacing() - shiftpix));
        }
        break;
    }
    case IconOnly: {
        setIconSize(QSize(w - 2 * spacing() - shiftpix, h - 2 * spacing() - shiftpix));
        break;
    }
    default: {
        setIconSize(QSize(w - 2 * spacing() - shiftpix, h - 2 * spacing() - shiftpix));
        break;
    }
    }
#ifdef QX_RIBBON_DEBUG
    qDebug() << "RibbonGalleryGroup::recalcGridSize(" << galleryHeight << "): gridSize=" << gridSize()
             << " iconSize=" << iconSize();
#endif
}

///
/// \brief 设置默认的预设样式
/// \param style
///
void RibbonGalleryGroup::setGalleryGroupStyle(RibbonGalleryGroup::GalleryGroupStyle style)
{
    Q_D(RibbonGalleryGroup);
    d->m_preStyle = style;
    if (style == IconWithWordWrapText) {
        setWordWrap(true);
    } else {
        setWordWrap(false);
    }
    recalcGridSize();
}

RibbonGalleryGroup::GalleryGroupStyle RibbonGalleryGroup::galleryGroupStyle() const
{
    Q_D(const RibbonGalleryGroup);
    return d->m_preStyle;
}

void RibbonGalleryGroup::addItem(const QString &text, const QIcon &icon)
{
    Q_D(RibbonGalleryGroup);
    RibbonGalleryGroupModel *model = d->groupModel();
    if (Q_NULLPTR == model) {
        return;
    }
    model->append(new RibbonGalleryItem(text, icon));
}

void RibbonGalleryGroup::addItem(QAction *act)
{
    Q_D(RibbonGalleryGroup);
    RibbonGalleryGroupModel *model = d->groupModel();
    if (Q_NULLPTR == model) {
        return;
    }
    d->m_actionGroup->addAction(act);
    model->append(new RibbonGalleryItem(act));
}

void RibbonGalleryGroup::addItems(const QList<QAction *> &acts)
{
    Q_D(RibbonGalleryGroup);
    RibbonGalleryGroupModel *model = d->groupModel();
    if (Q_NULLPTR == model) {
        return;
    }
    for (QAction *a : acts) {
        d->m_actionGroup->addAction(a);
    }
    for (int i = 0; i < acts.size(); ++i) {
        model->append(new RibbonGalleryItem(acts[i]));
    }
}

void RibbonGalleryGroup::setGroupTitle(const QString &title)
{
    Q_D(RibbonGalleryGroup);
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
    RibbonGalleryGroupModel *model = d->groupModel();
    if (Q_NULLPTR == model) {
        return;
    }
    QModelIndex ind = model->index(i, 0, QModelIndex());
    QItemSelectionModel *selmodel = selectionModel();
    if (selmodel) {
        selmodel->select(ind, QItemSelectionModel::SelectCurrent);
    }
}

void RibbonGalleryGroup::setDisplayRow(DisplayRow r)
{
    Q_D(RibbonGalleryGroup);
    d->m_displayRow = r;
    recalcGridSize();
}

RibbonGalleryGroup::DisplayRow RibbonGalleryGroup::displayRow() const
{
    Q_D(const RibbonGalleryGroup);
    return d->m_displayRow;
}

/**
 * @brief 设置grid最小的宽度，默认为0（不限制）
 * @param w
 */
void RibbonGalleryGroup::setGridMinimumWidth(int w)
{
    Q_D(RibbonGalleryGroup);
    d->m_gridMinimumWidth = w;
}

/**
 * @brief grid最小的宽度，默认为0（不限制）
 * @return
 */
int RibbonGalleryGroup::gridMinimumWidth() const
{
    Q_D(const RibbonGalleryGroup);
    return d->m_gridMinimumWidth;
}

/**
 * @brief 设置grid最大的宽度，默认为0（不限制）
 * @param w
 */
void RibbonGalleryGroup::setGridMaximumWidth(int w)
{
    Q_D(RibbonGalleryGroup);
    d->m_gridMaximumWidth = w;
}

/**
 * @brief grid最大的的宽度，默认为0（不限制）
 * @param w
 */
int RibbonGalleryGroup::gridMaximumWidth() const
{
    Q_D(const RibbonGalleryGroup);
    return d->m_gridMaximumWidth;
}

QSize RibbonGalleryGroup::sizeHint() const
{
    QSize sz = QListView::sizeHint();
    Q_D(const RibbonGalleryGroup);
    int dr = d->displayRowCount();
    return QSize(sz.width(), sz.height() / dr);
}

/**
 * @brief 获取RibbonGalleryGroup管理的actiongroup
 * @return
 */
QActionGroup *RibbonGalleryGroup::actionGroup() const
{
    Q_D(const RibbonGalleryGroup);
    return d->m_actionGroup;
}

void RibbonGalleryGroup::onItemClicked(const QModelIndex &index)
{
    Q_D(RibbonGalleryGroup);
    d->itemActivate(index, QAction::Trigger);
}

void RibbonGalleryGroup::onItemEntered(const QModelIndex &index)
{
    Q_D(RibbonGalleryGroup);
    d->itemActivate(index, QAction::Hover);
}

QX_RIBBON_END_NAMESPACE
