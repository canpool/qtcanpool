/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#ifndef ABOUTDIALOG_H
#define ABOUTDIALOG_H

#include <QDialog>

class AboutDialog : public QDialog
{
    Q_OBJECT
public:
    explicit AboutDialog(const QString &softlogo, const QString &softname, const QString &softver,
                         const QString &softdesc, QWidget *parent = nullptr);
    virtual ~AboutDialog();
};

#endif   // ABOUTDIALOG_H
