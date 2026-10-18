/**
 * Copyleft (C) 2024 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/

#include <QAction>
#include <QApplication>
#include <QDir>
#include <QFile>
#include <QMainWindow>
#include <QMenu>
#include <QMenuBar>
#include <QStatusBar>
#include <QTextEdit>
#include <QToolBar>

#include "qxdock/dockmanager.h"
#include "qxdock/dockstylesheet.h"
#include "qxdock/dockwidget.h"
#include "qxdock/dockwindow.h"

QX_DOCK_USE_NAMESPACE

/**
 * Dockable toolbars around a docking layout.
 *
 * The 4.5 plan listed a "toolbar docking subsystem (multi-row, automatic
 * rearrangement, floating, dragging between rows)" as missing from this library.
 * Checked against the code, what is missing is a toolbar area, and the answer is
 * that the library should not grow one: a window that hosts a DockWindow is
 * almost always a QMainWindow, and QMainWindow already docks toolbars and does
 * all four of those things. A second implementation inside qxdock would have to
 * reproduce the layout and the drag machinery of the one that is already in the
 * window, and would then have to agree with it about what a toolbar row is.
 *
 * What this sample demonstrates is the arrangement rather than a class: a
 * DockWindow in the central area, toolbars around it, and the two halves of the
 * layout stored and restored by different calls, because they belong to different
 * owners. QMainWindow knows about the toolbars and the main window,
 * DockWindow::saveStateToFile() knows about the dock widgets, and neither can be
 * asked about the other's half.
 */
class MainWindow : public QMainWindow
{
public:
    MainWindow()
    {
        setWindowTitle("qxdock: dockable toolbars next to a docking layout");

        addToolBar(createToolBar("File", {"New", "Open", "Save"}));
        addToolBar(createToolBar("Edit", {"Undo", "Redo"}));
        addToolBar(createToolBar("View", {"Zoom In", "Zoom Out"}));

        m_dockWindow = new DockWindow(this);
        setCentralWidget(m_dockWindow);
        createDockWidgets();
        createLayoutMenu();

        resize(1000, 700);
    }
private:
    QToolBar *createToolBar(const QString &title, const QStringList &actions)
    {
        // The object name is what QMainWindow::saveState() records a toolbar by.
        auto *toolBar = new QToolBar(title, this);
        toolBar->setObjectName(title);
        toolBar->setAllowedAreas(Qt::AllToolBarAreas);
        for (const QString &action : actions) {
            toolBar->addAction(action);
        }
        return toolBar;
    }

    void createDockWidgets()
    {
        static const struct {
            const char *name;
            Qx::DockWidgetArea area;
        } docks[] = {
            {"Explorer", Qx::LeftDockWidgetArea},
            {"Output", Qx::BottomDockWidgetArea},
        };

        for (const auto &dock : docks) {
            auto *w = new DockWidget(QString::fromLatin1(dock.name), m_dockWindow);
            w->setWidget(new QTextEdit(QString("%1: dock this against a window edge, or drag its tab out to "
                                               "float it. The toolbars around the central area use "
                                               "QMainWindow's own docking and are unaffected by either.")
                                           .arg(QString::fromLatin1(dock.name)),
                                       w));
            m_dockWindow->addDockWidget(dock.area, w);
        }
    }

    void createLayoutMenu()
    {
        QMenu *menu = menuBar()->addMenu("Layout");

        QAction *save = menu->addAction("Save");
        connect(save, &QAction::triggered, this, [this]() {
            const bool toolBars = saveToFile(toolBarStateFile(), saveState());
            const bool docks = m_dockWindow->saveStateToFile(dockStateFile());
            statusBar()->showMessage(report("saved", toolBars, docks));
        });

        QAction *restore = menu->addAction("Restore");
        connect(restore, &QAction::triggered, this, [this]() {
            // Each half is restored by its own owner; handing one the other's
            // state would only give it something to complain about.
            const bool toolBars = restoreFromFile(toolBarStateFile());
            const bool docks = m_dockWindow->restoreStateFromFile(dockStateFile());
            statusBar()->showMessage(report("restored", toolBars, docks));
        });

        statusBar()->showMessage(
            QString("layout files: %1 and %2")
                .arg(QDir::toNativeSeparators(toolBarStateFile()), QDir::toNativeSeparators(dockStateFile())));
    }

    // QMainWindow has no state file helper of its own, which is the whole point:
    // the two halves of the layout are not one thing, so they are not stored by
    // one call either.
    bool saveToFile(const QString &fileName, const QByteArray &state)
    {
        QFile file(fileName);
        if (!file.open(QIODevice::WriteOnly)) {
            return false;
        }
        return file.write(state) == state.size();
    }

    bool restoreFromFile(const QString &fileName)
    {
        QFile file(fileName);
        if (!file.open(QIODevice::ReadOnly)) {
            return false;
        }
        return restoreState(file.readAll());
    }

    QString report(const QString &what, bool toolBars, bool docks)
    {
        return QString("toolbars: %1, docks: %2")
            .arg(toolBars ? what : QStringLiteral("failed"), docks ? what : QStringLiteral("failed"));
    }

    QString toolBarStateFile()
    {
        return QDir::temp().filePath("qxdock-toolbars-main.bin");
    }

    QString dockStateFile()
    {
        return QDir::temp().filePath("qxdock-toolbars-docks.xml");
    }

    DockWindow *m_dockWindow = nullptr;
};

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    DockManager::setAutoHideConfigFlags(DockManager::DefaultAutoHideConfig);
    a.setStyleSheet(dockStyleSheet(DockManager::testConfigFlag(DockManager::FocusHighlighting)));

    MainWindow w;
    w.show();

    return a.exec();
}
