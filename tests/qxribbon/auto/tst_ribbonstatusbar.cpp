/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#include "tst_global.h"

#include "qxribbon/ribbonstatusbar.h"

#include <QAction>
#include <QFrame>
#include <QLabel>
#include <QMainWindow>
#include <QSignalSpy>
#include <QToolButton>

QX_RIBBON_USE_NAMESPACE

namespace
{

/// A window with the ribbon status bar in its place, the smallest host the bar
/// can be asked to lay its two sections out in.
///
/// The window is shown before anything is added, which is the order a real host
/// uses too: QStatusBar lays its items out from addWidget() while it is visible,
/// so the geometry below is current as soon as the segment is added.
struct StatusBarShell {
    QMainWindow window;
    RibbonStatusBar *bar;

    StatusBarShell()
        : bar(new RibbonStatusBar())
    {
        window.setStatusBar(bar);
        window.resize(600, 400);
        window.show();
    }
};

QLabel *segment(RibbonStatusBar *bar, const QString &text)
{
    const QList<QLabel *> labels = bar->findChildren<QLabel *>(QStringLiteral("qx_RibbonStatusBarSegment"));
    for (QLabel *label : labels) {
        if (label->text() == text)
            return label;
    }
    return nullptr;
}

QToolButton *permanentButton(RibbonStatusBar *bar, QAction *action)
{
    const QList<QToolButton *> buttons =
        bar->findChildren<QToolButton *>(QStringLiteral("qx_RibbonStatusBarPermanentButton"));
    for (QToolButton *button : buttons) {
        if (button->defaultAction() == action)
            return button;
    }
    return nullptr;
}

}   // namespace

class tst_RibbonStatusBar : public QObject
{
    Q_OBJECT
private slots:
    void objectNames();
    void leftSegments();
    void permanentActions();
    void segmentTracksItsAction();
    void showMessageHidesTheTemporarySection();
};

void tst_RibbonStatusBar::objectNames()
{
    StatusBarShell shell;
    shell.bar->addAction(tr("Ready"));
    shell.bar->addSeparator();

    // The names a theme stylesheet targets; a rename here is a theme change.
    QCOMPARE(shell.bar->objectName(), QStringLiteral("qx_RibbonStatusBar"));
    QVERIFY(shell.bar->findChild<QLabel *>(QStringLiteral("qx_RibbonStatusBarSegment")) != nullptr);
    QVERIFY(shell.bar->findChild<QFrame *>(QStringLiteral("qx_RibbonStatusBarSeparator")) != nullptr);
}

void tst_RibbonStatusBar::leftSegments()
{
    StatusBarShell shell;
    QAction *document = shell.bar->addAction(tr("Ready"));
    QAction *separator = shell.bar->addSeparator();
    QAction *position = shell.bar->addAction(tr("Ln 1, Col 1"));

    // The left section holds the segments in the order they were added, each with
    // the text it was given, and the separator between two of them.
    QVERIFY(document != nullptr);
    QVERIFY(separator != nullptr);
    QVERIFY(position != nullptr);
    QCOMPARE(document->text(), tr("Ready"));
    QVERIFY(separator->isSeparator());

    QLabel *documentLabel = segment(shell.bar, tr("Ready"));
    QLabel *positionLabel = segment(shell.bar, tr("Ln 1, Col 1"));
    QVERIFY(documentLabel != nullptr);
    QVERIFY(positionLabel != nullptr);
    QVERIFY(documentLabel->isVisible());
    QVERIFY(positionLabel->geometry().left() >= documentLabel->geometry().right());
}

void tst_RibbonStatusBar::permanentActions()
{
    StatusBarShell shell;
    shell.bar->addAction(tr("Ready"));
    QAction *normal = new QAction(tr("Normal"), shell.bar);
    QAction *pageLayout = new QAction(tr("Page Layout"), shell.bar);
    shell.bar->addPermanentAction(normal);
    shell.bar->addPermanentAction(pageLayout);

    // Each permanent action gets a button of its own on the right, and clicking it
    // triggers the action the host owns.
    QToolButton *normalButton = permanentButton(shell.bar, normal);
    QToolButton *pageLayoutButton = permanentButton(shell.bar, pageLayout);
    QVERIFY(normalButton != nullptr);
    QVERIFY(pageLayoutButton != nullptr);
    QCOMPARE(normalButton->defaultAction(), normal);

    QLabel *documentLabel = segment(shell.bar, tr("Ready"));
    QVERIFY(documentLabel != nullptr);
    QVERIFY(normalButton->geometry().left() > documentLabel->geometry().right());
    QVERIFY(normalButton->geometry().right() <= shell.bar->width());
    QVERIFY(pageLayoutButton->geometry().left() >= normalButton->geometry().right());

    QSignalSpy triggered(normal, &QAction::triggered);
    QTest::mouseClick(normalButton, Qt::LeftButton);
    QCOMPARE(triggered.count(), 1);
}

void tst_RibbonStatusBar::segmentTracksItsAction()
{
    StatusBarShell shell;
    QAction *document = shell.bar->addAction(tr("Ready"));
    QLabel *label = segment(shell.bar, tr("Ready"));
    QVERIFY(label != nullptr);

    // The segment shows the action's text, so the host updates it through the
    // action rather than by reaching for a label it was never given.
    document->setText(tr("Saving..."));
    QCOMPARE(label->text(), tr("Saving..."));
}

void tst_RibbonStatusBar::showMessageHidesTheTemporarySection()
{
    StatusBarShell shell;
    shell.bar->addAction(tr("Ready"));
    QAction *normal = new QAction(tr("Normal"), shell.bar);
    shell.bar->addPermanentAction(normal);

    QLabel *label = segment(shell.bar, tr("Ready"));
    QToolButton *button = permanentButton(shell.bar, normal);
    QVERIFY(label != nullptr);
    QVERIFY(button != nullptr);

    // A message takes the temporary section and the permanent one keeps its place,
    // which is the whole reason the two sections exist.
    shell.bar->showMessage(tr("Working"), 0);
    QVERIFY(label->isHidden());
    QVERIFY(button->isVisible());

    shell.bar->clearMessage();
    QVERIFY(label->isVisible());
}

TEST_ADD(tst_RibbonStatusBar)

#include "tst_ribbonstatusbar.moc"
