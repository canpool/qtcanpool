/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#include "mainwindow.h"

#include "controlpages.h"
#include "demopage.h"
#include "homepage.h"
#include "settingspage.h"
#include "titlebar.h"

#include "qxapp/qxthemebundle.h"
#include "qxfluent/fluentcommandbar.h"
#include "qxfluent/fluentnavigationview.h"
#include "qxfluent/fluentwindowmaterial.h"
#include "qxtheme/qxtheme.h"
#include "qxtheme/qxthememanager.h"
#include "qxtheme/qxthememotion.h"
#include "qxwindow/windowagentwidget.h"

#include <QtCore/QHash>
#include <QtCore/QSet>
#include <QtCore/QSignalBlocker>
#include <QtCore/QTextStream>
#include <QtCore/QTimer>
#include <QAction>
#include <QEvent>
#include <QActionGroup>
#include <QLineEdit>
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
QX_WINDOW_USE_NAMESPACE

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

/*! Wraps \a page so that a screen taller than the window scrolls on its own. */
QWidget *scrollable(DemoPage *page)
{
    auto *area = new QScrollArea;
    area->setWidgetResizable(true);
    area->setWidget(page);
    area->setFrameShape(QFrame::NoFrame);
    return area;
}

/*! How long a single click on the application mark waits before it opens the
 *  system menu, so that the second click of a double click can cancel it. Short
 *  enough that the menu still feels like it opened on the click. */
constexpr int kSystemMenuDelay = 75;
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

    createNavigation();
    layout->addWidget(m_nav, 1);

    setCentralWidget(central);

    buildTitleBar();
    installWindowAgent();

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

void MainWindow::setSearch(const QString &text)
{
    // The field is written rather than the pane: what the pane lists is the
    // field's answer, and a way in that skipped it would be a second search.
    m_titleBar->searchField()->setText(text);
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

void MainWindow::createNavigation()
{
    m_nav = new FluentNavigationView(this);
    m_nav->setObjectName(QStringLiteral("navigation"));
    m_stack = new QStackedWidget(this);
    m_stack->setObjectName(QStringLiteral("screens"));

    // The pane holds one thing, and what it opens is a screen at a time: a
    // stacked widget is what turns "one room beside the pane" into "one of
    // them", and it is also what keeps a screen's state while another is shown.
    const auto addPage = [this](DemoPage *page, QStyle::StandardPixmap pixmap, const QString &group,
                                const QString &parent) {
        Destination destination;
        // The name comes from the page and not from a second list beside it: a
        // pane that called a page one thing and the page another would be a
        // demo of two names for one control.
        destination.text = page->title();
        destination.icon = style()->standardIcon(pixmap);
        destination.group = group;
        destination.parent = parent;
        destination.page = scrollable(page);
        m_stack->addWidget(destination.page);
        m_destinations.append(destination);
    };

    HomePage *home = new HomePage(m_stack);
    m_settings = new SettingsPage(m_stack);

    addPage(home, QStyle::SP_DirHomeIcon, QString(), QString());

    QStringList titles;
    const QList<ControlPages::ControlGroup> groups = ControlPages::build(m_stack);
    for (const ControlPages::ControlGroup &group : groups) {
        for (const ControlPages::ControlEntry &entry : group.pages) {
            titles.append(entry.page->title());
            addPage(entry.page, QStyle::SP_FileIcon, group.name, entry.parent);
        }
    }
    // The first control page is one destination past Home, and the index on the
    // first screen opens them by number.
    home->setCatalogue(titles, 1);
    connect(home, &HomePage::destinationRequested, this, [this](int index) {
        m_nav->setCurrentIndex(index);
    });

    // Settings is a destination of its own rather than the last control: it is
    // about the window the controls are shown in, which is a different question
    // from what the controls are - so it gets a heading of its own rather than
    // being read as one more of the feedback controls.
    addPage(m_settings, QStyle::SP_FileDialogDetailedView, DemoPage::tr("Application"), QString());

    m_nav->setContent(m_stack);
    // The pane is built by the same code that narrows it, so there is one place
    // that knows what a destination is and not two that have to agree.
    filterDestinations(QString());
}

void MainWindow::buildTitleBar()
{
    m_titleBar = new TitleBar(this);
    m_titleBar->setTitle(windowTitle());

    FluentCommandBar *bar = m_titleBar->commandBar();
    QxThemeManager *engine = QxThemeManager::instance();

    // Folding the pane is the one piece of behaviour a Fluent navigation pane
    // has that a list of buttons does not: it is what an application does on a
    // narrow window, where the words are the first thing there is no room for.
    // The button asks the view and is told the answer back (see
    // createConnections): the pane is the view's state, and a button that kept
    // its own copy of it would be a second place the same fact lives.
    m_fold = bar->addCommand(FluentCommandBar::ItemRole::Toggle, tr("Fold the pane"));
    m_fold->setChecked(!m_nav->isPaneOpen());
    connect(m_fold, &QAction::toggled, this, [this](bool on) {
        m_nav->setPaneOpen(!on);
    });

    // Light and dark are one click apart in the caption, and the pair is asked
    // of the engine rather than named here: which two themes are the same theme
    // in the other light is the engine's answer, and a window that wrote the
    // ids down would be wrong the day another family is registered.
    QAction *dark = bar->addCommand(FluentCommandBar::ItemRole::Toggle, tr("Dark"));
    dark->setChecked(engine->isDark());
    connect(dark, &QAction::toggled, this, [](bool on) {
        QxThemeManager *engine = QxThemeManager::instance();
        const QString target = on ? engine->systemDarkTheme() : engine->systemLightTheme();
        if (!target.isEmpty()) {
            engine->setTheme(target);
        }
    });
    connect(engine, &QxThemeManager::themeChanged, dark, [engine, dark](const QString &) {
        // The engine is the only one that knows when the theme changed - a menu
        // entry, a restored setting or the system's own answer all arrive here,
        // and the switch has to say what is in force rather than what was
        // clicked before it.
        QSignalBlocker blocker(dark);
        dark->setChecked(engine->isDark());
    });

    // The theme menu lives in the caption rather than in a menu bar, because a
    // Fluent application has no menu bar.
    QAction *theme = bar->addCommand(FluentCommandBar::ItemRole::DropDown, tr("Theme"));
    auto *menu = new QMenu(bar);
    auto *group = new QActionGroup(bar);
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
    connect(engine, &QxThemeManager::themeChanged, menu, [group](const QString &id) {
        // A menu that remembered what was checked rather than what is in force
        // would show a tick beside a theme the window is no longer drawn in,
        // whichever of the two put the other one on.
        const QList<QAction *> entries = group->actions();
        for (QAction *entry : entries) {
            QSignalBlocker blocker(entry);
            entry->setChecked(entry->data().toString() == id);
        }
    });

    QAction *about = bar->addCommand(FluentCommandBar::ItemRole::Button, tr("About"));
    connect(about, &QAction::triggered, this, [this]() {
        statusBar()->showMessage(tr("A Fluent application built out of QxFluent: a caption of its own, navigation, "
                                    "commands, and controls - no ribbon, and no style sheet of its own."),
                                 8000);
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
    //    there starts a window move instead of reaching a button.
#ifndef Q_OS_MAC
    m_agent->setSystemButton(WindowAgentBase::WindowIcon, m_titleBar->iconButton());
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

    // The two parts of the caption that are not chrome: the field a search is
    // typed into and the row of commands. Everything else in a caption is where
    // a press starts a move.
    m_agent->setHitTestVisible(m_titleBar->searchField(), true);
    m_agent->setHitTestVisible(m_titleBar->commandBar(), true);

#ifdef Q_OS_WINDOWS
    // The application mark behaves the way the system's does: one click opens
    // the system menu, two close the window. A click cannot tell the two apart,
    // so the menu is put on a short delay that the double click cancels.
    connect(m_titleBar->iconButton(), &QAbstractButton::clicked, this, [this]() {
        m_titleBar->iconButton()->setProperty("double-click-close", false);
        QTimer::singleShot(kSystemMenuDelay, this, [this]() {
            if (m_titleBar->iconButton()->property("double-click-close").toBool()) {
                return;
            }
            m_agent->showSystemMenu(
                m_titleBar->iconButton()->mapToGlobal(QPoint{0, m_titleBar->iconButton()->height()}));
        });
    });
    connect(m_titleBar->iconButton(), &CaptionButton::doubleClicked, this, [this]() {
        m_titleBar->iconButton()->setProperty("double-click-close", true);
        close();
    });
#endif

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

void MainWindow::createConnections()
{
    // The pane says which destination is in force, and the stack follows: there
    // is one way the screen changes, whether it was clicked, walked to with the
    // keyboard, or asked for from the command line. The index is a position in
    // the list the pane is showing, which a search can shorten.
    connect(m_nav, &FluentNavigationView::currentIndexChanged, this, [this](int index) {
        if (index >= 0 && index < m_shown.count()) {
            m_stack->setCurrentWidget(m_shown.at(index)->page);
        }
    });
    connect(m_nav, &FluentNavigationView::currentItemChanged, this, [this](QAction *item) {
        if (item) {
            statusBar()->showMessage(tr("Showing: %1").arg(item->text()), 4000);
        }
    });

    connect(m_titleBar, &TitleBar::searchTextChanged, this, &MainWindow::filterDestinations);

    connect(m_settings, &SettingsPage::paneOpenChanged, this, [this](bool open) {
        m_nav->setPaneOpen(open);
    });

    // The pane is the view's state, and the caption's fold and the switch on the
    // settings page are two ways of asking for it - so neither of them remembers
    // what was last clicked, both are told what is in force. Without this,
    // folding from one of them left the other saying the pane was still open.
    //
    // The action is set without a QSignalBlocker on purpose: a blocker would
    // silence QAction::changed as well, and that is what a command bar repaints
    // on - the button would hold the right state and go on drawing the old one.
    // What keeps this from going round is the view: setPaneOpen() does nothing
    // when the pane is already there, so the toggle this may raise dies there.
    connect(m_nav, &FluentNavigationView::paneOpenChanged, this, [this](bool open) {
        m_fold->setChecked(!open);
        m_settings->setPaneOpen(open);
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

void MainWindow::filterDestinations(const QString &needle)
{
    const QString trimmed = needle.trimmed();
    QWidget *currentPage = m_stack->currentWidget();

    m_nav->clear();
    m_shown.clear();
    // Which destinations carry others, and which of those have a page the
    // search wants: a part is on the pane when the search named it or named
    // something under it, because the page a search found has to be listed
    // where it stands - under the part, not beside it as if the group had no
    // parts at all.
    QSet<QString> parts;
    QSet<QString> wanted;
    for (const Destination &destination : m_destinations) {
        if (destination.parent.isEmpty()) {
            continue;
        }
        parts.insert(destination.parent);
        if (trimmed.isEmpty() || destination.text.contains(trimmed, Qt::CaseInsensitive)) {
            wanted.insert(destination.parent);
        }
    }

    // The heading the pane is under, which is the pane's state rather than the
    // destination's: a group a search emptied leaves no heading behind, and a
    // search that keeps one destination of a group still says which group it
    // is - the heading is written when the listing changes, not once per group
    // in the table.
    QString listing;
    QHash<QString, QAction *> opened;
    for (const Destination &destination : m_destinations) {
        const bool listed = trimmed.isEmpty() || destination.text.contains(trimmed, Qt::CaseInsensitive);
        // A part is listed for what is under it as well as for itself, and it
        // is a destination either way: pressing it is a journey to its own
        // page that also opens or closes what stands under it.
        const bool shown = parts.contains(destination.text) ? (listed || wanted.contains(destination.text)) : listed;
        if (!shown) {
            continue;
        }
        if (destination.group != listing) {
            listing = destination.group;
            opened.clear();
            if (!listing.isEmpty()) {
                m_nav->addHeader(listing);
            }
        }
        m_shown.append(&destination);
        // A part is listed before its pages, so by the time one of them arrives
        // the row to put it under exists: the pane is told where a destination
        // stands, and it is the pane that decides what a parent does.
        QAction *action = m_nav->addItem(destination.text, destination.icon, opened.value(destination.parent));
        if (parts.contains(destination.text)) {
            opened.insert(destination.text, action);
        }
    }

    // A search that narrows the pane away from the screen in front must not
    // close it: what is being read stays, and a pane that lost its mark says so
    // by marking nothing. A pane with room for the screen keeps marking it.
    int index = -1;
    for (int i = 0; i < m_shown.count(); ++i) {
        if (m_shown.at(i)->page == currentPage) {
            index = i;
            break;
        }
    }
    if (index < 0 && m_stack->currentWidget() == Q_NULLPTR && !m_shown.isEmpty()) {
        // The first build, and nothing in front yet: the pane's first answer is
        // the screen that comes up, so that the window never opens on a blank.
        index = 0;
        m_stack->setCurrentWidget(m_shown.at(0)->page);
    }
    if (index >= 0) {
        m_nav->setCurrentIndex(index);
    }
}
