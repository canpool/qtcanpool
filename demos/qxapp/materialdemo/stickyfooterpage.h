/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#ifndef STICKYFOOTERPAGE_H
#define STICKYFOOTERPAGE_H

#include "demopage.h"

#include "qxmaterial/qxmaterial_global.h"

QX_MATERIAL_BEGIN_NAMESPACE
class MaterialPagination;
QX_MATERIAL_END_NAMESPACE

/*!
 * A page too short to fill the window, and the footer that stays at the bottom
 * of it anyway.
 *
 * The whole of this screen is one decision: what takes up the room that the
 * content does not. Left to the layout, a short page pushes its footer up under
 * the last line, and the reader gets a page that ends two thirds of the way down
 * with a band of nothing under it - which reads as a page that failed to finish
 * loading. The stretch between the two is what puts the footer at the foot of
 * the window instead of under the content, and it keeps it there as the window
 * changes height.
 *
 * The footer is a paginator because that is what a foot of a page is for on a
 * screen that has more to say than it is saying: this one article of four, and
 * the three that follow it.
 */
class StickyFooterPage : public DemoPage
{
    Q_OBJECT
public:
    explicit StickyFooterPage(QWidget *parent = Q_NULLPTR);
private:
    QX_MATERIAL_PREPEND_NAMESPACE(MaterialPagination) *m_pages = Q_NULLPTR;
};

#endif   // STICKYFOOTERPAGE_H
