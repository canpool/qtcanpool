/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#include "qxpluginspec.h"

#include "qxplugin.h"

#include <QtCore/QJsonArray>
#include <QtCore/QJsonValue>

QX_PLUGIN_BEGIN_NAMESPACE

class QxPluginSpecPrivate
{
    QX_DECLARE_PUBLIC(QxPluginSpec)
public:
    QString filePath;
    QString id;
    QVersionNumber version;
    QVersionNumber compatVersion;
    QString vendor;
    QString copyright;
    QString category;
    QString description;
    QString url;
    QStringList dependencies;
    QStringList optionalDependencies;
    bool enabledByDefault = true;
    QxPluginState state = QxPluginState::Invalid;
    QxPlugin *plugin = Q_NULLPTR;
    /*! Non-empty only for a real failure (missing / version / load / init). A skipped-by-default
     *  or config-disabled plugin keeps this empty, so error().isEmpty() is the "merely skipped" test. */
    QString error;
};

QxPluginSpec::QxPluginSpec()
    : d_ptr(new QxPluginSpecPrivate)
{
}

QxPluginSpec::~QxPluginSpec(){QX_FINI_PRIVATE()}

QString QxPluginSpec::id() const
{
    Q_D(const QxPluginSpec);
    return d->id;
}

QVersionNumber QxPluginSpec::version() const
{
    Q_D(const QxPluginSpec);
    return d->version;
}

QVersionNumber QxPluginSpec::compatVersion() const
{
    Q_D(const QxPluginSpec);
    return d->compatVersion;
}

QString QxPluginSpec::vendor() const
{
    Q_D(const QxPluginSpec);
    return d->vendor;
}

QString QxPluginSpec::copyright() const
{
    Q_D(const QxPluginSpec);
    return d->copyright;
}

QString QxPluginSpec::category() const
{
    Q_D(const QxPluginSpec);
    return d->category;
}

QString QxPluginSpec::description() const
{
    Q_D(const QxPluginSpec);
    return d->description;
}

QString QxPluginSpec::url() const
{
    Q_D(const QxPluginSpec);
    return d->url;
}

QStringList QxPluginSpec::dependencies() const
{
    Q_D(const QxPluginSpec);
    return d->dependencies;
}

QStringList QxPluginSpec::optionalDependencies() const
{
    Q_D(const QxPluginSpec);
    return d->optionalDependencies;
}

bool QxPluginSpec::isEnabledByDefault() const
{
    Q_D(const QxPluginSpec);
    return d->enabledByDefault;
}

bool QxPluginSpec::isDisabled() const
{
    Q_D(const QxPluginSpec);
    return d->state == QxPluginState::Disabled;
}

QxPluginState QxPluginSpec::state() const
{
    Q_D(const QxPluginSpec);
    return d->state;
}

QxPlugin *QxPluginSpec::plugin() const
{
    Q_D(const QxPluginSpec);
    return d->plugin;
}

QString QxPluginSpec::filePath() const
{
    Q_D(const QxPluginSpec);
    return d->filePath;
}

QString QxPluginSpec::error() const
{
    Q_D(const QxPluginSpec);
    return d->error;
}

bool QxPluginSpec::read(const QString &filePath, const QJsonObject &metaData)
{
    Q_D(QxPluginSpec);
    d->filePath = filePath;
    d->state = QxPluginState::Invalid;
    d->error.clear();

    const QString name = metaData.value(QStringLiteral("Name")).toString();
    if (name.isEmpty()) {
        d->error = QStringLiteral("plugin metadata has no \"Name\"");
        return false;
    }
    d->id = name;

    QVersionNumber ver = QVersionNumber::fromString(metaData.value(QStringLiteral("Version")).toString());
    if (ver.isNull())
        ver = QVersionNumber(1, 0, 0);
    d->version = ver;
    QVersionNumber compat = QVersionNumber::fromString(metaData.value(QStringLiteral("CompatVersion")).toString());
    if (compat.isNull())
        compat = ver;
    d->compatVersion = compat;

    d->vendor = metaData.value(QStringLiteral("Vendor")).toString();
    d->copyright = metaData.value(QStringLiteral("Copyright")).toString();
    d->category = metaData.value(QStringLiteral("Category")).toString();
    d->description = metaData.value(QStringLiteral("Description")).toString();
    d->url = metaData.value(QStringLiteral("Url")).toString();
    d->enabledByDefault = metaData.value(QStringLiteral("EnabledByDefault")).toBool(true);

    const QJsonValue deps = metaData.value(QStringLiteral("Dependencies"));
    if (!deps.isUndefined()) {
        if (!deps.isArray()) {
            d->error = QStringLiteral("plugin '%1': \"Dependencies\" is not an array").arg(name);
            return false;
        }
        for (const QJsonValue &entry : deps.toArray()) {
            const QJsonObject obj = entry.toObject();
            const QString depName = obj.value(QStringLiteral("Name")).toString();
            if (depName.isEmpty())
                continue;
            const QString type = obj.value(QStringLiteral("Type")).toString();
            if (type == QStringLiteral("optional")) {
                d->optionalDependencies.append(depName);
            } else if (type != QStringLiteral("test")) {
                // "test" dependencies are force-loaded for a test run only and do
                // not affect load order. Nothing here has a test mode, so the
                // entry is read and dropped rather than mistaken for a soft edge
                // that would reorder the run behind the caller's back. Anything
                // else - an explicit "required", or no Type at all - is a hard
                // dependency, which is what the metadata means by a bare entry.
                d->dependencies.append(depName);
            }
        }
    }

    d->state = QxPluginState::Read;
    return true;
}

void QxPluginSpec::setState(QxPluginState state)
{
    Q_D(QxPluginSpec);
    d->state = state;
}

void QxPluginSpec::setError(const QString &error)
{
    Q_D(QxPluginSpec);
    d->error = error;
}

void QxPluginSpec::setPlugin(QxPlugin *plugin)
{
    Q_D(QxPluginSpec);
    d->plugin = plugin;
}

QX_PLUGIN_END_NAMESPACE
