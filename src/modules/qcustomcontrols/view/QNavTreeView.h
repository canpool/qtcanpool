/**
 * Copyleft (C) 2023 maminjie <canpool@163.com>
 **/
#ifndef QNAVTREEVIEW_H
#define QNAVTREEVIEW_H

#include <QStyledItemDelegate>
#include <QTreeView>

class QStandardItemModel;
class QStandardItem;

class QNavTreeView : public QTreeView
{
    Q_OBJECT
public:
    QNavTreeView(QWidget *parent);

    void addItem(const QString &text, QStandardItem *parent = nullptr);
    void addItem(const QString &parentText, const QString &text);
    void setItemInfo(const QString &itemText, const QString &info);

    QColor bgColor() const { return m_bgColor; }
    void setBgColor(const QColor &color) { m_bgColor = color; }

    QColor hoverBgColor() const { return m_hoverBgColor; }
    void setHoverBgColor(const QColor &color) { m_hoverBgColor = color; }

    QColor checkedBgColor() const { return m_checkedBgColor; }
    void setCheckedBgColor(const QColor &color) { m_checkedBgColor = color; }

    QColor textColor() const { return m_textColor; }
    void setTextColor(const QColor &color) { m_textColor = color; }

    QColor hoverTextColor() const { return m_hoverTextColor; }
    void setHoverTextColor(const QColor &color) { m_hoverTextColor = color; }

    QColor checkedTextColor() const { return m_checkedTextColor; }
    void setCheckedTextColor(const QColor &color) { m_checkedTextColor = color; }

    QString infoStr(const QModelIndex &index);
    QColor lineColor() const { return m_lineColor; }
    void setLineColor(const QColor &color) { m_lineColor = color; }
private:
    QStandardItemModel *m_model;
    QColor m_bgColor;
    QColor m_hoverBgColor;
    QColor m_checkedBgColor;
    QColor m_textColor;
    QColor m_hoverTextColor;
    QColor m_checkedTextColor;
    QColor m_lineColor;

    QMap<QString, QString> m_infoMap;
};

class QNavTreeDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    explicit QNavTreeDelegate(QNavTreeView *view, QObject *parent);
    ~QNavTreeDelegate();
protected:
    QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const;
    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;
private:
    QNavTreeView *m_view;
    QStandardItemModel *m_model;
};

#endif   // QTNAVTREEVIEW_H
