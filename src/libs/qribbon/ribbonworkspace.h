/**
 * Copyleft (C) 2023 maminjie <canpool@163.com>
 **/
#pragma once

#include "ribbon_global.h"
#include <QWidget>

QRIBBON_BEGIN_NAMESPACE

/* RibbonWorkspace */
class QRIBBON_EXPORT RibbonWorkspace : public QWidget
{
    Q_OBJECT
public:
    RibbonWorkspace(QWidget *parent = Q_NULL);
    virtual ~RibbonWorkspace();
protected:
    virtual void paintEvent(QPaintEvent *event);
    virtual void mousePressEvent(QMouseEvent *event);
private:
    Q_DISABLE_COPY(RibbonWorkspace)
};

QRIBBON_END_NAMESPACE
