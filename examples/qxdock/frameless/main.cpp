/**
 * Copyleft (C) 2024 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/

#include <QApplication>
#include <QLabel>
#include <QMouseEvent>
#include <QTextEdit>
#include <QVBoxLayout>
#include <QWidget>
#include <QWindow>

#include "qxdock/dockmanager.h"
#include "qxdock/dockpanel.h"
#include "qxdock/dockwidget.h"
#include "qxdock/dockwindow.h"

#include "themestart.h"

QX_DOCK_USE_NAMESPACE

namespace
{

// Qt 6 moved the mouse position to QPointF; Qt 5 has the integer one only.
QPoint globalPosOf(QMouseEvent *e)
{
#if (QT_VERSION >= QT_VERSION_CHECK(6, 0, 0))
    return e->globalPosition().toPoint();
#else
    return e->globalPos();
#endif
}

// The width of the host's own border: the strip the window is resized by. It has
// to belong to the host, because a mouse press inside the dock window goes to the
// dock window - and should, since that is how docking is dragged.
const int kResizeBorder = 4;

Qt::Edges edgesAt(const QPoint &pos, const QSize &size)
{
    Qt::Edges edges;
    if (pos.x() < kResizeBorder) {
        edges |= Qt::LeftEdge;
    } else if (pos.x() >= size.width() - kResizeBorder) {
        edges |= Qt::RightEdge;
    }
    if (pos.y() < kResizeBorder) {
        edges |= Qt::TopEdge;
    } else if (pos.y() >= size.height() - kResizeBorder) {
        edges |= Qt::BottomEdge;
    }
    return edges;
}

}   // namespace

/**
 * The title bar of a frameless window: it is what the window is moved by, because
 * a window without a frame cannot be moved by its frame.
 */
class TitleBar : public QWidget
{
public:
    explicit TitleBar(QWidget *parent)
        : QWidget(parent)
    {
        setFixedHeight(30);
        setAutoFillBackground(true);

        auto *layout = new QVBoxLayout(this);
        layout->setContentsMargins(8, 0, 8, 0);

        auto *title = new QLabel("DockWindow in a frameless window", this);
        layout->addWidget(title, 0, Qt::AlignVCenter);
    }
protected:
    void mousePressEvent(QMouseEvent *e) override
    {
        if (e->button() != Qt::LeftButton) {
            QWidget::mousePressEvent(e);
            return;
        }
        // Remember where inside the window the press happened, so the window
        // follows the pointer instead of jumping to it.
        m_pressOffset = e->pos();
        e->accept();
    }

    void mouseMoveEvent(QMouseEvent *e) override
    {
        if (!(e->buttons() & Qt::LeftButton)) {
            QWidget::mouseMoveEvent(e);
            return;
        }
        window()->move(globalPosOf(e) - m_pressOffset);
        e->accept();
    }

    void mouseDoubleClickEvent(QMouseEvent *e) override
    {
        if (e->button() != Qt::LeftButton) {
            QWidget::mouseDoubleClickEvent(e);
            return;
        }
        if (window()->isMaximized()) {
            window()->showNormal();
        } else {
            window()->showMaximized();
        }
        e->accept();
    }
private:
    QPoint m_pressOffset;
};

/**
 * A frameless main window holding a DockWindow.
 *
 * Docking itself does not care about the frame - a DockWindow docks, floats and
 * auto-hides the same whatever its top level looks like (that is what
 * tst_DockWindow::framelessHost holds on to). What a frameless top level takes
 * away is the frame, and putting it back is the host's job: this is the example
 * of that side of the arrangement. Two things are needed and neither of them
 * belongs in the library.
 *
 * The first is a title bar, since there is no frame to drag. The second is a
 * border of its own, because the mouse presses near the edge have to reach the
 * host rather than the dock window inside it - which is also why the border is
 * part of the layout instead of a decoration drawn on top.
 *
 * If the application already has a frameless main window of its own - qxwindow's,
 * for instance - then it has both of these already and the DockWindow just goes
 * inside it.
 */
class MainWindow : public QWidget
{
public:
    MainWindow()
    {
        setWindowFlags(Qt::Window | Qt::FramelessWindowHint);
        setWindowTitle("qxdock: frameless host");
        setMouseTracking(true);
        resize(900, 650);

        auto *layout = new QVBoxLayout(this);
        layout->setContentsMargins(kResizeBorder, kResizeBorder, kResizeBorder, kResizeBorder);
        layout->setSpacing(0);
        layout->addWidget(new TitleBar(this));

        m_dockWindow = new DockWindow(this);
        layout->addWidget(m_dockWindow, 1);

        createDockWidgets();
    }
protected:
    void mousePressEvent(QMouseEvent *e) override
    {
        const Qt::Edges edges = edgesAt(e->pos(), size());
        if (e->button() == Qt::LeftButton && edges != Qt::Edges()) {
            // Hand the gesture to the window system: it resizes the window the
            // way the platform does, and the drag loop it starts is one this
            // process does not have to write.
            window()->windowHandle()->startSystemResize(edges);
            e->accept();
            return;
        }
        QWidget::mousePressEvent(e);
    }

    void mouseMoveEvent(QMouseEvent *e) override
    {
        const Qt::Edges edges = edgesAt(e->pos(), size());
        if (edges == Qt::Edges()) {
            unsetCursor();
        } else if ((edges & Qt::LeftEdge && edges & Qt::TopEdge) || (edges & Qt::RightEdge && edges & Qt::BottomEdge)) {
            setCursor(Qt::SizeFDiagCursor);
        } else if ((edges & Qt::RightEdge && edges & Qt::TopEdge) || (edges & Qt::LeftEdge && edges & Qt::BottomEdge)) {
            setCursor(Qt::SizeBDiagCursor);
        } else if (edges & (Qt::LeftEdge | Qt::RightEdge)) {
            setCursor(Qt::SizeHorCursor);
        } else {
            setCursor(Qt::SizeVerCursor);
        }
        QWidget::mouseMoveEvent(e);
    }
private:
    void createDockWidgets()
    {
        static const struct {
            const char *name;
            Qx::DockWidgetArea area;
        } docks[] = {
            {"Explorer", Qx::LeftDockWidgetArea},
            {"Properties", Qx::RightDockWidgetArea},
            {"Output", Qx::BottomDockWidgetArea},
        };

        for (const auto &dock : docks) {
            auto *w = new DockWidget(QString::fromLatin1(dock.name), m_dockWindow);
            auto *edit = new QTextEdit(QString("%1: drag a tab out to float this, or onto an edge to dock it "
                                               "somewhere else. The window is frameless, so it is moved by its "
                                               "title bar and resized by the border around the dock area.")
                                           .arg(QString::fromLatin1(dock.name)),
                                       w);
            w->setWidget(edit);
            m_dockWindow->addDockWidget(dock.area, w);
        }
    }

    DockWindow *m_dockWindow = nullptr;
};

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    // The floating windows the dock area opens are top levels of their own and
    // are unaffected by the host having no frame; this just makes the example
    // show the auto-hide side of docking as well.
    DockManager::setAutoHideConfigFlags(DockManager::DefaultAutoHideConfig);
    // The library ships no look of its own; a theme carries the dock's rules.
    installExampleTheme();

    MainWindow w;
    w.show();

    return a.exec();
}
