/**
 * Copyleft (C) 2023 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MIT
**/
#pragma once

#include <QObject>

#ifdef Q_OS_WINDOWS
#include <QAbstractNativeEventFilter>

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
#define QTRESULT qintptr
#else
#define QTRESULT long
#endif // QTRESULT
#endif // Q_OS_WINDOWS

class QWidget;
class FramelessHelperPrivate;

#ifdef Q_OS_WINDOWS
class FramelessHelper : public QObject, public QAbstractNativeEventFilter
#else
class FramelessHelper : public QObject
#endif
{
    Q_OBJECT
public:
    explicit FramelessHelper(QObject *parent);
    ~FramelessHelper();
    // 激活窗体
    void activateOn(QWidget *topLevelWidget);
    // 移除窗体
    void removeFrom(QWidget *topLevelWidget);
    // 设置窗体移动
    void setWidgetMovable(bool movable);
    // 设置窗体缩放
    void setWidgetResizable(bool resizable);
    // 设置橡皮筋移动
    void setRubberBandOnMove(bool movable);
    // 设置橡皮筋缩放
    void setRubberBandOnResize(bool resizable);
    // 设置边框的宽度
    void setBorderWidth(int width);
    // 设置标题栏高度
    void setTitleHeight(int height);
    bool widgetResizable();
    bool widgetMovable();
    bool rubberBandOnMove();
    bool rubberBandOnResisze();
    uint borderWidth();
    uint titleHeight();
protected:
    // 事件过滤，进行移动、缩放等
    virtual bool eventFilter(QObject *obj, QEvent *event);
#ifdef Q_OS_WINDOWS
    virtual bool nativeEventFilter(const QByteArray &eventType, void *message, QTRESULT *result);
#endif
private:
    FramelessHelperPrivate *d;
};
