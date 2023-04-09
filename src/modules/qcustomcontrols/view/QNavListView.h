/**
 * Copyleft (C) 2023 maminjie <canpool@163.com>
 **/
#ifndef QNAVLISTVIEW_H
#define QNAVLISTVIEW_H

#include <QAbstractListModel>
#include <QListView>
#include <QStringList>
#include <QStyledItemDelegate>

class QNavListModel;
class QNavListDelegate;

class QNavListView : public QListView
{
    Q_OBJECT
public:
    enum IconStyle {
        Cross,
        Triangle
    };

    explicit QNavListView(QWidget *parent);
    ~QNavListView();
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
    void setIconBgcVisible(bool visible);   // Backgroud-Color

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

    QNavListModel *m_pModel;
    QNavListDelegate *m_pDelegate;
};

class QNavListModel : public QAbstractListModel
{
    Q_OBJECT
public:
    struct TreeNode {
        TreeNode() : label(QString()), level(1), collapse(false), theLast(false), info(QString()) {}

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

    explicit QNavListModel(QObject *parent);
    ~QNavListModel();

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

Q_DECLARE_METATYPE(QNavListModel::TreeNode *)

class QNavListDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    explicit QNavListDelegate(QObject *parent);
    ~QNavListDelegate();
protected:
    QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const;
    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;
private:
    void drawBranchImage(QPainter *painter, const QStyleOptionViewItem &option, QNavListModel::TreeNode *node) const;
    void drawTip(QPainter *painter, const QStyleOptionViewItem &option, QNavListModel::TreeNode *node) const;
private:
    QNavListView *view;
};

#endif   // QTNAVLISTVIEW_H
