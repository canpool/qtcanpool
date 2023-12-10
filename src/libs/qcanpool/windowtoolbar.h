/**
 * Copyright (C) 2023 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#pragma once

#include "qcanpool.h"
#include <QWidget>

QCANPOOL_BEGIN_NAMESPACE

class WindowToolBarPrivate;

class QCANPOOL_SHARED_EXPORT WindowToolBar : public QWidget
{
    Q_OBJECT
public:
    explicit WindowToolBar(QWidget *parent, Qt::WindowFlags flags = Qt::WindowFlags());
    ~WindowToolBar();

    QAction *addAction(const QIcon &icon, const QString &text);
    void addAction(QAction *action);
    void insertAction(QAction *before, QAction *action);
    QAction *addSeparator();
    QAction *insertSeparator(QAction *before);
    QAction *addWidget(QWidget *widget);
    QAction *insertWidget(QAction *before, QWidget *widget);

    void updateWindowFlags(Qt::WindowFlags flags);
    void setWindowStates(Qt::WindowStates states);

    /** default is disabled */
    bool signalIsEnabled() const;
    QSize iconSize() const;
public Q_SLOTS:
    void setIconSize(const QSize &size);
    void setSignalEnabled(bool enable);
Q_SIGNALS:
    void buttonMinimizeClicked();
    void buttonMaximzieClicked();
    void buttonCloseClicked();
protected:
    virtual bool eventFilter(QObject *object, QEvent *event);
private:
    QCP_DECLARE_PRIVATE(WindowToolBar)
};

QCANPOOL_END_NAMESPACE
