/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#include "feedbackpage.h"

#include "qxfluent/fluentbadge.h"
#include "qxfluent/fluentinfobar.h"
#include "qxfluent/fluentprogressring.h"
#include "qxfluent/fluentslider.h"
#include "qxfluent/fluenttokens.h"

#include <QComboBox>
#include <QLabel>
#include <QPushButton>
#include <QWidget>

namespace
{
// A row of two things that belong beside each other: the count, and the two
// buttons that change it. A badge on its own is a number with nothing to be a
// count of.
QWidget *badgeRow(FluentBadge *badge, QPushButton *add, QPushButton *clear)
{
    auto *row = new QWidget;
    auto *layout = new QHBoxLayout(row);
    layout->setContentsMargins(QMargins());
    layout->setSpacing(8);
    layout->addWidget(badge);
    layout->addWidget(add);
    layout->addWidget(clear);
    layout->addStretch(1);
    return row;
}
}   // namespace

FeedbackPage::FeedbackPage(QWidget *parent)
    : DemoPage(tr("Feedback"),
               tr("What the application says while it works, and what it says when it has "
                  "finished - or when it has not."),
               parent)
{
    addSection(tr("Waiting"));

    addNote(tr("A ring that knows how far it has got draws the arc; one that does not sweeps instead, "
               "because a progress indicator that stops moving is one the user takes for a hang."));

    m_known = new FluentProgressRing(this);
    m_known->setValue(65);
    addRow(tr("How far"), m_known);

    m_drive = new FluentSlider(this);
    m_drive->setRange(0, 100);
    m_drive->setValue(65);
    connect(m_drive, &FluentSlider::valueChanged, this, [this](int value) {
        m_known->setValue(value);
    });
    addRow(tr("Set it"), m_drive);

    m_unknown = new FluentProgressRing(this);
    m_unknown->setIndeterminate(true);
    addRow(tr("How far is not known"), m_unknown);

    addSection(tr("A count the user has not looked at"));

    m_badge = new FluentBadge(this);
    m_badge->setCount(12);
    m_badge->setMaximumCount(99);
    m_add = new QPushButton(tr("Add one"));
    m_clear = new QPushButton(tr("Clear"));
    connect(m_add, &QPushButton::clicked, this, [this]() {
        // Past its maximum a badge stops counting and says so: "127" is not
        // more useful than "99+", and it is wider.
        m_badge->setCount(qMin(m_badge->count() + 1, m_badge->maximumCount() + 20));
    });
    connect(m_clear, &QPushButton::clicked, this, [this]() {
        m_badge->setCount(0);
    });
    addBlock(badgeRow(m_badge, m_add, m_clear));

    addSection(tr("Something happened"));

    m_info = new FluentInfoBar(this);
    m_info->setSeverity(FluentInfoBar::Severity::Success);
    m_info->setTitle(tr("Saved"));
    m_info->setMessage(tr("The four severities are four colours, and the colour is the answer to how much "
                          "the user has to care."));
    m_info->setOpen(true);
    addBlock(m_info);

    auto *severity = new QComboBox(this);
    severity->addItem(tr("Informational"), int(FluentInfoBar::Severity::Informational));
    severity->addItem(tr("Success"), int(FluentInfoBar::Severity::Success));
    severity->addItem(tr("Warning"), int(FluentInfoBar::Severity::Warning));
    severity->addItem(tr("Error"), int(FluentInfoBar::Severity::Error));
    severity->setCurrentIndex(1);
    connect(severity, QOverload<int>::of(&QComboBox::activated), this, [this, severity](int index) {
        m_info->setSeverity(static_cast<FluentInfoBar::Severity>(severity->itemData(index).toInt()));
    });
    addRow(tr("Severity"), severity);

    addStretch();
}
