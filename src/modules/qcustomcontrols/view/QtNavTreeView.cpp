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
#include "QtNavTreeView.h"

#include <QStandardItemModel>
#include <QPainter>

QtNavTreeView::QtNavTreeView(QWidget *parent)
    : QTreeView(parent),
    m_model(new QStandardItemModel(this)),
    m_bgColor(QColor("#31495A")),
    m_hoverBgColor(QColor("#77D8C4")),
    m_checkedBgColor(QColor("#18BD9B")),
    m_textColor(QColor("#F0F0F0")),
    m_hoverTextColor(QColor("#000000")),
    m_checkedTextColor(QColor("#F0F0F0")),
    m_lineColor(QColor("#404244"))
{
    this->setModel(m_model);

    this->setHeaderHidden(true);
    this->setEditTriggers(QAbstractItemView::NoEditTriggers);
    this->setRootIsDecorated(false);
    this->setStyleSheet("QTreeView::branch {image:none;}");
    this->setItemDelegate(new QtNavTreeDelegate(this, this));
}

void QtNavTreeView::addItem(const QString &text, QStandardItem *parent)
{
    if (parent == nullptr) {
        m_model->appendRow(new QStandardItem(text));
    } else {
        parent->appendRow(new QStandardItem(text));
    }
}

void QtNavTreeView::addItem(const QString &parentText, const QString &text)
{
    QList<QStandardItem *> ls = m_model->findItems(parentText, Qt::MatchRecursive);

    if (!ls.isEmpty()) {
        foreach (QStandardItem *item, ls) {
            addItem(text, item);
        }
    }
}

void QtNavTreeView::setItemInfo(const QString &itemText, const QString &info)
{
    if (m_infoMap.contains(itemText)) {
        m_infoMap.remove(itemText);
        m_infoMap.insert(itemText, info);
        return;
    }

    m_infoMap.insert(itemText, info);
}

QString QtNavTreeView::infoStr(const QModelIndex &index)
{
    QString itemText = m_model->itemFromIndex(index)->data(Qt::DisplayRole).toString();

    if (m_infoMap.contains(itemText)) {
        return m_infoMap.value(itemText);
    }

    return QString();
}

QtNavTreeDelegate::QtNavTreeDelegate(QtNavTreeView *view, QObject *parent)
    : QStyledItemDelegate(parent),
    m_view(view),
    m_model(static_cast<QStandardItemModel *>(view->model()))
{
}

QtNavTreeDelegate::~QtNavTreeDelegate()
{

}

QSize QtNavTreeDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    Q_UNUSED(option);
    Q_UNUSED(index);
    return QSize(50, 30);
}

void QtNavTreeDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option,
                              const QModelIndex &index) const
{
    painter->setRenderHint(QPainter::Antialiasing);

    QColor colorBg;
    if (option.state & QStyle::State_Selected) {
        colorBg = m_view->checkedBgColor();
    } else if (option.state & QStyle::State_MouseOver) {
        colorBg = m_view->hoverBgColor();
    } else {
        colorBg = m_view->bgColor();
    }
    QRect bgRect(m_view->x() - 10, option.rect.top(), m_view->width(), option.rect.height());
    painter->fillRect(bgRect, colorBg);

    if (m_model->itemFromIndex(index)->hasChildren()) {
        QPixmap pix(18, 16);
        pix.fill(Qt::transparent);
        QPainter p(&pix);
        p.setRenderHint(QPainter::Antialiasing);
        int penWidth = 2;

        QColor icoColorSelected;
        QColor icoColorNormal;
        QColor icoColorHover;

        icoColorSelected = m_view->checkedTextColor();
        icoColorNormal = m_view->textColor();
        icoColorHover = m_view->hoverTextColor();

        p.setBrush(Qt::NoBrush);

        if (option.state & QStyle::State_Selected) {
            p.setPen(QPen(icoColorSelected, penWidth));
        } else if (option.state & QStyle::State_MouseOver) {
            p.setPen(QPen(icoColorHover, penWidth));
        } else {
            p.setPen(QPen(icoColorNormal, penWidth));
        }

        if (!m_view->isExpanded(index)) {
            p.drawLine(QPointF(8, 8), QPointF(18, 8));
            p.drawLine(QPointF(12, 4), QPointF(12, 12));
        } else {
            p.drawLine(QPointF(8, 8), QPointF(18, 8));
        }

        QPixmap img(pix);
        QRect targetRect = option.rect;
        targetRect.setWidth(16);
        targetRect.setHeight(16);
        QPoint c = option.rect.center();
        c.setX(8 + option.rect.x());
        targetRect.moveCenter(c);
        painter->drawPixmap(targetRect, img, img.rect());
    }

    QColor colorText;
    if (option.state & QStyle::State_Selected) {
        colorText = m_view->checkedTextColor();
    } else if (option.state & QStyle::State_MouseOver) {
        colorText = m_view->hoverTextColor();
    } else {
        colorText = m_view->textColor();
    }

    painter->setPen(QPen(colorText));

    int margin = 25;
    QRect rect = option.rect;
    rect.setWidth(rect.width() - margin);
    rect.setX(rect.x() + margin);

    QFont normalFont("Microsoft Yahei", 9);
    painter->setFont(normalFont);
    painter->drawText(rect, Qt::AlignLeft | Qt::AlignVCenter, index.data(Qt::DisplayRole).toString());

    // draw tips
    QString recordInfo = m_view->infoStr(index);
    if (recordInfo.isNull() || recordInfo.isEmpty() || !recordInfo.length()) {
        return;
    }

    QPen decorationPen(option.state & QStyle::State_Selected ?
                           m_view->checkedBgColor() : m_view->checkedTextColor());
    QBrush decorationBrush(option.state & QStyle::State_Selected ?
                               m_view->checkedTextColor() : m_view->checkedBgColor());
    QFont decorationFont("Microsoft Yahei", 8);
    painter->setFont(decorationFont);

    QRect decoration = option.rect;
    decoration.setHeight(15);
    decoration.moveCenter(option.rect.center());
    decoration.setLeft(option.rect.right() - 45);
    decoration.setRight(option.rect.right() - 5);

    painter->setPen(decorationPen);
    QPainterPath path;
    path.addRoundedRect(decoration, 7, 7);
    painter->fillPath(path, decorationBrush);

    if (recordInfo.toInt() > 999) {
        recordInfo = "999+";
    }
    if (recordInfo.length() > 4) {
        recordInfo = recordInfo.mid(0, 4) + "..";
    }

    painter->drawText(decoration, Qt::AlignCenter, recordInfo);
}
