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
#ifndef QTNAVTREEVIEW_H
#define QTNAVTREEVIEW_H

#include <QTreeView>
#include <QStyledItemDelegate>

class QStandardItemModel;
class QStandardItem;

class QtNavTreeView : public QTreeView
{
    Q_OBJECT
public:
    QtNavTreeView(QWidget *parent);

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

class QtNavTreeDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    explicit QtNavTreeDelegate(QtNavTreeView *view, QObject *parent);
    ~QtNavTreeDelegate();

protected:
    QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const;
    void paint(QPainter *painter, const QStyleOptionViewItem &option,
               const QModelIndex &index) const;

private:
    QtNavTreeView *m_view;
    QStandardItemModel *m_model;
};

#endif // QTNAVTREEVIEW_H
