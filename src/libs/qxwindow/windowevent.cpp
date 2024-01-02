/**
 * Copyright (C) 2023-2024 maminjie <canpool@163.com>
 * SPDX-License-Identifier: Apache-2.0
**/
#include "windowevent_p.h"
#include <QtCore/QAbstractNativeEventFilter>
#include <QtCore/QCoreApplication>

QX_WINDOW_BEGIN_NAMESPACE

WindowNativeEventFilter::WindowNativeEventFilter() : m_nativeDispatcher(nullptr)
{
}

WindowNativeEventFilter::~WindowNativeEventFilter()
{
    if (m_nativeDispatcher)
        m_nativeDispatcher->removeNativeEventFilter(this);
}

WindowNativeEventDispatcher::WindowNativeEventDispatcher() = default;

WindowNativeEventDispatcher::~WindowNativeEventDispatcher()
{
    for (const auto &observer : qAsConst(m_nativeEventFilters)) {
        observer->m_nativeDispatcher = nullptr;
    }
}

bool WindowNativeEventDispatcher::nativeDispatch(const QByteArray &eventType, void *message,
                                           QT_NATIVE_EVENT_RESULT_TYPE *result)
{
    for (const auto &ef : qAsConst(m_nativeEventFilters)) {
        if (ef->nativeEventFilter(eventType, message, result))
            return true;
    }
    return false;
}

void WindowNativeEventDispatcher::installNativeEventFilter(WindowNativeEventFilter *filter)
{
    if (!filter || filter->m_nativeDispatcher)
        return;

    m_nativeEventFilters.append(filter);
    filter->m_nativeDispatcher = this;
}

void WindowNativeEventDispatcher::removeNativeEventFilter(WindowNativeEventFilter *filter)
{
    if (!m_nativeEventFilters.removeOne(filter)) {
        return;
    }
    filter->m_nativeDispatcher = nullptr;
}

// Avoid adding multiple global native event filters to QGuiApplication
// in this library.
class AppMasterNativeEventFilter : public QAbstractNativeEventFilter, public WindowNativeEventDispatcher
{
public:
    AppMasterNativeEventFilter()
    {
        qApp->installNativeEventFilter(this);
    }

    // The base class removes automatically
    ~AppMasterNativeEventFilter() override = default;

    bool nativeEventFilter(const QByteArray &eventType, void *message, QT_NATIVE_EVENT_RESULT_TYPE *result) override
    {
        return nativeDispatch(eventType, message, result);
    }

    static AppMasterNativeEventFilter *instance;

    friend class AppNativeEventFilter;
};

AppMasterNativeEventFilter *AppMasterNativeEventFilter::instance = nullptr;

AppNativeEventFilter::AppNativeEventFilter()
{
    if (!AppMasterNativeEventFilter::instance) {
        AppMasterNativeEventFilter::instance = new AppMasterNativeEventFilter();
    }
    AppMasterNativeEventFilter::instance->installNativeEventFilter(this);
}

AppNativeEventFilter::~AppNativeEventFilter()
{
    AppMasterNativeEventFilter::instance->removeNativeEventFilter(this);
    if (AppMasterNativeEventFilter::instance->m_nativeEventFilters.isEmpty()) {
        delete AppMasterNativeEventFilter::instance;
        AppMasterNativeEventFilter::instance = nullptr;
    }
}


WindowSharedEventFilter::WindowSharedEventFilter() : m_sharedDispatcher(nullptr)
{
}

WindowSharedEventFilter::~WindowSharedEventFilter()
{
    if (m_sharedDispatcher)
        m_sharedDispatcher->removeSharedEventFilter(this);
}

WindowSharedEventDispatcher::WindowSharedEventDispatcher() = default;

WindowSharedEventDispatcher::~WindowSharedEventDispatcher()
{
    for (const auto &observer : qAsConst(m_sharedEventFilters)) {
        observer->m_sharedDispatcher = nullptr;
    }
}

bool WindowSharedEventDispatcher::sharedDispatch(QObject *obj, QEvent *event)
{
    for (const auto &ef : qAsConst(m_sharedEventFilters)) {
        if (ef->sharedEventFilter(obj, event))
            return true;
    }
    return false;
}

void WindowSharedEventDispatcher::installSharedEventFilter(WindowSharedEventFilter *filter)
{
    if (!filter || filter->m_sharedDispatcher)
        return;

    m_sharedEventFilters.append(filter);
    filter->m_sharedDispatcher = this;
}

void WindowSharedEventDispatcher::removeSharedEventFilter(WindowSharedEventFilter *filter)
{
    if (!m_sharedEventFilters.removeOne(filter)) {
        return;
    }
    filter->m_sharedDispatcher = nullptr;
}

QX_WINDOW_END_NAMESPACE
