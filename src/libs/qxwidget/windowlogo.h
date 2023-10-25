/**
 * Copyright (C) 2023 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#pragma once

#include "qxwidget_global.h"
#include <QWidget>

QX_WIDGET_BEGIN_NAMESPACE

class WindowLogoPrivate;
/* WindowLogo */
class QX_WIDGET_EXPORT WindowLogo : public QWidget
{
    Q_OBJECT
public:
    explicit WindowLogo(const QPixmap &pixmap, QWidget *parent = Q_NULLPTR);
    ~WindowLogo();

    void setPixmap(const QPixmap &pixmap);
    QPixmap pixmap() const;

    void setHasMenu(bool has);
    bool hasMenu();

    void setRound(bool round);

    virtual QSize sizeHint() const;
    virtual QSize minimumSizeHint() const;
signals:
    void menuTriggered(QMouseEvent *e);
    void clicked();
protected:
    virtual void mousePressEvent(QMouseEvent *event);
    virtual void mouseReleaseEvent(QMouseEvent *event);
    virtual void paintEvent(QPaintEvent *event);
private:
    QX_DECLARE_PRIVATE(WindowLogo)
};

QX_WIDGET_END_NAMESPACE
