/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#include "tst_global.h"

#include "tst_probe.h"

#include "qxdock/dockwidget.h"
#include "qxdock/dockwindow.h"

#include <QApplication>
#include <QStringList>
#include <QTest>
#include <QWidget>

QX_DOCK_USE_NAMESPACE

namespace
{
const char *const kProbeProperty = "qxProbe";
const char *const kProbeName = "qxProbeId";

/*! Every widget in \a window whose class is \a what, and the window itself when
 *  it is one - a window is not among its own children. */
QWidget *widgetOfClass(QWidget *window, const QString &what)
{
    if (QLatin1String(window->metaObject()->className()) == what) {
        return window;
    }
    for (QWidget *widget : window->findChildren<QWidget *>()) {
        if (QLatin1String(widget->metaObject()->className()) == what) {
            return widget;
        }
    }
    return nullptr;
}

/*! Every dock class the tree of \a window mentions, in the order it mentions
 *  them. The matrix is built from what is really there rather than from a list
 *  that would silently skip a class that is not. */
QStringList dockClasses(QWidget *window)
{
    QStringList classes;
    for (const QString &line : Probe::tree(window).split(QLatin1Char('\n'))) {
        const QString name = line.trimmed().section(QLatin1Char(' '), 0, 0);
        if (!name.startsWith(QStringLiteral("QxDock::")) || classes.contains(name)) {
            continue;
        }
        classes.append(name);
    }
    return classes;
}

/*! Whether \a widget can be asked what it painted at all. A widget that is not
 *  showing has nothing to grab, and "did the rule land" is not a question that
 *  can be answered about it. */
bool canBeRead(QWidget *widget)
{
    return Probe::painted(widget) != QStringLiteral("(not painted)") &&
           Probe::painted(widget) != QStringLiteral("transparent");
}

/*! The four ways a class can be named in a rule, the way an application would
 *  spell each of them: the namespaced type the bundled sheets use, the same as
 *  a class, narrowed by a dynamic property, and narrowed by an object name.
 *
 *  \a widget is given both a property and a name for the duration, because a
 *  selector that names one cannot be tested on a widget that has none. */
QStringList selectorsFor(QWidget *widget)
{
    const QString name = QString::fromLatin1(widget->metaObject()->className()).section(QStringLiteral("::"), 1);
    const QString id = widget->objectName().isEmpty() ? QString::fromLatin1(kProbeName) : widget->objectName();
    widget->setProperty(kProbeProperty, QStringLiteral("1"));
    widget->setObjectName(id);
    return QStringList{QStringLiteral("QxDock--%1").arg(name), QStringLiteral(".QxDock--%1").arg(name),
                       QStringLiteral("QxDock--%1[%2=\"1\"]").arg(name, QString::fromLatin1(kProbeProperty)),
                       QStringLiteral("QxDock--%1#%2").arg(name, id)};
}
}   // namespace

/*!
 * A dock is styled from a sheet it does not own: the application builds one, on
 * every theme change, and every dock class paints from whatever landed in it.
 * So "can an application style this class" is a question about selectors, and
 * it is worth asking of all of them at once - 4.6 ended with a class that was
 * believed to answer to none of the four forms, and with a fix rolled back
 * because of it.
 *
 * The matrix is read, not declared: the classes are whatever the tree of a
 * window that is standing there mentions, and a rule is judged by whether the
 * widget painted something it did not paint before.
 */
class tst_DockSheet : public QObject
{
    Q_OBJECT
private slots:
    void init();
    void cleanup();

    void everyDockClassAnswersAllFourSelectors();
    void aColourLandsOnEveryClassThatDoesNotPaintItsOwnSurface();
private:
    bool lands(QWidget *widget, const QString &rule);

    QString m_sheet;
};

void tst_DockSheet::init()
{
    m_sheet = qApp->styleSheet();
}

void tst_DockSheet::cleanup()
{
    qApp->setStyleSheet(m_sheet);
}

/*! Whether \a rule changes what \a widget paints. */
bool tst_DockSheet::lands(QWidget *widget, const QString &rule)
{
    qApp->setStyleSheet(QString());
    Probe::settle(widget);
    const QString before = Probe::painted(widget);

    qApp->setStyleSheet(rule);
    Probe::settle(widget);
    return Probe::painted(widget) != before;
}

/*! A rule has to reach the class it names, whichever of the four ways it names
 *  it. A class that answers to only some of them is a class an application can
 *  style only by knowing which. */
void tst_DockSheet::everyDockClassAnswersAllFourSelectors()
{
    DockWindow window;
    window.resize(800, 600);
    DockWidget pane(QStringLiteral("pane"));
    window.addDockWidget(Qx::CenterDockWidgetArea, &pane);
    window.show();
    Probe::settle(&window);

    for (const QString &name : dockClasses(&window)) {
        QWidget *widget = widgetOfClass(&window, name);
        QVERIFY2(widget, qPrintable(Probe::tree(&window)));
        if (!canBeRead(widget)) {
            continue;
        }
        const QStringList selectors = selectorsFor(widget);
        for (const QString &selector : selectors) {
            // The border is the mount point that shows up whatever a class
            // paints over itself, which is the point of asking it here: a
            // colour is a claim about a surface, and some of these have one of
            // their own.
            QVERIFY2(lands(widget, QStringLiteral("%1 { border: 6px solid #ff0000; }").arg(selector)),
                     qPrintable(QStringLiteral("%1 did not answer to %2").arg(name, selector)));
        }
    }
}

/*! What the border cannot say: whether a colour reaches the surface. Most of
 *  these classes are painted by the style from the sheet, so a colour lands;
 *  the tab bar paints its own surface, so a colour on it does not - it is the
 *  one class in the matrix where the two mount points disagree, and it is
 *  pinned here rather than left to be rediscovered. */
void tst_DockSheet::aColourLandsOnEveryClassThatDoesNotPaintItsOwnSurface()
{
    DockWindow window;
    window.resize(800, 600);
    DockWidget pane(QStringLiteral("pane"));
    window.addDockWidget(Qx::CenterDockWidgetArea, &pane);
    window.show();
    Probe::settle(&window);

    const QString tabBar = QStringLiteral("QxDock::DockTabBar");
    QVERIFY2(dockClasses(&window).contains(tabBar), qPrintable(Probe::tree(&window)));

    for (const QString &name : dockClasses(&window)) {
        QWidget *widget = widgetOfClass(&window, name);
        QVERIFY2(widget, qPrintable(Probe::tree(&window)));
        if (!canBeRead(widget)) {
            continue;
        }
        selectorsFor(widget);
        const bool lands = this->lands(
            widget,
            QStringLiteral("QxDock--%1 { background-color: #ff0000; }").arg(name.section(QStringLiteral("::"), 1)));
        if (name == tabBar) {
            QVERIFY2(!lands, "the tab bar takes a colour from the sheet now - it used to paint its own surface");
        } else {
            QVERIFY2(lands, qPrintable(QStringLiteral("a colour did not land on %1").arg(name)));
        }
    }
}

TEST_ADD(tst_DockSheet)

#include "tst_docksheet.moc"
