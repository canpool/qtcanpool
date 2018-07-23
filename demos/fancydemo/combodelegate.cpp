/*!
 * \file combodelegate.cpp
 * \brief
 * \author canpool@163.com
 * \version v0.1.0
 * \date
 * \b
 * \li
 *
 */
#include "combodelegate.h"
#include <QComboBox>

ComboDelegate::ComboDelegate(const QStringList items, QObject *parent)
    : QItemDelegate(parent)
{
    m_items = items;
}

QWidget* ComboDelegate::createEditor(QWidget *parent,
                                     const QStyleOptionViewItem &option,
                                     const QModelIndex &index) const
{
    Q_UNUSED(option);
    Q_UNUSED(index);

    QComboBox *combox = new QComboBox(parent);
//    combox->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);
    combox->addItems(m_items);
    combox->installEventFilter(const_cast<ComboDelegate*>(this));
    return combox;
}

void ComboDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    QString value = index.model()->data(index).toString();
    QComboBox *box = static_cast<QComboBox*>(editor);
    int i = box->findText(value);
    box->setCurrentIndex(i);

}

void ComboDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
    QComboBox *box = static_cast<QComboBox*>(editor);
    model->setData(index,box->currentText());
}

void ComboDelegate::updateEditorGeometry(QWidget *editor,
                                         const QStyleOptionViewItem &option,
                                         const QModelIndex &index) const
{
    Q_UNUSED(index);
    editor->setGeometry(option.rect);
}
