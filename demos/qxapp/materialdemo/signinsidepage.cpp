/**
 * Copyright (C) 2026 maminjie <canpool@163.com>
 * SPDX-License-Identifier: MulanPSL-2.0
 **/
#include "signinsidepage.h"

#include "qxmaterial/materialautocomplete.h"
#include "qxmaterial/materialbutton.h"
#include "qxmaterial/materialcheckbox.h"
#include "qxmaterial/materialtextfield.h"
#include "qxmaterial/materialtokens.h"
#include "qxcontrols/qxcontrol.h"

#include <QtCore/QCoreApplication>
#include <QtCore/QStringList>
#include <QtGui/QPainter>
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
// The band's width and the form's: the band is a picture and a sentence, so it
// asks for less than half of a wide page and stops asking below that, and the
// form keeps the width a form takes whether or not there is room to spare.
constexpr int kBandWidth = 320;
constexpr int kFormWidth = 320;
constexpr int kInset = 32;
constexpr int kGap = 12;
constexpr int kSplitHeight = 360;

QLabel *line(const QString &text, MaterialType::Scale scale, bool strong, QWidget *parent)
{
    auto *label = new QLabel(text, parent);
    label->setFont(MaterialType::font(scale, strong));
    label->setWordWrap(true);
    return label;
}

/*! A label that reads on the band rather than on the page.
 *
 *  The band is the accent, so what stands on it is the colour the palette pairs
 *  with the accent rather than the page's ink - asked for through the palette
 *  and not written down, because which colour that is is the theme's answer and
 *  this page has four themes to live with. */
QLabel *onBand(const QString &text, MaterialType::Scale scale, bool strong, QWidget *parent)
{
    QLabel *label = line(text, scale, strong, parent);
    // Transparent as well as coloured: the themed sheet fills every QWidget with
    // the surface colour, and a label that let it through would put a dark card
    // behind every line of a page that has no cards.
    label->setStyleSheet(QStringLiteral("color: palette(highlighted-text); background: transparent;"));
    return label;
}

/*! The band itself: the accent, painted where it stands.
 *
 *  A themed sheet fills every QWidget with the surface colour - a dark scheme
 *  has to repaint what the platform style would otherwise leave light - and that
 *  fill is the sheet's, so it wins over a widget's own window colour. A band
 *  that asked for the accent through QPalette::Window therefore came out the
 *  colour of the page in the dark and looked like nothing at all. It paints its
 *  own fill instead, where no sheet can reach. */
class Band : public QWidget
{
public:
    explicit Band(QWidget *parent)
        : QWidget(parent)
    {
    }
protected:
    void paintEvent(QPaintEvent *) override
    {
        QPainter painter(this);
        painter.fillRect(rect(), palette().color(QPalette::Highlight));
    }
};
}   // namespace

SignInSidePage::SignInSidePage(QWidget *parent)
    : DemoPage(tr("Entrance"),
               tr("The same decision as the screen before this one - signing in - arranged with a band "
                  "down one side."),
               parent)
{
    addNote(tr("A band of the accent down one side and the form beside it. The form stops having to say what "
               "it is for, so it is three rows and a command - and the band is where a product says what the "
               "account is, in the only place on the screen that is not asking anything."));

    auto *split = new QWidget(this);
    split->setMinimumHeight(kSplitHeight);
    auto *splitLayout = new QHBoxLayout(split);
    splitLayout->setContentsMargins(QMargins());
    splitLayout->setSpacing(0);

    auto *band = new Band(split);
    band->setFixedWidth(kBandWidth);

    auto *bandLayout = new QVBoxLayout(band);
    bandLayout->setContentsMargins(kInset, kInset, kInset, kInset);
    bandLayout->setSpacing(kGap);
    bandLayout->addWidget(onBand(tr("Canpool"), MaterialType::Scale::Title, true, band));
    bandLayout->addStretch(1);
    bandLayout->addWidget(onBand(tr("Twelve albums, one account, and nothing leaves this window."),
                                 MaterialType::Scale::Title, true, band));
    bandLayout->addWidget(onBand(tr("The record keeper"), MaterialType::Scale::Label, false, band));
    bandLayout->addStretch(2);
    splitLayout->addWidget(band);

    // The form keeps its own width in the middle of the room that is left: a
    // field stretched across half a window is a field whose end is nowhere near
    // its label.
    auto *side = new QWidget(split);
    auto *sideLayout = new QHBoxLayout(side);
    sideLayout->setContentsMargins(QMargins());
    auto *form = new QWidget(side);
    form->setFixedWidth(kFormWidth);
    auto *layout = new QVBoxLayout(form);
    layout->setContentsMargins(kInset, kInset, kInset, kInset);
    layout->setSpacing(kGap);

    layout->addWidget(line(tr("Sign in"), MaterialType::Scale::Headline, true, form));
    layout->addWidget(line(tr("An account this window already knows, or one written out in full."),
                           MaterialType::Scale::Body, false, form));

    m_account = new MaterialAutocomplete(form);
    m_account->setVariant(MaterialTextField::Variant::Outlined);
    m_account->setLabel(tr("Account"));
    m_account->setOptions(QStringList{QStringLiteral("mara@canpool.example"), QStringLiteral("jonas@canpool.example"),
                                      QStringLiteral("inês@canpool.example"),
                                      QStringLiteral("the.mill@canpool.example")});
    layout->addWidget(m_account);

    m_password = new MaterialTextField(form);
    m_password->setVariant(MaterialTextField::Variant::Outlined);
    m_password->setLabel(tr("Password"));
    // The field has no echo mode of its own yet, and a password row that shows
    // what was typed is not a password row: the editor behind the field is
    // reachable, so this is asked of it and not worked around on the page.
    m_password->lineEdit()->setEchoMode(QLineEdit::Password);
    layout->addWidget(m_password);

    auto *remember = new MaterialCheckBox(form);
    remember->setText(tr("Keep me signed in"));
    remember->setChecked(true);
    layout->addWidget(remember);

    auto *submit = new MaterialButton(form);
    submit->setVariant(MaterialButton::Variant::Filled);
    submit->setText(tr("Sign in"));
    connect(submit, &QxControl::clicked, this, &SignInSidePage::attempt);
    layout->addWidget(submit, 0, Qt::AlignLeft);

    QLabel *aside = line(tr("No account yet? The next screen makes one."), MaterialType::Scale::Label, false, form);
    aside->setForegroundRole(QPalette::PlaceholderText);
    layout->addWidget(aside);

    connect(m_account, &MaterialTextField::returnPressed, this, &SignInSidePage::attempt);
    connect(m_password, &MaterialTextField::returnPressed, this, &SignInSidePage::attempt);

    sideLayout->addStretch(1);
    sideLayout->addWidget(form);
    sideLayout->addStretch(1);
    splitLayout->addWidget(side, 1);

    addBlock(split);
    addStretch();
}

void SignInSidePage::attempt()
{
    // What was written and what was taken are two questions, and this is the
    // screen where the difference shows: an account typed out in full is its own
    // answer even though no row was ever taken.
    const QString written = m_account->text().trimmed();
    if (written.isEmpty() || m_password->text().isEmpty()) {
        Q_EMIT messageRequested(tr("Both rows, and then this window would say who was signed in. It has nothing "
                                   "to go on yet."));
        return;
    }
    const QString chosen = m_account->currentText();
    Q_EMIT messageRequested(
        tr("Signed in as %1, on a screen with no server behind it.").arg(chosen.isEmpty() ? written : chosen));
}
