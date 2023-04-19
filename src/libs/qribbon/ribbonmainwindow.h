/**
 * Copyleft (C) 2023 maminjie <canpool@163.com>
 **/
#pragma once

#include "ribbon_global.h"
#include <QMainWindow>

QRIBBON_BEGIN_NAMESPACE

class StatusBar;
class RibbonBar;
class OfficeFrameHelper;
class RibbonMainWindowPrivate;

class QRIBBON_EXPORT RibbonMainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit RibbonMainWindow(QWidget *parent = Q_NULL, Qt::WindowFlags flags = Qt::WindowFlags());
    ~RibbonMainWindow();
public:
    RibbonBar *ribbonBar() const;
    void setRibbonBar(RibbonBar *ribbonBar);

#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
    using QMainWindow::setCentralWidget;
    void setCentralWidget(QStyle *style);
#endif
protected:
    void setFrameHelper(OfficeFrameHelper *helper);

#ifdef Q_OS_WIN
    #if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
    virtual bool nativeEvent(const QByteArray &eventType, void *message, long *result);
    #else
    virtual bool winEvent(MSG *message, long *result);
    #endif
#endif   // Q_OS_WIN
private:
    friend class OfficeFrameHelperWin;
    QTC_DECLARE_PRIVATE(RibbonMainWindow)
    Q_DISABLE_COPY(RibbonMainWindow)
};

QRIBBON_END_NAMESPACE
