// Copyright (C) 2026 maminjie <canpool@163.com>
// SPDX-License-Identifier: MulanPSL-2.0

// Demonstrates the on-demand plugin loading added in 4.5: QxPluginManager::
// resolvePlugins() reads and orders every plugin's metadata and starts none of
// them, and QxPluginManager::requestPlugin() starts one plugin - with the
// dependencies it still needs - the moment the host first asks for it.
//
// A host built this way can bring its window up first and pay for each module
// only when that module is actually used, instead of loading everything at
// startup. The same metadata still drives a full loadPlugins() when the host
// would rather start everything at once.

#include "qxplugin/qxplugin.h"
#include "qxplugin/qxplugincontext.h"
#include "qxplugin/qxpluginmanager.h"
#include "qxplugin/qxpluginspec.h"

#include <QtCore/QCoreApplication>
#include <QtCore/QJsonArray>
#include <QtCore/QJsonObject>
#include <QtCore/QStringList>

#include <cstdio>

QX_PLUGIN_USE_NAMESPACE

// A plugin that records the two lifecycle moments a load reports, so the example
// can show the order resolve/request walks them in. initialize() runs first, in
// dependency order; extensionsInitialized() follows it, once the whole request
// - the plugin and every dependency it pulled in - is up.
// The namespace and the base class share the name QxPlugin, so every use of
// the class is qualified from the global scope (::QxPlugin::QxPlugin): with
// QX_PLUGIN_USE_NAMESPACE in effect the bare name is ambiguous on MSVC.
class DemoPlugin : public ::QxPlugin::QxPlugin
{
    Q_OBJECT
public:
    explicit DemoPlugin(const QString &name, QStringList *log)
        : ::QxPlugin::QxPlugin(nullptr)
        , m_name(name)
        , m_log(log)
    {
    }

    bool initialize(QxPluginContext *, QString *) override
    {
        m_log->append(m_name + QStringLiteral(": initialized"));
        return true;
    }

    void extensionsInitialized() override
    {
        m_log->append(m_name + QStringLiteral(": extensions initialized"));
    }
private:
    QString m_name;
    QStringList *m_log;
};

// Builds a plugin.json-shaped metadata object, exactly what add_qtc_plugin writes
// into a plugin's binary. The manager reads this to resolve dependencies and to
// decide what would start; it never needs the plugin object to do so.
static QJsonObject metaData(const QString &name, const QStringList &deps = QStringList())
{
    QJsonObject o;
    o.insert(QStringLiteral("Name"), name);
    o.insert(QStringLiteral("Version"), QStringLiteral("1.0.0"));
    o.insert(QStringLiteral("CompatVersion"), QStringLiteral("1.0.0"));
    o.insert(QStringLiteral("EnabledByDefault"), true);
    if (!deps.isEmpty()) {
        QJsonArray arr;
        for (const QString &dep : deps)
            arr.append(QJsonObject{{QStringLiteral("Name"), dep}});
        o.insert(QStringLiteral("Dependencies"), arr);
    }
    return o;
}

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);

    QStringList log;
    QxPluginManager manager;

    // a and c are independent; b needs a. Discovery order is a, b, c. In a real
    // host these would be loaded from disk; registerStaticPlugin() stands in for
    // that here so the example stays self-contained.
    manager.registerStaticPlugin(QStringLiteral("a"), metaData(QStringLiteral("a")), [&log]() {
        return new DemoPlugin(QStringLiteral("a"), &log);
    });
    manager.registerStaticPlugin(QStringLiteral("b"), metaData(QStringLiteral("b"), {QStringLiteral("a")}), [&log]() {
        return new DemoPlugin(QStringLiteral("b"), &log);
    });
    manager.registerStaticPlugin(QStringLiteral("c"), metaData(QStringLiteral("c")), [&log]() {
        return new DemoPlugin(QStringLiteral("c"), &log);
    });

    // Read and order every plugin's metadata, start none of them. The window can
    // come up now, and the load order is already known for a manager UI.
    manager.resolvePlugins();
    std::printf("after resolvePlugins(): started %d plugin(s)\n", static_cast<int>(log.size()));

    QStringList order;
    for (QxPluginSpec *s : manager.specs())
        order.append(s->id());
    std::printf("load order would be: %s\n", order.join(QStringLiteral(" -> ")).toUtf8().constData());

    // Ask for b: a starts first (b needs it), then b. c and anything else is
    // left alone.
    ::QxPlugin::QxPlugin *b = manager.requestPlugin(QStringLiteral("b"));
    std::printf("requestPlugin(\"b\") -> %s\n",
                (b ? manager.spec(QStringLiteral("b"))->id() : QStringLiteral("null")).toUtf8().constData());
    std::printf("  lifecycle so far: %s\n", log.join(QStringLiteral(", ")).toUtf8().constData());

    // Ask for c on its own later. The plugins already up are not started again,
    // and c's extensionsInitialized() does not re-run for them.
    ::QxPlugin::QxPlugin *c = manager.requestPlugin(QStringLiteral("c"));
    std::printf("requestPlugin(\"c\") -> %s\n",
                (c ? manager.spec(QStringLiteral("c"))->id() : QStringLiteral("null")).toUtf8().constData());
    std::printf("  lifecycle so far: %s\n", log.join(QStringLiteral(", ")).toUtf8().constData());

    return 0;
}

#include "main.moc"
