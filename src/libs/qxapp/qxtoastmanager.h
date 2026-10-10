/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#ifndef QXTOASTMANAGER_H
#define QXTOASTMANAGER_H

#include "qxapp_global.h"
#include "qxtoast.h"

#include <QtCore/QList>
#include <QtCore/QObject>

QT_BEGIN_NAMESPACE
class QEvent;
QT_END_NAMESPACE

QX_APP_BEGIN_NAMESPACE

class QxToastManagerPrivate;

/*!
 * The stack of toasts of one window.
 *
 * A manager belongs to a host window - the shell, or any other widget that is
 * big enough to carry an overlay - and puts every toast it is handed into a
 * column anchored to one of that window's corners. It is the queue that keeps
 * the screen readable: once more than maxVisible() toasts are up, the oldest
 * one is evicted, so a burst of messages leaves the last few on screen instead
 * of covering the window.
 *
 * @code
 * QxToastManager *toasts = new QxToastManager(shell);
 * toasts->setPosition(QxToastManager::BottomRight);
 * toasts->show(tr("Connected"), QxToast::Success);
 * toasts->show(tr("The device did not answer"), QxToast::Error, 10000);
 * @endcode
 *
 * The queue this class reports is what is on screen: show() adds to it, a
 * timeout, a click, an eviction or dismissAll() takes out of it, and both
 * happen straight away rather than when the animation is over. The toast widget
 * fades out behind the bookkeeping, which is what lets the tests assert on the
 * stack without waiting for a clock.
 *
 * The manager owns the toasts it creates. They are children of the host, so
 * destroying the host destroys them too.
 */
class QX_APP_EXPORT QxToastManager : public QObject
{
    Q_OBJECT
public:
    /*! The corner of the host the stack is anchored to. */
    enum Position {
        TopLeft,
        TopRight,
        BottomLeft,
        BottomRight,
    };
    Q_ENUM(Position)

    /*! Milliseconds a toast stays on screen when the caller does not say. */
    static constexpr int DefaultTimeout = QxToast::DefaultTimeout;

    explicit QxToastManager(QWidget *host, QObject *parent = Q_NULLPTR);
    ~QxToastManager() override;

    QWidget *host() const;

    /*! How many toasts share the screen before the oldest one is evicted. */
    int maxVisible() const;
    void setMaxVisible(int count);

    Position position() const;
    void setPosition(Position position);

    /*! Distance kept between the plate and the two edges it is anchored to. */
    int margin() const;
    void setMargin(int pixels);
    /*! Distance between two plates of the stack. */
    int spacing() const;
    void setSpacing(int pixels);
    /*! Widest a plate may get; a narrow host shrinks it further. */
    int maxWidth() const;
    void setMaxWidth(int pixels);
    /*! Milliseconds the fade in and the fade out take; zero turns them off. */
    int fadeDuration() const;
    void setFadeDuration(int ms);

    int count() const;
    /*! The toasts that are on screen, oldest first - the order they stack in. */
    QList<QxToast *> toasts() const;
    /*! The toast at \a index of the stack, null when it is out of range. */
    QxToast *toastAt(int index) const;
    bool contains(QxToast *toast) const;

    /*!
     * Creates a toast, puts it at the end of the stack and starts it. Adding one
     * past maxVisible() evicts the oldest, so the returned toast is always on
     * screen. A negative \a timeoutMs is the "stays until dismissed" case.
     */
    QxToast *show(const QString &text, QxToast::Level level = QxToast::Information,
                  int timeoutMs = QxToast::DefaultTimeout);
    /*! Dismisses every toast of the stack, oldest first. */
    void dismissAll();

Q_SIGNALS:
    /*! A toast has joined the stack and is on its way in. */
    void toastShown(QxToast *toast);
    /*! A toast has left the stack; its widget fades out right after this. */
    void toastDismissed(QxToast *toast);
protected:
    bool eventFilter(QObject *watched, QEvent *event) override;
private:
    Q_DISABLE_COPY(QxToastManager)
    QX_DECLARE_PRIVATE(QxToastManager)
};

QX_APP_END_NAMESPACE

#endif   // QXTOASTMANAGER_H
