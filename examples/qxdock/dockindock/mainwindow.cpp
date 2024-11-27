#include "mainwindow.h"

#include "qxdock/dockgroup.h"
#include "qxdock/dockgroupmanager.h"
#include "qxdock/dockpanel.h"

#include <QLabel>
#include <QMenuBar>
#include <QMessageBox>
#include <QSettings>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , m_groupManager(new DockGroupManager("persist"))
{
    resize(400, 400);

    m_group = new DockGroup(this, true, m_groupManager.get());
    setCentralWidget(m_group);

    m_group->attachViewMenu(menuBar()->addMenu("View"));

    DockPanel *prevPanel = NULL;
    for (int i = 0; i != 3; ++i) {
        // Create example content label - this can be any application specific widget
        QLabel *l = new QLabel();
        l->setWordWrap(true);
        l->setAlignment(Qt::AlignTop | Qt::AlignLeft);
        l->setText("dock in dock");

        prevPanel = m_group->addTabWidget(l, "Top label " + QString::number(i), prevPanel);
    }

    auto lastTopLevelPanel = prevPanel;

    for (int j = 0; j != 2; ++j) {
        DockGroup *group = m_group->createGroup("Group " + QString::number(j), lastTopLevelPanel);

        prevPanel = nullptr;
        for (int i = 0; i != 3; ++i) {
            // Create example content label - this can be any application specific widget
            QLabel *l = new QLabel();
            l->setWordWrap(true);
            l->setAlignment(Qt::AlignTop | Qt::AlignLeft);
            l->setText("dock in dock");

            prevPanel = group->addTabWidget(l, "ZInner " + QString::number(j) + "/" + QString::number(i), prevPanel);
        }

        // create sub-group
        auto subGroup = group->createGroup("SubGroup " + QString::number(j), prevPanel);
        prevPanel = nullptr;
        for (int i = 0; i != 3; ++i) {
            // Create example content label - this can be any application specific
            // widget
            QLabel *l = new QLabel();
            l->setWordWrap(true);
            l->setAlignment(Qt::AlignTop | Qt::AlignLeft);
            l->setText("dock in dock");

            prevPanel =
                subGroup->addTabWidget(l, "SubInner " + QString::number(j) + "/" + QString::number(i), prevPanel);
        }
    }

    m_groupManager->loadPerspectives();
}

MainWindow::~MainWindow()
{
    m_groupManager->savePerspectives();
}
