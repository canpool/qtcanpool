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
#ifndef QTNAVLISTVIEW_H
#define QTNAVLISTVIEW_H

#include <QStyledItemDelegate>
#include <QAbstractListModel>
#include <QListView>
#include <QStringList>

class QtNavListModel;
class QtNavListDelegate;

class QtNavListView : public QListView
{
    Q_OBJECT
public:
    enum IconStyle { Cross, Triangle };

    explicit QtNavListView(QWidget *parent);
    ~QtNavListView();

public:
    // gets
    bool infoVisible() const { return m_infoVisible; }
    bool lineVisible() const { return m_lineVisible; }
    bool iconBgcVisible() const { return m_iconBgcVisible; }

    IconStyle iconStyle() const { return m_style; }

    QColor lineColor() const { return m_lineColor; }

    QColor normalBackColor() const { return m_normalBackColor; }
    QColor selectedBackColor() const { return m_selectedBackColor; }
    QColor hoverBackColor() const { return m_hoverBackColor; }

    QColor normalTextColor() const { return m_normalTextColor; }
    QColor selectedTextColor() const { return m_selectedTextColor; }
    QColor hoverTextColor() const { return m_hoverTextColor; }

public slots:
    // sets
    void readData(const QString &xmlFileName);
    void setData(const QStringList &listItem);

    void setInfoVisible(bool visible);
    void setLineVisible(bool visible);
    void setIconBgcVisible(bool visible); // Backgroud-Color

    void setIconStyle(IconStyle style);

    void setLineColor(const QColor &color);
    void setBackColor(const QColor &normalColor, const QColor &selectedColor, const QColor &hoverColor);
    void setTextColor(const QColor &normalColor, const QColor &selectedColor, const QColor &hoverColor);

private:
    bool m_infoVisible;
    bool m_lineVisible;
    bool m_iconBgcVisible;
    IconStyle m_style;

    QColor m_lineColor;

    QColor m_normalBackColor;
    QColor m_selectedBackColor;
    QColor m_hoverBackColor;

    QColor m_normalTextColor;
    QColor m_selectedTextColor;
    QColor m_hoverTextColor;

    QtNavListModel *m_pModel;
    QtNavListDelegate *m_pDelegate;
};

class QtNavListModel : public QAbstractListModel
{
    Q_OBJECT
public:
    struct TreeNode {
        TreeNode() : label(QString()), level(1),
            collapse(false), theLast(false), info(QString()) {}

        QString label;
        int level;
        bool collapse;
        bool theLast;
        QString info;
        QList<TreeNode *> childrens;
    };

    struct ListNode {
        ListNode() : label(QString()), treeNode(nullptr) {}

        QString label;
        TreeNode *treeNode;
    };

    explicit QtNavListModel(QObject *parent);
    ~QtNavListModel();

public slots:
    void readData(const QString &xmlFileName);
    void setItems(const QStringList &listItem);
    void collapse(const QModelIndex &index);

protected:
    int rowCount(const QModelIndex &parent) const;
    QVariant data(const QModelIndex &index, int role) const;

private:
    void refreshList();

private:
    QList<TreeNode *> m_treeNodes;
    QList<ListNode> m_listNodes;
};

Q_DECLARE_METATYPE(QtNavListModel::TreeNode *)

class QtNavListDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    explicit QtNavListDelegate(QObject *parent);
    ~QtNavListDelegate();

protected:
    QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const;
    void paint(QPainter *painter, const QStyleOptionViewItem &option,
               const QModelIndex &index) const;

private:
    void drawBranchImage(QPainter *painter, const QStyleOptionViewItem &option,
                         QtNavListModel::TreeNode *node) const;
    void drawTip(QPainter *painter, const QStyleOptionViewItem &option,
                 QtNavListModel::TreeNode *node) const;

private:
    QtNavListView *view;
};

#endif // QTNAVLISTVIEW_H
