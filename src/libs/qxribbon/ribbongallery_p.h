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
/* RibbonGalleryViewport */
class QX_RIBBON_EXPORT RibbonGalleryViewport : public QWidget
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
public slots:
    void widgetTitleChanged(QWidget *w, const QString &title);
private:
    QX_DECLARE_PRIVATE(RibbonGalleryViewport)
};

QX_RIBBON_END_NAMESPACE
