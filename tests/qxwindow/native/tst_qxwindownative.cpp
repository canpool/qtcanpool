/*!
 * \file
 *
 * The Windows window procedure, driven through a real window.
 *
 * Everything else in tests/qxwindow/ runs offscreen, which means it never has a
 * window procedure behind it. windowcontext_win.cpp is the largest file in the
 * library and lives entirely on the far side of that line, so this file is the
 * only place any of it is exercised. It is a target of its own rather than a file
 * in tst_qxwindow because it has to run under QT_QPA_PLATFORM=windows while every
 * other suite has to stay on offscreen; with the wrong platform each case skips,
 * so an ordinary offscreen pass reports it as a no-op. See doc/design/4.3-TASKS.md
 * section 8, R5 - this is the construction plan written there, and
 * doc/design/4.4-TASKS.md D2 for the round that built it.
 *
 * Two things about writing this kind of case are worth knowing before reading on,
 * because both were paid for once already:
 *
 *  - A point that goes into WM_NCHITTEST is in device pixels, while Qt hands the
 *    test device independent ones. They only agree at 100% scaling. At 125% a
 *    WM_NCHITTEST built from mapToGlobal's answer lands somewhere else entirely
 *    and reports the wrong region, which looks exactly like a library bug.
 *  - show() maps the size the window carried at that moment and the agent moves
 *    the window to the centre of the screen the first time it is shown, so any
 *    resize that happens before show() is discarded. The geometry the cases are
 *    written against is only real once the window has been shown and resized in
 *    that order.
 *
 * And one thing not to write a case about: the frame is not removed by clearing
 * window style bits. WS_CAPTION survives, which is why the first case measures
 * the client area instead of reading GWL_STYLE. R5 section 2 records the probe
 * that found this.
 */
#include "tst_global.h"

#include "qxwindow/windowagentwidget.h"

#include <QtGui/QGuiApplication>
#include <QtWidgets/QApplication>
#include <QtWidgets/QWidget>

#if defined(Q_OS_WINDOWS)
#include <windows.h>
#endif

QX_WINDOW_USE_NAMESPACE

class tst_QxWindowNative : public QObject
{
    Q_OBJECT
private slots:
#if defined(Q_OS_WINDOWS)
    void clientAreaReachesTheTopEdge();
    void hitTestAnswersForTheTitleBar();
    void hitTestAnswersForTheSystemButtons();
    void theFrameIsRemovedWithoutClearingTheStyleBits();
#endif
};

#if defined(Q_OS_WINDOWS)

namespace
{

constexpr int kWidth = 640;
constexpr int kHeight = 420;
constexpr int kTitleBarHeight = 32;
constexpr int kButtonSize = 32;

/*!
 * Whether the platform plugin behind this run installs a window procedure at all.
 *
 * The whole file is about the one the "windows" plugin installs. The offscreen
 * plugin has none - its windows have no native state to ask about - so every case
 * skips there instead of failing, which is what lets the same binary sit in an
 * ordinary offscreen ctest run without turning it red.
 */
bool onTheWindowsPlatform()
{
    return QGuiApplication::platformName() == QStringLiteral("windows");
}

/*!
 * The four distances between a window's rectangle and its client area, in device
 * pixels.
 *
 * This is the observable WM_NCCALCSIZE leaves behind. The message itself cannot be
 * driven from here: QWKHookedWndProc forwards it to Qt first and only reaches
 * nonClientCalcSizeHandler from the global native event filter, which a hand-sent
 * message never travels through, so SendMessage gets back exactly what it sent.
 * What the handler did is still readable afterwards - the client area either
 * starts at the top of the window or it does not.
 */
struct Insets {
    int left = 0;
    int top = 0;
    int right = 0;
    int bottom = 0;
};

Insets insetsOf(HWND hwnd)
{
    RECT window{};
    RECT client{};
    POINT origin{0, 0};
    if (!::GetWindowRect(hwnd, &window) || !::GetClientRect(hwnd, &client) || !::ClientToScreen(hwnd, &origin)) {
        return {};
    }

    Insets insets;
    insets.left = origin.x - window.left;
    insets.top = origin.y - window.top;
    insets.right = window.right - (origin.x + client.right);
    insets.bottom = window.bottom - (origin.y + client.bottom);
    return insets;
}

/*!
 * HT* values are small integers and their names are the only readable thing about
 * them; a failure saying "expected -4, actual -2" just sends the reader to
 * WinUser.h.
 */
QString hitTestName(LRESULT value)
{
    switch (value) {
    case HTCLIENT:
        return QStringLiteral("HTCLIENT");
    case HTCAPTION:
        return QStringLiteral("HTCAPTION");
    case HTCLOSE:
        return QStringLiteral("HTCLOSE");
    case HTTOP:
        return QStringLiteral("HTTOP");
    // HTMINBUTTON and HTMAXBUTTON are aliases of the older HTREDUCE and HTZOOM
    // spellings of the same two values, so only one name out of each pair can
    // appear here - listing both is a duplicate case label, not two regions.
    case HTMINBUTTON:
        return QStringLiteral("HTMINBUTTON");
    case HTMAXBUTTON:
        return QStringLiteral("HTMAXBUTTON");
    default:
        break;
    }
    return QStringLiteral("HT%1").arg(static_cast<qlonglong>(value));
}

/*!
 * The window the agent is installed on, with a title bar strip and a close button
 * and shown for real. The case using it has to have confirmed the platform first:
 * the constructor hands the widget to the agent, which reaches for a native window
 * handle.
 */
class AgentWindow
{
public:
    AgentWindow()
        : m_titleBar(new QWidget(&m_host))
        , m_closeButton(new QWidget(m_titleBar))
    {
    }

    /*!
     * Returns false only when the agent refuses the widget, which it does for a
     * widget it has already taken over. That is a mistake in the case rather than
     * anything the platform can answer, so the caller turns it into a QVERIFY.
     */
    bool build()
    {
        m_host.resize(kWidth, kHeight);

        if (!m_agent.setup(&m_host)) {
            return false;
        }
        m_agent.setTitleBar(m_titleBar);
        m_agent.setSystemButton(WindowAgentBase::Close, m_closeButton);

        m_host.show();
        QApplication::processEvents();

        // The agent centres the window the first time it is shown, and show() maps
        // the size the window carried at that moment, so this is the earliest the
        // requested size can take effect. It is also not necessarily the size that
        // comes back: Qt adds the frame margins the agent told it about, which makes
        // the window a title bar taller than it was asked for. The cases therefore
        // read the size back rather than assuming theirs was honoured - the
        // assertions are about edges, and an edge is wherever the window ended up.
        m_host.resize(kWidth, kHeight);

        m_titleBar->setGeometry(0, 0, m_host.width(), kTitleBarHeight);
        m_closeButton->setGeometry(m_host.width() - kButtonSize, 0, kButtonSize, kTitleBarHeight);

        // A hidden title bar is ignored by the hit test - it is treated as a window
        // with no title bar at all - so this is not decoration.
        m_titleBar->show();
        m_closeButton->show();

        QApplication::processEvents();
        QTest::qWait(50);
        return true;
    }

    HWND handle() const
    {
        return reinterpret_cast<HWND>(m_host.winId());
    }

    /*!
     * The screen point a WM_NCHITTEST about \a local would carry. The conversion
     * from Qt's device independent coordinates to the device pixels the message
     * works in is explicit here rather than left to whatever the display happens
     * to be scaled to.
     */
    QPoint nativePoint(const QPoint &local) const
    {
        POINT origin{0, 0};
        ::ClientToScreen(handle(), &origin);
        const qreal ratio = m_host.devicePixelRatioF();
        return QPoint(origin.x + qRound(local.x() * ratio), origin.y + qRound(local.y() * ratio));
    }

    /*!
     * Windows' own answer for a point, which is what decides whether a press moves
     * the window, resizes it, or reaches the widget. Sending the message is enough;
     * there is no need to move a real cursor.
     */
    LRESULT hitTest(const QPoint &local) const
    {
        const QPoint point = nativePoint(local);
        return ::SendMessageW(handle(), WM_NCHITTEST, 0, MAKELPARAM(point.x(), point.y()));
    }

    int width() const
    {
        return m_host.width();
    }

    int height() const
    {
        return m_host.height();
    }

    /*! The centre of the close button, in the host's coordinates. */
    QPoint closeButtonCentre() const
    {
        return m_closeButton->geometry().center() + m_titleBar->pos();
    }
private:
    QWidget m_host;
    QWidget *m_titleBar;
    QWidget *m_closeButton;
    WindowAgentWidget m_agent;
};

/*!
 * An ordinary window of the same size, to compare against. On its own, "the client
 * area starts at the top" means nothing - the reader has to know what a window
 * normally looks like before that can be called a change.
 */
class PlainWindow
{
public:
    PlainWindow()
    {
        m_host.resize(kWidth, kHeight);
    }

    void build()
    {
        m_host.show();
        QApplication::processEvents();
        m_host.resize(kWidth, kHeight);
        QApplication::processEvents();
        QTest::qWait(50);
    }

    HWND handle() const
    {
        return reinterpret_cast<HWND>(m_host.winId());
    }
private:
    QWidget m_host;
};

}   // namespace

void tst_QxWindowNative::clientAreaReachesTheTopEdge()
{
    if (!onTheWindowsPlatform()) {
        QSKIP("there is no window procedure to ask under this platform plugin");
    }

    AgentWindow window;
    QVERIFY(window.build());

    PlainWindow plain;
    plain.build();

    const Insets plainInsets = insetsOf(plain.handle());
    const Insets agentInsets = insetsOf(window.handle());

    // The yardstick first. If an ordinary window did not reserve a caption there
    // would be nothing for the agent to have removed, and the case below would pass
    // for the wrong reason.
    QVERIFY2(plainInsets.top > 0, "an ordinary window has no caption to begin with");

    // The whole top frame is gone, which is the point of answering WM_NCCALCSIZE
    // at all: DWM paints the caption there no matter what margin is set, so the
    // only way to lose it is to remove it and draw a line in its place.
    QCOMPARE(agentInsets.top, 0);

#if defined(QX_WINDOW_ENABLE_SYSTEM_BORDERS)
    // The other three edges are deliberately kept so the window keeps the native
    // resizable frame and DWM's shadow; the handler calls DefWindowProc for them
    // and re-applies the original top afterwards. So they must still be there, and
    // they must be equal, because that is the shape a frame has.
    QVERIFY2(agentInsets.left > 0, "the resizable frame should have been kept");
    QCOMPARE(agentInsets.left, agentInsets.right);
    QCOMPARE(agentInsets.left, agentInsets.bottom);
    // ...and the kept frame is the same one an ordinary window has, which is what
    // "we did not invent a frame, we kept this one" means.
    QCOMPARE(agentInsets.left, plainInsets.left);
#else
    // Built without the system borders the frame is dropped whole, so nothing is
    // left on any edge.
    QCOMPARE(agentInsets.left, 0);
    QCOMPARE(agentInsets.right, 0);
    QCOMPARE(agentInsets.bottom, 0);
#endif
}

void tst_QxWindowNative::hitTestAnswersForTheTitleBar()
{
    if (!onTheWindowsPlatform()) {
        QSKIP("there is no window procedure to ask under this platform plugin");
    }

    AgentWindow window;
    QVERIFY(window.build());

    // Clear of the strip at the very top on purpose: the middle of the title bar is
    // the one place where the answer has to come from the title bar being
    // registered, and not from the border arithmetic happening to agree.
    const QPoint titleBarBody(window.width() - 200, kTitleBarHeight / 2);
    QCOMPARE(hitTestName(window.hitTest(titleBarBody)), QStringLiteral("HTCAPTION"));

    // Well below the title bar. This is the answer that lets the widget underneath
    // see the click at all; a caption or a frame edge here would swallow it.
    const QPoint clientArea(window.width() / 2, window.height() / 2);
    QCOMPARE(hitTestName(window.hitTest(clientArea)), QStringLiteral("HTCLIENT"));

    // The top couple of pixels are not the title bar even though the title bar is
    // drawn there: they are the resize strip that replaced the frame the handler
    // removed. Without it a frameless window could only be resized sideways.
    QCOMPARE(hitTestName(window.hitTest(QPoint(window.width() / 2, 1))), QStringLiteral("HTTOP"));
}

void tst_QxWindowNative::hitTestAnswersForTheSystemButtons()
{
    if (!onTheWindowsPlatform()) {
        QSKIP("there is no window procedure to ask under this platform plugin");
    }

    AgentWindow window;
    QVERIFY(window.build());

    // A press on the close button has to be reported as HTCLOSE rather than
    // HTCAPTION, or Windows drags the window instead of pressing the button - and
    // on Windows 11 the same answer is what makes the snap-layout flyout appear.
    // This is the reason the agent takes system buttons at all.
    QCOMPARE(hitTestName(window.hitTest(window.closeButtonCentre())), QStringLiteral("HTCLOSE"));

    // One button's width to the left is title bar again, so the button claims only
    // its own rectangle rather than the rest of the strip.
    const QPoint besideTheButton = window.closeButtonCentre() - QPoint(kButtonSize, 0);
    QCOMPARE(hitTestName(window.hitTest(besideTheButton)), QStringLiteral("HTCAPTION"));
}

void tst_QxWindowNative::theFrameIsRemovedWithoutClearingTheStyleBits()
{
    if (!onTheWindowsPlatform()) {
        QSKIP("there is no window procedure to ask under this platform plugin");
    }

    AgentWindow window;
    QVERIFY(window.build());

    const LONG_PTR style = ::GetWindowLongPtrW(window.handle(), GWL_STYLE);

    // WS_SYSMENU is the one style bit the agent really does change. Left on, the
    // system icon and the system title bar show through when mica is enabled.
    QVERIFY2(!(style & WS_SYSMENU), "the agent should have cleared WS_SYSMENU");

    // WS_CAPTION and WS_THICKFRAME stay, and that is the trap this case exists to
    // write down: the window still has a caption, and the frame is removed by
    // answering WM_NCCALCSIZE rather than by touching the style. A case looking for
    // a cleared WS_CAPTION would assert something that never happens - which is why
    // clientAreaReachesTheTopEdge() reads the geometry instead. R5 section 2 has the
    // probe that found it.
    QVERIFY2(style & WS_CAPTION, "the frame is not removed through the window style");
    QVERIFY2(style & WS_THICKFRAME, "the window keeps the resizable frame bit");
}

#endif   // defined(Q_OS_WINDOWS)

TEST_ADD(tst_QxWindowNative)

#include "tst_qxwindownative.moc"
