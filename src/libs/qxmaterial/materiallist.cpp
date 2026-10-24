/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#include "materiallist.h"

#include "materiallistitem.h"

#include <QtWidgets/QVBoxLayout>

QX_MATERIAL_BEGIN_NAMESPACE

class MaterialListPrivate
{
public:
    QX_DECLARE_PUBLIC(MaterialList)

    QVBoxLayout *layout = Q_NULLPTR;
    MaterialListItem *current = Q_NULLPTR;

    void choose(MaterialListItem *item);
};

void MaterialListPrivate::choose(MaterialListItem *item)
{
    if (current == item) {
        return;
    }
    MaterialListItem *previous = current;
    if (previous) {
        previous->setSelected(false);
    }
    current = item;
    if (current) {
        current->setSelected(true);
    }
    Q_EMIT q_ptr->currentItemChanged(current);
}

MaterialList::MaterialList(QWidget *parent)
    : QWidget(parent)
    , d_ptr(new MaterialListPrivate)
{
    QX_SET_PRIVATE(d_ptr)
    d_ptr->layout = new QVBoxLayout(this);
    d_ptr->layout->setContentsMargins(0, 0, 0, 0);
    d_ptr->layout->setSpacing(0);
    d_ptr->layout->addStretch(1);
}

MaterialList::~MaterialList()
{
    // A row answers when it is taken away, and a list takes its rows with it -
    // but a list deletes its own data before its children, so by the time the
    // last row says it is gone there is nothing left of the list to tell, and
    // answering would be reading a member of an object that has already been
    // destroyed (a window closed on a list of rows took the process with it the
    // first time). The answers are dropped here instead, while the list is
    // still whole: every row is going with it, and none of them is news. This
    // has to happen before the data goes, since that is where the rows are.
    for (int i = 0; i < count(); ++i) {
        QObject::disconnect(itemAt(i), Q_NULLPTR, this, Q_NULLPTR);
    }
    QX_FINI_PRIVATE()
}

int MaterialList::count() const
{
    return d_ptr->layout->count() - 1;   // the stretch is not a row
}

MaterialListItem *MaterialList::itemAt(int index) const
{
    if (index < 0 || index >= count()) {
        return Q_NULLPTR;
    }
    return static_cast<MaterialListItem *>(d_ptr->layout->itemAt(index)->widget());
}

MaterialListItem *MaterialList::currentItem() const
{
    return d_ptr->current;
}

void MaterialList::addItem(MaterialListItem *item)
{
    Q_D(MaterialList);
    if (!item) {
        return;
    }
    // The stretch keeps the rows packed at the top when the list is given
    // more room than its rows ask for; the row goes in ahead of it.
    d_ptr->layout->insertWidget(d_ptr->layout->count() - 1, item);
    // A row that arrived late still arrives in view: the layout does not
    // show a widget the list was already showing without.
    item->show();
    connect(item, &MaterialListItem::clicked, this, [this, item]() {
        Q_EMIT itemClicked(item);
        d_ptr->choose(item);
    });
    // A row that is taken away is no longer the chosen one: the list does not
    // keep a name without a row behind it. A row taken away *with* the list is
    // not among these - the destructor drops the answers before the rows go,
    // because by then there is no list left to hear one.
    connect(item, &QObject::destroyed, this, [this, item]() {
        if (d_ptr->current == item) {
            d_ptr->current = Q_NULLPTR;
            Q_EMIT currentItemChanged(Q_NULLPTR);
        }
    });
}

MaterialListItem *MaterialList::addItem(const QString &text, const QString &secondary, const QIcon &icon)
{
    MaterialListItem *item = new MaterialListItem(this);
    item->setText(text);
    if (!secondary.isEmpty()) {
        item->setSecondaryText(secondary);
    }
    if (!icon.isNull()) {
        item->setIcon(icon);
    }
    addItem(item);
    return item;
}

void MaterialList::setCurrentItem(MaterialListItem *item)
{
    Q_D(MaterialList);
    if (!item || itemAt(d_ptr->layout->indexOf(item)) != item) {
        return;
    }
    d_ptr->choose(item);
}

void MaterialList::clearSelection()
{
    Q_D(MaterialList);
    if (!d_ptr->current) {
        return;
    }
    d_ptr->current->setSelected(false);
    d_ptr->current = Q_NULLPTR;
    Q_EMIT currentItemChanged(Q_NULLPTR);
}

QX_MATERIAL_END_NAMESPACE
