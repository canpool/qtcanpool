/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#include "qxobjectpool.h"

QX_PLUGIN_BEGIN_NAMESPACE

QxObjectPool::QxObjectPool(QObject *parent)
    : QObject(parent)
{
}

QxObjectPool::~QxObjectPool() = default;

void QxObjectPool::addObject(QObject *object)
{
    if (!object)
        return;
    if (m_objects.contains(QPointer<QObject>(object)))
        return;

    m_objects.append(object);
    emit objectAdded(object);
}

void QxObjectPool::removeObject(QObject *object)
{
    if (!object)
        return;

    const int index = m_objects.indexOf(QPointer<QObject>(object));
    if (index < 0)
        return;

    emit aboutToRemoveObject(object);
    m_objects.removeAt(index);
}

QList<QObject *> QxObjectPool::objects() const
{
    // Nulls are what a destroyed object leaves behind, and this is the one place
    // that has to look past them; every other query comes through here.
    QList<QObject *> live;
    live.reserve(m_objects.count());
    for (const QPointer<QObject> &entry : m_objects) {
        if (entry)
            live.append(entry.data());
    }
    return live;
}

QObject *QxObjectPool::objectByName(const QString &name) const
{
    if (name.isEmpty())
        return Q_NULLPTR;

    const QList<QObject *> live = objects();
    for (QObject *object : live) {
        if (object->objectName() == name)
            return object;
    }
    return Q_NULLPTR;
}

QX_PLUGIN_END_NAMESPACE
