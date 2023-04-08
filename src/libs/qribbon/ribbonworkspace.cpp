/**
 * Copyleft (C) 2023 maminjie <canpool@163.com>
 **/
#include "ribbonworkspace.h"
#include "commonstyle.h"
#include <QPainter>
#include <QStyleOption>
#include <qevent.h>

QRIBBON_USE_NAMESPACE

/*!
    \class QRibbon::RibbonWorkspace
    \internal
*/
RibbonWorkspace::RibbonWorkspace(QWidget *parent) : QWidget(parent)
{
    setAttribute(Qt::WA_MouseTracking);
    setAutoFillBackground(true);
}

RibbonWorkspace::~RibbonWorkspace() {}

void RibbonWorkspace::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    QPainter p(this);
    QStyleOption opt;
    opt.init(this);
    if (event->rect().intersects(opt.rect))
        style()->drawPrimitive((QStyle::PrimitiveElement)CommonStyle::PE_Workspace, &opt, &p, this);
}

void RibbonWorkspace::mousePressEvent(QMouseEvent *event)
{
    Q_UNUSED(event);
    setFocus();
}
