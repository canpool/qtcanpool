/***************************************************************************
 **
 **  Copyright (C) 2018 MaMinJie <canpool@163.com>
 **  Contact: https://github.com/canpool
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
#include "simplefilter.h"

#include <QLineEdit>
#include <QTableWidget>
#include <QTreeWidget>
#include <QComboBox>
#include <QHBoxLayout>
#include <QTreeView>

enum Type{
    NoneType,
    TableType,
    TreeType,
    ViewType
};

class SimpleFilterPrivate : public QObject
{
    Q_OBJECT
public:
    explicit SimpleFilterPrivate(QObject *parent = 0);
    void init();

    /// tree
    void showTreeItem(QTreeWidgetItem *item);
    void parentExpand(QTreeWidgetItem *item);
    void search(QTreeWidgetItem *item, const QString &text);
    void searchChild(QTreeWidgetItem *item, const QString &text);

    /// table
    void showTableItems();
    void search(const QString &text);

    // view
    void showViewItem(const QModelIndex &index);
    void parentExpand(const QModelIndex &index);
    void search(const QModelIndex &index, const QString &text);
    void searchChild(const QModelIndex &index, const QString &text);

    bool contentMatch(const QString &source, const QString &text);

    /// member variables
    QTableWidget *m_table;
    QTreeWidget *m_tree;
    QTreeView *m_view;
    QAbstractItemModel *m_model;
    QLineEdit *m_lineEdit;

    SimpleFilter *q;
    int m_type;

public slots:
    void textChanged(QString text);
};

SimpleFilterPrivate::SimpleFilterPrivate(QObject *parent)
    : QObject(parent),m_table(0),m_tree(0),
      m_view(0),m_model(0),
      m_lineEdit(0),q(0),m_type(NoneType)
{

}

void SimpleFilterPrivate::init()
{
    m_lineEdit = new QLineEdit();
    m_lineEdit->setFixedHeight(22);
    m_lineEdit->setPlaceholderText(tr("Filter"));
    m_lineEdit->setClearButtonEnabled(true);
    connect(m_lineEdit, SIGNAL(textChanged(QString)), this, SLOT(textChanged(QString)));

    QHBoxLayout *layout = new QHBoxLayout();
    layout->setMargin(0);
    layout->setSpacing(0);
    layout->addWidget(m_lineEdit);
    q->setLayout(layout);
}

void SimpleFilterPrivate::showTreeItem(QTreeWidgetItem *item)
{
    for(int i = 0; i < item->childCount(); i++){
        QTreeWidgetItem *childItem = item->child(i);
        childItem->setHidden(false);
//        childItem->setTextColor(0,Qt::black);
        showTreeItem(childItem);
    }
}

void SimpleFilterPrivate::parentExpand(QTreeWidgetItem *item)
{
    if(item->parent() != NULL){
        QTreeWidgetItem *parentItem = item->parent();
        if(parentItem->isHidden()){
            parentItem->setHidden(false);
        }
        if(!parentItem->isExpanded()){
            parentItem->setExpanded(true);
        }
        parentExpand(parentItem);
    }
}

void SimpleFilterPrivate::search(QTreeWidgetItem *item, const QString &text)
{
    for(int i = 0; i < item->childCount(); ++i) {
        QTreeWidgetItem *childItem = item->child(i);
        if (contentMatch(childItem->text(0), text)) {
            childItem->setExpanded(true);
            parentExpand(childItem);
//            childItem->setTextColor(0,QColor(200,0,100));
            childItem->setHidden(false);
            searchChild(childItem, text);
        }else{
//            childItem->setBackground(0, m_itemBrush);
//            childItem->setTextColor(0,Qt::black);
            childItem->setHidden(true);
            search(childItem, text);
        }
    }
}

void SimpleFilterPrivate::searchChild(QTreeWidgetItem *item, const QString &text)
{
    for(int i = 0; i < item->childCount(); ++i) {
        QTreeWidgetItem *childItem = item->child(i);
        childItem->setHidden(false);
//        childItem->setExpanded(true);
        if (contentMatch(childItem->text(0), text)) {
//            childItem->setExpanded(true);
            parentExpand(childItem);
//            childItem->setTextColor(0,QColor(200,0,100));
        }else{
//            childItem->setExpanded(false);
//            childItem->setTextColor(0,Qt::black);
//            childItem->setHidden(true); // 20171217
        }
        searchChild(childItem, text);
    }
}

void SimpleFilterPrivate::showTableItems()
{
    if(m_table == 0) return;
    for(int i = 0; i < m_table->rowCount(); i++){
        m_table->showRow(i);
    }
}

void SimpleFilterPrivate::search(const QString &text)
{
    if(m_table == 0) return;
    for(int i = 0; i < m_table->rowCount(); i++){
        int j;
        for(j = 0; j < m_table->columnCount(); j++){
            QTableWidgetItem *item = m_table->item(i, j);
            if(contentMatch(item->text(),text)){
                m_table->showRow(i);
                break;
            }
        }
        if(j == m_table->columnCount()){
            m_table->hideRow(i);
        }
    }
}

void SimpleFilterPrivate::showViewItem(const QModelIndex &index)
{
    for(int i = 0; i < m_model->rowCount(index); i++){
        m_view->setRowHidden(i, index, false);
        QModelIndex child = m_model->index(i, 0, index);
        showViewItem(child);
    }
}

void SimpleFilterPrivate::parentExpand(const QModelIndex &index)
{
    QModelIndex parent = index.parent();
    if(!parent.isValid()) return;
    m_view->expand(parent);
    QModelIndex root = parent.parent();
//    if(!root.isValid()) return; // no need
    if(m_view->isRowHidden(parent.row(), root)){
        m_view->setRowHidden(parent.row(), root, false);
    }
    if(!m_view->isExpanded(parent)){
        m_view->expand(parent);
    }
    parentExpand(parent);
}

void SimpleFilterPrivate::search(const QModelIndex &index, const QString &text)
{
    for(int i = 0; i < m_model->rowCount(index); i++){
        QModelIndex child = m_model->index(i, 0, index);
        if(contentMatch(child.data().toString(), text)){
            m_view->expand(child);
            parentExpand(child);
            m_view->setRowHidden(child.row(), index, false);
            searchChild(child, text);
        }else{
            m_view->setRowHidden(child.row(), index, true);
            search(child, text);
        }
    }
}

void SimpleFilterPrivate::searchChild(const QModelIndex &index, const QString &text)
{
    for(int i = 0; i < m_model->rowCount(index); i++){
        QModelIndex child = m_model->index(i, 0, index);
        m_view->setRowHidden(child.row(), index, false);
        if(contentMatch(child.data().toString(), text)){
//            m_view->expand(child);
            parentExpand(child);
        }else{
//            m_view->setRowHidden(child.row(), index, true);
        }
        searchChild(child, text);
    }
}

bool SimpleFilterPrivate::contentMatch(const QString &source, const QString &text)
{
    // simple
//    if(-1 != source.indexOf(text,0,Qt::CaseInsensitive)){
//        return true;
//    }
//    return false;

    // complex
    QStringList keys = text.split("*");
    QString value = source;
    int count = 0;
    int index;
    for(int i = 0; i < keys.count(); i++){
        index = value.indexOf(keys.at(i), 0, Qt::CaseInsensitive);
        if(index != -1){
            count++;
            value = value.mid(index+keys.at(i).count());
        }
    }
    if(count == keys.count()){
        return true;
    }
    return false;
}

void SimpleFilterPrivate::textChanged(QString text)
{
    if(m_table){
        showTableItems();
        if(!text.isEmpty()){
            search(text);
        }
    }else if(m_tree){
        showTreeItem(m_tree->invisibleRootItem());
//        m_tree->collapseAll();
        if(!text.isEmpty()){
            search(m_tree->invisibleRootItem(), text);
        }
    }else if(m_view){
        showViewItem(m_view->rootIndex());
        m_view->collapseAll();
        if(!text.isEmpty()){
            search(m_view->rootIndex(), text);
        }
    }
}

SimpleFilter::SimpleFilter(QTableWidget *table, QWidget *parent)
    : QWidget(parent),d(new SimpleFilterPrivate(this))
{
    d->m_table = table;
    if(table){
        d->m_type = TableType;
    }
    d->q = this;
    d->init();
}

SimpleFilter::SimpleFilter(QTreeWidget *tree, QWidget *parent)
    : QWidget(parent),d(new SimpleFilterPrivate(this))
{
    d->m_tree = tree;
    if(tree){
        d->m_type = TreeType;
    }
    d->q = this;
    d->init();
}

SimpleFilter::SimpleFilter(QTreeView *view, QWidget *parent)
    : QWidget(parent),d(new SimpleFilterPrivate(this))
{
    d->m_view = view;
    if(view){
        d->m_model = view->model();
        d->m_type = ViewType;
    }
    d->q = this;
    d->init();
}

void SimpleFilter::setTableWidget(QTableWidget *table)
{
    if(!table) return;
    if(d->m_type == TableType || d->m_type == NoneType){
        d->m_table = table;
        d->m_type = TableType;
    }
}

void SimpleFilter::setTreeWidget(QTreeWidget *tree)
{
    if(!tree) return;
    if(d->m_type == TreeType || d->m_type == NoneType){
        d->m_tree = tree;
        d->m_type = TreeType;
    }
}

void SimpleFilter::setTreeView(QTreeView *view)
{
    if(!view) return;
    if(d->m_type == ViewType || d->m_type == NoneType){
        d->m_view = view;
        d->m_model = view->model();
        d->m_type = ViewType;
    }
}

void SimpleFilter::clear()
{
    d->m_lineEdit->clear();
}



#include "simplefilter.moc"
