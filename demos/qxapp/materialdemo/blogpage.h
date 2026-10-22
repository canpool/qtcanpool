/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#ifndef BLOGPAGE_H
#define BLOGPAGE_H

#include "demopage.h"

/*!
 * Blog: a page that is text, and has to stay readable for the length of it.
 *
 * Most of what a window shows is a control and a word beside it; a post is the
 * other case - one column of prose that the user reads down, with headings in
 * it and a quotation pulled out of it. The library ships no rich text of its
 * own, so the post is a \c QTextBrowser holding local rich text: no markdown
 * library, no web engine, and the document is part of the demo rather than
 * fetched from anywhere.
 */
class BlogPage : public DemoPage
{
    Q_OBJECT
public:
    explicit BlogPage(QWidget *parent = Q_NULLPTR);
};

#endif   // BLOGPAGE_H
