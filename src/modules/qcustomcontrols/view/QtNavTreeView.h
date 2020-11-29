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
