/**
 * Copyleft (C) 2023 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MIT
 **/
#pragma once

#include "qxribbon_global.h"
#include <QWidget>

class QLabel;

QX_RIBBON_BEGIN_NAMESPACE

class RibbonGalleryViewportPrivate;
/*
 * RibbonGalleryViewport, the popup RibbonGallery shows for "show more".
 *
 * It was privatized on purpose (it belongs to RibbonGallery, which hands out
 * nothing but the widget it manages), and it carries no export macro for that
 * reason - the declaration has to match the status, not the accident of this
 * header being installed. Every .h listed in a library's SOURCES is installed,
 * _p.h included, so being reachable on disk is not a promise: a caller that
 * wants its own popup overrides RibbonGallery::showMoreTrigger() instead and
 * never names this class.
 */
class RibbonGalleryViewport : public QWidget
{
    Q_OBJECT
public:
    RibbonGalleryViewport(QWidget *parent);
    ~RibbonGalleryViewport();
public:
    void addWidget(QWidget *w);
    void addWidget(QWidget *w, const QString &title);
    void removeWidget(QWidget *w);
    QLabel *getWidgetTitleLabel(QWidget *w);
public Q_SLOTS:
    void widgetTitleChanged(QWidget *w, const QString &title);
private:
    QX_DECLARE_PRIVATE(RibbonGalleryViewport)
};

QX_RIBBON_END_NAMESPACE
