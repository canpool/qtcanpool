#include "tst_global.h"

#include "qxdock/dockgroup.h"
#include "qxdock/dockgroupmanager.h"
#include "qxdock/docksubwindow.h"
#include "qxdock/dockwidget.h"
#include "qxdock/dockpanel.h"

#include <QLabel>
#include <QTemporaryDir>

QX_DOCK_USE_NAMESPACE

class tst_DockGroup : public QObject
{
    Q_OBJECT
private slots:
    void constructionAndBasicApi();
    void addTabWidget();
    void createGroup();
    void groupNameValidation();
    void perspectives();
    void persistenceRoundTrip();
};

void tst_DockGroup::constructionAndBasicApi()
{
    QTemporaryDir dir;
    DockGroupManager mgr(dir.path());

    // The public constructor makes the group its own top level and adopts the
    // manager and the can-create-new-groups flag from the caller.
    DockGroup g(nullptr, true, &mgr);

    QCOMPARE(g.canCreateNewGroups(), true);
    QCOMPARE(g.groupManager(), &mgr);
    QCOMPARE(g.isTopLevel(), true);
    QCOMPARE(g.topLevelGroup(), &g);

    // Every group owns exactly one DockSubWindow that backs its docking area.
    DockSubWindow *sw = g.subWindow();
    QVERIFY(sw != nullptr);
    QCOMPARE(sw->group(), &g);
    // The top-level group has no name, so its sub window has no group name.
    QCOMPARE(sw->getGroupName(), QString());
}

void tst_DockGroup::addTabWidget()
{
    QTemporaryDir dir;
    DockGroupManager mgr(dir.path());
    DockGroup g(nullptr, true, &mgr);

    QLabel *w1 = new QLabel("one");
    QLabel *w2 = new QLabel("two");
    QLabel *w3 = new QLabel("three");

    DockPanel *p1 = g.addTabWidget(w1, "Tab1", nullptr);
    DockPanel *p2 = g.addTabWidget(w2, "Tab2", p1);
    DockPanel *p3 = g.addTabWidget(w3, "Tab3", p2);

    QVERIFY(p1 != nullptr);
    QVERIFY(p2 != nullptr);
    QVERIFY(p3 != nullptr);

    // The three widgets all land in the top-level sub window.
    QCOMPARE(g.subWindow()->getWidgetsInGUIOrder().size(), 3);
    QCOMPARE(g.subWindow()->allDockWidgets(true, true).size(), 3);

    QStringList names;
    for (auto dw : g.subWindow()->getWidgetsInGUIOrder())
        names << dw->objectName();
    names.sort();
    QCOMPARE(names, QStringList() << "Tab1" << "Tab2" << "Tab3");
}

void tst_DockGroup::createGroup()
{
    QTemporaryDir dir;
    DockGroupManager mgr(dir.path());
    DockGroup g(nullptr, true, &mgr);

    // A top-level group has no name and reports itself as top level.
    QCOMPARE(g.isTopLevel(), true);
    QCOMPARE(g.topLevelGroup(), &g);

    DockGroup *child = g.createGroup("Group1", nullptr);
    QVERIFY(child != nullptr);

    // A created group carries the group name and is no longer top level; it
    // still belongs to the same top-level group and manager, and inherits the
    // can-create-new-groups flag from the top level.
    QCOMPARE(child->objectName(), QString("Group1"));
    QCOMPARE(child->isTopLevel(), false);
    QCOMPARE(child->topLevelGroup(), &g);
    QCOMPARE(child->groupManager(), &mgr);
    QCOMPARE(child->canCreateNewGroups(), true);
    QCOMPARE(child->subWindow()->getGroupName(), QString("Group1"));

    // The top-level sub window now spans two sub windows: itself and the child.
    QCOMPARE(g.subWindow()->allSubWindows(true, true).size(), 2);
}

void tst_DockGroup::groupNameValidation()
{
    QTemporaryDir dir;
    DockGroupManager mgr(dir.path());
    DockGroup g(nullptr, true, &mgr);

    // An empty name is rejected; an unused, non-empty name is accepted.
    QCOMPARE(g.getGroupNameError(""), QString("Group must have a non-empty name"));
    QCOMPARE(g.getGroupNameError("FreeName"), QString());

    DockGroup *child = g.createGroup("Group1", nullptr);
    QVERIFY(child != nullptr);
    // A name that is already in use is rejected.
    QCOMPARE(g.getGroupNameError("Group1"), QString("Group name 'Group1' already used"));

    // The persist-group-name <-> group-name mapping is a pure string transform;
    // it only understands the Child- prefixed form.
    QCOMPARE(DockSubWindow::getGroupNameFromPersistGroupName("Child-Group1"), QString("Group1"));
}

void tst_DockGroup::perspectives()
{
    QTemporaryDir dir;
    DockGroupManager mgr(dir.path());
    DockGroup g(nullptr, true, &mgr);

    // No perspective is known until one is added (loadPerspectives was not
    // called, and the in-memory list starts empty).
    QCOMPARE(mgr.perspectiveNames(), QStringList());

    g.addTabWidget(new QLabel("a"), "A", nullptr);

    mgr.addPerspective("Perspective1", &g);
    QCOMPARE(mgr.perspectiveNames(), QStringList() << "Perspective1");

    mgr.removePerspective("Perspective1");
    QCOMPARE(mgr.perspectiveNames(), QStringList());

    mgr.addPerspective("Perspective2", &g);
    QCOMPARE(mgr.perspectiveNames(), QStringList() << "Perspective2");

    mgr.removePerspectives();
    QCOMPARE(mgr.perspectiveNames(), QStringList());
}

void tst_DockGroup::persistenceRoundTrip()
{
    QTemporaryDir dir;

    {
        DockGroupManager mgr(dir.path());
        DockGroup g(nullptr, true, &mgr);

        g.addTabWidget(new QLabel("a"), "A", nullptr);
        g.createGroup("Group1", nullptr);

        mgr.addPerspective("Saved", &g);
        mgr.savePerspectives();
    }

    // A fresh manager on the same folder reloads the persisted perspective.
    DockGroupManager mgr2(dir.path());
    mgr2.loadPerspectives();
    QCOMPARE(mgr2.perspectiveNames(), QStringList() << "Saved");
}

TEST_ADD(tst_DockGroup)

#include "tst_dockgroup.moc"
