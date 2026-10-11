/*!
 * \file
 *
 * A5 walks the public surface of qxwindow and asks two questions of every entry
 * point: does anything in this repository call it, and does any test cover it.
 * See doc/design/4.3-TASKS.md, A5.
 *
 * The surface is two classes - WindowAgentBase and WindowAgentWidget. tst_qxwindow
 * covers the accessors: title bar, system buttons, hit-test visibility, caption
 * class names, window attributes. What it does not reach is the three members
 * that are not accessors at all but commands into the window context:
 * centralize(), raise() and showSystemMenu().
 *
 * Those three are worth a file of their own for two reasons that pull in opposite
 * directions. They have no caller anywhere in this repository - which is exactly
 * the kind of public API an evaluation has to point at, and equally the kind that
 * cannot be removed on the spot (K26 gives a removal a buffer of at least one
 * release). And they are the part of the surface whose effect is not observable
 * offscreen: each one goes straight to the window context, and offscreen there is
 * no window manager to move, raise or pop a menu for. So what can honestly be
 * asserted here is the half that is ours - that they are safe before a host
 * exists, and that they leave the agent's bookkeeping alone.
 */
#include "tst_global.h"

#include "qxwindow/windowagentbase.h"
#include "qxwindow/windowagentwidget.h"

#include <QPoint>
#include <QWidget>

QX_WINDOW_USE_NAMESPACE

class tst_QxWindowApi : public QObject
{
    Q_OBJECT
private slots:
    void commandsAreSafeBeforeSetup();
    void commandsLeaveTheBookkeepingAlone();
};

/*!
 * A freshly constructed agent already has a context - the accessors answer rather
 * than dereferencing nothing - so the three commands are reachable before any
 * host exists. Each one has to notice that and return, and the agent has to stay
 * usable afterwards: taking a host is still allowed, which is the whole point.
 */
void tst_QxWindowApi::commandsAreSafeBeforeSetup()
{
    WindowAgentWidget agent;

    agent.showSystemMenu(QPoint(0, 0));
    agent.centralize();
    agent.raise();

    // Still the same empty agent, and still able to take a host.
    QVERIFY(!agent.titleBar());
    QVERIFY(!agent.systemButton(WindowAgentBase::Close));
    QVERIFY(!agent.windowAttribute(QStringLiteral("qxwindow-not-an-attribute")).isValid());

    QWidget window;
    QVERIFY(agent.setup(&window));
}

/*!
 * With a host in place, the commands go to the context for real - and on a
 * headless platform that means they are asked to move, raise or pop a menu on a
 * window that has no window manager. What must not happen is any of them
 * disturbing what the agent remembers, because that is what every other part of
 * the library reads back.
 */
void tst_QxWindowApi::commandsLeaveTheBookkeepingAlone()
{
    QWidget window;
    WindowAgentWidget agent;
    QVERIFY(agent.setup(&window));

    QWidget titleBar;
    QWidget closeButton;
    agent.setTitleBar(&titleBar);
    agent.setSystemButton(WindowAgentBase::Close, &closeButton);
    agent.setHitTestVisible(&closeButton, true);
    agent.addCaptionClassName(QStringLiteral("QxWindowApiTestTitleBar"));

    agent.showSystemMenu(QPoint(1, 2));
    agent.centralize();
    agent.raise();

    QCOMPARE(agent.titleBar(), &titleBar);
    QCOMPARE(agent.systemButton(WindowAgentBase::Close), &closeButton);
    QVERIFY(agent.isHitTestVisible(&closeButton));
}

TEST_ADD(tst_QxWindowApi)

#include "tst_qxwindowapi.moc"
