/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#ifndef HOMEPAGE_H
#define HOMEPAGE_H

#include "demopage.h"

class QGridLayout;

/*! The first screen: what this application is, and the index of what it
 *  demonstrates.
 *
 *  A gallery has two ways in - the pane on the left and the index on the first
 *  page - and they say the same thing in different shapes: the pane is a list to
 *  walk down, the index is the whole set at once, which is what a reader wants
 *  before they know what they are looking for. What the index cannot know is
 *  where each control ended up, so the window hands the destinations over and
 *  the page asks for one by number.
 */
class HomePage : public DemoPage
{
    Q_OBJECT
public:
    explicit HomePage(QWidget *parent = Q_NULLPTR);

    /*! Puts \a titles in the index, the first of them at destination
     *  \a firstDestination. */
    void setCatalogue(const QStringList &titles, int firstDestination);
Q_SIGNALS:
    /*! The reader asked for destination \a index. */
    void destinationRequested(int index);
private:
    QGridLayout *m_index = Q_NULLPTR;
    int m_firstDestination = 0;
};

#endif   // HOMEPAGE_H
