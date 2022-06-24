/****************************************************************************
**
** Qtitan Library by Developer Machines (Microsoft-Ribbon implementation for Qt.C++)
** 
** Copyright (c) 2009-2015 Developer Machines (http://www.devmachines.com)
**           ALL RIGHTS RESERVED
** 
**  The entire contents of this file is protected by copyright law and
**  international treaties. Unauthorized reproduction, reverse-engineering
**  and distribution of all or any portion of the code contained in this
**  file is strictly prohibited and may result in severe civil and 
**  criminal penalties and will be prosecuted to the maximum extent 
**  possible under the law.
**
**  RESTRICTIONS
**
**  THE SOURCE CODE CONTAINED WITHIN THIS FILE AND ALL RELATED
**  FILES OR ANY PORTION OF ITS CONTENTS SHALL AT NO TIME BE
**  COPIED, TRANSFERRED, SOLD, DISTRIBUTED, OR OTHERWISE MADE
**  AVAILABLE TO OTHER INDIVIDUALS WITHOUT WRITTEN CONSENT
**  AND PERMISSION FROM DEVELOPER MACHINES
**
**  CONSULT THE END USER LICENSE AGREEMENT FOR INFORMATION ON
**  ADDITIONAL RESTRICTIONS.
**
****************************************************************************/
#include <qevent.h>
#include <QPainter>
#include <QStyleOption>

#include "QtnRibbonWorkspace.h"
#include "QtnCommonStyle.h"

#ifdef DEBUG_MEMORY_ENABLED
#include "QtitanMSVSDebug.h"
#endif

QTITAN_USE_NAMESPACE


/*!
    \class Qtitan::RibbonWorkspace
    \internal
*/
RibbonWorkspace::RibbonWorkspace(QWidget* parent)
    : QWidget(parent)
{
    setAttribute(Qt::WA_MouseTracking);
    setAutoFillBackground(true);
}

RibbonWorkspace::~RibbonWorkspace()
{
}

void RibbonWorkspace::paintEvent(QPaintEvent* event)
{
    Q_UNUSED(event);
    QPainter p(this);
    QStyleOption opt;
    opt.init(this);
    if (event->rect().intersects(opt.rect))
        style()->drawPrimitive((QStyle::PrimitiveElement)CommonStyle::PE_Workspace, &opt, &p, this);
}

void RibbonWorkspace::mousePressEvent(QMouseEvent* event)
{
    Q_UNUSED(event);
    setFocus();
}
