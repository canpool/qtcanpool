/***************************************************************************
 **
 **  Copyright (C) 2019 MaMinJie <canpool@163.com>
 **  Contact: https://github.com/canpool
 **  Reference: http://www.qtcn.org
 **
 **  This program is free software: you can redistribute it and/or modify
 **  it under the terms of the GNU General Public License as published by
 **  the Free Software Foundation, either version 3 of the License, or
 **  (at your option) any later version.
 **
 **  This program is distributed in the hope that it will be useful,
 **  but WITHOUT ANY WARRANTY; without even the implied warranty of
 **  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 **  GNU General Public License for more details.
 **
 **  You should have received a copy of the GNU General Public License
 **  along with this program.  If not, see http://www.gnu.org/licenses/.
 **
****************************************************************************/
#include "QtNavListView.h"

#include <QPainter>
#include <QFile>
#include <QDomDocument>

QtNavListView::QtNavListView(QWidget *parent)
    : QListView(parent),
      m_infoVisible(true),
      m_lineVisible(true),
      m_iconBgcVisible(true),
      m_style(Cross),
      m_lineColor(QColor(214, 216, 224)),
      m_normalBackColor(QColor(239, 241, 250)),
      m_selectedBackColor(QColor(133, 153, 216)),
      m_hoverBackColor(QColor(209, 216, 240)),
      m_normalTextColor(QColor(58, 58, 58)),
      m_selectedTextColor(QColor(255, 255, 255)),
      m_hoverTextColor(QColor(59, 59, 59)),
      m_pModel(new QtNavListModel(this)),
      m_pDelegate(new QtNavListDelegate(this))
{
    setMouseTracking(true);

    connect(this, SIGNAL(doubleClicked(QModelIndex)),
            m_pModel, SLOT(collapse(QModelIndex)));
}

QtNavListView::~QtNavListView()
{
    delete m_pModel;
    delete m_pDelegate;
}

void QtNavListView::readData(const QString &xmlFileName)
{
    m_pModel->readData(xmlFileName);
    setModel(m_pModel);
    setItemDelegate(m_pDelegate);
}

void QtNavListView::setData(const QStringList &listItem)
{
    m_pModel->setItems(listItem);
    setModel(m_pModel);
    setItemDelegate(m_pDelegate);
}

void QtNavListView::setInfoVisible(bool visible)
{
    m_infoVisible = visible;
}

void QtNavListView::setLineVisible(bool visible)
{
    m_lineVisible = visible;
}

void QtNavListView::setIconBgcVisible(bool visible)
{
    m_iconBgcVisible = visible;
}

void QtNavListView::setIconStyle(QtNavListView::IconStyle style)
{
    m_style = style;
}

void QtNavListView::setLineColor(const QColor &color)
{
    m_lineColor = color;
}

void QtNavListView::setBackColor(const QColor &normalColor,
                                 const QColor &selectedColor,
                                 const QColor &hoverColor)
{
    m_normalBackColor = normalColor;
    m_selectedBackColor = selectedColor;
    m_hoverBackColor = hoverColor;
}

void QtNavListView::setTextColor(const QColor &normalColor,
                                 const QColor &selectedColor,
                                 const QColor &hoverColor)
{
    m_normalTextColor = normalColor;
    m_selectedTextColor = selectedColor;
    m_hoverTextColor = hoverColor;
}

QtNavListModel::QtNavListModel(QObject *parent)
    : QAbstractListModel(parent)
{

}

QtNavListModel::~QtNavListModel()
{
    for (QList<TreeNode *>::iterator it = m_treeNodes.begin(); it != m_treeNodes.end(); /* */) {
        for (QList<TreeNode *>::iterator child = (*it)->childrens.begin(); child != (*it)->childrens.end();) {
            delete(*child);
            child = (*it)->childrens.erase(child);
        }

        delete(*it);
        it = m_treeNodes.erase(it);
    }
}

void QtNavListModel::readData(const QString &xmlFileName)
{
    QFile xml(xmlFileName);

    if (!xml.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return;
    }

    QDomDocument doc;
    if (!doc.setContent(&xml, false)) {
        return;
    }

    m_treeNodes.clear();
    m_listNodes.clear();

    QDomNode root = doc.documentElement().firstChildElement("layout");
    QDomNodeList children = root.childNodes();

    for (int i = 0; i != children.count(); ++i) {
        QDomElement nodeInfo = children.at(i).toElement();
        TreeNode *node = new TreeNode;
        node->label = nodeInfo.attribute("label");
        node->collapse = nodeInfo.attribute("collapse").toInt();
        node->info = nodeInfo.attribute("info");
        node->level = 1;

        QDomNodeList secondLevel = nodeInfo.childNodes();

        for (int j = 0; j != secondLevel.count(); ++j) {
            QDomElement secNodeInfo = secondLevel.at(j).toElement();
            TreeNode *secNode = new TreeNode;
            secNode->label = secNodeInfo.attribute("label");
            secNode->info = secNodeInfo.attribute("info");
            secNode->collapse = false;
            secNode->level = 2;
            secNode->theLast = (j == secondLevel.count() - 1 && i != children.count() - 1);
            node->childrens.push_back(secNode);
        }

        m_treeNodes.push_back(node);
    }

    refreshList();
    beginResetModel();
    endResetModel();
}

void QtNavListModel::setItems(const QStringList &listItem)
{
    int count = listItem.count();
    if (count == 0) {
        return;
    }

    m_treeNodes.clear();
    m_listNodes.clear();

    // datas format: title | fatherTitle | isCollapse | info
    for (int i = 0; i < count; ++i) {
        QString item = listItem.at(i);
        QStringList list = item.split("|");
        if (list.count() < 4) {
            continue;
        }

        QString title = list.at(0);
        QString fatherTitle = list.at(1);
        QString collapse = list.at(2);
        QString info = list.at(3);

        // fatherTitle need empty
        if (fatherTitle.isEmpty()) {
            TreeNode *node = new TreeNode;
            node->label = title;
            node->collapse = collapse.toInt();
            node->info = info;
            node->level = 1;

            for (int j = 0; j < count; ++j) {
                QString secItem = listItem.at(j);
                QStringList secList = secItem.split("|");
                if (secList.count() < 4) {
                    continue;
                }

                QString secTitle = secList.at(0);
                QString secFatherTitle = secList.at(1);
                QString secInfo = secList.at(3);

                if (secFatherTitle == title) {
                    TreeNode *secNode = new TreeNode;
                    secNode->label = secTitle;
                    secNode->info = secInfo;
                    secNode->collapse = false;
                    secNode->level = 2;
                    secNode->theLast = (j == count - 1);
                    node->childrens.push_back(secNode);
                }
            }

            m_treeNodes.push_back(node);
        }
    }

    refreshList();
    beginResetModel();
    endResetModel();
}

void QtNavListModel::collapse(const QModelIndex &index)
{
    TreeNode *node = m_listNodes[index.row()].treeNode;

    int count = node->childrens.size();
    if (count == 0) {
        return;
    }

    node->collapse = !node->collapse;

    if (!node->collapse) {
        beginInsertRows(QModelIndex(), index.row() + 1, index.row() + count);
        endInsertRows();
    } else {
        beginRemoveRows(QModelIndex(), index.row() + 1, index.row() + count);
        endRemoveRows();
    }
    refreshList();
}

int QtNavListModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return static_cast<int>(m_listNodes.size());
}

QVariant QtNavListModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid()) {
        return QVariant();
    }

    auto row = index.row();

    if (row >= m_listNodes.size() || index.row() < 0) {
        return QVariant();
    }

    if (role == Qt::DisplayRole) {
        return m_listNodes[row].label;
    } else if (role == Qt::UserRole) {
        return QVariant::fromValue(m_listNodes[row].treeNode);
    }

    return QVariant();
}

void QtNavListModel::refreshList()
{
    m_listNodes.clear();

    for (QList<TreeNode *>::iterator it = m_treeNodes.begin(); it != m_treeNodes.end(); ++it) {
        ListNode node;
        node.label = (*it)->label;
        node.treeNode = *it;

        m_listNodes.push_back(node);

        if ((*it)->collapse) {
            continue;
        }

        for (QList<TreeNode *>::iterator child = (*it)->childrens.begin();
             child != (*it)->childrens.end(); ++child) {
            ListNode node;
            node.label = (*child)->label;
            node.treeNode = *child;
            node.treeNode->theLast = false;
            m_listNodes.push_back(node);
        }

        if (!m_listNodes.empty()) {
            m_listNodes.back().treeNode->theLast = true;
        }
    }
}

QtNavListDelegate::QtNavListDelegate(QObject *parent)
    : QStyledItemDelegate(parent),
      view(qobject_cast<QtNavListView *>(parent))
{

}

QtNavListDelegate::~QtNavListDelegate()
{

}

QSize QtNavListDelegate::sizeHint(const QStyleOptionViewItem &option,
                                  const QModelIndex &index) const
{
    Q_UNUSED(option);
    QtNavListModel::TreeNode *node = index.data(Qt::UserRole).value<QtNavListModel::TreeNode *>();

    if (node->level == 1) {
        return QSize(50, 35);
    } else {
        return QSize(50, 28);
    }
}

void QtNavListDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option,
                              const QModelIndex &index) const
{
    painter->setRenderHint(QPainter::Antialiasing);
    QtNavListModel::TreeNode *node =
            index.data(Qt::UserRole).value<QtNavListModel::TreeNode *>();

    // draw background
    QColor backColor;
    if (option.state & QStyle::State_Selected) {
        backColor = view->selectedBackColor();
    } else if (option.state & QStyle::State_MouseOver) {
        backColor = view->hoverBackColor();
    } else {
        backColor = view->normalBackColor();
    }
    painter->fillRect(option.rect, backColor);

    if (node->childrens.size() > 0) {
        drawBranchImage(painter, option, node);
    }

    // draw text
    QColor textColor;
    if (option.state & QStyle::State_Selected) {
        textColor = view->selectedTextColor();
    } else if (option.state & QStyle::State_MouseOver) {
        textColor = view->hoverTextColor();
    } else {
        textColor = view->normalTextColor();
    }
    painter->setPen(QPen(textColor));

    int margin = 25;
    if (node->level == 2) {
        margin = 35;
    }
    QRect rect = option.rect;
    rect.setWidth(rect.width() - margin);
    rect.setX(rect.x() + margin);

    QFont normalFont("Microsoft Yahei", 9);
    painter->setFont(normalFont);
    painter->drawText(rect, Qt::AlignLeft | Qt::AlignVCenter,
                      index.data(Qt::DisplayRole).toString());

    // draw line
    if (view->lineVisible()) {
        painter->setPen(QPen(view->lineColor(), 1));
        const QRect &rect = option.rect;
        if (node->level == 1 || (node->level == 2 && node->theLast)) {
            painter->drawLine(QPointF(rect.x(), rect.y() + rect.height()),
                              QPointF(rect.x() + rect.width(), rect.y() + rect.height()));
        }
    }

    drawTip(painter, option, node);
}

void QtNavListDelegate::drawBranchImage(QPainter *painter, const QStyleOptionViewItem &option,
                                        QtNavListModel::TreeNode *node) const
{
    QPixmap pix(18, 16);
    pix.fill(Qt::transparent);
    QPainter p(&pix);
    p.setRenderHint(QPainter::Antialiasing);
    int penWidth = 2;

    QColor iconSelectedColor, iconNormalColor, iconHoverColor;

    if (view->iconBgcVisible()) {
        iconSelectedColor = view->normalBackColor();
        iconNormalColor = view->selectedBackColor();
        iconHoverColor = view->normalBackColor();
    } else {
        iconSelectedColor = view->selectedTextColor();
        iconNormalColor = view->normalTextColor();
        iconHoverColor = view->hoverTextColor();
    }

    if (view->iconStyle() == QtNavListView::Cross) {
        p.setBrush(Qt::NoBrush);

        if (option.state & QStyle::State_Selected) {
            p.setPen(QPen(iconSelectedColor, penWidth));
        } else if (option.state & QStyle::State_MouseOver) {
            p.setPen(QPen(iconHoverColor, penWidth));
        } else {
            p.setPen(QPen(iconNormalColor, penWidth));
        }

        if (node->collapse) {
            p.drawLine(QPointF(8, 8), QPointF(18, 8));
            p.drawLine(QPointF(12, 4), QPointF(12, 12));
        } else {
            p.drawLine(QPointF(8, 8), QPointF(18, 8));
        }
    } else if (view->iconStyle() == QtNavListView::Triangle) {
        p.setPen(Qt::NoPen);

        if (option.state & QStyle::State_Selected) {
            p.setBrush(iconSelectedColor);
        } else if (option.state & QStyle::State_MouseOver) {
            p.setBrush(iconHoverColor);
        } else {
            p.setBrush(iconNormalColor);
        }

        QVector<QPointF> points;
        if (node->collapse) {
            points.append(QPointF(6, 4));
            points.append(QPointF(6, 12));
            points.append(QPointF(16, 8));
        } else {
            points.append(QPointF(6, 4));
            points.append(QPointF(18, 4));
            points.append(QPointF(12, 10));
        }
        p.drawPolygon(points);
    }

    QPixmap img(pix);
    QRect targetRect = option.rect;
    targetRect.setWidth(16);
    targetRect.setHeight(16);
    QPoint c = option.rect.center();
    c.setX(8);
    targetRect.moveCenter(c);
    painter->drawPixmap(targetRect, img, img.rect());
}

void QtNavListDelegate::drawTip(QPainter *painter, const QStyleOptionViewItem &option,
                                QtNavListModel::TreeNode *node) const
{
    QString recordInfo = node->info;
    if (!view->infoVisible() || recordInfo.isEmpty()) {
        return;
    }
    QFont decorationFont("Microsoft Yahei", 8);
    painter->setFont(decorationFont);

    // set background
    QRect decoration = option.rect;
    decoration.setHeight(15);
    decoration.moveCenter(option.rect.center());
    decoration.setLeft(option.rect.right() - 45);
    decoration.setRight(option.rect.right() - 5);
    QPainterPath path;
    path.addRoundedRect(decoration, 7, 7);

    QPen decorationPen((option.state & QStyle::State_Selected) ?
                           view->selectedBackColor() : view->selectedTextColor());
    QBrush decorationBrush((option.state & QStyle::State_Selected) ?
                               view->selectedTextColor() : view->selectedBackColor());
    painter->setPen(decorationPen);
    painter->fillPath(path, decorationBrush);

    // correction
    if (recordInfo.toInt() > 999) {
        recordInfo = "999+";
    }
    if (recordInfo.length() > 4) {
        recordInfo = recordInfo.mid(0, 4) + "..";
    }

    painter->drawText(decoration, Qt::AlignCenter, recordInfo);
}
