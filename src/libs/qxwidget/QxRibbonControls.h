/**
 * Copyleft (C) 2023 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
**/
#pragma once

#include "QxGlobal.h"

#include <QWidget>
#include <QCheckBox>
#include <QComboBox>
#include <QToolButton>
#include <QLineEdit>
#include <QMenu>

QX_BEGIN_NAMESPACE

/* RibbonControl */
class QX_WIDGET_EXPORT RibbonControl : public QWidget
{
    Q_OBJECT
public:
    explicit RibbonControl(QWidget *parent = Q_NULLPTR);
};

/* RibbonCheckBox */
class QX_WIDGET_EXPORT RibbonCheckBox : public QCheckBox
{
    Q_OBJECT
public:
    explicit RibbonCheckBox(QWidget *parent = Q_NULLPTR);
};

/* RibbonComboBox */
class QX_WIDGET_EXPORT RibbonComboBox : public QComboBox
{
    Q_OBJECT
public:
    explicit RibbonComboBox(QWidget *parent = Q_NULLPTR);
};

/* RibbonControlButton */
class QX_WIDGET_EXPORT RibbonControlButton : public QToolButton
{
    Q_OBJECT
public:
    explicit RibbonControlButton(QWidget *parent = Q_NULLPTR);
};

/* RibbonLineEdit */
class QX_WIDGET_EXPORT RibbonLineEdit : public QLineEdit
{
    Q_OBJECT
public:
    explicit RibbonLineEdit(QWidget *parent = Q_NULLPTR);
};

/* RibbonMenu */
class QX_WIDGET_EXPORT RibbonMenu : public QMenu
{
    Q_OBJECT
public:
    explicit RibbonMenu(QWidget *parent = Q_NULLPTR);
    explicit RibbonMenu(const QString &title, QWidget *parent = Q_NULLPTR);

    QAction *addRibbonMenu(RibbonMenu *menu);
    RibbonMenu *addRibbonMenu(const QString &title);
    RibbonMenu *addRibbonMenu(const QIcon &icon, const QString &title);
    QAction *addWidget(QWidget *w);
};

/* RibbonSeparator */
class QX_WIDGET_EXPORT RibbonSeparator : public QWidget
{
    Q_OBJECT
public:
    RibbonSeparator(int height, QWidget *parent = Q_NULLPTR);
    RibbonSeparator(QWidget *parent = Q_NULLPTR);

    virtual QSize sizeHint() const Q_DECL_OVERRIDE;
    void setTopBottomMargins(int top, int bottom);
protected:
    void paintEvent(QPaintEvent *event) Q_DECL_OVERRIDE;
private:
    int m_topMargin;
    int m_bottomMargin;
};

QX_END_NAMESPACE
