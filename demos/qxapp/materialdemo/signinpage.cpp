/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#include "signinpage.h"

#include "qxmaterial/materialcard.h"
#include "qxmaterial/materialchip.h"
#include "qxmaterial/materialswitch.h"
#include "qxmaterial/materialtextfield.h"
#include "qxmaterial/materialtokens.h"
#include "qxcontrols/qxcontrol.h"

#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QX_CONTROLS_USE_NAMESPACE
QX_MATERIAL_USE_NAMESPACE

namespace
{
// One card, and the width a form takes: wide enough for an address, narrow
// enough that the eye does not have to travel across the window to read it.
constexpr int kCardWidth = 384;
constexpr int kCardPadding = 16;

QLabel *line(const QString &text, MaterialType::Scale scale, bool strong, QWidget *parent)
{
    auto *label = new QLabel(text, parent);
    label->setFont(MaterialType::font(scale, strong));
    label->setWordWrap(true);
    return label;
}
}   // namespace

SignInPage::SignInPage(QWidget *parent)
    : DemoPage(tr("Sign in"), tr("The screen whose whole job is a decision."), parent)
{
    auto *row = new QWidget(this);
    auto *rowLayout = new QHBoxLayout(row);
    rowLayout->setContentsMargins(QMargins());

    auto *card = new MaterialCard(MaterialCard::Variant::Elevated, row);
    card->setFixedWidth(kCardWidth);
    auto *layout = new QVBoxLayout(card);
    const int inset = MaterialCard::shadowMargin(MaterialCard::Variant::Elevated) + kCardPadding;
    layout->setContentsMargins(inset, inset, inset, inset);
    layout->setSpacing(12);

    layout->addWidget(line(tr("Sign in"), MaterialType::Scale::Headline, true, card));
    layout->addWidget(line(tr("The same account everywhere: nothing here sends it anywhere, because this "
                              "window has no server behind it."),
                           MaterialType::Scale::Body, false, card));

    // Two variants on purpose: the two answers to "where does the field end" are
    // the library's, and a page that showed only one of them would leave the
    // other undecided.
    m_email = new MaterialTextField(card);
    m_email->setVariant(MaterialTextField::Variant::Outlined);
    m_email->setLabel(tr("Email"));
    layout->addWidget(m_email);

    m_password = new MaterialTextField(card);
    m_password->setVariant(MaterialTextField::Variant::Filled);
    m_password->setLabel(tr("Password"));
    layout->addWidget(m_password);

    auto *remember = new MaterialSwitch(card);
    remember->setText(tr("Keep me signed in"));
    layout->addWidget(remember);

    auto *submit = new MaterialChip(MaterialChip::Kind::Assist, card);
    submit->setText(tr("Sign in"));
    connect(submit, &QxControl::clicked, this, &SignInPage::attempt);
    layout->addWidget(submit, 0, Qt::AlignLeft);

    QLabel *aside =
        line(tr("No account yet? One is made the first time you sign in."), MaterialType::Scale::Label, false, card);
    aside->setForegroundRole(QPalette::PlaceholderText);
    layout->addWidget(aside);

    // Enter is the same answer as the chip: a form that only answers the pointer
    // makes the keyboard a second-class way through it.
    connect(m_email, &MaterialTextField::returnPressed, this, &SignInPage::attempt);
    connect(m_password, &MaterialTextField::returnPressed, this, &SignInPage::attempt);

    rowLayout->addStretch(1);
    rowLayout->addWidget(card);
    rowLayout->addStretch(1);
    addBlock(row);

    addStretch();
}

void SignInPage::attempt()
{
    // The screen says what it is missing rather than what went wrong: an empty
    // field is not an error, it is a question that has not been answered.
    const QString email = m_email->text().trimmed();
    if (email.isEmpty() || m_password->text().isEmpty()) {
        Q_EMIT messageRequested(tr("Both fields, and then this card would say who was signed in. Right now it "
                                   "has nothing to go on."));
        return;
    }
    Q_EMIT messageRequested(tr("Signed in as %1, in a window with no server behind it.").arg(email));
}
