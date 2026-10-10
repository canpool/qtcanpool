/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#include "qxplugincontext.h"

QX_PLUGIN_BEGIN_NAMESPACE

QxPluginContext::QxPluginContext(QxObjectPool *pool)
    : m_pool(pool)
{
}

QxPluginContext::~QxPluginContext() = default;

void QxPluginContext::addObject(QObject *object)
{
    if (m_pool)
        m_pool->addObject(object);
}

void QxPluginContext::removeObject(QObject *object)
{
    if (m_pool)
        m_pool->removeObject(object);
}

QList<QObject *> QxPluginContext::objects() const
{
    return m_pool ? m_pool->objects() : QList<QObject *>();
}

QObject *QxPluginContext::objectByName(const QString &name) const
{
    return m_pool ? m_pool->objectByName(name) : Q_NULLPTR;
}

QX_PLUGIN_END_NAMESPACE
