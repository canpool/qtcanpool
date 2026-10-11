#include "mainwindow.h"

#include "qxdock/dockgroup.h"
#include "qxdock/dockgroupmanager.h"

#include <QLabel>
#include <QMenuBar>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , m_groupManager(new DockGroupManager("perspectives"))
{
    resize(500, 400);

    // A top-level DockGroup owns one DockSubWindow that backs its docking area.
    // DockGroupManager persists the named perspectives of this group to disk.
    m_group = new DockGroup(this, true, m_groupManager.get());
    setCentralWidget(m_group);

    // The view menu is filled automatically when shown; it exposes the organize
    // / load / save perspective actions that DockSubWindow provides for this group.
    m_group->attachViewMenu(menuBar()->addMenu("View"));

    // A few docked widgets in the top-level group.
    QLabel *l = new QLabel("Top-level docked content");
    l->setWordWrap(true);
    l->setAlignment(Qt::AlignTop | Qt::AlignLeft);
    m_group->addTabWidget(l, "Top", nullptr);

    // A nested sub-group demonstrates DockGroup::createGroup().
    DockGroup *sub = m_group->createGroup("Sub", nullptr);
    QLabel *sl = new QLabel("Nested group content");
    sl->setWordWrap(true);
    sl->setAlignment(Qt::AlignTop | Qt::AlignLeft);
    sub->addTabWidget(sl, "Nested", nullptr);

    // Restore any perspective saved on a previous run.
    m_groupManager->loadPerspectives();
}

MainWindow::~MainWindow()
{
    m_groupManager->savePerspectives();
}
