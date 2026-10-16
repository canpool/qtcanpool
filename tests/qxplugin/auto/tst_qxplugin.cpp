#include "tst_global.h"

#include "qxplugin/qxplugin.h"
#include "qxplugin/qxplugincontext.h"
#include "qxplugin/qxpluginmanager.h"
#include "qxplugin/qxobjectpool.h"
#include "qxplugin/qxpluginspec.h"

#include "qxapp/qxappshell.h"

#include <QtCore/QJsonArray>
#include <QtCore/QJsonObject>
#include <QtCore/QStringList>
#include <QtCore/QVersionNumber>
#include <QtTest/QSignalSpy>
#include <QtWidgets/QWidget>

QX_PLUGIN_USE_NAMESPACE
QX_APP_USE_NAMESPACE

// Needed by QSignalSpy on Qt 5, and by nothing else. The spy stores each signal
// argument in a QVariant, so reading one back with value<QxPluginSpec *>() needs
// the argument type to be a declared metatype - and QxPluginSpec is not a QObject,
// so Qt 5 has nothing to infer it from and refuses to compile. Qt 6 does not need
// this: moc generates a QMetaType interface for every parameter type it sees, so
// the same line compiles there unregistered. Declared in the test rather than in
// qxpluginspec.h because the test is what round-trips through QVariant; the header
// is frozen for this release.
//
// Unqualified on purpose: Q_DECLARE_METATYPE registers the type under the spelling
// it is handed, and Qt 5's QSignalSpy looks a signal argument up by the name moc
// wrote into the meta-object - the signal is declared inside the namespace, so that
// name is "QxPluginSpec*". Qualifying this would register "QxPlugin::QxPluginSpec*"
// instead, which never matches, and the spy would silently stop reading arguments.
Q_DECLARE_METATYPE(QxPluginSpec *)

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

/*! Something a plugin could publish, for the typed half of the pool lookup. */
class FakeService : public QObject
{
    Q_OBJECT
public:
    int answer() const
    {
        return 42;
    }
};

/*!
 * A plugin that writes down what it was told to do, so the order of the calls
 * can be asserted rather than assumed. initialize(), extensionsInitialized()
 * and shutdown() are three separate moments of the same life, and the library
 * makes a claim about the order of each.
 */
class OrderPlugin : public QxPlugin
{
    Q_OBJECT
public:
    OrderPlugin(QStringList *log, const QString &name)
        : QxPlugin(Q_NULLPTR)
        , m_log(log)
        , m_name(name)
    {
    }

    bool initialize(QxPluginContext *, QString *) override
    {
        m_log->append(m_name + QStringLiteral("-init"));
        return true;
    }

    void extensionsInitialized() override
    {
        m_log->append(m_name + QStringLiteral("-extend"));
    }

    void shutdown() override
    {
        m_log->append(m_name + QStringLiteral("-stop"));
    }
private:
    QStringList *m_log;
    QString m_name;
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
    void optionalDependencyOrdersTheRun();
    void objectPoolPublishesAndFinds();
    void circularDependency();
    void versionMismatch();
    void disabledByDefaultIsSkipped();
    void enabledListOverridesDefaultOff();
    void initializeFailureCascadesAndIsolates();
    void interfaceTooOldIsRejected();
    void hostContextAdapter();
    void pluginCannotReachTheManager();
    void shutdownReversesOrder();
    void signalsReportTheRun();
    void extensionsInitializedFollowsInitializationOrder();
    void managerAccessorsRoundTrip();
    void dependencyGraph();
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

/*!
 * An optional dependency that is really there still orders the run, and that is
 * not a detail: the wanting side can only look up what the providing side has
 * already published, so "soft" has to mean "sorts when present" rather than
 * "ignored". What it must never do is drag the dependent down with it - that is
 * the whole difference from a required dependency, and it is what lets a plugin
 * recommend one that the user is free to switch off.
 */
void tst_QxPlugin::optionalDependencyOrdersTheRun()
{
    const auto registerBoth = [](QxPluginManager &mgr) {
        // Registered the wrong way round on purpose: only the metadata can put
        // them in the right order.
        mgr.registerStaticPlugin(QStringLiteral("b"),
                                 metaDataOptional(QStringLiteral("b"), QStringLiteral("1.0.0"), QStringLiteral("a")),
                                 []() {
                                     return new FakePlugin;
                                 });
        mgr.registerStaticPlugin(QStringLiteral("a"), metaData(QStringLiteral("a"), QStringLiteral("1.0.0")), []() {
            return new FakePlugin;
        });
    };

    // Present: initialized after its optional dependency, like a required edge.
    {
        QxPluginManager mgr;
        registerBoth(mgr);
        mgr.loadPlugins();
        QVERIFY(!mgr.hasError());
        QCOMPARE(mgr.spec(QStringLiteral("a"))->state(), QxPluginState::Initialized);

        QStringList order;
        for (QxPluginSpec *s : mgr.specs())
            order.append(s->id());
        QCOMPARE(order, QStringList({QStringLiteral("a"), QStringLiteral("b")}));
    }

    // Absent: the declaration blocks nothing at all, which is exactly how an
    // optional dependency differs from one that was never declared.
    {
        QxPluginManager mgr;
        mgr.registerStaticPlugin(
            QStringLiteral("b"),
            metaDataOptional(QStringLiteral("b"), QStringLiteral("1.0.0"), QStringLiteral("ghost")), []() {
                return new FakePlugin;
            });
        mgr.loadPlugins();
        QVERIFY(!mgr.hasError());
        QCOMPARE(mgr.spec(QStringLiteral("b"))->state(), QxPluginState::Initialized);
    }

    // Present but switched off: no edge, and above all no cascade. b starts even
    // though the plugin it recommended is not running.
    {
        QxPluginManager mgr;
        mgr.setDisabledPlugins({QStringLiteral("a")});
        registerBoth(mgr);
        mgr.loadPlugins();
        QVERIFY(!mgr.hasError());
        QCOMPARE(mgr.spec(QStringLiteral("a"))->state(), QxPluginState::Disabled);
        QCOMPARE(mgr.spec(QStringLiteral("b"))->state(), QxPluginState::Initialized);
    }
}

/*!
 * The pool is where two plugins meet when neither has the other's header: one
 * publishes an object under a name, the other asks for it and gets back a
 * QObject. Everything asserted here is about that being enough - and about the
 * pool never outliving what was put in it.
 */
void tst_QxPlugin::objectPoolPublishesAndFinds()
{
    QxAppShell shell;
    QxPluginContext *context = shell.pluginContext();
    QxObjectPool *pool = shell.pluginPool();
    QVERIFY(context != Q_NULLPTR);
    QVERIFY(pool != Q_NULLPTR);

    QVERIFY(context->objects().isEmpty());
    QVERIFY(context->objectByName(QStringLiteral("nobody")) == Q_NULLPTR);
    QVERIFY(context->object<FakeService>() == Q_NULLPTR);

    QSignalSpy added(pool, &QxObjectPool::objectAdded);
    QSignalSpy removed(pool, &QxObjectPool::aboutToRemoveObject);

    auto *service = new FakeService;
    service->setObjectName(QStringLiteral("service"));
    context->addObject(service);
    QCOMPARE(added.count(), 1);
    QCOMPARE(context->objects().count(), 1);
    QVERIFY(context->objects().first() == service);
    QVERIFY(context->objectByName(QStringLiteral("service")) == service);
    QVERIFY(context->objectByName(QString()) == Q_NULLPTR);
    // The same object reached by type: when the two ends do share a declaration,
    // neither of them has to know the name.
    QVERIFY(context->object<FakeService>() == service);
    QCOMPARE(context->object<FakeService>()->answer(), 42);

    context->addObject(service);   // publishing twice changes nothing
    context->addObject(Q_NULLPTR);
    QCOMPARE(context->objects().count(), 1);
    QCOMPARE(added.count(), 1);

    context->removeObject(service);
    QCOMPARE(removed.count(), 1);
    QCOMPARE(context->objects().count(), 0);
    context->removeObject(service);   // already gone: no-op, no second signal
    QCOMPARE(removed.count(), 1);
    delete service;

    // An object that is simply destroyed leaves by itself. The pool holds it
    // through a guarded pointer, so a plugin that is unloaded mid-session cannot
    // leave a dangling entry behind - and nobody has to remember anything for
    // that to be true.
    auto *doomed = new FakeService;
    doomed->setObjectName(QStringLiteral("doomed"));
    context->addObject(doomed);
    QCOMPARE(context->objects().count(), 1);
    delete doomed;
    QCOMPARE(context->objects().count(), 0);
    QVERIFY(context->objectByName(QStringLiteral("doomed")) == Q_NULLPTR);
    QCOMPARE(added.count(), 2);
    QCOMPARE(removed.count(), 1);   // destroyed, not removed: nothing to announce
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
    QStringList log;
    QxPluginManager mgr;
    mgr.registerStaticPlugin(QStringLiteral("a"), metaData(QStringLiteral("a"), QStringLiteral("1.0.0")), [&log]() {
        return new OrderPlugin(&log, QStringLiteral("a"));
    });
    mgr.registerStaticPlugin(QStringLiteral("b"),
                             metaData(QStringLiteral("b"), QStringLiteral("1.0.0"), {QStringLiteral("a")}), [&log]() {
                                 return new OrderPlugin(&log, QStringLiteral("b"));
                             });
    mgr.loadPlugins();
    QVERIFY(!mgr.hasError());

    log.clear();
    mgr.shutdown();

    // The promise the name makes: teardown walks the initialization order
    // backwards, so a plugin is torn down after everything that needed it.
    QCOMPARE(log, QStringList({QStringLiteral("b-stop"), QStringLiteral("a-stop")}));
    QCOMPARE(mgr.spec(QStringLiteral("a"))->state(), QxPluginState::Stopped);
    QCOMPARE(mgr.spec(QStringLiteral("b"))->state(), QxPluginState::Stopped);
}

/*!
 * pluginInitialized() and pluginFailed() are the only two things a manager UI
 * can connect to, so what they report has to be worth reading: one call per
 * plugin that started, one per plugin that did not, and the failure carries the
 * same reason the spec holds. A plugin that was merely skipped by default did
 * not fail and is not announced at all - "did not run" and "went wrong" are
 * different answers, and only one of them deserves a notification.
 */
void tst_QxPlugin::signalsReportTheRun()
{
    QxPluginManager mgr;
    mgr.registerStaticPlugin(QStringLiteral("ok"), metaData(QStringLiteral("ok"), QStringLiteral("1.0.0")), []() {
        return new FakePlugin(true);
    });
    mgr.registerStaticPlugin(QStringLiteral("bad"), metaData(QStringLiteral("bad"), QStringLiteral("1.0.0")), []() {
        return new FakePlugin(false);
    });
    mgr.registerStaticPlugin(QStringLiteral("opt"), metaData(QStringLiteral("opt"), QStringLiteral("1.0.0"), {}, false),
                             []() {
                                 return new FakePlugin(true);
                             });

    // Qt 5 only. Q_DECLARE_METATYPE registers the type lazily, on the first request
    // for its metatype id, and the spies below are that first request - too late,
    // because they resolve their arguments while being constructed. Ask for the id
    // now. Qt 6 reads the type straight off the meta-object and needs none of this.
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    qRegisterMetaType<QxPluginSpec *>();
#endif

    QSignalSpy initialized(&mgr, &QxPluginManager::pluginInitialized);
    QSignalSpy failed(&mgr, &QxPluginManager::pluginFailed);

    mgr.loadPlugins();

    QCOMPARE(initialized.count(), 1);
    QCOMPARE(failed.count(), 1);

    QxPluginSpec *okSpec = mgr.spec(QStringLiteral("ok"));
    QCOMPARE(initialized.first().at(0).value<QxPluginSpec *>(), okSpec);

    QxPluginSpec *badSpec = mgr.spec(QStringLiteral("bad"));
    QCOMPARE(failed.first().at(0).value<QxPluginSpec *>(), badSpec);
    QCOMPARE(failed.first().at(1).toString(), badSpec->error());

    // Skipped by default: disabled, but neither a failure nor an announcement.
    QxPluginSpec *optSpec = mgr.spec(QStringLiteral("opt"));
    QCOMPARE(optSpec->state(), QxPluginState::Disabled);
    QVERIFY(optSpec->error().isEmpty());
}

/*!
 * extensionsInitialized() is a second pass, not a per-plugin callback: the
 * whole set is up before any of them is told to finish. That is what makes it
 * the place to publish something that depends on every peer being initialized,
 * so the order is part of the contract - and it is not a reverse-order
 * guarantee, which is worth pinning precisely because Qt Creator's is.
 */
void tst_QxPlugin::extensionsInitializedFollowsInitializationOrder()
{
    QStringList log;
    QxPluginManager mgr;
    // Registered the wrong way round on purpose: only the dependency can put
    // them in this order.
    mgr.registerStaticPlugin(QStringLiteral("b"),
                             metaData(QStringLiteral("b"), QStringLiteral("1.0.0"), {QStringLiteral("a")}), [&log]() {
                                 return new OrderPlugin(&log, QStringLiteral("b"));
                             });
    mgr.registerStaticPlugin(QStringLiteral("a"), metaData(QStringLiteral("a"), QStringLiteral("1.0.0")), [&log]() {
        return new OrderPlugin(&log, QStringLiteral("a"));
    });

    mgr.loadPlugins();
    QVERIFY(!mgr.hasError());

    // Both initialize before either extends.
    QCOMPARE(log, QStringList({QStringLiteral("a-init"), QStringLiteral("b-init"), QStringLiteral("a-extend"),
                               QStringLiteral("b-extend")}));
}

/*!
 * The accessors that tell a caller what the manager is working with. They have
 * no observable effect on loadPlugins() beyond what they configure, which is
 * exactly why nothing was pinning them; the defaults matter to a caller that
 * never sets them, and hasHostVersion() is how "accept anything" is told apart
 * from "accept up to here".
 */
void tst_QxPlugin::managerAccessorsRoundTrip()
{
    QxPluginManager mgr;

    QCOMPARE(mgr.requiredInterfaceVersion(), 1);
    mgr.setRequiredInterfaceVersion(3);
    QCOMPARE(mgr.requiredInterfaceVersion(), 3);

    QCOMPARE(mgr.hasHostVersion(), false);
    QCOMPARE(mgr.pluginPaths(), QStringList());
    mgr.setPluginPaths({QStringLiteral("/tmp/nowhere")});
    QCOMPARE(mgr.pluginPaths(), QStringList({QStringLiteral("/tmp/nowhere")}));

    mgr.setHostVersion(QVersionNumber(4, 1, 0));
    QCOMPARE(mgr.hasHostVersion(), true);
    QCOMPARE(mgr.hostVersion(), QVersionNumber(4, 1, 0));
}

TEST_ADD(tst_QxPlugin)

/*!
 * E5: the dependency graph is queryable, and before anything is loaded.
 *
 * The manager has always resolved the graph - topologically, with cycles
 * rejected - but only as a step of loadPlugins(), so the answer existed only
 * afterwards and only as a load order. A window that has to warn "turning this
 * off also disables those" needs it beforehand.
 */
void tst_QxPlugin::dependencyGraph()
{
    QxPluginManager mgr;
    // a <- b <- c, plus d which nothing touches, plus a plugin asking for
    // something that is not there.
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
    mgr.registerStaticPlugin(QStringLiteral("d"), metaData(QStringLiteral("d"), QStringLiteral("1.0.0")), []() {
        return new FakePlugin;
    });
    mgr.registerStaticPlugin(QStringLiteral("e"),
                             metaData(QStringLiteral("e"), QStringLiteral("1.0.0"), {QStringLiteral("ghost")}), []() {
                                 return new FakePlugin;
                             });

    // Nothing has been loaded yet, and the graph is already readable.
    QCOMPARE(mgr.specs(), QList<QxPluginSpec *>());
    QCOMPARE(mgr.dependenciesOf(QStringLiteral("c")), QStringList({QStringLiteral("b")}));
    QCOMPARE(mgr.dependenciesOf(QStringLiteral("a")), QStringList());
    QCOMPARE(mgr.dependenciesOf(QStringLiteral("nope")), QStringList());

    // Direct dependents, and the closure that would go down with a.
    QCOMPARE(mgr.dependentsOf(QStringLiteral("b")), QStringList({QStringLiteral("c")}));
    QCOMPARE(mgr.dependentsOf(QStringLiteral("a")), QStringList({QStringLiteral("b")}));
    QCOMPARE(mgr.dependentsOf(QStringLiteral("a"), true), QStringList({QStringLiteral("b"), QStringLiteral("c")}));
    // Nothing depends on d, and a plugin is never its own dependent.
    QCOMPARE(mgr.dependentsOf(QStringLiteral("d")), QStringList());
    QCOMPARE(mgr.dependentsOf(QStringLiteral("d"), true), QStringList());
    QCOMPARE(mgr.dependentsOf(QStringLiteral("a"), true).contains(QStringLiteral("a")), false);

    // The dependency nobody has is sayable before the load refuses it.
    QCOMPARE(mgr.missingDependenciesOf(QStringLiteral("e")), QStringList({QStringLiteral("ghost")}));
    QCOMPARE(mgr.missingDependenciesOf(QStringLiteral("c")), QStringList());
    QCOMPARE(mgr.missingDependenciesOf(QStringLiteral("nope")), QStringList());

    // The closure is the one the load actually applies: e and c are refused or
    // cascaded, and what stops is not asserted here - initializeFailureCascades
    // covers that - only that the graph still reads the same afterwards.
    mgr.loadPlugins();
    QCOMPARE(mgr.dependentsOf(QStringLiteral("a"), true), QStringList({QStringLiteral("b"), QStringLiteral("c")}));
    QCOMPARE(mgr.missingDependenciesOf(QStringLiteral("e")), QStringList({QStringLiteral("ghost")}));
}

#include "tst_qxplugin.moc"
