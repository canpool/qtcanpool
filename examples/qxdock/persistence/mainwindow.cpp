#include "mainwindow.h"

#include "qxdock/dockwindow.h"
#include "qxdock/dockwidget.h"

#include <QMenuBar>
#include <QMenu>
#include <QAction>
#include <QTextEdit>
#include <QStatusBar>
#include <QFileDialog>
#include <QStandardPaths>
#include <QDir>
#include <QFile>

QX_DOCK_USE_NAMESPACE

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    QMenu *file = menuBar()->addMenu(tr("File"));
    QAction *save = file->addAction(tr("Save layout..."));
    QAction *restore = file->addAction(tr("Restore layout..."));
    connect(save, &QAction::triggered, this, &MainWindow::saveLayout);
    connect(restore, &QAction::triggered, this, &MainWindow::restoreLayout);

    m_window = new DockWindow(this);
    setCentralWidget(m_window);

    // Central editor.
    QTextEdit *editor = new QTextEdit();
    editor->setPlainText(tr("Dock the panels, rearrange them, then choose "
                            "File > Save layout. Restore layout reads the same "
                            "XML file back. The file is plain, human-readable "
                            "XML - open it in a text editor and diff two saves."));
    DockWidget *central = new DockWidget("Editor");
    central->setWidget(editor);
    m_window->setCentralWidget(central);

    DockWidget *explorer = new DockWidget("Explorer");
    explorer->setWidget(new QTextEdit());
    m_window->addDockWidget(Qx::LeftDockWidgetArea, explorer);

    DockWidget *output = new DockWidget("Output");
    output->setWidget(new QTextEdit());
    m_window->addDockWidget(Qx::BottomDockWidgetArea, output);

    DockWidget *props = new DockWidget("Properties");
    props->setWidget(new QTextEdit());
    m_window->addDockWidget(Qx::RightDockWidgetArea, props);

    // Bring the last saved layout back on launch, if one exists (C2).
    const QString path = defaultLayoutPath();
    if (QFile::exists(path)) {
        if (m_window->restoreStateFromFile(path))
            statusBar()->showMessage(tr("Restored layout from %1").arg(path));
        else
            statusBar()->showMessage(tr("Could not restore %1").arg(path));
    }

    resize(960, 600);
}

MainWindow::~MainWindow()
{
}

QString MainWindow::defaultLayoutPath() const
{
    const QString dir = QStandardPaths::writableLocation(QStandardPaths::TempLocation);
    return QDir(dir).filePath(QStringLiteral("qxdock_persistence_demo.xml"));
}

void MainWindow::saveLayout()
{
    const QString path =
        QFileDialog::getSaveFileName(this, tr("Save layout"), defaultLayoutPath(), QStringLiteral("XML (*.xml)"));
    if (path.isEmpty())
        return;
    if (m_window->saveStateToFile(path))
        statusBar()->showMessage(tr("Saved layout to %1").arg(path));
    else
        statusBar()->showMessage(tr("Failed to save %1").arg(path));
}

void MainWindow::restoreLayout()
{
    const QString path =
        QFileDialog::getOpenFileName(this, tr("Restore layout"), defaultLayoutPath(), QStringLiteral("XML (*.xml)"));
    if (path.isEmpty())
        return;
    if (m_window->restoreStateFromFile(path))
        statusBar()->showMessage(tr("Restored layout from %1").arg(path));
    else
        statusBar()->showMessage(tr("Failed to restore %1").arg(path));
}
