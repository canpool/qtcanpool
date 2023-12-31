/**
 * Copyright (C) 2023-2024 maminjie <canpool@163.com>
 * SPDX-License-Identifier: Apache-2.0
**/
#ifndef WINDOWEVENT_H
#define WINDOWEVENT_H

//
//  W A R N I N G !!!
//  -----------------
//
// This file is not part of the QxWindow API. It is used purely as an
// implementation detail. This header file may change from version to
// version without notice, or may even be removed.
//

#include "qxwindow_global.h"

QX_WINDOW_BEGIN_NAMESPACE

class WindowNativeEventFilter;

class QX_WINDOW_EXPORT WindowNativeEventDispatcher
{
public:
    WindowNativeEventDispatcher();
    virtual ~WindowNativeEventDispatcher();
public:
    virtual bool nativeDispatch(const QByteArray &eventType, void *message, QT_NATIVE_EVENT_RESULT_TYPE *result);
public:
    void installNativeEventFilter(WindowNativeEventFilter *filter);
    void removeNativeEventFilter(WindowNativeEventFilter *filter);
protected:
    QVector<WindowNativeEventFilter *> m_nativeEventFilters;

    friend class WindowNativeEventFilter;

    Q_DISABLE_COPY(WindowNativeEventDispatcher)
};

class QX_WINDOW_EXPORT WindowNativeEventFilter
{
public:
    WindowNativeEventFilter();
    virtual ~WindowNativeEventFilter();
public:
    virtual bool nativeEventFilter(const QByteArray &eventType, void *message, QT_NATIVE_EVENT_RESULT_TYPE *result) = 0;
protected:
    WindowNativeEventDispatcher *m_nativeDispatcher;

    friend class WindowNativeEventDispatcher;

    Q_DISABLE_COPY(WindowNativeEventFilter)
};

// Automatically install to QCoreApplication at construction
class QX_WINDOW_EXPORT AppNativeEventFilter : public WindowNativeEventFilter
{
public:
    AppNativeEventFilter();
    ~AppNativeEventFilter() override;
};


class WindowSharedEventFilter;

class QX_WINDOW_EXPORT WindowSharedEventDispatcher
{
public:
    WindowSharedEventDispatcher();
    virtual ~WindowSharedEventDispatcher();
public:
    virtual bool sharedDispatch(QObject *obj, QEvent *event);
public:
    void installSharedEventFilter(WindowSharedEventFilter *filter);
    void removeSharedEventFilter(WindowSharedEventFilter *filter);
protected:
    QVector<WindowSharedEventFilter *> m_sharedEventFilters;

    friend class WindowSharedEventFilter;

    Q_DISABLE_COPY(WindowSharedEventDispatcher)
};

class QX_WINDOW_EXPORT WindowSharedEventFilter
{
public:
    WindowSharedEventFilter();
    virtual ~WindowSharedEventFilter();
public:
    virtual bool sharedEventFilter(QObject *obj, QEvent *event) = 0;
protected:
    WindowSharedEventDispatcher *m_sharedDispatcher;

    friend class WindowSharedEventDispatcher;

    Q_DISABLE_COPY(WindowSharedEventFilter)
};

QX_WINDOW_END_NAMESPACE

#endif   // WINDOWEVENT_H
