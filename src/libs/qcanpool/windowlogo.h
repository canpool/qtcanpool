/**
 * Copyright (C) 2023 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#pragma once

#include "qcanpool.h"
#include <QWidget>

QCANPOOL_BEGIN_NAMESPACE

class WindowLogoPrivate;
/* WindowLogo */
class QCANPOOL_SHARED_EXPORT WindowLogo : public QWidget
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
    QCP_DECLARE_PRIVATE(WindowLogo)
};

QCANPOOL_END_NAMESPACE
