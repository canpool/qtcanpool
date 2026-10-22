/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#include "demopage.h"

#include "qxfluent/fluenttokens.h"

#include <QtCore/QMargins>
#include <QLabel>
#include <QFormLayout>
#include <QVBoxLayout>

QX_FLUENT_USE_NAMESPACE

namespace
{
// The room a screen keeps between the window and what it says, and the room
// between the things it says: twenty-four is the page margin Fluent uses, and
// twelve is what a heading keeps from what it is the heading of.
constexpr int kPageMargin = 24;
constexpr int kSectionGap = 12;

QLabel *bodyLabel(const QString &text, QWidget *parent)
{
    auto *label = new QLabel(text, parent);
    label->setWordWrap(true);
    label->setFont(FluentType::font(FluentType::Ramp::Body));
    return label;
}
}   // namespace

DemoPage::DemoPage(const QString &title, const QString &intro, QWidget *parent)
    : QWidget(parent)
{
    m_layout = new QVBoxLayout(this);
    m_layout->setContentsMargins(QMargins(kPageMargin, kPageMargin, kPageMargin, kPageMargin));
    m_layout->setSpacing(kSectionGap);

    auto *heading = new QLabel(title, this);
    heading->setFont(FluentType::font(FluentType::Ramp::Title, true));
    m_layout->addWidget(heading);

    if (!intro.isEmpty()) {
        auto *lead = bodyLabel(intro, this);
        // The small print under a heading, which is the step of the ramp that
        // exists for exactly this: caption, not body, and not the heading's own
        // size made smaller.
        lead->setFont(FluentType::font(FluentType::Ramp::Caption));
        m_layout->addWidget(lead);
        m_layout->addSpacing(kSectionGap);
    }
}

void DemoPage::addSection(const QString &text)
{
    auto *heading = new QLabel(text, this);
    heading->setFont(FluentType::font(FluentType::Ramp::Subtitle, true));
    m_layout->addWidget(heading);
    // A section starts its own set of rows: a row added after a block belongs to
    // the block's section and not to the one that is opening.
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
        // Indented a little under the section it belongs to, so what reads as a
        // list of settings is not flush with what reads as a heading.
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
