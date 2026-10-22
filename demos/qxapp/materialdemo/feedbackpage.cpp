/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#include "feedbackpage.h"

#include "qxmaterial/materialbadge.h"
#include "qxmaterial/materialprogressring.h"
#include "qxmaterial/materialskeleton.h"
#include "qxmaterial/materialslider.h"
#include "qxmaterial/materialtokens.h"

#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QWidget>

QX_MATERIAL_USE_NAMESPACE

namespace
{
// A row of two things that belong beside each other: the count, and the two
// buttons that change it. A badge on its own is a number with nothing to be a
// count of.
QWidget *badgeRow(MaterialBadge *badge, QPushButton *add, QPushButton *clear)
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

    m_known = new MaterialProgressRing(this);
    // A ring is born indeterminate: a value it has been handed while it still
    // sweeps is stored but not shown, so being determinate is said out loud.
    m_known->setIndeterminate(false);
    m_known->setValue(65);
    addRow(tr("How far"), m_known);

    m_drive = new MaterialSlider(this);
    m_drive->setRange(0, 100);
    m_drive->setValue(65);
    connect(m_drive, &MaterialSlider::valueChanged, this, [this](int value) {
        m_known->setValue(value);
    });
    addRow(tr("Set it"), m_drive);

    m_unknown = new MaterialProgressRing(this);
    m_unknown->setIndeterminate(true);
    addRow(tr("How far is not known"), m_unknown);

    addSection(tr("What has not arrived yet"));

    addNote(tr("A screen that is waiting can show nothing and then jump, or it can show the shape of what "
               "is coming - a line where a line will be, a disc where a face will be - so that when it "
               "arrives it lands where the user was already looking. These breathe, unless the user asked "
               "for less motion."));
    auto *shapes = new QWidget(this);
    auto *shapeLayout = new QHBoxLayout(shapes);
    shapeLayout->setContentsMargins(QMargins());
    shapeLayout->setSpacing(8);
    auto *line = new MaterialSkeleton(MaterialSkeleton::Shape::Rectangle, shapes);
    line->setFixedSize(220, 16);
    auto *face = new MaterialSkeleton(MaterialSkeleton::Shape::Circle, shapes);
    face->setFixedSize(48, 48);
    shapeLayout->addWidget(line);
    shapeLayout->addWidget(face);
    shapeLayout->addStretch(1);
    addBlock(shapes);

    addSection(tr("A count the user has not looked at"));

    m_badge = new MaterialBadge(this);
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

    addNote(tr("A snackbar is a sentence at the bottom of what is going on. It is not an info bar: it goes "
               "away on its own, because what it says is not something the user has to answer."));
    m_say = new QPushButton(tr("Say something"));
    connect(m_say, &QPushButton::clicked, this, [this]() {
        Q_EMIT messageRequested(tr("The snackbar went up from under the bottom edge, and will go down on "
                                   "its own."));
    });
    addRow(tr("Say it"), m_say);

    addStretch();
}
