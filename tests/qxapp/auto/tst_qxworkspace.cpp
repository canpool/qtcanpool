#include "tst_global.h"

#include "qxapp/qxappshell.h"
#include "qxapp/qxworkspacemanager.h"

#include "qxcore/qxsettings.h"
#include "qxdock/dockwidget.h"
#include "qxdock/dockwindow.h"

#include <QtCore/QTemporaryDir>
#include <QtCore/QVariant>
#include <QtWidgets/QPlainTextEdit>
#include <QtWidgets/QTreeWidget>

QX_APP_USE_NAMESPACE
QX_CORE_USE_NAMESPACE
QX_DOCK_USE_NAMESPACE

/*!
 * The named layouts of a shell.
 *
 * Every case runs against a real settings file in a temporary directory,
 * because the whole point of a workspace is that it outlives the process: a
 * stand-in storage would let a bug hide where the manager keeps state in memory
 * instead of writing it out.
 *
 * The cases that apply a workspace show the shell first. Restoring a dock state
 * hides and shows the dock window on the way, and a widget that never owned a
 * platform window has no visibility to talk about.
 */
class tst_QxWorkspaceManager : public QObject
{
    Q_OBJECT
private slots:
    void initTestCase();

    void withoutAShell();
    void emptyManager();
    void saving();
    void savingAgainOverwrites();
    void namelessIsRefused();
    void applyingBringsTheLayoutBack();
    void unknownWorkspace();
    void renaming();
    void renamingOntoAnExistingOne();
    void removing();
    void removingTheCurrentOne();
    void clearing();
    void signalsOnEveryChange();
    void shellAccessors();
    void theFileIsTheStorage();
private:
    /*! A settings file of its own per case, so no case depends on another. */
    QString newSettingsFile();
    /*! A shell with two pages and two docks, shown and ready to be arranged. */
    QxAppShell *newShell(const QString &file);

    QTemporaryDir m_dir;
    int m_sequence = 0;
};

void tst_QxWorkspaceManager::initTestCase()
{
    QVERIFY(m_dir.isValid());
}

QString tst_QxWorkspaceManager::newSettingsFile()
{
    return m_dir.filePath(QStringLiteral("workspace-%1.ini").arg(++m_sequence));
}

QxAppShell *tst_QxWorkspaceManager::newShell(const QString &file)
{
    QxAppShell *shell = new QxAppShell;
    shell->setSettings(new QxSettings(file, QSettings::IniFormat));
    shell->addPage(QStringLiteral("one"), QIcon(), QStringLiteral("One"), new QWidget);
    shell->addPage(QStringLiteral("two"), QIcon(), QStringLiteral("Two"), new QWidget);
    shell->addDock(Qx::LeftDockWidgetArea, QStringLiteral("explorer"), QStringLiteral("Explorer"), new QTreeWidget);
    shell->addDock(Qx::BottomDockWidgetArea, QStringLiteral("output"), QStringLiteral("Output"), new QPlainTextEdit);
    shell->show();
    return shell;
}

void tst_QxWorkspaceManager::withoutAShell()
{
    QxWorkspaceManager manager(Q_NULLPTR);

    QVERIFY(manager.shell() == Q_NULLPTR);
    QVERIFY(manager.settings() == Q_NULLPTR);
    QCOMPARE(manager.count(), 0);
    QCOMPARE(manager.workspaceNames(), QStringList());
    QCOMPARE(manager.contains(QStringLiteral("one")), false);
    QCOMPARE(manager.currentWorkspace(), QString());

    // Without a shell there is nothing to arrange and nowhere to store it, so
    // every write refuses instead of failing quietly.
    QCOMPARE(manager.saveWorkspace(QStringLiteral("one")), false);
    QCOMPARE(manager.applyWorkspace(QStringLiteral("one")), false);
    QCOMPARE(manager.removeWorkspace(QStringLiteral("one")), false);
    QCOMPARE(manager.renameWorkspace(QStringLiteral("one"), QStringLiteral("two")), false);
    manager.clearWorkspaces();
    QCOMPARE(manager.count(), 0);
}

void tst_QxWorkspaceManager::emptyManager()
{
    QScopedPointer<QxAppShell> shell(newShell(newSettingsFile()));
    QxWorkspaceManager *manager = shell->workspaceManager();

    QCOMPARE(manager->shell(), shell.data());
    QVERIFY(manager->settings() != Q_NULLPTR);
    QCOMPARE(manager->count(), 0);
    QCOMPARE(manager->workspaceNames(), QStringList());
    QCOMPARE(manager->currentWorkspace(), QString());
}

void tst_QxWorkspaceManager::saving()
{
    QScopedPointer<QxAppShell> shell(newShell(newSettingsFile()));
    QxWorkspaceManager *manager = shell->workspaceManager();

    QCOMPARE(manager->saveWorkspace(QStringLiteral("Writing")), true);
    QCOMPARE(manager->saveWorkspace(QStringLiteral("Review")), true);

    // The order a menu wants is the order they were saved in, which is why it
    // is kept in a list of its own: childGroups() comes back in whatever order
    // the backend likes.
    const QStringList expected{QStringLiteral("Writing"), QStringLiteral("Review")};
    QCOMPARE(manager->workspaceNames(), expected);
    QCOMPARE(manager->count(), 2);
    QCOMPARE(manager->contains(QStringLiteral("Writing")), true);
    QCOMPARE(manager->contains(QStringLiteral("nope")), false);

    // Saving is also picking: what is on screen is what the name now means.
    QCOMPARE(manager->currentWorkspace(), QStringLiteral("Review"));

    // The name is trimmed, so a menu entry built from user input and the one
    // looked up later are the same string.
    QCOMPARE(manager->saveWorkspace(QStringLiteral("  Padded  ")), true);
    QCOMPARE(manager->contains(QStringLiteral("Padded")), true);
    QCOMPARE(manager->currentWorkspace(), QStringLiteral("Padded"));
}

void tst_QxWorkspaceManager::savingAgainOverwrites()
{
    QScopedPointer<QxAppShell> shell(newShell(newSettingsFile()));
    QxWorkspaceManager *manager = shell->workspaceManager();

    manager->saveWorkspace(QStringLiteral("Writing"));
    shell->setCurrentPage(QStringLiteral("two"));
    QCOMPARE(manager->saveWorkspace(QStringLiteral("Writing")), true);

    QCOMPARE(manager->count(), 1);
    QCOMPARE(manager->workspaceNames(), QStringList{QStringLiteral("Writing")});
}

void tst_QxWorkspaceManager::namelessIsRefused()
{
    QScopedPointer<QxAppShell> shell(newShell(newSettingsFile()));
    QxWorkspaceManager *manager = shell->workspaceManager();

    QCOMPARE(manager->saveWorkspace(QString()), false);
    QCOMPARE(manager->saveWorkspace(QStringLiteral("   ")), false);
    QCOMPARE(manager->count(), 0);
    QCOMPARE(manager->applyWorkspace(QString()), false);
}

void tst_QxWorkspaceManager::applyingBringsTheLayoutBack()
{
    QScopedPointer<QxAppShell> shell(newShell(newSettingsFile()));
    QxWorkspaceManager *manager = shell->workspaceManager();

    shell->setCurrentPage(QStringLiteral("two"));
    DockWidget *output = shell->dock(QStringLiteral("output"));
    QVERIFY(output != Q_NULLPTR);
    QCOMPARE(output->isClosed(), false);

    QCOMPARE(manager->saveWorkspace(QStringLiteral("Writing")), true);

    // The user rearranges: another page in front, a panel closed.
    shell->setCurrentPage(QStringLiteral("one"));
    output->toggleView(false);
    QCOMPARE(output->isClosed(), true);
    QCOMPARE(shell->currentPageId(), QStringLiteral("one"));

    QCOMPARE(manager->applyWorkspace(QStringLiteral("Writing")), true);
    QCOMPARE(shell->currentPageId(), QStringLiteral("two"));
    QCOMPARE(output->isClosed(), false);
    QCOMPARE(manager->currentWorkspace(), QStringLiteral("Writing"));
}

void tst_QxWorkspaceManager::unknownWorkspace()
{
    QScopedPointer<QxAppShell> shell(newShell(newSettingsFile()));
    QxWorkspaceManager *manager = shell->workspaceManager();

    QCOMPARE(manager->applyWorkspace(QStringLiteral("nope")), false);
    QCOMPARE(manager->removeWorkspace(QStringLiteral("nope")), false);
    QCOMPARE(manager->renameWorkspace(QStringLiteral("nope"), QStringLiteral("other")), false);
    QCOMPARE(manager->currentWorkspace(), QString());
}

void tst_QxWorkspaceManager::renaming()
{
    QScopedPointer<QxAppShell> shell(newShell(newSettingsFile()));
    QxWorkspaceManager *manager = shell->workspaceManager();

    shell->setCurrentPage(QStringLiteral("two"));
    manager->saveWorkspace(QStringLiteral("Writing"));
    manager->saveWorkspace(QStringLiteral("Review"));

    QSignalSpy removed(manager, &QxWorkspaceManager::workspaceRemoved);
    QSignalSpy saved(manager, &QxWorkspaceManager::workspaceSaved);

    QCOMPARE(manager->renameWorkspace(QStringLiteral("Writing"), QStringLiteral("Drafting")), true);

    // A rename keeps the place in the order and moves what was stored.
    const QStringList expected{QStringLiteral("Drafting"), QStringLiteral("Review")};
    QCOMPARE(manager->workspaceNames(), expected);
    QCOMPARE(manager->contains(QStringLiteral("Writing")), false);
    QCOMPARE(manager->contains(QStringLiteral("Drafting")), true);
    QCOMPARE(removed.count(), 1);
    QCOMPARE(removed.at(0).at(0).toString(), QStringLiteral("Writing"));
    QCOMPARE(saved.count(), 1);
    QCOMPARE(saved.at(0).at(0).toString(), QStringLiteral("Drafting"));

    shell->setCurrentPage(QStringLiteral("one"));
    QCOMPARE(manager->applyWorkspace(QStringLiteral("Drafting")), true);
    QCOMPARE(shell->currentPageId(), QStringLiteral("two"));

    // Renaming a name onto itself is the no-op it looks like.
    QCOMPARE(manager->renameWorkspace(QStringLiteral("Drafting"), QStringLiteral("Drafting")), true);
    QCOMPARE(manager->workspaceNames(), expected);
    QCOMPARE(removed.count(), 1);
    QCOMPARE(saved.count(), 1);
}

void tst_QxWorkspaceManager::renamingOntoAnExistingOne()
{
    QScopedPointer<QxAppShell> shell(newShell(newSettingsFile()));
    QxWorkspaceManager *manager = shell->workspaceManager();

    manager->saveWorkspace(QStringLiteral("Writing"));
    manager->saveWorkspace(QStringLiteral("Review"));

    // Overwriting would throw an arrangement away without being asked.
    QCOMPARE(manager->renameWorkspace(QStringLiteral("Writing"), QStringLiteral("Review")), false);
    QCOMPARE(manager->renameWorkspace(QStringLiteral("Writing"), QString()), false);
    const QStringList expected{QStringLiteral("Writing"), QStringLiteral("Review")};
    QCOMPARE(manager->workspaceNames(), expected);
}

void tst_QxWorkspaceManager::removing()
{
    QScopedPointer<QxAppShell> shell(newShell(newSettingsFile()));
    QxWorkspaceManager *manager = shell->workspaceManager();

    manager->saveWorkspace(QStringLiteral("Writing"));
    manager->saveWorkspace(QStringLiteral("Review"));

    QCOMPARE(manager->removeWorkspace(QStringLiteral("Writing")), true);
    QCOMPARE(manager->workspaceNames(), QStringList{QStringLiteral("Review")});
    QCOMPARE(manager->contains(QStringLiteral("Writing")), false);

    // The stored layout went with it, not just the entry in the list.
    QCOMPARE(manager->applyWorkspace(QStringLiteral("Writing")), false);
    QCOMPARE(manager->removeWorkspace(QStringLiteral("Writing")), false);
}

void tst_QxWorkspaceManager::removingTheCurrentOne()
{
    QScopedPointer<QxAppShell> shell(newShell(newSettingsFile()));
    QxWorkspaceManager *manager = shell->workspaceManager();

    manager->saveWorkspace(QStringLiteral("Writing"));
    QCOMPARE(manager->currentWorkspace(), QStringLiteral("Writing"));

    QSignalSpy spy(manager, &QxWorkspaceManager::currentWorkspaceChanged);
    QCOMPARE(manager->removeWorkspace(QStringLiteral("Writing")), true);

    // There is no workspace on screen any more, so there is no current one.
    QCOMPARE(manager->currentWorkspace(), QString());
    QCOMPARE(spy.count(), 1);
    QCOMPARE(spy.at(0).at(0).toString(), QString());
}

void tst_QxWorkspaceManager::clearing()
{
    QScopedPointer<QxAppShell> shell(newShell(newSettingsFile()));
    QxWorkspaceManager *manager = shell->workspaceManager();

    manager->saveWorkspace(QStringLiteral("Writing"));
    manager->saveWorkspace(QStringLiteral("Review"));
    QCOMPARE(manager->count(), 2);

    QSignalSpy removed(manager, &QxWorkspaceManager::workspaceRemoved);
    manager->clearWorkspaces();

    QCOMPARE(manager->count(), 0);
    QCOMPARE(manager->workspaceNames(), QStringList());
    QCOMPARE(manager->currentWorkspace(), QString());

    // One signal per name, so a consumer that keeps a menu in step does not
    // have to guess what disappeared.
    QCOMPARE(removed.count(), 2);

    // Clearing an empty manager is not worth a signal.
    manager->clearWorkspaces();
    QCOMPARE(removed.count(), 2);
}

void tst_QxWorkspaceManager::signalsOnEveryChange()
{
    QScopedPointer<QxAppShell> shell(newShell(newSettingsFile()));
    QxWorkspaceManager *manager = shell->workspaceManager();

    QSignalSpy saved(manager, &QxWorkspaceManager::workspaceSaved);
    QSignalSpy applied(manager, &QxWorkspaceManager::workspaceApplied);
    QSignalSpy removed(manager, &QxWorkspaceManager::workspaceRemoved);
    QSignalSpy current(manager, &QxWorkspaceManager::currentWorkspaceChanged);

    manager->saveWorkspace(QStringLiteral("Writing"));
    QCOMPARE(saved.count(), 1);
    QCOMPARE(current.count(), 1);
    QCOMPARE(current.at(0).at(0).toString(), QStringLiteral("Writing"));

    // Applying the workspace that is already current is still an apply; it is
    // the arrangement that is wanted back, whatever the bookkeeping says.
    manager->applyWorkspace(QStringLiteral("Writing"));
    QCOMPARE(applied.count(), 1);
    QCOMPARE(current.count(), 1);

    manager->saveWorkspace(QStringLiteral("Review"));
    QCOMPARE(current.count(), 2);
    QCOMPARE(current.at(1).at(0).toString(), QStringLiteral("Review"));

    manager->applyWorkspace(QStringLiteral("Writing"));
    QCOMPARE(applied.count(), 2);
    QCOMPARE(current.count(), 3);

    // A refused apply reports nothing.
    manager->applyWorkspace(QStringLiteral("nope"));
    QCOMPARE(applied.count(), 2);

    manager->removeWorkspace(QStringLiteral("Review"));
    QCOMPARE(removed.count(), 1);
}

void tst_QxWorkspaceManager::shellAccessors()
{
    QScopedPointer<QxAppShell> shell(newShell(newSettingsFile()));

    // The shell owns one manager, so asking twice is not two managers.
    QxWorkspaceManager *manager = shell->workspaceManager();
    QCOMPARE(shell->workspaceManager(), manager);

    // The four shell methods are the ones a menu needs; they are one-line
    // delegations, so what they do is the manager's own contract.
    QCOMPARE(shell->saveWorkspace(QStringLiteral("Writing")), true);
    QCOMPARE(shell->workspaceNames(), QStringList{QStringLiteral("Writing")});
    QCOMPARE(shell->applyWorkspace(QStringLiteral("Writing")), true);
    QCOMPARE(shell->applyWorkspace(QStringLiteral("nope")), false);
    QCOMPARE(shell->saveWorkspace(QString()), false);
}

void tst_QxWorkspaceManager::theFileIsTheStorage()
{
    const QString file = newSettingsFile();

    {
        QScopedPointer<QxAppShell> shell(newShell(file));
        shell->setCurrentPage(QStringLiteral("two"));
        shell->saveWorkspace(QStringLiteral("Writing"));
    }

    // A second shell - a second run of the application, really - reads the same
    // file, which is the only reason a workspace is worth having.
    QScopedPointer<QxAppShell> shell(newShell(file));
    QxWorkspaceManager *manager = shell->workspaceManager();
    QCOMPARE(manager->workspaceNames(), QStringList{QStringLiteral("Writing")});
    QCOMPARE(manager->currentWorkspace(), QStringLiteral("Writing"));

    QCOMPARE(shell->currentPageId(), QStringLiteral("one"));
    QCOMPARE(manager->applyWorkspace(QStringLiteral("Writing")), true);
    QCOMPARE(shell->currentPageId(), QStringLiteral("two"));

    // The keys are part of the contract: the shell's own layout and the
    // workspaces live in one group, and the workspaces are named sub-groups.
    QxSettings reader(file, QSettings::IniFormat);
    reader.beginGroup(QStringLiteral("ui"));
    QCOMPARE(reader.stringListValue(QStringLiteral("workspaceNames")), QStringList{QStringLiteral("Writing")});
    QCOMPARE(reader.stringValue(QStringLiteral("currentWorkspace")), QStringLiteral("Writing"));
    QVERIFY(!reader.value(QStringLiteral("workspace/Writing/dockState")).toByteArray().isEmpty());
    QCOMPARE(reader.stringValue(QStringLiteral("workspace/Writing/currentPage")), QStringLiteral("two"));
    reader.endGroup();
}

TEST_ADD(tst_QxWorkspaceManager)

#include "tst_qxworkspace.moc"
