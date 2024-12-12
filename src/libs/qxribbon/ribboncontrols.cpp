/**
 * Copyleft (C) 2023 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MIT
**/
#include "ribboncontrols.h"

QX_RIBBON_BEGIN_NAMESPACE

/* RibbonControl */
RibbonControl::RibbonControl(QWidget *parent) : QWidget(parent)
{

}

/* RibbonCheckBox */
RibbonCheckBox::RibbonCheckBox(QWidget *parent) : QCheckBox(parent)
{
}

/* RibbonComboBox */
RibbonComboBox::RibbonComboBox(QWidget *parent) : QComboBox(parent)
{
}

/* RibbonControlButton */
RibbonControlButton::RibbonControlButton(QWidget *parent) : QToolButton(parent)
{
}

/* RibbonLineEdit */
RibbonLineEdit::RibbonLineEdit(QWidget *parent) : QLineEdit(parent)
{
}

QX_RIBBON_END_NAMESPACE
