/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#include "mainwindow.h"

#include "albumpage.h"
#include "blogpage.h"
#include "catalogpage.h"
#include "orderpage.h"
#include "homepage.h"
#include "demopage.h"
#include "pricingpage.h"
#include "settingspage.h"
#include "signinpage.h"
#include "titlebar.h"

#include "qxapp/qxthemebundle.h"
#include "qxmaterial/materialdrawer.h"
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
#include <QFontMetrics>
#include <QResizeEvent>
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

/*! The two widths at which this window changes shape.
 *
 *  They are not round numbers picked for looks: a rail and a page worth reading
 *  need a thousand between them, and under six hundred and forty a column takes
 *  too much of the window to still be a column - which is where Material stops
 *  putting the destinations at the side and brings them over the page instead.
 *  The three answers are Material's own: a rail, a rail folded to its icons, and
 *  a drawer.
 */
constexpr int kWideWindow = 1000;
constexpr int kNarrowWindow = 640;

/*! Which of the three shapes a window \a across pixels wide is in: 0 a rail
 *  beside the page, 1 the rail folded to its icons, 2 the destinations in a
 *  drawer over the page. */
int shapeOf(int across)
{
    if (across >= kWideWindow) {
        return 0;
    }
    return across >= kNarrowWindow ? 1 : 2;
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
    // The shape is worked out here as well as on every resize, because the
    // window the user sees first is already a size and the shell was built
    // before there was one to ask about.
    applyWidth(width());
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

    // A rail cuts a name that does not fit its column, and a cut name is the one
    // thing the rail cannot say another way - so the names are measured here and
    // the measurement is printed: the test that reads this holds the budget, and
    // a name that grows is caught as a number rather than as a picture nobody
    // looks twice at. The type is the family's label scale, which is the type
    // the rail draws its names in.
    const QFontMetrics metrics(MaterialType::font(MaterialType::Scale::Label));

    // What that type is worth on this platform, as two four-glyph strings. The
    // offscreen plugin has no font database and draws every glyph as the same
    // box, so there a name is as wide as its letter count times sixteen and no
    // name fits a real column - a number the reader of this has to be able to
    // tell apart from a measurement with letters in it.
    out << "type \"iiii\" " << metrics.horizontalAdvance(QStringLiteral("iiii")) << " \"MMMM\" "
        << metrics.horizontalAdvance(QStringLiteral("MMMM")) << '\n';

    const QList<QAction *> places = m_rail->destinations();
    for (QAction *place : places) {
        out << "destination \"" << place->text() << "\" " << metrics.horizontalAdvance(place->text()) << '\n';
    }
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

void MainWindow::resizeEvent(QResizeEvent *event)
{
    QMainWindow::resizeEvent(event);
    applyWidth(event->size().width());
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

    // The drawer stands over the same room the page does, which is why it is
    // built here and not beside the rail: it is what the window does instead of
    // the rail when the window is too narrow for one.
    createDrawer();
}

void MainWindow::createDrawer()
{
    m_drawer = new MaterialDrawer(centralWidget());
    m_drawer->setObjectName(QStringLiteral("drawer"));
    m_drawer->setVariant(MaterialDrawer::Variant::Temporary);

    // The destinations are the rail's, in the rail's order and with the names
    // the rail took from the pages: two lists would be two places a screen has
    // a name, and one of them would be wrong the day a page is renamed.
    const QList<QAction *> places = m_rail->destinations();
    for (QAction *place : places) {
        m_drawer->addDestination(place->text(), place->icon());
    }
    m_drawer->setCurrentIndex(m_rail->currentIndex());
}

void MainWindow::applyWidth(int across)
{
    const int shape = shapeOf(across);
    if (shape == m_shape) {
        // The window has not crossed a width, so nothing about the destinations
        // has changed - and a window that undid the user's own fold on every
        // resize would be working against them.
        return;
    }
    m_shape = shape;

    // Down to the icons at the middle width and gone at the narrow one: what a
    // rail costs the page is a column's worth of it, and a page with less than
    // that to spare is a page the destinations come over instead. The floating
    // action button goes with the rail, since the rail's head is where it
    // stands; a window this narrow would put it over the page, which is the
    // page's own arrangement and not this window's.
    m_rail->setVisible(shape != 2);
    m_rail->setFolded(shape == 1);
    if (shape == 2) {
        giveMenuButton();
    } else {
        takeMenuButton();
        m_drawer->close();
    }
}

void MainWindow::giveMenuButton()
{
    if (m_menu) {
        return;
    }
    // The way into the drawer stands where the bar's leading icon stands. The
    // bar owns the action and destroys it when the place is cleared, which is
    // why the pointer is dropped with it rather than kept.
    m_menu = m_bar->setLeadingAction(tr("Destinations"), style()->standardIcon(QStyle::SP_TitleBarMenuButton));
    connect(m_menu, &QAction::triggered, this, [this]() {
        m_drawer->toggle();
    });
}

void MainWindow::takeMenuButton()
{
    if (!m_menu) {
        return;
    }
    m_bar->clearLeadingAction();
    m_menu = Q_NULLPTR;
}

void MainWindow::buildTitleBar()
{
    m_titleBar = new TitleBar(this);
    // The app bar is the library's own control and the caption is only where the
    // window puts it: what the screen is called and what can be done to it stay
    // the bar's to draw and the rail's to say, which is why nothing here sets the
    // title again after the first screen.
    m_bar = m_titleBar->appBar();
    // The bar opens on the name of the screen that opens first, which is the
    // page's own answer and not a second copy of it kept here.
    m_bar->setTitle(m_firstTitle);

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
    m_titleBar->setWindowAgent(m_agent);

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
    //    the caption gestures the hit test gave for free: drag, a double click
    //    to maximize, and the system menu on a right click - the caption's
    //    wrapper pays back one at a time; see TitleBar.
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

    // A destination is a screen and the icon beside its name, and the name is
    // the page's own - the rail, the caption and the page's heading all read it
    // from there, so a place has one name rather than three.
    const auto addPage = [this](DemoPage *page, QStyle::StandardPixmap pixmap) {
        if (m_firstTitle.isEmpty()) {
            m_firstTitle = page->title();
        }
        m_rail->addDestination(page->title(), style()->standardIcon(pixmap));
        m_stack->addWidget(scrollable(page));
        // The snackbar belongs to the window, so a screen that has something to
        // say says it through the window rather than keeping one of its own.
        connect(page, &DemoPage::messageRequested, this, [this](const QString &message) {
            m_snackbar->showMessage(message);
        });
    };

    // Six templates, then the catalogue: a template is a whole screen built out
    // of this library - which is what an application would actually build - and
    // the catalogue is the library itself, one control per row.
    addPage(new HomePage(m_stack), QStyle::SP_DirHomeIcon);
    addPage(new AlbumPage(m_stack), QStyle::SP_DirIcon);
    addPage(new PricingPage(m_stack), QStyle::SP_FileDialogListView);
    addPage(new SignInPage(m_stack), QStyle::SP_ArrowForward);
    addPage(new BlogPage(m_stack), QStyle::SP_FileIcon);
    addPage(new OrderPage(m_stack), QStyle::SP_FileDialogDetailedView);
    addPage(new CatalogPage(m_stack), QStyle::SP_FileDialogContentsView);

    // What the window is, as opposed to what it is showing, stays last: it is
    // not a place the user works in.
    m_settings = new SettingsPage(m_stack);
    addPage(m_settings, QStyle::SP_FileDialogInfoView);
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

    connect(m_settings, &SettingsPage::variantRequested, this, [this](MaterialTopAppBar::Variant variant) {
        m_bar->setVariant(variant);
    });
    connect(m_settings, &SettingsPage::scrolledChanged, this, [this](bool scrolled) {
        m_bar->setScrolled(scrolled);
    });

    // The drawer is the rail for a window too narrow to hold one, so the two
    // are one answer in two shapes: each is told where the other is, and
    // choosing a destination in the drawer is choosing that destination - after
    // which the panel goes away, because the question it was asked has been
    // answered.
    connect(m_drawer, &MaterialDrawer::currentIndexChanged, m_rail, &MaterialNavigationRail::setCurrentIndex);
    connect(m_rail, &MaterialNavigationRail::currentIndexChanged, m_drawer, &MaterialDrawer::setCurrentIndex);
    connect(m_drawer, &MaterialDrawer::currentDestinationChanged, this, [this](QAction *destination) {
        if (destination) {
            m_drawer->close();
        }
    });
}
