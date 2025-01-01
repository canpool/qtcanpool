/**
 * Copyright (C) 2018-2022 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
**/
#ifndef FANCYWINDOW_H
#define FANCYWINDOW_H

#include "qcanpool.h"
#include <QMainWindow>

QCANPOOL_BEGIN_NAMESPACE

class FancyBar;
class FancyWindowPrivate;

/* FancyWindow */
class QCANPOOL_SHARED_EXPORT FancyWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit FancyWindow(QWidget *parent = nullptr);
    virtual ~FancyWindow();

    FancyBar *fancyBar() const;

    void setFixedSize(const QSize &s);
    void setFixedSize(int w, int h);
    void setFixedWidth(int w);
    void setFixedHeight(int h);

    void setWindowFlags(Qt::WindowFlags type);

Q_SIGNALS:
    void resizable(bool resizable);

private:
    FancyWindowPrivate *d;
};

QCANPOOL_END_NAMESPACE

#endif // FANCYWINDOW_H
