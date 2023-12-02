/**
 * Copyright (C) 2023 maminjie <canpool@163.com> (https://github.com/canpool)
 * Copyright (C) 2022-2023 Antonio Dias (https://github.com/antonypro)
 * SPDX-License-Identifier: MIT
 **/

#ifndef QGOODDIALOG_H
#define QGOODDIALOG_H

#include <QObject>
#include <QWindowList>
#include <QPointer>
#include <QEventLoop>

class QDialog;

class QGoodWindow;

class QGoodDialog : public QObject
{
    Q_OBJECT
private:
    explicit QGoodDialog(QDialog *dialog, QGoodWindow *childGw, QGoodWindow *parentGw);
public:
    static int exec(QDialog *dialog, QGoodWindow *childGw, QGoodWindow *parentGw);
private:
    int exec();
    bool eventFilter(QObject *watched, QEvent *event);
private:
    QEventLoop m_loop;
    QPointer<QDialog> m_dialog;
    QPointer<QGoodWindow> m_childGw;
    QPointer<QGoodWindow> m_parentGw;
#if defined Q_OS_WIN || defined Q_OS_MAC
    QWindowList m_windowList;
#endif
};

#endif   // QGOODDIALOG_H
