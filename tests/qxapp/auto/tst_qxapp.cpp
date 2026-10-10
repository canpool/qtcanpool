#include "tst_global.h"

#include "qxapp/qxappshell.h"
#include "qxapp/qxnavigationbar.h"
#include "qxapp/qxpluginmanagerdialog.h"
#include "qxapp/qxsplashscreen.h"

#include "qxcore/qxsettings.h"
#include "qxdock/dockwidget.h"
#include "qxdock/dockwindow.h"
#include "qxplugin/qxplugin.h"
#include "qxplugin/qxpluginmanager.h"
#include "qxplugin/qxpluginspec.h"

#include <QtCore/QJsonArray>
#include <QtCore/QJsonObject>
#include <QtCore/QSettings>
#include <QtGui/QIcon>
#include <QtWidgets/QPlainTextEdit>
#include <QtWidgets/QProgressBar>
#include <QtWidgets/QStackedWidget>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QTreeWidget>

QX_APP_USE_NAMESPACE
QX_CORE_USE_NAMESPACE
QX_DOCK_USE_NAMESPACE
QX_PLUGIN_USE_NAMESPACE

/* ------------------------------------------------------------------ rail -- */

/*!
 * The rail is a plain widget: it holds entries, moves the selection and keeps
 * the indices of the button group in step with the list. Nothing in it reaches
 * for a window, so every case runs without a display.
 */
class tst_QxNavigationBar : public QObject
{
    Q_OBJECT
private slots:
    void items();
    void selection();
    void insertionShiftsSelection();
    void removalShiftsSelection();
    void itemProperties();
    void outOfRangeIsHarmless();
};

void tst_QxNavigationBar::items()
{
    QxNavigationBar bar;
    QCOMPARE(bar.count(), 0);
    QCOMPARE(bar.currentIndex(), -1);

    // Adding does not select: the bar carries no policy of its own.
    QCOMPARE(bar.addItem(QIcon(), QStringLiteral("one")), 0);
    QCOMPARE(bar.addItem(QIcon(), QStringLiteral("two")), 1);
    QCOMPARE(bar.count(), 2);
    QCOMPARE(bar.currentIndex(), -1);
    QCOMPARE(bar.itemText(0), QStringLiteral("one"));
    QCOMPARE(bar.itemText(1), QStringLiteral("two"));

    bar.clear();
    QCOMPARE(bar.count(), 0);
    QCOMPARE(bar.itemText(0), QString());
    QVERIFY(bar.button(0) == Q_NULLPTR);

    // An empty icon is still an icon: the entry exists either way.
    QVERIFY(bar.addItem(QIcon(), QStringLiteral("three")) == 0);
    QCOMPARE(bar.count(), 1);
}

void tst_QxNavigationBar::selection()
{
    QxNavigationBar bar;
    bar.addItem(QIcon(), QStringLiteral("one"));
    bar.addItem(QIcon(), QStringLiteral("two"));

    QSignalSpy spy(&bar, &QxNavigationBar::currentChanged);

    bar.setCurrentIndex(1);
    QCOMPARE(bar.currentIndex(), 1);
    QCOMPARE(spy.count(), 1);
    QCOMPARE(spy.at(0).at(0).toInt(), 1);
    QVERIFY(bar.button(1)->isChecked());

    // Selecting the entry that is already selected is not a change.
    bar.setCurrentIndex(1);
    QCOMPARE(spy.count(), 1);

    // -1 clears the selection, and it reaches the buttons.
    bar.setCurrentIndex(-1);
    QCOMPARE(bar.currentIndex(), -1);
    QCOMPARE(spy.count(), 2);
    for (int i = 0; i < bar.count(); ++i) {
        QVERIFY(!bar.button(i)->isChecked());
    }

    // A click takes the same path as setCurrentIndex().
    bar.button(0)->click();
    QCOMPARE(bar.currentIndex(), 0);
    QCOMPARE(spy.count(), 3);
    QCOMPARE(spy.at(2).at(0).toInt(), 0);
}

void tst_QxNavigationBar::insertionShiftsSelection()
{
    QxNavigationBar bar;
    bar.addItem(QIcon(), QStringLiteral("one"));
    bar.addItem(QIcon(), QStringLiteral("two"));
    bar.setCurrentIndex(1);

    QSignalSpy spy(&bar, &QxNavigationBar::currentChanged);

    // An entry inserted in front moves the selection to the next index.
    bar.insertItem(0, QIcon(), QStringLiteral("zero"));
    QCOMPARE(bar.count(), 3);
    QCOMPARE(bar.itemText(0), QStringLiteral("zero"));
    QCOMPARE(bar.itemText(2), QStringLiteral("two"));
    QCOMPARE(bar.currentIndex(), 2);
    QCOMPARE(spy.count(), 1);
    QCOMPARE(spy.at(0).at(0).toInt(), 2);
    QVERIFY(bar.button(2)->isChecked());

    // An entry inserted behind it leaves the selection alone.
    bar.insertItem(3, QIcon(), QStringLiteral("three"));
    QCOMPARE(bar.count(), 4);
    QCOMPARE(bar.currentIndex(), 2);
    QCOMPARE(spy.count(), 1);
}

void tst_QxNavigationBar::removalShiftsSelection()
{
    QxNavigationBar bar;
    bar.addItem(QIcon(), QStringLiteral("one"));
    bar.addItem(QIcon(), QStringLiteral("two"));
    bar.addItem(QIcon(), QStringLiteral("three"));
    bar.setCurrentIndex(2);

    QSignalSpy spy(&bar, &QxNavigationBar::currentChanged);

    // Removing an entry in front moves the selection down.
    bar.removeItem(0);
    QCOMPARE(bar.count(), 2);
    QCOMPARE(bar.itemText(0), QStringLiteral("two"));
    QCOMPARE(bar.currentIndex(), 1);
    QCOMPARE(spy.count(), 1);
    QCOMPARE(spy.at(0).at(0).toInt(), 1);

    // Removing the selected entry clears the selection.
    bar.removeItem(1);
    QCOMPARE(bar.count(), 1);
    QCOMPARE(bar.currentIndex(), -1);
    QCOMPARE(spy.count(), 2);
    QCOMPARE(spy.at(1).at(0).toInt(), -1);
}

void tst_QxNavigationBar::itemProperties()
{
    QxNavigationBar bar;
    bar.setIconSize(QSize(32, 32));
    bar.addItem(QIcon(), QStringLiteral("one"));

    QCOMPARE(bar.iconSize(), QSize(32, 32));
    QCOMPARE(bar.button(0)->iconSize(), QSize(32, 32));

    bar.setItemText(0, QStringLiteral("renamed"));
    QCOMPARE(bar.itemText(0), QStringLiteral("renamed"));

    QCOMPARE(bar.isItemEnabled(0), true);
    bar.setItemEnabled(0, false);
    QCOMPARE(bar.isItemEnabled(0), false);
    bar.setItemEnabled(0, true);
    QCOMPARE(bar.isItemEnabled(0), true);

    bar.setItemToolTip(0, QStringLiteral("tip"));
    QCOMPARE(bar.button(0)->toolTip(), QStringLiteral("tip"));

    // The rail keeps its width; the window grows around it.
    QCOMPARE(bar.sizePolicy().horizontalPolicy(), QSizePolicy::Fixed);
}

void tst_QxNavigationBar::outOfRangeIsHarmless()
{
    QxNavigationBar bar;
    bar.addItem(QIcon(), QStringLiteral("one"));

    bar.removeItem(-1);
    bar.removeItem(5);
    bar.insertItem(9, QIcon(), QStringLiteral("nope"));
    bar.setCurrentIndex(3);
    bar.setCurrentIndex(-2);
    bar.setItemText(4, QStringLiteral("nope"));
    bar.setItemIcon(4, QIcon());
    bar.setItemEnabled(4, false);
    bar.setItemToolTip(4, QStringLiteral("nope"));

    QCOMPARE(bar.count(), 1);
    QCOMPARE(bar.currentIndex(), -1);
    QVERIFY(bar.button(4) == Q_NULLPTR);
    QCOMPARE(bar.itemText(4), QString());
    QCOMPARE(bar.itemIcon(4).isNull(), true);
    QCOMPARE(bar.isItemEnabled(4), false);
}

/* ----------------------------------------------------------------- splash -- */

class tst_QxSplashScreen : public QObject
{
    Q_OBJECT
private slots:
    void defaults();
    void progressIsClamped();
    void stepUpdatesBothFields();
    void nullLogoStillHasASurface();
    void rendersAndFinishes();
};

void tst_QxSplashScreen::defaults()
{
    QxSplashScreen splash;
    QCOMPARE(splash.applicationName(), QString());
    QCOMPARE(splash.version(), QString());
    QCOMPARE(splash.message(), QString());
    QCOMPARE(splash.progress(), -1);
}

void tst_QxSplashScreen::progressIsClamped()
{
    QxSplashScreen splash;

    splash.setProgress(40);
    QCOMPARE(splash.progress(), 40);

    splash.setProgress(1000);
    QCOMPARE(splash.progress(), 100);

    // A negative value is the documented "unknown", not a bar at zero.
    splash.setProgress(-5);
    QCOMPARE(splash.progress(), -1);
}

void tst_QxSplashScreen::stepUpdatesBothFields()
{
    QxSplashScreen splash(QPixmap(), QStringLiteral("MyApp"), QStringLiteral("3.0"));
    QCOMPARE(splash.applicationName(), QStringLiteral("MyApp"));
    QCOMPARE(splash.version(), QStringLiteral("3.0"));

    splash.step(25, QStringLiteral("Loading settings..."));
    QCOMPARE(splash.progress(), 25);
    QCOMPARE(splash.message(), QStringLiteral("Loading settings..."));

    // An empty message keeps the one on screen.
    splash.step(50);
    QCOMPARE(splash.progress(), 50);
    QCOMPARE(splash.message(), QStringLiteral("Loading settings..."));

    splash.setApplicationName(QStringLiteral("Other"));
    QCOMPARE(splash.applicationName(), QStringLiteral("Other"));
    splash.setVersion(QStringLiteral("3.1"));
    QCOMPARE(splash.version(), QStringLiteral("3.1"));
}

void tst_QxSplashScreen::nullLogoStillHasASurface()
{
    // Without artwork the splash falls back to a plain plate, otherwise there
    // would be nothing to draw the progress on.
    QxSplashScreen splash;
    QCOMPARE(splash.pixmap().size(), QSize(480, 300));
    QVERIFY(!splash.pixmap().isNull());
}

void tst_QxSplashScreen::rendersAndFinishes()
{
    QxSplashScreen splash(QPixmap(), QStringLiteral("MyApp"), QStringLiteral("3.0"));
    splash.show();
    splash.step(60, QStringLiteral("Halfway"));
    QVERIFY(splash.isVisible());

    splash.finish(Q_NULLPTR);
    QVERIFY(!splash.isVisible());
}

/* ------------------------------------------------------------------ shell -- */

class tst_QxAppShell : public QObject
{
    Q_OBJECT
private slots:
    void initTestCase();

    void emptyShell();
    void pages();
    void pageSelection();
    void removePage();
    void docks();
    void statusBar();
    void progress();
    void busyAndProgressShareOneIndicator();
    void persistence();
    void autoSaveOnClose();
private:
    /*! A settings file of its own per case, so no case depends on another. */
    QString newSettingsFile();

    QTemporaryDir m_dir;
    int m_sequence = 0;
};

void tst_QxAppShell::initTestCase()
{
    QVERIFY(m_dir.isValid());
}

QString tst_QxAppShell::newSettingsFile()
{
    return m_dir.filePath(QStringLiteral("shell-%1.ini").arg(++m_sequence));
}

void tst_QxAppShell::emptyShell()
{
    QxAppShell shell;

    QVERIFY(shell.navigationBar() != Q_NULLPTR);
    QVERIFY(shell.dockWindow() != Q_NULLPTR);
    QCOMPARE(shell.pageCount(), 0);
    QCOMPARE(shell.currentPageIndex(), -1);
    QCOMPARE(shell.currentPageId(), QString());
    QVERIFY(shell.currentPage() == Q_NULLPTR);
    QCOMPARE(shell.indexOfPage(QStringLiteral("nope")), -1);
    QCOMPARE(shell.page(QStringLiteral("nope")), static_cast<QWidget *>(Q_NULLPTR));
    QCOMPARE(shell.pageId(0), QString());

    QCOMPARE(shell.statusMessage(), QString());
    QCOMPARE(shell.isBusy(), false);
    QVERIFY(shell.autoSaveLayout());
    QVERIFY(shell.settings() != Q_NULLPTR);
}

void tst_QxAppShell::pages()
{
    QxAppShell shell;
    QSignalSpy spy(&shell, &QxAppShell::currentPageChanged);

    QWidget *first = new QWidget;
    shell.addPage(QStringLiteral("one"), QIcon(), QStringLiteral("One"), first);
    QCOMPARE(shell.pageCount(), 1);
    QCOMPARE(shell.page(QStringLiteral("one")), first);
    QCOMPARE(shell.indexOfPage(QStringLiteral("one")), 0);
    QCOMPARE(shell.pageId(0), QStringLiteral("one"));

    // The very first page has nothing to compete with and becomes current.
    QCOMPARE(shell.currentPageIndex(), 0);
    QCOMPARE(shell.currentPageId(), QStringLiteral("one"));
    QCOMPARE(shell.currentPage(), first);
    QCOMPARE(spy.count(), 1);

    // The rail mirrors the pages.
    QCOMPARE(shell.navigationBar()->count(), 1);
    QCOMPARE(shell.navigationBar()->itemText(0), QStringLiteral("One"));
    QCOMPARE(shell.navigationBar()->currentIndex(), 0);

    // A duplicate id and an empty id are refused, and the rejected widget stays
    // with the caller.
    QWidget *rejected = new QWidget;
    shell.addPage(QStringLiteral("one"), QIcon(), QStringLiteral("Again"), rejected);
    QCOMPARE(shell.pageCount(), 1);
    QVERIFY(rejected->parentWidget() == Q_NULLPTR);
    delete rejected;

    rejected = new QWidget;
    shell.addPage(QString(), QIcon(), QStringLiteral("Nameless"), rejected);
    QCOMPARE(shell.pageCount(), 1);
    QVERIFY(rejected->parentWidget() == Q_NULLPTR);
    delete rejected;

    // A page added behind the visible one does not move the selection.
    shell.addPage(QStringLiteral("two"), QIcon(), QStringLiteral("Two"), new QWidget);
    QCOMPARE(shell.pageCount(), 2);
    QCOMPARE(shell.currentPageId(), QStringLiteral("one"));
    QCOMPARE(spy.count(), 1);

    // A page inserted in front shifts the index of the visible page, and the
    // signal reports the new pair.
    shell.insertPage(0, QStringLiteral("zero"), QIcon(), QStringLiteral("Zero"), new QWidget);
    QCOMPARE(shell.pageCount(), 3);
    QCOMPARE(shell.pageId(0), QStringLiteral("zero"));
    QCOMPARE(shell.pageId(1), QStringLiteral("one"));
    QCOMPARE(shell.currentPageId(), QStringLiteral("one"));
    QCOMPARE(shell.currentPageIndex(), 1);
    QCOMPARE(spy.count(), 2);
    QCOMPARE(spy.at(1).at(0).toInt(), 1);
    QCOMPARE(spy.at(1).at(1).toString(), QStringLiteral("one"));

    // Pages really live in a stack, and the visible one is on top.
    QStackedWidget *stack = qobject_cast<QStackedWidget *>(first->parentWidget());
    QVERIFY(stack != Q_NULLPTR);
    QCOMPARE(stack->count(), 3);
    QCOMPARE(stack->currentWidget(), first);

    // Out-of-range insertions are ignored.
    shell.insertPage(9, QStringLiteral("late"), QIcon(), QStringLiteral("Late"), new QWidget);
    QCOMPARE(shell.pageCount(), 3);
}

void tst_QxAppShell::pageSelection()
{
    QxAppShell shell;
    shell.addPage(QStringLiteral("one"), QIcon(), QStringLiteral("One"), new QWidget);
    shell.addPage(QStringLiteral("two"), QIcon(), QStringLiteral("Two"), new QWidget);
    QCOMPARE(shell.currentPageId(), QStringLiteral("one"));

    QSignalSpy spy(&shell, &QxAppShell::currentPageChanged);

    // Driving the rail and driving the API lead to the same state.
    shell.navigationBar()->setCurrentIndex(1);
    QCOMPARE(shell.currentPageId(), QStringLiteral("two"));
    QCOMPARE(shell.currentPageIndex(), 1);
    QCOMPARE(spy.count(), 1);

    shell.setCurrentPage(QStringLiteral("one"));
    QCOMPARE(shell.currentPageIndex(), 0);
    QCOMPARE(shell.navigationBar()->currentIndex(), 0);
    QCOMPARE(spy.count(), 2);

    // Bringing the visible page to the front again is not a change.
    shell.setCurrentPage(QStringLiteral("one"));
    QCOMPARE(spy.count(), 2);

    // Unknown ids and out-of-range indices are ignored.
    shell.setCurrentPage(QStringLiteral("nope"));
    shell.setCurrentPage(7);
    QCOMPARE(shell.currentPageId(), QStringLiteral("one"));
    QCOMPARE(shell.currentPageIndex(), 0);
    QCOMPARE(spy.count(), 2);
}

void tst_QxAppShell::removePage()
{
    QxAppShell shell;
    QWidget *one = new QWidget;
    QWidget *two = new QWidget;
    QWidget *three = new QWidget;
    shell.addPage(QStringLiteral("one"), QIcon(), QStringLiteral("One"), one);
    shell.addPage(QStringLiteral("two"), QIcon(), QStringLiteral("Two"), two);
    shell.addPage(QStringLiteral("three"), QIcon(), QStringLiteral("Three"), three);

    QSignalSpy spy(&shell, &QxAppShell::currentPageChanged);
    shell.setCurrentPage(QStringLiteral("three"));
    QCOMPARE(spy.count(), 1);

    // Removing a page in front of the visible one only moves its index, and the
    // signal tells the caller about the new one.
    shell.removePage(QStringLiteral("two"));
    QCOMPARE(shell.pageCount(), 2);
    QCOMPARE(shell.currentPageId(), QStringLiteral("three"));
    QCOMPARE(shell.currentPageIndex(), 1);
    QCOMPARE(shell.navigationBar()->count(), 2);
    QCOMPARE(shell.navigationBar()->currentIndex(), 1);
    QCOMPARE(spy.count(), 2);
    QCOMPARE(spy.at(1).at(0).toInt(), 1);

    // The page handed back is still alive but no longer part of the shell.
    QVERIFY(two->parentWidget() == Q_NULLPTR);
    delete two;

    // Removing the visible page falls back to the one that took its place.
    shell.removePage(QStringLiteral("three"));
    QCOMPARE(shell.currentPageId(), QStringLiteral("one"));
    QCOMPARE(shell.currentPageIndex(), 0);
    QCOMPARE(shell.navigationBar()->currentIndex(), 0);
    QCOMPARE(spy.count(), 3);
    QVERIFY(three->parentWidget() == Q_NULLPTR);
    delete three;

    // The last page leaves an empty shell behind.
    shell.removePage(QStringLiteral("one"));
    QCOMPARE(shell.pageCount(), 0);
    QCOMPARE(shell.currentPageIndex(), -1);
    QCOMPARE(shell.currentPageId(), QString());
    QCOMPARE(shell.navigationBar()->count(), 0);
    QCOMPARE(shell.navigationBar()->currentIndex(), -1);
    QCOMPARE(spy.count(), 4);

    // Unknown ids are ignored.
    shell.removePage(QStringLiteral("nope"));
    QCOMPARE(shell.pageCount(), 0);
}

void tst_QxAppShell::docks()
{
    QxAppShell shell;

    DockWidget *explorer =
        shell.addDock(Qx::LeftDockWidgetArea, QStringLiteral("explorer"), QStringLiteral("Explorer"), new QTreeWidget);
    QVERIFY(explorer != Q_NULLPTR);
    QCOMPARE(explorer->objectName(), QStringLiteral("explorer"));
    QCOMPARE(shell.dock(QStringLiteral("explorer")), explorer);
    QCOMPARE(shell.dockWindow()->findDockWidget(QStringLiteral("explorer")), explorer);

    // A dock without an id, or one that reuses an id, is refused.
    QVERIFY(shell.addDock(Qx::LeftDockWidgetArea, QStringLiteral("explorer"), QStringLiteral("Again"), Q_NULLPTR) ==
            Q_NULLPTR);
    QWidget *orphan = new QWidget;
    QVERIFY(shell.addDock(Qx::LeftDockWidgetArea, QString(), QStringLiteral("Nameless"), orphan) == Q_NULLPTR);
    QVERIFY(orphan->parentWidget() == Q_NULLPTR);
    delete orphan;

    QVERIFY(shell.dock(QStringLiteral("nope")) == Q_NULLPTR);

    DockWidget *output =
        shell.addDock(Qx::BottomDockWidgetArea, QStringLiteral("output"), QStringLiteral("Output"), new QPlainTextEdit);
    QVERIFY(output != Q_NULLPTR);
    QVERIFY(output != explorer);

    // The workspace of the shell is a dock widget of its own, hence the extra
    // entry in the map.
    QCOMPARE(shell.dockWindow()->dockWidgetsMap().count(), 3);
}

void tst_QxAppShell::statusBar()
{
    QxAppShell shell;
    QVERIFY(shell.statusBar() != Q_NULLPTR);

    QCOMPARE(shell.statusMessage(), QString());
    shell.setStatusMessage(QStringLiteral("Loading..."));
    QCOMPARE(shell.statusMessage(), QStringLiteral("Loading..."));

    QCOMPARE(shell.isBusy(), false);
    shell.setBusy(true);
    QCOMPARE(shell.isBusy(), true);
    QVERIFY(shell.statusBar()->findChild<QProgressBar *>() != Q_NULLPTR);

    shell.setBusy(false);
    QCOMPARE(shell.isBusy(), false);

    // The message survives a busy round trip: they are different widgets.
    QCOMPARE(shell.statusMessage(), QStringLiteral("Loading..."));
}

void tst_QxAppShell::progress()
{
    QxAppShell shell;

    QCOMPARE(shell.progressMinimum(), 0);
    QCOMPARE(shell.progressMaximum(), 100);
    QCOMPARE(shell.isProgressVisible(), false);

    // Setting a progress means wanting it seen, so there is no separate call
    // for that.
    shell.setProgress(40);
    QCOMPARE(shell.progress(), 40);
    QCOMPARE(shell.isProgressVisible(), true);

    // Out of range is clamped, not refused: a caller counting files should not
    // have to guard the last one.
    shell.setProgress(1000);
    QCOMPARE(shell.progress(), 100);
    shell.setProgress(-5);
    QCOMPARE(shell.progress(), 0);

    shell.setProgressRange(0, 1000);
    QCOMPARE(shell.progressMinimum(), 0);
    QCOMPARE(shell.progressMaximum(), 1000);
    shell.setProgress(250);
    QCOMPARE(shell.progress(), 250);

    // A range with no room in it is the busy indicator's own signal.
    shell.setProgressRange(0, 0);
    QCOMPARE(shell.progressMaximum(), 1000);

    shell.clearProgress();
    QCOMPARE(shell.isProgressVisible(), false);
    QCOMPARE(shell.progress(), 0);
}

void tst_QxAppShell::busyAndProgressShareOneIndicator()
{
    QxAppShell shell;

    shell.setBusy(true);
    QCOMPARE(shell.isBusy(), true);
    QCOMPARE(shell.isProgressVisible(), true);

    // A percentage says more than "working", so it takes the indicator over.
    shell.setProgress(30);
    QCOMPARE(shell.isBusy(), false);
    QCOMPARE(shell.progress(), 30);
    QCOMPARE(shell.isProgressVisible(), true);

    // Going busy takes it back; the last call is the one worth looking at.
    shell.setBusy(true);
    QCOMPARE(shell.isBusy(), true);
    QCOMPARE(shell.isProgressVisible(), true);
    shell.setBusy(false);
    QCOMPARE(shell.isBusy(), false);
    QCOMPARE(shell.isProgressVisible(), false);

    // The message line is its own widget: "what" and "how far" are not the same
    // answer, and both are on screen at once.
    shell.setStatusMessage(QStringLiteral("Copying..."));
    shell.setProgress(10);
    QCOMPARE(shell.statusMessage(), QStringLiteral("Copying..."));
    shell.clearProgress();
    QCOMPARE(shell.statusMessage(), QStringLiteral("Copying..."));
}

void tst_QxAppShell::persistence()
{
    const QString file = newSettingsFile();

    {
        QxAppShell shell;
        shell.setSettings(new QxSettings(file, QSettings::IniFormat));
        shell.addPage(QStringLiteral("one"), QIcon(), QStringLiteral("One"), new QWidget);
        shell.addPage(QStringLiteral("two"), QIcon(), QStringLiteral("Two"), new QWidget);
        shell.setCurrentPage(QStringLiteral("two"));
        shell.addDock(Qx::BottomDockWidgetArea, QStringLiteral("output"), QStringLiteral("Output"), new QPlainTextEdit);
        shell.saveLayout();
    }

    // The keys are part of the contract, and the page is stored by id, not by
    // index: inserting a page must not change which page comes back.
    QxSettings reader(file, QSettings::IniFormat);
    reader.beginGroup(QStringLiteral("ui"));
    QCOMPARE(reader.stringValue(QStringLiteral("currentPage")), QStringLiteral("two"));
    QVERIFY(!reader.value(QStringLiteral("windowGeometry")).toByteArray().isEmpty());
    QVERIFY(!reader.value(QStringLiteral("dockState")).toByteArray().isEmpty());
    reader.endGroup();

    QxAppShell restored;
    restored.setSettings(new QxSettings(file, QSettings::IniFormat));
    restored.addPage(QStringLiteral("one"), QIcon(), QStringLiteral("One"), new QWidget);
    restored.addPage(QStringLiteral("two"), QIcon(), QStringLiteral("Two"), new QWidget);
    restored.restoreLayout();
    QCOMPARE(restored.currentPageId(), QStringLiteral("two"));

    // A stored page that no longer exists is skipped instead of crashing.
    QxAppShell other;
    other.setSettings(new QxSettings(file, QSettings::IniFormat));
    other.addPage(QStringLiteral("home"), QIcon(), QStringLiteral("Home"), new QWidget);
    other.restoreLayout();
    QCOMPARE(other.currentPageId(), QStringLiteral("home"));
}

void tst_QxAppShell::autoSaveOnClose()
{
    const QString file = newSettingsFile();

    {
        QxAppShell shell;
        shell.setSettings(new QxSettings(file, QSettings::IniFormat));
        shell.addPage(QStringLiteral("one"), QIcon(), QStringLiteral("One"), new QWidget);
        shell.addPage(QStringLiteral("two"), QIcon(), QStringLiteral("Two"), new QWidget);
        shell.setCurrentPage(QStringLiteral("two"));

        // The window is shown before it is closed, the way an application uses
        // it. Once a widget owns a QWindow, close() routes through
        // QWindow::close(), which returns without delivering a close event
        // while the window has no platform window yet -- the state of a window
        // that has been created but never shown. Showing it first keeps the
        // close event synchronous, and therefore the save that hangs off it.
        shell.show();
        QVERIFY(shell.autoSaveLayout());
        QVERIFY(shell.close());
    }

    QxSettings reader(file, QSettings::IniFormat);
    reader.beginGroup(QStringLiteral("ui"));
    QCOMPARE(reader.stringValue(QStringLiteral("currentPage")), QStringLiteral("two"));
    reader.endGroup();

    // With the automatic save off, closing the window writes nothing.
    const QString other = newSettingsFile();
    {
        QxAppShell shell;
        shell.setSettings(new QxSettings(other, QSettings::IniFormat));
        shell.addPage(QStringLiteral("one"), QIcon(), QStringLiteral("One"), new QWidget);
        shell.setCurrentPage(QStringLiteral("one"));
        shell.setAutoSaveLayout(false);
        QCOMPARE(shell.autoSaveLayout(), false);

        shell.show();
        QVERIFY(shell.close());
    }

    QxSettings untouched(other, QSettings::IniFormat);
    QVERIFY(!untouched.contains(QStringLiteral("ui/currentPage")));
}

/* --------------------------------------------------------------- plugin -- */

/*! A plugin that always starts, so the dialog cases can look at the bookkeeping. */
class ReadyPlugin : public QxPlugin
{
    Q_OBJECT
public:
    explicit ReadyPlugin(QObject *parent = Q_NULLPTR)
        : QxPlugin(parent)
    {
    }

    bool initialize(QxPluginContext *, QString *) override
    {
        return true;
    }
};

/*! A plugin.json-shaped metadata object, the same one add_qtc_plugin produces. */
static QJsonObject pluginMeta(const QString &name, const QStringList &deps = QStringList(),
                              bool enabledByDefault = true)
{
    QJsonObject meta;
    meta.insert(QStringLiteral("Name"), name);
    meta.insert(QStringLiteral("Version"), QStringLiteral("1.0.0"));
    meta.insert(QStringLiteral("EnabledByDefault"), enabledByDefault);
    if (!deps.isEmpty()) {
        QJsonArray array;
        for (const QString &dep : deps)
            array.append(QJsonObject{{QStringLiteral("Name"), dep}});
        meta.insert(QStringLiteral("Dependencies"), array);
    }
    return meta;
}

class tst_QxPluginManagerDialog : public QObject
{
    Q_OBJECT
private slots:
    void initTestCase();

    void listsEveryPluginInLoadOrder();
    void switchesFollowTheMetadata();
    void togglingStoresDeviationsOnly();
    void switchesSurviveARestart();
    void diagnosticsCarryTheManagerError();
    void storedListsRoundTripThroughTheHelpers();
private:
    /*! a -> b -> c, plus an opt-in plugin that is off until it is asked for. */
    static void registerPlugins(QxPluginManager &manager);

    /*! A settings file of its own per case, so no case depends on another. */
    QString newSettingsFile();

    QTemporaryDir m_dir;
    int m_sequence = 0;
};

void tst_QxPluginManagerDialog::initTestCase()
{
    QVERIFY(m_dir.isValid());
}

QString tst_QxPluginManagerDialog::newSettingsFile()
{
    return m_dir.filePath(QStringLiteral("plugins-%1.ini").arg(++m_sequence));
}

void tst_QxPluginManagerDialog::registerPlugins(QxPluginManager &manager)
{
    const auto ready = []() {
        return new ReadyPlugin;
    };
    manager.registerStaticPlugin(QStringLiteral("a"), pluginMeta(QStringLiteral("a")), ready);
    manager.registerStaticPlugin(QStringLiteral("b"), pluginMeta(QStringLiteral("b"), {QStringLiteral("a")}), ready);
    manager.registerStaticPlugin(QStringLiteral("c"), pluginMeta(QStringLiteral("c"), {QStringLiteral("b")}), ready);
    // Off by default: it only starts once it is asked for by name, which is the
    // case the enable list exists for.
    manager.registerStaticPlugin(QStringLiteral("optin"), pluginMeta(QStringLiteral("optin"), {}, false), ready);
}

void tst_QxPluginManagerDialog::listsEveryPluginInLoadOrder()
{
    QxPluginManager manager;
    registerPlugins(manager);
    manager.loadPlugins();
    QVERIFY(!manager.hasError());

    QxPluginManagerDialog dialog(&manager);

    // Every plugin the manager knows about, the switched-off one included: that
    // one is exactly what someone opens this dialog to look at.
    QCOMPARE(dialog.count(), 4);
    QCOMPARE(dialog.rowOf(QStringLiteral("a")), 0);
    QCOMPARE(dialog.rowOf(QStringLiteral("b")), 1);
    QCOMPARE(dialog.rowOf(QStringLiteral("c")), 2);
    // The ones that load come first, in the order they load in; the opt-in
    // plugin, which does not load, follows by name.
    QCOMPARE(dialog.rowOf(QStringLiteral("optin")), 3);
    QCOMPARE(dialog.rowOf(QStringLiteral("nope")), -1);

    dialog.setCurrentPlugin(QStringLiteral("b"));
    QCOMPARE(dialog.rowOf(QStringLiteral("b")), 1);

    // No manager is not a crash, it is an empty list.
    QxPluginManagerDialog empty(Q_NULLPTR);
    QCOMPARE(empty.count(), 0);
    QCOMPARE(empty.diagnostics(), QStringLiteral("No errors."));
}

void tst_QxPluginManagerDialog::switchesFollowTheMetadata()
{
    QxPluginManager manager;
    registerPlugins(manager);
    manager.loadPlugins();

    QxPluginManagerDialog dialog(&manager);
    QCOMPARE(dialog.isChecked(QStringLiteral("a")), true);
    // Off by default and nothing said otherwise: the switch is off.
    QCOMPARE(dialog.isChecked(QStringLiteral("optin")), false);
    QCOMPARE(dialog.isChecked(QStringLiteral("nope")), false);
    QVERIFY(!dialog.isModified());

    // An unknown id is refused rather than invented.
    dialog.setChecked(QStringLiteral("nope"), true);
    QCOMPARE(dialog.isChecked(QStringLiteral("nope")), false);
    QVERIFY(!dialog.isModified());

    // Flipping a switch is a change on screen, not a change on disk.
    dialog.setChecked(QStringLiteral("a"), false);
    QVERIFY(dialog.isModified());
}

void tst_QxPluginManagerDialog::togglingStoresDeviationsOnly()
{
    const QString file = newSettingsFile();
    QxPluginManager manager;
    registerPlugins(manager);
    manager.loadPlugins();

    QxSettings settings(file, QSettings::IniFormat);
    QxPluginManagerDialog dialog(&manager, &settings);

    // An untouched dialog stores nothing, so the metadata stays the default a
    // fresh installation starts from.
    dialog.apply();
    QVERIFY(!settings.contains(QStringLiteral("plugins/disabled")));
    QVERIFY(!settings.contains(QStringLiteral("plugins/enabled")));

    // Switching off a plugin the metadata turned on, and on one it turned off:
    // two deviations, one per list.
    dialog.setChecked(QStringLiteral("c"), false);
    dialog.setChecked(QStringLiteral("optin"), true);
    QVERIFY(dialog.isModified());

    QSignalSpy spy(&dialog, &QxPluginManagerDialog::applied);
    dialog.apply();
    QCOMPARE(spy.count(), 1);
    QVERIFY(!dialog.isModified());

    QCOMPARE(QxPluginManagerDialog::disabledPlugins(&settings), QStringList({QStringLiteral("c")}));
    QCOMPARE(QxPluginManagerDialog::enabledPlugins(&settings), QStringList({QStringLiteral("optin")}));

    // The manager honours both lists when the host reads them back at start-up.
    QxPluginManager reloaded;
    registerPlugins(reloaded);
    reloaded.setDisabledPlugins(QxPluginManagerDialog::disabledPlugins(&settings));
    reloaded.setEnabledPlugins(QxPluginManagerDialog::enabledPlugins(&settings));
    reloaded.loadPlugins();
    QVERIFY(!reloaded.hasError());
    QCOMPARE(reloaded.spec(QStringLiteral("a"))->state(), QxPluginState::Initialized);
    QCOMPARE(reloaded.spec(QStringLiteral("c"))->state(), QxPluginState::Disabled);
    QCOMPARE(reloaded.spec(QStringLiteral("optin"))->state(), QxPluginState::Initialized);
    QVERIFY(reloaded.plugin(QStringLiteral("optin")) != Q_NULLPTR);
}

void tst_QxPluginManagerDialog::switchesSurviveARestart()
{
    const QString file = newSettingsFile();
    QxPluginManager manager;
    registerPlugins(manager);
    manager.loadPlugins();

    QxSettings settings(file, QSettings::IniFormat);
    QxPluginManagerDialog first(&manager, &settings);
    first.setChecked(QStringLiteral("a"), false);
    first.setChecked(QStringLiteral("optin"), true);
    first.apply();

    // A dialog over the same settings comes up showing what was stored rather
    // than the metadata defaults - and does not call that a change.
    QxPluginManagerDialog second(&manager, &settings);
    QCOMPARE(second.isChecked(QStringLiteral("a")), false);
    QCOMPARE(second.isChecked(QStringLiteral("optin")), true);
    QVERIFY(!second.isModified());

    // Discarding puts the switches back where the dialog loaded them from.
    second.setChecked(QStringLiteral("a"), true);
    QVERIFY(second.isModified());
    second.revert();
    QCOMPARE(second.isChecked(QStringLiteral("a")), false);
    QVERIFY(!second.isModified());

    // Closing with Cancel is the same discard.
    second.setChecked(QStringLiteral("optin"), false);
    QVERIFY(second.isModified());
    second.reject();
    QCOMPARE(second.isChecked(QStringLiteral("optin")), true);
    QVERIFY(!second.isModified());
}

void tst_QxPluginManagerDialog::diagnosticsCarryTheManagerError()
{
    const auto ready = []() {
        return new ReadyPlugin;
    };
    QxPluginManager manager;
    manager.registerStaticPlugin(QStringLiteral("good"), pluginMeta(QStringLiteral("good")), ready);
    manager.registerStaticPlugin(QStringLiteral("broken"),
                                 pluginMeta(QStringLiteral("broken"), {QStringLiteral("ghost")}), ready);
    manager.loadPlugins();
    QVERIFY(manager.hasError());

    QxPluginManagerDialog dialog(&manager);
    QCOMPARE(dialog.count(), 2);
    // The plugin that could not start is a row of its own, not a missing one -
    // and the reason it gives is on screen in one piece.
    QVERIFY(dialog.rowOf(QStringLiteral("broken")) >= 0);
    QVERIFY(dialog.diagnostics().contains(QStringLiteral("broken")));
    QVERIFY(dialog.diagnostics().contains(QStringLiteral("ghost")));
}

void tst_QxPluginManagerDialog::storedListsRoundTripThroughTheHelpers()
{
    const QString file = newSettingsFile();
    QxSettings settings(file, QSettings::IniFormat);

    QCOMPARE(QxPluginManagerDialog::disabledPlugins(&settings), QStringList());
    QCOMPARE(QxPluginManagerDialog::enabledPlugins(&settings), QStringList());

    QxPluginManagerDialog::setDisabledPlugins(&settings, {QStringLiteral("a")});
    QxPluginManagerDialog::setEnabledPlugins(&settings, {QStringLiteral("b")});
    QCOMPARE(QxPluginManagerDialog::disabledPlugins(&settings), QStringList({QStringLiteral("a")}));
    QCOMPARE(QxPluginManagerDialog::enabledPlugins(&settings), QStringList({QStringLiteral("b")}));

    // An empty list takes the key out again, so "nothing stored" stays
    // distinguishable from "an empty list stored".
    QxPluginManagerDialog::setDisabledPlugins(&settings, QStringList());
    QxPluginManagerDialog::setEnabledPlugins(&settings, QStringList());
    QVERIFY(!settings.contains(QStringLiteral("plugins/disabled")));
    QVERIFY(!settings.contains(QStringLiteral("plugins/enabled")));

    // No settings object means "nothing stored", not a crash.
    QCOMPARE(QxPluginManagerDialog::disabledPlugins(Q_NULLPTR), QStringList());
    QCOMPARE(QxPluginManagerDialog::enabledPlugins(Q_NULLPTR), QStringList());
    QxPluginManagerDialog::setDisabledPlugins(Q_NULLPTR, {QStringLiteral("a")});
    QxPluginManagerDialog::setEnabledPlugins(Q_NULLPTR, {QStringLiteral("a")});
}

TEST_ADD(tst_QxNavigationBar)
TEST_ADD(tst_QxSplashScreen)
TEST_ADD(tst_QxAppShell)
TEST_ADD(tst_QxPluginManagerDialog)

#include "tst_qxapp.moc"
