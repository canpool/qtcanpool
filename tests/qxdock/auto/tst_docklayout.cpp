/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#include "tst_global.h"

#include "tst_probe.h"

#include "qxdock/dockcontainer.h"
#include "qxdock/docksplitter.h"
#include "qxdock/dockwidget.h"
#include "qxdock/dockwindow.h"

#include <QApplication>
#include <QBoxLayout>
#include <QSplitter>
#include <QTest>
#include <QVariant>

QX_DOCK_USE_NAMESPACE

namespace
{
/*! The root splitter is protected - a container's own business, and not part of
 *  what an application asks a dock for - so the test reaches it the way a
 *  subclass would. */
class SplitterWindow : public DockWindow
{
public:
    using DockWindow::DockWindow;

    QSplitter *root() const
    {
        return rootSplitter();
    }
};

/*! How long a splitter is along the way it divides things up. */
int lengthOf(QSplitter *splitter)
{
    return splitter->orientation() == Qt::Horizontal ? splitter->width() : splitter->height();
}

/*! What a splitter is holding: every part non-zero, and the parts together
 *  filling the splitter - which is what "measured again" means. A pane that was
 *  handed a size out of the old metrics is either zero or leaves a gap, and
 *  both show up in these two numbers. */
bool fillsItself(QSplitter *splitter)
{
    const QList<int> sizes = splitter->sizes();
    if (sizes.isEmpty()) {
        return false;
    }
    int total = 0;
    for (int size : sizes) {
        if (size <= 0) {
            return false;
        }
        total += size;
    }
    const int handles = (sizes.count() - 1) * splitter->handleWidth();
    return qAbs(total + handles - lengthOf(splitter)) <= 1;
}

QString describe(QSplitter *splitter)
{
    return QStringLiteral("%1 parts %2 in %3 pixels")
        .arg(splitter->count())
        .arg(QVariant::fromValue(splitter->sizes()).toString())
        .arg(lengthOf(splitter));
}
}   // namespace

/*!
 * A docking layout is not a picture: it is a set of sizes that were measured
 * once, and everything wrong with it shows up as a size measured against
 * something that has since changed. These are the scenes that change something
 * and then read the sizes back - a pane joining and leaving the tree, and the
 * chrome growing taller under the panes - because 4.6 ended with a defect of
 * exactly this shape filed as expected rather than fixed: the panes kept the
 * sizes they were divided up with while the chrome was 17 pixels shorter.
 */
class tst_DockLayout : public QObject
{
    Q_OBJECT
private slots:
    void init();
    void cleanup();

    void theRowIsMeasuredAgainWhenAPaneJoins();
    void theRowIsMeasuredAgainWhenAPaneLeaves();
    void aTallerTabBarIsTakenOutOfThePaneUnderIt();
private:
    QFont m_font;
    QString m_sheet;
};

void tst_DockLayout::init()
{
    m_font = qApp->font();
    m_sheet = qApp->styleSheet();
}

void tst_DockLayout::cleanup()
{
    qApp->setFont(m_font);
    qApp->setStyleSheet(m_sheet);
}

/*! A pane joining the tree is not a pane added to the end of a list of sizes:
 *  the row is divided up again, and every part of it is measured again. */
void tst_DockLayout::theRowIsMeasuredAgainWhenAPaneJoins()
{
    QWidget host;
    host.setLayout(new QVBoxLayout);
    SplitterWindow dock(&host);
    host.layout()->addWidget(&dock);
    host.resize(800, 600);

    DockWidget left(QStringLiteral("left"));
    DockWidget center(QStringLiteral("center"));
    dock.addDockWidget(Qx::LeftDockWidgetArea, &left);
    dock.addDockWidget(Qx::CenterDockWidgetArea, &center);
    host.show();
    Probe::settle(&host);

    QSplitter *root = dock.root();
    QVERIFY(root);
    QVERIFY2(fillsItself(root), qPrintable(describe(root)));
    const int panes = Probe::lines(Probe::tree(&host), QStringLiteral("QxDock::DockWidget ")).count();

    DockWidget right(QStringLiteral("right"));
    dock.addDockWidget(Qx::RightDockWidgetArea, &right);
    Probe::settle(&host);

    // The root splitter is not the object it was: a pane joining the tree can
    // rebuild it, and a test holding on to the old one reads a widget that has
    // gone. What stays is the pane, and the room it was given.
    root = dock.root();
    QVERIFY(root);
    QCOMPARE(Probe::lines(Probe::tree(&host), QStringLiteral("QxDock::DockWidget ")).count(), panes + 1);
    QVERIFY(right.isVisible());
    QVERIFY(right.width() > 0);
    QVERIFY(right.height() > 0);
    QVERIFY2(fillsItself(root), qPrintable(describe(root)));
}

/*! And a pane leaving is the same measurement run in the other direction: what
 *  it was holding goes back to the panes that are still there, rather than
 *  staying behind as a gap. */
void tst_DockLayout::theRowIsMeasuredAgainWhenAPaneLeaves()
{
    QWidget host;
    host.setLayout(new QVBoxLayout);
    SplitterWindow dock(&host);
    host.layout()->addWidget(&dock);
    host.resize(800, 600);

    DockWidget left(QStringLiteral("left"));
    DockWidget center(QStringLiteral("center"));
    DockWidget right(QStringLiteral("right"));
    dock.addDockWidget(Qx::LeftDockWidgetArea, &left);
    dock.addDockWidget(Qx::CenterDockWidgetArea, &center);
    dock.addDockWidget(Qx::RightDockWidgetArea, &right);
    host.show();
    Probe::settle(&host);

    QSplitter *root = dock.root();
    QVERIFY(root);
    QVERIFY2(fillsItself(root), qPrintable(describe(root)));
    const int panes = Probe::lines(Probe::tree(&host), QStringLiteral("QxDock::DockWidget ")).count();

    dock.removeDockWidget(&right);
    Probe::settle(&host);

    root = dock.root();
    QVERIFY(root);
    QCOMPARE(Probe::lines(Probe::tree(&host), QStringLiteral("QxDock::DockWidget ")).count(), panes - 1);
    QVERIFY(!right.isVisible());
    QVERIFY2(fillsItself(root), qPrintable(describe(root)));
}

/*! The tab bar is part of what a pane is measured against: when it grows, the
 *  pane under it has to give up exactly that much room. A pane that kept its
 *  old height is a pane whose content is now partly under the tab bar. */
void tst_DockLayout::aTallerTabBarIsTakenOutOfThePaneUnderIt()
{
    QWidget host;
    host.setLayout(new QVBoxLayout);
    SplitterWindow dock(&host);
    host.layout()->addWidget(&dock);
    host.resize(800, 600);

    DockWidget first(QStringLiteral("first"));
    DockWidget second(QStringLiteral("second"));
    QWidget content;
    first.setWidget(&content);
    dock.addDockWidget(Qx::LeftDockWidgetArea, &first);
    dock.addDockWidget(Qx::LeftDockWidgetArea, &second);
    host.show();
    Probe::settle(&host);

    const int tabBarBefore = Probe::heightOf(Probe::tree(&host), QStringLiteral("DockTabBar"));
    const int contentBefore = content.height();
    QVERIFY(tabBarBefore > 0);
    QVERIFY(contentBefore > 0);

    // The chrome is measured from the sheet and not from the font: the tab
    // bar's height is a number the style states. So the way to make it grow is
    // a rule that says so, and the selector a dock answers to is its own base
    // class's - which is what the 4.6 note about ".QFrame" is about.
    qApp->setStyleSheet(QStringLiteral("QFrame { min-height: 40px; }"));
    Probe::settle(&host);

    const int tabBarAfter = Probe::heightOf(Probe::tree(&host), QStringLiteral("DockTabBar"));
    QVERIFY2(tabBarAfter > tabBarBefore, qPrintable(QStringLiteral("the tab bar stayed %1 tall").arg(tabBarBefore)));
    // What the tab bar took is what the pane under it gave up.
    QCOMPARE(contentBefore - content.height(), tabBarAfter - tabBarBefore);
}

TEST_ADD(tst_DockLayout)

#include "tst_docklayout.moc"
