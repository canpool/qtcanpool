/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#include "demopage.h"

#include "qxmaterial/materialtokens.h"

#include <QtCore/QMargins>
#include <QFormLayout>
#include <QLabel>
#include <QVBoxLayout>

QX_MATERIAL_USE_NAMESPACE

namespace
{
// The room a screen keeps between the window and what it says, and the room
// between the things it says: sixteen is the page margin Material uses, and
// twelve is what a heading keeps from what it is the heading of.
constexpr int kPageMargin = 16;
constexpr int kSectionGap = 12;

QLabel *bodyLabel(const QString &text, QWidget *parent)
{
    auto *label = new QLabel(text, parent);
    label->setWordWrap(true);
    label->setFont(MaterialType::font(MaterialType::Scale::Body));
    return label;
}
}   // namespace

DemoPage::DemoPage(const QString &title, const QString &intro, QWidget *parent)
    : QWidget(parent)
    , m_title(title)
{
    m_layout = new QVBoxLayout(this);
    m_layout->setContentsMargins(QMargins(kPageMargin, kPageMargin, kPageMargin, kPageMargin));
    m_layout->setSpacing(kSectionGap);

    auto *heading = new QLabel(title, this);
    heading->setFont(MaterialType::font(MaterialType::Scale::Headline, true));
    m_layout->addWidget(heading);

    if (!intro.isEmpty()) {
        auto *lead = bodyLabel(intro, this);
        // The small print on a control is the step of the scale that exists for
        // exactly this: a label, not a body, and not the heading's own size
        // made smaller.
        lead->setFont(MaterialType::font(MaterialType::Scale::Label));
        m_layout->addWidget(lead);
        m_layout->addSpacing(kSectionGap);
    }
}

QString DemoPage::title() const
{
    return m_title;
}

void DemoPage::addSection(const QString &text)
{
    auto *heading = new QLabel(text, this);
    heading->setFont(MaterialType::font(MaterialType::Scale::Title, true));
    m_layout->addWidget(heading);
    // A section starts its own set of rows: a row added after a block belongs
    // to the block's section and not to the one that is opening.
    m_form = Q_NULLPTR;
}

void DemoPage::addNote(const QString &text)
{
    m_layout->addWidget(bodyLabel(text, this));
}

void DemoPage::addRow(const QString &caption, QWidget *control)
{
    if (!m_form) {
        m_form = new QFormLayout;
        m_form->setContentsMargins(QMargins());
        m_form->setSpacing(kSectionGap);
        m_form->setFieldGrowthPolicy(QFormLayout::ExpandingFieldsGrow);
        m_layout->addLayout(m_form);
    }
    auto *label = bodyLabel(caption, this);
    m_form->addRow(label, control);
}

void DemoPage::addBlock(QWidget *widget)
{
    m_layout->addWidget(widget);
    m_form = Q_NULLPTR;
}

void DemoPage::addStretch()
{
    m_layout->addStretch(1);
    m_form = Q_NULLPTR;
}
