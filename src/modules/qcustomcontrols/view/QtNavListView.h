/***************************************************************************
 **
 **  Copyright (C) 2019-2020 MaMinJie <canpool@163.com>
 **  Contact: https://github.com/canpool
 **           https://gitee.com/icanpool
 **  Reference: http://www.qtcn.org
 **
 **  GNU Lesser General Public License Usage
 **  Alternatively, this file may be used under the terms of the GNU Lesser
 **  General Public License version 3 as published by the Free Software
 **  Foundation and appearing in the file LICENSE.LGPL3 included in the
 **  packaging of this file. Please review the following information to
 **  ensure the GNU Lesser General Public License version 3 requirements
 **  will be met: https://www.gnu.org/licenses/lgpl-3.0.html.
 **
 **  GNU General Public License Usage
 **  Alternatively, this file may be used under the terms of the GNU
 **  General Public License version 2.0 or (at your option) the GNU General
 **  Public license version 3 or any later version approved by the KDE Free
 **  Qt Foundation. The licenses are as published by the Free Software
 **  Foundation and appearing in the file LICENSE.GPL2 and LICENSE.GPL3
 **  included in the packaging of this file. Please review the following
 **  information to ensure the GNU General Public License requirements will
 **  be met: https://www.gnu.org/licenses/gpl-2.0.html and
 **  https://www.gnu.org/licenses/gpl-3.0.html.
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
