/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/

/*!
 * \file
 *
 * The "frameless window cannot be maximised on first show" entry on the
 * limitations page used to be something only a person could notice: open the
 * demo, drag the title bar, watch the window slide instead of restoring. A3 makes
 * it answerable by a test instead, and it pins two different things.
 *
 *  - That this library's own window is not the kind of window that hits it. The
 *    regression needs three conditions at once, and the first one - a window that
 *    asks Qt for Qt::FramelessWindowHint - is the only one the library controls.
 *    The native backend leaves the frame alone and works through the window proc,
 *    which is exactly why RibbonAppWindow is unaffected. This holds offscreen, so
 *    it runs on every Windows leg.
 *  - What the regression looks like when it does happen, stated against a plain
 *    Qt window with no qtcanpool in it at all: after showNormal() the window is
 *    still the size of the screen, because Qt rewrote normalGeometry() with the
 *    maximised rectangle. That needs a window manager, so it skips offscreen -
 *    but it is written as an expected failure on the combination that has it, so
 *    the day Qt fixes the plugin the case reports an unexpected pass rather than
 *    staying quiet.
 *
 * See doc/pages/limitations.md and doc/design/4.3-TASKS.md, A3.
 */
#include "tst_global.h"

#include "qxwindow/windowagentwidget.h"

#include <QGuiApplication>
#include <QWidget>

QX_WINDOW_USE_NAMESPACE

class tst_QxWindowFrameless : public QObject
{
    Q_OBJECT
private slots:
#if defined(Q_OS_WINDOWS)
    void nativeWindowIsNotFrameless();
#endif
    void restoreReturnsThePreMaximisedGeometry();
};

#if defined(Q_OS_WINDOWS)
void tst_QxWindowFrameless::nativeWindowIsNotFrameless()
{
    QWidget window;
    WindowAgentWidget agent;

    QVERIFY(agent.setup(&window));

    // Which backend is in charge is a configure-time answer (QX_WINDOW_CONFIG_NATIVE)
    // and the platform name does not give it - CI runs every leg under offscreen -
    // so the observable difference between the two is used instead: the Qt backend
    // asks Qt for a frameless window and simulates everything on top of it, while
    // the native backend reaches around Qt and never touches the window flags.
    QVERIFY2(!(window.windowFlags() & Qt::FramelessWindowHint),
             "the window is frameless again, which puts it back on the Qt path that "
             "cannot be shown maximised on first show on Windows");
}
#endif

void tst_QxWindowFrameless::restoreReturnsThePreMaximisedGeometry()
{
    // Maximising is the mechanism, so this needs a window manager. The offscreen
    // platform has a screen rectangle but no window manager, and its windows have
    // no native state to disagree with Qt about.
    if (QGuiApplication::platformName() == QStringLiteral("offscreen")) {
        QSKIP("the probe needs a real window system; offscreen has no window manager");
    }

    QWidget window;
    window.resize(400, 300);
    // The first of the three conditions: a frameless window. The second is below -
    // the very first show is the maximised one. The third is the platform itself.
    window.setWindowFlag(Qt::FramelessWindowHint);

    window.showMaximized();
    if (!QTest::qWaitForWindowExposed(&window)) {
        QSKIP("the window could not be mapped here");
    }

    window.showNormal();
    QTest::qWait(200);

#if defined(Q_OS_WINDOWS) && QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    QEXPECT_FAIL("",
                 "Qt 6 on Windows reports WindowNoState after showNormal() while the "
                 "native window is still maximised, and rewrites normalGeometry() with "
                 "the maximised rectangle - so restoring leaves the window full screen",
                 Continue);
#endif
    QCOMPARE(window.geometry().size(), QSize(400, 300));
}

TEST_ADD(tst_QxWindowFrameless)

#include "tst_qxwindowframeless.moc"
