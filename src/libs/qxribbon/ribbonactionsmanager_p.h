/**
 * Copyleft (C) 2023 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MIT
**/
#pragma once

#include "qxribbon_global.h"

#include <QAbstractListModel>
#include <QAction>
#include <QMap>
#include <QObject>
#include <QString>

QX_RIBBON_BEGIN_NAMESPACE

class RibbonActionsManager;
class RibbonActionsManagerModelPrivate;

class RibbonActionsManagerModel : public QAbstractListModel
{
    Q_OBJECT
public:
    explicit RibbonActionsManagerModel(QObject *p = Q_NULLPTR);
    explicit RibbonActionsManagerModel(RibbonActionsManager *m, QObject *p = Q_NULLPTR);
    ~RibbonActionsManagerModel();

    virtual int rowCount(const QModelIndex &parent) const Q_DECL_OVERRIDE;
    virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const Q_DECL_OVERRIDE;
    virtual Qt::ItemFlags flags(const QModelIndex &index) const Q_DECL_OVERRIDE;
    virtual QVariant data(const QModelIndex &index, int role) const Q_DECL_OVERRIDE;
    void setFilter(int tag);
    void update();
    void setupActionsManager(RibbonActionsManager *m);
    void uninstallActionsManager();
    QAction *indexToAction(QModelIndex index) const;
    void search(const QString &text);

private Q_SLOTS:
    void onActionTagChanged(int tag, bool isdelete);
private:
    QX_DECLARE_PRIVATE(RibbonActionsManagerModel)
};

QX_RIBBON_END_NAMESPACE
