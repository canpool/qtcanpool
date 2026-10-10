/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#ifndef QXPLUGINCONTEXT_H
#define QXPLUGINCONTEXT_H

#include "qxplugin_global.h"
#include "qxobjectpool.h"

#include "qxcore/qxsettings.h"

#include <QtCore/QList>
#include <QtCore/QObject>
#include <QtCore/QString>
#include <QtCore/QStringList>
#include <QtGui/QIcon>
#include <QtWidgets/QWidget>

QX_PLUGIN_BEGIN_NAMESPACE

/*!
 * The thin surface a plugin programs against.
 *
 * The host - the application shell that loaded the plugin - implements this
 * interface and hands it to the plugin as the context; a plugin never sees the
 * host's window class. Keeping the surface in qxplugin, which depends on
 * nothing above qxcore, is what keeps the host's class name out of every
 * plugin: the host implements this interface, the plugin codes against it.
 *
 * The surface only lists the host abilities a plugin is allowed to use. Adding
 * a capability here is therefore a deliberate, versioned extension of the
 * plugin contract; anything host-specific stays behind the concrete host class.
 *
 * "A plugin never sees the host's window class" leaves one gap that a
 * host-mediated surface cannot close by itself: two plugins that want to
 * cooperate - one offering something, one wanting it - have no way to find each
 * other. The object pool below is the host's answer to that, and it is
 * deliberately the whole of it. What a plugin publishes is a choice it makes;
 * what it gets back is only ever a QObject, so the two ends still need no common
 * header and no link between them. See QxObjectPool for the mechanics.
 */
class QX_PLUGIN_EXPORT QxPluginContext
{
public:
    /*!
     * \a pool is the host's object pool and stays the host's property. A host
     * that offers no pool passes nothing, and every pool call below then does
     * nothing.
     */
    explicit QxPluginContext(QxObjectPool *pool = Q_NULLPTR);
    virtual ~QxPluginContext();

    /*! Where a newly added dock lands. Mirrors Qx::DockWidgetArea without coupling the plugin to qxdock. */
    enum DockArea {
        LeftDock = 0x1,
        RightDock = 0x2,
        TopDock = 0x4,
        BottomDock = 0x8
    };

    /*! Toast severity understood by every host. The host maps it onto its own toast types. */
    enum class ToastLevel {
        Information,
        Success,
        Warning,
        Error
    };

    // Pages -----------------------------------------------------------------
    /*!
     * Adds a page to the host's central workspace and registers it under \a id,
     * which must be stable and unique across the session. \a icon and \a title
     * identify the page in the host's navigation; the host decides how to
     * surface them (for example a rail entry). The host takes ownership of
     * \a page while it is installed.
     */
    virtual void addPage(const QString &id, const QIcon &icon, const QString &title, QWidget *page) = 0;
    /*! Brings the page registered under \a id to the front. Unknown id: no-op. */
    virtual void setCurrentPage(const QString &id) = 0;
    /*! Id of the page currently on screen, or an empty string when there is none. */
    virtual QString currentPageId() const = 0;

    // Docks -----------------------------------------------------------------
    /*!
     * Creates a dock widget named \a id, titled \a title, filled with \a widget
     * and dropped in \a area. Returns the dock, or null when \a id is empty or
     * already in use.
     */
    virtual QWidget *addDock(DockArea area, const QString &id, const QString &title, QWidget *widget) = 0;

    // Status bar ------------------------------------------------------------
    /*! Replaces the host's persistent status-line message. */
    virtual void setStatusMessage(const QString &message) = 0;
    /*! Shows or hides the host's indeterminate "working" indicator. */
    virtual void setBusy(bool busy) = 0;
    /*! Sets the range the progress runs in; \a maximum must be above \a minimum. */
    virtual void setProgressRange(int minimum, int maximum) = 0;
    /*! Shows how far the work has got, clamped to the range set by setProgressRange(). */
    virtual void setProgress(int value) = 0;
    /*! Hides the progress or busy indicator and resets it to the minimum. */
    virtual void clearProgress() = 0;

    // Notifications ---------------------------------------------------------
    /*! Shows a toast over the host; \a timeoutMs < 0 means the host's default. */
    virtual void showToast(const QString &text, ToastLevel level = ToastLevel::Information, int timeoutMs = -1) = 0;

    // Config ----------------------------------------------------------------
    /*! The settings object backing persistence; never null. */
    virtual QX_CORE_PREPEND_NAMESPACE(QxSettings) * settings() const = 0;

    // Object pool -----------------------------------------------------------
    /*
     * Where plugins meet. A plugin publishes what it is willing to offer, and
     * looks up what it needs; neither end includes the other's header, and the
     * host is not asked to know either of them.
     *
     * The lookups are by objectName() and by type. The name is the one that
     * works between two plugins that share nothing - what comes back is used
     * through the meta-object - while the type covers the cases where a common
     * declaration exists on both sides.
     */
    /*!
     * Publishes \a object under its objectName(), where other plugins can find
     * it. Publishing is deliberate: a plugin that publishes nothing stays
     * unreachable, which is why this is a call and not something that happens to
     * every plugin automatically.
     *
     * The pool does not take ownership, and the entry disappears when \a object
     * is destroyed. Removing it in shutdown() anyway is the tidier habit: it
     * goes away before the plugin does, rather than with it.
     */
    void addObject(QObject *object);
    /*! Takes \a object out of the pool. The object is not deleted; an unknown one is a no-op. */
    void removeObject(QObject *object);
    /*! Everything published by any plugin and still alive, in publication order. */
    QList<QObject *> objects() const;
    /*! The first live published object named \a name, or null. */
    QObject *objectByName(const QString &name) const;

    /*!
     * The first live published object castable to \a T, or null.
     *
     * For the cases where the two ends share a declaration of \a T - a host
     * service, or a type both link the same library for. Where they share
     * nothing, objectByName() is the lookup that still works.
     */
    template <typename T> T *object() const
    {
        return m_pool ? m_pool->object<T>() : Q_NULLPTR;
    }
protected:
    /*! The host's pool, or null when the host offers none. Owned by the host. */
    QxObjectPool *m_pool = Q_NULLPTR;
};

QX_PLUGIN_END_NAMESPACE

#endif   // QXPLUGINCONTEXT_H
