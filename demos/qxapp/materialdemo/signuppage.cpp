/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#include "signuppage.h"

#include "qxmaterial/materialbutton.h"
#include "qxmaterial/materialcard.h"
#include "qxmaterial/materialcheckbox.h"
#include "qxmaterial/materialtextfield.h"
#include "qxmaterial/materialtokens.h"
#include "qxmaterial/materialtogglebuttongroup.h"
#include "qxcontrols/qxcontrol.h"

#include <QtCore/QCoreApplication>
#include <QtCore/QStringList>
#include <QtGui/QPalette>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QX_CONTROLS_USE_NAMESPACE
QX_MATERIAL_USE_NAMESPACE

namespace
{
// The width of the form, and the room between its rows: a card wide enough for
// two names side by side and no wider, because a form that fills a window reads
// as a window with fields in it rather than as a form.
constexpr int kFormWidth = 480;
constexpr int kCardPadding = 16;
constexpr int kGap = 12;

QLabel *line(const QString &text, MaterialType::Scale scale, bool strong, QWidget *parent)
{
    auto *label = new QLabel(text, parent);
    label->setFont(MaterialType::font(scale, strong));
    label->setWordWrap(true);
    return label;
}

/*! One row of the form, outlined, under a label of its own. */
MaterialTextField *field(const QString &label, MaterialTextField::Variant variant, QWidget *parent)
{
    auto *field = new MaterialTextField(parent);
    field->setVariant(variant);
    field->setLabel(label);
    return field;
}
}   // namespace

SignUpPage::SignUpPage(QWidget *parent)
    : DemoPage(tr("Sign up"),
               tr("The form that makes the account the screen before this one signs into: four rows, one of them "
                  "about the account rather than about the person."),
               parent)
{
    addNote(tr("A card and nothing else. Signing in could put its form beside a band of the accent, because it "
               "was asking about something that already exists; making an account has to ask four questions in "
               "order first, and a card is what holds a list of questions without arranging them into a story."));

    auto *card = new MaterialCard(MaterialCard::Variant::Elevated, this);
    const int inset = MaterialCard::shadowMargin(MaterialCard::Variant::Elevated) + kCardPadding;
    card->setFixedWidth(kFormWidth + 2 * inset);
    auto *layout = new QVBoxLayout(card);
    layout->setContentsMargins(inset, inset, inset, inset);
    layout->setSpacing(kGap);

    layout->addWidget(line(tr("Create an account"), MaterialType::Scale::Headline, true, card));
    layout->addWidget(line(tr("One account, whichever kind: the window keeps it, and nothing here sends it "
                              "anywhere."),
                           MaterialType::Scale::Body, false, card));

    m_kind = new MaterialToggleButtonGroup(card);
    m_kind->addButton(tr("Personal"));
    m_kind->addButton(tr("Team"));
    m_kind->setChecked(0, true);
    layout->addWidget(m_kind, 0, Qt::AlignLeft);

    // The two names share a row because they are one answer: which name to
    // write on the account. A form that stacked them would read as two.
    auto *names = new QWidget(card);
    auto *namesLayout = new QHBoxLayout(names);
    namesLayout->setContentsMargins(QMargins());
    namesLayout->setSpacing(kGap);
    m_name = field(tr("First name"), MaterialTextField::Variant::Outlined, names);
    m_surname = field(tr("Last name"), MaterialTextField::Variant::Outlined, names);
    namesLayout->addWidget(m_name, 1);
    namesLayout->addWidget(m_surname, 1);
    layout->addWidget(names);

    m_email = field(tr("Email"), MaterialTextField::Variant::Outlined, card);
    layout->addWidget(m_email);

    m_password = field(tr("Password"), MaterialTextField::Variant::Filled, card);
    // A password row shows what was typed unless the editor behind the field is
    // told not to; the field has no echo mode of its own yet.
    m_password->lineEdit()->setEchoMode(QLineEdit::Password);
    layout->addWidget(m_password);

    m_terms = new MaterialCheckBox(card);
    m_terms->setText(tr("I have read what the terms say"));
    layout->addWidget(m_terms);

    auto *submit = new MaterialButton(card);
    submit->setVariant(MaterialButton::Variant::Filled);
    submit->setText(tr("Create the account"));
    connect(submit, &QxControl::clicked, this, &SignUpPage::create);
    layout->addWidget(submit, 0, Qt::AlignLeft);

    QLabel *aside = line(tr("One already exists? The screen before this one signs into it."),
                         MaterialType::Scale::Label, false, card);
    aside->setForegroundRole(QPalette::PlaceholderText);
    layout->addWidget(aside);

    for (MaterialTextField *widget : {m_name, m_surname, m_email, m_password}) {
        connect(widget, &MaterialTextField::returnPressed, this, &SignUpPage::create);
    }

    auto *host = new QWidget(this);
    auto *hostLayout = new QHBoxLayout(host);
    hostLayout->setContentsMargins(QMargins());
    hostLayout->addWidget(card);
    hostLayout->addStretch(1);
    addBlock(host);

    addStretch();
}

void SignUpPage::create()
{
    // Every question at once, in the order the form asks them: a form that
    // reported one missing row per attempt would make the reader walk back to it
    // as many times as it has rows.
    QStringList missing;
    if (m_name->text().trimmed().isEmpty()) {
        missing << tr("the first name");
    }
    if (m_surname->text().trimmed().isEmpty()) {
        missing << tr("the last name");
    }
    const QString email = m_email->text().trimmed();
    if (email.isEmpty()) {
        missing << tr("an email");
    }
    if (m_password->text().isEmpty()) {
        missing << tr("a password");
    }
    if (!missing.isEmpty()) {
        Q_EMIT messageRequested(
            tr("An account needs all four, and this one is still missing %1.").arg(missing.join(tr(", "))));
        return;
    }
    if (!m_terms->isChecked()) {
        Q_EMIT messageRequested(tr("The box under the password is not decoration: it is the only thing on this "
                                   "screen the reader has to agree to before the account can be made."));
        return;
    }
    const QString kind = m_kind->isChecked(1) ? tr("a team account") : tr("a personal account");
    Q_EMIT messageRequested(tr("%1 would now have %2, in a window that keeps accounts nowhere.").arg(email, kind));
}
