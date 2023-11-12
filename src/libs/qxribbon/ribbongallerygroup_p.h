/**
 * Copyleft (C) 2023 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MIT
 **/
#pragma once

#include "qxribbon_global.h"

#include <QAction>
#include <QIcon>
#include <QMap>
#include <QVariant>
#include <QList>
#include <QListView>
#include <QStyledItemDelegate>

QX_RIBBON_BEGIN_NAMESPACE

class RibbonGalleryItemPrivate;

class RibbonGalleryItem
{
public:
    RibbonGalleryItem();
    RibbonGalleryItem(const QString &text, const QIcon &icon);
    RibbonGalleryItem(QAction *act);
    virtual ~RibbonGalleryItem();

    void setData(int role, const QVariant &data);
    virtual QVariant data(int role) const;

    void setText(const QString &text);
    QString text() const;

    void setToolTip(const QString &text);
    QString toolTip() const;

    void setIcon(const QIcon &ico);
    QIcon icon() const;

    bool isSelectable() const;
    void setSelectable(bool isSelectable);

    bool isEnable() const;
    void setEnable(bool isEnable);

    void setFlags(Qt::ItemFlags flag);
    virtual Qt::ItemFlags flags() const;

    void setAction(QAction *act);
    QAction *action();

    void setTextAlignment(Qt::Alignment a);
    Qt::Alignment textAlignment() const;
private:
    RibbonGalleryItemPrivate *d;
};

/* RibbonGalleryGroupItemDelegate */
class RibbonGalleryGroup;
class RibbonGalleryGroupItemDelegate : public QStyledItemDelegate
{
public:
    RibbonGalleryGroupItemDelegate(RibbonGalleryGroup *group, QObject *parent = Q_NULLPTR);

    virtual void paint(QPainter *painter, const QStyleOptionViewItem &option,
                       const QModelIndex &index) const Q_DECL_OVERRIDE;

    virtual QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const Q_DECL_OVERRIDE;
    virtual void paintIconOnly(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;
    virtual void paintIconWithText(QPainter *painter, const QStyleOptionViewItem &option,
                                   const QModelIndex &index) const;
    virtual void paintIconWithTextWordWrap(QPainter *painter, const QStyleOptionViewItem &option,
                                           const QModelIndex &index) const;
private:
    RibbonGalleryGroup *m_group;
};

/* RibbonGalleryGroupModel */
class RibbonGalleryGroupModel : public QAbstractListModel
{
    Q_OBJECT
public:
    explicit RibbonGalleryGroupModel(QObject *parent = Q_NULLPTR);
    virtual ~RibbonGalleryGroupModel();

    virtual int rowCount(const QModelIndex &parent) const Q_DECL_OVERRIDE;
    virtual Qt::ItemFlags flags(const QModelIndex &index) const Q_DECL_OVERRIDE;
    virtual QVariant data(const QModelIndex &index, int role) const Q_DECL_OVERRIDE;
    virtual QModelIndex index(int row, int column, const QModelIndex &parent) const Q_DECL_OVERRIDE;
    virtual bool setData(const QModelIndex &index, const QVariant &value, int role) Q_DECL_OVERRIDE;
    void clear();
    RibbonGalleryItem *at(int row) const;
    void insert(int row, RibbonGalleryItem *item);
    RibbonGalleryItem *take(int row);
    void append(RibbonGalleryItem *item);
private:
    QList<RibbonGalleryItem *> m_items;
};

QX_RIBBON_END_NAMESPACE
