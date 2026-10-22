/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#include "mainwindow.h"

#include "collectionpage.h"
#include "demopage.h"
#include "feedbackpage.h"
#include "homepage.h"
#include "inputspage.h"
#include "settingspage.h"

#include "qxapp/qxthemebundle.h"
#include "qxfluent/fluentcommandbar.h"
#include "qxfluent/fluentnavigationview.h"
#include "qxfluent/fluentwindowmaterial.h"
#include "qxtheme/qxtheme.h"
#include "qxtheme/qxthememanager.h"
#include "qxtheme/qxthememotion.h"

#include <QtCore/QTextStream>
#include <QAction>
#include <QActionGroup>
#include <QMenu>
#include <QScrollArea>
#include <QStackedWidget>
#include <QStatusBar>
#include <QStyle>
#include <QVBoxLayout>
#include <QWidget>

#include <cstdio>

QX_APP_USE_NAMESPACE
QX_THEME_USE_NAMESPACE

namespace
{
/*! One line of the --inspect tree, and the whole of what a visual regression
 *  test parses out of it.
 *
 *  The rectangle is in window coordinates because a capture is of the window:
 *  a widget's x() and y() are in its parent's coordinates, and cropping a
 *  window-sized picture with them would cut out the wrong piece of a control
 *  that sits two levels down. */
void describe(QTextStream &out, QWidget *widget, int depth)
{
    const QPoint spot = widget->mapTo(widget->window(), QPoint(0, 0));
    out << QString(depth * 2, QLatin1Char(' ')) << widget->metaObject()->className() << QLatin1Char(' ')
        << QLatin1Char('"') << widget->objectName() << QLatin1String("\" (") << spot.x() << QLatin1Char(',') << spot.y()
        << QLatin1Char(' ') << widget->width() << QLatin1Char('x') << widget->height() << QLatin1Char(')');
    if (!widget->isVisible()) {
        out << QLatin1String(" hidden");
    }
    out << QLatin1Char('\n');

    const QList<QObject *> children = widget->children();
    for (QObject *child : children) {
        if (QWidget *childWidget = qobject_cast<QWidget *>(child)) {
            describe(out, childWidget, depth + 1);
        }
    }
}

/*! Adds \a page to the stack and returns the screen, wrapped so that a screen
 *  taller than the window scrolls on its own. */
QWidget *scrollable(DemoPage *page)
{
    auto *area = new QScrollArea;
    area->setWidgetResizable(true);
    area->setWidget(page);
    area->setFrameShape(QFrame::NoFrame);
    return area;
}
}   // namespace

MainWindow::MainWindow(QWidget *parent, bool screenshotMode, const QString &themeId)
    : QMainWindow(parent)
{
    setObjectName(QStringLiteral("fluentDemo"));
    setWindowTitle(tr("Fluent demo"));

    createTheme(themeId);
    // Re-asserted here, and the position is the whole of the point: the manager
    // restores the stored switch when it is first made, which happens inside
    // createTheme - so a switch set before the window exists is thrown away by
    // the restore - and the screens are built after it, where two of the
    // controls start an animation the moment they are constructed. A capture
    // must not land on a moving target, and must not write the user's settings
    // either, which is why the manager's own setter is not the one asked.
    if (screenshotMode) {
        QxTheme::setMotionReduced(true);
    }

    auto *central = new QWidget(this);
    auto *layout = new QVBoxLayout(central);
    layout->setContentsMargins(QMargins());
    layout->setSpacing(0);

    createCommandBar();
    layout->addWidget(m_bar);

    createNavigation();
    layout->addWidget(m_nav, 1);

    setCentralWidget(central);
    resize(1100, 760);

    createConnections();
    statusBar()->showMessage(tr("Ready"));
}

void MainWindow::applyTheme(const QString &themeId)
{
    // One call, because the theme engine is what installs the sheet and the
    // palette: adding anything here would be a second path to the same result.
    QxThemeManager::instance()->setTheme(themeId);
}

void MainWindow::setDestination(int index)
{
    m_nav->setCurrentIndex(index);
}

void MainWindow::printTree()
{
    QTextStream out(stdout);
    describe(out, this, 0);
}

void MainWindow::createTheme(const QString &themeId)
{
    QxThemeManager *engine = QxThemeManager::instance();
    // The bundle is registered here rather than in main(), so the window is the
    // one place that decides what this demo offers.
    registerBundledThemes(engine);
    engine->setSystemThemePair(QStringLiteral("soft-light"), QStringLiteral("soft-dark"));
    if (!themeId.isEmpty()) {
        engine->setTheme(themeId);
    } else if (!engine->hasTheme(engine->theme())) {
        engine->setTheme(QStringLiteral("soft-light"));
    }
}

void MainWindow::createCommandBar()
{
    m_bar = new FluentCommandBar(this);
    m_bar->setObjectName(QStringLiteral("commandBar"));

    QAction *about = m_bar->addCommand(FluentCommandBar::ItemRole::Button, tr("About"));
    connect(about, &QAction::triggered, this, [this]() {
        statusBar()->showMessage(tr("A Fluent application built out of QxFluent: navigation, commands, and "
                                    "controls - no ribbon, and no style sheet of its own."),
                                 8000);
    });

    // Folding the pane is the one piece of behaviour a Fluent navigation pane
    // has that a list of buttons does not: it is what an application does on a
    // narrow window, where the words are the first thing there is no room for.
    QAction *fold = m_bar->addCommand(FluentCommandBar::ItemRole::Toggle, tr("Fold the pane"));
    connect(fold, &QAction::toggled, this, [this](bool on) {
        m_nav->setPaneOpen(!on);
    });

    // The theme menu lives in the command row rather than in a menu bar,
    // because a Fluent application has no menu bar.
    QAction *theme = m_bar->addCommand(FluentCommandBar::ItemRole::DropDown, tr("Theme"));
    QxThemeManager *engine = QxThemeManager::instance();
    auto *menu = new QMenu(m_bar);
    auto *group = new QActionGroup(m_bar);
    group->setExclusive(true);
    const QStringList ids = engine->themeIds();
    for (const QString &id : ids) {
        QAction *entry = menu->addAction(engine->themeInfo(id).name);
        entry->setCheckable(true);
        entry->setChecked(engine->theme() == id);
        entry->setData(id);
        group->addAction(entry);
    }
    theme->setMenu(menu);
    connect(group, &QActionGroup::triggered, this, [](QAction *entry) {
        QxThemeManager::instance()->setTheme(entry->data().toString());
    });
}

void MainWindow::createNavigation()
{
    m_nav = new FluentNavigationView(this);
    m_nav->setObjectName(QStringLiteral("navigation"));
    m_stack = new QStackedWidget(this);
    m_stack->setObjectName(QStringLiteral("screens"));

    // The pane holds one thing, and what it opens is a screen at a time: a
    // stacked widget is what turns "one room beside the pane" into "one of
    // five", and it is also what keeps a screen's state while another is shown.
    struct Destination {
        QStyle::StandardPixmap pixmap;
        const char *name;
        DemoPage *page;
    };
    HomePage *home = new HomePage(m_stack);
    InputsPage *inputs = new InputsPage(m_stack);
    FeedbackPage *feedback = new FeedbackPage(m_stack);
    CollectionPage *collection = new CollectionPage(m_stack);
    m_settings = new SettingsPage(m_stack);

    const Destination destinations[] = {{QStyle::SP_DirHomeIcon, QT_TR_NOOP("Home"), home},
                                        {QStyle::SP_ComputerIcon, QT_TR_NOOP("Input"), inputs},
                                        {QStyle::SP_MessageBoxInformation, QT_TR_NOOP("Feedback"), feedback},
                                        {QStyle::SP_DirIcon, QT_TR_NOOP("Collection"), collection},
                                        {QStyle::SP_FileDialogDetailedView, QT_TR_NOOP("Settings"), m_settings}};

    for (const Destination &destination : destinations) {
        // A destination is a QAction, and the view puts the one that was
        // clicked in force and triggers it - so an application that connected
        // to it hears about it, and the stack follows the view's own answer
        // rather than a second copy of the same decision.
        m_nav->addItem(tr(destination.name), style()->standardIcon(destination.pixmap));
        m_stack->addWidget(scrollable(destination.page));
    }

    m_nav->setContent(m_stack);
    m_nav->setCurrentIndex(0);
}

void MainWindow::createConnections()
{
    // The pane says which destination is in force, and the stack follows: there
    // is one way the screen changes, whether it was clicked, walked to with the
    // keyboard, or asked for from the command line.
    connect(m_nav, &FluentNavigationView::currentIndexChanged, this, [this](int index) {
        m_stack->setCurrentIndex(index);
    });
    connect(m_nav, &FluentNavigationView::currentItemChanged, this, [this](QAction *item) {
        statusBar()->showMessage(tr("Showing: %1").arg(item->text()), 4000);
    });

    connect(m_settings, &SettingsPage::paneOpenChanged, this, [this](bool open) {
        m_nav->setPaneOpen(open);
    });
    connect(m_settings, &SettingsPage::materialRequested, this, [this](FluentWindowMaterial::Material material) {
        if (!m_material) {
            m_material = new FluentWindowMaterial(this);
        }
        // What the platform answered is handed back to the page, which is the
        // thing that says it: on anything other than Windows the honest answer
        // is no, and that is worth showing rather than hiding.
        m_settings->reportMaterial(m_material->apply(material));
    });
}
