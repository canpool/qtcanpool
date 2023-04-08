/**
 * Copyleft (C) 2023 maminjie <canpool@163.com>
 **/
#include "ribbonmainwindow.h"
#include "officeframehelper.h"
#include "ribbonbar.h"
#include "ribbonstyle.h"
#include <QLayout>
#include <QPainter>

QRIBBON_USE_NAMESPACE

QRIBBON_BEGIN_NAMESPACE

class RibbonMainWindowPrivate : public QObject
{
public:
    QTC_DECLARE_PUBLIC(RibbonMainWindow)
public:
    explicit RibbonMainWindowPrivate();
public:
    OfficeFrameHelper *m_frameHelper;
    bool m_attrOpaquePaintEvent;
    bool m_attrNoSystemBackground;
};

QRIBBON_END_NAMESPACE

RibbonMainWindowPrivate::RibbonMainWindowPrivate() { m_frameHelper = Q_NULL; }

/*!
    \class RibbonMainWindow
    \inmodule ribbon
    \brief RibbonMainWindow class represents the main window of the application used to create the Ribbon UI.
*/

/*!
\brief Constructs RibbonMainWindow object with the given \a parent and \a flags.
*/
RibbonMainWindow::RibbonMainWindow(QWidget *parent, Qt::WindowFlags flags) : QMainWindow(parent, flags)
{
    RibbonBar::loadTranslation();
    QTC_INIT_PRIVATE(RibbonMainWindow);
    setObjectName("RibbonMainWindow");
    QTC_D(RibbonMainWindow);
    d->m_attrOpaquePaintEvent = testAttribute(Qt::WA_OpaquePaintEvent);
    d->m_attrNoSystemBackground = testAttribute(Qt::WA_NoSystemBackground);
}

/*!
\brief Destructor of the RibbonMainWindow object.
*/
RibbonMainWindow::~RibbonMainWindow() { QTC_FINI_PRIVATE(); }

/*
\brief Returns a pointer to RibbonBar object. If RibbonBar does not exist, then the function will create an empty Ribbon
UI panel.
*/
RibbonBar *RibbonMainWindow::ribbonBar() const
{
    RibbonBar *ribbonBar = Q_NULL;

    QWidget *menu = menuWidget();
    if (menu)
        ribbonBar = qobject_cast<RibbonBar *>(menu);

    if (!menu && !ribbonBar) {
        RibbonMainWindow *self = const_cast<RibbonMainWindow *>(this);
        ribbonBar = new RibbonBar(self);
        self->setMenuWidget(ribbonBar);
    }
    return ribbonBar;
}

/*
\brief Sets the RibbonBar to the main window. Object RibbonMainWindow takes ownership of the \a ribbonBar. The method is
used when needed to set the custom Ribbon Bar object, whose class, inherited from RibbonBar.
*/
void RibbonMainWindow::setRibbonBar(RibbonBar *ribbonBar) { setMenuWidget(ribbonBar); }

#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
/*! \internal */
void RibbonMainWindow::setCentralWidget(QStyle *style) { Q_UNUSED(style); }
#endif

/*! \internal */
void RibbonMainWindow::setFrameHelper(OfficeFrameHelper *helper)
{
    QTC_D(RibbonMainWindow);
    d->m_frameHelper = helper;

    if (helper) {
        d->m_attrOpaquePaintEvent = testAttribute(Qt::WA_OpaquePaintEvent);
        d->m_attrNoSystemBackground = testAttribute(Qt::WA_NoSystemBackground);
        setAttribute(Qt::WA_OpaquePaintEvent, true);
        setAttribute(Qt::WA_NoSystemBackground, true);
    } else {
        setAttribute(Qt::WA_OpaquePaintEvent, d->m_attrOpaquePaintEvent);
        setAttribute(Qt::WA_NoSystemBackground, d->m_attrNoSystemBackground);
    }
}

#ifdef Q_OS_WIN
    #if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
bool RibbonMainWindow::nativeEvent(const QByteArray &eventType, void *message, long *result)
    #else
bool RibbonMainWindow::winEvent(MSG *message, long *result)
    #endif
{
    QTC_D(RibbonMainWindow);
    #if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
    if (d->m_frameHelper) {
        MSG *msg = static_cast<MSG *>(message);
        if (d->m_frameHelper->winEvent(msg, result))
            return true;
    }

    return QMainWindow::nativeEvent(eventType, message, result);
    #else
    if (d->m_frameHelper)
        return d->m_frameHelper->winEvent(message, result);

    return QMainWindow::winEvent(message, result);
    #endif
}
#endif   // Q_OS_WIN
