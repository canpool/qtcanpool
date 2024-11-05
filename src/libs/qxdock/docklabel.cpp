/**
 * Copyleft (C) 2024 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/

#include "docklabel.h"
#include "dockutils.h"

QX_DOCK_BEGIN_NAMESPACE

class DockLabelPrivate
{
public:
    QX_DECLARE_PUBLIC(DockLabel)
public:
    DockLabelPrivate();
public:
    QString m_text;
};

DockLabelPrivate::DockLabelPrivate()
{

}

QxDock::DockLabel::DockLabel(QWidget *parent, Qt::WindowFlags f)
    : DockLabel("", parent, f)
{
}

QxDock::DockLabel::DockLabel(const QString &text, QWidget *parent, Qt::WindowFlags f)
    : QLabel(text, parent, f)
{
    QX_INIT_PRIVATE(DockLabel);

    Q_D(DockLabel);
    if (!text.isEmpty()) {
        d->m_text = text;
        internal::setToolTip(this, text);
    }
}


DockLabel::~DockLabel()
{
    QX_FINI_PRIVATE()
}


QX_DOCK_END_NAMESPACE
