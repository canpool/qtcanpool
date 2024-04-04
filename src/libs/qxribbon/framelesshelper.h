/**
 * Copyleft (C) 2023-2024 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MIT
**/
#pragma once

#include <QObject>
#include <QAbstractNativeEventFilter>

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
#define QTRESULT qintptr
#else
#define QTRESULT long
#endif // QTRESULT

class QWidget;
class FramelessHelperPrivate;

class FramelessHelper : public QObject, public QAbstractNativeEventFilter
{
    Q_OBJECT
public:
    explicit FramelessHelper(QObject *parent);
    ~FramelessHelper();

    void addWidget(QWidget *w);
    void removeWidget(QWidget *w);

    bool widgetResizable() const;
    void setWidgetResizable(bool resizable);

    bool widgetMovable() const;
    void setWidgetMovable(bool movable);

    int titleHeight() const;
    void setTitleHeight(int height);

    int borderWidth() const;
    void setBorderWidth(int width);

    void addCaptionClassName(const QString &name);

signals:
    void windowIconChanged(const QIcon &icon);
    void windowTitleChanged(const QString &title);
    void windowStateChanged(Qt::WindowStates state);

protected:
    virtual bool eventFilter(QObject* object, QEvent* event);
    virtual bool nativeEventFilter(const QByteArray &eventType, void *message, QTRESULT *result);

private:
    FramelessHelperPrivate *d;
};
