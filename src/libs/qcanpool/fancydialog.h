/**
 * Copyright (C) 2018-2022 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
**/
#ifndef FANCYDIALOG_H
#define FANCYDIALOG_H

#include "qcanpool.h"
#include <QDialog>

QCANPOOL_BEGIN_NAMESPACE

class FancyDialogPrivate;

/* FancyDialog */
class QCANPOOL_SHARED_EXPORT FancyDialog : public QDialog
{
    Q_OBJECT
public:
    explicit FancyDialog(QWidget *parent = nullptr, Qt::WindowFlags f = Qt::WindowFlags());
    virtual ~FancyDialog();

    void setCentralWidget(QWidget *w);
    void setWidgetResizable(bool resizable);
    void setTitleBarHeight(int height);

    QAction *addAction(const QIcon &icon, const QString &text);
    void addAction(QAction *action);

    QLayout *layout() const;
    void setLayout(QLayout *layout);

    void setFixedSize(const QSize &s);
    void setFixedSize(int w, int h);
    void setFixedWidth(int w);
    void setFixedHeight(int h);

    void setWindowFlags(Qt::WindowFlags type);

private:
    FancyDialogPrivate *d;
};

QCANPOOL_END_NAMESPACE

#endif // FANCYDIALOG_H
