/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#include "tst_global.h"

#include "tst_probe.h"

#include "qxcore/qxsettings.h"
#include "qxdock/docksplitter.h"
#include "qxdock/dockwidget.h"
#include "qxdock/dockwindow.h"
#include "qxtheme/qxtheme.h"
#include "qxtheme/qxthememanager.h"

#include <QApplication>
#include <QBoxLayout>
#include <QList>
#include <QPalette>
#include <QSplitter>
#include <QTemporaryDir>
#include <QTest>

QX_DOCK_USE_NAMESPACE
QX_THEME_USE_NAMESPACE
QX_CORE_USE_NAMESPACE

namespace
{
const char *const kShort = "chrome-short";
const char *const kTall = "chrome-tall";
const char *const kGrey = "chrome-grey";
const char *const kBlue = "chrome-blue";

QxThemeInfo makeTheme(const QString &id, const QString &name, const QString &css)
{
    QxThemeInfo info(id, name, false);
    info.styleSheet = css;
    return info;
}

/*! The root splitter is protected, so the test reaches it the way a subclass
 *  would - the same door the layout and sequence scenes use. */
class SplitterWindow : public DockWindow
{
public:
    using DockWindow::DockWindow;

    QSplitter *root() const
    {
        return rootSplitter();
    }
};
}   // namespace

/*!
 * A theme switch is the one change a dock cannot answer with a single number:
 * the chrome is painted from a sheet the application builds on every switch,
 * while the panes are holding sizes the user arranged and were measured once.
 *
 * These two scenes say which of those follows the switch and which does not,
 * in-process, where the screenshot suite has to say it with a pixel budget
 * over a whole window. That budget is the only reason the 4.6 defect - the
 * panes keeping sizes measured against a chrome that had since shrunk - was
 * filed as expected rather than fixed: re-dividing the row would also throw
 * away the arrangement, which is a worse thing to do to a user than leaving
 * 17 pixels behind. So the trade is pinned here rather than changed.
 */
class tst_DockTheme : public QObject
{
    Q_OBJECT
private slots:
    void init();
    void cleanup();

    void theChromeFollowsTheSwitchWhileTheRowKeepsTheSizesItWasGiven();
    void theChromeIsRepaintedWhenTheThemeChanges();
private:
    void registerThemes();

    QPalette m_palette;
    QString m_sheet;
    QTemporaryDir m_dir;
};

void tst_DockTheme::init()
{
    m_palette = qApp->palette();
    m_sheet = qApp->styleSheet();
    QVERIFY(m_dir.isValid());

    // The manager is a process-wide singleton that reads back the last choice
    // on start, so every case hands it a settings file of its own before it
    // registers anything. What a case leaves behind is what the next one would
    // otherwise inherit.
    QxThemeManager *manager = QxThemeManager::instance();
    manager->setSettings(new QxSettings(m_dir.filePath(QStringLiteral("theme.ini")), QSettings::IniFormat));
    registerThemes();
}

void tst_DockTheme::cleanup()
{
    QxThemeManager *manager = QxThemeManager::instance();
    for (const QString &id : manager->themeIds()) {
        manager->unregisterTheme(id);
    }
    qApp->setPalette(m_palette);
    qApp->setStyleSheet(m_sheet);
}

void tst_DockTheme::registerThemes()
{
    QxThemeManager *manager = QxThemeManager::instance();
    for (const QString &id : manager->themeIds()) {
        manager->unregisterTheme(id);
    }
    // The dock's chrome is a number the sheet states, which is why a theme is
    // the honest way to change it: it is what an application actually hands
    // over on a switch.
    manager->registerTheme(makeTheme(QString::fromLatin1(kShort), QStringLiteral("chrome short"),
                                     QStringLiteral("QFrame { min-height: 24px; background-color: #404040; }")));
    manager->registerTheme(makeTheme(QString::fromLatin1(kTall), QStringLiteral("chrome tall"),
                                     QStringLiteral("QFrame { min-height: 40px; background-color: #404040; }")));
    manager->registerTheme(makeTheme(QString::fromLatin1(kGrey), QStringLiteral("chrome grey"),
                                     QStringLiteral("QFrame { background-color: #404040; }")));
    manager->registerTheme(makeTheme(QString::fromLatin1(kBlue), QStringLiteral("chrome blue"),
                                     QStringLiteral("QFrame { background-color: #305090; }")));
}

/*! The chrome is measured from the sheet on every switch, and what it takes it
 *  takes out of the pane under it. The row the panes sit in is a different
 *  thing: it was divided up once, and a switch does not divide it again - which
 *  is a choice, not an oversight, and it is pinned here so that a change to it
 *  has to be made on purpose. */
void tst_DockTheme::theChromeFollowsTheSwitchWhileTheRowKeepsTheSizesItWasGiven()
{
    QxThemeManager::instance()->setTheme(QString::fromLatin1(kShort));

    QWidget host;
    host.setLayout(new QVBoxLayout);
    SplitterWindow dock(&host);
    host.layout()->addWidget(&dock);
    host.resize(800, 600);

    DockWidget first(QStringLiteral("first"));
    QWidget content;
    first.setWidget(&content);
    dock.addDockWidget(Qx::LeftDockWidgetArea, &first);
    host.show();
    Probe::settle(&host);

    const int tabBarBefore = Probe::heightOf(Probe::tree(&host), QStringLiteral("DockTabBar"));
    const int contentBefore = content.height();
    const QList<int> sizesBefore = dock.root()->sizes();
    QVERIFY(tabBarBefore > 0);
    QVERIFY(contentBefore > 0);
    QVERIFY(!sizesBefore.isEmpty());

    QxThemeManager::instance()->setTheme(QString::fromLatin1(kTall));
    Probe::settle(&host);

    const int tabBarAfter = Probe::heightOf(Probe::tree(&host), QStringLiteral("DockTabBar"));
    QVERIFY2(tabBarAfter > tabBarBefore,
             qPrintable(QStringLiteral("the tab bar stayed %1 tall across the switch").arg(tabBarBefore)));
    // What the chrome took, the pane under it gave up - that is the part a
    // switch does recompute.
    QCOMPARE(contentBefore - content.height(), tabBarAfter - tabBarBefore);
    // And this is the part it does not: the row keeps the sizes it was given,
    // which is why a switched window and a fresh one are not the same shape.
    QCOMPARE(dock.root()->sizes(), sizesBefore);
}

/*! The other half of what a switch has to do: the chrome that was already up
 *  has to be painted in the new colours, not only measured in the new metrics.
 *  A surface that kept the old one is what the screenshot suite's pixel budget
 *  is really looking for, and this is the same question asked of one widget. */
void tst_DockTheme::theChromeIsRepaintedWhenTheThemeChanges()
{
    QxThemeManager::instance()->setTheme(QString::fromLatin1(kGrey));

    QWidget host;
    host.setLayout(new QVBoxLayout);
    SplitterWindow dock(&host);
    host.layout()->addWidget(&dock);
    host.resize(800, 600);

    DockWidget first(QStringLiteral("first"));
    dock.addDockWidget(Qx::LeftDockWidgetArea, &first);
    host.show();
    Probe::settle(&host);

    // The title bar is the chrome the dock paints from the sheet; a tree is
    // what says where it is, so the widget is found the way a capture would
    // find it rather than by looking the class up in the private headers.
    const QString before = Probe::tree(&host);
    const QString titleBefore = Probe::lines(before, QStringLiteral("DockTitleBar")).value(0);
    QVERIFY2(!titleBefore.isEmpty(), qPrintable(before));

    QxThemeManager::instance()->setTheme(QString::fromLatin1(kBlue));
    Probe::settle(&host);

    const QString after = Probe::tree(&host);
    const QString titleAfter = Probe::lines(after, QStringLiteral("DockTitleBar")).value(0);
    QVERIFY2(!titleAfter.isEmpty(), qPrintable(after));
    // What it painted, and not where it is: a tree line carries both, and a
    // chrome that moved is not a chrome that was repainted.
    const QString inkBefore = titleBefore.section(QLatin1Char('|'), 1).trimmed();
    const QString inkAfter = titleAfter.section(QLatin1Char('|'), 1).trimmed();
    QVERIFY2(inkAfter != inkBefore,
             qPrintable(QStringLiteral("the title bar painted %1 both before and after the switch").arg(inkBefore)));
}

TEST_ADD(tst_DockTheme)

#include "tst_docktheme.moc"
