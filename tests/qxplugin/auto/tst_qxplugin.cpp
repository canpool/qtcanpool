#include "tst_global.h"

#include "qxplugin/qxplugin.h"
#include "qxplugin/qxplugincontext.h"
#include "qxplugin/qxpluginmanager.h"
#include "qxplugin/qxpluginspec.h"

#include "qxapp/qxappshell.h"

#include <QtCore/QJsonArray>
#include <QtCore/QJsonObject>
#include <QtCore/QStringList>
#include <QtCore/QVersionNumber>
#include <QtWidgets/QWidget>

QX_PLUGIN_USE_NAMESPACE
QX_APP_USE_NAMESPACE

/*! A controllable plugin: refuses to initialize, or reports an old interface. */
class FakePlugin : public QxPlugin
{
    Q_OBJECT
public:
    explicit FakePlugin(bool ok = true, int iface = 1)
        : QxPlugin(Q_NULLPTR)
        , m_ok(ok)
        , m_iface(iface)
    {
    }

    bool initialize(QxPluginContext *, QString *errorString) override
    {
        if (!m_ok) {
            if (errorString)
                *errorString = QStringLiteral("fake init refused");
            return false;
        }
        return true;
    }

    int interfaceVersion() const override
    {
        return m_iface;
    }

    bool m_ok;
    int m_iface;
};

/*! Builds a plugin.json-shaped metadata object, exactly what add_qtc_plugin produces. */
static QJsonObject metaData(const QString &name, const QString &version, const QStringList &deps = QStringList(),
                            bool enabledByDefault = true, const QString &compat = QString())
{
    QJsonObject o;
    o.insert(QStringLiteral("Name"), name);
    o.insert(QStringLiteral("Version"), version);
    if (!compat.isEmpty())
        o.insert(QStringLiteral("CompatVersion"), compat);
    o.insert(QStringLiteral("EnabledByDefault"), enabledByDefault);
    if (!deps.isEmpty()) {
        QJsonArray arr;
        for (const QString &dep : deps)
            arr.append(QJsonObject{{QStringLiteral("Name"), dep}});
        o.insert(QStringLiteral("Dependencies"), arr);
    }
    return o;
}

static QJsonObject metaDataOptional(const QString &name, const QString &version, const QString &optionalDep)
{
    QJsonObject o;
    o.insert(QStringLiteral("Name"), name);
    o.insert(QStringLiteral("Version"), version);
    QJsonArray arr;
    arr.append(
        QJsonObject{{QStringLiteral("Name"), optionalDep}, {QStringLiteral("Type"), QStringLiteral("optional")}});
    o.insert(QStringLiteral("Dependencies"), arr);
    return o;
}

class tst_QxPlugin : public QObject
{
    Q_OBJECT
private slots:
    void specReadsMetadata();
    void dependencyOrder();
    void missingDependency();
    void optionalDependencyMayBeAbsent();
    void circularDependency();
    void versionMismatch();
    void disabledByDefaultIsSkipped();
    void enabledListOverridesDefaultOff();
    void initializeFailureCascadesAndIsolates();
    void interfaceTooOldIsRejected();
    void hostContextAdapter();
    void pluginCannotReachTheManager();
    void shutdownReversesOrder();
};

void tst_QxPlugin::specReadsMetadata()
{
    QJsonObject o = metaData(QStringLiteral("alpha"), QStringLiteral("1.2.3"),
                             {QStringLiteral("beta"), QStringLiteral("gamma")}, true, QStringLiteral("1.0.0"));
    QxPluginSpec spec;
    QVERIFY(spec.read(QStringLiteral("/tmp/alpha.json"), o));
    QCOMPARE(spec.id(), QStringLiteral("alpha"));
    QCOMPARE(spec.version(), QVersionNumber(1, 2, 3));
    QCOMPARE(spec.compatVersion(), QVersionNumber(1, 0, 0));
    QCOMPARE(spec.dependencies(), QStringList({QStringLiteral("beta"), QStringLiteral("gamma")}));
    QCOMPARE(spec.isEnabledByDefault(), true);
    QCOMPARE(spec.state(), QxPluginState::Read);

    // optional dependency lands in its own list and never blocks resolution
    QxPluginSpec opt;
    QVERIFY(opt.read(QStringLiteral("/tmp/x.json"),
                     metaDataOptional(QStringLiteral("x"), QStringLiteral("1.0.0"), QStringLiteral("ghost"))));
    QCOMPARE(opt.dependencies(), QStringList());
    QCOMPARE(opt.optionalDependencies(), QStringList({QStringLiteral("ghost")}));
}

void tst_QxPlugin::dependencyOrder()
{
    QxPluginManager mgr;
    mgr.registerStaticPlugin(QStringLiteral("a"), metaData(QStringLiteral("a"), QStringLiteral("1.0.0")), []() {
        return new FakePlugin;
    });
    mgr.registerStaticPlugin(QStringLiteral("b"),
                             metaData(QStringLiteral("b"), QStringLiteral("1.0.0"), {QStringLiteral("a")}), []() {
                                 return new FakePlugin;
                             });
    mgr.registerStaticPlugin(QStringLiteral("c"),
                             metaData(QStringLiteral("c"), QStringLiteral("1.0.0"), {QStringLiteral("b")}), []() {
                                 return new FakePlugin;
                             });

    mgr.loadPlugins();
    QVERIFY(!mgr.hasError());

    QStringList order;
    for (QxPluginSpec *s : mgr.specs())
        order.append(s->id());
    QCOMPARE(order, QStringList({QStringLiteral("a"), QStringLiteral("b"), QStringLiteral("c")}));

    QCOMPARE(mgr.spec(QStringLiteral("c"))->state(), QxPluginState::Initialized);
    QVERIFY(mgr.plugin(QStringLiteral("a")) != Q_NULLPTR);
}

void tst_QxPlugin::missingDependency()
{
    QxPluginManager mgr;
    mgr.registerStaticPlugin(QStringLiteral("a"), metaData(QStringLiteral("a"), QStringLiteral("1.0.0")), []() {
        return new FakePlugin;
    });
    mgr.registerStaticPlugin(QStringLiteral("b"),
                             metaData(QStringLiteral("b"), QStringLiteral("1.0.0"), {QStringLiteral("ghost")}), []() {
                                 return new FakePlugin;
                             });

    mgr.loadPlugins();
    QVERIFY(mgr.hasError());

    // The missing dep is reported by name on the plugin that needed it.
    QxPluginSpec *b = mgr.spec(QStringLiteral("b"));
    QCOMPARE(b->state(), QxPluginState::Disabled);
    QVERIFY(b->error().contains(QStringLiteral("ghost")));
    QVERIFY(b->error().contains(QStringLiteral("missing required dependency")));

    // The independent plugin still runs - the host keeps starting.
    QCOMPARE(mgr.spec(QStringLiteral("a"))->state(), QxPluginState::Initialized);
    QVERIFY(mgr.plugin(QStringLiteral("a")) != Q_NULLPTR);
}

void tst_QxPlugin::optionalDependencyMayBeAbsent()
{
    QxPluginManager mgr;
    mgr.registerStaticPlugin(QStringLiteral("x"),
                             metaDataOptional(QStringLiteral("x"), QStringLiteral("1.0.0"), QStringLiteral("ghost")),
                             []() {
                                 return new FakePlugin;
                             });

    mgr.loadPlugins();
    QVERIFY(!mgr.hasError());
    QCOMPARE(mgr.spec(QStringLiteral("x"))->state(), QxPluginState::Initialized);
}

void tst_QxPlugin::circularDependency()
{
    QxPluginManager mgr;
    mgr.registerStaticPlugin(QStringLiteral("a"),
                             metaData(QStringLiteral("a"), QStringLiteral("1.0.0"), {QStringLiteral("b")}), []() {
                                 return new FakePlugin;
                             });
    mgr.registerStaticPlugin(QStringLiteral("b"),
                             metaData(QStringLiteral("b"), QStringLiteral("1.0.0"), {QStringLiteral("a")}), []() {
                                 return new FakePlugin;
                             });

    mgr.loadPlugins();
    QVERIFY(mgr.hasError());
    QVERIFY(mgr.spec(QStringLiteral("a"))->error().contains(QStringLiteral("circular dependency")));
    QVERIFY(mgr.spec(QStringLiteral("b"))->error().contains(QStringLiteral("circular dependency")));
    // The diagnostic names the participants rather than just "cycle detected".
    QVERIFY(mgr.errorString().contains(QStringLiteral("a -> b -> a")) ||
            mgr.errorString().contains(QStringLiteral("b -> a -> b")));
}

void tst_QxPlugin::versionMismatch()
{
    QxPluginManager mgr;
    mgr.setHostVersion(QVersionNumber(2, 0, 0));
    // Plugin only supports host >= 3.0.0.
    mgr.registerStaticPlugin(QStringLiteral("a"),
                             metaData(QStringLiteral("a"), QStringLiteral("4.0.0"), {}, true, QStringLiteral("3.0.0")),
                             []() {
                                 return new FakePlugin;
                             });

    mgr.loadPlugins();
    QVERIFY(mgr.hasError());
    QxPluginSpec *a = mgr.spec(QStringLiteral("a"));
    QCOMPARE(a->state(), QxPluginState::Disabled);
    QVERIFY(a->error().contains(QStringLiteral("requires host version in [3.0.0, 4.0.0]")));
    QVERIFY(a->error().contains(QStringLiteral("2.0.0")));
}

void tst_QxPlugin::disabledByDefaultIsSkipped()
{
    QxPluginManager mgr;
    mgr.registerStaticPlugin(QStringLiteral("a"), metaData(QStringLiteral("a"), QStringLiteral("1.0.0"), {}, false),
                             []() {
                                 return new FakePlugin;
                             });

    mgr.loadPlugins();
    // A skipped-by-default plugin is not a failure.
    QVERIFY(!mgr.hasError());
    QxPluginSpec *a = mgr.spec(QStringLiteral("a"));
    QCOMPARE(a->state(), QxPluginState::Disabled);
    QVERIFY(a->error().isEmpty());
    QVERIFY(mgr.plugin(QStringLiteral("a")) == Q_NULLPTR);
    QCOMPARE(mgr.specs().size(), 0);
}

void tst_QxPlugin::enabledListOverridesDefaultOff()
{
    const auto registerOptIn = [](QxPluginManager &mgr) {
        mgr.registerStaticPlugin(QStringLiteral("opt"),
                                 metaData(QStringLiteral("opt"), QStringLiteral("1.0.0"), {}, false), []() {
                                     return new FakePlugin;
                                 });
    };

    // Off by default and nobody asked for it: skipped, and skipping is not a failure.
    {
        QxPluginManager mgr;
        registerOptIn(mgr);
        mgr.loadPlugins();
        QVERIFY(!mgr.hasError());
        QCOMPARE(mgr.spec(QStringLiteral("opt"))->state(), QxPluginState::Disabled);
    }

    // Named in the enable list: the metadata default is overridden and it starts.
    {
        QxPluginManager mgr;
        mgr.setEnabledPlugins({QStringLiteral("opt")});
        QCOMPARE(mgr.enabledPlugins(), QStringList({QStringLiteral("opt")}));
        registerOptIn(mgr);
        mgr.loadPlugins();
        QVERIFY(!mgr.hasError());
        QCOMPARE(mgr.spec(QStringLiteral("opt"))->state(), QxPluginState::Initialized);
        QVERIFY(mgr.plugin(QStringLiteral("opt")) != Q_NULLPTR);
    }

    // In both lists: forced off wins, so the plugin stays off.
    {
        QxPluginManager mgr;
        mgr.setEnabledPlugins({QStringLiteral("opt")});
        mgr.setDisabledPlugins({QStringLiteral("opt")});
        registerOptIn(mgr);
        mgr.loadPlugins();
        QVERIFY(!mgr.hasError());
        QCOMPARE(mgr.spec(QStringLiteral("opt"))->state(), QxPluginState::Disabled);
    }

    // An enable list entry for an ordinary plugin changes nothing: it starts anyway.
    {
        QxPluginManager mgr;
        mgr.setEnabledPlugins({QStringLiteral("plain")});
        mgr.registerStaticPlugin(QStringLiteral("plain"), metaData(QStringLiteral("plain"), QStringLiteral("1.0.0")),
                                 []() {
                                     return new FakePlugin;
                                 });
        mgr.loadPlugins();
        QVERIFY(!mgr.hasError());
        QCOMPARE(mgr.spec(QStringLiteral("plain"))->state(), QxPluginState::Initialized);
    }
}

void tst_QxPlugin::initializeFailureCascadesAndIsolates()
{
    QxPluginManager mgr;
    mgr.registerStaticPlugin(QStringLiteral("base"), metaData(QStringLiteral("base"), QStringLiteral("1.0.0")), []() {
        return new FakePlugin(true);
    });
    mgr.registerStaticPlugin(QStringLiteral("broken"),
                             metaData(QStringLiteral("broken"), QStringLiteral("1.0.0"), {QStringLiteral("base")}),
                             []() {
                                 return new FakePlugin(false);
                             });
    mgr.registerStaticPlugin(QStringLiteral("child"),
                             metaData(QStringLiteral("child"), QStringLiteral("1.0.0"), {QStringLiteral("broken")}),
                             []() {
                                 return new FakePlugin(true);
                             });

    mgr.loadPlugins();
    QVERIFY(mgr.hasError());

    // base succeeded: failure isolation keeps the run going.
    QCOMPARE(mgr.spec(QStringLiteral("base"))->state(), QxPluginState::Initialized);
    QVERIFY(mgr.plugin(QStringLiteral("base")) != Q_NULLPTR);

    // broken refused initialize.
    QxPluginSpec *broken = mgr.spec(QStringLiteral("broken"));
    QCOMPARE(broken->state(), QxPluginState::Disabled);
    QCOMPARE(broken->error(), QStringLiteral("fake init refused"));

    // child is disabled because its required dependency did not start.
    QxPluginSpec *child = mgr.spec(QStringLiteral("child"));
    QCOMPARE(child->state(), QxPluginState::Disabled);
    QVERIFY(child->error().contains(QStringLiteral("broken")));
    QVERIFY(child->error().contains(QStringLiteral("not available")));
}

void tst_QxPlugin::interfaceTooOldIsRejected()
{
    QxPluginManager mgr;
    mgr.setRequiredInterfaceVersion(1);
    mgr.registerStaticPlugin(QStringLiteral("old"), metaData(QStringLiteral("old"), QStringLiteral("1.0.0")), []() {
        return new FakePlugin(true, 0);
    });

    mgr.loadPlugins();
    QVERIFY(mgr.hasError());
    QxPluginSpec *old = mgr.spec(QStringLiteral("old"));
    QCOMPARE(old->state(), QxPluginState::Disabled);
    QVERIFY(old->error().contains(QStringLiteral("interface version 0")));
    QVERIFY(old->error().contains(QStringLiteral("required 1")));
    // Rejected before initialize(), so the instance was never asked to start.
    QVERIFY(old->error().contains(QStringLiteral("below the required")));
}

void tst_QxPlugin::hostContextAdapter()
{
    // The host (QxAppShell) implements QxPluginContext; a plugin programs only
    // against the context and never sees the window class.
    QxAppShell shell;
    QxPluginManager mgr;
    mgr.setContext(shell.pluginContext());
    mgr.registerStaticPlugin(QStringLiteral("plug"), metaData(QStringLiteral("plug"), QStringLiteral("1.0.0")),
                             [this]() {
                                 Q_UNUSED(this);
                                 class Plug : public FakePlugin
                                 {
                                 public:
                                     bool initialize(QxPluginContext *ctx, QString *) override
                                     {
                                         QWidget *page = new QWidget;
                                         ctx->addPage(QStringLiteral("plug"), QIcon(), QStringLiteral("Plugin"), page);
                                         ctx->addDock(QxPluginContext::BottomDock, QStringLiteral("plugout"),
                                                      QStringLiteral("Out"), new QWidget);
                                         ctx->setStatusMessage(QStringLiteral("plugin loaded"));
                                         ctx->showToast(QStringLiteral("hi"), QxPluginContext::ToastLevel::Success);
                                         // settings() must be reachable through the framework-neutral context
                                         if (ctx->settings() == Q_NULLPTR)
                                             return false;
                                         return true;
                                     }
                                 };
                                 return new Plug;
                             });

    mgr.loadPlugins();
    QVERIFY(!mgr.hasError());
    QCOMPARE(mgr.spec(QStringLiteral("plug"))->state(), QxPluginState::Initialized);
    QCOMPARE(shell.currentPageId(), QStringLiteral("plug"));
    QVERIFY(shell.dock(QStringLiteral("plugout")) != Q_NULLPTR);
    QCOMPARE(shell.statusMessage(), QStringLiteral("plugin loaded"));
}

/*!
 * QxPluginContext is the whole of what a plugin is handed, so a plugin must not
 * be able to reach the manager and, through it, its peers. Keeping the promise
 * is a runtime property: an instance that is parented to the manager can cast
 * its own parent() back, which would quietly undo the contract for static
 * builds. Ownership has to survive that, so the parent is checked to exist and
 * to be somebody else.
 */
void tst_QxPlugin::pluginCannotReachTheManager()
{
    QxAppShell shell;
    QxPluginManager mgr;
    mgr.setContext(shell.pluginContext());
    mgr.registerStaticPlugin(QStringLiteral("a"), metaData(QStringLiteral("a"), QStringLiteral("1.0.0")), []() {
        return new FakePlugin;
    });
    mgr.registerStaticPlugin(QStringLiteral("b"),
                             metaData(QStringLiteral("b"), QStringLiteral("1.0.0"), {QStringLiteral("a")}), []() {
                                 return new FakePlugin;
                             });

    mgr.loadPlugins();
    QVERIFY(!mgr.hasError());

    // The leading :: is required here for the same reason the modules need it:
    // this file has "using namespace QxPlugin", so a bare QxPlugin is ambiguous
    // between the namespace and the class it holds.
    ::QxPlugin::QxPlugin *a = mgr.plugin(QStringLiteral("a"));
    QVERIFY(a != Q_NULLPTR);

    // Still owned - dropping the parent instead of replacing it would leak.
    QVERIFY(a->parent() != Q_NULLPTR);
    // ... but not by the manager, and not by anything on the way to it.
    QVERIFY(qobject_cast<QxPluginManager *>(a->parent()) == Q_NULLPTR);
    for (QObject *ancestor = a->parent(); ancestor != Q_NULLPTR; ancestor = ancestor->parent()) {
        QVERIFY(ancestor != &mgr);
    }
    // Nothing in that chain hands out the peers either.
    for (QObject *ancestor = a->parent(); ancestor != Q_NULLPTR; ancestor = ancestor->parent()) {
        QVERIFY(qobject_cast<QxPluginManager *>(ancestor) == Q_NULLPTR);
    }

    // The one handle it did get is the context the host handed over, unchanged.
    QCOMPARE(a->context(), shell.pluginContext());
}

void tst_QxPlugin::shutdownReversesOrder()
{
    QxPluginManager mgr;
    mgr.registerStaticPlugin(QStringLiteral("a"), metaData(QStringLiteral("a"), QStringLiteral("1.0.0")), []() {
        return new FakePlugin;
    });
    mgr.registerStaticPlugin(QStringLiteral("b"),
                             metaData(QStringLiteral("b"), QStringLiteral("1.0.0"), {QStringLiteral("a")}), []() {
                                 return new FakePlugin;
                             });
    mgr.loadPlugins();
    QVERIFY(!mgr.hasError());

    mgr.shutdown();
    QCOMPARE(mgr.spec(QStringLiteral("a"))->state(), QxPluginState::Stopped);
    QCOMPARE(mgr.spec(QStringLiteral("b"))->state(), QxPluginState::Stopped);
}

TEST_ADD(tst_QxPlugin)

#include "tst_qxplugin.moc"
