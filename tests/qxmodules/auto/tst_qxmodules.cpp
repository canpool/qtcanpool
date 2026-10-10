#include "tst_global.h"

#include "qxapp/qxappshell.h"
#include "qxdock/dockwidget.h"
#include "qxplugin/qxplugin.h"
#include "qxplugin/qxpluginmanager.h"
#include "qxplugin/qxpluginspec.h"

#include <QtCore/QStringList>
#include <QtWidgets/QPlainTextEdit>

QX_APP_USE_NAMESPACE
QX_DOCK_USE_NAMESPACE
QX_PLUGIN_USE_NAMESPACE

/*
 * The module samples are plugins, so the only honest way to test them is the
 * way a host uses them: point a manager at the directory they were built into
 * and let it load them. tests/CMakeLists.txt bakes that directory in, because
 * the framework's plugin layout is per platform and guessing it would test the
 * guess rather than the modules.
 *
 * What is checked is what a module can promise - that its metadata arrived, that
 * a declared dependency ordered the run, and that initialize() really put
 * something on the shell. What a module looks like is its own business.
 */

namespace
{
/*! The directory the sample modules were built into. */
QString modulePluginDir()
{
    return QStringLiteral(QXMODULES_PLUGIN_DIR);
}

/*! A shell with the modules loaded into it, in a state a test can ask about. */
void loadModules(QxAppShell *shell, QxPluginManager *manager)
{
    manager->setPluginPaths({modulePluginDir()});
    manager->setContext(shell->pluginContext());
    manager->loadPlugins();
}

int indexOf(const QList<QxPluginSpec *> &specs, const QString &id)
{
    for (int i = 0; i < specs.count(); ++i) {
        if (specs.at(i)->id() == id) {
            return i;
        }
    }
    return -1;
}
}   // namespace

class tst_QxModules : public QObject
{
    Q_OBJECT
private slots:
    void discoversTheModuleMetadata();
    void dependencyOrdersTheRun();
    void modulesFillTheShell();
    void publicApiIsReachableByName();
};

/*! The three modules are found on disk, read, and initialized without help. */
void tst_QxModules::discoversTheModuleMetadata()
{
    QxAppShell shell;
    QxPluginManager manager;
    loadModules(&shell, &manager);

    QCOMPARE(manager.hasError(), false);
    QCOMPARE(manager.errorString(), QString());
    QCOMPARE(manager.allSpecs().count(), 3);

    for (const QString &id : {QStringLiteral("output"), QStringLiteral("notebook"), QStringLiteral("filetree")}) {
        QxPluginSpec *spec = manager.spec(id);
        QVERIFY2(spec != Q_NULLPTR, qPrintable(id));
        QCOMPARE(spec->state(), QxPluginState::Initialized);
        QCOMPARE(spec->vendor(), QStringLiteral("QtCanpool"));
        QCOMPARE(spec->isEnabledByDefault(), true);
        QVERIFY(!spec->description().isEmpty());
        QVERIFY(!spec->filePath().isEmpty());
    }

    // The dependency edge is metadata, not code: filetree's PLUGIN_DEPENDS wrote
    // it into the generated plugin.json, and the manager read it from there.
    QCOMPARE(manager.spec(QStringLiteral("output"))->dependencies(), QStringList());
    QCOMPARE(manager.spec(QStringLiteral("filetree"))->dependencies(), QStringList({QStringLiteral("output")}));
}

/*! A required dependency is initialized first, whatever order the files turn up in. */
void tst_QxModules::dependencyOrdersTheRun()
{
    QxAppShell shell;
    QxPluginManager manager;
    loadModules(&shell, &manager);

    const QList<QxPluginSpec *> specs = manager.specs();
    QCOMPARE(specs.count(), 3);

    const int outputAt = indexOf(specs, QStringLiteral("output"));
    const int fileTreeAt = indexOf(specs, QStringLiteral("filetree"));
    QVERIFY(outputAt >= 0);
    QVERIFY(fileTreeAt >= 0);
    QVERIFY2(outputAt < fileTreeAt, "filetree requires output, so output has to be initialized first");
}

/*! What the modules put on the shell is really there, not just reported. */
void tst_QxModules::modulesFillTheShell()
{
    QxAppShell shell;
    QxPluginManager manager;
    loadModules(&shell, &manager);

    // output: a dock of its own and a line on the status bar
    QVERIFY(shell.dock(QStringLiteral("output")) != Q_NULLPTR);
    QCOMPARE(shell.statusMessage(), QStringLiteral("Output panel ready"));

    // notebook: one page in the central workspace, addressable by its id
    QVERIFY(shell.page(QStringLiteral("notebook")) != Q_NULLPTR);
    QCOMPARE(shell.pageCount(), 1);
    shell.setCurrentPage(QStringLiteral("notebook"));
    QCOMPARE(shell.currentPageId(), QStringLiteral("notebook"));

    // filetree: a dock as well, and a different area from output's
    QVERIFY(shell.dock(QStringLiteral("filetree")) != Q_NULLPTR);
}

/*!
 * The two ends of the wiring a host can make without any module header: filetree
 * says what it opened, output takes a line. A plugin holds no handle to its
 * peers, so connecting them is the host's job - but nothing new is needed for
 * it, because both ends are already reachable through the meta-object.
 */
void tst_QxModules::publicApiIsReachableByName()
{
    QxAppShell shell;
    QxPluginManager manager;
    loadModules(&shell, &manager);

    QObject *output = manager.plugin(QStringLiteral("output"));
    QObject *fileTree = manager.plugin(QStringLiteral("filetree"));
    QVERIFY(output != Q_NULLPTR);
    QVERIFY(fileTree != Q_NULLPTR);

    QVERIFY(fileTree->metaObject()->indexOfSignal("fileActivated(QString)") >= 0);
    QVERIFY(output->metaObject()->indexOfMethod("appendLine(QString)") >= 0);

    // The host wires the two, by name and without either module's header.
    QVERIFY(QObject::connect(fileTree, SIGNAL(fileActivated(QString)), output, SLOT(appendLine(QString))));

    QPlainTextEdit *view = shell.findChild<QPlainTextEdit *>(QStringLiteral("outputView"));
    QVERIFY(view != Q_NULLPTR);
    const int linesBefore = view->toPlainText().count(QLatin1Char('\n'));

    QVERIFY(QMetaObject::invokeMethod(fileTree, "activateFile", Q_ARG(QString, QStringLiteral("src/main.cpp"))));

    QCOMPARE(view->toPlainText().count(QLatin1Char('\n')), linesBefore + 1);
    QVERIFY(view->toPlainText().contains(QStringLiteral("src/main.cpp")));
}

TEST_ADD(tst_QxModules)

#include "tst_qxmodules.moc"
