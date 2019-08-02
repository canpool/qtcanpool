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
#ifndef QTN_RIBBONMAINWINDOW_H
#define QTN_RIBBONMAINWINDOW_H

#include <QMainWindow>

#include "QtitanDef.h"

QTITAN_BEGIN_NAMESPACE

class StatusBar;
class RibbonBar;
class OfficeFrameHelper;
class RibbonMainWindowPrivate;

/* RibbonMainWindow */
class QTITAN_EXPORT RibbonMainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit RibbonMainWindow(QWidget* parent = Q_NULL, Qt::WindowFlags flags = 0);
    ~RibbonMainWindow();

public:
    RibbonBar* ribbonBar() const;
    void setRibbonBar(RibbonBar* ribbonBar);

#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
    using QMainWindow::setCentralWidget;
    void setCentralWidget(QStyle* style);
#endif

protected:
    void setFrameHelper(OfficeFrameHelper* helper);

protected:
    virtual void paintEvent(QPaintEvent* event);

#ifdef Q_OS_WIN
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
    virtual bool nativeEvent(const QByteArray& eventType, void *message, long *result);
#else
    virtual bool winEvent(MSG* message, long* result);
#endif
#endif // Q_OS_WIN

private:
    friend class OfficeFrameHelperWin;
    QTN_DECLARE_PRIVATE(RibbonMainWindow)
    Q_DISABLE_COPY(RibbonMainWindow)
};

QTITAN_END_NAMESPACE

#endif // QTN_RIBBONMAINWINDOW_H
