/**
 * Copyleft (C) 2023 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MIT
**/
#pragma once

#include "qxribbon_global.h"

#include <QWidget>
#include <QCheckBox>
#include <QComboBox>
#include <QToolButton>
#include <QLineEdit>

QX_RIBBON_BEGIN_NAMESPACE

/* RibbonControl */
class QX_RIBBON_EXPORT RibbonControl : public QWidget
{
    Q_OBJECT
public:
    explicit RibbonControl(QWidget *parent = Q_NULLPTR);
};

/* RibbonCheckBox */
class QX_RIBBON_EXPORT RibbonCheckBox : public QCheckBox
{
    Q_OBJECT
public:
    explicit RibbonCheckBox(QWidget *parent = Q_NULLPTR);
};

/* RibbonComboBox */
class QX_RIBBON_EXPORT RibbonComboBox : public QComboBox
{
    Q_OBJECT
public:
    explicit RibbonComboBox(QWidget *parent = Q_NULLPTR);
};

/* RibbonControlButton */
class QX_RIBBON_EXPORT RibbonControlButton : public QToolButton
{
    Q_OBJECT
public:
    explicit RibbonControlButton(QWidget *parent = Q_NULLPTR);
};

/* RibbonLineEdit */
class QX_RIBBON_EXPORT RibbonLineEdit : public QLineEdit
{
    Q_OBJECT
public:
    explicit RibbonLineEdit(QWidget *parent = Q_NULLPTR);
};

QX_RIBBON_END_NAMESPACE
