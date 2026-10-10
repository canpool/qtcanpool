#include "tst_global.h"

#include "qxwindow/windowagentbase.h"
#include "qxwindow/windowagentwidget.h"

#include <QSignalSpy>
#include <QVariant>
#include <QWidget>

QX_WINDOW_USE_NAMESPACE

/*!
 * What is worth testing in a window agent is the bookkeeping its context does:
 * which widget is the title bar, which one stands in for a system button, and
 * which ones are allowed to receive mouse events. All of that lives in the
 * shared WindowContext and behaves the same on every backend, so it is
 * exercised here against a plain QWidget.
 *
 * Anything that needs a real window manager - dragging the window, the system
 * menu, the Windows 10 border workaround - is deliberately left out. It would
 * differ per platform and only produce CI noise.
 */
class tst_QxWindow : public QObject
{
    Q_OBJECT
private slots:
    void accessorsBeforeSetup();
    void setupOnlyOnce();
    void titleBarRoundTrip();
    void titleBarChangeResetsDependents();
    void systemButtonRoundTrip();
    void hitTestVisible();
    void captionClassName();
    void windowAttributes();
};

/*!
 * A fresh agent has no context host yet, but its accessors must still answer
 * instead of dereferencing a null context.
 */
void tst_QxWindow::accessorsBeforeSetup()
{
    WindowAgentWidget agent;

    QVERIFY(!agent.titleBar());
    QVERIFY(!agent.systemButton(WindowAgentBase::Close));
    QVERIFY(!agent.windowAttribute(QStringLiteral("qxwindow-not-an-attribute")).isValid());
}

/*!
 * Taking over a second widget would leave the first one half configured, so the
 * agent refuses and keeps its original host.
 */
void tst_QxWindow::setupOnlyOnce()
{
    QWidget window;
    WindowAgentWidget agent;

    QVERIFY(agent.setup(&window));

    QWidget other;
    QVERIFY(!agent.setup(&other));
}

void tst_QxWindow::titleBarRoundTrip()
{
    QWidget window;
    WindowAgentWidget agent;
    QVERIFY(agent.setup(&window));

    QSignalSpy spy(&agent, &WindowAgentWidget::titleBarChanged);
    QVERIFY(spy.isValid());

    QWidget titleBar;
    QVERIFY(!agent.titleBar());

    agent.setTitleBar(&titleBar);
    QCOMPARE(agent.titleBar(), &titleBar);
    QCOMPARE(spy.count(), 1);

    // Setting the same widget again is not a change, so nothing is emitted.
    agent.setTitleBar(&titleBar);
    QCOMPARE(agent.titleBar(), &titleBar);
    QCOMPARE(spy.count(), 1);

    QWidget otherTitleBar;
    agent.setTitleBar(&otherTitleBar);
    QCOMPARE(agent.titleBar(), &otherTitleBar);
    QCOMPARE(spy.count(), 2);
}

/*!
 * Everything anchored to the old title bar has to be dropped when the title bar
 * is replaced - a stale system button would be a dangling widget pointer.
 */
void tst_QxWindow::titleBarChangeResetsDependents()
{
    QWidget window;
    WindowAgentWidget agent;
    QVERIFY(agent.setup(&window));

    QWidget titleBar;
    QWidget closeButton;
    agent.setTitleBar(&titleBar);
    agent.setSystemButton(WindowAgentBase::Close, &closeButton);
    agent.setHitTestVisible(&closeButton, true);

    QCOMPARE(agent.systemButton(WindowAgentBase::Close), &closeButton);
    QVERIFY(agent.isHitTestVisible(&closeButton));

    QWidget otherTitleBar;
    agent.setTitleBar(&otherTitleBar);

    QVERIFY(!agent.systemButton(WindowAgentBase::Close));
    QVERIFY(!agent.isHitTestVisible(&closeButton));
}

void tst_QxWindow::systemButtonRoundTrip()
{
    QWidget window;
    WindowAgentWidget agent;
    QVERIFY(agent.setup(&window));

    QWidget titleBar;
    agent.setTitleBar(&titleBar);

    QSignalSpy spy(&agent, &WindowAgentWidget::systemButtonChanged);
    QVERIFY(spy.isValid());

    QWidget closeButton;
    QVERIFY(!agent.systemButton(WindowAgentBase::Close));

    agent.setSystemButton(WindowAgentBase::Close, &closeButton);
    QCOMPARE(agent.systemButton(WindowAgentBase::Close), &closeButton);
    QCOMPARE(spy.count(), 1);

    // Assigning the same widget again is not a change.
    agent.setSystemButton(WindowAgentBase::Close, &closeButton);
    QCOMPARE(spy.count(), 1);

    agent.removeSystemButton(WindowAgentBase::Close);
    QVERIFY(!agent.systemButton(WindowAgentBase::Close));

    // The buttons are independent of each other.
    QWidget minimizeButton;
    agent.setSystemButton(WindowAgentBase::Minimize, &minimizeButton);
    QCOMPARE(agent.systemButton(WindowAgentBase::Minimize), &minimizeButton);
    QVERIFY(!agent.systemButton(WindowAgentBase::Maximize));
}

void tst_QxWindow::hitTestVisible()
{
    QWidget window;
    WindowAgentWidget agent;
    QVERIFY(agent.setup(&window));

    QWidget child;
    QVERIFY(!agent.isHitTestVisible(&child));

    agent.setHitTestVisible(&child, true);
    QVERIFY(agent.isHitTestVisible(&child));

    // Asking twice for the same state is still fine.
    agent.setHitTestVisible(&child, true);
    QVERIFY(agent.isHitTestVisible(&child));

    agent.setHitTestVisible(&child, false);
    QVERIFY(!agent.isHitTestVisible(&child));
}

/*!
 * addCaptionClassName() has no getter, so this only pins down that the call is
 * accepted and leaves the agent alone. The names take effect inside the hit
 * testing, which needs a real window manager.
 */
void tst_QxWindow::captionClassName()
{
    QWidget window;
    WindowAgentWidget agent;
    QVERIFY(agent.setup(&window));

    agent.addCaptionClassName(QStringLiteral("QxWindowTestTitleBar"));
    QVERIFY(!agent.titleBar());
}

/*!
 * The base context keeps window attributes in a map, so an attribute that was
 * never set reads back as an invalid value regardless of the backend.
 */
void tst_QxWindow::windowAttributes()
{
    QWidget window;
    WindowAgentWidget agent;
    QVERIFY(agent.setup(&window));

    const QString unknown = QStringLiteral("qxwindow-not-an-attribute");
    QVERIFY(!agent.windowAttribute(unknown).isValid());

    // Clearing an attribute that was never set is a no-op that succeeds.
    QVERIFY(agent.setWindowAttribute(unknown, QVariant()));
    QVERIFY(!agent.windowAttribute(unknown).isValid());

    // A real attribute is not asserted on: whether the backend accepts it
    // depends on the platform, not on the agent. The call must not crash.
    agent.setWindowAttribute(unknown, 42);
    agent.centralize();
    agent.raise();
    agent.showSystemMenu(QPoint(0, 0));
}

TEST_ADD(tst_QxWindow)

#include "tst_qxwindow.moc"
