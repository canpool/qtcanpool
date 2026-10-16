#include "tst_global.h"

#include "qxdock/dockdrag.h"
#include "qxdock/dockwindow.h"
#include "qxdock/dockwidget.h"
#include "qxdock/dockpanel.h"

#include <QDragEnterEvent>
#include <QMimeData>
#include <QSharedPointer>
#include <QSignalSpy>

QX_DOCK_USE_NAMESPACE

// The drop policy and the mime gate are protected; exposing them here is how the
// test reaches the code dropEvent() itself runs.
class ProbeWindow : public DockWindow
{
public:
    using DockWindow::dragEnterEvent;
    using DockWindow::handleDockWidgetDrop;
};

class tst_DockDrag : public QObject
{
    Q_OBJECT
private slots:
    void payload();
    void foreignPayload();
    void drop();
};

/*!
 * The payload is the only thing that crosses a process boundary, so it is what
 * has to be right: a receiver reads the name to identify the dock widget and the
 * title to build one it does not have.
 */
void tst_DockDrag::payload()
{
    QSharedPointer<QMimeData> data(DockDrag::createMimeData("dw1", "Dock 1"));

    QVERIFY(!DockDrag::mimeType().isEmpty());
    QCOMPARE(data->hasFormat(DockDrag::mimeType()), true);
    QCOMPARE(DockDrag::canDecode(data.data()), true);
    QCOMPARE(DockDrag::dockWidgetName(data.data()), QString("dw1"));
    QCOMPARE(DockDrag::dockWidgetTitle(data.data()), QString("Dock 1"));

    // The payload is text, so a receiver that has never heard of qxdock can
    // still read what arrived.
    QVERIFY(QString::fromUtf8(data->data(DockDrag::mimeType())).startsWith("QxDock/1"));

    // The title is a label, not an identity: it may be left out.
    QSharedPointer<QMimeData> untitled(DockDrag::createMimeData("dw2"));
    QCOMPARE(DockDrag::canDecode(untitled.data()), true);
    QCOMPARE(DockDrag::dockWidgetName(untitled.data()), QString("dw2"));
    QCOMPARE(DockDrag::dockWidgetTitle(untitled.data()), QString());
}

/*!
 * Everything that is not one of our payloads has to be refused, and the refusals
 * that matter are the near misses: the right mime type with a payload that says
 * nothing usable.
 */
void tst_DockDrag::foreignPayload()
{
    QMimeData text;
    text.setText("just some text");
    QCOMPARE(DockDrag::canDecode(&text), false);
    QCOMPARE(DockDrag::dockWidgetName(&text), QString());
    QCOMPARE(DockDrag::dockWidgetTitle(&text), QString());

    QMimeData empty;
    QCOMPARE(DockDrag::canDecode(&empty), false);

    // Our mime type, but nothing that identifies a dock widget.
    QMimeData truncated;
    truncated.setData(DockDrag::mimeType(), QByteArray("QxDock/1"));
    QCOMPARE(DockDrag::canDecode(&truncated), false);

    QMimeData nameless;
    nameless.setData(DockDrag::mimeType(), QByteArray("QxDock/1\n\nDock 1"));
    QCOMPARE(DockDrag::canDecode(&nameless), false);

    // A payload of a future version must not be read as if it were this one.
    QMimeData otherVersion;
    otherVersion.setData(DockDrag::mimeType(), QByteArray("QxDock/2\ndw1\nDock 1"));
    QCOMPARE(DockDrag::canDecode(&otherVersion), false);
    QCOMPARE(DockDrag::dockWidgetName(&otherVersion), QString());
}

/*!
 * The window is the drop target.
 *
 * A note on the shape of this test: Qt delivers a drop to a widget only from a
 * real drag - a QDropEvent handed to sendEvent() is silently discarded and never
 * reaches dropEvent(), while a QDragEnterEvent does get through. So the gate that
 * decides whether a drag is ours at all is tested through dragEnterEvent(), and
 * what a drop does is tested through handleDockWidgetDrop(), which is the whole
 * body of dropEvent() and what an application would reimplement.
 */
void tst_DockDrag::drop()
{
    ProbeWindow wd;
    QVERIFY(wd.acceptDrops());

    DockWidget *dw1 = new DockWidget("dw1");
    wd.addDockWidget(Qx::LeftDockWidgetArea, dw1);
    DockWidget *dw2 = new DockWidget("dw2");
    wd.addDockWidget(Qx::RightDockWidgetArea, dw2);

    dw2->toggleView(false);
    QCOMPARE(dw2->isClosed(), true);

    QSignalSpy spy(&wd, &DockWindow::dockWidgetDropped);
    QVERIFY(spy.isValid());

    // A drag is only let in when it carries one of our payloads.
    {
        QSharedPointer<QMimeData> data(DockDrag::createMimeData("dw2", "Dock 2"));
        QDragEnterEvent enter(QPoint(5, 5), Qt::MoveAction, data.data(), Qt::LeftButton, Qt::NoModifier);
        wd.dragEnterEvent(&enter);
        QCOMPARE(enter.isAccepted(), true);

        QMimeData text;
        text.setText("nothing to do with docking");
        QDragEnterEvent foreign(QPoint(5, 5), Qt::MoveAction, &text, Qt::LeftButton, Qt::NoModifier);
        wd.dragEnterEvent(&foreign);
        QCOMPARE(foreign.isAccepted(), false);

        // Nothing has been dropped yet, so nothing has happened to the window.
        QCOMPARE(spy.count(), 0);
        QCOMPARE(dw2->isClosed(), true);
    }

    // A dock widget of this window that was closed comes back, and the drop is
    // reported with the position it landed on.
    {
        QSharedPointer<QMimeData> data(DockDrag::createMimeData("dw2", "Dock 2"));
        const QPoint pos(100, 50);
        QCOMPARE(wd.handleDockWidgetDrop(data.data(), pos), true);

        QCOMPARE(dw2->isClosed(), false);
        QCOMPARE(spy.count(), 1);
        QCOMPARE(spy.at(0).at(0).toString(), QString("dw2"));
        QCOMPARE(spy.at(0).at(1).value<QPoint>(), wd.mapToGlobal(pos));
    }

    // A dock widget that is here already is left where it is, but the
    // application is still told, so it can decide to move it.
    {
        QSharedPointer<QMimeData> data(DockDrag::createMimeData("dw1", "Dock 1"));
        QCOMPARE(wd.handleDockWidgetDrop(data.data(), QPoint(10, 10)), true);

        QCOMPARE(dw1->isClosed(), false);
        QCOMPARE(spy.count(), 2);
        QCOMPARE(spy.at(1).at(0).toString(), QString("dw1"));
    }

    // A name this window does not know - the drag came from another process -
    // is reported all the same; building it is the application's job.
    {
        QSharedPointer<QMimeData> data(DockDrag::createMimeData("not-here", "Not Here"));
        QCOMPARE(wd.handleDockWidgetDrop(data.data(), QPoint(20, 20)), true);

        QCOMPARE(spy.count(), 3);
        QCOMPARE(spy.at(2).at(0).toString(), QString("not-here"));
    }

    // A payload that is not ours is refused without reporting anything.
    {
        QMimeData text;
        text.setText("just some text");
        QCOMPARE(wd.handleDockWidgetDrop(&text, QPoint(30, 30)), false);
        QCOMPARE(spy.count(), 3);
    }
}

TEST_ADD(tst_DockDrag)

#include "tst_dockdrag.moc"
