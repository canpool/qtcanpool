/****************************************************************************
**
** Qtitan Library by Developer Machines (Microsoft-Ribbon implementation for Qt.C++)
** 
** Copyright (c) 2009-2013 Developer Machines (http://www.devmachines.com)
** Copyright (c) 2019 MaMinJie <canpool@163.com>
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
#include <QPainter>
#include <QLayout>

#include "QtnRibbonBar.h"
#include "QtnRibbonStyle.h"
#include "QtnRibbonMainWindow.h"
#ifdef QTITAN_DEMO
#include "QtnDemo.h"
#endif

#include "QtnOfficeFrameHelper.h"

QTITAN_USE_NAMESPACE

QTITAN_BEGIN_NAMESPACE
/* RibbonMainWindowPrivate */
class RibbonMainWindowPrivate : public QObject
{
public:
    QTN_DECLARE_PUBLIC(RibbonMainWindow)
public:
    explicit RibbonMainWindowPrivate();
public:
    OfficeFrameHelper* m_frameHelper;
    bool m_attrOpaquePaintEvent;
    bool m_attrNoSystemBackground;
};
QTITAN_END_NAMESPACE

RibbonMainWindowPrivate::RibbonMainWindowPrivate()
{
    m_frameHelper = Q_NULL;
}


/*!
    \class Qtitan::RibbonMainWindow
    \inmodule QtitanRibbon
    \brief RibbonMainWindow class represents the main window of the application used to create the Ribbon UI.
*/

/*!
\brief Constructs RibbonMainWindow object with the given \a parent and \a flags.
*/ 
RibbonMainWindow::RibbonMainWindow(QWidget* parent, Qt::WindowFlags flags)
  : QMainWindow(parent, flags)
{
    QTN_INIT_PRIVATE(RibbonMainWindow);
    setObjectName(QLatin1String("RibbonMainWindow"));
    QTN_D(RibbonMainWindow);
    d.m_attrOpaquePaintEvent = testAttribute(Qt::WA_OpaquePaintEvent);
    d.m_attrNoSystemBackground = testAttribute(Qt::WA_NoSystemBackground);
#ifdef QTITAN_DEMO
    START_QTITAN_DEMO
#endif
}

/*!
\brief Destructor of the RibbonMainWindow object.
*/ 
RibbonMainWindow::~RibbonMainWindow()
{
    QTN_FINI_PRIVATE();
}

RibbonBar* RibbonMainWindow::ribbonBar() const
{
    RibbonBar* ribbonBar = Q_NULL;

    QWidget* menu = menuWidget();
    if (menu)
        ribbonBar = qobject_cast<RibbonBar*>(menu);

    if (!menu && !ribbonBar) 
    {
        RibbonMainWindow* self = const_cast<RibbonMainWindow*>(this);
        ribbonBar = new RibbonBar(self);
        self->setMenuWidget(ribbonBar);
    }
    return ribbonBar;
}

void RibbonMainWindow::setRibbonBar(RibbonBar* ribbonBar)
{
    setMenuWidget(ribbonBar);
}

#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
/*! \internal */
void RibbonMainWindow::setCentralWidget(QStyle* style)
{
    Q_UNUSED(style);
}
#endif

/*! \internal */
void RibbonMainWindow::setFrameHelper(OfficeFrameHelper* helper)
{
    QTN_D(RibbonMainWindow);
    d.m_frameHelper = helper;

    if (helper)
    {
        d.m_attrOpaquePaintEvent = testAttribute(Qt::WA_OpaquePaintEvent);
        d.m_attrNoSystemBackground = testAttribute(Qt::WA_NoSystemBackground);
        setAttribute(Qt::WA_OpaquePaintEvent, true);
        setAttribute(Qt::WA_NoSystemBackground, true);
    }
    else
    {
        setAttribute(Qt::WA_OpaquePaintEvent, d.m_attrOpaquePaintEvent);
        setAttribute(Qt::WA_NoSystemBackground, d.m_attrNoSystemBackground);
    }
}

/*! \reimp */
void RibbonMainWindow::paintEvent(QPaintEvent* event)
{
    Q_UNUSED(event);
    QTN_D(RibbonMainWindow);
    if (d.m_frameHelper)
    {
        QPainter p(this);
        p.fillRect(0, 0, width(), height(), palette().brush(backgroundRole()));
    }
}

#ifdef Q_OS_WIN
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
bool RibbonMainWindow::nativeEvent(const QByteArray& eventType, void* message, long* result)
#else
bool RibbonMainWindow::winEvent(MSG* message, long* result)
#endif
{
    QTN_D(RibbonMainWindow);
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
    if (d.m_frameHelper)
    {
        MSG* msg = static_cast<MSG *>(message);
        if (d.m_frameHelper->winEvent(msg, result))
            return true;
    }

    return QMainWindow::nativeEvent(eventType, message, result);
#else
    if (d.m_frameHelper)
        return d.m_frameHelper->winEvent(message, result);

    return QMainWindow::winEvent(message, result);
#endif
}
#endif // Q_OS_WIN
