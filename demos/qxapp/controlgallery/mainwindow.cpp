/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#include "mainwindow.h"

#include "fluentpage.h"
#include "gallerypage.h"
#include "materialpage.h"

#include "qxapp/qxthemebundle.h"
#include "qxtheme/qxtheme.h"
#include "qxtheme/qxthememanager.h"
#include "qxtheme/qxthememotion.h"

#include <QtCore/QTextStream>
#include <QActionGroup>
#include <QMenu>
#include <QMenuBar>
#include <QScrollArea>
#include <QTabWidget>

#include <cstdio>

QX_APP_USE_NAMESPACE
QX_THEME_USE_NAMESPACE

namespace
{
/*!
 * The floor a capture is taken at.
 *
 * Fixed in width, because there is no window manager to maximise the window and
 * the golden would otherwise be whatever the offscreen screen happens to be.
 * The height is only a floor: the taller page is asked for its own figure and
 * wins when it is bigger - see MainWindow::captureSize().
 */
constexpr int kCaptureWidth = 1280;
constexpr int kCaptureHeight = 1080;

/*!
 * One line of the --inspect tree, and the whole of what the visual regression
 * test parses out of it.
 *
 * The rectangle is in window coordinates because a capture is of the window: a
 * widget's x() and y() are in its parent's coordinates, and cropping a
 * window-sized picture with them would cut out the wrong piece of a control
 * that sits two levels down.
 */
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
}   // namespace

MainWindow::MainWindow(QWidget *parent, bool screenshotMode, const QString &styleId, const QString &themeId)
    : QMainWindow(parent)
{
    Q_UNUSED(screenshotMode)
    setObjectName(QStringLiteral("controlGallery"));
    setWindowTitle(tr("Control gallery"));

    createTheme(themeId);
    // Re-asserted here, and the position is the whole of the point: the manager
    // restores the stored switch when it is first made, which happens inside
    // createTheme - so a switch set before the window exists is thrown away by
    // the restore - and the pages are built after it, where two of the controls
    // on show start an animation the moment they are constructed. A capture must
    // not land on a moving target, and must not write the user's settings either,
    // which is why the manager's own setter is not the one asked: this is the
    // process-wide switch without the write.
    if (screenshotMode) {
        QxTheme::setMotionReduced(true);
    }

    m_tabs = new QTabWidget(this);
    m_fluent = new FluentPage(m_tabs);
    m_material = new MaterialPage(m_tabs);
    // Each page scrolls on its own: a gallery that made the reader choose
    // between a window tall enough for two dozen controls and a window that fits
    // a screen would lose either way.
    auto addPage = [this](GalleryPage *page, const QString &label) {
        auto *area = new QScrollArea(m_tabs);
        area->setWidgetResizable(true);
        area->setWidget(page);
        m_styleTabs.insert(page->styleId(), m_tabs->addTab(area, label));
        m_styleOrder.append(page->styleId());
    };
    addPage(m_fluent, tr("Fluent"));
    addPage(m_material, tr("Material"));
    setCentralWidget(m_tabs);

    createStyleMenu();

    resize(1100, 900);
    // The tab that was asked for, last: a tab switch after the window has its
    // size is the same code path the Style menu and the --style sequence take,
    // so there is one way the current set changes and not two.
    applyStyle(styleId.isEmpty() ? QStringLiteral("fluent") : styleId);
}

void MainWindow::applyStyle(const QString &id)
{
    const int tab = m_styleTabs.value(id, -1);
    if (tab >= 0) {
        m_tabs->setCurrentIndex(tab);
    }
}

void MainWindow::applyTheme(const QString &themeId)
{
    // One call, because the theme engine is what installs the sheet and the
    // palette: adding anything here would be a second path to the same result.
    QxThemeManager::instance()->setTheme(themeId);
}

void MainWindow::printTree()
{
    QTextStream out(stdout);
    describe(out, this, 0);
}

QSize MainWindow::captureSize() const
{
    QSize size(kCaptureWidth, kCaptureHeight);

    // Measured on the tab on show: a hidden page's viewport is a placeholder
    // (about eighty pixels), so subtracting it from the window height would
    // count the page's own height twice and the capture would grow on every
    // ask.
    const QScrollArea *shown = qobject_cast<QScrollArea *>(m_tabs->currentWidget());
    if (!shown) {
        return size;
    }
    const int chrome = height() - shown->viewport()->height();

    const GalleryPage *pages[] = {m_fluent, m_material};
    for (const GalleryPage *page : pages) {
        if (page) {
            size.setHeight(qMax(size.height(), page->height() + chrome));
        }
    }
    return size;
}

void MainWindow::createTheme(const QString &themeId)
{
    QxThemeManager *engine = QxThemeManager::instance();
    // The bundle is registered here rather than in main(), so the window is the
    // one place that decides what this demo offers.
    registerBundledThemes(engine);
    engine->setSystemThemePair(QStringLiteral("soft-light"), QStringLiteral("soft-dark"));
    // The engine has already applied whatever it restored. A named theme replaces
    // it; without one, a demo that has neither a restored theme nor a
    // command-line one would sit on the palette Qt came up with - the system's,
    // not one of ours.
    if (!themeId.isEmpty()) {
        engine->setTheme(themeId);
    } else if (!engine->hasTheme(engine->theme())) {
        engine->setTheme(QStringLiteral("soft-light"));
    }

    createThemeMenu();
}

void MainWindow::createThemeMenu()
{
    QxThemeManager *engine = QxThemeManager::instance();
    QMenu *menu = menuBar()->addMenu(tr("Theme"));

    QActionGroup *group = new QActionGroup(this);
    group->setExclusive(true);
    // One entry per registration: the ids are the engine's, the names are what
    // was registered under them, so a family added to res/themes.qrc appears
    // here without this file being told about it.
    const QStringList ids = engine->themeIds();
    for (const QString &id : ids) {
        QAction *entry = menu->addAction(engine->themeInfo(id).name);
        entry->setCheckable(true);
        entry->setChecked(engine->theme() == id);
        entry->setData(id);
        group->addAction(entry);
    }

    connect(group, &QActionGroup::triggered, [](QAction *entry) {
        QxThemeManager::instance()->setTheme(entry->data().toString());
    });
    connect(engine, &QxThemeManager::themeChanged, this, [group](const QString &id) {
        // Following the system swaps the theme without the menu being clicked,
        // so the check marks are read back from the engine rather than left where
        // the last click put them.
        const QList<QAction *> entries = group->actions();
        for (QAction *entry : entries) {
            entry->setChecked(entry->data().toString() == id);
        }
    });

    menu->addSeparator();
    // The motion switch is a gallery concern above all: two of the controls on
    // show are animated by definition - a skeleton breathes and a ring sweeps -
    // and this is where a reader sees what asking for less motion does to them.
    QAction *reduceMotion = menu->addAction(tr("Reduce motion"));
    reduceMotion->setCheckable(true);
    reduceMotion->setChecked(engine->reduceMotion());
    connect(reduceMotion, &QAction::toggled, [engine](bool on) {
        engine->setReduceMotion(on);
    });
}

void MainWindow::createStyleMenu()
{
    QMenu *menu = menuBar()->addMenu(tr("Style"));

    QActionGroup *group = new QActionGroup(this);
    group->setExclusive(true);
    // In the order the tabs were added, not in the order the hash happens to
    // walk: a menu of two entries is still a menu, and a reader who saw Material
    // first would take it for the default.
    for (const QString &id : m_styleOrder) {
        const int tab = m_styleTabs.value(id);
        QAction *entry = menu->addAction(m_tabs->tabText(tab));
        entry->setCheckable(true);
        entry->setChecked(m_tabs->currentIndex() == tab);
        entry->setData(id);
        group->addAction(entry);
    }
    connect(group, &QActionGroup::triggered, this, [this](QAction *entry) {
        applyStyle(entry->data().toString());
    });
    connect(m_tabs, &QTabWidget::currentChanged, this, [this, group](int index) {
        // The tab can change without the menu being clicked - the --style
        // sequence does exactly that - so the check marks are read back from the
        // tab widget.
        const QList<QAction *> entries = group->actions();
        for (QAction *entry : entries) {
            entry->setChecked(m_styleTabs.value(entry->data().toString(), -1) == index);
        }
    });
}
