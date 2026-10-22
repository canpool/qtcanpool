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
#include "titlebar.h"

#include "qxapp/qxthemebundle.h"
#include "qxmaterial/materialfab.h"
#include "qxmaterial/materialnavigationrail.h"
#include "qxmaterial/materialsnackbar.h"
#include "qxmaterial/materialtokens.h"
#include "qxmaterial/materialtopappbar.h"
#include "qxtheme/qxtheme.h"
#include "qxtheme/qxthememanager.h"
#include "qxtheme/qxthememotion.h"
#include "qxwindow/windowagentwidget.h"

#include <QtCore/QTextStream>
#include <QAction>
#include <QEvent>
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

void MainWindow::changeEvent(QEvent *event)
{
    if (event->type() == QEvent::WindowStateChange && m_titleBar) {
        // The button is told, not asked: the glyph follows the check, and the
        // window is the one thing that knows whether it is maximized.
        m_titleBar->maximizeButton()->setChecked(isMaximized());
    }
    QMainWindow::changeEvent(event);
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
    // A rail is the whole width it has and never opens: what it is beside is
    // the window's to arrange, which is a row of two here - the column of
    // destinations, and the room they open into. The app bar is not in this
    // layout: it is the caption, and a caption is not part of the body.
    auto *body = new QWidget(this);
    auto *bodyLayout = new QHBoxLayout(body);
    bodyLayout->setContentsMargins(QMargins());
    bodyLayout->setSpacing(0);

    createNavigation();
    bodyLayout->addWidget(m_rail);
    bodyLayout->addWidget(m_stack, 1);

    setCentralWidget(body);

    buildTitleBar();
    installWindowAgent();

    // The snackbar belongs to the window it rises in, and it is not in a
    // layout: it places itself at the bottom of its parent and rises from
    // under the edge, which a layout would not let it do.
    m_snackbar = new MaterialSnackbar(body);
    m_snackbar->setObjectName(QStringLiteral("snackbar"));
    m_snackbar->hide();
}

void MainWindow::buildTitleBar()
{
    m_titleBar = new TitleBar(this);
    // The app bar is the library's own control and the caption is only where the
    // window puts it: what the screen is called and what can be done to it stay
    // the bar's to draw and the rail's to say, which is why nothing here sets the
    // title again after the first screen.
    m_bar = m_titleBar->appBar();
    m_bar->setTitle(tr("Home"));

    // What can be done to the screen as a whole stands at the bar's far end,
    // drawn as icons - an action without one holds a pressable place and shows
    // nothing, so each is given a standard icon the way the rail's are. What
    // they do is the demo's to say, and the snackbar is how it says it.
    connect(m_bar->addTrailingAction(tr("Search"), style()->standardIcon(QStyle::SP_FileDialogContentsView)),
            &QAction::triggered, this, [this]() {
                m_snackbar->showMessage(tr("Nothing to search yet: the bar's actions are the application's "
                                           "to give work to."));
            });
    connect(m_bar->addTrailingAction(tr("More"), style()->standardIcon(QStyle::SP_FileDialogListView)),
            &QAction::triggered, this, [this]() {
                m_snackbar->showMessage(tr("Nothing further up: a bar that had a menu in it would open it "
                                           "here."));
            });
}

void MainWindow::installWindowAgent()
{
    // 1. Take the window over. From here on it draws no system frame: the agent
    //    answers the hit test that decides which part of the client area still
    //    behaves as border, as caption and as system button.
    m_agent = new WindowAgentWidget(this);
    m_agent->setup(this);

    // 2. Hand the bar over as the caption. It is an ordinary widget and the
    //    agent is told which one it is, nothing more.
    setMenuWidget(m_titleBar);
    m_agent->setTitleBar(m_titleBar);

    // 3. Name the system buttons. Naming them is what keeps them clickable: the
    //    hit test calls everything else in the bar the caption, and a press
    //    there starts a window move instead of reaching a button. There is no
    //    WindowIcon among them, because a Material app bar carries no
    //    application mark - the corner a mark would have taken holds nothing.
#ifndef Q_OS_MAC
    m_agent->setSystemButton(WindowAgentBase::Minimize, m_titleBar->minimizeButton());
    m_agent->setSystemButton(WindowAgentBase::Maximize, m_titleBar->maximizeButton());
    m_agent->setSystemButton(WindowAgentBase::Close, m_titleBar->closeButton());

    connect(m_titleBar->minimizeButton(), &QAbstractButton::clicked, this, &QWidget::showMinimized);
    connect(m_titleBar->maximizeButton(), &QAbstractButton::clicked, this, [this]() {
        if (isMaximized()) {
            showNormal();
        } else {
            showMaximized();
        }
    });
    connect(m_titleBar->closeButton(), &QAbstractButton::clicked, this, &QWidget::close);
#endif

    // 4. The app bar as a whole is client area, because it is one control that
    //    draws its own icons: naming the bar is how a press on one of them
    //    reaches the bar rather than starting a window move. What that costs -
    //    a press on the bar no longer drags - the caption's wrapper pays back by
    //    watching for a drag once it has travelled far enough; see TitleBar.
    m_agent->setHitTestVisible(m_bar, true);

#ifdef Q_OS_MAC
    // macOS reserves a corner of the title bar for the three buttons it draws
    // itself, and this is how wide that corner is. Without it the hit test and
    // the screen disagree about where the buttons are.
    m_agent->setSystemButtonAreaCallback([](const QSize &size) {
        static constexpr const int width = 75;
        return QRect(QPoint(size.width() - width, 0), QSize(width, size.height()));
    });
#endif
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
