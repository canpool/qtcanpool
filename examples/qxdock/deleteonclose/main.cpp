#include <QMainWindow>
#include <QAction>
#include <QTextEdit>
#include <QApplication>
#include <QMenuBar>
#include <QDebug>

#include "qxdock/dockwindow.h"
#include "qxdock/dockmanager.h"
#include "qxdock/dockwidget.h"
#include "qxdock/dockpanel.h"

QX_DOCK_USE_NAMESPACE

class MainWindow : public QMainWindow
{
private:
    DockWindow *m_window = nullptr;
protected:
    virtual void closeEvent(QCloseEvent *event) override
    {
        QMainWindow::closeEvent(event);
        if (m_window) {
            m_window->deleteLater();
        }
    }
public:
    void setDockWindow(DockWindow *wd)
    {
        m_window = wd;
    }
};

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;

    DockManager::setConfigFlag(DockManager::FocusHighlighting, true);
    DockManager::setConfigFlag(DockManager::AllTabsHaveCloseButton, true);
    auto wd = new DockWindow(&w);
    w.setCentralWidget(wd);
    w.setDockWindow(wd);
    QObject::connect(wd, &DockWindow::focusedDockWidgetChanged, [](DockWidget *old, DockWidget *now) {
        static int count = 0;
        qDebug() << count++ << " DockWindow::focusedDockWidgetChanged old: " << (old ? old->objectName() : "-")
                 << " now: " << now->objectName() << " visible: " << now->isVisible();
        now->widget()->setFocus();
    });

    QAction *action = new QAction("New [DockWidgetDeleteOnClose]", &w);
    w.menuBar()->addAction(action);

    int i = 0;
    QObject::connect(action, &QAction::triggered, [&]() {
        auto dw = new DockWidget(QStringLiteral("test %1 [DockWidgetDeleteOnClose]").arg(i++), &w);
        auto editor = new QTextEdit(QStringLiteral("qt dock..."), dw);
        dw->setWidget(editor);
        dw->setFeature(DockWidget::DockWidgetDeleteOnClose, true);
        auto panel = wd->addDockWidgetTab(Qx::CenterDockWidgetArea, dw);
        qDebug() << "doc dock widget created!" << dw << panel;
    });

    auto dw = new DockWidget(QStringLiteral("test %1 [DeleteContentOnClose]").arg(i++), &w);
    auto editor = new QTextEdit(QStringLiteral("recreated qt dock......"), dw);
    dw->setWidget(editor);
    dw->setFeature(DockWidget::DeleteContentOnClose, true);
    dw->setWidgetFactory([](QWidget *dw) {
        static int timesRecreated = 0;
        return new QTextEdit(QStringLiteral("recreated qt dock... times %1").arg(++timesRecreated), dw);
    });
    auto panel = wd->addDockWidgetTab(Qx::CenterDockWidgetArea, dw);
    qDebug() << "DeleteContentOnClose dock widget created!" << dw << panel;

    action = new QAction("Toggle [DeleteContentOnClose]", &w);
    w.menuBar()->addAction(action);

    QObject::connect(action, &QAction::triggered, [dw]() {
        dw->toggleView(dw->isClosed());
        qDebug() << QString("dock widget %1! contents widget %2!")
                        .arg(dw->isClosed() ? QStringLiteral("closed") : QStringLiteral("open"),
                             dw->widget() ? QStringLiteral("created") : QStringLiteral("deleted"));
    });

    action = new QAction("New", &w);
    w.menuBar()->addAction(action);
    QObject::connect(action, &QAction::triggered, [&]() {
        auto dw = new DockWidget(QStringLiteral("test %1").arg(i++), &w);
        auto editor = new QTextEdit(QStringLiteral("qt dock..."), dw);
        dw->setWidget(editor);
        auto panel = wd->addDockWidgetTab(Qx::CenterDockWidgetArea, dw);
        qDebug() << "dock widget created!" << dw << panel;
    });

    w.show();

    return a.exec();
}
