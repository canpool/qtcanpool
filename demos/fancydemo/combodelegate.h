#ifndef COMBODELEGATE_H
#define COMBODELEGATE_H

#include <QItemDelegate>

class ComboDelegate : public QItemDelegate
{
    Q_OBJECT
public:
    ComboDelegate(const QStringList items, QObject *parent = 0);

    QWidget *createEditor(QWidget * parent, const QStyleOptionViewItem & option,
                          const QModelIndex & index) const;
    void setEditorData(QWidget * editor, const QModelIndex & index) const;
    void setModelData(QWidget * editor, QAbstractItemModel * model, const QModelIndex & index) const;
    void updateEditorGeometry(QWidget * editor, const QStyleOptionViewItem & option,
                              const QModelIndex & index) const;
private:
    QStringList m_items;
};

#endif // COMBODELEGATE_H
