/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#ifndef TST_PROBE_H
#define TST_PROBE_H

#include <QString>
#include <QStringList>

class QWidget;
class QSplitter;

/*!
 * The instruments a scene test reaches for, kept here so that the suites that
 * need them ask the same question the same way.
 *
 * A control answers "what does it look like" by painting, and a test that wants
 * to say something about that has to look at the pixels. Two suites that each
 * wrote their own way of looking would each be right about their own widgets
 * and incomparable with each other - which is how a difference between what a
 * window was asked for and what it drew stayed unexplained for a whole release.
 * So the geometry, the tree and the ink are read here, once, and a scene test
 * says what it expects of the reading.
 *
 * It is test infrastructure and not a library: it is compiled into the suites
 * that ask for it, and nothing ships it.
 */
namespace Probe
{

/*!
 * One widget, in one line: what class it is, what it is called, where it is,
 * how big, whether it is showing, and what it painted.
 */
QString line(QWidget *widget);

/*! The tree under \a root, one line per widget, in the order they were made. */
QString tree(QWidget *root);

/*!
 * What \a widget painted behind its own children, measured rather than read.
 *
 * A style sheet keeps no public record of what it resolved, and a widget with
 * no rule of its own renders nothing at all - so "does this one cover the
 * surface it sits on" is not a question a sheet can answer. Painting the widget
 * alone and counting what it filled is the answer.
 */
QString painted(QWidget *widget);

/*! The colour of the glyph \a widget carries - a label's pixmap, a button's
 *  icon - or empty when it carries none. An icon that never got tinted keeps
 *  the colour it shipped with, which is the one thing the eye reports. */
QString glyph(QWidget *widget);

/*!
 * Run the layout and drain the queue, so that what a tree says is what a
 * capture of the same moment shows.
 *
 * A tree read before the layout has run describes a window that has not been
 * laid out: what it says about geometry is what the window will not be.
 */
void settle(QWidget *window);

/*!
 * Show what collapsed. An auto hide container paints nothing while it is shut,
 * and off a real desktop it shuts again before anything can look at it - so the
 * view worth describing is the one that has to be asked for.
 */
void revealCollapsed(QWidget *root);

/*! The lines of \a tree that mention \a what. */
QStringList lines(const QString &tree, const QString &what);

/*!
 * The height the first line of \a tree that mentions \a what reports, or -1
 * when no such line says one.
 *
 * A scene test mostly wants to know whether a thing survived a change - a tab
 * bar that kept the height it had before a theme was switched is the shape of
 * the bug that went unexplained - and that is one number out of one line.
 */
int heightOf(const QString &tree, const QString &what);

/*! How long \a splitter is along the way it divides things up. */
int lengthOf(QSplitter *splitter);

/*!
 * Whether \a splitter measures what it is holding: every part non-zero, and the
 * parts together as long as the splitter.
 *
 * A docking layout is a set of sizes that were measured once, and everything
 * wrong with it shows up as a size measured against something that has since
 * changed - a pane handed a size out of the old metrics is either zero or
 * leaves a gap. Both are visible in these two numbers and neither is visible
 * in a single-step test, which is why this is asked after every step of a
 * sequence rather than once at the end.
 */
bool fillsItself(QSplitter *splitter);

/*! \a splitter in one line: how many parts, what size each, how long it is.
 *  The line a failed QVERIFY2 prints, so it has to name the number that was
 *  wrong and not only that one was. */
QString describe(QSplitter *splitter);

}   // namespace Probe

#endif   // TST_PROBE_H
