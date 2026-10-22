/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#include "mainwindow.h"

#include "demopage.h"
#include "feedbackpage.h"
#include "homepage.h"
#include "selectionpage.h"
#include "settingspage.h"

#include "qxapp/qxthemebundle.h"
#include "qxmaterial/materialfab.h"
#include "qxmaterial/materialnavigationrail.h"
#include "qxmaterial/materialsnackbar.h"
#include "qxmaterial/materialtokens.h"
#include "qxmaterial/materialtopappbar.h"
#include "qxtheme/qxtheme.h"
#include "qxtheme/qxthememanager.h"
#include "qxtheme/qxthememotion.h"

#include <QtCore/QTextStream>
#include <QAction>
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
    area->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    area->setWidget(page);
    area->setFrameShape(QFrame::NoFrame);
    return area;
}
}   // namespace

MainWindow::MainWindow(QWidget *parent, bool screenshotMode, const QString &themeId)
    : QMainWindow(parent)
{
    setObjectName(QStringLiteral("materialDemo"));
    setWindowTitle(tr("Material demo"));

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

    createShell();
    createConnections();

    resize(1100, 760);
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
    m_rail->setCurrentIndex(index);
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

void MainWindow::createShell()
{
    auto *central = new QWidget(this);
    auto *layout = new QVBoxLayout(central);
    layout->setContentsMargins(QMargins());
    layout->setSpacing(0);

    m_bar = new MaterialTopAppBar(central);
    m_bar->setObjectName(QStringLiteral("appBar"));
    // What the screen is called is the rail's to say and the bar's to show:
    // the two are told by the same answer, which is why the title is set here
    // and not by each screen.
    m_bar->setTitle(tr("Home"));
    layout->addWidget(m_bar);

    // A rail is the whole width it has and never opens: what it is beside is
    // the window's to arrange, which is a row of two here - the column of
    // destinations, and the room they open into.
    auto *body = new QWidget(central);
    auto *bodyLayout = new QHBoxLayout(body);
    bodyLayout->setContentsMargins(QMargins());
    bodyLayout->setSpacing(0);

    createNavigation();
    bodyLayout->addWidget(m_rail);
    bodyLayout->addWidget(m_stack, 1);

    layout->addWidget(body, 1);
    setCentralWidget(central);

    // The snackbar belongs to the window it rises in, and it is not in a
    // layout: it places itself at the bottom of its parent and rises from
    // under the edge, which a layout would not let it do.
    m_snackbar = new MaterialSnackbar(central);
    m_snackbar->setObjectName(QStringLiteral("snackbar"));
    m_snackbar->hide();
}

void MainWindow::createNavigation()
{
    m_rail = new MaterialNavigationRail;
    m_rail->setObjectName(QStringLiteral("rail"));

    // The floating action button is the one thing that is not a destination
    // but belongs above them, which is what the header at the top of a rail is
    // for - and what it does is this application's to say.
    m_fab = new MaterialFAB(m_rail);
    m_fab->setText(tr("Compose"));
    m_rail->setHeaderWidget(m_fab);

    m_stack = new QStackedWidget(this);
    m_stack->setObjectName(QStringLiteral("screens"));

    HomePage *home = new HomePage(m_stack);
    SelectionPage *selection = new SelectionPage(m_stack);
    FeedbackPage *feedback = new FeedbackPage(m_stack);
    m_settings = new SettingsPage(m_stack);

    // The rail holds one thing, and what it opens is a screen at a time: a
    // stacked widget is what turns "one room beside the rail" into "one of
    // four", and it is also what keeps a screen's state while another is shown.
    struct Destination {
        QStyle::StandardPixmap pixmap;
        const char *name;
        DemoPage *page;
    };
    const Destination destinations[] = {{QStyle::SP_DirHomeIcon, QT_TR_NOOP("Home"), home},
                                        {QStyle::SP_ComputerIcon, QT_TR_NOOP("Selection"), selection},
                                        {QStyle::SP_MessageBoxInformation, QT_TR_NOOP("Feedback"), feedback},
                                        {QStyle::SP_FileDialogDetailedView, QT_TR_NOOP("Settings"), m_settings}};

    for (const Destination &destination : destinations) {
        // A destination is a QAction, and the rail puts the one that was
        // clicked in force - so an application that connected to it hears
        // about it, and the stack follows the rail's own answer rather than a
        // second copy of the same decision.
        m_rail->addDestination(tr(destination.name), style()->standardIcon(destination.pixmap));
        m_stack->addWidget(scrollable(destination.page));
    }
}

void MainWindow::createConnections()
{
    // The rail says which destination is in force, and the stack follows: there
    // is one way the screen changes, whether it was clicked, walked to with the
    // keyboard, or asked for from the command line. The bar's title follows
    // with it, because the name of the screen is the screen's.
    connect(m_rail, &MaterialNavigationRail::currentIndexChanged, this, [this](int index) {
        m_stack->setCurrentIndex(index);
    });
    connect(m_rail, &MaterialNavigationRail::currentDestinationChanged, this, [this](QAction *destination) {
        if (!destination) {
            return;
        }
        m_bar->setTitle(destination->text());
        statusBar()->showMessage(tr("Showing: %1").arg(destination->text()), 4000);
    });

    connect(m_fab, &MaterialFAB::clicked, this, [this]() {
        // A floating action button is the one thing a screen is for, and what
        // this screen is for is a demo: the snackbar says so honestly rather
        // than pretending something was composed.
        m_snackbar->showMessage(tr("Nothing was composed: the floating action button is where an "
                                   "application puts the one thing its screen is for."));
    });

    // The snackbar is the window's, so a screen that has something to say says
    // it through the window rather than keeping one of its own.
    FeedbackPage *feedback = qobject_cast<FeedbackPage *>(m_stack->widget(2));
    if (feedback) {
        connect(feedback, &FeedbackPage::messageRequested, this, [this](const QString &message) {
            m_snackbar->showMessage(message);
        });
    }

    connect(m_settings, &SettingsPage::variantRequested, this, [this](MaterialTopAppBar::Variant variant) {
        m_bar->setVariant(variant);
    });
    connect(m_settings, &SettingsPage::scrolledChanged, this, [this](bool scrolled) {
        m_bar->setScrolled(scrolled);
    });
}
