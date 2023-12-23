/**
 * Copyright (C) 2023 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
**/
#ifndef FRAMELESSHELPER_H
#define FRAMELESSHELPER_H

#include "qxframeless_global.h"
#include <QObject>
#include <QAbstractNativeEventFilter>

#ifndef QXRESULT
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
#define QXRESULT qintptr
#else
#define QXRESULT long
#endif // QT_VERSION
#endif // QXRESULT

QX_FRAMELESS_BEGIN_NAMESPACE

class FramelessHelperPrivate;

class QX_FRAMELESS_EXPORT FramelessHelper : public QObject, public QAbstractNativeEventFilter
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
    virtual bool nativeEventFilter(const QByteArray &eventType, void *message, QXRESULT *result);

private:
    QX_DECLARE_PRIVATE(FramelessHelper)
};

QX_FRAMELESS_END_NAMESPACE

#endif // FRAMELESSHELPER_H
