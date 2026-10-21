/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#include "gallerypage.h"

#include <QtWidgets/QLabel>

GalleryPage::GalleryPage(const QString &styleId, QWidget *parent)
    : QWidget(parent)
    , m_styleId(styleId)
{
    m_form = new QFormLayout(this);
    // A control keeps the size it asked for. The default policy is already
    // FieldsStayAtSizeHint and is restated because it is the whole reason a row
    // can be cropped: a slider stretched to the width of the page is not the
    // slider's sizeHint any more, and a capture of it says nothing about the
    // one an application gets by default.
    m_form->setFieldGrowthPolicy(QFormLayout::FieldsStayAtSizeHint);
    m_form->setLabelAlignment(Qt::AlignRight | Qt::AlignVCenter);
    m_form->setFormAlignment(Qt::AlignLeft | Qt::AlignTop);
    m_form->setHorizontalSpacing(24);
    m_form->setVerticalSpacing(8);
    m_form->setContentsMargins(16, 12, 16, 12);
}

QString GalleryPage::styleId() const
{
    return m_styleId;
}

void GalleryPage::addSection(const QString &text)
{
    QLabel *heading = new QLabel(text, this);
    // A section is a row of its own, spanning both columns: the caption column
    // is far too narrow for a heading, and a heading squeezed into it would be
    // a second caption rather than a division.
    m_form->addRow(heading);
    QFont font = heading->font();
    font.setBold(true);
    heading->setFont(font);
}

QList<QWidget *> GalleryPage::controls() const
{
    return m_controls;
}
