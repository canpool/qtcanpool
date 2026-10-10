/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#ifndef QXOBJECTPOOL_H
#define QXOBJECTPOOL_H

#include "qxplugin_global.h"

#include <QtCore/QList>
#include <QtCore/QObject>
#include <QtCore/QPointer>
#include <QtCore/QString>

QX_PLUGIN_BEGIN_NAMESPACE

/*!
 * The meeting point for two plugins that want to cooperate.
 *
 * A plugin is handed a QxPluginContext and no handle to its peers, so the two
 * ends of any plugin-to-plugin agreement - "I have a line worth reporting" and
 * "I have somewhere to put it" - have no way to find each other by themselves.
 * This is where the providing end puts what it offers, and where the wanting end
 * looks for it:
 *
 * @code
 * // the provider, in initialize():
 * setObjectName(QStringLiteral("output"));
 * context->addObject(this);
 * // the consumer, in initialize():
 * QObject *sink = context->objectByName(QStringLiteral("output"));
 * if (sink)
 *     connect(this, SIGNAL(fileActivated(QString)), sink, SLOT(appendLine(QString)));
 * @endcode
 *
 * What comes back is what somebody chose to put in - publishing is deliberate,
 * so a plugin that publishes nothing stays unreachable - and it is typed only as
 * far as QObject, which is what lets the two ends agree without either including
 * the other's header.
 *
 * The pool owns nothing. Its entries are guarded pointers, so an object that is
 * destroyed leaves the pool by itself: a publisher that is unloaded mid-session
 * cannot leave a dangling pointer behind, and never has to remember to clean up.
 * Deleting the object is still the business of whoever created it.
 *
 * A host reaches this through QxPluginContext, which hands over the host's own
 * pool; it is the context - not the manager - that a plugin sees.
 */
class QX_PLUGIN_EXPORT QxObjectPool : public QObject
{
    Q_OBJECT
public:
    explicit QxObjectPool(QObject *parent = Q_NULLPTR);
    ~QxObjectPool() override;

    /*!
     * Publishes \a object under its objectName(). Publishing the same object
     * twice changes nothing; a null object is ignored.
     */
    void addObject(QObject *object);
    /*! Takes \a object out. The object is not deleted, and an unknown one is a no-op. */
    void removeObject(QObject *object);

    /*! Everything published and still alive, in the order it was published. */
    QList<QObject *> objects() const;
    /*! The first live published object whose objectName() is \a name, or null. */
    QObject *objectByName(const QString &name) const;

    /*!
     * The first live published object castable to \a T, or null.
     *
     * Use this when the consumer and the provider share a declaration of \a T -
     * a host service, or a type both sides link the same library for. When they
     * share nothing at all, objectByName() is the lookup that still works, and
     * what comes back is driven through the meta-object.
     */
    template <typename T> T *object() const
    {
        const QList<QObject *> all = objects();
        for (QObject *candidate : all) {
            if (T *match = qobject_cast<T *>(candidate))
                return match;
        }
        return Q_NULLPTR;
    }

Q_SIGNALS:
    /*! Emitted after \a object joined the pool. */
    void objectAdded(QObject *object);
    /*!
     * Emitted just before removeObject() takes \a object out.
     *
     * An object that is simply destroyed leaves without this - the pool finds out
     * from the pointer going null, by which time there is nothing useful left to
     * announce. Subscribe to that object's own destroyed() signal if the moment
     * of its death matters to you.
     */
    void aboutToRemoveObject(QObject *object);
private:
    Q_DISABLE_COPY(QxObjectPool)
    QList<QPointer<QObject>> m_objects;
};

QX_PLUGIN_END_NAMESPACE

#endif   // QXOBJECTPOOL_H
