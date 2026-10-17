/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#include "tst_global.h"

#include "qxribbon/ribbonbackstageview.h"
#include "qxribbon/ribbonbar.h"

#include <QAbstractButton>
#include <QAction>
#include <QFrame>
#include <QLabel>
#include <QSignalSpy>
#include <QToolButton>
#include <QWidget>

QX_RIBBON_USE_NAMESPACE

namespace
{

/// A window with a ribbon bar and the backstage that belongs to it, the smallest
/// host the view can be asked to cover.
struct BackstageShell {
    QWidget window;
    RibbonBar *ribbonBar;
    RibbonBackstageView *view;

    BackstageShell()
        : ribbonBar(new RibbonBar(&window))
        , view(new RibbonBackstageView(ribbonBar))
    {
        window.resize(600, 400);
        window.show();
    }
};

QToolButton *entryButton(QWidget *view, QAction *action)
{
    const QList<QToolButton *> buttons = view->findChildren<QToolButton *>();
    for (QToolButton *button : buttons) {
        if (button->defaultAction() == action)
            return button;
    }
    return nullptr;
}

QLabel *makePage(const QString &title)
{
    QLabel *page = new QLabel(title);
    page->setWindowTitle(title);
    return page;
}

}   // namespace

class tst_RibbonBackstageView : public QObject
{
    Q_OBJECT
private slots:
    void openCoversTheHost();
    void objectNames();
    void pages();
    void entries();
    void hostAddedAction();
    void closeButton();
    void escape();
    void closePrevented();
    void followsHostResize();
    void applicationButtonOpens();
};

void tst_RibbonBackstageView::openCoversTheHost()
{
    BackstageShell shell;

    QSignalSpy shown(shell.view, &RibbonBackstageView::aboutToShow);
    QSignalSpy hidden(shell.view, &RibbonBackstageView::aboutToHide);

    shell.view->open();

    // The view takes over the whole host, the ribbon bar and all.
    QCOMPARE(shell.view->parentWidget(), &shell.window);
    QCOMPARE(shell.view->geometry(), QRect(0, 0, shell.window.width(), shell.window.height()));
    QVERIFY(shell.view->isVisible());
    QCOMPARE(shown.count(), 1);
    QCOMPARE(hidden.count(), 0);

    shell.view->close();
    QVERIFY(shell.view->isHidden());
    QCOMPARE(shown.count(), 1);
    QCOMPARE(hidden.count(), 1);
}

void tst_RibbonBackstageView::objectNames()
{
    BackstageShell shell;
    QAction *save = shell.view->addAction(QIcon(), tr("Save"));
    QAction *separator = shell.view->addSeparator();

    // The names the theme stylesheets target; a rename here is a theme change.
    QCOMPARE(shell.view->objectName(), QStringLiteral("qx_RibbonBackstageView"));
    QCOMPARE(shell.view->findChild<QToolButton *>(QStringLiteral("qx_RibbonBackstageCloseButton")) != nullptr, true);
    QCOMPARE(entryButton(shell.view, save)->objectName(), QStringLiteral("qx_RibbonBackstageButton"));
    QCOMPARE(shell.view->findChild<QFrame *>(QStringLiteral("qx_RibbonBackstageSeparator")) != nullptr, true);
    QVERIFY(separator->isSeparator());
}

void tst_RibbonBackstageView::pages()
{
    BackstageShell shell;
    QLabel *home = makePage(tr("Home"));
    QLabel *info = makePage(tr("Info"));
    QAction *homeEntry = shell.view->addPage(home);
    QAction *infoEntry = shell.view->addPage(info);

    // The entry takes its text from the page's window title, and the page starts
    // hidden: nothing is up until the host says what should be.
    QCOMPARE(homeEntry->text(), tr("Home"));
    QCOMPARE(shell.view->activePage(), nullptr);
    QVERIFY(home->isHidden());

    shell.view->open();
    shell.view->setActivePage(home);
    QCOMPARE(shell.view->activePage(), home);
    QVERIFY(!home->isHidden());
    QVERIFY(info->isHidden());
    QCOMPARE(homeEntry->isChecked(), true);

    // Only one page is ever up, and the active page fills the area right of the column.
    shell.view->setActivePage(info);
    QCOMPARE(shell.view->activePage(), info);
    QVERIFY(!info->isHidden());
    QVERIFY(home->isHidden());
    QCOMPARE(infoEntry->isChecked(), true);
    QCOMPARE(homeEntry->isChecked(), false);
    QCOMPARE(info->geometry(), QRect(200, 0, shell.window.width() - 200, shell.window.height()));

    // A widget that is not one of the pages is ignored.
    QLabel *stranger = new QLabel(tr("stranger"));
    shell.view->setActivePage(stranger);
    QCOMPARE(shell.view->activePage(), info);
    delete stranger;
}

void tst_RibbonBackstageView::entries()
{
    BackstageShell shell;
    QAction *save = shell.view->addAction(QIcon(), tr("Save"));
    QAction *open = shell.view->addAction(QIcon(), tr("Open"));
    QAction *separator = shell.view->addSeparator();
    QLabel *home = makePage(tr("Home"));
    QAction *homeEntry = shell.view->addPage(home);

    // The column holds the entries in the order they were added, each below the
    // one before it and inside the column; a separator has no button.
    shell.view->open();
    QRect saveRect = shell.view->actionGeometry(save);
    QRect openRect = shell.view->actionGeometry(open);
    QRect homeRect = shell.view->actionGeometry(homeEntry);
    QVERIFY(saveRect.isValid());
    QVERIFY(openRect.isValid());
    QVERIFY(homeRect.isValid());
    QCOMPARE(saveRect.left(), 0);
    QVERIFY(openRect.top() >= saveRect.bottom());
    QVERIFY(homeRect.top() >= openRect.bottom());
    QVERIFY(saveRect.width() <= 200);
    QCOMPARE(shell.view->actionGeometry(separator), QRect());
    QCOMPARE(shell.view->actionGeometry(open), openRect);

    // A command entry triggers its action.
    QSignalSpy saveTriggered(save, &QAction::triggered);
    QTest::mouseClick(entryButton(shell.view, save), Qt::LeftButton);
    QCOMPARE(saveTriggered.count(), 1);

    // A page entry brings its page up.
    QTest::mouseClick(entryButton(shell.view, homeEntry), Qt::LeftButton);
    QCOMPARE(shell.view->activePage(), home);
}

void tst_RibbonBackstageView::hostAddedAction()
{
    BackstageShell shell;
    // An action the host adds through QWidget::addAction becomes an entry as well.
    QAction *action = new QAction(tr("Host"));
    shell.view->addAction(action);

    shell.view->open();
    QVERIFY(shell.view->actionGeometry(action).isValid());
    QSignalSpy triggered(action, &QAction::triggered);
    QTest::mouseClick(entryButton(shell.view, action), Qt::LeftButton);
    QCOMPARE(triggered.count(), 1);
}

void tst_RibbonBackstageView::closeButton()
{
    BackstageShell shell;
    shell.view->open();
    QToolButton *closeButton = shell.view->findChild<QToolButton *>(QStringLiteral("qx_RibbonBackstageCloseButton"));
    QVERIFY(closeButton != nullptr);

    QTest::mouseClick(closeButton, Qt::LeftButton);
    QVERIFY(shell.view->isHidden());
}

void tst_RibbonBackstageView::escape()
{
    BackstageShell shell;
    shell.view->open();

    QTest::keyClick(shell.view, Qt::Key_Escape);
    QVERIFY(shell.view->isHidden());
}

void tst_RibbonBackstageView::closePrevented()
{
    BackstageShell shell;
    shell.view->addAction(QIcon(), tr("Save"));
    shell.view->open();
    shell.view->setClosePrevented(true);
    QCOMPARE(shell.view->isClosePrevented(), true);

    // Esc and the back button are both refused while the close is prevented.
    QTest::keyClick(shell.view, Qt::Key_Escape);
    QVERIFY(!shell.view->isHidden());
    QTest::mouseClick(shell.view->findChild<QToolButton *>(QStringLiteral("qx_RibbonBackstageCloseButton")),
                      Qt::LeftButton);
    QVERIFY(!shell.view->isHidden());

    shell.view->setClosePrevented(false);
    QTest::keyClick(shell.view, Qt::Key_Escape);
    QVERIFY(shell.view->isHidden());
}

void tst_RibbonBackstageView::followsHostResize()
{
    BackstageShell shell;
    shell.view->open();

    shell.window.resize(800, 600);
    QCOMPARE(shell.view->size(), QSize(800, 600));

    // The active page keeps filling the area right of the column.
    QLabel *home = makePage(tr("Home"));
    shell.view->addPage(home);
    shell.view->setActivePage(home);
    QCOMPARE(home->geometry(), QRect(200, 0, 600, 600));
}

void tst_RibbonBackstageView::applicationButtonOpens()
{
    BackstageShell shell;
    QAbstractButton *appButton = shell.ribbonBar->applicationButton();

    // The panel starts hidden and answers the application button by itself.
    QVERIFY(shell.view->isHidden());
    QTest::mouseClick(appButton, Qt::LeftButton);
    QVERIFY(shell.view->isVisible());

    // A second click on the button the panel answers to closes it again.
    QTest::mouseClick(appButton, Qt::LeftButton);
    QVERIFY(shell.view->isHidden());
}

TEST_ADD(tst_RibbonBackstageView)

#include "tst_ribbonbackstageview.moc"
