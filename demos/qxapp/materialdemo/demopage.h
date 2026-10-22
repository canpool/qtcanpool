/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#ifndef DEMOPAGE_H
#define DEMOPAGE_H

#include <QtWidgets/QWidget>

class QFormLayout;
class QVBoxLayout;

/*!
 * One screen of the demo: the thing the navigation rail opens.
 *
 * A page here is a screen, the way an application would build one out of these
 * controls: a name, a line saying what the screen is for, and then sections
 * that hold either a caption-and-control row or something with room of its own.
 * The difference between the two is what the helpers below are for, and it is
 * the only arrangement a page decides.
 *
 * The typography is Material's own scale, asked for here rather than left to
 * whichever font the application happens to be built with: a screen that reads
 * at one size from top to bottom is a screen with no shape to it.
 */
class DemoPage : public QWidget
{
    Q_OBJECT
public:
    explicit DemoPage(const QString &title, const QString &intro, QWidget *parent = Q_NULLPTR);

    /*! What this screen is called: the rail, the caption and the page's own
     *  heading all read it here, so a place has one name rather than three. */
    QString title() const;

    /*! Starts a section named \a text. Rows and blocks after this belong to it. */
    void addSection(const QString &text);
    /*! A wrapped paragraph at body size: what a screen says about itself. */
    void addNote(const QString &text);
    /*! Adds \a control as a row captioned \a caption. */
    void addRow(const QString &caption, QWidget *control);
    /*! Adds \a widget across the room the page has, for something too wide to
     *  be a row - a card, a row of chips, a surface that answers the pointer. */
    void addBlock(QWidget *widget);
    /*! Takes up what is left, so a short screen does not stretch its rows. */
    void addStretch();
Q_SIGNALS:
    /*! Something on this screen has a sentence to say, and the window is what
     *  says it: a snackbar belongs to the window it rises in, so a screen that
     *  has something to report asks rather than keeping one of its own. */
    void messageRequested(const QString &message);
private:
    QString m_title;
    QVBoxLayout *m_layout = Q_NULLPTR;
    /*! The rows of the current section; a section or a block ends it. */
    QFormLayout *m_form = Q_NULLPTR;
};

#endif   // DEMOPAGE_H
